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

#ifndef _H_LIBSCRIPT_VALUE_H_
#define _H_LIBSCRIPT_VALUE_H_

#include <ray/script/libscript_stack.h>

/// @addtogroup script
/// @{

_NAME_BEGIN

class Arg;

class EXPORT Value : public Stack
{
    friend class Script;
public:
    static const struct P_Nil {} Nil;

    Value(RawInterface raw);
    Value(Stack& stack);
    Value(Arg& arg);
    Value(Value& copy);
    virtual ~Value();

    /// @brief Push value in stack and check type
    /// @warning May be thow a exception
    void pushRef(VALUETYPE check);

    /// @brief Push value in stack and check type
    /// @return The value is or not given type
    bool pushRefSafe(VALUETYPE check);

    /// @brief Set stack top to new value, and pop stack top
    /// @note If stack is empty, Will to push a nil then to reset
    void reset();
    void reset(Stack& stack);

    const char* typeName();
    VALUETYPE type();

    /// Try to conversion to base types
    bool toBoolean();
    long long toInteger();
    double toNumber();
    void* toUserData();
    const char* toString();

    template<typename _Class> _Class* toClass()
    {
        pushRef(T_Userdata);
        std::string metaname =
            std::string(METATABLEPREFIX) + typeid(_Class).raw_name();
        checkudata_L(-1, metaname.c_str());
        auto info = (ClassInfo<_Class>*)touserdata(-1);
        pop(1);
        return info->readonly ? nullptr : info->obj;
    }

    template<typename _Class> const _Class* toClass() const
    {
        pushRef(T_Userdata);
        std::string metaname =
            std::string(METATABLEPREFIX) + typeid(_Class).raw_name();
        checkudata_L(-1, metaname.c_str());
        auto info = (ClassInfo<_Class>*)touserdata(_index);
        pop(1);
        return info->obj;
    }

    template<typename _Class> const ClassInfo<_Class>* toClassInfo()
    {
        pushRef(T_Userdata);
        std::string metaname =
            std::string(METATABLEPREFIX) + typeid(_Class).raw_name();
        checkudata_L(-1, metaname.c_str());
        auto block = (ClassInfo<_Class>*)touserdata(_index);
        pop(1);
        return info;
    }

    bool isBoolean();
    bool iscFunction();
    bool isFunction();
    bool isLightUserdata();
    bool isNil();
    bool isNumber();
    bool isString();
    bool isTable();
    bool isThread();
    bool isUserdata();

    operator bool();
    operator int();
    operator long();
    operator long long();
    operator float();
    operator double();
    operator void*();
    operator const char*();

    bool operator ==(Value& value);
    bool operator != (Value& value);
    Value& operator=(Value& copy);

protected:
    ValueHandler* _handler;

private:
    static void endOfHandlerRefBuffer();

    void releaseHandler();
};

/// @}

_NAME_END

#endif