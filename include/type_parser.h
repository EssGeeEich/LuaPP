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
#include "fwd.h"
#include "types.h"
#include "state.h"
#include "variable.h"

namespace Lua {
    namespace impl {
        template <typename T> struct IntegerConverter {
            typedef typename std::enable_if<std::is_integral<T>::value, Lua::Arg<T> >::type Arg;
            static Arg Read(lua_State* s, int id) {
                if(lua_isinteger(s,id))
                {
                    int rv = 0;
                    lua_Integer i = lua_tointegerx(s,id,&rv);
                    if(!rv)
                        return Arg::Nil();
                    return Arg::ToCopy(i);
                }
                else if(lua_isnumber(s,id))
                {
                    int rv = 0;
                    lua_Number i = lua_tonumberx(s,id,&rv);
                    if(!rv)
                        return Arg::Nil();
                    return Arg::ToCopy(static_cast<lua_Integer>(i));
                }
                return Arg::Nil();
            }
            static int Push(lua_State* s, T const& v) {
                lua_pushinteger(s,v);
                return 1;
            }
            static std::string Name() {
                return "integer";
            }
        };
        template <typename T> struct NumberConverter {
            typedef typename std::enable_if<std::is_floating_point<T>::value, Lua::Arg<T> >::type Arg;
            static Arg Read(lua_State* s, int id) {
                if(lua_isnumber(s,id))
                {
                    int rv = 0;
                    lua_Number i = lua_tonumberx(s,id,&rv);
                    if(!rv)
                        return Arg::Nil();
                    return Arg::ToCopy(i);
                }
                else if(lua_isinteger(s,id))
                {
                    int rv = 0;
                    lua_Integer i = lua_tointegerx(s,id,&rv);
                    if(!rv)
                        return Arg::Nil();
                    return Arg::ToCopy(static_cast<lua_Number>(i));
                }
                return Arg::Nil();
            }
            static int Push(lua_State* s, T const& v) {
                lua_pushnumber(s,v);
                return 1;
            }
            static std::string Name() {
                return "number";
            }
        };
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
        static std::string Name() {
            return metatable::name();
        }
        template <typename U>
        static int Push(lua_State* s, U&& v) {
            typename GenericDecay<T>::type* p = nullptr;
            typename GenericDecay<U>::type* r = p;
            p = r; (void)p; (void)r;
            
            Lua::State state = Lua::State::use_existing_state(s);
            state.luapp_push_object<T>(std::forward<U>(v));
            return 1;
        }
    };

    // void
    template <> struct TypeConverter<void> {};
    
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
        static int Push(lua_State* s, std::string const& v) {
            lua_pushlstring(s,v.c_str(),v.size());
            return 1;
        }
        static std::string Name() {
            return "string";
        }
    };

    template <> struct TypeConverter<bool> {
        typedef Lua::Arg<bool> Arg;
        static Arg Read(lua_State* s, int id) {
            bool v = lua_toboolean(s,id) != 0;
            return Arg::ToCopy(v);
        }
        static int Push(lua_State* s, bool v) {
            lua_pushboolean(s,v ? 1 : 0);
            return 1;
        }
        static std::string Name() {
            return "boolean";
        }
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
        typedef Lua::Arg< Lua::Array<T> > Arg;
        static Arg Read(lua_State* s, int id) {
            if(!lua_istable(s,id))
                return Arg::Nil();
            Lua::Array<T> v;
            for(int i = 1; ; ++i)
            {
                lua_pushinteger(s,i);
                if(!lua_next(s,id))
                    break;
                Lua::Arg<T> argt = TypeConverter<T>::Read(s,lua_absindex(s,-1));
                if(!argt)
                    break;
                v.m_data.push_back(*argt);
                lua_pop(s,2);
            }
            return Arg::ToMove(std::move(v));
        }
        static int Push(lua_State* s, Lua::Array<T> const& v) {
            int top = lua_gettop(s);
            lua_createtable(s,v.m_data.size(),0);
            for(std::size_t i = 0; i < v.m_data.size(); ++i) {
                if(TypeConverter<T>::Push(s,v.m_data[i]) != 1)
                {
                    lua_settop(s,top);
                    throw lua_exception("Lua::Array Push: An argument is taking multiple spots in the stack.");
                }
                lua_rawseti(s,-2,i+1);
            }
            return 1;
        }
        static std::string Name() {
            return TypeConverter<T>::Name() + " array";
        }
    };

    template <typename T> struct TypeConverter<Lua::Map<T>> {
        typedef Lua::Arg<Lua::Map<T>> Arg;
        static Arg Read(lua_State* s, int id) {
            if(!lua_istable(s,id))
                return Arg::Nil();
            Lua::Map<T> m;
            for(lua_pushnil(s); lua_next(s,id); lua_pop(s,1))
            {
                TypeConverter<std::string>::Arg arg = TypeConverter<std::string>::Read(s,lua_absindex(s,-2));
                if(!arg)
                    continue;
                Lua::Arg<T> argt = TypeConverter<T>::Read(s,lua_absindex(s,-1));
                if(!argt)
                    continue;
                m.m_data.insert(std::make_pair(
                    *arg,
                    *argt
                ));
            }
            return m;
        }
        static int Push(lua_State* s, Lua::Map<T> const& v) {
            lua_createtable(s,0,v.m_data.size());
            for(auto it = v.m_data.begin(); it != v.m_data.end(); ++it)
            {
                TypeConverter<std::string>::Push(s, it->first);
                TypeConverter<T>::Push(s, it->second);
                lua_settable(s,-3);
            }
            return 1;
        }
        static std::string Name() {
            return TypeConverter<T>::Name() + " table";
        }
    };
	
	template <> struct TypeConverter<std::shared_ptr<Lua::Variable>> {
		typedef Lua::Arg<std::shared_ptr<Lua::Variable>> Arg;
		static Arg Read(lua_State* s, int id) {
			return Arg::ToMove(std::shared_ptr<Lua::Variable>((Lua::State::use_existing_state(s).luapp_read(id))));
		}
		static std::string Name() {
			return "variable";
		}
	};
    
    template <> struct TypeConverter<Lua::ReturnValues> {
        static int Push(lua_State* s, Lua::ReturnValues const& args) {
            return args.push(s);
        }
    };
}

#endif //__LUAPP_TYPEEXT_H__
