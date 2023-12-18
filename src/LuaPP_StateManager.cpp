#include "StateManager.hpp"
#include "State.hpp"

namespace Lua {

StateManager& StateManager::Get() {
	static StateManager manager;
	return manager;
}

StateManager::StateManager() {}

std::shared_ptr<Lua::State> StateManager::Create() {
	std::shared_ptr<Lua::State> newState = std::shared_ptr<Lua::State>(new Lua::State());
	if(!newState->GetState())
		return std::shared_ptr<Lua::State>();
	
	Cleanup();
	newState->setSelf(std::weak_ptr<Lua::State>(newState));
	m_states[newState->GetState()] = newState;
	return newState;
}

std::shared_ptr<Lua::State> StateManager::Find(lua_State* state) {
	auto it = m_states.find(state);
	if(it != m_states.end() && !it->second.expired())
		return it->second.lock();
	
	return std::shared_ptr<Lua::State>();
}

void StateManager::Cleanup() {
	for(auto it = m_states.begin(); it != m_states.end();)
		it = it->second.expired() ? m_states.erase(it) : std::next(it);
}

}
