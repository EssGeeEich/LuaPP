#include "Utils.hpp"

#ifdef _DEBUG
#	include <map>
#	include <iostream>
#endif

namespace Lua {
#ifdef _DEBUG
std::map<AllocType, int> allocations;

void markAllocation(AllocType alloc, int count) {
	auto it = allocations.find(alloc);
	if(it == allocations.end()) {
		allocations[alloc] = count;
	}
	else {
		it->second += count;
	}
}
void printAllocations() {
	for(auto it = allocations.begin(); it != allocations.end(); ++it) {
		std::cout << "AT: " << it->first << " = " << it->second << std::endl;
	}
	std::cout << "---" << std::endl;
}
#else
void markAllocation(AllocType, int) {}
void printAllocations() {}
#endif

lua_exception::lua_exception(std::string what)
	: m_what(std::move(what)) {}
void lua_exception::setText(std::string what) {
	m_what = std::move(what);
}
char const* lua_exception::what() const noexcept {
	return m_what.c_str();
}

}
