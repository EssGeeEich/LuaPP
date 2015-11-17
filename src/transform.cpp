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

#include "transform.h"
#include <utility>

namespace Lua {
	invalid_lua_arg::invalid_lua_arg(lua_State* state, int id, std::string what)
		: m_which( (lua_gettop(state) - id) + 1 ), m_what(std::move(what)) {
		if(m_what.empty())
			m_what = "Invalid argument: %d!";
	}
	int invalid_lua_arg::which() const { return m_which; }
	char const* invalid_lua_arg::what() const noexcept { return m_what.c_str(); }
}
