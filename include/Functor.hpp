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

#ifndef LUAPP_FUNCTOR_HPP
#define LUAPP_FUNCTOR_HPP

#include "FwdDecl.hpp"
#include <functional>

namespace Lua::impl {

class Functor {
	friend class State;
	static int RegisterMetatable(lua_State*);
	static int Call(lua_State*);
	static int Destroy(lua_State*);
	typedef std::function<int(Lua::State&)> functor_type;

public:
	static void Register(lua_State*);
	static int Push(lua_State*, functor_type);
};

}

#endif
