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

#ifndef LUAPP_STATE_HPP
#define LUAPP_STATE_HPP
#include <memory>

#include "LuaInclude.hpp"
#include "FwdDecl.hpp"
#include "Enums.hpp"
#include "Reference.hpp"
#include "MetatableManager.hpp"

// Documentation tag
#define tagged(pops, pushes, errors)

namespace Lua {
	class State {
		friend class StateManager;
		lua_State* m_state;
		std::weak_ptr<State> m_self;
		
		State(State const&) =delete;
		State& operator= (State const&) =delete;
		
	protected:
		State();
		
	public:
		State(State&&);
		State& operator= (State&&);
		~State();
		
		void setSelf(std::weak_ptr<State> self);		
		void close();
		
		explicit operator bool() const noexcept;
		bool operator ! () const noexcept;
		lua_State* GetState() const noexcept;
		
		bool IsValidIndex(int);
		bool IsAcceptableIndex(int);
		
		// Adds various extra LuaPP features
		tagged(0,0,-)                   void luapp_register_metatables();
		
        // References. Supported without LuaPP features.
        tagged(1,0,e)					std::shared_ptr<Reference> luapp_pop_reference(int refTable = LUA_REGISTRYINDEX);
		tagged(1,0,e)					std::shared_ptr<Reference> luapp_read_reference(int index, int refTable = LUA_REGISTRYINDEX);
        tagged(0,1,e)					void luapp_push_reference(std::shared_ptr<Reference>);
		tagged(0,0,-)					void luapp_destroy_reference(std::shared_ptr<Reference>);
		tagged(0,0,-)					void luapp_destroy_reference(Reference*);
		
        // Required for most users. Might need luapp_register_metatables.
        tagged(0,0,-)					template <typename T> void luapp_register_object(bool allowConstructor=true) { impl::MetatableManager<T>::Register(GetState(), allowConstructor); }
        tagged(0,1,-)                   int luapp_push_translated_function(std::function<int(Lua::State&)> const& function);
        tagged(0,0,-)            inline void luapp_add_translated_function(char const* name, std::function<int(Lua::State&)> const& function) { luapp_push_translated_function(function); setglobal(name); }
        tagged(0,1,-)                   template <typename T, typename ... Args> typename Lua::GenericDecay<T>::type* luapp_push_object(Args&& ... args) { return impl::MetatableManager<T>::Construct(GetState(),std::forward<Args>(args)...); }
		tagged(0,1,-)					template <typename T> typename Lua::GenericDecay<T>::type* luapp_move_object(T&& arg);
        tagged(0,0,0)                   template <typename T> T* luapp_get_object(int arg) { return impl::MetatableManager<T>::FromStack(GetState(),arg); }
        tagged(0,0,e)                   template <typename T> T& luapp_require_object(int arg) { T* ptr = impl::MetatableManager<T>::FromStack(GetState(),arg); if(!ptr) luaL_error(GetState(),"C++ / Lua Error: Stack item %d is not of type %s!",arg,impl::MetatableDescriptorImpl<T>::name()); return *ptr; }
        tagged(0,0,0)                   template <typename T> static T* luapp_get_object(lua_State* s, int arg) { return impl::MetatableManager<T>::FromStack(s,arg); }
        tagged(0,0,e)                   template <typename T> static T& luapp_require_object(lua_State* s, int arg) { T* ptr = impl::MetatableManager<T>::FromStack(s,arg); if(!ptr) luaL_error(s,"C++ / Lua Error: Stack item %d is not of type %s!",arg,impl::MetatableDescriptorImpl<T>::name()); return *ptr; }
        
        tagged(0,n,-)                   template <typename T> int luapp_push_returnvalue(T const& arg) { return Lua::TypeConverter<typename GenericDecay<T>::type>::Push(GetState(),arg); }
		
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
		tagged(0,1,e)					int getiuservalue(int,int);
		tagged(0|1,0|1|2,m)				int getinfo(char const*, lua_Debug*);
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
		tagged(0,1,e)					void* newuserdatauv(size_t, int);
		tagged(1,2|0,e)					int next(int);
		tagged(nargs+1,nresults|1,-)	int pcall(int =0,int =0,int =0,int =0, lua_KFunction =nullptr);
		tagged(n,0,-)					void pop(int);
		tagged(0,1,-)					void pushboolean(bool);
		tagged(n,1,e)					void pushcclosure(lua_CFunction, int =0);
		tagged(0,1,-)					void pushcfunction(lua_CFunction);
		tagged(0,1,e)					template <typename ... Args> char const* pushfstring(char const* fmt, Args&& ... args) { impl::VerifyVarArgs<Args...>::test(); return lua_pushfstring(m_state,fmt,std::forward<Args>(args)...); }
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
		tagged(0,0,-)					lua_Unsigned rawlen(int);
		tagged(2,0,e)					void rawset(int);
		tagged(1,0,e)					void rawseti(int,lua_Integer);
		tagged(1,0,e)					void rawsetp(int,void const*);
		tagged(0,0,e)					void register_(char const*, lua_CFunction);
		tagged(1,0,-)					void remove(int);
		tagged(1,0,-)					void replace(int);
		tagged(0,?,-)					int resetthread();
		tagged(?,?,-)					int resume(lua_State*,int,int*);
		tagged(0,0,-)					void setallocf(lua_Alloc,void*);
		tagged(1,0,e)					void setfield(int,char const*);
		tagged(1,0,e)					void setglobal(char const*);
		tagged(1,0,-)					int setiuservalue(int, int);
		tagged(1,0,-)					int setmetatable(int);
		tagged(2,0,e)					void settable(int);
		tagged(?,?,-)					void settop(int);
		tagged(1,0,-)					int setuservalue(int);
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
		tagged(0,0,v)					lua_Number version();
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
		tagged(0,0,v)					template <typename ... Args> int error(char const* fmt, Args&& ... args) { impl::VerifyVarArgs<Args...>::test(); return luaL_error(GetState(),fmt,std::forward<Args>(args)...); }
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
	};
}


#endif
