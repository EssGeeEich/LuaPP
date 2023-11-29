#include "Reference.h"
#include "State.hpp"

namespace Lua {

Reference::Reference() { reset(); }
Reference::Reference(std::weak_ptr<Lua::State> state, int refTable, int refKey) 
	: m_state(state), m_refTable(refTable), m_refKey(refKey) {}
Reference::Reference(Reference&& o) : Reference() { *this = std::move(o); }
Reference& Reference::operator= (Reference&& o) {
	std::swap(m_state, o.m_state);
	std::swap(m_refTable, o.m_refTable);
	std::swap(m_refKey, o.m_refKey);
	o.destroy();
	return *this;
}

void Reference::reset() {
	m_state.reset();
	m_refTable = LUA_REGISTRYINDEX;
	m_refKey = LUA_NOREF;
}

Reference::~Reference() { destroy(); }

void Reference::destroy() {
	std::shared_ptr<State> state = m_state.lock();
	if(state)
		state->luapp_destroy_reference(this);
	
	reset();
}

Reference::operator bool() const noexcept {
	return !(m_state.expired() || m_refKey == LUA_REFNIL || m_refKey == LUA_NOREF);
}

std::weak_ptr<Lua::State> Reference::state() const noexcept { return m_state; }
int Reference::table() const noexcept { return m_refTable; }
int Reference::key() const noexcept { return m_refKey; }

}
