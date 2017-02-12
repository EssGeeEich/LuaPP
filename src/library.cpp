#include "library.h"
#include "typeext.h"
namespace Lua {
    //FunctionFunctor::FunctionFunctor(std::function<int (lua_State *)> f)
    //    : m_functor(std::move(f)) {}
    //int FunctionFunctor::operator ()(lua_State* state) const {
    //    if(m_functor)
    //        return m_functor(state);
    //    return 0;
    //}

    int LuaFunctor::RegisterMetatable(lua_State* state)
    {
        luaL_newmetatable(state, "LuaFunctor");
        lua_pushcfunction(state, &LuaFunctor::Destroy);
        lua_setfield(state, -2, "__gc");
        lua_pushcfunction(state, &LuaFunctor::Call);
        lua_setfield(state, -2, "__call");
        lua_pop(state, 1);
        return 0;
    }
    int LuaFunctor::Call(lua_State* state)
    {
        functor_type* p = (functor_type*)(luaL_checkudata(state,1,"LuaFunctor"));
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
    int LuaFunctor::Destroy(lua_State* state)
    {
        functor_type* p = (functor_type*)(luaL_checkudata(state,1,"LuaFunctor"));
        if(p)
            p->~functor_type();
        return 0;
    }
    void LuaFunctor::Register(lua_State* state)
    {
        luaL_requiref(state, "LuaFunctor", &LuaFunctor::RegisterMetatable, 1);
        lua_pop(state,1);
    }
    int LuaFunctor::Push(lua_State* s, functor_type f)
    {
        functor_type* p = (functor_type*)(lua_newuserdata(s,sizeof(functor_type)));
        if(!p)
            return 0;
        new (p) functor_type(std::move(f));
        luaL_getmetatable(s, "LuaFunctor");
        lua_setmetatable(s, -2);
        return 1;
    }
}
