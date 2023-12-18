#include "Utils.hpp"

namespace Lua {
    lua_exception::lua_exception(std::string what)
        : m_what(std::move(what)) {}
    void lua_exception::setText(std::string what)
    {
        m_what = std::move(what);
    }
    char const* lua_exception::what() const noexcept { return m_what.c_str(); }
}
