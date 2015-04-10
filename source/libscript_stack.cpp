// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------

#include <ray/script/libscript_stack.h>
#include <ray/script/libscript_luaapi.h>

#include <ray/platform/except.h>

_NAME_BEGIN

int Stack::REGISTRYINDEX()
{
    return LUA_REGISTRYINDEX;
}

Stack::Stack(Stack& stack) : _c_state(stack._c_state)
{

}

Stack::Stack(RawInterface state) : _c_state(state)
{
   
}

Stack::~Stack()
{
    
}

int Stack::absindex(int index)
{
    return lua_absindex(LUASTATE(_c_state), index);
}

void Stack::arith(ARITHOP op)
{
    lua_arith(LUASTATE(_c_state), op);
}

CFunction Stack::atpanic(CFunction panicf)
{
    return lua_atpanic(LUASTATE(_c_state), panicf);
}

void Stack::call(int nargs, int nresults)
{
    lua_call(LUASTATE(_c_state), nargs, nresults);
}

void Stack::callk(int nargs, int nresults, int ctx, CFunction k)
{
    lua_callk(LUASTATE(_c_state), nargs, nresults, ctx, k);
}

void Stack::checkstack(int extra)
{
    lua_checkstack(LUASTATE(_c_state), extra);
}

void Stack::close(RawInterface L)
{
    lua_close(L);
}

int Stack::compare(int index1, int index2, COMPAREOP op)
{
    return lua_compare(LUASTATE(_c_state), index1, index2, op);
}

void Stack::concat(int n)
{
    lua_concat(LUASTATE(_c_state), n);
}

void Stack::copy(int fromidx, int toidx)
{
    lua_copy(LUASTATE(_c_state), fromidx, toidx);
}

void Stack::createtable(int narr, int nrec)
{
    lua_createtable(LUASTATE(_c_state), narr, nrec);
}

int Stack::dump(ChunkWriterFunc writer, void* data)
{
    return lua_dump(LUASTATE(_c_state), writer, data);
}

int Stack::error()
{
    return lua_error(LUASTATE(_c_state));
}

int Stack::gc(GCBEHAVIOR what, int data)
{
    return lua_gc(LUASTATE(_c_state), what, data);
}

int Stack::getctx(int* ctx)
{
    return lua_getctx(LUASTATE(_c_state), ctx);
}

void Stack::getfield(int index, const char* k)
{
    lua_getfield(LUASTATE(_c_state), index, k);
}

void Stack::getglobal(const char* name)
{
    lua_getglobal(LUASTATE(_c_state), name);
}

int Stack::getmetatable(int index)
{
    return lua_getmetatable(LUASTATE(_c_state), index);
}

void Stack::gettable(int index)
{
    lua_gettable(LUASTATE(_c_state), index);
}

int Stack::gettop()
{
    return lua_gettop(LUASTATE(_c_state));
}

void Stack::getuservalue(int index)
{
    lua_getuservalue(LUASTATE(_c_state), index);
}

void Stack::insert(int index)
{
    lua_insert(LUASTATE(_c_state), index);
}

bool Stack::isboolean(int index)
{
    return lua_isboolean(LUASTATE(_c_state), index);
}

bool Stack::iscfunction(int index)
{
    return lua_iscfunction(LUASTATE(_c_state), index);
}

bool Stack::isfunction(int index)
{
    return lua_isfunction(LUASTATE(_c_state), index);
}

bool Stack::islightuserdata(int index)
{
    return lua_islightuserdata(LUASTATE(_c_state), index);
}

bool Stack::isnil(int index)
{
    return lua_isnil(LUASTATE(_c_state), index);
}

bool Stack::isnone(int index)
{
    return lua_isnone(LUASTATE(_c_state), index);
}

bool Stack::isnoneornil(int index)
{
    return lua_isnoneornil(LUASTATE(_c_state), index);
}

bool Stack::isnumber(int index)
{
    return lua_isnumber(LUASTATE(_c_state), index);
}

bool Stack::isstring(int index)
{
    return lua_isstring(LUASTATE(_c_state), index);
}

bool Stack::istable(int index)
{
    return lua_istable(LUASTATE(_c_state), index);
}

bool Stack::isthread(int index)
{
    return lua_isthread(LUASTATE(_c_state), index);
}

bool Stack::isuserdata(int index)
{
    return lua_isuserdata(LUASTATE(_c_state), index);
}

void Stack::len(int index)
{
    lua_len(LUASTATE(_c_state), index);
}

int Stack::load(ChunkReaderFunc reader, void* data, const char* source, const char* mode)
{
    return lua_load(LUASTATE(_c_state), reader, data, source, mode);
}

RawInterface Stack::newstate(MemAllocFunc f, void *ud)
{
    return lua_newstate(f, ud);
}

void Stack::newtable()
{
    lua_newtable(LUASTATE(_c_state));
}

RawInterface Stack::newthread()
{
    return lua_newthread(LUASTATE(_c_state));
}

void* Stack::newuserdata(std::size_t size)
{
    return lua_newuserdata(LUASTATE(_c_state), size);
}

int Stack::next(int index)
{
    return lua_next(LUASTATE(_c_state), index);
}

Stack::THREADSTATUS Stack::pcall(int nargs, int nresults, int msgh)
{
    return (Stack::THREADSTATUS)lua_pcall(LUASTATE(_c_state), nargs, nresults, msgh);
}

Stack::THREADSTATUS Stack::pcallk(int nargs, int nresults, int errfunc, int ctx, CFunction k)
{
    return (Stack::THREADSTATUS)lua_pcallk(LUASTATE(_c_state), nargs, nresults, errfunc, ctx, k);
}

void Stack::pop(int n)
{
    lua_pop(LUASTATE(_c_state), n);
}

void Stack::pushboolean(int b)
{
    lua_pushboolean(LUASTATE(_c_state), b);
}

void Stack::pushcclosure(CFunction fn, int n)
{
    lua_pushcclosure(LUASTATE(_c_state), fn, n);
}

void Stack::pushcfunction(CFunction f)
{
    lua_pushcclosure(LUASTATE(_c_state), f, 0);
}

const char* Stack::pushfstring(const char* fmt, ...)
{
    va_list vlArgs;
    va_start(vlArgs, fmt);
    const char* s = lua_pushfstring(LUASTATE(_c_state), fmt, vlArgs);
    va_end(vlArgs);
    return s;
}

void Stack::pushglobaltable()
{
    lua_pushglobaltable(LUASTATE(_c_state));
}

void Stack::pushinteger(long long n)
{
    lua_pushinteger(LUASTATE(_c_state), n);
}

void Stack::pushlightuserdata(void* p)
{
    lua_pushlightuserdata(LUASTATE(_c_state), p);
}

void Stack::pushlstring(const char* s, std::size_t len)
{
    lua_pushlstring(LUASTATE(_c_state), s, len);
}

void Stack::pushnil()
{
    lua_pushnil(LUASTATE(_c_state));
}

void Stack::pushnumber(double n)
{
    lua_pushnumber(LUASTATE(_c_state), n);
}

const char* Stack::pushstring(const char* s)
{
    return lua_pushstring(LUASTATE(_c_state), s);
}

int Stack::pushthread()
{
    return lua_pushthread(LUASTATE(_c_state));
}

void Stack::pushunsigned(unsigned int n)
{
    lua_pushunsigned(LUASTATE(_c_state), n);
}

void Stack::pushValue(int index)
{
    lua_pushvalue(LUASTATE(_c_state), index);
}

const char* Stack::pushvfstring(const char* fmt, char* argp)
{
    return lua_pushvfstring(LUASTATE(_c_state), fmt, argp);
}

int Stack::rawequal(int index1, int index2)
{
    return lua_rawequal(LUASTATE(_c_state), index1, index2);
}

void Stack::rawget(int index)
{
    lua_rawget(LUASTATE(_c_state), index);
}

void Stack::rawgeti(int index, int n)
{
    lua_rawgeti(LUASTATE(_c_state), index, n);
}

void Stack::rawgetp(int index, const void* p)
{
    lua_rawgetp(LUASTATE(_c_state), index, p);
}

std::size_t Stack::rawlen(int index)
{
    return lua_rawlen(LUASTATE(_c_state), index);
}

void Stack::rawset(int index)
{
    lua_rawset(LUASTATE(_c_state), index);
}

void Stack::rawseti(int index, int n)
{
    lua_rawseti(LUASTATE(_c_state), index, n);
}

void Stack::rawsetp(int index, const void* p)
{
    lua_rawgetp(LUASTATE(_c_state), index, p);
}

void Stack::register_(const char* name, CFunction f)
{
    lua_register(LUASTATE(_c_state), name, f);
}

void Stack::remove(int index)
{
    lua_remove(LUASTATE(_c_state), index);
}

void Stack::replace(int index)
{
    lua_replace(LUASTATE(_c_state), index);
}

Stack::THREADSTATUS Stack::resume(RawInterface from, int nargs)
{
    return (THREADSTATUS)lua_resume(LUASTATE(_c_state), LUASTATE(from), nargs);
}

void Stack::setfield(int index, const char* k)
{
    lua_setfield(LUASTATE(_c_state), index, k);
}

void Stack::setglobal(const char* name)
{
    lua_setglobal(LUASTATE(_c_state), name);
}

void Stack::setmetatable(int index)
{
    lua_setmetatable(LUASTATE(_c_state), index);
}

void Stack::settable(int index)
{
    lua_settable(LUASTATE(_c_state), index);
}

void Stack::settop(int index)
{
    lua_settop(LUASTATE(_c_state), index);
}

void Stack::setuservalue(int index)
{
    lua_setuservalue(LUASTATE(_c_state), index);
}

Stack::THREADSTATUS Stack::status()
{
    return (THREADSTATUS)lua_status(LUASTATE(_c_state));
}

bool Stack::toboolean(int index)
{
    return lua_toboolean(LUASTATE(_c_state), index) == 1;
}

CFunction Stack::tocfunction(int index)
{
    return lua_tocfunction(LUASTATE(_c_state), index);
}

long long Stack::tointeger(int index)
{
    return lua_tointeger(LUASTATE(_c_state), index);
}

long long Stack::tointegerx(int index, int* isnum)
{
    return lua_tointegerx(LUASTATE(_c_state), index, isnum);
}

const char* Stack::tolstring(int index, size_t *len)
{
    return lua_tolstring(LUASTATE(_c_state), index, len);
}

double Stack::tonumber(int index)
{
    return lua_tonumber(LUASTATE(_c_state), index);
}

double Stack::tonumberx(int index, int* isnum)
{
    return lua_tonumberx(LUASTATE(_c_state), index, isnum);
}

const void* Stack::topointer(int index)
{
    return lua_topointer(LUASTATE(_c_state), index);
}

const char* Stack::tostring(int index)
{
    return lua_tostring(LUASTATE(_c_state), index);
}

RawInterface Stack::tothread(int index)
{
    return lua_tothread(LUASTATE(_c_state), index);
}

unsigned int Stack::tounsigned(int index)
{
    return lua_tounsigned(LUASTATE(_c_state), index);
}

unsigned int Stack::tounsignedx(int index, int* isnum)
{
    return lua_tounsignedx(LUASTATE(_c_state), index, isnum);
}

void* Stack::touserdata(int index)
{
    return lua_touserdata(LUASTATE(_c_state), index);
}

Stack::VALUETYPE Stack::type(int index)
{
    return (VALUETYPE)lua_type(LUASTATE(_c_state), index);
}

const char* Stack::typename_(VALUETYPE tp)
{
    return lua_typename(LUASTATE(_c_state), tp);
}

int Stack::upvalueindex(int i)
{
    return lua_upvalueindex(i);
}

void Stack::xmove(Stack& to, int n)
{
    lua_xmove(LUASTATE(_c_state), LUASTATE(to.getInterface()), n);
}

int Stack::yield(int nresults)
{
    return lua_yield(LUASTATE(_c_state), nresults);
}

int Stack::yieldk(int nresults, int ctx, CFunction k)
{
    return lua_yieldk(LUASTATE(_c_state), nresults, ctx, k);
}

const char* Stack::getupvalue(int funcindex, int n)
{
    return lua_getupvalue(LUASTATE(_c_state), funcindex, n);
}

const char* Stack::setupvalue(int funcindex, int n)
{
    return lua_setupvalue(LUASTATE(_c_state), funcindex, n);
}

void Stack::sameThread(Stack& stack)
{
    if (_c_state != stack._c_state)
        SCRIPT_EXCEPTION("Different thread");
}

bool Stack::sameThreadSafe(Stack& stack)
{
    return _c_state == stack._c_state;
}

void Stack::argcheck_L(int cond, int arg, const char* extramsg)
{
    luaL_argcheck(LUASTATE(_c_state), cond, arg, extramsg);
}

int Stack::argerror_L(int arg, const char* extramsg)
{
    return luaL_argerror(LUASTATE(_c_state), arg, extramsg);
}

int Stack::callmeta_L(int obj, const char* e)
{
    return luaL_callmeta(LUASTATE(_c_state), obj, e);
}

void Stack::checkany_L(int arg)
{
    luaL_checkany(LUASTATE(_c_state), arg);
}

int Stack::checkint_L(int arg)
{
    return luaL_checkint(LUASTATE(_c_state), arg);
}

long long Stack::checkinteger_L(int arg)
{
    return luaL_checkinteger(LUASTATE(_c_state), arg);
}

long Stack::checklong_L(int arg)
{
    return luaL_checklong(LUASTATE(_c_state), arg);
}

const char* Stack::checklstring_L(int arg, std::size_t* l)
{
    return luaL_checklstring(LUASTATE(_c_state), arg, l);
}

double Stack::checknumber_L(int arg)
{
    return luaL_checknumber(LUASTATE(_c_state), arg);
}

int Stack::checkoption_L(int arg, const char* def, const char* const lst[])
{
    return luaL_checkoption(LUASTATE(_c_state), arg, def, lst);
}

void Stack::checkstack_L(int sz, const char* msg)
{
    luaL_checkstack(LUASTATE(_c_state), sz, msg);
}

const char* Stack::checkstring_L(int arg)
{
    return luaL_checkstring(LUASTATE(_c_state), arg);
}

void Stack::checktype_L(int arg, VALUETYPE t)
{
    luaL_checktype(LUASTATE(_c_state), arg, t);
}

void* Stack::checkudata_L(int arg, const char* tname)
{
    return luaL_checkudata(LUASTATE(_c_state), arg, tname);
}

unsigned int Stack::checkunsigned_L(int arg)
{
    return luaL_checkunsigned(LUASTATE(_c_state), arg);
}

void Stack::checkversion_L()
{
    luaL_checkversion(LUASTATE(_c_state));
}

int Stack::dofile_L(const char* filename)
{
    return luaL_dofile(LUASTATE(_c_state), filename);
}

int Stack::dostring_L(const char* str)
{
    return luaL_dostring(LUASTATE(_c_state), str);
}

int Stack::error_L(const char* fmt, ...)
{
    va_list vlArgs;
    va_start(vlArgs, fmt);
    int n = luaL_error(LUASTATE(_c_state), fmt, vlArgs);
    va_end(vlArgs);
    return n;
}

int Stack::execresult_L(int stat)
{
    return luaL_execresult(LUASTATE(_c_state), stat);
}

int Stack::fileresult_L(int stat, const char* fname)
{
    return luaL_fileresult(LUASTATE(_c_state), stat, fname);
}

int Stack::getmetafield_L(int obj, const char* e)
{
    return luaL_getmetafield(LUASTATE(_c_state), obj, e);
}

void Stack::getmetatable_L(const char* tname)
{
    luaL_getmetatable(LUASTATE(_c_state), tname);
}

int Stack::getsubtable_L(int idx, const char* fname)
{
    return luaL_getsubtable(LUASTATE(_c_state), idx, fname);
}

const char* Stack::gsub_L(const char* s, const char* p, const char* r)
{
    return luaL_gsub(LUASTATE(_c_state), s, p, r);
}

int Stack::len_L(int index)
{
    return luaL_len(LUASTATE(_c_state), index);
}

int Stack::loadbuffer_L(const char* buff, std::size_t sz, const char* name)
{
    return luaL_loadbuffer(LUASTATE(_c_state), buff, sz, name);
}

int Stack::loadbufferx_L(const char* buff, std::size_t sz, const char* name, const char* mode)
{
    return luaL_loadbufferx(LUASTATE(_c_state), buff, sz, name, mode);
}

int Stack::loadfile_L(const char* filename)
{
    return luaL_loadfile(LUASTATE(_c_state), filename);
}

int Stack::loadfilex_L(const char* filename, const char* mode)
{
    return luaL_loadfilex(LUASTATE(_c_state), filename, mode);
}

int Stack::loadstring_L(const char* s)
{
    return luaL_loadstring(LUASTATE(_c_state), s);
}

int Stack::newmetatable_L(const char* tname)
{
    return luaL_newmetatable(LUASTATE(_c_state), tname);
}

RawInterface Stack::newstate_L()
{
    return luaL_newstate();
}

void Stack::openlibs_L()
{
    return luaL_openlibs(LUASTATE(_c_state));
}

int Stack::optint_L(int arg, int d)
{
    return luaL_optint(LUASTATE(_c_state), arg, d);
}

long long Stack::optinteger_L(int arg, long long d)
{
    return luaL_optinteger(LUASTATE(_c_state), arg, d);
}

long Stack::optlong_L(int arg, long d)
{
    return luaL_optlong(LUASTATE(_c_state), arg, d);
}

const char* Stack::optlstring_L(int arg, const char* d, std::size_t* l)
{
    return luaL_optlstring(LUASTATE(_c_state), arg, d, l);
}

double Stack::optnumber_L(int arg, double d)
{
    return luaL_optnumber(LUASTATE(_c_state), arg, d);
}

const char* Stack::optstring_L(int arg, const char* d)
{
    return luaL_optstring(LUASTATE(_c_state), arg, d);
}

unsigned int Stack::optunsigned_L(int arg, unsigned int u)
{
    return luaL_optunsigned(LUASTATE(_c_state), arg, u);
}

int Stack::ref_L(int t)
{
    return luaL_ref(LUASTATE(_c_state), t);
}

void Stack::requiref_L(const char* modname, CFunction openf, int glb)
{
    luaL_requiref(LUASTATE(_c_state), modname, openf, glb);
}

void Stack::setmetatable_L(const char* tname)
{
    luaL_setmetatable(LUASTATE(_c_state), tname);
}

void* Stack::testudata_L(int arg, const char* tname)
{
    return luaL_testudata(LUASTATE(_c_state), arg, tname);
}

const char* Stack::tolstring_L(int idx, std::size_t* len)
{
    return luaL_tolstring(LUASTATE(_c_state), idx, len);
}

void Stack::traceback_L(RawInterface L1, const char* msg, int level)
{
    luaL_traceback(LUASTATE(_c_state), L1, msg, level);
}

const char* Stack::typename_L(int index)
{
    return luaL_typename(LUASTATE(_c_state), index);
}

void Stack::unref_L(int t, int ref)
{
    luaL_unref(LUASTATE(_c_state), t,  ref);
}

void Stack::where_L(int lvl)
{
    luaL_where(LUASTATE(_c_state), lvl);
}

_NAME_END