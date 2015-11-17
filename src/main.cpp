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
**	File created on: 15/11/2015
*/

#include "state.h"
#include <iostream>

// Create a metatable for std::string.
// The destructor and constructor for this object are the default ones.
// They need to be public.
template <> struct MetatableDescriptor<std::string> {
	static char const* name() { return "stdstring_mt"; }	// Actual Lua metatable name
	static char const* luaname() { return "stdstring"; }	// Table name, to call the constructor from
	static char const* constructor() { return "create"; }	// Constructor method name. To be used like "stdstring.create()"
	static Lua::function_storage<std::string> metatable()	// Return a std::map<string, function> of class methods.
	{
		static Lua::function_storage<std::string> mt;		// Making this static: Performs better.
		if(mt.empty()) {
			// Populate the methods table:
			// Erase the string's contents.
			mt["clear"] = Lua::TransformClassCall(&std::string::clear);
			
			// Append a string. This function has multiple overloads, so we need to specify the one we want.
			mt["append"] = Lua::TransformClassCall(Lua::resolve<std::string& (std::string::*) (char const*), &std::string::append>());
			
			// Let's make push_back an alias for append.
			mt["push_back"] = mt["append"];
			
			// Return a character. Please note that C++ strings are zero-indexed.
			// Also, we cannot return C++ references - they are converted to plain lua variables. You can't return a stdstring yet.
			mt["at"] = Lua::TransformClassCall(Lua::resolve<std::string::const_reference (std::string::*)(std::string::size_type) const,&std::string::at>());
			
			// Display the string. This function shows you how a behind-the-scenes conversion looks like.
			// The lua_State* will contain the function parameters plus two.
			mt["display"] = [](std::string& v, lua_State*) -> int {
				std::cout << "metatable 'display' called: " << v << std::endl;
				return 0;
			};
		}
		return mt;
	}
};

int main()
{
	Lua::State state = Lua::State::create();
	state.AddObject<std::string>();
	
	state.loadstring(
		"x = stdstring.create()\n"\
		"x:append('hi guy')\n"\
		"y = stdstring.create()\n"\
		"y:append(x:at(3))\n"\
		"y:display()"
	);
	state.call();
	return 0;
}
