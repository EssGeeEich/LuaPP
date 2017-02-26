/*	Copyright (c) 2015 SGH
**	
**	Permission is granted to use, modify and redistribute this software.
**	Modified versions of this software MUST be marked as such.
**	
**	This software is provided "AS IS". In no event shall
**	the authors or copyright holders be liable for any claim,
**	damages or other liability. The above copyright notice
**	and this permission notice shall be included in all copies
**	or substantial portions of the software.
**	
**	File created on: 17/11/2015
*/

#ifndef __LUAPP_TYPEEXT_H__
#define __LUAPP_TYPEEXT_H__
#include "types.h"

namespace Lua {

    namespace impl {
        template <typename T> struct IntegerConverter {
            typedef typename std::enable_if<std::is_integral<T>::value, Lua::Arg<T> >::type Arg;
            static Arg Read(lua_State* s, int id) {
                if(!lua_isinteger(s,id))
                    return Arg::Nil();
                int rv = 0;
                lua_Integer i = lua_tointegerx(s,id,&rv);
                if(!rv)
                    return Arg::Nil();
                return Arg::ToCopy(i);
            }
            static void Push(lua_State* s, T const& v) {
                lua_pushinteger(s,v);
            }
            enum { PushCount = 1 };
        };
        template <typename T> struct NumberConverter {
            typedef typename std::enable_if<std::is_floating_point<T>::value, Lua::Arg<T> >::type Arg;
            static Arg Read(lua_State* s, int id) {
                if(!lua_isnumber(s,id))
                    return Arg::Nil();
                int rv = 0;
                lua_Number i = lua_tonumberx(s,id,&rv);
                if(!rv)
                    return Arg::Nil();
                return Arg::ToCopy(i);
            }
            static void Push(lua_State* s, T const& v) {
                lua_pushnumber(s,v);
            }
            enum { PushCount = 1 };
        };
        template <int... Is> struct sequence {};
        template <int n, int ... Is> struct make_sequence : make_sequence<n-1,n-1,Is...> {};
        template <int ... Is> struct make_sequence<0, Is...> : sequence<Is...> {};

        template <typename T, typename F, int ... Is>
        void for_each_impl(T&& t, F f, sequence<Is...>)
        {
            auto l = { (f(std::get<Is>(t)),0)... };
            (void)l;
        }

        template <typename ... Ts, typename F>
        void for_each_in_tuple(std::tuple<Ts...> const& t, F f)
        {
            for_each_impl(t, f, make_sequence<sizeof...(Ts)>());
        }
    }

    // For classes with a metatype
    template <typename T> struct TypeConverter {
        typedef Lua::MetatableDescriptorImpl<T> metatable;
        typedef Lua::Arg<T> Arg;
        static Arg Read(lua_State* s, int id) {
            T* p = (T*)luaL_checkudata(s,id,metatable::name());
            if(!p)
                return Arg::Nil();

            return Arg::ToReference(*p);
        }
        // You need references to push something of this type.
        // static void Write(lua_State* state, T const& data) {}
    };

    // std::string
    template <> struct TypeConverter<std::string> {
        typedef Lua::Arg<std::string> Arg;
        static Arg Read(lua_State* s, int id) {
            if(!lua_isstring(s,id))
                return Arg::Nil();
            size_t size = 0;
            char const* str = lua_tolstring(s,id,&size);
            if(!str)
                return Arg::Nil();
            return Arg::ToMove(std::string(str,size));
        }
        static void Push(lua_State* s, std::string const& v) {
            lua_pushlstring(s,v.c_str(),v.size());
        }
        enum { PushCount = 1 };
    };

    template <> struct TypeConverter<bool> {
        typedef Lua::Arg<bool> Arg;
        static Arg Read(lua_State* s, int id) {
            bool v = lua_toboolean(s,id) != 0;
            return Arg::ToCopy(v);
        }
        static void Push(lua_State* s, bool v) {
            lua_pushboolean(s,v ? 1 : 0);
        }
        enum { PushCount = 1 };
    };

    template <> struct TypeConverter<unsigned short> : public impl::IntegerConverter<unsigned short>{};
    template <> struct TypeConverter<unsigned int> : public impl::IntegerConverter<unsigned int>{};
    template <> struct TypeConverter<unsigned long> : public impl::IntegerConverter<unsigned long>{};
    template <> struct TypeConverter<unsigned long long> : public impl::IntegerConverter<unsigned long long>{};
    template <> struct TypeConverter<short> : public impl::IntegerConverter<short>{};
    template <> struct TypeConverter<int> : public impl::IntegerConverter<int>{};
    template <> struct TypeConverter<long> : public impl::IntegerConverter<long>{};
    template <> struct TypeConverter<long long> : public impl::IntegerConverter<long long>{};

    template <> struct TypeConverter<float> : public impl::NumberConverter<float>{};
    template <> struct TypeConverter<double> : public impl::NumberConverter<double>{};
    template <> struct TypeConverter<long double> : public impl::NumberConverter<long double>{};

    template <typename T> struct TypeConverter<Lua::Array<T>> {
        typedef typename std::enable_if< TypeConverter<T>::PushCount == 1,
            Lua::Arg<Lua::Array<Lua::Arg<T>>>
        >::type Arg;
        static Arg Read(lua_State* s, int id) {
            if(!lua_istable(s,id))
                return Arg::Nil();
            Lua::Array<T> v;
            for(int i = 1; ; ++i)
            {
                lua_pushinteger(s,i);
                if(!lua_next(s,id))
                    break;
                v.m_data.push_back(TypeConverter<T>::Read(s,lua_absindex(s,-1)));
                lua_pop(s,2);
            }
            return Arg::ToMove(std::move(v));
        }
        static void Push(lua_State* s, Lua::Array<T> const& v) {
            lua_createtable(s,v.m_data.size(),0);
            for(std::size_t i = 0; i < v.m_data.size(); ++i) {
                TypeConverter<T>::Push(s,v[i]);
                lua_rawseti(s,-2,i+1);
            }
        }
        enum { PushCount = 1 };
    };

    template <typename T> struct TypeConverter<Lua::Map<T>> {
        typedef typename std::enable_if< TypeConverter<T>::PushCount == 1,
            Lua::Arg<Lua::Map<Lua::Arg<T>>>
        >::type Arg;
        static Arg Read(lua_State* s, int id) {
            if(!lua_istable(s,id))
                return Arg::Nil();
            Lua::Map<T> m;
            for(lua_pushnil(s); lua_next(s,id); lua_pop(s,1))
            {
                TypeConverter<std::string>::Arg arg = TypeConverter<std::string>::Read(s,lua_absindex(s,-2));
                if(!arg)
                    continue;
                m.m_data.insert(std::make_pair(
                    *arg,
                    TypeConverter<T>::Read(s,lua_absindex(s,-1))
                ));
            }
            return m;
        }
        static void Push(lua_State* s, Lua::Map<T> const& v) {
            lua_createtable(s,0,v.m_data.size());
            for(auto it = v.m_data.begin(); it != v.m_data.end(); ++it)
            {
                TypeConverter<std::string>::Push(s, it->first);
                TypeConverter<T>::Push(s, it->second);
                lua_settable(s,-3);
            }
        }
        enum { PushCount = 1 };
    };

    template <> struct TypeConverter<Lua::ReturnValues<>> {
        static void Push(lua_State*, Lua::ReturnValues<> const&) {}
        enum { PushCount = 0 };
    };

    namespace impl {
        class ItemPusher {
            lua_State* m_state;
        public:
            ItemPusher(lua_State* state)
                : m_state(state) {}

            template <typename T>
            void operator() (T&& v)
            {
                TypeConverter<T>::Push(m_state,std::forward<T>(v));
            }
        };

        template <typename ...> struct SizeCalculator;

        template <typename T>
        struct SizeCalculator<T> {
            constexpr std::size_t calc()
            {
                return TypeConverter<T>::PushCount;
            }
        };
        template <typename T, typename ... Args>
        struct SizeCalculator<T,Args...> {
            constexpr std::size_t calc()
            {
                return TypeConverter<T>::PushCount +
                        SizeCalculator<Args...>::calc();
            }
        };
        template <>
        struct SizeCalculator<> {
            constexpr std::size_t calc() { return 0; }
        };
    }
    template <typename T, typename ... Args> struct TypeConverter<Lua::ReturnValues<T,Args...>> {
        static void Push(lua_State* s, Lua::ReturnValues<T,Args...> const& args) {
            impl::ItemPusher ip(s);
            impl::for_each_in_tuple(args.m_data, ip);
        }

        enum {
            PushCount = impl::SizeCalculator<T,Args...>::calc()
        };
    };
}

#endif //__LUAPP_TYPEEXT_H__
