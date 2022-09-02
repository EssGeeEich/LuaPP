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

#ifndef __LUAPP_STATE_H__
#define __LUAPP_STATE_H__
#include <memory>

#include "luainclude.h"
#include "fwd.h"
#include "enums.h"
#include "variable.h"
#include "reference.h"
#include "library.h"

// Documentation tag
#define tagged(pops, pushes, errors)

namespace Lua {
	typedef ::lua_Alloc lua_Alloc;
	typedef ::lua_CFunction lua_CFunction;
	typedef ::lua_Integer lua_Integer;
	typedef ::lua_KContext lua_KContext;
	typedef ::lua_KFunction lua_KFunction;
	typedef ::lua_Number lua_Number;
	typedef ::lua_Reader lua_Reader;
	typedef ::lua_State lua_State;
	typedef ::lua_Writer lua_Writer;
	typedef ::luaL_Reg luaL_Reg;
	
	class AutoState {
		lua_State* m_state;
        bool m_del;
		AutoState(AutoState const&) =delete;
		AutoState& operator= (AutoState const&) =delete;
	public:
		AutoState(lua_State*, bool =false);
		~AutoState();
		lua_State* get() const;
	};
	
	template <typename T> struct CheckVarArgImpl;
	template <> struct CheckVarArgImpl<lua_Integer> {};
	template <> struct CheckVarArgImpl<lua_Number> {};
	template <typename T> struct CheckVarArgImpl<T*> {};
	template <> struct CheckVarArgImpl<int> {};
	template <> struct CheckVarArgImpl<char> {};
	template <typename T, size_t N> struct CheckVarArgImpl<T[N]> {};
	
	inline void CheckVarArg() {}
	template <typename T, typename ... Args> void CheckVarArg(T const&, Args&& ... args) {
		CheckVarArgImpl<T> v; ((void)v);
		CheckVarArg(std::forward<Args>(args)...);
	}
	
	class State {
		std::shared_ptr<AutoState> m_state;
        std::uint64_t m_lastMetatableIndex;
		
	public:
		static State create();
        static State use_existing_state(lua_State*);
		
		State();
		State(State const&);
		State(State&&);
		State& operator= (State const&);
		State& operator= (State&&);
		~State();
		
		explicit operator bool() const noexcept;
		bool operator ! () const noexcept;
		lua_State* GetState() const noexcept;
		
		State& reset();
		State& close();
		
		bool IsValidIndex(int);
		bool IsAcceptableIndex(int);
		
		tagged(0,0,-)					int absindex(int);
		tagged(2|1,1,e)					void arith(Operator);
		tagged(0,0,-)					lua_CFunction atpanic(lua_CFunction);
		tagged(nargs+1,nresults,e)		void call(int =0, int =0, int =0, lua_KFunction =nullptr);
		tagged(0,0,-)					int checkstack(int);
		tagged(0,0,e)					int compare(int,int,CompareOp);
		tagged(n,1,e)					void concat(int);
		tagged(0,0,-)					void copy(int,int);
		tagged(0,1,e)					void createtable(int,int);
		tagged(0,0,e)					int dump(lua_Writer,void*,int);
		tagged(1,0,v)					int error();
		tagged(0,0,e)					int gc(GcWhat,int);
		tagged(0,0,-)					lua_Alloc getallocf(void**);
		tagged(0,1,e)					int getfield(int,char const*);
		tagged(0,1,e)					int getglobal(char const*);
		tagged(0,0|1,-)					int getmetatable(int);
		tagged(1,1,e)					int gettable(int);
		tagged(0,0,-)					int gettop();
		tagged(0,1,e)					int getuservalue(int);
		tagged(1,1,-)					void insert(int);
		tagged(0,0,-)					bool isboolean(int);
		tagged(0,0,-)					bool iscfunction(int);
		tagged(0,0,-)					bool isfunction(int);
		tagged(0,0,-)					bool isinteger(int);
		tagged(0,0,-)					bool islightuserdata(int);
		tagged(0,0,-)					bool isnil(int);
		tagged(0,0,-)					bool isnone(int);
		tagged(0,0,-)					bool isnoneornil(int);
		tagged(0,0,-)					bool isnumber(int);
		tagged(0,0,-)					bool isstring(int);
		tagged(0,0,-)					bool istable(int);
		tagged(0,0,-)					bool isthread(int);
		tagged(0,0,-)					bool isuserdata(int);
		tagged(0,1,e)					void len(int);
		tagged(0,1,-)					int load(lua_Reader,void*,char const*,char const*);
		tagged(0,1,e)					void newtable();
		tagged(0,1,e)					lua_State* newthread();
		tagged(0,1,e)					void* newuserdata(size_t);
		tagged(1,2|0,e)					int next(int);
		tagged(nargs+1,nresults|1,-)	int pcall(int =0,int =0,int =0,int =0, lua_KFunction =nullptr);
		tagged(n,0,-)					void pop(int);
		tagged(0,1,-)					void pushboolean(bool);
		tagged(n,1,e)					void pushcclosure(lua_CFunction, int =0);
		tagged(0,1,-)					void pushcfunction(lua_CFunction);
		tagged(0,1,e)					template <typename ... Args> char const* pushfstring(char const* fmt, Args&& ... args) { CheckVarArg(args...); return lua_pushfstring(m_state,fmt,std::forward<Args>(args)...); }
		tagged(0,1,-)					void pushinteger(lua_Integer);
		tagged(0,1,-)					void pushlightuserdata(void*);
		tagged(0,1,e)					template <size_t len> char const* pushliteral(char const (&p)[len]) { static_assert(len>0,"Invalid char const[] size for State::pushliteral."); return pushlstring(p,len-1); }
		tagged(0,1,e)					char const* pushlstring(char const*, size_t =0);
		tagged(0,1,e)					void pushnil();
		tagged(0,1,e)					void pushnumber(lua_Number);
		tagged(0,1,e)					char const* pushstdstring(std::string const&);
		tagged(0,1,e)					char const* pushstring(char const*);
		tagged(0,1,-)					int pushthread();
		tagged(0,1,-)					void pushvalue(int);
		tagged(0,1,e)					char const* pushvfstring(char const*, va_list);
		tagged(0,0,-)					int rawequal(int,int);
		tagged(1,1,-)					int rawget(int);
		tagged(0,1,-)					int rawgeti(int,lua_Integer);
		tagged(0,1,-)					int rawgetp(int,void const*);
		tagged(0,0,-)					size_t rawlen(int);
		tagged(2,0,e)					void rawset(int);
		tagged(1,0,e)					void rawseti(int,lua_Integer);
		tagged(1,0,e)					void rawsetp(int,void const*);
		tagged(0,0,e)					void register_(char const*, lua_CFunction);
		tagged(1,0,-)					void remove(int);
		tagged(1,0,-)					void replace(int);
		tagged(?,?,-)					int resume(lua_State*,int);
		tagged(0,0,-)					void setallocf(lua_Alloc,void*);
		tagged(1,0,e)					void setfield(int,char const*);
		tagged(1,0,e)					void setglobal(char const*);
		tagged(1,0,-)					int setmetatable(int);
		tagged(2,0,e)					void settable(int);
		tagged(?,?,-)					void settop(int);
		tagged(1,0,-)					void setuservalue(int);
		tagged(0,0,-)					int status();
		tagged(0,0,-)					bool toboolean(int);
		tagged(0,0,-)					lua_CFunction tocfunction(int);
		tagged(0,0,-)					lua_Integer tointeger(int);
		tagged(0,0,-)					lua_Integer tointegerx(int, int*);
		tagged(0,0,e)					char const* tolstring(int, size_t* =nullptr);
		tagged(0,0,e)					std::string tostdstring(int);
		tagged(0,0,-)					lua_Number tonumber(int);
		tagged(0,0,-)					lua_Number tonumberx(int, int*);
		tagged(0,0,-)					void const* topointer(int);
		tagged(0,0,e)					char const* tostring(int);
		tagged(0,0,-)					lua_State* tothread(int);
		tagged(0,0,-)					void* touserdata(int);
		tagged(0,0,-)					Type type(int);
		tagged(0,0,-)					char const* typename_(int);
		tagged(0,0,v)					lua_Number const* version();
		tagged(?,?,-)					void xmove(lua_State*, int);
		tagged(?,?,-)					int yieldk(int, int =0, lua_KFunction =nullptr);
		
		// Aux
		tagged(0,0,v)					int argerror(int, char const*);
		tagged(0,0|1,e)					int callmeta(int, char const*);
		tagged(0,0,v)					void checkany(int);
		tagged(0,0,v)					lua_Integer checkinteger(int);
		tagged(0,0,v)					char const* checklstring(int, size_t* =nullptr);
		tagged(0,0,v)					lua_Number checknumber(int);
		tagged(0,0,v)					int checkoption(int, char const*, char const* const*);
		tagged(0,0,v)					void checkstack(int, char const*);
		tagged(0,0,v)					std::string checkstdstring(int);
		tagged(0,0,v)					char const* checkstring(int);
		tagged(0,0,v)					void checktype(int,Type);
		tagged(0,0,v)					void* checkudata(int, char const*);
		tagged(0,0,v)					void checkversion();
		tagged(0,0,v)					template <typename ... Args> int error(char const* fmt, Args&& ... args) { CheckVarArg(std::forward<Args>(args)...); return luaL_error(m_state,fmt,std::forward<Args>(args)...); }
		tagged(0,0|1,e)					int getmetafield(int, char const*);
		tagged(0,1,-)					int getmetatable(char const*);
		tagged(0,1,e)					int getsubtable(int, char const*);
		tagged(0,1,e)					char const* gsub(char const*, char const*, char const*);
		tagged(0,0,e)					lua_Integer len_aux(int);
		tagged(0,1,-)					int loadbuffer(char const*, size_t, char const*, char const* =nullptr);
		tagged(0,1,e)					int loadfile(char const*, char const* =nullptr);
		tagged(0,1,-)					int loadstring(char const*);
		tagged(0,1,e)					int newmetatable(char const*);
		tagged(0,0,e)					void openlibs();
		tagged(0,0,v)					lua_Integer optinteger(int,lua_Integer);
		tagged(0,0,v)					char const* optlstring(int,char const*, size_t* =nullptr);
		tagged(0,0,v)					lua_Number optnumber(int, lua_Number);
		tagged(0,0,v)					char const* optstring(int, char const*);
		tagged(1,0,e)					int ref(int);
		tagged(0,1,e)					void requiref(char const*, lua_CFunction, int);
		tagged(num,0,e)					void setfuncs(luaL_Reg const*, int);
		tagged(0,0,-)					void setmetatable(char const*);
		tagged(0,0,e)					void* testudata(int, char const*);
		tagged(0,1,e)					char const* tolstring_aux(int, size_t*);
		tagged(0,0,-)					char const* typename_aux(int);
		tagged(0,0,-)					void unref(int, int);
		tagged(0,1,e)					void where(int);

		// Works with:
		/* Bool
		 * LightUserData
		 * Number
		 * String
		 * Table
		 * CFunction
		 */
        tagged(0,1,e)					void luapp_push(Variable const&);
        tagged(0,1,e)					void luapp_push(nil_t const&);
		
		// Works with:
		/* Bool
		 * LightUserData
		 * Number
		 * String
		 * Table
		 * CFunction
		 */
        tagged(0,0,e)					Variable* luapp_read(int);
		
        // Works with references
        tagged(1,0,e)					Reference luapp_pop_reference(int table);
        tagged(0,1,e)					void luapp_push_reference(Reference const&, int table);
        tagged(0,0,-)					void luapp_destroy_reference(Reference const&, int table);
		
        // Required for most users
        tagged(0,0,-)                   void luapp_register_metatables();
        tagged(0,0,-)					template <typename T> void luapp_register_object(bool w_constructor=true) { MetatableManager<MetatableDescriptorImpl<T>>::Register(GetState(),w_constructor); }
        tagged(0,1,-)                   int luapp_push_translated_function(std::function<int(lua_State*)> const& function);
        tagged(0,0,-)            inline void luapp_add_translated_function(char const* name, std::function<int(lua_State*)> const& function) { luapp_push_translated_function(function); setglobal(name); }
        tagged(0,1,-)                   template <typename T, typename ... Args> typename Lua::GenericDecay<T>::type* luapp_push_object(Args&& ... args) { return MetatableManager<MetatableDescriptorImpl<T>>::Construct(GetState(),std::forward<Args>(args)...); }
        tagged(0,0,0)                   template <typename T> T* luapp_get_object(int arg) { return MetatableManager<MetatableDescriptorImpl<T>>::FromStack(GetState(),arg); }
        tagged(0,0,e)                   template <typename T> T& luapp_require_object(int arg) { T* ptr = MetatableManager<MetatableDescriptorImpl<T>>::FromStack(GetState(),arg); if(!ptr) luaL_error(GetState(),"C++ / Lua Error: Stack item %d is not of type %s!",arg,MetatableDescriptorImpl<T>::name()); return *ptr; }
        tagged(0,0,0)                   template <typename T> static T* luapp_get_object(lua_State* s, int arg) { return MetatableManager<MetatableDescriptorImpl<T>>::FromStack(s,arg); }
        tagged(0,0,e)                   template <typename T> static T& luapp_require_object(lua_State* s, int arg) { T* ptr = MetatableManager<MetatableDescriptorImpl<T>>::FromStack(s,arg); if(!ptr) luaL_error(s,"C++ / Lua Error: Stack item %d is not of type %s!",arg,MetatableDescriptorImpl<T>::name()); return *ptr; }
        
        tagged(0,n,-)                   template <typename T> int luapp_push_returnvalue(T const& arg) { return Lua::TypeConverter<typename GenericDecay<T>::type>::Push(GetState(),arg); }
	};
}

#include "transform.h"
#include "type_parser.h"

#endif // __LUAPP_STATE_H__
