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

template <typename T> struct MetatableDescriptor;
/*	template <> struct MetatableDescriptor<std::string> {
 *		static char const* name() { return "stdstring"; }
 *		static char const* constructor() { return "create"; }
 *		static Lua::function_storage<std::string> metatable() {
 *			static Lua::function_storage<std::string> mt;
 *			if(mt.empty()) {
 *				// Add metatable functions
 *				mt["display"] = [](std::string& v, lua_State*) -> int {
 *					std::cout << v;
 *					return 0;
 *				};
 *			}
 *			return mt;
 *		}
 *	};
 */

namespace Lua {
	template <typename T> using function_storage = std::map<std::string,std::function<int(T&, lua_State*)>>;
	
	// Only accessible from ObjectWrapper.
	class FunctorWrapper {
		template <typename T> friend class ObjectWrapper;
		typedef std::function<int(lua_State*)> functor_type;
		
		static int Exec(lua_State* state) {
			functor_type* p = (functor_type*)(luaL_checkudata(state,1,"luapp_functor_metatable"));
			if(!p)
				return 0;
			return (*p)(state);
		}
		static int Destroy(lua_State* state) {
			functor_type* p = (functor_type*)(luaL_checkudata(state,1,"luapp_functor_metatable"));
			if(p)
				p->~functor_type();
			return 0;
		}
		static int ApplyStateFunc(lua_State* state)
		{
			luaL_newmetatable(state, "luapp_functor_metatable");
			lua_pushcfunction(state, &FunctorWrapper::Exec);
			lua_setfield(state, -2, "__call");
			lua_pushcfunction(state, &FunctorWrapper::Destroy);
			lua_setfield(state, -2, "__gc");
			lua_pop(state,1);
			return 0;
		}
		static int Construct(lua_State* state, functor_type callback, int ownerLocation)
		{
			functor_type* p = (functor_type*)lua_newuserdata(state,sizeof(functor_type));
			if(!p)
				return 0;
			new (p) functor_type(callback);
			luaL_getmetatable(state,"luapp_functor_metatable");
			lua_pushvalue(state, ownerLocation);
			lua_setfield(state,-2,"parent");
			lua_setmetatable(state,-2);
			return 1;
		}
	};
	
	// Only accessible from State.
	template <typename T>
	class ObjectWrapper {
		typedef MetatableDescriptor<T> metatable;
		friend class State;
		
		static int Destroy(lua_State* state)  {
			T* p = (T*)luaL_checkudata(state,1,metatable::name());
			if(p)
				p->~T();
			return 0;
		}
		static int ExecMetaTable(lua_State* state) {
			T* p = (T*)luaL_checkudata(state,1,metatable::name());
			if(!p)
				return 0;
			size_t len = 0;
			char const* ptr = lua_tolstring(state,2,&len);
			
			std::string str = std::string(ptr,len);
			function_storage<T> mt = MetatableDescriptor<T>::metatable();
			auto it = mt.find(str);
			if(it == mt.end() || !(it->second))
				return 0;
			typename function_storage<T>::value_type::second_type copy = it->second;
			return FunctorWrapper::Construct(state,[=](lua_State* s) -> int { return copy(*p,s); },1);
		}

		static int ConstructObject(lua_State* state) {
			T* p = (T*)lua_newuserdata(state,sizeof(T));
			if(!p)
				return 0;
			new (p) T();
			luaL_setmetatable(state, metatable::name());
			return 1;
		}
		static int ApplyStateFunc(lua_State* state) {
			luaL_Reg reg[2] = {
				{metatable::constructor(), &ObjectWrapper::ConstructObject},
				{nullptr, nullptr}
			};
			
			lua_createtable(state,0,1); // This is returned
			luaL_setfuncs(state,reg,0);
			
			luaL_newmetatable(state, metatable::name());
			lua_pushcfunction(state,&ObjectWrapper::ExecMetaTable);
			lua_setfield(state, -2, "__index");
			lua_pushcfunction(state,&ObjectWrapper::Destroy);
			lua_setfield(state, -2, "__gc");
			lua_pop(state,1);
			return 1;
		}

		static void applyState(lua_State* state) {
			luaL_requiref(state,"luapp_functor",&FunctorWrapper::ApplyStateFunc,1);
			lua_pop(state,1);
			luaL_requiref(state,metatable::luaname(),&ObjectWrapper::ApplyStateFunc,1);
			lua_pop(state,1);
		}
	public:
		// Push a new T to the stack
		static int PushNew(lua_State* state) {
			return ConstructObject(state);
		}
	};
}

#endif // __LUAPP_LIBRARY_H__
