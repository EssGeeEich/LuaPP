#include "State.hpp"
#include <utility>

namespace Lua {

State::State()
	: m_state(luaL_newstate()) {}
State::State(State&& o)
	: m_state(nullptr) {
	*this = std::move(o);
}
State& State::operator=(State&& o) {
	std::swap(m_state, o.m_state);
	std::swap(m_self, o.m_self);
	o.close();
	return *this;
}
State::~State() {
	close();
}

void State::setSelf(std::weak_ptr<State> self) {
	m_self = self;
}
void State::close() {
	if(!m_state)
		return;
	lua_close(m_state);
	m_state = nullptr;
}

State::operator bool() const noexcept {
	return !!m_state;
}
bool State::operator!() const noexcept {
	return !m_state;
}
lua_State* State::GetState() const noexcept {
	return m_state;
}

bool State::IsValidIndex(int index) {
	return (1 <= index) && (index <= gettop());
}
bool State::IsAcceptableIndex(int index) {
	return type(index) != TP_NONE;
}

void State::luapp_register_metatables() {
	impl::Functor::Register(GetState());
}

char const* State::pushstdstring(std::string const& p) {
	return pushlstring(p.c_str(), p.size());
}
std::string State::tostdstring(int index) {
	size_t sz(0);
	char const* str = tolstring(index, &sz);
	if(!str || !sz)
		return std::string();
	return std::string(str, sz);
}
std::string State::checkstdstring(int index) {
	size_t sz(0);
	char const* str = checklstring(index, &sz);
	if(!str || !sz)
		return std::string();
	return std::string(str, sz);
}

std::shared_ptr<Reference> State::luapp_pop_reference(int refTable) {
	return std::shared_ptr<Reference>(new Reference(m_self, refTable, ref(refTable)));
}
std::shared_ptr<Reference> State::luapp_read_reference(int index, int refTable) {
	pushvalue(index);
	return luapp_pop_reference(refTable);
}
void State::luapp_push_reference(std::shared_ptr<Reference> reference) {
	if(!reference || !*reference || reference->state().lock() != m_self.lock())
		pushnil();
	else
		rawgeti(reference->table(), reference->key());
}
void State::luapp_destroy_reference(std::shared_ptr<Reference> reference) {
	luapp_destroy_reference(reference.get());
}
void State::luapp_destroy_reference(Reference* reference) {
	if(!m_state || !reference || !*reference || reference->state().lock() != m_self.lock())
		return;
	unref(reference->table(), reference->key());
}
int State::luapp_push_translated_function(std::function<int(Lua::State&)> const& function) {
	return impl::Functor::Push(GetState(), function);
}

}
