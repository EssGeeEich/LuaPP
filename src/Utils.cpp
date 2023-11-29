#include "Utils.hpp"

namespace Lua {
    invalid_lua_arg::invalid_lua_arg(lua_State* state, int id, std::string what)
        : lua_exception(what),
            m_which( (lua_gettop(state) - id) + 1 )
    {
        if(what.empty())
        {
			// Just to reserve enough memory to avoid reallocations...
			what.reserve(sizeof("Invalid argument: 999!"));
			
            what = "Invalid argument: ";
            what += std::to_string(m_which);
            what += "!";
            lua_exception::setText(what);
        }
	}
    int invalid_lua_arg::which() const { return m_which; }

    lua_exception::lua_exception(std::string what)
        : m_what(std::move(what)) {}
    void lua_exception::setText(std::string what)
    {
        m_what = std::move(what);
    }
    char const* lua_exception::what() const noexcept { return m_what.c_str(); }
}
