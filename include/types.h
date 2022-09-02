#ifndef TYPES_H
#define TYPES_H
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <memory>
#include <type_traits>
#include <cstring>
#include "luainclude.h"
#include "fwd.h"
#include "library.h"
#include "util.h"

namespace Lua {
    namespace impl {
        template <int... Is> struct sequence {};
        template <int n, int ... Is> struct make_sequence : make_sequence<n-1,n-1,Is...> {};
        template <int ... Is> struct make_sequence<0, Is...> : sequence<Is...> {};
    
        template <typename T, typename F, int ... Is>
        void for_each_impl(T&& t, F& f, sequence<Is...>)
        {
            auto l = { (f(std::get<Is>(t)),0)... };
            (void)l;
        }
    
        template <typename ... Ts, typename F>
        void for_each_in_tuple(std::tuple<Ts...>& t, F& f)
        {
            for_each_impl(t, f, make_sequence<sizeof...(Ts)>());
        }
        
        class RvPusher {
            lua_State* m_state;
            int m_count;
        public:
            RvPusher(lua_State* state)
                : m_state(state), m_count(0) {}
            
            int count() const { return m_count; }
            
            template <typename T>
            void operator() (T&& v)
            {
                m_count += TypeConverter<typename GenericDecay<T>::type>::Push(m_state, std::forward<T>(v));
            }
        };
    }
    
    class ReturnValues {
        std::function<int(lua_State*)> m_fn;
        
    public:
        ReturnValues() =default;
        ReturnValues(ReturnValues const&) =default;
        ReturnValues& operator= (ReturnValues const&) =default;
        
        int push(lua_State* state) const {
            if(m_fn)
                return m_fn(state);
            return 0;
        }
        
        template <typename ... Args>
        struct impl {
            typedef decltype(std::make_tuple(std::forward<Args>(std::declval<Args>())...)) tuple_type;
            static int do_push(tuple_type& tpl, lua_State* state) {
                Lua::impl::RvPusher rp(state);
                Lua::impl::for_each_in_tuple(tpl, rp);
                return rp.count();
            }
        };
        
        template <typename ... Args>
        explicit ReturnValues(Args&& ... args)
        {
            m_fn = std::bind(ReturnValues::impl<Args...>::do_push,
                             std::make_tuple(std::forward<Args>(args)...),
                             std::placeholders::_1);
        }
    };
    
    template <typename ... Args>
    ReturnValues Return(Args&& ... args) {
        return ReturnValues(std::forward<Args>(args)...);
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
