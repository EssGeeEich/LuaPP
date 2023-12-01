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

#ifndef LUAPP_UTILS_HPP
#define LUAPP_UTILS_HPP

#include "LuaInclude.hpp"
#include <exception>
#include <string>

namespace Lua {
    class lua_exception : public std::exception {
        std::string m_what;
    protected:
        void setText(std::string what);
    public:
        lua_exception(std::string what);
        char const* what() const noexcept override;
    };

    class invalid_lua_arg : public lua_exception {
        int m_which;
    public:
        invalid_lua_arg(lua_State* state, int id, std::string what = std::string());
        int which() const;
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
    
    template <typename T> struct TupleDecay {
        typedef T type;
    };
    template <typename T> struct TupleDecay<T*> {
        typedef T* type;
    };
    template <typename T> struct TupleDecay<T const&> {
        typedef T type;
    };
    template <typename T> struct TupleDecay<T&> {
        typedef T& type;
    };
    template <typename T> struct TupleDecay<T&&> {
        typedef T type;
    };
	
	namespace impl {
		template <typename...> struct VerifyVarArgs;
		template <> struct VerifyVarArgs<> {
			static constexpr void test() {}
		};
		template <> struct VerifyVarArgs<lua_Integer> {
			static constexpr void test() {}
		};
		template <> struct VerifyVarArgs<lua_Number> {
			static constexpr void test() {}
		};
		template <> struct VerifyVarArgs<int> {
			static constexpr void test() {}
		};
		template <> struct VerifyVarArgs<char> {
			static constexpr void test() {}
		};
		template <> struct VerifyVarArgs<char*> {
			static constexpr void test() {}
		};
		template <> struct VerifyVarArgs<char const*> {
			static constexpr void test() {}
		};
		template <size_t N> struct VerifyVarArgs<char(&)[N]> {
			static constexpr void test() {}
		};
		template <size_t N> struct VerifyVarArgs<char const(&)[N]> {
			static constexpr void test() {}
		};
		template <typename T, typename U, typename ... Args> struct VerifyVarArgs<T, U, Args...> {
			static constexpr void test() {
				VerifyVarArgs<T>::test();
				VerifyVarArgs<U, Args...>::test();
			}
		};
	}
}
#endif
