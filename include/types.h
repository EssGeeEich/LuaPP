#ifndef TYPES_H
#define TYPES_H
#include <lua.hpp>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <type_traits>
#include "library.h"
#include "util.h"

namespace Lua {
    template <typename ... Args>
    struct ReturnValues {
        std::tuple<Args...> m_data;
        ReturnValues(Args&& ... args) : m_data(std::forward<Args>(args)...) {}
        ReturnValues(ReturnValues const&) =default;
        ReturnValues(ReturnValues&&) =default;
        ReturnValues& operator= (ReturnValues const&) =default;
        ReturnValues& operator= (ReturnValues&&) =default;
    };
    template <typename ... Args>
    ReturnValues<Args...> Return(Args&& ... args) {
        return ReturnValues<Args...>(std::forward<Args>(args)...);
    }

    template <typename T>
    class Arg {
        std::shared_ptr<T> m_data;

    public:
        Arg() =default;
        Arg(Arg const&) =default;
        Arg(Arg&&) =default;
        Arg& operator= (Arg const&) =default;
        Arg& operator= (Arg&&) =default;
        
        // Lua::Nil
        Arg(nil_t const&) {}
        explicit Arg(T const& data)
            : m_data(std::make_shared<T>(data)) {}
        explicit Arg(std::shared_ptr<T> data) : m_data(std::move(data)) {}

        T get_safe(T const& fallback = T()) const {
            if(m_data)
                return *m_data;
            return fallback;
        }

        T* get() {
            return m_data.get();
        }
        T& operator* () const {
            return *(m_data.get());
        }
        T* operator-> () const {
            return m_data.get();
        }
        explicit operator bool() const {
            return m_data.get() != nullptr;
        }
        static Arg<T> ToMove(T&& moved) {
            return Arg<T>(std::make_shared<T>(std::move(moved)));
        }
        static Arg<T> ToCopy(T const& copy) {
            return Arg<T>(std::make_shared<T>(copy));
        }
        static Arg<T> ToReference(T& ref) {
            return Arg<T>(std::shared_ptr<T>(std::shared_ptr<T>(),&ref));
        }
        static Arg<T> Nil() {
            return Arg<T>(std::shared_ptr<T>());
        }
    };
    template <typename T>
    Arg<T> MoveToArg(T&& moved) {
        return Arg<T>::ToMove(std::move(moved));
    }
    template <typename T>
    Arg<T> CopyToArg(T const& copy) {
        return Arg<T>::ToCopy(copy);
    }
    template <typename T>
    Arg<T> RefToArg(T& ref) {
        return Arg<T>::ToReference(ref);
    }
    extern Arg<nil_t> NilArg;

    template <typename T>
    struct Map {
        std::map<std::string,T> m_data;
        
        Map() =default;
        Map(Map const&) =default;
        Map(Map&&) =default;
        Map& operator= (Map const&) =default;
        Map& operator= (Map&&) =default;
    };

    template <typename T> struct Array {
        std::vector<T> m_data;
        
        Array() =default;
        Array(Array const&) =default;
        Array(Array&&) =default;
        Array& operator= (Array const&) =default;
        Array& operator= (Array&&) =default;
    };
}
#endif // TYPES_H
