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
**	File created on: 15/11/2015
*/

#ifndef __LUAPP_LIBRARY_H__
#define __LUAPP_LIBRARY_H__

#include <lua.hpp>
#include <map>
#include <functional>
#include <type_traits>
#include <vector>
#include "util.h"

template <typename T> struct MetatableDescriptor;
enum {
    MTD_NO_ARGS         =2,
    MTD_ONE_ARG           ,
    MTD_TWO_ARGS          ,
    MTD_THREE_ARGS        ,
    MTD_FOUR_ARGS         ,
    MTD_FIVE_ARGS         ,
    MTD_SIX_ARGS          ,
    MTD_SEVEN_ARGS        ,
    MTD_EIGHT_ARGS        ,
    MTD_FIRST_ARG       =3,
    MTD_SECOND_ARG        ,
    MTD_THIRD_ARG         ,
    MTD_FOURTH_ARG        ,
    MTD_FIFTH_ARG         ,
    MTD_SIXTH_ARG         ,
    MTD_SEVENTH_ARG       ,
    MTD_EIGHTH_ARG
};
/*	template <> struct MetatableDescriptor<std::string> {
 *		static char const* name() { return "stdstring"; }
 *		static char const* constructor() { return "create"; }
 *		static void metatable(Lua::function_storage<std::string>& mt) {
 *			// Add metatable functions
 *			mt["display"] = [](std::string& v, lua_State*) -> int {
 *				std::cout << v;
 *				return 0;
 *			};
 *      }
 *	};
 */

namespace Lua {
    template <typename T>
    class ClassMemberFunctor {
        std::function<int(T&, lua_State*)> m_functor;
    public:
        ClassMemberFunctor() {}
        ClassMemberFunctor(std::function<int(T&, lua_State*)> f) : m_functor(std::move(f)) {}

        int operator() (T& data, lua_State* state) const {
            if(m_functor)
                return m_functor(data,state);
            return 0;
        }
    };
    template <typename T> using member_function_storage = std::map<std::string,ClassMemberFunctor<T>>;

    //class FunctionFunctor {
    //    std::function<int(lua_State*)> m_functor;
    //public: // This also requires the default constructor.
    //    FunctionFunctor(std::function<int(lua_State*)> f);
    //    int operator() (lua_State* state) const;
    //};
    //typedef std::map<std::string,FunctionFunctor> function_storage;

    template <typename T>
    struct MetatableDescriptorImpl {
        static char const* name() { return MetatableDescriptor<T>::name(); }
        static char const* constructor() { return MetatableDescriptor<T>::constructor(); }
        static char const* luaname() { return MetatableDescriptor<T>::luaname(); }
        static void metatable(Lua::member_function_storage<T>* & dest) {
            static Lua::member_function_storage<T> mt;
            if(mt.empty())
                MetatableDescriptor<T>::metatable(mt);
            dest = &mt;
        }
    };

    class LuaFunctor {
        friend class State;
        static int RegisterMetatable(lua_State*);
        static int Call(lua_State*);
        static int Destroy(lua_State*);
        typedef std::function<int(lua_State*)> functor_type;
    public:
        static void Register(lua_State*);
        static int Push(lua_State*, functor_type);
    };

    template <typename T> class MetatableManager;
    template <typename T>
    class MetatableManager<MetatableDescriptorImpl<T>> {
        typedef MetatableDescriptorImpl<T> metatable;

        static int RegisterMetatable(lua_State* state) {
            luaL_newmetatable(state, metatable::name());
            lua_pushcfunction(state, &MetatableManager<metatable>::Destroy);
            lua_setfield(state, -2, "__gc");
            lua_pushcfunction(state, &MetatableManager<metatable>::Index);
            lua_setfield(state, -2, "__index");
            lua_pop(state, 1);

            std::vector<luaL_Reg> reg;
            reg.push_back({metatable::constructor(), &MetatableManager<metatable>::LuaConstruct});
            reg.push_back({nullptr, nullptr});

            lua_createtable(state, 0, reg.size() - 1);
            luaL_setfuncs(state,reg.data(),0);
            return 1;
        }
        static int Index(lua_State* state) {
            T* p = (T*)(luaL_checkudata(state,1,metatable::name()));
            if(!p)
                return 0;

            std::string str;

            {
                size_t len = 0;
                char const* ptr = lua_tolstring(state,2,&len);
                str.assign(ptr,len);
            }

            Lua::member_function_storage<T>* mtPtr = nullptr;
            metatable::metatable(mtPtr);
            if(mtPtr)
            {
                Lua::member_function_storage<T>& mt = *mtPtr;
                auto it = mt.find(str);
                if(it == mt.end())
                    return 0;
                typename member_function_storage<T>::value_type::second_type selected_function = it->second;
                return LuaFunctor::Push(state, [=](lua_State* state) -> int { return selected_function(*p, state); });
            }
            return 0;
        }
        static int Destroy(lua_State* state) {
            T* p = (T*)(luaL_checkudata(state,1,metatable::name()));
            if(p)
            {
                try {
                    p->~T();
                } catch(lua_exception& e) {
                    return luaL_error(state, "C++ / Lua Exception thrown while destructing object %s.\n%s", metatable::name(), e.what());
                } catch(std::exception& e) {
                    return luaL_error(state, "C++ Exception thrown while destructing object %s.\n%s", metatable::name(), e.what());
                } catch(...) {
                    return luaL_error(state, "Unknown C++ Exception thrown while destructing object %s.",metatable::name());
                }
            }
            return 0;
        }
        static int LuaConstruct(lua_State* state) {
            return ConstructLua(state);
        }
    public:
        static T* FromStack(lua_State* state, int arg) {
            return (T*)luaL_checkudata(state,arg,metatable::name());
        }
        template <typename ... Args>
        static T* Construct(lua_State* state, Args&& ... args) {
            T* p = (T*)lua_newuserdata(state, sizeof(T));
            if(!p)
                return nullptr;
            luaL_getmetatable(state, metatable::name());
            try {
                new (p) T(std::forward<Args>(args)...);
            } catch(lua_exception& e) {
                lua_pop(state, 2);
                luaL_error(state, "C++ / Lua Exception thrown while constructing object %s.\n%s", metatable::name(), e.what());
                return nullptr;
            } catch(std::exception& e) {
                lua_pop(state, 2);
                luaL_error(state, "C++ Exception thrown while constructing object %s.\n%s", metatable::name(), e.what());
                return nullptr;
            } catch(...) {
                lua_pop(state,2);
                luaL_error(state, "Unknown C++ Exception thrown while constructing object %s.",metatable::name());
                return nullptr;
            }

            lua_setmetatable(state, -2);
            return p;
        }
        template <typename ... Args>
        static int ConstructLua(lua_State* state, Args&& ... args) {
            return Construct(state, std::forward<Args>(args)...) ? 1 : 0;
        }

        static void Register(lua_State* state) {
            LuaFunctor::Register(state);
            luaL_requiref(state, metatable::luaname(), &MetatableManager<metatable>::RegisterMetatable,1);
            lua_pop(state, 1);
        }
    };
}

#endif // __LUAPP_LIBRARY_H__
