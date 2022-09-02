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

#ifndef __LUAPP_TRANSFORM_H__
#define __LUAPP_TRANSFORM_H__

#include <functional>
#include <stdexcept>
#include <tuple>
#include "luainclude.h"
#include "util.h"
#include "type_parser.h"

#define LUAPP_TRANSFORM_PARAMETER(firstID, Is) (firstID - ArgCount + 1 + Is - (sizeof...(BoundArgs)))
namespace Lua {
    namespace Tuple {
        template <typename X> struct GetRetVal;
        template <typename Class, typename RetVal, typename ... Args>
        struct GetRetVal<RetVal (Class::*)(Args...)> {
            typedef RetVal value;
        };
        template <typename Class, typename RetVal, typename ... Args>
        struct GetRetVal<RetVal (Class::*)(Args...) const> {
            typedef RetVal value;
        };
        template <typename RetVal, typename ... Args>
        struct GetRetVal<RetVal (*)(Args...)> {
            typedef RetVal value;
        };
        template <typename RetVal, typename ... Args>
        struct GetRetVal<std::function<RetVal(Args...)>> {
            typedef RetVal value;
        };

        template <typename Class, typename Fptr, typename Tuple, bool Done, int Total, int... N>
        struct CallImpl {
            static typename GetRetVal<Fptr>::value call(Class& instance, Fptr fptr, Tuple&& t)
            {
                return CallImpl<Class,Fptr,Tuple,Total == 1 + sizeof...(N), Total, N..., sizeof...(N)>::call(instance,fptr,std::forward<Tuple>(t));
            }
        };

        template <typename Class, typename Fptr, typename Tuple, int Total, int... N>
        struct CallImpl<Class, Fptr, Tuple, true, Total, N...>
        {
            static typename GetRetVal<Fptr>::value call(Class& instance, Fptr fptr, Tuple&& t)
            {
                return (instance.*fptr)(std::get<N>(std::forward<Tuple>(t))...);
            }
        };
        template <typename Class, typename Fptr, typename Tuple, bool Done, int Total, int... N>
        struct ConstCallImpl {
            static typename GetRetVal<Fptr>::value call(Class const& instance, Fptr const fptr, Tuple&& t)
            {
                return ConstCallImpl<Class,Fptr,Tuple,Total == 1 + sizeof...(N), Total, N..., sizeof...(N)>::call(instance,fptr,std::forward<Tuple>(t));
            }
        };

        template <typename Class, typename Fptr, typename Tuple, int Total, int... N>
        struct ConstCallImpl<Class, Fptr, Tuple, true, Total, N...>
        {
            static typename GetRetVal<Fptr>::value call(Class const& instance, Fptr const fptr, Tuple&& t)
            {
                return (instance.*fptr)(std::get<N>(std::forward<Tuple>(t))...);
            }
        };

        template <typename Fptr, typename Tuple, bool Done, int Total, int... N>
        struct FncCallImpl {
            static typename GetRetVal<Fptr>::value call(Fptr fptr, Tuple&& t)
            {
                return FncCallImpl<Fptr, Tuple,Total == 1 + sizeof...(N), Total, N..., sizeof...(N)>::call(fptr,std::forward<Tuple>(t));
            }
        };

        template <typename Fptr, typename Tuple, int Total, int... N>
        struct FncCallImpl<Fptr, Tuple, true, Total, N...>
        {
            static typename GetRetVal<Fptr>::value call(Fptr fptr, Tuple&& t)
            {
                return fptr(std::get<N>(std::forward<Tuple>(t))...);
            }
        };

        template <typename Class, typename RetVal, typename Tuple, typename ... Args>
        RetVal Call(Class& instance, RetVal (Class::* fptr)(Args...), Tuple&& t)
        {
            typedef typename std::decay<Tuple>::type ttype;
            return CallImpl<Class,decltype(fptr),Tuple,0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::call(instance,fptr,std::forward<Tuple>(t));
        }
        template <typename Class, typename RetVal, typename Tuple, typename ... Args>
        RetVal Call(Class const& instance, RetVal (Class::* fptr)(Args...) const, Tuple&& t)
        {
            typedef typename std::decay<Tuple>::type ttype;
            return ConstCallImpl<Class,decltype(fptr),Tuple,0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::call(instance,fptr,std::forward<Tuple>(t));
        }
        template <typename RetVal, typename Tuple, typename ... Args>
        RetVal Call(RetVal (* fptr)(Args...), Tuple&& t)
        {
            typedef typename std::decay<Tuple>::type ttype;
            return FncCallImpl<decltype(fptr),Tuple,0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::call(fptr,std::forward<Tuple>(t));
        }
        template <typename RetVal, typename Tuple, typename ... Args>
        RetVal Call(std::function<RetVal(Args...)> const& fptr, Tuple&& t)
        {
            typedef typename std::decay<Tuple>::type ttype;
            return FncCallImpl<std::function<RetVal(Args...)>,Tuple,0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::call(fptr,std::forward<Tuple>(t));
        }

        template <template <typename> class Transform, typename Tuple> struct Convert;
        template <template <typename> class Transform, typename ... Args>
        struct Convert<Transform, std::tuple<Args...>>
        {
            typedef std::tuple<typename Transform<Args>::type...> type;
        };

        template <size_t count, typename T> struct RemoveHead {
            typedef std::tuple<> type;
        };
        template <size_t N, typename T, typename ... Args>
        struct RemoveHead<N,std::tuple<T,Args...>>
        {
            typedef typename RemoveHead<N-1, std::tuple<Args...>>::type type;
        };
        template <typename T, typename ... Args>
        struct RemoveHead<0, std::tuple<T,Args...>> {
            typedef std::tuple<T,Args...> type;
        };
    }

    namespace impl {
        template <typename T> struct SingleArgReader {
            static T Read(lua_State* state, int currentIndex, int startArg) {
                Lua::Arg<T> arg = Lua::TypeConverter<T>::Read(state,currentIndex);
                if(!arg)
                {
                    std::string errorMsg = "bad argument #";
                    errorMsg += std::to_string(currentIndex - startArg + 1);
                    errorMsg += " to 'CppFunc' (";
                    errorMsg += Lua::TypeConverter<T>::Name();
                    errorMsg += " expected, got ";
                    switch(lua_type(state,currentIndex))
                    {
                    case LUA_TNIL:
                        errorMsg += "nil";
                        break;
                    case LUA_TNUMBER:
                        errorMsg += "number";
                        break;
                    case LUA_TBOOLEAN:
                        errorMsg += "boolean";
                        break;
                    case LUA_TSTRING:
                        errorMsg += "string";
                        break;
                    case LUA_TTABLE:
                        errorMsg += "table";
                        break;
                    case LUA_TFUNCTION:
                        errorMsg += "function";
                        break;
                    case LUA_TUSERDATA:
                        {
                            if(!lua_getmetatable(state,currentIndex))
                            {
                                errorMsg += "userdata";
                            }
                            else
                            {
                                lua_getfield(state,-1,"__name");
                                char const* text = lua_tostring(state,-1);
                                if(text)
                                    errorMsg += text;
                                else
                                    errorMsg += "userdata";
                                lua_pop(state,2);
                            }
                        }
                        break;
                    case LUA_TTHREAD:
                        errorMsg += "thread";
                        break;
                    case LUA_TLIGHTUSERDATA:
                        errorMsg += "luserdata";
                        break;
                    default:
                        errorMsg += "unknown";
                        break;
                    }
                    errorMsg += ")";

                    throw Lua::invalid_lua_arg(state,currentIndex,errorMsg);
                }
                return *arg;
            }
        };
        template <typename T> struct SingleArgReader<Lua::Arg<T>> {
            static Lua::Arg<T> Read(lua_State* state, int currentIndex, int) {
                return Lua::TypeConverter<T>::Read(state,currentIndex);
            }
        };

        template <typename T> struct TupleLuaReader;
        template <> struct TupleLuaReader<std::tuple<>> {};
        template <typename T>
        struct TupleLuaReader<std::tuple<T>>
        {
            static std::tuple<T> Read(lua_State* state, int currentIndex, int startArg)
            {
                return std::make_tuple(SingleArgReader<T>::Read(state,currentIndex,startArg));
            }
        };
        template <typename T, typename Y, typename ... Args>
        struct TupleLuaReader<std::tuple<T,Y,Args...>>
        {
            static std::tuple<T,Y,Args...> Read(lua_State* state, int currentIndex, int startArg)
            {
                return std::tuple_cat(
                    std::make_tuple(SingleArgReader<T>::Read(state,currentIndex,startArg)),
                    TupleLuaReader<std::tuple<Y,Args...>>::Read(state, currentIndex+1,startArg)
                );
            }
        };

        template <typename ... Args>
        void ReadLuaTuple(std::tuple<Args...>& dest, lua_State* state, int currentIndex, int argc, int boundc)
        {
            int startArg = currentIndex - argc + 1 + boundc;
            dest = TupleLuaReader<std::tuple<Args...>>::Read(state,startArg,startArg);
        }
        inline void ReadLuaTuple(std::tuple<>&, lua_State*, int, int, int){}

        template <typename T> struct TransformFunction;


        // Nonconst Member Function
        template <typename ReturnValue, typename Class, typename ... Args>
        struct TransformFunction<ReturnValue (Class::*)(Args...)> {
            typedef typename ReturnDecay<ReturnValue>::type LuaReturnValue;
            enum { ArgCount = sizeof...(Args) };

            template <typename ... BoundArgs>
            static int Call(Class& instance, ReturnValue (Class::* fptr)(Args...), lua_State* state, int firstID, BoundArgs&& ... bound)
            {
                Caller::running_state = state;
                using FullArgsType = std::tuple<typename TupleDecay<Args>::type...>;
                using SecondTupleType = typename Tuple::RemoveHead< sizeof...(BoundArgs), FullArgsType >::type;
                SecondTupleType stt;
                ReadLuaTuple(stt, state, firstID, ArgCount, sizeof...(BoundArgs));
                return Lua::TypeConverter<LuaReturnValue>::Push(state, Tuple::Call(instance, fptr, std::tuple_cat(std::make_tuple(bound...),stt)));
            }
        };
        template <typename Class, typename ... Args>
        struct TransformFunction<void (Class::*)(Args...)> {
            enum { ArgCount = sizeof...(Args) };

            template <typename ... BoundArgs>
            static int Call(Class& instance, void (Class::* fptr)(Args...), lua_State* state, int firstID, BoundArgs&& ... bound)
            {
                Caller::running_state = state;
                using FullArgsType = std::tuple<typename TupleDecay<Args>::type...>;
                using SecondTupleType = typename Tuple::RemoveHead< sizeof...(BoundArgs), FullArgsType >::type;
                SecondTupleType stt;
                ReadLuaTuple(stt, state, firstID, ArgCount, sizeof...(BoundArgs));
                Tuple::Call(instance, fptr, std::tuple_cat(std::make_tuple(std::forward<BoundArgs>(bound)...),stt));
                return 0;
            }
        };


        // Const Member Function
        template <typename ReturnValue, typename Class, typename ... Args>
        struct TransformFunction<ReturnValue (Class::*)(Args...) const> {
            typedef typename ReturnDecay<ReturnValue>::type LuaReturnValue;
            enum { ArgCount = sizeof...(Args) };

            // A temporary is being used...???
            static int Call(Class&& instance, ...);

            template <typename ... BoundArgs>
            static int Call(Class const& instance, ReturnValue (Class::* fptr)(Args...) const, lua_State* state, int firstID, BoundArgs&& ... bound)
            {
                Caller::running_state = state;
                using FullArgsType = std::tuple<typename TupleDecay<Args>::type...>;
                using SecondTupleType = typename Tuple::RemoveHead< sizeof...(BoundArgs), FullArgsType >::type;
                SecondTupleType stt;
                ReadLuaTuple(stt, state, firstID, ArgCount, sizeof...(BoundArgs));
                return Lua::TypeConverter<LuaReturnValue>::Push(state, Tuple::Call(instance, fptr, std::tuple_cat(std::make_tuple(bound...),stt)));
            }
        };
        template <typename Class, typename ... Args>
        struct TransformFunction<void (Class::*)(Args...) const> {
            enum { ArgCount = sizeof...(Args) };

            // A temporary is being used...???
            static int Call(Class&& instance, ...);

            template <typename ... BoundArgs>
            static int Call(Class const& instance, void (Class::* fptr)(Args...) const, lua_State* state, int firstID, BoundArgs&& ... bound)
            {
                Caller::running_state = state;
                using FullArgsType = std::tuple<typename TupleDecay<Args>::type...>;
                using SecondTupleType = typename Tuple::RemoveHead< sizeof...(BoundArgs), FullArgsType >::type;
                SecondTupleType stt;
                ReadLuaTuple(stt, state, firstID, ArgCount, sizeof...(BoundArgs));
                Tuple::Call(instance, fptr, std::tuple_cat(std::make_tuple(std::forward<BoundArgs>(bound)...),stt));
                return 0;
            }
        };


        // Generic Function
        template <typename ReturnValue, typename ... Args>
        struct TransformFunction<ReturnValue (*)(Args...)> {
            typedef typename ReturnDecay<ReturnValue>::type LuaReturnValue;
            enum { ArgCount = sizeof...(Args) };

            template <typename ... BoundArgs>
            static int Call(ReturnValue (* fptr)(Args...), lua_State* state, int firstID, BoundArgs&& ... bound)
            {
                Caller::running_state = state;
                using FullArgsType = std::tuple<typename TupleDecay<Args>::type...>;
                using SecondTupleType = typename Tuple::RemoveHead< sizeof...(BoundArgs), FullArgsType >::type;
                SecondTupleType stt;
                ReadLuaTuple(stt, state, firstID, ArgCount, sizeof...(BoundArgs));
                return Lua::TypeConverter<LuaReturnValue>::Push(state, Tuple::Call(fptr, std::tuple_cat(std::make_tuple(bound...),stt)));
            }
        };
        template <typename ... Args>
        struct TransformFunction<void (*)(Args...)> {
            enum { ArgCount = sizeof...(Args) };

            template <typename ... BoundArgs>
            static int Call(void (* fptr)(Args...), lua_State* state, int firstID, BoundArgs&& ... bound)
            {
                Caller::running_state = state;
                using FullArgsType = std::tuple<typename TupleDecay<Args>::type...>;
                using SecondTupleType = typename Tuple::RemoveHead< sizeof...(BoundArgs), FullArgsType >::type;
                SecondTupleType stt;
                ReadLuaTuple(stt, state, firstID, ArgCount, sizeof...(BoundArgs));
                Tuple::Call(fptr, std::tuple_cat(std::make_tuple(std::forward<BoundArgs>(bound)...),stt));
                return 0;
            }
        };

        // Generic std::function
        template <typename ReturnValue, typename ... Args>
        struct TransformFunction< std::function<ReturnValue(Args...)>> {
            typedef typename ReturnDecay<ReturnValue>::type LuaReturnValue;
            enum { ArgCount = sizeof...(Args) };

            template <typename ... BoundArgs>
            static int Call(std::function<ReturnValue(Args...)> const& fptr, lua_State* state, int firstID, BoundArgs&& ... bound)
            {
                Caller::running_state = state;
                using FullArgsType = std::tuple<typename TupleDecay<Args>::type...>;
                using SecondTupleType = typename Tuple::RemoveHead< sizeof...(BoundArgs), FullArgsType >::type;
                SecondTupleType stt;
                ReadLuaTuple(stt, state, firstID, ArgCount, sizeof...(BoundArgs));
                return Lua::TypeConverter<LuaReturnValue>::Push(state, Tuple::Call(fptr, std::tuple_cat(std::forward_as_tuple(bound...),stt)));
            }
        };
        template <typename ... Args>
        struct TransformFunction<std::function<void(Args...)>> {
            enum { ArgCount = sizeof...(Args) };

            template <typename ... BoundArgs>
            static int Call(std::function<void(Args...)> const& fptr, lua_State* state, int firstID, BoundArgs&& ... bound)
            {
                Caller::running_state = state;
                using FullArgsType = std::tuple<typename TupleDecay<Args>::type...>;
                using SecondTupleType = typename Tuple::RemoveHead< sizeof...(BoundArgs), FullArgsType >::type;
                SecondTupleType stt;
                ReadLuaTuple(stt, state, firstID, ArgCount, sizeof...(BoundArgs));
                Tuple::Call(fptr, std::tuple_cat(std::make_tuple(std::forward<BoundArgs>(bound)...),stt));
                return 0;
            }
        };
    }
    
    template <typename T> struct LuaExplicitArguments;
    template <typename Class, typename RetVal, typename ... Args>
    struct LuaExplicitArguments<RetVal (Class::*)(Args...)> {
        static constexpr int count(bool x=false) { // True = this is bound
            return x ? 1 : 2;
        }
    };
    template <typename Class, typename RetVal, typename ... Args>
    struct LuaExplicitArguments<RetVal (Class::*)(Args...) const> {
        static constexpr int count(bool x=false) { // True = this is bound
            return x ? 1 : 2;
        }
    };
    template <typename RetVal, typename ... Args>
    struct LuaExplicitArguments<std::function<RetVal(Args...)>> {
        static constexpr int count(bool =false) { // True = this is like a member function
            return 1;
        }
    };
    template <typename RetVal, typename ... Args>
    struct LuaExplicitArguments<RetVal (*)(Args...)> {
        static constexpr int count() { // Generic function
            return 1;
        }
    };

    template <typename Class, typename RetVal, typename ... Args, typename ... BoundArgs>
    std::function<int(Class&, lua_State*)> static Transform(RetVal (Class::* fptr)(Args...), BoundArgs&& ... bound) {
        return [=](Class& instance, lua_State* state) -> int {
            static_assert(sizeof...(Args) >= sizeof...(BoundArgs), "Lua Function Transform: Too many bound arguments!");

            constexpr size_t required_args = sizeof...(Args) - sizeof...(BoundArgs);
            int lua_args = lua_gettop(state) - LuaExplicitArguments<RetVal (Class::*)(Args...)>::count();
            if(lua_args != static_cast<int>(required_args))
            {
                return luaL_error(state,(std::string("C++ Method: Invalid argument count!\nExpected: ") + std::to_string(required_args) + (", Given: ") + std::to_string(lua_args) + "!").c_str());
            }

            try {
                return impl::TransformFunction<decltype(fptr)>::Call(instance, fptr, state, lua_gettop(state), bound ...);
            } catch(std::exception& e) {
                return luaL_error(state,"C++ Exception Thrown.\n%s",e.what());
            }
        };
    }

    template <typename Class, typename RetVal, typename ... Args, typename ... BoundArgs>
    std::function<int(Class&, lua_State*)> static Transform(RetVal (Class::* fptr)(Args...) const, BoundArgs&& ... bound) {
        return [=](Class& instance, lua_State* state) -> int {
            static_assert(sizeof...(Args) >= sizeof...(BoundArgs), "Lua Function Transform: Too many bound arguments!");

            constexpr size_t required_args = sizeof...(Args) - sizeof...(BoundArgs);
            int lua_args = lua_gettop(state) - LuaExplicitArguments<RetVal (Class::*)(Args...) const>::count();
            if(lua_args != static_cast<int>(required_args))
            {
                return luaL_error(state,(std::string("C++ Method: Invalid argument count!\nExpected: ") + std::to_string(required_args) + (", Given: ") + std::to_string(lua_args) + "!").c_str());
            }

            try {
                return impl::TransformFunction<decltype(fptr)>::Call(instance, fptr, state, lua_gettop(state), bound...);
            } catch(std::exception& e) {
                return luaL_error(state,"C++ Exception Thrown.\n%s",e.what());
            }
        };
    }

    template <typename RetVal, typename ... Args, typename ... BoundArgs>
    std::function<int(lua_State*)> static Transform(RetVal (* fptr)(Args...), BoundArgs&& ... bound) {
        return [=](lua_State* state) -> int {
            static_assert(sizeof...(Args) >= sizeof...(BoundArgs), "Lua Function Transform: Too many bound arguments!");

            constexpr size_t required_args = sizeof...(Args) - sizeof...(BoundArgs);
            int lua_args = lua_gettop(state) - LuaExplicitArguments<RetVal (*)(Args...)>::count();
            if(lua_args != static_cast<int>(required_args))
            {
                return luaL_error(state,(std::string("C++ Method: Invalid argument count!\nExpected: ") + std::to_string(required_args) + (", Given: ") + std::to_string(lua_args) + "!").c_str());
            }

            try {
                return impl::TransformFunction<decltype(fptr)>::Call(fptr, state, lua_gettop(state), bound...);
            } catch(std::exception& e) {
                return luaL_error(state,"C++ Exception Thrown.\n%s",e.what());
            }
        };
    }

    template <typename Class, typename RetVal, typename ... Args, typename ... BoundArgs>
    std::function<int(lua_State*)> static Transform(Class& instance, RetVal (Class::* fptr)(Args...), BoundArgs&& ... bound) {
        return [=, &instance](lua_State* state) -> int {
            static_assert(sizeof...(Args) >= sizeof...(BoundArgs), "Lua Function Transform: Too many bound arguments!");

            constexpr size_t required_args = sizeof...(Args) - sizeof...(BoundArgs);
            int lua_args = lua_gettop(state) - LuaExplicitArguments<RetVal (Class::*)(Args...)>::count(true);
            if(lua_args != static_cast<int>(required_args))
            {
                return luaL_error(state,(std::string("C++ Method: Invalid argument count!\nExpected: ") + std::to_string(required_args) + (", Given: ") + std::to_string(lua_args) + "!").c_str());
            }

            try {
                return impl::TransformFunction<decltype(fptr)>::Call(instance, fptr, state, lua_gettop(state), bound...);
            } catch(std::exception& e) {
                return luaL_error(state,"C++ Exception Thrown.\n%s",e.what());
            }
        };
    }

    template <typename Class, typename RetVal, typename ... Args, typename ... BoundArgs>
    std::function<int(lua_State*)> static Transform(Class const& instance, RetVal (Class::* fptr)(Args...) const, BoundArgs&& ... bound) {
        return [=, &instance](lua_State* state) -> int {
            static_assert(sizeof...(Args) >= sizeof...(BoundArgs), "Lua Function Transform: Too many bound arguments!");

            constexpr size_t required_args = sizeof...(Args) - sizeof...(BoundArgs);
            int lua_args = lua_gettop(state) - LuaExplicitArguments<RetVal (Class::*)(Args...) const>::count(true);
            if(lua_args != static_cast<int>(required_args))
            {
                return luaL_error(state,(std::string("C++ Method: Invalid argument count!\nExpected: ") + std::to_string(required_args) + (", Given: ") + std::to_string(lua_args) + "!").c_str());
            }

            try {
                return impl::TransformFunction<decltype(fptr)>::Call(instance, fptr, state, lua_gettop(state), bound...);
            } catch(std::exception& e) {
                return luaL_error(state,"C++ Exception Thrown.\n%s",e.what());
            }
        };
    }

    template <typename RetVal, typename ... Args, typename ... BoundArgs>
    std::function<int(lua_State*)> static Transform(std::function<RetVal(Args...)> const& func, BoundArgs&& ... bound) {
        return [=](lua_State* state) -> int {
            static_assert(sizeof...(Args) >= sizeof...(BoundArgs), "Lua Function Transform: Too many bound arguments!");

            constexpr size_t required_args = sizeof...(Args) - sizeof...(BoundArgs);
            int lua_args = lua_gettop(state) - LuaExplicitArguments<std::function<RetVal(Args...)>>::count();
            if(lua_args != static_cast<int>(required_args))
            {
                return luaL_error(state,(std::string("C++ Method: Invalid argument count!\nExpected: ") + std::to_string(required_args) + (", Given: ") + std::to_string(lua_args) + "!").c_str());
            }

            try {
                return impl::TransformFunction<std::function<RetVal(Args...)>>::Call(func, state, lua_gettop(state), bound...);
            } catch(std::exception& e) {
                return luaL_error(state,"C++ Exception Thrown.\n%s",e.what());
            }
        };
    }

    template <typename Class, typename RetVal, typename ... Args, typename ... BoundArgs>
    std::function<int(Class&,lua_State*)> static TransformSelf(std::function<RetVal(Class&,Args...)> const& func, BoundArgs&& ... bound) {
        return [=](Class& instance, lua_State* state) -> int {
            static_assert(sizeof...(Args) >= sizeof...(BoundArgs), "Lua Function Transform: Too many bound arguments!");

            constexpr size_t required_args = sizeof...(Args) - sizeof...(BoundArgs);
            int lua_args = lua_gettop(state) - LuaExplicitArguments<std::function<RetVal(Class&,Args...)>>::count(true);
            if(lua_args != static_cast<int>(required_args))
            {
                return luaL_error(state,(std::string("C++ Method: Invalid argument count!\nExpected: ") + std::to_string(required_args) + (", Given: ") + std::to_string(lua_args) + "!").c_str());
            }

            try {
                return impl::TransformFunction<std::function<RetVal(Class&,Args...)>>::Call(func, state, lua_gettop(state), instance, bound...);
            } catch(std::exception& e) {
                return luaL_error(state,"C++ Exception Thrown.\n%s",e.what());
            }
        };
    }

    template <typename Class, typename RetVal, typename ... Args, typename ... BoundArgs>
    std::function<int(Class const&,lua_State*)> static TransformSelf(std::function<RetVal(Class const&,Args...)> const& func, BoundArgs&& ... bound) {
        return [=](Class const& instance, lua_State* state) -> int {
            static_assert(sizeof...(Args) >= sizeof...(BoundArgs), "Lua Function Transform: Too many bound arguments!");

            constexpr size_t required_args = sizeof...(Args) - sizeof...(BoundArgs);
            int lua_args = lua_gettop(state) - LuaExplicitArguments<std::function<RetVal(Class const&,Args...)>>::count();
            if(lua_args != static_cast<int>(required_args))
            {
                return luaL_error(state,(std::string("C++ Method: Invalid argument count!\nExpected: ") + std::to_string(required_args) + (", Given: ") + std::to_string(lua_args) + "!").c_str());
            }

            try {
                return impl::TransformFunction<std::function<RetVal(Class const&,Args...)>>::Call(func, state, lua_gettop(state), instance, bound...);
            } catch(std::exception& e) {
                return luaL_error(state,"C++ Exception Thrown.\n%s",e.what());
            }
        };
    }
    
    template <typename Class, typename RetVal, typename ... Args, typename ... BoundArgs>
    std::function<int(Class&, lua_State*)> static TransformPlain(std::function<int(Class&, lua_State*)> const& fnc) {
        return [=](Class& instance, lua_State* state) -> int {
            try {
                return fnc(instance, state);
            } catch(std::exception& e) {
                return luaL_error(state,"C++ Exception Thrown.\n%s",e.what());
            }
        };
    }
}


#undef LUAPP_TRANSFORM_PARAMETER
#endif // __LUAPP_TRANSFORM_H__
