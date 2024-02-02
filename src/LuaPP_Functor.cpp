#include "Functor.hpp"
#include "Utils.hpp"
#include "StateManager.hpp"
#include "State.hpp"
#include <memory>

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

int Functor::Call(lua_State* s)
{	
	std::shared_ptr<Lua::State> state = Lua::StateManager::Get().Find(s);
	if(!state)
		return 0;
	
	functor_type* p = (functor_type*)(state->checkudata(1,"luapp_functor"));
    if(!p || !(*p))
		return 0;
	
	try {
		return (*p)(*state);
	} catch(lua_exception& e) {
		return state->error("C++ / Lua Exception: %s", e.what());
	} catch(std::exception& e) {
		return state->error("C++ Exception: %s", e.what());
	} catch(...) {
		return state->error("Unknown C++ Exception thrown.");
	}
}

int Functor::Destroy(lua_State* state)
{
	functor_type* p = (functor_type*)(luaL_checkudata(state,1,"luapp_functor"));
    if(p) {
		p->~functor_type();
        markAllocation(AT_UDATA, -1);
    }
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

    markAllocation(AT_UDATA, +1);
	new (p) functor_type(std::move(f));
	luaL_getmetatable(s, "luapp_functor");
	lua_setmetatable(s, -2);
	return 1;
}

}
