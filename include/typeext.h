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
**	File created on: 17/11/2015
*/

#ifndef __LUAPP_TYPEEXT_H__
#define __LUAPP_TYPEEXT_H__
#include <lua.hpp>
#include <string>
#include <vector>
#include <map>
#include "library.h"

namespace Lua {
	class invalid_lua_arg : public std::exception {
		int m_which;
		std::string m_what;
	public:
		invalid_lua_arg(lua_State* state, int id, std::string what = std::string());
		int which() const;
		char const* what() const noexcept override;
	};
	
	template <typename T> struct GenericDecay {
		typedef T type;
	};
	template <typename T> struct GenericDecay<T*> {
		typedef T* type;
	};
	template <typename T> struct GenericDecay<T const&> {
		typedef T type;
	};
	template <typename T> struct GenericDecay<T&> {
		typedef T type;
	};
	template <typename T> struct GenericDecay<T&&> {
		typedef T type;
	};
	
	template <typename T> struct ReturnDecay {
		typedef T type;
	};
	template <typename T> struct ReturnDecay<T*> {
		typedef T* type;
	};
	template <typename T> struct ReturnDecay<T const&> {
		typedef T type;
	};
	template <typename T> struct ReturnDecay<T&> {
		typedef void type;
	};
	template <typename T> struct ReturnDecay<T&&> {
		typedef T type;
	};
}


template <typename ...> struct FromLua;
template <typename> struct LuaPush;
template <> struct LuaPush<void> { static int push(lua_State*, ...) { return 0; } };

// Lua ---> C++ Function

template <typename T> struct IntegerFromLua {
	static T transform(lua_State* s, int id) {
		if(!lua_isinteger(s,id))
			throw Lua::invalid_lua_arg(s,id,"Argument %d is not an integer!");
		int rv = 0;
		lua_Integer i = lua_tointegerx(s,id,&rv);
		if(!rv)
			throw Lua::invalid_lua_arg(s,id,"Argument %d could not be converted to an integer!");
		return static_cast<T>(i);
	}
};

template <typename T> struct NumberFromLua {
	static T transform(lua_State* s, int id) {
		if(!lua_isnumber(s,id))
			throw Lua::invalid_lua_arg(s,id,"Argument %d is not a number!");
		int rv = 0;
		lua_Number i = lua_tonumberx(s,id,&rv);
		if(!rv)
			throw Lua::invalid_lua_arg(s,id,"Argument %d could not be converted to a number!");
		return static_cast<T>(i);
	}
};

template <> struct FromLua<signed char> {
	static signed char transform(lua_State* s, int id) {
		if(!lua_isstring(s,id))
			throw Lua::invalid_lua_arg(s,id, "Argument %d does not contain a character!");
		size_t size = 0;
		char const* str= lua_tolstring(s,id,&size);
		if(!str || size != 1)
			throw Lua::invalid_lua_arg(s,id, "Argument %d does not contain a character!");
		return static_cast<signed char>(*str);
	}
};
template <> struct FromLua<char> {
	static char transform(lua_State* s, int id) {
		if(!lua_isstring(s,id))
			throw Lua::invalid_lua_arg(s,id, "Argument %d does not contain a character!");
		size_t size = 0;
		char const* str= lua_tolstring(s,id,&size);
		if(!str || size != 1)
			throw Lua::invalid_lua_arg(s,id, "Argument %d does not contain a character!");
		return *str;
	}
};

template <> struct FromLua<short> : public IntegerFromLua<short> {};
template <> struct FromLua<unsigned short> : public IntegerFromLua<unsigned short>{};
template <> struct FromLua<int> : public IntegerFromLua<int> {};
template <> struct FromLua<unsigned int> : public IntegerFromLua<unsigned int>{};
template <> struct FromLua<long> : public IntegerFromLua<long> {};
template <> struct FromLua<unsigned long> : public IntegerFromLua<unsigned long>{};
template <> struct FromLua<long long> : public IntegerFromLua<long long> {};
template <> struct FromLua<unsigned long long> : public IntegerFromLua<unsigned long long>{};
template <> struct FromLua<unsigned char> : public IntegerFromLua<unsigned char>{};

template <> struct FromLua<float> : public NumberFromLua<float> {};
template <> struct FromLua<double> : public NumberFromLua<double> {};
template <> struct FromLua<long double> : public NumberFromLua<long double> {};

template <> struct FromLua<char const*> {
	static char const* transform(lua_State* s, int id) {
		if(!lua_isstring(s,id))
			throw Lua::invalid_lua_arg(s,id,"Argument %d is not a string!");
		size_t size = 0;
		char const* str = lua_tolstring(s,id,&size);
		if(!str)
			throw Lua::invalid_lua_arg(s,id,"Argument %d is not a string!");
		return str;
	}
};
template <> struct FromLua<std::string> {
	static std::string transform(lua_State* s, int id) {
		if(!lua_isstring(s,id))
			throw Lua::invalid_lua_arg(s,id,"Argument %d is not a string!");
		size_t size = 0;
		char const* str = lua_tolstring(s,id,&size);
		if(!str)
			throw Lua::invalid_lua_arg(s,id,"Argument %d is not a string!");
		return std::string(str,size);
	}
};
template <typename T> struct FromLua<std::vector<T>> {
	static std::vector<T> transform(lua_State* s, int id) {
		std::vector<T> elem;
		for(int i=1; ; ++i)
		{
			lua_pushinteger(s,i);
			if(!lua_next(s,id))
				break;
			elem.push_back(FromLua<T>::transform(s,lua_absindex(s,-1)));
			lua_pop(s,2);
		}
		return elem;
	}
};
template <typename T> struct FromLua<std::map<std::string,T>> {
	static std::map<std::string,T> transform(lua_State* state, int id) {
		std::map<std::string,T> elem;
		for(lua_pushnil(state); lua_next(state,id); lua_pop(state,1))
		{
			elem[
				FromLua<std::string>::transform(state,lua_absindex(state,-2))
			] =
				FromLua<T>::transform(state,lua_absindex(state,-1));
		}
		return elem;
	}
};

// C++ Function ---> Lua
template <typename T> struct IntegerToLua {
	static int push(lua_State* state, T const& num) {
		lua_pushinteger(state,static_cast<lua_Integer>(num));
		return 1;
	}
};
template <typename T> struct NumberToLua {
	static int push(lua_State* state, T const& num) {
		lua_pushnumber(state,static_cast<lua_Number>(num));
		return 1;
	}
};

template <> struct LuaPush<signed char> {
	static int push(lua_State* state, signed char ch) {
		char b = static_cast<char>(ch);
		lua_pushlstring(state,&b,1);
		return 1;
	}
};
template <> struct LuaPush<char> {
	static int push(lua_State* state, char ch) {
		lua_pushlstring(state,&ch,1);
		return 1;
	}
};

template <> struct LuaPush<short> : public IntegerToLua<short> {};
template <> struct LuaPush<unsigned short> : public IntegerToLua<unsigned short> {};
template <> struct LuaPush<int> : public IntegerToLua<int> {};
template <> struct LuaPush<unsigned int> : public IntegerToLua<unsigned int> {};
template <> struct LuaPush<long> : public IntegerToLua<long> {};
template <> struct LuaPush<unsigned long> : public IntegerToLua<unsigned long> {};
template <> struct LuaPush<long long> : public IntegerToLua<long long> {};
template <> struct LuaPush<unsigned long long> : public IntegerToLua<unsigned long long> {};
template <> struct LuaPush<unsigned char> : public IntegerToLua<unsigned char> {};

template <> struct LuaPush<float> : public NumberToLua<float> {};
template <> struct LuaPush<double> : public NumberToLua<double> {};
template <> struct LuaPush<long double> : public NumberToLua<long double> {};


template <> struct LuaPush<char const*> {
	static int push(lua_State* state, char const* ptr) {
		if(!ptr)
			lua_pushnil(state);
		else
			lua_pushstring(state,ptr);
		return 1;
	}
};
template <> struct LuaPush<std::string> {
	static int push(lua_State* state, std::string const& v) {
		lua_pushlstring(state,v.c_str(),v.size());
		return 1;
	}
};
template <typename T> struct LuaPush<std::vector<T>> {
	static int push(lua_State* state, std::vector<T> const& v) {
		lua_createtable(state,v.size(),0);
		for(typename std::vector<T>::size_type i=0; i < v.size(); ++i) {
			int rv = LuaPush<T>::push(state,v);
			if(rv == 1)
			{ lua_rawseti(state,-2,i+1); }
			else if(rv != 0)
			{ luaL_error(state,"Error returning values from C++: LuaPush did not return successfully."); }
			// else nothing returned - ignore this kv pair
		}
		return 1;
	}
};
template <typename T> struct LuaPush<std::map<std::string,T>> {
	static int push(lua_State* state, std::map<std::string,T> const& v) {
		lua_createtable(state,0,v.size());
		for(auto it = v.begin(); it != v.end(); ++it)
		{
			int rv = LuaPush<std::string>::push(state,it->first);
			if(rv != 1)
				luaL_error(state,"Error returning values from C++: LuaPush did not return successfully.");
			rv = LuaPush<T>::push(state,it->second);
			if(rv == 0)
			{ lua_pop(state,1); }
			else if(rv == 1)
			{ lua_settable(state,-3); }
			else
			{ luaL_error(state,"Error returning values from C++: LuaPush did not return successfully."); }
		}
		return 1;
	}
};
#endif // __LUAPP_TYPEEXT_H__
