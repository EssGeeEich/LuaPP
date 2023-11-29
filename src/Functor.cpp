#include "Functor.hpp"
#include "Utils.hpp"

namespace Lua::impl {

int Functor::RegisterMetatable(lua_State* state)
{
	luaL_newmetatable(state, "luapp_functor");
	lua_pushcfunction(state, &Functor::Destroy);
	lua_setfield(state, -2, "__gc");
	lua_pushcfunction(state, &Functor::Call);
	lua_setfield(state, -2, "__call");
	lua_pop(state, 1);
	return 0;
}

int Functor::Call(lua_State* state)
{
	functor_type* p = (functor_type*)(luaL_checkudata(state,1,"luapp_functor"));
	if(!p || !(*p))
		return 0;
	int rv = 0;
	try {
		rv = (*p)(state);
	} catch(lua_exception& e) {
		return luaL_error(state,"C++ / Lua Exception: %s",e.what());
	} catch(std::exception& e) {
		return luaL_error(state,"C++ Exception: %s",e.what());
	} catch(...) {
		return luaL_error(state,"Unknown C++ Exception thrown.");
	}
	return rv;
}

int Functor::Destroy(lua_State* state)
{
	functor_type* p = (functor_type*)(luaL_checkudata(state,1,"luapp_functor"));
	if(p)
		p->~functor_type();
	return 0;
}

void Functor::Register(lua_State* state)
{
	luaL_requiref(state, "luapp_functor", &Functor::RegisterMetatable, 1);
	lua_pop(state,1);
}

int Functor::Push(lua_State* s, functor_type f)
{
	functor_type* p = (functor_type*)(lua_newuserdata(s,sizeof(functor_type)));
	if(!p)
		return 0;
	new (p) functor_type(std::move(f));
	luaL_getmetatable(s, "luapp_functor");
	lua_setmetatable(s, -2);
	return 1;
}

}
