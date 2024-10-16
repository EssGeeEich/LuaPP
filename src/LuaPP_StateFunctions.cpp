#include "State.hpp"

namespace Lua {

// clang-format off
int State::absindex(int index) { return lua_absindex(GetState(),index); }
void State::arith(Operator op) { return lua_arith(GetState(),static_cast<int>(op)); }
lua_CFunction State::atpanic(lua_CFunction f) { return lua_atpanic(GetState(),f); }
void State::call(int a, int b, int c, lua_KFunction d) { return lua_callk(GetState(),a,b,c,d); }
int State::checkstack(int v) { return lua_checkstack(GetState(),v); }
int State::closethread(lua_State* a) { return lua_closethread(GetState(),a); }
int State::compare(int a, int b, CompareOp c) { return lua_compare(GetState(),a,b,static_cast<int>(c)); }
void State::concat(int v) { return lua_concat(GetState(),v); }
void State::copy(int a, int b) { return lua_copy(GetState(),a,b); }
void State::createtable(int a, int b) { return lua_createtable(GetState(),a,b); }
int State::dump(lua_Writer w, void* p, int n) { return lua_dump(GetState(),w,p,n); }
int State::error() { return lua_error(GetState()); }
int State::gc(GcWhat a, int b) { return lua_gc(GetState(),static_cast<int>(a),b); }
lua_Alloc State::getallocf(void** p) { return lua_getallocf(GetState(),p); }
int State::getfield(int v, char const* f) { return lua_getfield(GetState(),v,f); }
int State::getglobal(char const* name) { return lua_getglobal(GetState(),name); }
int State::getiuservalue(int index, int n) { return lua_getiuservalue(GetState(),index,n); }
int State::getinfo(char const* what, lua_Debug* ar) { return lua_getinfo(GetState(), what, ar); }
int State::getmetatable(int index) { return lua_getmetatable(GetState(),index); }
int State::gettable(int index) { return lua_gettable(GetState(),index); }
int State::gettop() { return lua_gettop(GetState()); }
int State::getuservalue(int index) { return lua_getuservalue(GetState(),index); }
void State::insert(int index) { return lua_insert(GetState(),index); }
bool State::isboolean(int index) { return lua_isboolean(GetState(),index) != 0; }
bool State::iscfunction(int index) { return lua_iscfunction(GetState(),index) != 0; }
bool State::isfunction(int index) { return lua_isfunction(GetState(),index) != 0; }
bool State::isinteger(int index) { return lua_isinteger(GetState(),index) != 0; }
bool State::islightuserdata(int index) { return lua_islightuserdata(GetState(),index) != 0; }
bool State::isnil(int index) { return lua_isnil(GetState(),index) != 0; }
bool State::isnone(int index) { return lua_isnone(GetState(),index) != 0; }
bool State::isnoneornil(int index) { return lua_isnoneornil(GetState(),index) != 0; }
bool State::isnumber(int index) { return lua_isnumber(GetState(),index) != 0; }
bool State::isstring(int index) { return lua_isstring(GetState(),index) != 0; }
bool State::istable(int index) { return lua_istable(GetState(),index) != 0; }
bool State::isthread(int index) { return lua_isthread(GetState(),index) != 0; }
bool State::isuserdata(int index) { return lua_isuserdata(GetState(),index) != 0; }
void State::len(int index) { return lua_len(GetState(),index); }
int State::load(lua_Reader a, void* b, char const* c, char const* d) { return lua_load(GetState(),a,b,c,d); }
void State::newtable() { return lua_newtable(GetState()); }
lua_State* State::newthread() { return lua_newthread(GetState()); }
void* State::newuserdata(size_t size) { return lua_newuserdata(GetState(),size); }
void* State::newuserdatauv(size_t size, int nuvalue) { return lua_newuserdatauv(GetState(),size,nuvalue); }
int State::next(int index) { return lua_next(GetState(),index); }
int State::pcall(int a, int b, int c, int d, lua_KFunction e) { return lua_pcallk(GetState(),a,b,c,d,e); }
void State::pop(int count) { return lua_pop(GetState(),count); }
void State::pushboolean(bool v) { return lua_pushboolean(GetState(),v ? 1 : 0); }
void State::pushcclosure(lua_CFunction f, int c) { return lua_pushcclosure(GetState(),f,c); }
void State::pushcfunction(lua_CFunction f) { return lua_pushcfunction(GetState(),f); }
void State::pushinteger(lua_Integer i) { return lua_pushinteger(GetState(),i); }
void State::pushlightuserdata(void* d) { return lua_pushlightuserdata(GetState(),d); }
char const* State::pushlstring(char const* s, size_t l) { return lua_pushlstring(GetState(),s,l); }
void State::pushnil() { return lua_pushnil(GetState()); }
void State::pushnumber(lua_Number n) { return lua_pushnumber(GetState(),n); }
char const* State::pushstring(char const* s) { return lua_pushstring(GetState(),s); }
int State::pushthread() { return lua_pushthread(GetState()); }
void State::pushvalue(int index) { return lua_pushvalue(GetState(),index); }
char const* State::pushvfstring(char const* fmt, va_list list) { return lua_pushvfstring(GetState(),fmt,list); }
int State::rawequal(int a, int b) { return lua_rawequal(GetState(),a,b); }
int State::rawget(int index) { return lua_rawget(GetState(),index); }
int State::rawgeti(int index, lua_Integer v) { return lua_rawgeti(GetState(),index,v); }
int State::rawgetp(int index, void const* p) { return lua_rawgetp(GetState(),index,p); }
lua_Unsigned State::rawlen(int index) { return lua_rawlen(GetState(),index); }
void State::rawset(int index) { return lua_rawset(GetState(),index); }
void State::rawseti(int index, lua_Integer i) { return lua_rawseti(GetState(),index,i); }
void State::rawsetp(int index, void const* p) { return lua_rawsetp(GetState(),index,p); }
void State::register_(char const* s, lua_CFunction f) { return lua_register(GetState(),s,f); }
void State::remove(int index) { return lua_remove(GetState(),index); }
void State::replace(int index) { return lua_replace(GetState(),index); }
int State::resetthread() { return lua_resetthread(GetState()); }
int State::resume(lua_State* state2, int index, int* nres) { return lua_resume(GetState(),state2,index,nres); }
void State::setallocf(lua_Alloc alloc, void* p) { return lua_setallocf(GetState(),alloc,p); }
void State::setfield(int index, char const* f) { return lua_setfield(GetState(),index,f); }
void State::setglobal(char const* glob) { return lua_setglobal(GetState(),glob); }
int State::setiuservalue(int index, int n) { return lua_setiuservalue(GetState(),index,n); }
int State::setmetatable(int index) { return lua_setmetatable(GetState(),index); }
void State::settable(int index) { return lua_settable(GetState(),index); }
void State::settop(int index) { return lua_settop(GetState(),index); }
int State::setuservalue(int index) { return lua_setuservalue(GetState(),index); }
int State::status() { return lua_status(GetState()); }
bool State::toboolean(int index) { return lua_toboolean(GetState(),index) != 0; }
lua_CFunction State::tocfunction(int index) { return lua_tocfunction(GetState(),index); }
lua_Integer State::tointeger(int index) { return lua_tointeger(GetState(),index); }
lua_Integer State::tointegerx(int index, int* p) { return lua_tointegerx(GetState(),index,p); }
char const* State::tolstring(int index, size_t* sz) { return lua_tolstring(GetState(),index,sz); }
lua_Number State::tonumber(int index) { return lua_tonumber(GetState(),index); }
lua_Number State::tonumberx(int index, int* p) { return lua_tonumberx(GetState(),index,p); }
void const* State::topointer(int index) { return lua_topointer(GetState(),index); }
char const* State::tostring(int index) { return lua_tostring(GetState(),index); }
lua_State* State::tothread(int index) { return lua_tothread(GetState(),index); }
void* State::touserdata(int index) { return lua_touserdata(GetState(),index); }
Type State::type(int index) { return static_cast<Type>(lua_type(GetState(),index)); }
char const* State::typename_(int index) { return lua_typename(GetState(),index); }
lua_Number State::version() { return lua_version(GetState()); }
void State::xmove(lua_State* state2, int index) { return lua_xmove(GetState(),state2,index); }
int State::yieldk(int a, int b, lua_KFunction c) { return lua_yieldk(GetState(),a,b,c); }

int State::argerror(int index, char const* err) { return luaL_argerror(GetState(),index,err); }
int State::callmeta(int index, char const* m) { return luaL_callmeta(GetState(),index,m); }
void State::checkany(int index) { return luaL_checkany(GetState(),index); }
lua_Integer State::checkinteger(int index) { return luaL_checkinteger(GetState(),index); }
char const* State::checklstring(int index, size_t* len) { return luaL_checklstring(GetState(),index,len); }
lua_Number State::checknumber(int index) { return luaL_checknumber(GetState(),index); }
int State::checkoption(int index, char const* def, char const* const* opts) { return luaL_checkoption(GetState(),index,def,opts); }
void State::checkstack(int n, char const* v) { return luaL_checkstack(GetState(),n,v); }
char const* State::checkstring(int index) { return luaL_checkstring(GetState(),index); }
void State::checktype(int index, Type type) { return luaL_checktype(GetState(),index,static_cast<int>(type)); }
void* State::checkudata(int index, char const* ud) { return luaL_checkudata(GetState(),index,ud); }
void State::checkversion() { return luaL_checkversion(GetState()); }
int State::getmetafield(int index, char const* f) { return luaL_getmetafield(GetState(),index,f); }
int State::getmetatable(char const* mt) { return luaL_getmetatable(GetState(),mt); }
int State::getsubtable(int v, char const* t) { return luaL_getsubtable(GetState(),v,t); }
char const* State::gsub(char const* a, char const* b, char const* c) { return luaL_gsub(GetState(),a,b,c); }
lua_Integer State::len_aux(int index) { return luaL_len(GetState(),index); }
int State::loadbuffer(char const* b, size_t n, char const* c, char const* m) { return luaL_loadbufferx(GetState(),b,n,c,m); }
int State::loadfile(char const* f, char const* m) { return luaL_loadfilex(GetState(),f,m); }
int State::loadstring(char const* s) { return luaL_loadstring(GetState(),s); }
int State::newmetatable(char const* mt) { return luaL_newmetatable(GetState(),mt); }
void State::openlibs() { return luaL_openlibs(GetState()); }
lua_Integer State::optinteger(int index, lua_Integer i) { return luaL_optinteger(GetState(),index,i); }
char const* State::optlstring(int index, char const* v, size_t* sz) { return luaL_optlstring(GetState(),index,v,sz); }
lua_Number State::optnumber(int index, lua_Number i) { return luaL_optnumber(GetState(),index,i); }
char const* State::optstring(int index, char const* v) { return luaL_optstring(GetState(),index,v); }
int State::ref(int index) { return int(luaL_ref(GetState(),index)); }
void State::requiref(char const* r, lua_CFunction f, int n) { return luaL_requiref(GetState(),r,f,n); }
void State::setfuncs(luaL_Reg const* r, int c) { return luaL_setfuncs(GetState(),r,c); }
void State::setmetatable(char const* mt) { return luaL_setmetatable(GetState(),mt); }
void* State::testudata(int n, char const* ud) { return luaL_testudata(GetState(),n,ud); }
char const* State::tolstring_aux(int n, size_t* p) { return luaL_tolstring(GetState(),n,p); }
char const* State::typename_aux(int n) { return luaL_typename(GetState(),n); }
void State::unref(int n, int r) { return luaL_unref(GetState(),n,r); }
void State::where(int index) { return luaL_where(GetState(),index); }
// clang-format on

}
