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

#include <ray/script/libscript_value.h>

#include <vector>
#include <ray/platform/except.h>
#include <ray/script/libscript_helper.h>

_NAME_BEGIN

#define NOT_STRING_PRINT_BUFF 64
static const char* _s_not_string = "[Is %s]";
static char _s_not_string_print_buff[NOT_STRING_PRINT_BUFF];

const Value::P_Nil Value::Nil = Value::P_Nil();

// To prevent when copy ray::Value to call many Lua APIs
// (Call the luaL_ref and luaL_unref neet so many time)
class ValueHandler
{
public:
    int value = 0;
    int refCount = 0;

    static ValueHandler* create(int value)
    {
        ValueHandler* hander = nullptr;

        if (_s_free_list.size())
        {
            hander = _s_free_list.back();
            _s_free_list.pop_back();
        }
        else
        {
            hander = new ValueHandler;
        }

        hander->value = value;
        hander->refCount = 1;

        ++_s_using_count;

        return hander;
    }

    static void release(ValueHandler* handler)
    {
        if (handler->refCount == 1)
        {
            handler->refCount = 0;
            _s_free_list.push_back(handler);
            --_s_using_count;
        }
        else
        {
            --handler->refCount;
        }
    }

    static ValueHandler* ref(ValueHandler* handler)
    {
        ++handler->refCount;
        return handler;
    }
    
    static void clearFree()
    {
        while (_s_free_list.size())
        {
            delete _s_free_list.back();
            _s_free_list.pop_back();
        }
    }

    static std::size_t getUsingCount()
    {
        return _s_using_count;
    }

private:
    static std::vector<ValueHandler*> _s_free_list;
    static std::size_t _s_using_count;
};

std::vector<ValueHandler*> ValueHandler::_s_free_list;
std::size_t ValueHandler::_s_using_count = 0;

Value::Value(RawInterface raw) : Stack(raw)
{
    if (gettop() == 0)
        throw failure("No element in the stack");

    _handler = ValueHandler::create(ref_L(Stack::REGISTRYINDEX()));
}

Value::Value(Stack& stack) : Stack(stack)
{
    if (gettop() == 0)
        throw failure("No element in the stack");

    _handler = ValueHandler::create(ref_L(Stack::REGISTRYINDEX()));
}

Value::Value(Arg& arg) : Stack(arg)
{
    /// copy arg and push onto the stack
    arg.pushValue(arg.getIndex());

    _handler = ValueHandler::create(ref_L(Stack::REGISTRYINDEX()));
}

Value::Value(Value& copy) : Stack(copy)
{
    _handler = ValueHandler::ref(copy._handler);
}

Value::~Value()
{
    releaseHandler();
}

void Value::pushRef(VALUETYPE check)
{
    rawgeti(Stack::REGISTRYINDEX(), _handler->value);

    if (check != NoneMask) 
    {
        if (Stack::type(-1) != check)
            throw failure("Bad stackobj type.");
    }
}

bool Value::pushRefSafe(VALUETYPE check)
{
    rawgeti(Stack::REGISTRYINDEX(), _handler->value);

    if (check != NoneMask)
    {
        if (Stack::type(-1) != check)
        {
            this->pop(1);
            return false;
        }
    }

    return true;
}

void Value::reset()
{
    if (gettop() == 0)
        pushnil();

    releaseHandler();

    _handler = ValueHandler::create(ref_L(Stack::REGISTRYINDEX()));
}

void Value::reset(Stack& stack)
{
    if (stack.gettop() == 0)
        stack.pushnil();

    releaseHandler();

    _c_state = stack.getInterface();

    _handler = ValueHandler::create(ref_L(Stack::REGISTRYINDEX()));
}

const char* Value::typeName()
{
    pushRefSafe(NoneMask);
    const char* result = typename_L(-1);
    pop(1);
    return result;
}

Stack::VALUETYPE Value::type()
{
    pushRefSafe(NoneMask);
    VALUETYPE result = Stack::type(-1);
    pop(1);
    return result;
}

bool Value::toBoolean()
{
    bool result;
    pushRefSafe(NoneMask);
    result = toboolean(-1);
    pop(1);
    return result;
}

long long Value::toInteger()
{
    int result;
    pushRefSafe(NoneMask);
    result = tointeger(-1);
    pop(1);
    return result;
}

double Value::toNumber()
{
    double result;
    pushRefSafe(NoneMask);
    result = tonumber(-1);
    pop(1);
    return result;
}

void* Value::toUserData()
{
    void* result;
    pushRefSafe(NoneMask);
    result = touserdata(-1);
    pop(1);
    return result;
}

const char* Value::toString()
{
    const char* result;
    pushRefSafe(NoneMask);
    result = tostring(-1);
    pop(1);

    if (result == nullptr)
    {
        std::sprintf(_s_not_string_print_buff, _s_not_string, typeName());
        result = _s_not_string_print_buff;
    }

    return result;
}

bool Value::isBoolean()
{
    pushRefSafe(NoneMask);
    bool b = isboolean(-1);
    pop(1);
    return b;
}

bool Value::iscFunction()
{
    pushRefSafe(NoneMask);
    bool b = iscfunction(-1);
    pop(1);
    return b;
}

bool Value::isFunction()       
{
    pushRefSafe(NoneMask);
    bool b = isfunction(-1);
    pop(1);
    return b;
}

bool Value::isLightUserdata()  
{
    pushRefSafe(NoneMask);
    bool b = islightuserdata(-1);
    pop(1);
    return b;
}

bool Value::isNil()
{
    pushRefSafe(NoneMask);
    bool b = isnil(-1);
    pop(1);
    return b;
}

bool Value::isNumber()
{
    pushRefSafe(NoneMask);
    bool b = isnumber(-1);
    pop(1);
    return b;
}

bool Value::isString()
{
    pushRefSafe(NoneMask);
    bool b = isstring(-1);
    pop(1);
    return b;
}

bool Value::isTable()
{ 
    pushRefSafe(NoneMask);
    bool b = istable(-1);
    pop(1);
    return b;
}

bool Value::isThread()
{ 
    pushRefSafe(NoneMask);
    bool b = isthread(-1);
    pop(1);
    return b;
}

bool Value::isUserdata()
{ 
    pushRefSafe(NoneMask);
    bool b = isuserdata(-1);
    pop(1);
    return b;
}

Value::operator bool()
{
    return toBoolean();
}

Value::operator int()
{
    return toInteger();
}

Value::operator long()
{
    return toInteger();
}

Value::operator long long()
{
    return toInteger();
}

Value::operator float()
{
    return toNumber();
}

Value::operator double()
{
    return toNumber();
}

Value::operator void*()
{
    return toUserData();
}

Value::operator const char*()
{
    return toString();
}

bool Value::operator ==(Value& value)
{
    if (!sameThreadSafe(value))
        return false;

    if (this->_c_state != value.getInterface())
        SCRIPT_EXCEPTION("Different thread");

    this->pushRefSafe(NoneMask);
    value.pushRefSafe(NoneMask);
    int result = rawequal(-2, -1);
    pop(2);
    return result == 1;
}

bool Value::operator != (Value& value)
{
    return !this->operator==(value);
}

Value& Value::operator=(Value& copy)
{
    sameThread(copy);

    releaseHandler();

    _handler = ValueHandler::ref(copy._handler);

    return *this;
}

void Value::releaseHandler()
{
    if (_handler->refCount == 1)
    {
        unref_L(Stack::REGISTRYINDEX(), _handler->value);
    }

    ValueHandler::release(_handler);

    _handler = nullptr;
}

void Value::endOfHandlerRefBuffer()
{
    if (ValueHandler::getUsingCount() != 0)
        SCRIPT_EXCEPTION("Some handler is not release");

    ValueHandler::clearFree();
}

_NAME_END