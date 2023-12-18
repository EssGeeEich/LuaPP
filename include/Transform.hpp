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

#ifndef LUAPP_TRANSFORM_HPP
#define LUAPP_TRANSFORM_HPP

#include "LuaInclude.hpp"
#include "Utils.hpp"
#include "TypeConverter.hpp"

#include <functional>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <optional>

namespace Lua {
	namespace impl {
		template <typename T>
		void AssignOrSwap(T& oldReference, T&& newRReference) {
			if constexpr(std::is_trivially_move_assignable<T>::value)
				oldReference = std::move(newRReference);
			else if constexpr(std::is_trivially_assignable<T, T>::value)
				oldReference = newRReference;
			else if constexpr(std::is_swappable<T>::value) {
				using std::swap;
				swap(oldReference, newRReference);
			}
			else if constexpr(std::is_move_assignable<T>::value)
				oldReference = std::move(newRReference);
			else // if constexpr(std::is_copy_assignable<T>::value)
				oldReference = newRReference;
		}
		template <typename T> struct LuaArgumentReader {
			static std::size_t Read(Lua::State& state, int luaIndex, T& destination) {
				std::optional<T> retVal = TypeConverter<T>::Read(state, luaIndex);
				
				if(!retVal) {
					throw lua_exception(
						std::string("Error: Function argument #") + std::to_string(luaIndex)
							+ " should be of type " + TypeConverter<typename std::remove_cv<typename std::remove_pointer<T>::type>::type>::Name() + ", got "
							+ state.typename_aux(luaIndex) + " instead.");
				}
				
				AssignOrSwap(destination, std::move(*retVal));
				return 1;
			}
		};
		template <typename T> struct LuaArgumentReader<std::optional<T>> {
			static std::size_t Read(Lua::State& state, int luaIndex, std::optional<T>& destination) {
				std::optional<T> retVal = TypeConverter<T>::Read(state, luaIndex);
				
				using std::swap;
				swap(destination, retVal);
				
				return 1;
			}
		};
		template <> struct LuaArgumentReader<Lua::State*> {
			static std::size_t Read(Lua::State& state, int, Lua::State*& destination) {
				destination = &state;
				return 0;
			}
		};
		
		template <std::size_t N, typename T> struct TupleArgumentReader;
		template <typename ... Args> struct TupleArgumentReader<0, std::tuple<Args...>> {
			static std::size_t Read(Lua::State&, int lastIndex, std::tuple<Args...>&) { return lastIndex; }
		};
		template <std::size_t N, typename ... Args> struct TupleArgumentReader<N, std::tuple<Args...>> {
			static std::size_t Read(Lua::State& state, int luaIndex, std::tuple<Args...>& args) {
				std::size_t argsRead = LuaArgumentReader<typename std::decay<decltype(std::get<sizeof...(Args)-N>(args))>::type>::Read(state, luaIndex, std::get<sizeof...(Args)-N>(args));
				return TupleArgumentReader<N-1, std::tuple<Args...>>::Read(state, luaIndex + static_cast<int>(argsRead), args);
			}
		};
		
		template <typename TFncRetVal, typename ... TFncArgs>
		std::function<int(Lua::State&)> Transform(std::function<TFncRetVal(TFncArgs...)> function) {
			return [function](Lua::State& state) -> int {
				try {
					typedef std::tuple<typename std::decay<TFncArgs>::type...> functionArguments;
					functionArguments arguments;
					
					if constexpr(!std::is_same<functionArguments, std::tuple<>>::value) {
						int const argPadding = 2;
						
						std::size_t const argsRead = TupleArgumentReader<sizeof...(TFncArgs), functionArguments>::Read(state, argPadding, arguments) - 3;
						int const argsGiven = state.gettop() - 2;
						if(argsGiven > static_cast<int>(argsRead))
							throw lua_exception(std::string("Too many arguments provided. Read: ") + std::to_string(argsRead) + ", Given: " + std::to_string(argsGiven));
					}
					
					if constexpr(std::is_void<TFncRetVal>::value) {
						std::apply(function, arguments);
						return 0;
					}
					else {
						return static_cast<int>(TypeConverter<TFncRetVal>::Push(state, std::apply(function, arguments)));
					}
				} catch(std::exception& e) {
					return state.error("C++ Exception Thrown.\n%s", e.what());
				}
			};
		}
		
		template <typename T> struct DeductFunction;
		template <typename RT, typename ...Args> struct DeductFunction<RT(Args...)> { using type = std::function<RT(Args...)>; };
		template <typename RT, typename ...Args> struct DeductFunction<RT(*)(Args...)> { using type = std::function<RT(Args...)>; };
		template <typename C, typename RT, typename ...Args> struct DeductFunction<RT (C::*)(Args...) const> { using type = std::function<RT(C const*, Args...)>; };
		template <typename C, typename RT, typename ...Args> struct DeductFunction<RT (C::*)(Args...)> { using type = std::function<RT(C*, Args...)>; };
		template <typename T> struct DeductFunction<std::function<T>> { using type = std::function<T>; };
		// template <typename T> struct DeductFunction<T> : public DeductFunction<decltype(&T::operator())> {};
		
		template <typename F>
		auto deductFunction(F f) -> typename DeductFunction<F>::type { return {f}; }
	}
	
	template <typename T>
	inline std::function<int(Lua::State&)> Transform(T fnc) {
		return impl::Transform(impl::deductFunction<T>(fnc));
	}

	template <typename TClass, typename TRetVal, typename ...TArgs>
	inline std::function<int(Lua::State&)> Transform(TRetVal (TClass::*fptr)(TArgs...), TClass* instance) {
		return impl::Transform(std::function<TRetVal(TArgs...)>([fptr,instance](TArgs... args) -> TRetVal {
			return ((*instance).*fptr)(std::forward<TArgs>(args)...);
		}));
	}
}

#endif
