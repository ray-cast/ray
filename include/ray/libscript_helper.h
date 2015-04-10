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

#ifndef _H_LIBSCRIPT_HELPER_H_
#define _H_LIBSCRIPT_HELPER_H_

#include <ray/script/libscript_value.h>

/// @addtogroup script
/// @{

_NAME_BEGIN

class EXPORT Arg final : public Stack
{
public:
    Arg(Stack stack, int index) : Stack(stack), _index(index) { }

    const char* typeName()  { return typename_L(_index); }
    VALUETYPE type()        { return Stack::type(_index); }

    /// Try to conversion to base types
    bool toBoolean()        { return toboolean(_index); }
    long long toInteger()   { return tointeger(_index); }
    double toNumber()       { return tonumber(_index); }
    void* toUserData()      { return touserdata(_index); }
    const char* toString()  { const char* str = tostring(_index); return str ? str : ""; }

    template<typename _Class> _Class* toClass()
    { 
        std::string metaname = 
            std::string(METATABLEPREFIX) + typeid(_Class).raw_name();

        checkudata_L(_index, metaname.c_str());

        auto info = (ClassInfo<_Class>*)touserdata(_index);

        return info->readonly ? nullptr : info->obj;
    }

    template<typename _Class> const _Class* toReadOnlyClass()
    {
        std::string metaname =
            std::string(METATABLEPREFIX) + typeid(_Class).raw_name();

        checkudata_L(_index, metaname.c_str());

        auto info = (ClassInfo<_Class>*)touserdata(_index);

        return info->obj;
    }

    template<typename _Class> const ClassInfo<_Class>* toClassInfo()
    {
        std::string metaname =
            std::string(METATABLEPREFIX) + typeid(_Class).raw_name();

        checkudata_L(_index, metaname.c_str());

        return (ClassInfo<_Class>*)touserdata(_index);
    }

    bool isBoolean()        { return isboolean(_index); }
    bool iscFunction()      { return iscfunction(_index); }
    bool isFunction()       { return isfunction(_index); }
    bool isLightUserdata()  { return islightuserdata(_index); }
    bool isNil()            { return isnil(_index); }
    bool isNoneOrNil()      { return isnoneornil(_index); }
    bool isNumber()         { return isnumber(_index); }
    bool isString()         { return isstring(_index); }
    bool isTable()          { return istable(_index); }
    bool isThread()         { return isthread(_index); }
    bool isUserdata()       { return isuserdata(_index); }

    operator bool()         { return toBoolean(); }
    operator int()          { return toInteger(); }
    operator long()         { return toInteger(); }
    operator long long()    { return toInteger(); }
    operator float()        { return toNumber(); }
    operator double()       { return toNumber(); }
    operator void*()        { return toUserData(); }
    operator const char*()  { return toString(); }

    int getIndex() const    { return _index; }

private:
    int _index;
};

class EXPORT Args : public Stack
{
public:
    Args(RawInterface raw) : Stack(raw) { _top = gettop(); }

    /// @brief Get a argument
    /// (Index begin with 1)
    Arg operator[](int i) { Arg arg(*this, i); return arg; }

    int count() const { return _top; }

private:
    int _top;
};

template<typename _Class>
class RefClass : public Class
{
public:
    RefClass(_Class* _c)
    {
        c = _c;
        metaname = std::string(METATABLEPREFIX) + typeid(_Class).raw_name();
        ref = true;
        readonly = false;
    }

    RefClass(_Class& _c)
    {
        c = &_c;
        metaname = std::string(METATABLEPREFIX) + typeid(_Class).raw_name();
        ref = true;
        readonly = false;
    }

    RefClass(const _Class* _c)
    {
        c = const_cast<_Class*>(_c);
        metaname = std::string(METATABLEPREFIX) + typeid(_Class).raw_name();
        ref = true;
        readonly = true;
    }
};

template<typename _Class>
class GCClass : public Class
{
public:
    GCClass(_Class* _c)
    {
        c = _c;
        metaname = std::string(METATABLEPREFIX) + typeid(_Class).raw_name();
        ref = false;
        readonly = false;
    }
};

class EXPORT Pusher final : public Stack
{
public:
    Pusher(RawInterface raw) : Stack(raw), _push_count(0) { }

    Pusher& push(bool b)        { pushboolean(b); ++_push_count; return *this; }
    Pusher& push(long long n)   { pushinteger(n); ++_push_count; return *this; }
    Pusher& push(int n)         { pushinteger(n); ++_push_count; return *this; }
    Pusher& push(unsigned n)    { pushinteger(n); ++_push_count; return *this; }
    Pusher& push(long n)        { pushinteger(n); ++_push_count; return *this; }
    Pusher& push(const std::string& str) { pushstring(str.c_str()); ++_push_count; return *this; }
    Pusher& push(const char* cstr) { pushstring(cstr); ++_push_count; return *this; }
    Pusher& push(double f)      { pushnumber(f); ++_push_count; return *this; }
    Pusher& push(CFunction func){ pushcfunction(func); ++_push_count; return *this; }
    Pusher& push(Value::P_Nil)  { pushnil(); ++_push_count; return *this; }
    Pusher& push(Value value)   { sameThread(value); value.pushRefSafe(NoneMask); ++_push_count; return *this; }
    Pusher& push(Class c);

    void reset()                { _push_count = 0; };
    int count() const           { return _push_count; };

private:
    int _push_count;
};



/// @}

_NAME_END

#endif