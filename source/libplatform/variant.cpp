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
#include <ray/variant.h>
#include <ray/format.h>
#include <ray/except.h>

_NAME_BEGIN

Variant::Variant()
    : _type(Void)
{
	_value.i = 0;
}

Variant::Variant(const Variant& rhs)
    : _type(Void)
{
    this->copy(rhs);
}

Variant::Variant(int rhs)
    : _type(Int)
{
	_value.i = rhs;
}

Variant::Variant(float rhs)
    : _type(Float)
{
    _value.f[0] = rhs;
}

Variant::Variant(bool rhs)
    : _type(Bool)
{
	_value.b = rhs;
}

Variant::Variant(const float4& rhs)
    : _type(Float4)
{
    _value.f[0] = rhs.x;
    _value.f[1] = rhs.y;
    _value.f[2] = rhs.z;
    _value.f[3] = rhs.w;
}

Variant::Variant(const std::string& rhs)
    :_type(String)
{
    _value.string = new std::string(rhs);
}

Variant::Variant(const char* chrPtr)
    : _type(String)
{
    _value.string = new std::string(chrPtr);
}

Variant::Variant(const void* ptr)
    : _type(Object)
{
    _value.object = ptr;
}

Variant::Variant(const float4x4& rhs)
    : _type(Float4x4)
{
    _value.matrix = new float4x4(rhs);
}

Variant::Variant(const std::vector<int>& rhs)
    : _type(IntArray)
{
    _value.intArray = new std::vector<int>(rhs);
}

Variant::Variant(const std::vector<float>& rhs)
    : _type(FloatArray)
{
    _value.floatArray = new std::vector<float>(rhs);
}

Variant::Variant(const std::vector<bool>& rhs)
    : _type(BoolArray)
{
    _value.boolArray = new std::vector<bool>(rhs);
}

Variant::Variant(const std::vector<float4>& rhs)
    : _type(Float4Array)
{
    _value.float4Array = new std::vector<float4>(rhs);
}

Variant::Variant(const std::vector<float4x4>& rhs)
    : _type(Float4x4Array)
{
    _value.float4x4Array = new std::vector<float4x4>(rhs);
}

Variant::Variant(const std::vector<std::string>& rhs)
    : _type(StringArray)
{
    _value.stringArray = new std::vector<std::string>(rhs);
}

Variant::~Variant()
{
    this->destroy();
}

void
Variant::setType(Type t)
{
    this->destroy();
    _type = t;
    switch (t)
    {
    case String:
        _value.string = new std::string;
        break;
    case Float4x4:
        _value.matrix = new float4x4;
        break;
    case Object:
        _value.object = nullptr;
        break;
    case IntArray:
        _value.intArray = new std::vector<int>;
        break;
    case FloatArray:
        _value.floatArray = new std::vector<float>;
        break;
    case BoolArray:
        _value.boolArray = new std::vector<bool>;
        break;
    case Float4Array:
        _value.float4Array = new std::vector<float4>;
        break;
    case Float4x4Array:
        _value.float4x4Array = new std::vector<float4x4>;
        break;
    case StringArray:
        _value.stringArray = new std::vector<std::string>;
        break;
    default:
        break;
    }
}

Variant::Type
Variant::getType() const
{
    return _type;
}

void
Variant::operator=(const Variant& rhs)
{
    this->destroy();
    this->copy(rhs);
}

void
Variant::operator=(int val)
{
    this->destroy();
    _type = Int;
    _value.i = val;
}

void
Variant::operator=(float val)
{
    this->destroy();
    _type = Float;
    _value.f[0] = val;
}

void
Variant::operator=(bool val)
{
    this->destroy();
    _type = Bool;
    _value.b = val;
}

void
Variant::operator=(const float4& val)
{
    this->destroy();
    _type = Float4;
    _value.f[0] = val.x;
    _value.f[1] = val.y;
    _value.f[2] = val.z;
    _value.f[3] = val.w;
}

void
Variant::operator=(const std::string& s)
{
    if (String == _type)
    {
        *_value.string = s;
    }
    else
    {
        this->destroy();
        _value.string = new std::string(s);
    }

    _type = String;
}

void
Variant::operator=(const char* chrPtr)
{
    *this = std::string(chrPtr);
}

void
Variant::operator=(const float4x4& val)
{
    if (Float4x4 == _type)
    {
        *_value.matrix = val;
    }
    else
    {
        this->destroy();
        _value.matrix = new float4x4(val);
    }
    _type = Float4x4;
}

void
Variant::operator=(const void* ptr)
{
    this->destroy();
    _type = Object;
    _value.object = ptr;
}

void
Variant::operator=(const std::vector<int>& val)
{
    if (IntArray == _type)
    {
        *_value.intArray = val;
    }
    else
    {
        this->destroy();
        _value.intArray = new std::vector<int>(val);
    }
    _type = IntArray;
}

void
Variant::operator=(const std::vector<float>& val)
{
    if (FloatArray == _type)
    {
        *_value.floatArray = val;
    }
    else
    {
        this->destroy();
        _value.floatArray = new std::vector<float>(val);
    }
    _type = FloatArray;
}

void
Variant::operator=(const std::vector<bool>& val)
{
    if (BoolArray == _type)
    {
        *_value.boolArray = val;
    }
    else
    {
        this->destroy();
        _value.boolArray = new std::vector<bool>(val);
    }
    _type = BoolArray;
}

void
Variant::operator=(const std::vector<float4>& val)
{
    if (Float4Array == _type)
    {
        *_value.float4Array = val;
    }
    else
    {
        this->destroy();
        _value.float4Array = new std::vector<float4>(val);
    }

    _type = Float4Array;
}

void
Variant::operator=(const std::vector<float4x4>& val)
{
    if (Float4x4Array == _type)
    {
        *_value.float4x4Array = val;
    }
    else
    {
        this->destroy();
        _value.float4x4Array = new std::vector<float4x4>(val);
    }
    _type = Float4x4Array;
}

void
Variant::operator=(const std::vector<std::string>& val)
{
    if (StringArray == _type)
    {
        *_value.stringArray = val;
    }
    else
    {
        this->destroy();
        _value.stringArray = new std::vector<std::string>(val);
    }
    _type = StringArray;
}

bool
Variant::operator==(const Variant& rhs) const
{
    if (rhs._type == _type)
    {
        switch (rhs._type)
        {
        case Void:
            return true;
        case Int:
            return (_value.i == rhs._value.i);
        case Bool:
            return (_value.b == rhs._value.b);
        case Float:
            return (_value.f[0] == rhs._value.f[0]);
        case String:
            return ((*_value.string) == (*rhs._value.string));
        case Float4:
            return ((_value.f[0] == rhs._value.f[0]) &&
                (_value.f[1] == rhs._value.f[1]) &&
                (_value.f[2] == rhs._value.f[2]) &&
                (_value.f[3] == rhs._value.f[3]));
        case Object:
            return (_value.object == rhs._value.object);
        case Float4x4:
            return _value.matrix == rhs._value.matrix;
        default:
            throw failure(__TEXT("Variant::operator==(): invalid variant type!"));
        }
    }
    return false;
}

bool
Variant::operator>(const Variant& rhs) const
{
    if (rhs._type == _type)
    {
        switch (rhs._type)
        {
        case Void:
            return true;
        case Int:
            return (_value.i > rhs._value.i);
        case Bool:
            return (_value.b > rhs._value.b);
        case Float:
            return (_value.f[0] > rhs._value.f[0]);
        case String:
            return ((*_value.string) > (*rhs._value.string));
        case Float4:
            return ((_value.f[0] > rhs._value.f[0]) &&
                (_value.f[1] > rhs._value.f[1]) &&
                (_value.f[2] > rhs._value.f[2]) &&
                (_value.f[3] > rhs._value.f[3]));
        case Object:
            return (_value.object > rhs._value.object);
        default:
            throw failure(__TEXT("Variant::operator>(): invalid variant type!"));
        }
    }
    return false;
}

bool
Variant::operator<(const Variant& rhs) const
{
    if (rhs._type == _type)
    {
        switch (rhs._type)
        {
        case Void:
            return true;
        case Int:
            return (_value.i < rhs._value.i);
        case Bool:
            return (_value.b < rhs._value.b);
        case Float:
            return (_value.f[0] < rhs._value.f[0]);
        case String:
            return ((*_value.string) < (*rhs._value.string));
        case Float4:
            return ((_value.f[0] < rhs._value.f[0]) &&
                (_value.f[1] < rhs._value.f[1]) &&
                (_value.f[2] < rhs._value.f[2]) &&
                (_value.f[3] < rhs._value.f[3]));
        case Object:
            return (_value.object < rhs._value.object);
        default:
            throw failure(__TEXT("Variant::operator<(): invalid variant type!"));
        }
    }
    return false;
}

bool
Variant::operator>=(const Variant& rhs) const
{
    if (rhs._type == _type)
    {
        switch (rhs._type)
        {
        case Void:
            return true;
        case Int:
            return (_value.i >= rhs._value.i);
        case Bool:
            return (_value.b >= rhs._value.b);
        case Float:
            return (_value.f[0] >= rhs._value.f[0]);
        case String:
            return ((*_value.string) >= (*rhs._value.string));
        case Float4:
            return ((_value.f[0] >= rhs._value.f[0]) &&
                (_value.f[1] >= rhs._value.f[1]) &&
                (_value.f[2] >= rhs._value.f[2]) &&
                (_value.f[3] >= rhs._value.f[3]));
        case Object:
            return (_value.object >= rhs._value.object);
        default:
            throw failure(__TEXT("Variant::operator>(): invalid variant type!"));
        }
    }

    return false;
}

bool
Variant::operator<=(const Variant& rhs) const
{
    if (rhs._type == _type)
    {
        switch (rhs._type)
        {
        case Void:
            return true;
        case Int:
            return (_value.i <= rhs._value.i);
        case Bool:
            return (_value.b <= rhs._value.b);
        case Float:
            return (_value.f[0] <= rhs._value.f[0]);
        case String:
            return ((*_value.string) <= (*rhs._value.string));
        case Float4:
            return ((_value.f[0] <= rhs._value.f[0]) &&
                (_value.f[1] <= rhs._value.f[1]) &&
                (_value.f[2] <= rhs._value.f[2]) &&
                (_value.f[3] <= rhs._value.f[3]));
        case Object:
            return (_value.object <= rhs._value.object);
        default:
            throw failure(__TEXT("Variant::operator<(): invalid variant type!"));
        }
    }
    return false;
}

bool
Variant::operator!=(const Variant& rhs) const
{
    return !(*this == rhs);
}

bool
Variant::operator==(int rhs) const
{
    assert(Int == _type);
    return (_value.i == rhs);
}

bool
Variant::operator==(float rhs) const
{
    assert(Float == _type);
    return (_value.f[0] == rhs);
}

bool
Variant::operator==(bool rhs) const
{
    assert(Bool == _type);
    return (_value.b == rhs);
}

bool
Variant::operator==(const std::string& rhs) const
{
    assert(String == _type);
    return ((*_value.string) == rhs);
}

bool
Variant::operator==(const char* chrPtr) const
{
    return *this == std::string(chrPtr);
}

bool
Variant::operator==(const float4& rhs) const
{
    assert(Float4 == _type);
    return ((_value.f[0] == rhs.x) && (_value.f[1] == rhs.y) && (_value.f[2] == rhs.z) && (_value.f[3] == rhs.w));
}

bool
Variant::operator==(const void* ptr) const
{
    assert(Object == _type);
    return _value.object == ptr;
}

bool
Variant::operator!=(int rhs) const
{
    assert(Int == _type);
    return (_value.i != rhs);
}

bool
Variant::operator!=(float rhs) const
{
    assert(Float == _type);
    return (_value.f[0] != rhs);
}

bool
Variant::operator!=(bool rhs) const
{
    assert(Bool == _type);
    return (_value.b != rhs);
}

bool
Variant::operator!=(const std::string& rhs) const
{
    assert(String == _type);
    return (*_value.string) != rhs;
}

bool
Variant::operator!=(const char* chrPtr) const
{
    return *this != std::string(chrPtr);
}

bool
Variant::operator!=(const float4& rhs) const
{
    assert(Float4 == _type);
    return ((_value.f[0] != rhs.x) || (_value.f[1] != rhs.y) || (_value.f[2] != rhs.z) || (_value.f[3] != rhs.w));
}

bool
Variant::operator!=(const void* ptr) const
{
    assert(Object == _type);
    return (_value.object == ptr);
}

void
Variant::assign(int val)
{
    this->setInt(val);
}

void
Variant::assign(float val)
{
    this->setFloat(val);
}

void
Variant::assign(bool val)
{
    this->setBool(val);
}

void
Variant::assign(const void* ptr)
{
    this->setObject(ptr);
}

void
Variant::assign(const float4& val)
{
    this->setFloat4(val);
}

void
Variant::assign(const float4x4& val)
{
    this->setFloat4x4(val);
}

void
Variant::assign(const std::string& val)
{
    this->setString(val);
}

void
Variant::assign(const std::vector<int>& val)
{
    this->setIntArray(val);
}

void
Variant::assign(const std::vector<float>& val)
{
    this->setFloatArray(val);
}

void
Variant::assign(const std::vector<bool>& val)
{
    this->setBoolArray(val);
}

void
Variant::assign(const std::vector<float4>& val)
{
    this->setFloat4Array(val);
}

void
Variant::assign(const std::vector<float4x4>& val)
{
    this->setFloat4x4Array(val);
}

void
Variant::assign(const std::vector<std::string>& val)
{
    this->setStringArray(val);
}

void
Variant::setInt(int val)
{
    *this = val;
}

int
Variant::getInt() const
{
    assert(Int == _type);
    return _value.i;
}

void
Variant::setFloat(float val)
{
    *this = val;
}

float
Variant::getFloat() const
{
    assert(Float == _type);
    return _value.f[0];
}

void
Variant::setBool(bool val)
{
    *this = val;
}

bool
Variant::getBool() const
{
    assert(Bool == _type);
    return _value.b;
}

void
Variant::setString(const std::string& val)
{
    *this = val;
}

const std::string&
Variant::getString() const
{
    assert(String == _type);
    return *(_value.string);
}

void
Variant::setFloat4(const float4& val)
{
    *this = val;
}

const float4&
Variant::getFloat4() const
{
    assert(Float4 == _type);
    return (float4&)_value.f;
}

void
Variant::setFloat4x4(const float4x4& val)
{
    *this = val;
}

const float4x4&
Variant::getFloat4x4() const
{
    assert(Float4x4 == _type);
    return *(_value.matrix);
}

void
Variant::setObject(const void* ptr)
{
    *this = ptr;
}

const void*
Variant::getObject() const
{
    assert(Object == _type);
    return _value.object;
}

void
Variant::setIntArray(const std::vector<int>& val)
{
    *this = val;
}

const std::vector<int>&
Variant::getIntArray() const
{
    assert(IntArray == _type);
    return *(_value.intArray);
}

void
Variant::setFloatArray(const std::vector<float>& val)
{
    *this = val;
}

const std::vector<float>&
Variant::getFloatArray() const
{
    assert(FloatArray == _type);
    return *(_value.floatArray);
}

void
Variant::setBoolArray(const std::vector<bool>& val)
{
    *this = val;
}

const std::vector<bool>&
Variant::getBoolArray() const
{
    assert(BoolArray == _type);
    return *(_value.boolArray);
}

void
Variant::setFloat4Array(const std::vector<float4>& val)
{
    *this = val;
}

const std::vector<float4>&
Variant::getFloat4Array() const
{
    assert(Float4Array == _type);
    return *(_value.float4Array);
}

void
Variant::setFloat4x4Array(const std::vector<float4x4>& val)
{
    *this = val;
}

const std::vector<float4x4>&
Variant::getFloat4x4Array() const
{
    assert(Float4x4Array == _type);
    return *(_value.float4x4Array);
}

void
Variant::setStringArray(const std::vector<std::string>& val)
{
    *this = val;
}

const std::vector<std::string>&
Variant::getStringArray() const
{
    assert(StringArray == _type);
    return *(_value.stringArray);
}

std::string
Variant::typeToString(Type t)
{
    switch (t)
    {
    case Void:          return "void";
    case Int:           return "int";
    case Float:         return "float";
    case Bool:          return "bool";
    case Float4:        return "float4";
    case String:        return "string";
	case Float4x4:      return "float4x4";
    case Blob:          return "blob";
    case Guid:          return "guid";
    case Object:        return "object";
    case IntArray:      return "intarray";
    case FloatArray:    return "floatarray";
    case BoolArray:     return "boolarray";
    case Float4Array:   return "float4array";
    case Float4x4Array: return "float4x4Array";
    case StringArray:   return "stringarray";
    default:
        throw failure(tformat(__TEXT("Variant::typeToString(): invalid type enum '%d'!")) % t);
    }
}

Variant::Type
Variant::stringToType(const std::string& str)
{
    if ("void" == str)             return Void;
    else if ("int" == str)              return Int;
    else if ("float" == str)            return Float;
    else if ("bool" == str)             return Bool;
    else if ("float4" == str)           return Float4;
    else if ("color" == str)            return Float4; // NOT A BUG!
    else if ("string" == str)           return String;
    else if ("Float4x4" == str)        return Float4x4;
    else if ("blob" == str)             return Blob;
    else if ("guid" == str)             return Guid;
    else if ("object" == str)           return Object;
    else if ("intarray" == str)         return IntArray;
    else if ("floatarray" == str)       return FloatArray;
    else if ("boolarray" == str)        return BoolArray;
    else if ("float4array" == str)      return Float4Array;
    else if ("Float4x4array" == str)   return Float4x4Array;
    else if ("stringarray" == str)      return StringArray;
    else
    {
        throw failure(tformat(__TEXT("Variant::stringToType(): invalid type string!")));
    }
}

void
Variant::destroy()
{
    if (String == _type)
    {
        assert(_value.string);
        delete _value.string;
        _value.string = 0;
    }
    else if (Float4x4 == _type)
    {
        assert(_value.matrix);
        delete _value.matrix;
        _value.matrix = 0;
    }
    else if (Object == _type)
    {
        if (_value.object)
        {
            //_value.object->Release();
            _value.object = 0;
        }
    }
    else if (IntArray == _type)
    {
        assert(_value.intArray);
        delete _value.intArray;
        _value.intArray = 0;
    }
    else if (FloatArray == _type)
    {
        assert(_value.floatArray);
        delete _value.floatArray;
        _value.floatArray = 0;
    }
    else if (BoolArray == _type)
    {
        assert(_value.boolArray);
        delete _value.boolArray;
        _value.boolArray = 0;
    }
    else if (Float4Array == _type)
    {
        assert(_value.float4Array);
        delete _value.float4Array;
        _value.float4Array = 0;
    }
    else if (Float4x4Array == _type)
    {
        assert(_value.float4x4Array);
        delete _value.float4x4Array;
        _value.float4x4Array = 0;
    }
    else if (StringArray == _type)
    {
        assert(_value.stringArray);
        delete _value.stringArray;
        _value.stringArray = 0;
    }

    _type = Void;
}

void
Variant::clear()
{
    this->destroy();
}

void
Variant::copy(const Variant& rhs)
{
    assert(Void == _type);
    _type = rhs._type;
    switch (rhs._type)
    {
    case Void:
        break;
    case Int:
        _value.i = rhs._value.i;
        break;
    case Float:
        _value.f[0] = rhs._value.f[0];
        break;
    case Bool:
        _value.b = rhs._value.b;
        break;
    case Float4:
        _value.f[0] = rhs._value.f[0];
        _value.f[1] = rhs._value.f[1];
        _value.f[2] = rhs._value.f[2];
        _value.f[3] = rhs._value.f[3];
        break;
    case String:
        _value.string = new std::string(*rhs._value.string);
        break;
    case Float4x4:
        _value.matrix = new float4x4(*rhs._value.matrix);
        break;
    case Object:
        _value.object = rhs._value.object;
        break;
    case IntArray:
        _value.intArray = new std::vector<int>(*rhs._value.intArray);
        break;
    case FloatArray:
        _value.floatArray = new std::vector<float>(*rhs._value.floatArray);
        break;
    case BoolArray:
        _value.boolArray = new std::vector<bool>(*rhs._value.boolArray);
        break;
    case Float4Array:
        _value.float4Array = new std::vector<float4>(*rhs._value.float4Array);
        break;
    case Float4x4Array:
        _value.float4x4Array = new std::vector<float4x4>(*rhs._value.float4x4Array);
        break;
    case StringArray:
        _value.stringArray = new std::vector<std::string>(*rhs._value.stringArray);
        break;
    default:
        throw failure(__TEXT("Variant::copy(): invalid type!"));
        break;
    }
}

_NAME_END