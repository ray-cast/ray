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
#include <ray/shader.h>

_NAME_BEGIN

ShaderVariant::ShaderVariant() noexcept
    : _type(ShaderVariantType::SPT_INT)
{
}

ShaderVariant::ShaderVariant(const std::string& name, float f) noexcept
    : _type(ShaderVariantType::SPT_INT)
{
    this->setName(name);
    this->assign(f);
}

ShaderVariant::ShaderVariant(const std::string& name, const Vector3& v) noexcept
    : _type(ShaderVariantType::SPT_INT)
{
    this->setName(name);
    this->assign(v);
}

ShaderVariant::ShaderVariant(const std::string& name, const Vector4& v) noexcept
    : _type(ShaderVariantType::SPT_INT)
{
    this->setName(name);
    this->assign(v);
}

ShaderVariant::ShaderVariant(const std::string& name, const Matrix4x4& m) noexcept
    : _type(ShaderVariantType::SPT_INT)
{
    this->setName(name);
    this->assign(m);
}

ShaderVariant::ShaderVariant(const std::string& name, ShaderVariantType type) noexcept
    : _type(ShaderVariantType::SPT_INT)
{
    this->setName(name);
    this->setType(type);
}

ShaderVariant::~ShaderVariant() noexcept
{
    this->setType(ShaderVariantType::SPT_NONE);
    this->close();
}

void
ShaderVariant::setup() noexcept
{
}

void
ShaderVariant::close() noexcept
{
}

void
ShaderVariant::setName(const std::string& name) noexcept
{
    _name = name;
}

const std::string&
ShaderVariant::getName() const noexcept
{
    return _name;
}

void
ShaderVariant::setSemantic(const std::string& semantic) noexcept
{
    _semantic = semantic;
}

const std::string&
ShaderVariant::getSemantic() const noexcept
{
    return _semantic;
}

bool
ShaderVariant::isSemantic() const noexcept
{
    return !_semantic.empty();
}

std::size_t
ShaderVariant::getSize() const noexcept
{
    if (_type != SPT_BUFFER)
    {
        switch (_type)
        {
        case SPT_BOOL:
            return sizeof(int);
        case SPT_INT:
            return sizeof(int);
        case SPT_INT2:
            return sizeof(int2);
        case SPT_FLOAT:
            return sizeof(float);
        case SPT_FLOAT2:
            return sizeof(float2);
        case SPT_FLOAT3:
            return sizeof(float3);
        case SPT_FLOAT4:
            return sizeof(float4);
        case SPT_FLOAT3X3:
            return sizeof(float3x3);
        case SPT_FLOAT4X4:
            return sizeof(float4x4);
        default:
            assert(false);
            return 0;
        }
    }
    else
    {
        std::size_t size = 0;
        for (auto& it : _params)
        {
            size += it->getSize();
        }
        return size;
    }
}

void
ShaderVariant::setType(ShaderVariantType type) noexcept
{
    if (_type != type)
    {
        if (_type == ShaderVariantType::SPT_FLOAT_ARRAY)
        {
            delete _value.farray;
            _value.farray = nullptr;
        }

        if (_type == ShaderVariantType::SPT_FLOAT3X3)
        {
            delete _value.m3;
            _value.m3 = nullptr;
        }

        if (_type == ShaderVariantType::SPT_FLOAT4X4)
        {
            delete _value.m4;
            _value.m4 = nullptr;
        }

        if (type == ShaderVariantType::SPT_FLOAT_ARRAY)
        {
            _value.farray = new std::vector<float>();
        }
        else if (type == ShaderVariantType::SPT_FLOAT3X3)
        {
            _value.m3 = new Matrix3x3;
        }
        else if (type == ShaderVariantType::SPT_FLOAT4X4)
        {
            _value.m4 = new Matrix4x4;
        }

        _type = type;
    }
}

ShaderVariantType
ShaderVariant::getType() const noexcept
{
    return _type;
}

void
ShaderVariant::assign(bool value) noexcept
{
    this->setType(ShaderVariantType::SPT_BOOL);
    if (_value.b != value)
    {
        this->onChangeBefore();
        _value.b = value;
        this->onChangeAfter();
    }
}

void
ShaderVariant::assign(int value) noexcept
{
    this->setType(ShaderVariantType::SPT_INT);
    if (_value.i[0] != value)
    {
        this->onChangeBefore();
        _value.i[0] = value;
        this->onChangeAfter();
    }
}

void
ShaderVariant::assign(const int2& value) noexcept
{
    this->setType(ShaderVariantType::SPT_INT2);
    if (_value.i[0] != value.x ||
        _value.i[1] != value.y)
    {
        this->onChangeBefore();
        _value.i[0] = value.x;
        _value.i[1] = value.y;
        this->onChangeAfter();
    }
}

void
ShaderVariant::assign(float value) noexcept
{
    this->setType(ShaderVariantType::SPT_FLOAT);
    if (_value.f[0] != value)
    {
        this->onChangeBefore();
        _value.f[0] = value;
        this->onChangeAfter();
    }
}

void
ShaderVariant::assign(const float2& value) noexcept
{
    this->setType(ShaderVariantType::SPT_FLOAT2);
    if (_value.f[0] != value.x ||
        _value.f[1] != value.y)
    {
        this->onChangeBefore();
        _value.f[0] = value.x;
        _value.f[1] = value.y;
        this->onChangeAfter();
    }
}

void
ShaderVariant::assign(const float3& value) noexcept
{
    this->setType(ShaderVariantType::SPT_FLOAT3);
    if (_value.f[0] != value.x ||
        _value.f[1] != value.y ||
        _value.f[2] != value.z)
    {
        this->onChangeBefore();
        _value.f[0] = value.x;
        _value.f[1] = value.y;
        _value.f[2] = value.z;
        this->onChangeAfter();
    }
}

void
ShaderVariant::assign(const float4& value) noexcept
{
    this->setType(ShaderVariantType::SPT_FLOAT4);
    if (_value.f[0] != value.x ||
        _value.f[1] != value.y ||
        _value.f[2] != value.z ||
        _value.f[3] != value.w)
    {
        this->onChangeBefore();
        _value.f[0] = value.x;
        _value.f[1] = value.y;
        _value.f[2] = value.z;
        _value.f[3] = value.w;
        this->onChangeAfter();
    }
}

void
ShaderVariant::assign(const float3x3& value) noexcept
{
    this->setType(ShaderVariantType::SPT_FLOAT3X3);
    this->onChangeBefore();
    *_value.m3 = value;
    this->onChangeAfter();
}

void
ShaderVariant::assign(const float4x4& value) noexcept
{
    this->setType(ShaderVariantType::SPT_FLOAT4X4);
    this->onChangeBefore();
    *_value.m4 = value;
    this->onChangeAfter();
}

void
ShaderVariant::assign(const std::vector<float>& value) noexcept
{
    this->setType(ShaderVariantType::SPT_FLOAT_ARRAY);
    this->onChangeBefore();
    *_value.farray = value;
    this->onChangeAfter();
}

void
ShaderVariant::assign(const std::vector<float2>& value) noexcept
{
    this->setType(ShaderVariantType::SPT_FLOAT2_ARRAY);
    this->onChangeBefore();
    *_value.farray2 = value;
    this->onChangeAfter();
}

void
ShaderVariant::assign(const std::vector<float3>& value) noexcept
{
    this->setType(ShaderVariantType::SPT_FLOAT3_ARRAY);
    *_value.farray3 = value;
    this->onChangeAfter();
}

void
ShaderVariant::assign(const std::vector<float4>& value) noexcept
{
    this->setType(ShaderVariantType::SPT_FLOAT4_ARRAY);
    this->onChangeBefore();
    *_value.farray4 = value;
    this->onChangeAfter();
}

void
ShaderVariant::assign(TexturePtr texture) noexcept
{
    this->setType(ShaderVariantType::SPT_TEXTURE);
    if (_texture != texture)
    {
        this->onChangeBefore();
        _texture = texture;
        this->onChangeAfter();
    }
}

const bool
ShaderVariant::getBool() const noexcept
{
    assert(_type == ShaderVariantType::SPT_BOOL);
    return _value.b;
}

const int
ShaderVariant::getInt() const noexcept
{
    assert(_type == ShaderVariantType::SPT_INT);
    return _value.i[0];
}

const int2&
ShaderVariant::getInt2() const noexcept
{
    assert(_type == ShaderVariantType::SPT_INT2);
    return (int2&)_value.i[0];
}

const float
ShaderVariant::getFloat() const noexcept
{
    assert(_type == ShaderVariantType::SPT_FLOAT);
    return _value.f[0];
}

const float2&
ShaderVariant::getFloat2() const noexcept
{
    assert(_type == ShaderVariantType::SPT_FLOAT2);
    return (float2&)_value.f[0];
}

const float3&
ShaderVariant::getFloat3() const noexcept
{
    assert(_type == ShaderVariantType::SPT_FLOAT3);
    return (float3&)_value.f[0];
}

const float4&
ShaderVariant::getFloat4() const noexcept
{
    assert(_type == ShaderVariantType::SPT_FLOAT4);
    return (float4&)_value.f[0];
}

const float3x3&
ShaderVariant::getFloat3x3() const noexcept
{
    assert(_type == ShaderVariantType::SPT_FLOAT3X3);
    return (float3x3&)*_value.m3;
}

const float4x4&
ShaderVariant::getFloat4x4() const noexcept
{
    assert(_type == ShaderVariantType::SPT_FLOAT4X4);
    return (float4x4&)*_value.m4;
}

const std::vector<float>&
ShaderVariant::getFloatArray() const noexcept
{
    assert(_type == ShaderVariantType::SPT_FLOAT_ARRAY);
    return *_value.farray;
}

const std::vector<float2>&
ShaderVariant::getFloat2Array() const noexcept
{
    assert(_type == ShaderVariantType::SPT_FLOAT2_ARRAY);
    return *_value.farray2;
}

const std::vector<float3>&
ShaderVariant::getFloat3Array() const noexcept
{
    assert(_type == ShaderVariantType::SPT_FLOAT3_ARRAY);
    return *_value.farray3;
}

const std::vector<float4>&
ShaderVariant::getFloat4Array() const noexcept
{
    assert(_type == ShaderVariantType::SPT_FLOAT4_ARRAY);
    return *_value.farray4;
}

TexturePtr
ShaderVariant::getTexture() const noexcept
{
    return _texture;
}

void
ShaderVariant::addParameter(ShaderVariantPtr arg) noexcept
{
    assert(std::find(_params.begin(), _params.end(), arg) == _params.end());
    _params.push_back(arg);
}

void
ShaderVariant::removeParameter(ShaderVariantPtr arg) noexcept
{
    auto it = std::find(_params.begin(), _params.end(), arg);
    if (it != _params.end())
    {
        _params.erase(it);
    }
}

ShaderVariantPtr
ShaderVariant::getParameter(const std::string& name) const noexcept
{
    for (auto& it : _params)
    {
        if (it->getName() == name)
        {
            return it;
        }
    }

    return nullptr;
}

const ShaderVariants&
ShaderVariant::getParameters() const noexcept
{
    return _params;
}

void
ShaderVariant::onChangeBefore() noexcept
{
}

void
ShaderVariant::onChangeAfter() noexcept
{
}

ShaderParameter::ShaderParameter() noexcept
    : _needUpdate(true)
    , _location(0)
    , _bindingPoint(0)
    , _bindingProgram(0)
{
}

ShaderParameter::~ShaderParameter() noexcept
{
}

void
ShaderParameter::setName(const std::string& name) noexcept
{
    _name = name;
}

const std::string&
ShaderParameter::getName() const noexcept
{
    return _name;
}

void
ShaderParameter::setLocation(std::size_t location) noexcept
{
    _location = location;
}

std::size_t
ShaderParameter::getLocation() const noexcept
{
    return _location;
}

void
ShaderParameter::setBindingPoint(std::size_t unit) noexcept
{
    _bindingPoint = unit;
}

std::size_t
ShaderParameter::getBindingPoint() const noexcept
{
    return _bindingPoint;
}

void
ShaderParameter::setBindingProgram(std::size_t program) noexcept
{
    _bindingProgram = program;
}

std::size_t
ShaderParameter::getBindingProgram() const noexcept
{
    return _bindingProgram;
}

void
ShaderParameter::needUpdate(bool update) noexcept
{
    _needUpdate = update;
}

bool
ShaderParameter::needUpdate() const noexcept
{
    return _needUpdate;
}

ShaderUniform::ShaderUniform() noexcept
    : _type(ShaderVariantType::SPT_NONE)
{
}

ShaderUniform::~ShaderUniform() noexcept
{
}

void
ShaderUniform::setType(ShaderVariantType type) noexcept
{
    _type = type;
}

ShaderVariantType
ShaderUniform::getType() const noexcept
{
    return _type;
}

void
ShaderUniform::setValue(ShaderVariantPtr value) noexcept
{
    if (value)
    {
        assert(_type == value->getType());
    }
    _value = value;
}

ShaderVariantPtr
ShaderUniform::getValue() const noexcept
{
    return _value;
}

Shader::Shader() noexcept
{
}

Shader::Shader(ShaderType type, const std::string& code) noexcept
{
    this->setType(type);
    this->setSource(code);
}

Shader::Shader(const std::string& type, const std::string& code) noexcept
{
    this->setType(type);
    this->setSource(code);
}

Shader::Shader(const std::wstring& type, const std::string& code) noexcept
{
    this->setType(type);
    this->setSource(code);
}

Shader::~Shader() noexcept
{
}

void
Shader::setType(ShaderType type) noexcept
{
    _type = type;
}

void
Shader::setType(const std::string& type) noexcept
{
    if (type == "vertex") { _type = ShaderType::ST_VERTEX; return; }
    if (type == "fragment") { _type = ShaderType::ST_FRAGMENT; return; }
    if (type == "geometry") { _type = ShaderType::ST_GEOMETRY; return; }

    _type = ShaderType::ST_VERTEX;
    assert(false);
}

void
Shader::setType(const std::wstring& type) noexcept
{
    if (type == L"vertex") { _type = ShaderType::ST_VERTEX; return; }
    if (type == L"fragment") { _type = ShaderType::ST_FRAGMENT; return; }
    if (type == L"geometry") { _type = ShaderType::ST_GEOMETRY; return; }

    _type = ShaderType::ST_VERTEX;
    assert(false);
}

void
Shader::setSource(const std::string& source) noexcept
{
    _source = source;
}

ShaderType
Shader::getType()const noexcept
{
    return _type;
}

const std::string&
Shader::getSource() const noexcept
{
    return _source;
}

ShaderObject::ShaderObject() noexcept
{
}

ShaderObject::~ShaderObject() noexcept
{
}

_NAME_END