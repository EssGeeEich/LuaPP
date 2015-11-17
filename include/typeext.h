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
template <> struct FromLua<char> {
	static char transform(lua_State* s, int id) {
		if(!lua_isstring(s,id))
			throw Lua::invalid_lua_arg(s,id);
		size_t size = 0;
		char const* str= lua_tolstring(s,id,&size);
		if(size != 1)
			throw Lua::invalid_lua_arg(s,id);
		return *str;
	}
};
template <> struct FromLua<char const*> {
	static char const* transform(lua_State* s, int id) {
		if(!lua_isstring(s,id))
			throw Lua::invalid_lua_arg(s,id);
		size_t size = 0;
		char const* str = lua_tolstring(s,id,&size);
		if(!str)
			throw Lua::invalid_lua_arg(s,id);
		return str;
	}
};
template <> struct FromLua<unsigned long long> {
	static unsigned long long transform(lua_State* s, int id) {
		if(!lua_isnumber(s,id))
			throw Lua::invalid_lua_arg(s,id);
		return lua_tointeger(s,id);
	}
};
template <> struct LuaPush<char const*> {
	static int push(lua_State* state, char const* ptr) {
		if(!ptr)
			lua_pushnil(state);
		else
			lua_pushstring(state,ptr);
		return 1;
	}
};
template <> struct LuaPush<char> {
	static int push(lua_State* state, char p) {
		lua_pushlstring(state,&p,1);
		return 1;
	}
};
#endif // __LUAPP_TYPEEXT_H__
