# LuaPP
LuaPP - Lua C++ Wrapper

This is the official project page for SGH's LuaPP - Lua C++ Wrapper!

Look at this piece of code, which shows you how easy it is to create a Lua context and wrap a C++ class:

```c++
#include <iostream> // For cout, endl and string. Also included from state.h
#include <luapp/state.h> // Everything under the Lua namespace.

// Consider a class like this:
class MyClass {
public:
  void print(std::string const& text) const {
    std::cout << text << std::endl;
  }
  void print_backwards(std::string const& text) const {
    for(size_t i = text.size(); i; --i)
    {
      std::cout << text[i-1];
    }
    std::cout << std::endl;
  }
  std::string return_value(std::string const& input) const {
    return input;
  }
};

// We now create a metatable descriptor for our object
template <> struct MetatableDescriptorImpl<MyClass> {
  // We provide a metatable name
  static char const* name() { return "myclass_mt"; }
  // We provide a table name, we will use this to construct our class from Lua code
  static char const* luaname() { return "MyClass"; }
  // We provide a constructor name, we will also use this to construct our class from Lua code
  static char const* constructor() { return "create"; }
  
  // We now provide the actual function table.
  // This needs to be global, so that every Lua state can find it without wasting Lua resources.
  static Lua::function_storage<MyClass> metatable()
  {
    static Lua::function_storage<MyClass> mt;
    if(mt.empty()) {
      // Load the metatable!
      // TransformClassCall will convert our functions to accept parameters from lua_State!
      mt["print"] = Lua::TransformClassCall(&MyClass::print);
      mt["rprint"] = Lua::TransformClassCall(&MyClass::print_backwards);
      mt["rv"] = Lua::TransformClassCall(&MyClass::return_value);
    }
    return mt;
  }
};

// Done! Now we can easily load our class with a SINGLE line of code!
int main()
{
  Lua::State state = Lua::State::create();
  // The following line registers MyClass.create to construct and return a MyClass instance.
  state.AddObject<MyClass>();
  
  state.loadstring(
    "x = MyClass.create()\n"\
    "x:print('Hello!')\n"\
    "x:rprint('Hello!')\n"\
    "x:print(\n"\
    "    x:rv('Printing a return value!')\n"\
    ")"
  );
  // The following line will raise an 'invalid argument' lua error, which can be caught with pcall.
  // state.loadstring("x = MyClass.create(); x:print({1,2,3});");
  state.call();
  return 0;
}
```

Erasing whitespaces and comments, this snippet is about 40 lines long, including the definition of our MyClass!

You can also check [`main.cpp`](https://github.com/EssGeeEich/LuaPP/blob/master/src/main.cpp) to see a wrapping demo for `std::string`.

Please note that LuaPP requires C++11 and a Lua library installed. It has been developed with Lua v5.3.
