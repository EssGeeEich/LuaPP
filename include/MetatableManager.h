/*	Copyright (c) 2023 Mauro Grassia
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
*/

#ifndef LUAPP_METATABLEMANAGER_HPP
#define LUAPP_METATABLEMANAGER_HPP

#include <map>
#include <functional>
#include <type_traits>
#include <vector>
#include <type_traits>
#include <string>

#include "LuaInclude.hpp"
#include "Utils.hpp"
#include "Functor.hpp"

template <typename T> struct MetatableDescriptor;

// TODO: Update
/*	template <> struct MetatableDescriptor<std::string> {
 *		static char const* name() { return "stdstring"; }
 *		static char const* constructor() { return "create"; }
 *      static bool construct(std::string* p) { return new (p) std::string(); }
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
	namespace impl {
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
	}
	
	template <typename T> using member_function_storage = std::map<std::string, impl::ClassMemberFunctor<T>>;
		
	namespace impl {	
		template <typename T>
		struct MetatableDescriptorImpl {
			// The mentioned type doesn't have a dedicated metatable!
			// Please write one!
			static char const* name() { return MetatableDescriptor<T>::name(); }
			static char const* constructor() { return MetatableDescriptor<T>::constructor(); }
			static char const* luaname() { return MetatableDescriptor<T>::luaname(); }
			static bool construct(T* location) { return MetatableDescriptor<T>::construct(location); }
			static void metatable(Lua::member_function_storage<T>* & dest) {
				static Lua::member_function_storage<T> mt;
				if(mt.empty())
					MetatableDescriptor<T>::metatable(mt);
				dest = &mt;
			}
		};
	}

	template <typename T> bool DefaultConstructor(T* location) {
		new (location) T();
		return true;
	}

    template <typename> class MetatableManager;
	
    template <typename T>
    class MetatableManager<impl::MetatableDescriptorImpl<T>> {
        typedef impl::MetatableDescriptorImpl<T> metatable;

        static int RegisterMetatable(lua_State* state) {
            int count = RegisterLoneMetatable(state);

            std::string constr = metatable::constructor();
            if(constr.empty())
                return count;
            
            std::vector<luaL_Reg> reg;
            reg.push_back({constr.c_str(), &MetatableManager<metatable>::ConstructLua});
            reg.push_back({nullptr, nullptr});

            lua_createtable(state, 0, static_cast<int>(reg.size() - 1));
            luaL_setfuncs(state,reg.data(),0);
            return count + 1;
        }
        static int RegisterLoneMetatable(lua_State* state) {
            luaL_newmetatable(state, metatable::name());
            lua_pushcfunction(state, &MetatableManager<metatable>::Destroy);
            lua_setfield(state, -2, "__gc");
            lua_pushcfunction(state, &MetatableManager<metatable>::Index);
            lua_setfield(state, -2, "__index");
            lua_pop(state, 1);
            return 0;
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
                return impl::Functor::Push(state, [=](lua_State* state) -> int { return selected_function(*p, state); });
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
        static int ConstructLua(lua_State* state) {
            T* p = (T*)lua_newuserdata(state, sizeof(T));
            if(!p)
                return 0;
            luaL_getmetatable(state, metatable::name());
            try {
                //new (p) T(std::forward<Args>(args)...);
                if(!metatable::construct(p))
                {
                    lua_pop(state, 2);
                    luaL_error(state, "C++ Error: Unable to construct object %s.\nUnknown error.", metatable::name());
                    return 0;     
                }
            } catch(lua_exception& e) {
                lua_pop(state, 2);
                luaL_error(state, "C++ / Lua Exception thrown while constructing object %s.\n%s", metatable::name(), e.what());
                return 0;
            } catch(std::exception& e) {
                lua_pop(state, 2);
                luaL_error(state, "C++ Exception thrown while constructing object %s.\n%s", metatable::name(), e.what());
                return 0;
            } catch(...) {
                lua_pop(state,2);
                luaL_error(state, "Unknown C++ Exception thrown while constructing object %s.",metatable::name());
                return 0;
            }

            lua_setmetatable(state, -2);
            return 1;
        }

        static void Register(lua_State* state, bool allowConstructor = true) {
            impl::Functor::Register(state);
            
            std::string lname = metatable::luaname();
            if(lname.empty())
            {
                lname = metatable::name();
                allowConstructor = false;
            }
            
            luaL_requiref(state, lname.c_str(), allowConstructor ?
                              &MetatableManager<metatable>::RegisterMetatable :
                              &MetatableManager<metatable>::RegisterLoneMetatable,
                          allowConstructor ? 1 : 0);
            lua_pop(state, 1);
        }
    };
	
}

#endif
