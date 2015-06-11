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
#include <ray/render_impl.h>

_NAME_BEGIN

ShaderParam::ShaderParam() noexcept
    : _type(ShaderParamType::SPT_INT)
{
}

ShaderParam::ShaderParam(const std::string& name, float f) noexcept
    : _type(ShaderParamType::SPT_INT)
{
    this->setName(name);
    this->assign(f);
}

ShaderParam::ShaderParam(const std::string& name, const Vector3& v) noexcept
    : _type(ShaderParamType::SPT_INT)
{
    this->setName(name);
    this->assign(v);
}

ShaderParam::ShaderParam(const std::string& name, const Vector4& v) noexcept
    : _type(ShaderParamType::SPT_INT)
{
    this->setName(name);
    this->assign(v);
}

ShaderParam::ShaderParam(const std::string& name, const Matrix4x4& m) noexcept
    : _type(ShaderParamType::SPT_INT)
{
    this->setName(name);
    this->assign(m);
}

ShaderParam::ShaderParam(const std::string& name, ShaderParamType type) noexcept
    : _type(ShaderParamType::SPT_INT)
{
    this->setName(name);
    this->setType(type);
}

ShaderParam::~ShaderParam() noexcept
{
    this->setType(ShaderParamType::SPT_NONE);
}

void
ShaderParam::setName(const std::string& name) noexcept
{
    _name = name;
}

const std::string&
ShaderParam::getName() const noexcept
{
    return _name;
}

void
ShaderParam::setSemantic(const std::string& semantic) noexcept
{
    _semantic = semantic;
}

const std::string&
ShaderParam::getSemantic() const noexcept
{
    return _semantic;
}

void
ShaderParam::setType(ShaderParamType type) noexcept
{
    if (_type != type)
    {
        if (_type == ShaderParamType::SPT_FLOAT_ARRAY)
        {
            delete _value.farray;
            _value.farray = nullptr;
        }

        if (_type == ShaderParamType::SPT_FLOAT3X3)
        {
            delete _value.m3;
            _value.m3 = nullptr;
        }

        if (_type == ShaderParamType::SPT_FLOAT4X4)
        {
            delete _value.m4;
            _value.m4 = nullptr;
        }

        if (type == ShaderParamType::SPT_FLOAT_ARRAY)
        {
            _value.farray = new std::vector<float>();
        }
        else if (type == ShaderParamType::SPT_FLOAT3X3)
        {
            _value.m3 = new Matrix3x3;
        }
        else if (type == ShaderParamType::SPT_FLOAT4X4)
        {
            _value.m4 = new Matrix4x4;
        }

        _type = type;
    }
}

ShaderParamType
ShaderParam::getType() const noexcept
{
    return _type;
}

void
ShaderParam::assign(bool value) noexcept
{
    this->setType(ShaderParamType::SPT_BOOL);
    if (_value.b != value)
    {
        this->onChangeBefore();
        _value.b = value;
        this->onChangeAfter();
    }
}

void
ShaderParam::assign(int value) noexcept
{
    this->setType(ShaderParamType::SPT_INT);
    if (_value.i[0] != value)
    {
        this->onChangeBefore();
        _value.i[0] = value;
        this->onChangeAfter();
    }
}

void
ShaderParam::assign(const int2& value) noexcept
{
    this->setType(ShaderParamType::SPT_INT2);
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
ShaderParam::assign(float value) noexcept
{
    this->setType(ShaderParamType::SPT_FLOAT);
    if (_value.f[0] != value)
    {
        this->onChangeBefore();
        _value.f[0] = value;
        this->onChangeAfter();
    }
}

void
ShaderParam::assign(const float2& value) noexcept
{
    this->setType(ShaderParamType::SPT_FLOAT2);
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
ShaderParam::assign(const float3& value) noexcept
{
    this->setType(ShaderParamType::SPT_FLOAT3);
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
ShaderParam::assign(const float4& value) noexcept
{
    this->setType(ShaderParamType::SPT_FLOAT4);
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
ShaderParam::assign(const float3x3& value) noexcept
{
    this->setType(ShaderParamType::SPT_FLOAT3X3);
    this->onChangeBefore();
    *_value.m3 = value;
    this->onChangeAfter();
}

void
ShaderParam::assign(const float4x4& value) noexcept
{
    this->setType(ShaderParamType::SPT_FLOAT4X4);
    this->onChangeBefore();
    *_value.m4 = value;
    this->onChangeAfter();
}

void
ShaderParam::assign(const std::vector<float>& value) noexcept
{
    this->setType(ShaderParamType::SPT_FLOAT_ARRAY);
    this->onChangeBefore();
    *_value.farray = value;
    this->onChangeAfter();
}

void
ShaderParam::assign(const std::vector<float2>& value) noexcept
{
    this->setType(ShaderParamType::SPT_FLOAT2_ARRAY);
    this->onChangeBefore();
    *_value.farray2 = value;
    this->onChangeAfter();
}

void
ShaderParam::assign(const std::vector<float3>& value) noexcept
{
    this->setType(ShaderParamType::SPT_FLOAT3_ARRAY);
    this->onChangeBefore();
    *_value.farray3 = value;
    this->onChangeAfter();
}

void
ShaderParam::assign(const std::vector<float4>& value) noexcept
{
    this->setType(ShaderParamType::SPT_FLOAT4_ARRAY);
    this->onChangeBefore();
    *_value.farray4 = value;
    this->onChangeAfter();
}

void
ShaderParam::assign(TexturePtr texture) noexcept
{
    this->setType(ShaderParamType::SPT_TEXTURE);
    if (_texture != texture)
    {
        this->onChangeBefore();
        _texture = texture;
        this->onChangeAfter();
    }
}

const bool
ShaderParam::getBool() const noexcept
{
    assert(_type == ShaderParamType::SPT_BOOL);
    return _value.b;
}

const int
ShaderParam::getInt() const noexcept
{
    assert(_type == ShaderParamType::SPT_INT);
    return _value.i[0];
}

const int2&
ShaderParam::getInt2() const noexcept
{
    assert(_type == ShaderParamType::SPT_INT2);
    return (int2&)_value.i[0];
}

const float
ShaderParam::getFloat() const noexcept
{
    assert(_type == ShaderParamType::SPT_FLOAT);
    return _value.f[0];
}

const float2&
ShaderParam::getFloat2() const noexcept
{
    assert(_type == ShaderParamType::SPT_FLOAT2);
    return (float2&)_value.f[0];
}

const float3&
ShaderParam::getFloat3() const noexcept
{
    assert(_type == ShaderParamType::SPT_FLOAT3);
    return (float3&)_value.f[0];
}

const float4&
ShaderParam::getFloat4() const noexcept
{
    assert(_type == ShaderParamType::SPT_FLOAT4);
    return (float4&)_value.f[0];
}

const float3x3&
ShaderParam::getFloat3x3() const noexcept
{
    assert(_type == ShaderParamType::SPT_FLOAT3X3);
    return (float3x3&)*_value.m3;
}

const float4x4&
ShaderParam::getFloat4x4() const noexcept
{
    assert(_type == ShaderParamType::SPT_FLOAT4X4);
    return (float4x4&)*_value.m4;
}

const std::vector<float>&
ShaderParam::getFloatArray() const noexcept
{
    assert(_type == ShaderParamType::SPT_FLOAT_ARRAY);
    return *_value.farray;
}

const std::vector<float2>&
ShaderParam::getFloat2Array() const noexcept
{
    assert(_type == ShaderParamType::SPT_FLOAT2_ARRAY);
    return *_value.farray2;
}

const std::vector<float3>&
ShaderParam::getFloat3Array() const noexcept
{
    assert(_type == ShaderParamType::SPT_FLOAT3_ARRAY);
    return *_value.farray3;
}

const std::vector<float4>&
ShaderParam::getFloat4Array() const noexcept
{
    assert(_type == ShaderParamType::SPT_FLOAT4_ARRAY);
    return *_value.farray4;
}

TexturePtr
ShaderParam::getTexture() const noexcept
{
    return _texture;
}

void
ShaderParam::onChangeBefore() noexcept
{
}

void
ShaderParam::onChangeAfter() noexcept
{
}

ShaderUniform::ShaderUniform() noexcept
    :_needUpdate(true)
{
}

ShaderUniform::~ShaderUniform()
{
}

void
ShaderUniform::needUpdate(bool update) noexcept
{
    _needUpdate = update;
}

bool
ShaderUniform::needUpdate() const noexcept
{
    return _needUpdate;
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

void
Shader::setIncludePath(const std::string& include) noexcept
{
    _path = include;
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

const std::string&
Shader::getIncludePath() const noexcept
{
    return _path;
}

ShaderProgram::ShaderProgram() noexcept
{
}

ShaderProgram::~ShaderProgram() noexcept
{
}

void
ShaderProgram::addShader(ShaderPtr shader) noexcept
{
    assert(std::find(_shaders.begin(), _shaders.end(), shader) == _shaders.end());
    _shaders.push_back(shader);
}

void
ShaderProgram::removeShader(ShaderPtr shader) noexcept
{
    auto it = std::find(_shaders.begin(), _shaders.end(), shader);
    if (it != _shaders.end())
    {
        _shaders.erase(it);
    }
}

Shaders&
ShaderProgram::getShaders() noexcept
{
    return _shaders;
}

ShaderAttributes&
ShaderProgram::getActiveAttributes() noexcept
{
    return _activeAttributes;
}

ShaderUniforms&
ShaderProgram::getActiveUniforms() noexcept
{
    return _activeUniforms;
}

ShaderUniformBlocks&
ShaderProgram::getActiveUniformBlocks() noexcept
{
    return _activeUniformBlocks;
}

ShaderSubroutines&
ShaderProgram::getActiveSubroutines() noexcept
{
    return _activeSubroutines;
}

ShaderConstantBuffer::ShaderConstantBuffer() noexcept
{
}

ShaderConstantBuffer::~ShaderConstantBuffer() noexcept
{
}

void
ShaderConstantBuffer::addParamArg(ShaderUniformPtr& arg) noexcept
{
    _uniforms.push_back(arg);
}

void
ShaderConstantBuffer::removeParamArg(ShaderUniformPtr& arg) noexcept
{
}

const ShaderUniforms&
ShaderConstantBuffer::getShaderParamArgs() const noexcept
{
    return _uniforms;
}

ShaderConstantBufferPtr
ShaderConstantBuffer::clone() const noexcept
{
    return std::make_shared<ShaderConstantBuffer>();
}

ShaderObject::ShaderObject() noexcept
{
}

ShaderObject::~ShaderObject() noexcept
{
    this->close();
}

bool
ShaderObject::setup() noexcept
{
    _program = RenderImpl::instance()->createShaderProgram(_shaders);
    return true;
}

void
ShaderObject::close() noexcept
{
    if (_program)
    {
        RenderImpl::instance()->destroyShaderProgram(_program);
        _program = nullptr;
    }
}

void
ShaderObject::addShader(ShaderPtr shader) noexcept
{
    _shaders.push_back(shader);
}

void
ShaderObject::removeShader(ShaderPtr shader) noexcept
{
    auto it = std::find(_shaders.begin(), _shaders.end(), shader);
    if (it != _shaders.end())
    {
        _shaders.erase(it);
    }
}

Shaders&
ShaderObject::getShaders() noexcept
{
    return _shaders;
}

ShaderProgramPtr
ShaderObject::getShaderProgram() noexcept
{
    return _program;
}

ShaderAttributes&
ShaderObject::getActiveAttributes() noexcept
{
    return _program->getActiveAttributes();
}

ShaderUniforms&
ShaderObject::getActiveUniforms() noexcept
{
    return _program->getActiveUniforms();
}

ShaderUniformBlocks&
ShaderObject::getActiveUniformBlocks() noexcept
{
    return _program->getActiveUniformBlocks();
}

ShaderSubroutines&
ShaderObject::getActiveSubroutines() noexcept
{
    return _program->getActiveSubroutines();
}

_NAME_END