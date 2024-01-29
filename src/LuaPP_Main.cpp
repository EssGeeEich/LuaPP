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

#ifndef LUAPP_NO_MAIN_CPP

#include "LuaPP.hpp"
#include <iostream>
#include <functional>
#include <cmath>

class TestCase {
    std::string m_data;
	std::shared_ptr<Lua::Reference> m_callbackReference;
public:
    // Optional
    TestCase() {
        std::cout << "TestCase constructed!" << std::endl;
    }
	
	void SetCallback(std::shared_ptr<Lua::Reference> fncReference) {
		// Exercise for the reader: Verify that this is actually a function.
		// Tip: Retrieve the Lua State, push the reference, verify its type... just remember to pop it from the stack afterwards!
		m_callbackReference = fncReference;
	}
	
	void RunCallback(Lua::State* state) {
		if(m_callbackReference && *m_callbackReference) {
			std::cout << "About to call Callback!" << std::endl;
			state->luapp_push_reference(m_callbackReference);
			state->pcall();
			std::cout << "Callback called!" << std::endl;
		}
	}
	
	std::string Text() const {
		return m_data;
	}

    void SetText(std::string const& text) {
        std::cout << "TestCase SetText: " << text << std::endl;
        m_data = text;
    }

    void Display() {
        std::cout << "TestCase Display: " << m_data << std::endl;
    }
	
	// Any and all Lua::State* arguments do not require any arguments from the client-side, and will automatically point to the current Lua State.
	Lua::ManualReturnValues Clone(Lua::State* currentState) {
		TestCase* newTestCase = currentState->luapp_push_object<TestCase>();
		newTestCase->m_data = m_data + " (Clone)";
		return Lua::ManualReturnValues(1);
    }
	
	void SetText_OptionalConcat(std::string mainText, std::optional<std::string> optionalConcat) {
		if(optionalConcat)
			SetText(mainText + *optionalConcat);
		else
			SetText(mainText);
	}
    
    void DisplayArguments(int Arg1, lua_Number Arg2, std::string const& Arg3) {
        std::cout << "TestCase DisplayArguments." << std::endl
                  << "Arg1: " << Arg1 << std::endl
                  << "Arg2: " << Arg2 << std::endl
                  << "Arg3: " << Arg3 << std::endl;
    }
    
    void Const() const {
        std::cout << "TestCase Const." << std::endl;
    }
};

std::string GlobalFunctionTakingClass(TestCase* testCase) {
	std::cout << "This global function has received: " << testCase->Text() << std::endl;
	return "hello!";
}

// Create a metatable for TestCase.
// This object needs to have a public default constructor.
// You can construct it with more arguments using luapp_push_object<TestCase>.
template <> struct MetatableDescriptor<TestCase> {
    static char const* name() { return "testcase_metatable"; }
    static char const* luaname() { return "testcase"; }
    static char const* constructor() { return "create"; }
    static bool construct(TestCase* tc) { new(tc) TestCase(); return true; }
    
    static void metatable(Lua::member_function_storage<TestCase>& mt)
    {
        mt["SetText"] = Lua::Transform(&TestCase::SetText);
		mt["SetText_Concat"] = Lua::Transform(&TestCase::SetText_OptionalConcat);
        mt["Display"] = Lua::Transform(&TestCase::Display);
        mt["DisplayArgs"] = Lua::Transform(&TestCase::DisplayArguments);
        mt["Const"] = Lua::Transform(&TestCase::Const);
		mt["Clone"] = Lua::Transform(&TestCase::Clone);
		mt["Text"] = Lua::Transform(&TestCase::Text);
		mt["SetCallback"] = Lua::Transform(&TestCase::SetCallback);
		mt["RunCallback"] = Lua::Transform(&TestCase::RunCallback);
    }
};

int main()
{
	std::shared_ptr<Lua::State> state = Lua::StateManager::Get().Create();
	if(!state)
		return 1;
	
    state->luapp_register_metatables();
    state->luapp_register_object<TestCase>();
	
    state->luapp_add_translated_function("rand", Lua::Transform(std::rand));
	state->luapp_add_translated_function("MyGlobalFnc", Lua::Transform(GlobalFunctionTakingClass));
    state->loadstring(R"literal(

-- "TestCase constructed!"
local TestCase = testcase.create();    -- See luaname() and constructor()

-- "TestCase SetText: Hello World!"
TestCase:SetText("Hello World!");

-- "TestCase SetText: Hello A!"
TestCase:SetText_Concat("Hello A!");

-- "TestCase SetText: Hello B!"
TestCase:SetText_Concat("Hello", " B!");

-- Errors out
-- TestCase:SetText_Concat("Hello", " B,", " and C!");

-- Errors out
-- TestCase:SetText_Concat();

-- "TestCase Const."
TestCase:Const();

-- "TestCase Display: Hello B!"
TestCase:Display();

-- "TestCase DisplayArguments. Arg1: 41 - Arg2: 1.5 - Arg3: Something something"
TestCase:DisplayArgs(rand(), 3.0 * 0.5, "Something something");

-- TestCase constructed!
clone = TestCase:Clone()

-- TestCase Display: Hello B! (Clone)
clone:Display();

clone:SetCallback(function()
	clone:SetText(""..rand())
end)

-- "This global function has received: Hello B! (Clone)"
MyGlobalFnc(clone);

-- About to call Callback!
-- TestCase SetText: 12345
-- Callback called!
clone:RunCallback();

)literal");
	
	try {
		state->call();
	} catch(Lua::lua_exception& e) {
		std::cout << "Lua exception: " << e.what() << std::endl;
		return 1;
	} catch(std::exception& e) {
		std::cout << "C++ exception: " << e.what() << std::endl;
		return 1;
	} catch(...) {
		std::cout << "Unknown C++ exception!" << std::endl;
		return 1;
	}	

	return 0;
}

#endif
