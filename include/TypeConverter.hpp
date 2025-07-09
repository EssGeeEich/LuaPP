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

#ifndef LUAPP_TYPEPARSER_HPP
#define LUAPP_TYPEPARSER_HPP

#include "FwdDecl.hpp"
#include "State.hpp"
#include "StateManager.hpp"

#include <optional>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <any>

namespace Lua {
namespace impl {
template <typename T>
struct IntegerConverter {
	typedef typename std::enable_if<std::is_integral<T>::value, std::optional<T>>::type Arg;
	static Arg Read(Lua::State& s, int id) {
		if(s.isinteger(id)) {
			int rv        = 0;
			lua_Integer i = s.tointegerx(id, &rv);
			if(!rv)
				return std::nullopt;
			return static_cast<T>(i);
		}
		else if(s.isnumber(id)) {
			int rv       = 0;
			lua_Number i = s.tonumberx(id, &rv);
			if(!rv)
				return std::nullopt;
			return static_cast<T>(i);
		}
		return std::nullopt;
	}
	static std::size_t Push(Lua::State& s, T v) {
		s.pushinteger(static_cast<lua_Integer>(v));
		return 1;
	}
	static std::string Name() { return "integer"; }
};
template <typename T>
struct NumberConverter {
	typedef typename std::enable_if<std::is_floating_point<T>::value, std::optional<T>>::type Arg;
	static Arg Read(Lua::State& s, int id) {
		if(s.isnumber(id)) {
			int rv       = 0;
			lua_Number i = s.tonumberx(id, &rv);
			if(!rv)
				return std::nullopt;
			return static_cast<T>(i);
		}
		else if(s.isinteger(id)) {
			int rv        = 0;
			lua_Integer i = s.tointegerx(id, &rv);
			if(!rv)
				return std::nullopt;
			return static_cast<T>(i);
		}
		return std::nullopt;
	}
	static std::size_t Push(Lua::State& s, T v) {
		s.pushnumber(static_cast<lua_Number>(v));
		return 1;
	}
	static std::string Name() { return "number"; }
};
}

// For classes with a metatype
template <typename T>
struct TypeConverter {
	typedef Lua::impl::MetatableDescriptorImpl<typename std::remove_cv<typename std::remove_pointer<T>::type>::type> metatable;
	typedef std::optional<T> Arg;

	static Arg Read(Lua::State& s, int id) {
		T p = reinterpret_cast<T>(s.checkudata(id, metatable::name()));
		if(!p)
			return std::nullopt;

		return p;
	}
	static std::string Name() { return metatable::name(); }
	static std::size_t Push(Lua::State& s, T&& v) {
		if(v)
			s.luapp_move_object<T>(std::move(v));
		else
			s.pushnil();
		return 1;
	}
};

// void
template <>
struct TypeConverter<void> {};

class ManualReturnValues {
	std::size_t m_values;

public:
	explicit inline ManualReturnValues(std::size_t values)
		: m_values(values) {}
	inline std::size_t count() const { return m_values; }
};

template <>
struct TypeConverter<ManualReturnValues> {
	static std::size_t Push(Lua::State&, ManualReturnValues const& mrv) { return mrv.count(); }
};

template <>
struct TypeConverter<ReferenceType> {
	typedef ReferenceType Arg;
	static Arg Read(Lua::State& s, int id) { return s.luapp_read_reference(id); }
	static std::size_t Push(Lua::State& s, Arg const& reference) {
		s.luapp_push_reference(reference);
		return 1;
	}
	static std::string Name() { return "reference"; }
};

template <Lua::Type type>
struct TypeCheckedReference {
	ReferenceType reference;
};

template <Lua::Type type>
struct TypeConverter<TypeCheckedReference<type>> {
	typedef TypeCheckedReference<type> Arg;
	static Arg Read(Lua::State& s, int id) {
		if(s.type(id) != type)
			return Arg();
		Arg rv;
		rv.reference = TypeConverter<ReferenceType>::Read(s, id);
		return rv;
	}
	static std::size_t Push(Lua::State& s, Arg const& reference) { return TypeConverter<ReferenceType>::Push(s, reference.reference); }
	static std::string Name() {
		std::string strType;
		switch(type) {
		case TP_NONE:
			strType = "typeless";
			break;
		case TP_NIL:
			strType = "nil";
			break;
		case TP_BOOL:
			strType = "bool";
			break;
		case TP_LIGHTUSERDATA:
			strType = "lightuserdata";
			break;
		case TP_NUMBER:
			strType = "number";
			break;
		case TP_STRING:
			strType = "string";
			break;
		case TP_TABLE:
			strType = "table";
			break;
		case TP_FUNCTION:
			strType = "function";
			break;
		case TP_USERDATA:
			strType = "userdata";
			break;
		case TP_THREAD:
			strType = "thread";
			break;
		default:
			strType = "unknown";
			break;
		}

		return strType + " reference";
	}
};

// std::string
template <>
struct TypeConverter<std::string> {
	typedef std::optional<std::string> Arg;
	static Arg Read(Lua::State& s, int id) {
		if(!s.isstring(id)) {
			if(s.isnumber(id))
				return std::to_string(s.tonumber(id));
			else if(s.isinteger(id))
				return std::to_string(s.tointeger(id));
			else if(s.isboolean(id))
				return s.toboolean(id) ? "true" : "false";
			return std::nullopt;
		}

		size_t size     = 0;
		char const* str = s.tolstring(id, &size);
		if(!str)
			return std::nullopt;
		return std::string(str, size);
	}
	static std::size_t Push(Lua::State& s, std::string const& v) {
		s.pushlstring(v.c_str(), v.size());
		return 1;
	}
	static std::string Name() { return "string"; }
};

template <>
struct TypeConverter<bool> {
	typedef std::optional<bool> Arg;
	static Arg Read(Lua::State& s, int id) { return s.toboolean(id) != 0; }
	static std::size_t Push(Lua::State& s, bool v) {
		s.pushboolean(v);
		return 1;
	}
	static std::string Name() { return "boolean"; }
};

// clang-format off
template <> struct TypeConverter<unsigned short> : public impl::IntegerConverter<unsigned short>{};
template <> struct TypeConverter<unsigned int> : public impl::IntegerConverter<unsigned int>{};
template <> struct TypeConverter<unsigned long> : public impl::IntegerConverter<unsigned long>{};
template <> struct TypeConverter<unsigned long long> : public impl::IntegerConverter<unsigned long long>{};
template <> struct TypeConverter<short> : public impl::IntegerConverter<short>{};
template <> struct TypeConverter<int> : public impl::IntegerConverter<int>{};
template <> struct TypeConverter<long> : public impl::IntegerConverter<long>{};
template <> struct TypeConverter<long long> : public impl::IntegerConverter<long long>{};

template <> struct TypeConverter<float> : public impl::NumberConverter<float>{};
template <> struct TypeConverter<double> : public impl::NumberConverter<double>{};
template <> struct TypeConverter<long double> : public impl::NumberConverter<long double>{};
// clang-format on

template <typename T>
struct TypeConverter<std::vector<T>> {
	typedef std::optional<std::vector<T>> Arg;
	static Arg Read(Lua::State& s, int id) {
		if(!s.istable(id))
			return std::nullopt;

		lua_Unsigned itemCount = s.rawlen(id);

		std::vector<T> v;
		v.reserve(itemCount);

		for(lua_Unsigned i = 0; i < itemCount; ++i) {
			s.rawgeti(id, i + 1);
			std::optional<T> argt = TypeConverter<T>::Read(s, s.absindex(-1));
			if(!argt) {
				s.pop(1);
				v.clear();
				break;
			}
			v.emplace_back(std::move(*argt));
			s.pop(1);
		}
		return std::move(v);
	}
	static std::size_t Push(lua_State* s, std::vector<T> const& v) {
		int const top = s.gettop();

		s.createtable(v.size(), 0);
		for(std::size_t i = 0; i < v.size(); ++i) {
			size_t const pushedValues = TypeConverter<T>::Push(s, v[i]);

			if(!pushedValues)
				continue;
			else if(pushedValues > 1) {
				s.settop(top);
				throw lua_exception("Lua::Array Push: An argument is taking multiple spots in the stack.");
			}

			s.rawseti(-2, i + 1);
		}
		return 1;
	}
	static std::string Name() { return TypeConverter<T>::Name() + " vector"; }
};

template <typename T>
struct TypeConverter<std::deque<T>> {
	typedef std::optional<std::deque<T>> Arg;
	static Arg Read(Lua::State& s, int id) {
		if(!s.istable(id))
			return std::nullopt;

		lua_Unsigned itemCount = s.rawlen(id);

		std::deque<T> v;
		v.reserve(itemCount);

		for(lua_Unsigned i = 0; i < itemCount; ++i) {
			s.rawgeti(id, i + 1);
			std::optional<T> argt = TypeConverter<T>::Read(s, s.absindex(-1));
			if(!argt) {
				s.pop(1);
				v.clear();
				break;
			}
			v.emplace_back(std::move(*argt));
			s.pop(1);
		}
		return std::move(v);
	}
	static std::size_t Push(Lua::State& s, std::deque<T> const& v) {
		int const top = s.gettop();

		s.createtable(v.size(), 0);
		for(std::size_t i = 0; i < v.size(); ++i) {
			size_t const pushedValues = TypeConverter<T>::Push(s, v[i]);

			if(!pushedValues)
				continue;
			else if(pushedValues > 1) {
				s.settop(top);
				throw lua_exception("Lua::Array Push: An argument is taking multiple spots in the stack.");
			}

			s.rawseti(-2, i + 1);
		}
		return 1;
	}
	static std::string Name() { return TypeConverter<T>::Name() + " deque"; }
};

template <typename T>
struct TypeConverter<std::list<T>> {
	typedef std::optional<std::list<T>> Arg;
	static Arg Read(Lua::State& s, int id) {
		if(!s.istable(id))
			return std::nullopt;

		lua_Unsigned itemCount = s.rawlen(id);

		std::list<T> v;
		// v.reserve(itemCount);

		for(lua_Unsigned i = 0; i < itemCount; ++i) {
			s.rawgeti(id, i + 1);
			std::optional<T> argt = TypeConverter<T>::Read(s, s.absindex(-1));
			if(!argt) {
				s.pop(1);
				v.clear();
				break;
			}
			v.emplace_back(std::move(*argt));
			s.pop(1);
		}
		return std::move(v);
	}
	static std::size_t Push(Lua::State& s, std::list<T> const& v) {
		int const top = s.gettop();

		s.createtable(v.size(), 0);
		for(std::size_t i = 0; i < v.size(); ++i) {
			size_t const pushedValues = TypeConverter<T>::Push(s, v[i]);

			if(!pushedValues)
				continue;
			else if(pushedValues > 1) {
				s.settop(top);
				throw lua_exception("Lua::Array Push: An argument is taking multiple spots in the stack.");
			}

			s.rawseti(-2, i + 1);
		}
		return 1;
	}
	static std::string Name() { return TypeConverter<T>::Name() + " list"; }
};

template <typename TKey, typename TValue>
struct TypeConverter<std::map<TKey, TValue>> {
	typedef std::optional<std::map<TKey, TValue>> Arg;

	static Arg Read(Lua::State& s, int id) {
		if(!s.istable(id))
			return std::nullopt;

		s.pushvalue(id);
		s.pushnil();

		std::map<TKey, TValue> m;
		while(s.next(-2)) {
			s.pushvalue(-2);

			std::optional<TKey> key = TypeConverter<TKey>::Read(s, s.absindex(-1));
			if(!key)
				continue;
			std::optional<TValue> value = TypeConverter<TValue>::Read(s, s.absindex(-2));
			if(!value)
				continue;
			m.emplace(std::make_pair(std::move(*key), std::move(*value)));
			s.pop(2);
		}
		s.pop(1);

		return std::move(m);
	}
	static size_t Push(Lua::State& s, std::map<TKey, TValue> const& v) {
		s.createtable(0, v.size());

		int const top = s.gettop();
		for(auto it = v.begin(); it != v.end(); ++it) {
			size_t const keyPushedValues = TypeConverter<TKey>::Push(s, it->first);
			if(!keyPushedValues || keyPushedValues > 1) {
				s.settop(top);
				continue;
			}

			size_t const valPushedValues = TypeConverter<TValue>::Push(s, it->second);
			if(!valPushedValues || valPushedValues > 1) {
				s.settop(top);
				continue;
			}

			s.settable(-3);
		}

		return 1;
	}
	static std::string Name() { return TypeConverter<TKey>::Name() + "->" + TypeConverter<TValue>::name() + " table"; }
};

template <>
struct TypeConverter<std::any> {
	typedef std::any Arg;

public:
	static Arg Read(Lua::State& s, int id) {
		switch(s.type(id)) {
		case Lua::TP_BOOL:
			return s.toboolean(id);
		case Lua::TP_NUMBER:
			{
				if(s.isinteger(id))
					return s.tointeger(id);
				else
					return s.tonumber(id);
			}
			return s.tonumber(id);
		case Lua::TP_STRING:
			{
				size_t size     = 0;
				char const* str = s.tolstring(id, &size);
				if(!str)
					return std::nullopt;
				return std::string(str, size);
			}
		case Lua::TP_TABLE:
			{
				s.pushvalue(id);
				s.pushnil();

				std::map<std::string, std::any> m;
				while(s.next(-2)) {
					s.pushvalue(-2);

					std::optional<std::string> key = TypeConverter<std::string>::Read(s, s.absindex(-1));
					if(!key)
						continue;

					std::any value = TypeConverter<std::any>::Read(s, s.absindex(-2));
					m.emplace(std::make_pair(std::move(*key), std::move(value)));
					s.pop(2);
				}
				s.pop(1);

				return std::move(m);
			}
		default:
			return TypeConverter<ReferenceType>::Read(s, id);
		}
	}

	static std::string Name() { return "std::any"; }
};
}

#endif
