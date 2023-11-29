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

#ifndef LUAPP_FWDDECL_HPP
#define LUAPP_FWDDECL_HPP
#include "Enums.hpp"

template <typename T> struct MetatableDescriptor;

namespace Lua {
    class LuaFunctor;
    class Reference;
    class AutoState;
    class State;
	class StateManager;
    template <typename> struct TypeConverter;
    class ReturnValues;
    template <typename> class Arg;
    template <typename> struct Map;
    template <typename> struct Array;
}
#endif
