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
    static void metatable(Lua::member_function_storage<std::string>& mt)
	{
        mt["clear"] = Lua::Transform(&std::string::clear);
        mt["append"] = Lua::Transform((std::string& (std::string::*)(char const*))&std::string::append);
        mt["append_str"] = Lua::TransformSelf(
            Lua::ToFnc(
                [](std::string& self, std::string other) -> int {
                    self += other;
                    return 0;
                }
            )
        );

        mt["push_back"] = mt["append"];
        mt["at"] = Lua::Transform((std::string::const_reference (std::string::*)(std::string::size_type) const)&std::string::at);
        mt["display"] = std::function<int(std::string&, lua_State*)>([](std::string& v, lua_State*) -> int {
            std::cout << "Metatable 'display' called. Value: " << v << std::endl;
            return 0;
        });
	}
};

int main()
{
	Lua::State state = Lua::State::create();
    state.luapp_register_metatables();
    state.luapp_register_object<std::string>();
	
    if(state.luapp_push_object<std::string>("reat!"))
        state.setglobal("z");
	state.loadstring(
		"x = stdstring.create()\n"\
		"x:append('hi guy')\n"\
		"y = stdstring.create()\n"\
		"y:append(x:at(3))\n"\
        "y:append_str(z)\n"\
		"y:display()"
	);
	state.call();
	return 0;
}
