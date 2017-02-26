#ifndef UTIL_H
#define UTIL_H
#include <exception>
#include <string>
#include <functional>
#include <lua.hpp>

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

    namespace Caller {
        // Currently, it's only being set in Lua::impl::TransformFunction.
        extern thread_local lua_State* running_state;
    }

    // ToFnc
    namespace impl {
        template <typename> struct ClassFunctor;
        template <typename RV, typename Class, typename ... Args>
        struct ClassFunctor<RV(Class::*)(Args...) const> {
            typedef std::function<RV(Args...)> type;
        };
    }
    template <typename T>
    typename impl::ClassFunctor<decltype(&T::operator())>::type ToFnc(T const& f)
    {
        return f;
    }

    struct nil_t {};
    extern nil_t const nil;
}
#endif // UTIL_H
