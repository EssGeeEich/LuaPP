#ifndef FWD_H
#define FWD_H
#include "enums.h"

template <typename T> struct MetatableDescriptor;

namespace Lua {
    class LuaFunctor;
    class Reference;
    class AutoState;
    class State;
    template <typename> struct TypeConverter;
    class ReturnValues;
    template <typename> class Arg;
    template <typename> struct Map;
    template <typename> struct Array;
}
#endif // FWD_H
