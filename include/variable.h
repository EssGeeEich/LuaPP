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
**	File created on: 14/11/2015
*/

#ifndef __LUAPP_VARIABLE_H__
#define __LUAPP_VARIABLE_H__
#include <lua.hpp>
#include <string>
#include <map>
#include <memory>
#include "enums.h"

namespace Lua {
	struct nil_t {};
	extern nil_t const nil;
	
	class Bool;
	class LightUserData;
	class Number;
	class String;
	class Table;
	class CFunction;
	class UserData;
	class Thread;
	class Nil;
	
	class Variable {
		Type m_type;
		
		Variable& operator= (Variable const&) =delete;
		Variable(Variable const&) =delete;
	protected:
		Variable(Type);
	public:
		virtual ~Variable();
		bool operator == (nil_t const&) const;
		bool operator != (nil_t const&) const;
		bool operator == (Variable const&) const;
		bool operator != (Variable const&) const;
		explicit operator bool() const noexcept;
		
		Type GetType() const noexcept;
		bool IsNil() const noexcept;
		bool IsThread() const noexcept;
		
		virtual bool IsBool() const;
		virtual bool IsBool(Bool* &);
		virtual bool IsBool(Bool const* &) const;
		
		virtual bool IsLightUserData() const;
		virtual bool IsLightUserData(LightUserData* &);
		virtual bool IsLightUserData(LightUserData const* &) const;
		
		virtual bool IsNumber() const;
		virtual bool IsNumber(Number* &);
		virtual bool IsNumber(Number const* &) const;
		
		virtual bool IsString() const;
		virtual bool IsString(String* &);
		virtual bool IsString(String const* &) const;
		
		virtual bool IsTable() const;
		virtual bool IsTable(Table* &);
		virtual bool IsTable(Table const* &) const;
		
		virtual bool IsCFunction() const;
		virtual bool IsCFunction(CFunction* &);
		virtual bool IsCFunction(CFunction const* &) const;
		
		virtual bool IsUserData() const;
		virtual bool IsUserData(UserData* &);
		virtual bool IsUserData(UserData const* &) const;
	};
	
	template <typename T, Type type>
	class BasicVariable : public Variable {
		T m_value;
	public:
		typedef T value_type;
		
		explicit BasicVariable(value_type const& value = value_type(0))
			: Variable(type), m_value(value) {}
		
		BasicVariable(BasicVariable const& o)
			: Variable(type), m_value(o.get()) {}
		
		BasicVariable& operator= (value_type const& o) {
			m_value = o;
			return *this;
		}
		BasicVariable& operator= (BasicVariable const& o) {
			m_value = o.m_value;
			return *this;
		}
		bool operator< (BasicVariable const& o) const { return m_value < o.m_value; }
		bool operator<=(BasicVariable const& o) const { return m_value <=o.m_value; }
		bool operator> (BasicVariable const& o) const { return m_value > o.m_value; }
		bool operator>=(BasicVariable const& o) const { return m_value >=o.m_value; }
		value_type get() const { return m_value; }
		value_type cget() const { return m_value; }
		value_type& get() { return m_value; }
	};
	
	class Nil : public Variable {
	public:
		Nil();
	};
	class Bool : public BasicVariable<bool,TP_BOOL> {
	public:
		using BasicVariable<bool,TP_BOOL>::BasicVariable;
		virtual bool IsBool() const override;
		virtual bool IsBool(Bool* &) override;
		virtual bool IsBool(Bool const* &) const override;
	};
	class LightUserData : public BasicVariable<void*,TP_LIGHTUSERDATA> {
	public:
		using BasicVariable<void*,TP_LIGHTUSERDATA>::BasicVariable;
		virtual bool IsLightUserData() const override;
		virtual bool IsLightUserData(LightUserData* &) override;
		virtual bool IsLightUserData(LightUserData const* &) const override;
	};
	class Number : public BasicVariable<lua_Number,TP_NUMBER> {
	public:
		using BasicVariable<lua_Number,TP_NUMBER>::BasicVariable;
		virtual bool IsNumber() const override;
		virtual bool IsNumber(Number* &) override;
		virtual bool IsNumber(Number const* &) const override;
	};
	class String : public BasicVariable<std::string,TP_STRING> {
	public:
		using BasicVariable<std::string,TP_STRING>::BasicVariable;
		virtual bool IsString() const override;
		virtual bool IsString(String* &) override;
		virtual bool IsString(String const* &) const override;
	};
	class Table : public BasicVariable<std::map<String,std::shared_ptr<Variable>>, TP_TABLE> {
	public:
		using BasicVariable<std::map<String,std::shared_ptr<Variable>>, TP_TABLE>::BasicVariable;
		virtual bool IsTable() const override;
		virtual bool IsTable(Table* &) override;
		virtual bool IsTable(Table const* &) const override;
	};
	class CFunction : public BasicVariable<lua_CFunction,TP_FUNCTION> {
	public:
		using BasicVariable<lua_CFunction,TP_FUNCTION>::BasicVariable;
		virtual bool IsCFunction() const override;
		virtual bool IsCFunction(CFunction* &) override;
		virtual bool IsCFunction(CFunction const* &) const override;
	};
	class UserData : public BasicVariable<void*,TP_USERDATA> {
	public:
		using BasicVariable<void*,TP_USERDATA>::BasicVariable;
		virtual bool IsUserData() const override;
		virtual bool IsUserData(UserData* &) override;
		virtual bool IsUserData(UserData const* &) const override;
	};
}
#endif // __LUAPP_VARIABLE_H__
