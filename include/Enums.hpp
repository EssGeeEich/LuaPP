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

#ifndef LUAPP_ENUMS_HPP
#define LUAPP_ENUMS_HPP

#include "LuaInclude.hpp"

namespace Lua {
	enum CompareOp {
		IS_EQUAL = LUA_OPEQ,
		IS_LESS_THAN = LUA_OPLT,
		IS_LEQUAL = LUA_OPLE
	};
	enum Operator {
		OP_ADD = LUA_OPADD,
		OP_SUB = LUA_OPSUB,
		OP_MUL = LUA_OPMUL,
		OP_MOD = LUA_OPMOD,
		OP_POW = LUA_OPPOW,
		OP_DIV = LUA_OPDIV,
		OP_IDIV=LUA_OPIDIV,
		OP_AND =LUA_OPBAND,
		OP_OR  = LUA_OPBOR,
		OP_XOR =LUA_OPBXOR,
		OP_SHL = LUA_OPSHL,
		OP_SHR = LUA_OPSHR,
		OP_UNM = LUA_OPUNM,
		OP_NOT =LUA_OPBNOT
	};
	enum Type {
		TP_NONE = LUA_TNONE,
		TP_NIL = LUA_TNIL,
		TP_BOOL = LUA_TBOOLEAN,
		TP_LIGHTUSERDATA = LUA_TLIGHTUSERDATA,
		TP_NUMBER = LUA_TNUMBER,
		TP_STRING = LUA_TSTRING,
		TP_TABLE = LUA_TTABLE,
		TP_FUNCTION = LUA_TFUNCTION,
		TP_USERDATA = LUA_TUSERDATA,
		TP_THREAD = LUA_TTHREAD,
		
		TP_COUNT = LUA_NUMTAGS
	};
	enum GcWhat {
		GC_STOP = LUA_GCSTOP,
		GC_RESTART = LUA_GCRESTART,
		GC_COLLECT = LUA_GCCOLLECT,
		GC_COUNT = LUA_GCCOUNT,
		GC_COUNTB = LUA_GCCOUNTB,
		GC_STEP = LUA_GCSTEP,
		GC_SETPAUSE = LUA_GCSETPAUSE,
		GC_SETSTEPMUL = LUA_GCSETSTEPMUL,
		GC_ISRUNNING = LUA_GCISRUNNING
	};
}

#endif
