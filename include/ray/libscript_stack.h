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

#ifndef _H_LIBSCRIPT_STACK_H_
#define _H_LIBSCRIPT_STACK_H_

#include <ray/script/libscript_fwd.h>

_NAME_BEGIN

/// @addtogroup script
/// @{

/// @brief Stack operations
/// Packaging lua API and constants
class EXPORT Stack
{
    friend class Value;
public:
    static int REGISTRYINDEX();

    enum VALUETYPE
    {
        T_Nil = 0,
        T_Boolean = 1,
        T_LightUserData = 2,
        T_Number = 3,
        T_String = 4,
        T_Table = 5,
        T_Function = 6,
        T_Userdata = 7,
        T_Thread = 8,
        NoneMask = 0xE9,
    };

    enum THREADSTATUS
    {
        STATUS_OK = 0,
        STATUS_YIELD = 1,
        STATUS_ERRRUN = 2,
        STATUS_ERRSYNTAX = 3,
        STATUS_ERRMEM = 4,
        STATUS_ERRGCMM = 5,
        STATUS_ERRERR = 6,
    };

    enum ARITHOP
    {
        OP_ADD = 0,
        OP_SUB = 1,
        OP_MUL = 2,
        OP_DIV = 3,
        OP_MOD = 4,
        OP_POW = 5,
        OP_UNM = 6,
    };

    enum GCBEHAVIOR
    {
        GC_STOP = 0,
        GC_RESTART = 1,
        GC_COLLECT = 2,
        GC_COUNT = 3,
        GC_COUNTB = 4,
        GC_STEP = 5,
        GC_SETPAUSE = 6,
        GC_SETSTEPMUL = 7,
        GC_SETMAJORINC = 8,
        GC_ISRUNNING = 9,
        GC_GEN = 10,
        GC_INC = 11,
    };

    enum COMPAREOP {
        OP_EQ = 0,  // ==
        OP_LT = 1,  // <
        OP_LE = 2,  // <=
    };

    Stack(RawInterface raw);

    Stack(Stack& stack);

    virtual ~Stack();

    void sameThread(Stack& stack);
    bool sameThreadSafe(Stack& stack);

    RawInterface getInterface() { return _c_state; }

    /// @brief clear stack
    void clear() { this->settop(0); }

    /// Lua base APIs
    /// reference to visit : http://www.lua.org/manual/5.2/

    int absindex(int index);
    void arith(ARITHOP op);
    CFunction atpanic(CFunction panicf);
    void call(int nargs, int nresults);
    void callk(int nargs, int nresults, int ctx, CFunction k);
    void checkstack(int extra);
    static void close(RawInterface L);
    int compare(int index1, int index2, COMPAREOP op);
    void concat(int n);
    void copy(int fromidx, int toidx);
    void createtable(int narr, int nrec);
    int dump(ChunkWriterFunc writer, void* data);
    int error();
    int gc(GCBEHAVIOR what, int data);
    int getctx(int* ctx);
    void getfield(int index, const char* k);
    void getglobal(const char* name);
    int getmetatable(int index);
    void gettable(int index);
    int gettop();
    void getuservalue(int index);
    void insert(int index);
    bool isboolean(int index);
    bool iscfunction(int index);
    bool isfunction(int index);
    bool islightuserdata(int index);
    bool isnil(int index);
    bool isnone(int index);
    bool isnoneornil(int index);
    bool isnumber(int index);
    bool isstring(int index);
    bool istable(int index);
    bool isthread(int index);
    bool isuserdata(int index);
    void len(int index);
    int load(ChunkReaderFunc reader, void* data, const char* source, const char* mode);
    static RawInterface newstate(MemAllocFunc f, void *ud);
    void newtable();
    RawInterface newthread();
    void* newuserdata(std::size_t size);
    int next(int index);
    THREADSTATUS pcall(int nargs, int nresults, int msgh);
    THREADSTATUS pcallk(int nargs, int nresults, int errfunc, int ctx, CFunction k);
    void pop(int n);
    void pushboolean(int b);
    void pushcclosure(CFunction fn, int n);
    void pushcfunction(CFunction f);
    const char* pushfstring(const char* fmt, ...);
    void pushglobaltable();
    void pushinteger(long long n);
    void pushlightuserdata(void* p);
    void pushlstring(const char* s, std::size_t len);
    void pushnil();
    void pushnumber(double n);
    const char* pushstring(const char* s);
    int pushthread();
    void pushunsigned(unsigned int n);
    void pushValue(int index);
    const char* pushvfstring(const char* fmt, char* argp);
    int rawequal(int index1, int index2);
    void rawget(int index);
    void rawgeti(int index, int n);
    void rawgetp(int index, const void* p);
    std::size_t rawlen(int index);
    void rawset(int index);
    void rawseti(int index, int n);
    void rawsetp(int index, const void* p);
    void register_(const char* name, CFunction f); 
    void remove(int index);
    void replace(int index);
    THREADSTATUS resume(RawInterface from, int nargs);
    // void lua_setallocf(lua_Alloc f, void* ud);
    void setfield(int index, const char* k);
    void setglobal(const char* name);
    void setmetatable(int index);
    void settable(int index);
    void settop(int index);
    void setuservalue(int index);
    THREADSTATUS status();
    bool toboolean(int index);
    CFunction tocfunction(int index);
    long long tointeger(int index);
    long long tointegerx(int index, int* isnum);
    const char* tolstring(int index, size_t* len);
    double tonumber(int index);
    double tonumberx(int index, int* isnum);
    const void* topointer(int index);
    const char* tostring(int index);
    RawInterface tothread(int index);
    unsigned int tounsigned(int index);
    unsigned int tounsignedx(int index, int* isnum);
    void* touserdata(int index);
    VALUETYPE type(int index);
    const char* typename_(VALUETYPE tp);
    static int upvalueindex(int i);
    void xmove(Stack& to, int n);
    int yield(int nresults);
    int yieldk(int nresults, int ctx, CFunction k);

    const char* getupvalue(int funcindex, int n);
    const char* setupvalue(int funcindex, int n);

    /// Auxiliary APIs

    void argcheck_L(int cond, int arg, const char* extramsg);
    int argerror_L(int arg, const char* extramsg);
    int callmeta_L(int obj, const char* e);
    void checkany_L(int arg);
    int checkint_L(int arg);
    long long checkinteger_L(int arg);
    long checklong_L(int arg);
    const char* checklstring_L(int arg, std::size_t* l);
    double checknumber_L(int arg);
    int checkoption_L(int arg, const char* def, const char* const lst[]);
    void checkstack_L(int sz, const char* msg);
    const char* checkstring_L(int arg);
    void checktype_L(int arg, VALUETYPE t);
    void* checkudata_L(int arg, const char* tname);
    unsigned int checkunsigned_L(int arg);
    void checkversion_L();
    int dofile_L(const char* filename);
    int dostring_L(const char* str);
    int error_L(const char* fmt, ...);
    int execresult_L(int stat);
    int fileresult_L(int stat, const char* fname);
    int getmetafield_L(int obj, const char* e);
    void getmetatable_L(const char* tname);
    int getsubtable_L(int idx, const char* fname);
    const char* gsub_L(const char* s, const char* p, const char* r);
    int len_L(int index);
    int loadbuffer_L(const char* buff, std::size_t sz, const char* name);
    int loadbufferx_L(const char* buff, std::size_t sz, const char* name, const char* mode);
    int loadfile_L(const char* filename);
    int loadfilex_L(const char* filename, const char* mode);
    int loadstring_L(const char* s);
    int newmetatable_L(const char* tname);
    static RawInterface newstate_L();
    void openlibs_L();
    int optint_L(int arg, int d);
    long long optinteger_L(int arg, long long d);
    long optlong_L(int arg, long d);
    const char* optlstring_L(int arg, const char* d, std::size_t* l);
    double optnumber_L(int arg, double d);
    const char* optstring_L(int arg, const char* d);
    unsigned int optunsigned_L(int arg, unsigned int u);
    int ref_L(int t);
    void requiref_L(const char* modname, CFunction openf, int glb);
    void setmetatable_L(const char* tname);
    void* testudata_L(int arg, const char* tname);
    const char* tolstring_L(int idx, std::size_t* len);
    void traceback_L(RawInterface L1, const char* msg, int level);
    const char* typename_L(int index);
    void unref_L(int t, int ref);
    void where_L(int lvl);

protected:
    RawInterface _c_state;
};

/// @}
_NAME_END

#endif