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

#ifndef LUAPP_UTILS_H
#define LUAPP_UTILS_H

#include "LuaInclude.hpp"
#include <exception>
#include <string>
#include <cmath>
#include <any>

namespace Lua {
    class lua_exception : public std::exception {
        std::string m_what;
    protected:
		void setText(std::string what);
    public:
		lua_exception(std::string what);
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

		template <typename T> struct GenericAnyCaster;
		template <> struct GenericAnyCaster<lua_Integer> {
			static lua_Integer read(std::any const& val) {
				if(std::int64_t const* v = std::any_cast<std::int64_t>(&val))
					return static_cast<lua_Integer>(*v);
				else if(std::uint64_t const* v = std::any_cast<std::uint64_t>(&val))
					return static_cast<lua_Integer>(*v);
				else if(std::int32_t const* v = std::any_cast<std::int32_t>(&val))
					return static_cast<lua_Integer>(*v);
				else if(std::uint32_t const* v = std::any_cast<std::uint32_t>(&val))
					return static_cast<lua_Integer>(*v);
				else if(std::int16_t const* v = std::any_cast<std::int16_t>(&val))
					return static_cast<lua_Integer>(*v);
				else if(std::uint16_t const* v = std::any_cast<std::uint16_t>(&val))
					return static_cast<lua_Integer>(*v);
				else if(std::int8_t const* v = std::any_cast<std::int8_t>(&val))
					return static_cast<lua_Integer>(*v);
				else if(std::uint8_t const* v = std::any_cast<std::uint8_t>(&val))
					return static_cast<lua_Integer>(*v);
				else if(float const* v = std::any_cast<float>(&val))
					return static_cast<lua_Integer>(std::llroundf(*v));
				else if(double const* v = std::any_cast<double>(&val))
					return static_cast<lua_Integer>(std::llround(*v));
				else if(long double const* v = std::any_cast<long double>(&val))
					return static_cast<lua_Integer>(std::llroundl(*v));
				else if(bool const* v = std::any_cast<bool>(&val))
					return (*v ? 1 : 0);
				return std::any_cast<lua_Integer>(val);
			}
		};
		template <> struct GenericAnyCaster<lua_Number> {
			static lua_Number read(std::any const& val) {
				if(double const* v = std::any_cast<double>(&val))
					return static_cast<lua_Number>(*v);
				else if(float const* v = std::any_cast<float>(&val))
					return static_cast<lua_Number>(*v);
				else if(long double const* v = std::any_cast<long double>(&val))
					return static_cast<lua_Number>(*v);
				else if(std::int64_t const* v = std::any_cast<std::int64_t>(&val))
					return static_cast<lua_Number>(*v);
				else if(std::uint64_t const* v = std::any_cast<std::uint64_t>(&val))
					return static_cast<lua_Number>(*v);
				else if(std::int32_t const* v = std::any_cast<std::int32_t>(&val))
					return static_cast<lua_Number>(*v);
				else if(std::uint32_t const* v = std::any_cast<std::uint32_t>(&val))
					return static_cast<lua_Number>(*v);
				else if(std::int16_t const* v = std::any_cast<std::int16_t>(&val))
					return static_cast<lua_Number>(*v);
				else if(std::uint16_t const* v = std::any_cast<std::uint16_t>(&val))
					return static_cast<lua_Number>(*v);
				else if(std::int8_t const* v = std::any_cast<std::int8_t>(&val))
					return static_cast<lua_Number>(*v);
				else if(std::uint8_t const* v = std::any_cast<std::uint8_t>(&val))
					return static_cast<lua_Number>(*v);
				else if(bool const* v = std::any_cast<bool>(&val))
					return (*v ? 1.0 : 0.0);
				return std::any_cast<lua_Number>(val);
			}
		};
		template <> struct GenericAnyCaster<std::string> {
			static std::string read(std::any const& val) {
				if(std::string const* v = std::any_cast<std::string>(&val))
					return *v;
				else if(std::int64_t const* v = std::any_cast<std::int64_t>(&val))
					return std::to_string(*v);
				else if(std::uint64_t const* v = std::any_cast<std::uint64_t>(&val))
					return std::to_string(*v);
				else if(std::int32_t const* v = std::any_cast<std::int32_t>(&val))
					return std::to_string(*v);
				else if(std::uint32_t const* v = std::any_cast<std::uint32_t>(&val))
					return std::to_string(*v);
				else if(std::int16_t const* v = std::any_cast<std::int16_t>(&val))
					return std::to_string(*v);
				else if(std::uint16_t const* v = std::any_cast<std::uint16_t>(&val))
					return std::to_string(*v);
				else if(std::int8_t const* v = std::any_cast<std::int8_t>(&val))
					return std::to_string(*v);
				else if(std::uint8_t const* v = std::any_cast<std::uint8_t>(&val))
					return std::to_string(*v);
				else if(double const* v = std::any_cast<double>(&val))
					return std::to_string(*v);
				else if(float const* v = std::any_cast<float>(&val))
					return std::to_string(*v);
				else if(long double const* v = std::any_cast<long double>(&val))
					return std::to_string(*v);
				else if(bool const* v = std::any_cast<bool>(&val))
					return (*v ? "true" : "false");
				return std::any_cast<std::string>(val);
			}
		};
		template <> struct GenericAnyCaster<bool> {
			static bool read(std::any const& val) {
				if(bool const* v = std::any_cast<bool>(&val))
					return *v;
				else if(std::string const* v = std::any_cast<std::string>(&val))
					return !v->empty();
				else if(std::int64_t const* v = std::any_cast<std::int64_t>(&val))
					return *v != 0;
				else if(std::uint64_t const* v = std::any_cast<std::uint64_t>(&val))
					return *v != 0;
				else if(std::int32_t const* v = std::any_cast<std::int32_t>(&val))
					return *v != 0;
				else if(std::uint32_t const* v = std::any_cast<std::uint32_t>(&val))
					return *v != 0;
				else if(std::int16_t const* v = std::any_cast<std::int16_t>(&val))
					return *v != 0;
				else if(std::uint16_t const* v = std::any_cast<std::uint16_t>(&val))
					return *v != 0;
				else if(std::int8_t const* v = std::any_cast<std::int8_t>(&val))
					return *v != 0;
				else if(std::uint8_t const* v = std::any_cast<std::uint8_t>(&val))
					return *v != 0;
				else if(double const* v = std::any_cast<double>(&val))
					return *v < -0.000001 || *v > 0.000001;
				else if(float const* v = std::any_cast<float>(&val))
					return *v < -0.000001f || *v > 0.000001f;
				else if(long double const* v = std::any_cast<long double>(&val))
					return *v < -0.000001l || *v > 0.000001l;
				return std::any_cast<bool>(val);
			}
		};
	}

	template <typename T>
	inline T soft_any_cast(std::any& val) { return impl::GenericAnyCaster<T>::read(val); }
}
#endif
