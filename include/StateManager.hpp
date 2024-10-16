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

#ifndef LUAPP_STATEMANAGER_HPP
#define LUAPP_STATEMANAGER_HPP

#include "FwdDecl.hpp"
#include <vector>
#include <map>
#include <memory>

namespace Lua {

class StateManager {
public:
	static StateManager& Get();
	std::shared_ptr<Lua::State> Create();
	std::shared_ptr<Lua::State> Find(lua_State*);

private:
	void Cleanup();
	StateManager();

	std::map<lua_State*, std::weak_ptr<Lua::State>> m_states;
};

}

#endif
