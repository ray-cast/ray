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
    : type(Void)
    , string(0)
{
}

Variant::Variant(const Variant& rhs)
    : type(Void)
{
    this->copy(rhs);
}

Variant::Variant(int rhs)
    : type(Int),
    i(rhs)
{
}

Variant::Variant(float rhs)
    : type(Float)
{
    this->f[0] = rhs;
}

Variant::Variant(bool rhs)
    : type(Bool)
    , b(rhs)
{
}

Variant::Variant(const float4& rhs)
    : type(Float4)
{
    this->f[0] = rhs.x;
    this->f[1] = rhs.y;
    this->f[2] = rhs.z;
    this->f[3] = rhs.w;
}

Variant::Variant(const std::string& rhs)
    :type(String)
{
    this->string = new std::string(rhs);
}

Variant::Variant(const char* chrPtr)
    : type(String)
{
    this->string = new std::string(chrPtr);
}

Variant::Variant(const void* ptr)
    : type(Object)
{
    this->object = ptr;
}

Variant::Variant(const Matrix4x4& rhs)
    : type(Mat4)
{
    this->m = new Matrix4x4(rhs);
}

Variant::Variant(const std::vector<int>& rhs)
    : type(IntArray)
{
    this->intArray = new std::vector<int>(rhs);
}

Variant::Variant(const std::vector<float>& rhs)
    : type(FloatArray)
{
    this->floatArray = new std::vector<float>(rhs);
}

Variant::Variant(const std::vector<bool>& rhs)
    : type(BoolArray)
{
    this->boolArray = new std::vector<bool>(rhs);
}

Variant::Variant(const std::vector<float4>& rhs)
    : type(Float4Array)
{
    this->float4Array = new std::vector<float4>(rhs);
}

Variant::Variant(const std::vector<Matrix4x4>& rhs)
    : type(Mat4Array)
{
    this->Matrix4x4Array = new std::vector<Matrix4x4>(rhs);
}

Variant::Variant(const std::vector<std::string>& rhs)
    : type(StringArray)
{
    this->stringArray = new std::vector<std::string>(rhs);
}

Variant::~Variant()
{
    this->destroy();
}

void
Variant::setType(Type t)
{
    this->destroy();
    this->type = t;
    switch (t)
    {
    case String:
        this->string = new std::string;
        break;
    case Mat4:
        this->m = new Matrix4x4;
        break;
    case Object:
        this->object = 0;
        break;
    case IntArray:
        this->intArray = new std::vector<int>;
        break;
    case FloatArray:
        this->floatArray = new std::vector<float>;
        break;
    case BoolArray:
        this->boolArray = new std::vector<bool>;
        break;
    case Float4Array:
        this->float4Array = new std::vector<float4>;
        break;
    case Mat4Array:
        this->Matrix4x4Array = new std::vector<Matrix4x4>;
        break;
    case StringArray:
        this->stringArray = new std::vector<std::string>;
        break;
    default:
        break;
    }
}

Variant::Type
Variant::getType() const
{
    return this->type;
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
    this->type = Int;
    this->i = val;
}

void
Variant::operator=(float val)
{
    this->destroy();
    this->type = Float;
    this->f[0] = val;
}

void
Variant::operator=(bool val)
{
    this->destroy();
    this->type = Bool;
    this->b = val;
}

void
Variant::operator=(const float4& val)
{
    this->destroy();
    this->type = Float4;
    this->f[0] = val.x;
    this->f[1] = val.y;
    this->f[2] = val.z;
    this->f[3] = val.w;
}

void
Variant::operator=(const std::string& s)
{
    if (String == this->type)
    {
        *this->string = s;
    }
    else
    {
        this->destroy();
        this->string = new std::string(s);
    }

    this->type = String;
}

void
Variant::operator=(const char* chrPtr)
{
    *this = std::string(chrPtr);
}

void
Variant::operator=(const Matrix4x4& val)
{
    if (Mat4 == this->type)
    {
        *this->m = val;
    }
    else
    {
        this->destroy();
        this->m = new Matrix4x4(val);
    }
    this->type = Mat4;
}

void
Variant::operator=(const void* ptr)
{
    this->destroy();
    this->type = Object;
    this->object = ptr;
}

void
Variant::operator=(const std::vector<int>& val)
{
    if (IntArray == this->type)
    {
        *this->intArray = val;
    }
    else
    {
        this->destroy();
        this->intArray = new std::vector<int>(val);
    }
    this->type = IntArray;
}

void
Variant::operator=(const std::vector<float>& val)
{
    if (FloatArray == this->type)
    {
        *this->floatArray = val;
    }
    else
    {
        this->destroy();
        this->floatArray = new std::vector<float>(val);
    }
    this->type = FloatArray;
}

void
Variant::operator=(const std::vector<bool>& val)
{
    if (BoolArray == this->type)
    {
        *this->boolArray = val;
    }
    else
    {
        this->destroy();
        this->boolArray = new std::vector<bool>(val);
    }
    this->type = BoolArray;
}

void
Variant::operator=(const std::vector<float4>& val)
{
    if (Float4Array == this->type)
    {
        *this->float4Array = val;
    }
    else
    {
        this->destroy();
        this->float4Array = new std::vector<float4>(val);
    }

    this->type = Float4Array;
}

void
Variant::operator=(const std::vector<Matrix4x4>& val)
{
    if (Mat4Array == this->type)
    {
        *this->Matrix4x4Array = val;
    }
    else
    {
        this->destroy();
        this->Matrix4x4Array = new std::vector<Matrix4x4>(val);
    }
    this->type = Mat4Array;
}

void
Variant::operator=(const std::vector<std::string>& val)
{
    if (StringArray == this->type)
    {
        *this->stringArray = val;
    }
    else
    {
        this->destroy();
        this->stringArray = new std::vector<std::string>(val);
    }
    this->type = StringArray;
}

bool
Variant::operator==(const Variant& rhs) const
{
    if (rhs.type == this->type)
    {
        switch (rhs.type)
        {
        case Void:
            return true;
        case Int:
            return (this->i == rhs.i);
        case Bool:
            return (this->b == rhs.b);
        case Float:
            return (this->f[0] == rhs.f[0]);
        case String:
            return ((*this->string) == (*rhs.string));
        case Float4:
            return ((this->f[0] == rhs.f[0]) &&
                (this->f[1] == rhs.f[1]) &&
                (this->f[2] == rhs.f[2]) &&
                (this->f[3] == rhs.f[3]));
        case Object:
            return (this->object == rhs.object);
        case Mat4:
            return this->m == rhs.m;
        default:
            throw failure(__TEXT("Variant::operator==(): invalid variant type!"));
        }
    }
    return false;
}

bool
Variant::operator>(const Variant& rhs) const
{
    if (rhs.type == this->type)
    {
        switch (rhs.type)
        {
        case Void:
            return true;
        case Int:
            return (this->i > rhs.i);
        case Bool:
            return (this->b > rhs.b);
        case Float:
            return (this->f[0] > rhs.f[0]);
        case String:
            return ((*this->string) > (*rhs.string));
        case Float4:
            return ((this->f[0] > rhs.f[0]) &&
                (this->f[1] > rhs.f[1]) &&
                (this->f[2] > rhs.f[2]) &&
                (this->f[3] > rhs.f[3]));
        case Object:
            return (this->object > rhs.object);
        default:
            throw failure(__TEXT("Variant::operator>(): invalid variant type!"));
        }
    }
    return false;
}

bool
Variant::operator<(const Variant& rhs) const
{
    if (rhs.type == this->type)
    {
        switch (rhs.type)
        {
        case Void:
            return true;
        case Int:
            return (this->i < rhs.i);
        case Bool:
            return (this->b < rhs.b);
        case Float:
            return (this->f[0] < rhs.f[0]);
        case String:
            return ((*this->string) < (*rhs.string));
        case Float4:
            return ((this->f[0] < rhs.f[0]) &&
                (this->f[1] < rhs.f[1]) &&
                (this->f[2] < rhs.f[2]) &&
                (this->f[3] < rhs.f[3]));
        case Object:
            return (this->object < rhs.object);
        default:
            throw failure(__TEXT("Variant::operator<(): invalid variant type!"));
        }
    }
    return false;
}

bool
Variant::operator>=(const Variant& rhs) const
{
    if (rhs.type == this->type)
    {
        switch (rhs.type)
        {
        case Void:
            return true;
        case Int:
            return (this->i >= rhs.i);
        case Bool:
            return (this->b >= rhs.b);
        case Float:
            return (this->f[0] >= rhs.f[0]);
        case String:
            return ((*this->string) >= (*rhs.string));
        case Float4:
            return ((this->f[0] >= rhs.f[0]) &&
                (this->f[1] >= rhs.f[1]) &&
                (this->f[2] >= rhs.f[2]) &&
                (this->f[3] >= rhs.f[3]));
        case Object:
            return (this->object >= rhs.object);
        default:
            throw failure(__TEXT("Variant::operator>(): invalid variant type!"));
        }
    }

    return false;
}

bool
Variant::operator<=(const Variant& rhs) const
{
    if (rhs.type == this->type)
    {
        switch (rhs.type)
        {
        case Void:
            return true;
        case Int:
            return (this->i <= rhs.i);
        case Bool:
            return (this->b <= rhs.b);
        case Float:
            return (this->f[0] <= rhs.f[0]);
        case String:
            return ((*this->string) <= (*rhs.string));
        case Float4:
            return ((this->f[0] <= rhs.f[0]) &&
                (this->f[1] <= rhs.f[1]) &&
                (this->f[2] <= rhs.f[2]) &&
                (this->f[3] <= rhs.f[3]));
        case Object:
            return (this->object <= rhs.object);
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
    assert(Int == this->type);
    return (this->i == rhs);
}

bool
Variant::operator==(float rhs) const
{
    assert(Float == this->type);
    return (this->f[0] == rhs);
}

bool
Variant::operator==(bool rhs) const
{
    assert(Bool == this->type);
    return (this->b == rhs);
}

bool
Variant::operator==(const std::string& rhs) const
{
    assert(String == this->type);
    return ((*this->string) == rhs);
}

bool
Variant::operator==(const char* chrPtr) const
{
    return *this == std::string(chrPtr);
}

bool
Variant::operator==(const float4& rhs) const
{
    assert(Float4 == this->type);
    return ((this->f[0] == rhs.x) && (this->f[1] == rhs.y) && (this->f[2] == rhs.z) && (this->f[3] == rhs.w));
}

bool
Variant::operator==(const void* ptr) const
{
    assert(Object == this->type);
    return this->object == ptr;
}

bool
Variant::operator!=(int rhs) const
{
    assert(Int == this->type);
    return (this->i != rhs);
}

bool
Variant::operator!=(float rhs) const
{
    assert(Float == this->type);
    return (this->f[0] != rhs);
}

bool
Variant::operator!=(bool rhs) const
{
    assert(Bool == this->type);
    return (this->b != rhs);
}

bool
Variant::operator!=(const std::string& rhs) const
{
    assert(String == this->type);
    return (*this->string) != rhs;
}

bool
Variant::operator!=(const char* chrPtr) const
{
    return *this != std::string(chrPtr);
}

bool
Variant::operator!=(const float4& rhs) const
{
    assert(Float4 == this->type);
    return ((this->f[0] != rhs.x) || (this->f[1] != rhs.y) || (this->f[2] != rhs.z) || (this->f[3] != rhs.w));
}

bool
Variant::operator!=(const void* ptr) const
{
    assert(Object == this->type);
    return (this->object == ptr);
}

void
Variant::setInt(int val)
{
    *this = val;
}

int
Variant::getInt() const
{
    assert(Int == this->type);
    return this->i;
}

void
Variant::setFloat(float val)
{
    *this = val;
}

float
Variant::getFloat() const
{
    assert(Float == this->type);
    return this->f[0];
}

void
Variant::setBool(bool val)
{
    *this = val;
}

bool
Variant::getBool() const
{
    assert(Bool == this->type);
    return this->b;
}

void
Variant::setString(const std::string& val)
{
    *this = val;
}

const std::string&
Variant::getString() const
{
    assert(String == this->type);
    return *(this->string);
}

void
Variant::setFloat4(const float4& val)
{
    *this = val;
}

float4
Variant::getFloat4() const
{
    assert(Float4 == this->type);
    return float4(this->f[0], this->f[1], this->f[2], this->f[3]);
}

void
Variant::setFloat4_X(float val)
{
    this->f[0] = val;
}

float
Variant::getFloat4_X() const
{
    assert(Float4 == this->type);
    return this->f[0];
}

void
Variant::setFloat4_Y(float val)
{
    this->f[1] = val;
}

float
Variant::getFloat4_Y() const
{
    assert(Float4 == this->type);
    return this->f[1];
}

void
Variant::setFloat4_Z(float val)
{
    this->f[2] = val;
}

float
Variant::getFloat4_Z() const
{
    assert(Float4 == this->type);
    return this->f[2];
}

void
Variant::setFloat4_W(float val)
{
    this->f[3] = val;
}

float
Variant::getFloat4_W() const
{
    assert(Float4 == this->type);
    return this->f[3];
}

void
Variant::setMatrix4x4(const Matrix4x4& val)
{
    *this = val;
}

const Matrix4x4&
Variant::getMatrix4x4() const
{
    assert(Mat4 == this->type);
    return *(this->m);
}

void
Variant::setObject(const void* ptr)
{
    *this = ptr;
}

const void*
Variant::getObject() const
{
    assert(Object == this->type);
    return this->object;
}

void
Variant::setIntArray(const std::vector<int>& val)
{
    *this = val;
}

const std::vector<int>&
Variant::getIntArray() const
{
    assert(IntArray == this->type);
    return *(this->intArray);
}

void
Variant::setFloatArray(const std::vector<float>& val)
{
    *this = val;
}

const std::vector<float>&
Variant::getFloatArray() const
{
    assert(FloatArray == this->type);
    return *(this->floatArray);
}

void
Variant::setBoolArray(const std::vector<bool>& val)
{
    *this = val;
}

const std::vector<bool>&
Variant::getBoolArray() const
{
    assert(BoolArray == this->type);
    return *(this->boolArray);
}

void
Variant::setFloat4Array(const std::vector<float4>& val)
{
    *this = val;
}

const std::vector<float4>&
Variant::getFloat4Array() const
{
    assert(Float4Array == this->type);
    return *(this->float4Array);
}

void
Variant::setMatrix4x4Array(const std::vector<Matrix4x4>& val)
{
    *this = val;
}

const std::vector<Matrix4x4>&
Variant::getMatrix4x4Array() const
{
    assert(Mat4Array == this->type);
    return *(this->Matrix4x4Array);
}

void
Variant::setStringArray(const std::vector<std::string>& val)
{
    *this = val;
}

const std::vector<std::string>&
Variant::getStringArray() const
{
    assert(StringArray == this->type);
    return *(this->stringArray);
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
    case Mat4:          return "Matrix4x4";
    case Blob:          return "blob";
    case Guid:          return "guid";
    case Object:        return "object";
    case IntArray:      return "intarray";
    case FloatArray:    return "floatarray";
    case BoolArray:     return "boolarray";
    case Float4Array:   return "float4array";
    case Mat4Array:     return "Matrix4x4array";
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
    else if ("Matrix4x4" == str)        return Mat4;
    else if ("blob" == str)             return Blob;
    else if ("guid" == str)             return Guid;
    else if ("object" == str)           return Object;
    else if ("intarray" == str)         return IntArray;
    else if ("floatarray" == str)       return FloatArray;
    else if ("boolarray" == str)        return BoolArray;
    else if ("float4array" == str)      return Float4Array;
    else if ("Matrix4x4array" == str)   return Mat4Array;
    else if ("stringarray" == str)      return StringArray;
    else
    {
        throw failure(tformat(__TEXT("Variant::stringToType(): invalid type string!")));
    }
}

void
Variant::destroy()
{
    if (String == this->type)
    {
        assert(this->string);
        delete this->string;
        this->string = 0;
    }
    else if (Mat4 == this->type)
    {
        assert(this->m);
        delete this->m;
        this->m = 0;
    }
    else if (Object == this->type)
    {
        if (this->object)
        {
            //this->object->Release();
            this->object = 0;
        }
    }
    else if (IntArray == this->type)
    {
        assert(this->intArray);
        delete this->intArray;
        this->intArray = 0;
    }
    else if (FloatArray == this->type)
    {
        assert(this->floatArray);
        delete this->floatArray;
        this->floatArray = 0;
    }
    else if (BoolArray == this->type)
    {
        assert(this->boolArray);
        delete this->boolArray;
        this->boolArray = 0;
    }
    else if (Float4Array == this->type)
    {
        assert(this->float4Array);
        delete this->float4Array;
        this->float4Array = 0;
    }
    else if (Mat4Array == this->type)
    {
        assert(this->Matrix4x4Array);
        delete this->Matrix4x4Array;
        this->Matrix4x4Array = 0;
    }
    else if (StringArray == this->type)
    {
        assert(this->stringArray);
        delete this->stringArray;
        this->stringArray = 0;
    }

    this->type = Void;
}

void
Variant::clear()
{
    this->destroy();
}

void
Variant::copy(const Variant& rhs)
{
    assert(Void == this->type);
    this->type = rhs.type;
    switch (rhs.type)
    {
    case Void:
        break;
    case Int:
        this->i = rhs.i;
        break;
    case Float:
        this->f[0] = rhs.f[0];
        break;
    case Bool:
        this->b = rhs.b;
        break;
    case Float4:
        this->f[0] = rhs.f[0];
        this->f[1] = rhs.f[1];
        this->f[2] = rhs.f[2];
        this->f[3] = rhs.f[3];
        break;
    case String:
        this->string = new std::string(*rhs.string);
        break;
    case Mat4:
        this->m = new Matrix4x4(*rhs.m);
        break;
    case Object:
        this->object = rhs.object;
        break;
    case IntArray:
        this->intArray = new std::vector<int>(*rhs.intArray);
        break;
    case FloatArray:
        this->floatArray = new std::vector<float>(*rhs.floatArray);
        break;
    case BoolArray:
        this->boolArray = new std::vector<bool>(*rhs.boolArray);
        break;
    case Float4Array:
        this->float4Array = new std::vector<float4>(*rhs.float4Array);
        break;
    case Mat4Array:
        this->Matrix4x4Array = new std::vector<Matrix4x4>(*rhs.Matrix4x4Array);
        break;
    case StringArray:
        this->stringArray = new std::vector<std::string>(*rhs.stringArray);
        break;
    default:
        throw failure(__TEXT("Variant::copy(): invalid type!"));
        break;
    }
}

_NAME_END