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

#ifndef __LUAPP_TRANSFORM_H__
#define __LUAPP_TRANSFORM_H__

#include <functional>
#include <lua.hpp>

template <typename ...> struct FromLua;
template <typename> struct LuaPush;
template <> struct LuaPush<void> { static int push(lua_State*, ...) { return 0; } };
template <> struct LuaPush<std::string> {
	static int push(lua_State* state, std::string const& p) {
		lua_pushlstring(state,p.c_str(),p.size());
		return 1;
	}
};

namespace Lua {
	class invalid_lua_arg {
		int m_which;
	public:
		invalid_lua_arg(lua_State* state, int id);
		int which() const;
	};

	namespace impl {
		template <typename RetVal, int ArgCount> struct TransformClassCallImpl {
			template <typename Class, typename ... Args>
			static int CallAndPush(Class& instance, RetVal (Class::* fptr)(Args...), lua_State* state, int firstID)
			{
				return LuaPush<typename std::decay<RetVal>::type>::push(state,(instance.*fptr)(FromLua<Args...>::transform(state,firstID--)));
			}
			template <typename Class, typename ... Args>
			static int CallAndPush(Class& instance, RetVal (Class::* fptr)(Args...) const, lua_State* state, int firstID)
			{
				return LuaPush<typename std::decay<RetVal>::type>::push(state,(instance.*fptr)(FromLua<Args...>::transform(state,firstID--)));
			}
		};
		template <int ArgCount> struct TransformClassCallImpl<void, ArgCount> {
			template <typename Class, typename ... Args>
			static int CallAndPush(Class& instance, void (Class::* fptr)(Args...), lua_State* state, int firstID)
			{
				(instance.*fptr)(FromLua<Args...>::transform(state,firstID--));
				return 0;
			}
			template <typename Class, typename ... Args>
			static int CallAndPush(Class& instance, void (Class::* fptr)(Args...) const, lua_State* state, int firstID)
			{
				(instance.*fptr)(FromLua<Args...>::transform(state,firstID--));
				return 0;
			}
		};
		template <typename RetVal> struct TransformClassCallImpl<RetVal, 0> {
			template <typename Class>
			static int CallAndPush(Class& instance, RetVal (Class::* fptr)(), lua_State* state, int firstID)
			{
				return LuaPush<typename std::decay<RetVal>::type>::push(state,(instance.*fptr)());
			}
			template <typename Class>
			static int CallAndPush(Class& instance, RetVal (Class::* fptr)() const, lua_State* state, int firstID)
			{
				return LuaPush<typename std::decay<RetVal>::type>::push(state,(instance.*fptr)());
			}
		};
		template <> struct TransformClassCallImpl<void, 0> {
			template <typename Class>
			static int CallAndPush(Class& instance, void (Class::* fptr)(), lua_State*, int)
			{
				(instance.*fptr)();
				return 0;
			}
			template <typename Class>
			static int CallAndPush(Class& instance, void (Class::* fptr)() const, lua_State*, int)
			{
				(instance.*fptr)();
				return 0;
			}
		};
	}
	
	template <typename ... Args, typename RetVal, typename Class>
	std::function<int(Class&, lua_State*)> static TransformClassCall(RetVal (Class::* fptr)(Args...))
	{
		return [=](Class& instance, lua_State* state) -> int {
			if(lua_gettop(state) < static_cast<int>(sizeof...(Args)))
			{
				return luaL_error(state,"Invalid argument count!");
			}
			try {
				return impl::TransformClassCallImpl<RetVal,sizeof...(Args)>::CallAndPush(instance,fptr,state,lua_gettop(state));
			} catch(invalid_lua_arg& e) {
				return luaL_error(state,"Invalid argument %d!", e.which());
			}
		};
	}
	template <typename ... Args, typename RetVal, typename Class>
	std::function<int(Class&, lua_State*)> static TransformClassCall(RetVal (Class::* fptr)(Args...) const)
	{
		return [=](Class& instance, lua_State* state) -> int {
			if(lua_gettop(state) < static_cast<int>(sizeof...(Args)))
			{
				return luaL_error(state,"Invalid argument count!");
			}
			try {
				return impl::TransformClassCallImpl<RetVal,sizeof...(Args)>::CallAndPush(instance,fptr,state,lua_gettop(state));
			} catch(invalid_lua_arg& e) {
				return luaL_error(state,"Invalid argument %d!", e.which());
			}
		};
	}
	
	template <typename Pointer, Pointer fptr> constexpr Pointer resolve() { return fptr; }
}

#endif // __LUAPP_TRANSFORM_H__
