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
**	File created on: 15/11/2015
*/

#include "variable.h"

namespace Lua {
	Variable::Variable(Type t) : m_type(t) {}
	Variable::~Variable() {}
	
	template <typename T>
	struct CheckHelper {
		T const* a;
		T const* b;
		explicit operator bool() const noexcept { return a && b; }
		bool comp() const {
			return a->get() == b->get();
		}
	};

	bool Variable::operator == (Variable const& o) const
	{
		if(GetType() != o.GetType())
			return false;
		
		if(GetType() == TP_NIL)
			return true;
		else if(GetType() == TP_BOOL)
		{
			CheckHelper<Bool> v;
			if(IsBool(v.a) && o.IsBool(v.b) && v)
				return v.comp();
		}
		else if(GetType() == TP_LIGHTUSERDATA)
		{
			CheckHelper<LightUserData> v;
			if(IsLightUserData(v.a) && o.IsLightUserData(v.b) && v)
				return v.comp();
		}
		else if(GetType() == TP_NUMBER)
		{
			CheckHelper<Number> v;
			if(IsNumber(v.a) && o.IsNumber(v.b) && v)
				return v.comp();
		}
		else if(GetType() == TP_STRING)
		{
			CheckHelper<String> v;
			if(IsString(v.a) && o.IsString(v.b) && v)
				return v.comp();
		}
		else if(GetType() == TP_TABLE)
		{
			CheckHelper<Table> v;
			if(IsTable(v.a) && o.IsTable(v.b) && v)
				return v.comp();
		}
		else if(GetType() == TP_FUNCTION)
		{
			CheckHelper<CFunction> v;
			if(IsCFunction(v.a) && o.IsCFunction(v.b) && v)
				return v.comp();
		}
		else if(GetType() == TP_USERDATA)
		{
			CheckHelper<UserData> v;
			if(IsUserData(v.a) && o.IsUserData(v.b) && v)
				return v.comp();
		}
		
		// Same type, no comparation
		// Might not work for Threads.
		return true;
	}
	bool Variable::operator == (nil_t const&) const
	{
		return IsNil();
	}
	bool Variable::operator != (Variable const& o) const
	{
		return !(*this == o);
	}
	bool Variable::operator != (nil_t const& o) const
	{
		return !(*this == o);
	}
	Variable::operator bool() const noexcept
	{
		return !IsNil();
	}
	
	Type Variable::GetType() const noexcept { return m_type; }
	bool Variable::IsNil() const noexcept { return m_type == TP_NIL; }
	bool Variable::IsThread() const noexcept { return m_type == TP_THREAD; }
	
	Nil::Nil() : Variable(TP_NIL) {}
#define VAR_TYPE_DEF(Type) \
	bool Variable::Is##Type() const { return false; }\
	bool Variable::Is##Type(Type* &) { return false; }\
	bool Variable::Is##Type(Type const* &) const { return false; }\
	bool Type::Is##Type() const { return true; }\
	bool Type::Is##Type(Type* &o) { o = this; return true; }\
	bool Type::Is##Type(Type const* &o) const { o = this; return true; }
	
	VAR_TYPE_DEF(Bool)
	VAR_TYPE_DEF(LightUserData)
	VAR_TYPE_DEF(Number)
	VAR_TYPE_DEF(String)
	VAR_TYPE_DEF(Table)
	VAR_TYPE_DEF(CFunction)
	VAR_TYPE_DEF(UserData)
}
