# LuaPP
LuaPP - Lua C++ Wrapper

This is the official project page for SGH's LuaPP - Lua C++ Wrapper!

Look at this piece of code, which shows you how easy it is to create a Lua context and wrap a C++ class:

```c++
#include <iostream> // For cout and endl. Also included from state.h
#include <cstring> // For strlen.
#include <luapp/state.h> // Everything under the Lua namespace.

// Consider a class like this:
class MyClass {
public:
  void print(char const* text) const {
    std::cout << text << std::endl;
  }
  void print_backwards(char const* text) const {
    size_t len = strlen(text);
    for(size_t i = len; i; --i)
    {
      std::cout << text[i-1];
    }
    std::cout << std::endl;
  }
};

// We now create a metatable descriptor for our object
template <> struct MetatableDescriptor<MyClass> {
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
  
  state.loadstring("x = MyClass.create(); x:print('Hello!'); x:rprint('Hello!');");
  
  // The following line will raise an 'invalid argument' lua error, which can be caught with pcall.
  // state.loadstring("x = MyClass.create(); x:print({1,2,3});");
  state.call();
  return 0;
}
```

Erasing whitespaces and comments, this snippet is about 40 lines long, including the definition of our MyClass!

You can also check [`main.cpp`](https://github.com/EssGeeEich/LuaPP/blob/master/src/main.cpp) to see a wrapping demo for `std::string`.
