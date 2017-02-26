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
#include <cmath>

class TestCase {
    std::string m_data;
public:
    // Optional
    TestCase() {
        std::cout << "TestCase constructed!" << std::endl;
    }

    void SetText(std::string const& text) {
        std::cout << "TestCase SetText: " << text << std::endl;
        m_data = text;
    }

    void Display() {
        std::cout << "TestCase Display: " << m_data << std::endl;
    }
    
    void DisplayArguments(int Arg1, float Arg2, std::string const& Arg3) {
        std::cout << "TestCase DisplayArguments." << std::endl
                  << "Arg1: " << Arg1 << std::endl
                  << "Arg2: " << Arg2 << std::endl
                  << "Arg3: " << Arg3 << std::endl;
    }
    
    void Const() const {
        std::cout << "TestCase Const." << std::endl;
    }
};

// Create a metatable for TestCase.
// This object needs to have a public default constructor.
// You can construct it with more arguments using luapp_push_object<TestCase>.
template <> struct MetatableDescriptor<TestCase> {
    static char const* name() { return "testcase_metatable"; }
    static char const* luaname() { return "testcase"; }
    static char const* constructor() { return "create"; }
    
    static void metatable(Lua::member_function_storage<TestCase>& mt)
    {
        mt["SetText"] = Lua::Transform(&TestCase::SetText);
        mt["Display"] = Lua::Transform(&TestCase::Display);
        mt["DisplayArgs"] = Lua::Transform(&TestCase::DisplayArguments);
        mt["DisplayArgs_BoundArguments"] = Lua::Transform(&TestCase::DisplayArguments, 42, 3.14);
        mt["Const"] = Lua::Transform(&TestCase::Const);
    }
};

int main()
{
	Lua::State state = Lua::State::create();
    state.luapp_register_metatables();
    state.luapp_register_object<TestCase>();
	
    state.luapp_add_translated_function("rand", Lua::Transform(std::rand));
    
    state.loadstring(R"literal(
    local TestCase = testcase.create();    -- See luaname() and constructor()
    TestCase:SetText("Hello World!");
    TestCase:Const();
    TestCase:Display();
    TestCase:DisplayArgs(rand(), 3.0 * 0.5, "Something something");
    TestCase:DisplayArgs_BoundArguments("The first two arguments have been bound to 42 and 3.14.");
    )literal");
	state.call();
	return 0;
}
