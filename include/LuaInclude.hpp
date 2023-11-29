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

#ifndef LUAPP_LUAINCLUDE_HPP
#define LUAPP_LUAINCLUDE_HPP

extern "C" {

#if USE_SYSTEM_LUA
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#else
#include "dep/lua/lua.h"
#include "dep/lua/lualib.h"
#include "dep/lua/lauxlib.h"
#endif

}

#endif
