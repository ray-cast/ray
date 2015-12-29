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
#include "egl2_shader.h"

#define EXCLUDE_PSTDINT
#include <hlslcc.hpp>

_NAME_BEGIN

__ImplementSubClass(EGL2Shader, GraphicsShader, "EGL2Shader")
__ImplementSubClass(EGL2ShaderObject, GraphicsProgram, "EGL2ShaderObject")
__ImplementSubClass(EGL2ShaderAttribute, ShaderAttribute, "EGL2ShaderAttribute")
__ImplementSubClass(EGL2ShaderUniform, ShaderUniform, "EGL2ShaderUniform")

EGL2ShaderVariant::EGL2ShaderVariant() noexcept
	: _location(0)
{
}

EGL2ShaderVariant::~EGL2ShaderVariant() noexcept
{
}

void
EGL2ShaderVariant::setLocation(GLint location) noexcept
{
	_location = location;
}

GLint
EGL2ShaderVariant::getLocation() const noexcept
{
	return _location;
}

void
EGL2ShaderVariant::setType(ShaderVariantType type) noexcept
{
	if (_type != type)
	{
		if (_type == ShaderVariantType::FloatArray)
		{
			delete _value.farray;
			_value.farray = nullptr;
		}
		else if (_type == ShaderVariantType::Float2Array)
		{
			delete _value.farray2;
			_value.farray2 = nullptr;
		}
		else if (_type == ShaderVariantType::Float3x3)
		{
			delete _value.m3;
			_value.m3 = nullptr;
		}
		else if (_type == ShaderVariantType::Float4x4)
		{
			delete _value.m4;
			_value.m4 = nullptr;
		}

		if (type == ShaderVariantType::FloatArray)
		{
			_value.farray = new std::vector<float>();
		}
		else if (type == ShaderVariantType::Float2Array)
		{
			_value.farray2 = new std::vector<float2>();
		}
		else if (type == ShaderVariantType::Float3x3)
		{
			_value.m3 = new Matrix3x3;
		}
		else if (type == ShaderVariantType::Float4x4)
		{
			_value.m4 = new Matrix4x4;
		}

		_type = type;
	}
}

ShaderVariantType
EGL2ShaderVariant::getType() const noexcept
{
	return _type;
}

void
EGL2ShaderVariant::assign(bool value) noexcept
{
	this->setType(ShaderVariantType::Bool);
	if (_value.b != value)
	{
		_value.b = value;
	}
}

void
EGL2ShaderVariant::assign(int value) noexcept
{
	this->setType(ShaderVariantType::Int);
	if (_value.i[0] != value)
	{
		_value.i[0] = value;
	}
}

void
EGL2ShaderVariant::assign(const int2& value) noexcept
{
	this->setType(ShaderVariantType::Int2);
	if (_value.i[0] != value.x ||
		_value.i[1] != value.y)
	{
		_value.i[0] = value.x;
		_value.i[1] = value.y;
	}
}

void
EGL2ShaderVariant::assign(float value) noexcept
{
	this->setType(ShaderVariantType::Float);
	if (_value.f[0] != value)
	{
		_value.f[0] = value;
	}
}

void
EGL2ShaderVariant::assign(const float2& value) noexcept
{
	this->setType(ShaderVariantType::Float2);
	if (_value.f[0] != value.x ||
		_value.f[1] != value.y)
	{
		_value.f[0] = value.x;
		_value.f[1] = value.y;
	}
}

void
EGL2ShaderVariant::assign(const float3& value) noexcept
{
	this->setType(ShaderVariantType::Float3);
	if (_value.f[0] != value.x ||
		_value.f[1] != value.y ||
		_value.f[2] != value.z)
	{
		_value.f[0] = value.x;
		_value.f[1] = value.y;
		_value.f[2] = value.z;
	}
}

void
EGL2ShaderVariant::assign(const float4& value) noexcept
{
	this->setType(ShaderVariantType::Float4);
	if (_value.f[0] != value.x ||
		_value.f[1] != value.y ||
		_value.f[2] != value.z ||
		_value.f[3] != value.w)
	{
		_value.f[0] = value.x;
		_value.f[1] = value.y;
		_value.f[2] = value.z;
		_value.f[3] = value.w;
	}
}

void
EGL2ShaderVariant::assign(const float3x3& value) noexcept
{
	this->setType(ShaderVariantType::Float3x3);
	*_value.m3 = value;
}

void
EGL2ShaderVariant::assign(const float4x4& value) noexcept
{
	this->setType(ShaderVariantType::Float4x4);
	*_value.m4 = value;
}

void
EGL2ShaderVariant::assign(const std::vector<float>& value) noexcept
{
	this->setType(ShaderVariantType::FloatArray);
	*_value.farray = value;
}

void
EGL2ShaderVariant::assign(const std::vector<float2>& value) noexcept
{
	this->setType(ShaderVariantType::Float2Array);
	*_value.farray2 = value;
}

void
EGL2ShaderVariant::assign(const std::vector<float3>& value) noexcept
{
	this->setType(ShaderVariantType::Float3Array);
	*_value.farray3 = value;
}

void
EGL2ShaderVariant::assign(const std::vector<float4>& value) noexcept
{
	this->setType(ShaderVariantType::Float4Array);
	*_value.farray4 = value;
}

bool
EGL2ShaderVariant::getBool() const noexcept
{
	assert(_type == ShaderVariantType::Bool);
	return _value.b;
}

int
EGL2ShaderVariant::getInt() const noexcept
{
	assert(_type == ShaderVariantType::Int);
	return _value.i[0];
}

float
EGL2ShaderVariant::getFloat() const noexcept
{
	assert(_type == ShaderVariantType::Float);
	return _value.f[0];
}

const int2&
EGL2ShaderVariant::getInt2() const noexcept
{
	assert(_type == ShaderVariantType::Int2);
	return (int2&)_value.i;
}

const float2&
EGL2ShaderVariant::getFloat2() const noexcept
{
	assert(_type == ShaderVariantType::Float2);
	return (float2&)_value.f;
}

const float3&
EGL2ShaderVariant::getFloat3() const noexcept
{
	assert(_type == ShaderVariantType::Float3);
	return (float3&)_value.f;
}

const float4&
EGL2ShaderVariant::getFloat4() const noexcept
{
	assert(_type == ShaderVariantType::Float4);
	return (float4&)_value.f;
}

const float3x3&
EGL2ShaderVariant::getFloat3x3() const noexcept
{
	assert(_type == ShaderVariantType::Float3x3);
	return (float3x3&)*_value.m3;
}

const float4x4&
EGL2ShaderVariant::getFloat4x4() const noexcept
{
	assert(_type == ShaderVariantType::Float4x4);
	return (float4x4&)*_value.m4;
}

const std::vector<float>&
EGL2ShaderVariant::getFloatArray() const noexcept
{
	assert(_type == ShaderVariantType::FloatArray);
	return *_value.farray;
}

const std::vector<float2>&
EGL2ShaderVariant::getFloat2Array() const noexcept
{
	assert(_type == ShaderVariantType::Float2Array);
	return *_value.farray2;
}

EGL2ShaderAttribute::EGL2ShaderAttribute() noexcept
	: _location(GL_NONE)
{
}

EGL2ShaderAttribute::~EGL2ShaderAttribute() noexcept
{
}

void
EGL2ShaderAttribute::setLocation(GLint location) noexcept
{
	_location = location;
}

GLint
EGL2ShaderAttribute::getLocation() const noexcept
{
	return _location;
}

EGL2ShaderUniform::EGL2ShaderUniform() noexcept
	: ShaderUniform(&_value)
{
}

EGL2ShaderUniform::~EGL2ShaderUniform() noexcept
{
}

void
EGL2ShaderUniform::setName(const std::string& name) noexcept
{
	ShaderUniform::setName(name);
}

void
EGL2ShaderUniform::setType(ShaderVariantType type) noexcept
{
	ShaderUniform::setType(type);
}

void
EGL2ShaderUniform::setLocation(GLint location) noexcept
{
	_value.setLocation(location);
}

GLint
EGL2ShaderUniform::getLocation() const noexcept
{
	return _value.getLocation();
}

bool
EGL2ShaderUniform::getBool() const noexcept
{
	return _value.getBool();
}

int
EGL2ShaderUniform::getInt() const noexcept
{
	return _value.getInt();
}

float
EGL2ShaderUniform::getFloat() const noexcept
{
	return _value.getFloat();
}

const int2&
EGL2ShaderUniform::getInt2() const noexcept
{
	return _value.getInt2();
}

const float2&
EGL2ShaderUniform::getFloat2() const noexcept
{
	return _value.getFloat2();
}

const float3&
EGL2ShaderUniform::getFloat3() const noexcept
{
	return _value.getFloat3();
}

const float4&
EGL2ShaderUniform::getFloat4() const noexcept
{
	return _value.getFloat4();
}

const float3x3&
EGL2ShaderUniform::getFloat3x3() const noexcept
{
	return _value.getFloat3x3();
}

const float4x4&
EGL2ShaderUniform::getFloat4x4() const noexcept
{
	return _value.getFloat4x4();
}

const std::vector<float>&
EGL2ShaderUniform::getFloatArray() const noexcept
{
	return _value.getFloatArray();
}

const std::vector<float2>&
EGL2ShaderUniform::getFloat2Array() const noexcept
{
	return _value.getFloat2Array();
}

EGL2Shader::EGL2Shader() noexcept
	: _instance(GL_NONE)
{
}

EGL2Shader::~EGL2Shader() noexcept
{
	this->close();
}

bool
EGL2Shader::setup(const ShaderDesc& shaderDesc) noexcept
{
	assert(_instance == GL_NONE);

	GLenum shaderType = EGL2Types::asShaderType(shaderDesc.getType());
	if (shaderType == GL_INVALID_ENUM)
		return false;

	if (shaderDesc.getByteCodes().empty())
	{
		GL_PLATFORM_LOG("This shader code cannot be null");
		return false;
	}

	GLSLShader shader;
	GLSLCrossDependencyData dependency;

	std::uint32_t flags = HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT | HLSLCC_FLAG_COMBINE_TEXTURE_SAMPLERS | HLSLCC_FLAG_INOUT_APPEND_SEMANTIC_NAMES;
	if (shaderDesc.getType() == ShaderType::Geometry)
		flags = HLSLCC_FLAG_GS_ENABLED;
	else if (shaderDesc.getType() == ShaderType::TessControl)
		flags = HLSLCC_FLAG_TESS_ENABLED;
	else if (shaderDesc.getType() == ShaderType::TessEvaluation)
		flags = HLSLCC_FLAG_TESS_ENABLED;

	if (!TranslateHLSLFromMem(shaderDesc.getByteCodes().data(), flags, GLLang::LANG_ES_100, 0, &dependency, &shader))
	{
		GL_PLATFORM_LOG("Can't conv bytecodes to glsl");
		return false;
	}

	_instance = glCreateShader(shaderType);
	if (_instance == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateShader() fail");
		return false;
	}

	glShaderSource(_instance, 1, &shader.sourceCode, 0);
	glCompileShader(_instance);

	GLint result = GL_FALSE;
	glGetShaderiv(_instance, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result)
	{
		GLint length = 0;
		glGetShaderiv(_instance, GL_INFO_LOG_LENGTH, &length);

		std::string log((std::size_t)length, 0);
		glGetShaderInfoLog(_instance, length, &length, (char*)log.data());

		GL_PLATFORM_LOG(log);
		return false;
	}

	return true;
}

void
EGL2Shader::close() noexcept
{
	if (_instance)
	{
		glDeleteShader(_instance);
		_instance = 0;
	}
}

GLuint
EGL2Shader::getInstanceID() const noexcept
{
	return _instance;
}

void
EGL2Shader::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2Shader::getDevice() noexcept
{
	return _device.lock();
}

EGL2ShaderObject::EGL2ShaderObject() noexcept
	: _program(0)
	, _isActive(false)
{
}

EGL2ShaderObject::~EGL2ShaderObject() noexcept
{
	this->close();
}

bool
EGL2ShaderObject::setup(const ShaderObjectDesc& programDesc) noexcept
{
	assert(_program == GL_NONE);

	_program = glCreateProgram();

	for (auto& shader : programDesc.getShaders())
	{
		auto glshader = std::make_shared<EGL2Shader>();
		if (glshader->setup(shader))
		{
			glAttachShader(_program, glshader->getInstanceID());
			_shaders.push_back(glshader);
		}
	}

	glLinkProgram(_program);

	GLint status = GL_FALSE;
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (!status)
	{
		GLint length = 0;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &length);

		std::string log((std::size_t)length, 0);
		glGetProgramInfoLog(_program, length, &length, (GLchar*)log.data());

		GL_PLATFORM_LOG(log);
		return false;
	}

	_isActive = false;

	_initActiveAttribute();
	_initActiveUniform();

	return true;
}

void
EGL2ShaderObject::close() noexcept
{
	if (_program)
	{
		glDeleteProgram(_program);
		_program = 0;
	}

	_shaders.clear();

	_activeAttributes.clear();
	_activeUniforms.clear();
}

void
EGL2ShaderObject::setActive(bool active) noexcept
{
	if (_isActive != active)
	{
		if (active)
			glUseProgram(_program);
		_isActive = active;
	}

	if (active)
	{
		for (auto& it : _activeUniforms)
		{
			this->_updateShaderUniform(it);
		}
	}
}

bool
EGL2ShaderObject::getActive() noexcept
{
	return _isActive;
}

GLuint
EGL2ShaderObject::getInstanceID() const noexcept
{
	return _program;
}

ShaderUniforms&
EGL2ShaderObject::getActiveUniforms() noexcept
{
	return _activeUniforms;
}

ShaderAttributes&
EGL2ShaderObject::getActiveAttributes() noexcept
{
	return _activeAttributes;
}

void
EGL2ShaderObject::_initActiveAttribute() noexcept
{
	GLint numAttribute = 0;
	GLint maxAttribute = 0;

	glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &numAttribute);
	glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribute);

	if (numAttribute)
	{
		auto nameAttribute = std::make_unique<GLchar[]>(maxAttribute + 1);
		nameAttribute[maxAttribute] = 0;

		for (GLint i = 0; i < numAttribute; ++i)
		{
			GLint size;
			GLenum type;

			glGetActiveAttrib(_program, (GLuint)i, maxAttribute, GL_NONE, &size, &type, nameAttribute.get());
			GLint location = glGetAttribLocation(_program, nameAttribute.get());

			std::string name = nameAttribute.get();
			std::string semantic;

			std::size_t off = name.find_last_of('_');
			if (off != std::string::npos)
			{
				semantic = name.substr(off + 1);
				name = name.substr(0, off);
			}

			std::uint8_t semanticIndex = 0;
			auto it = std::find_if(semantic.begin(), semantic.end(), [](char ch) { return ch >= '0' && ch <= '9'; });
			if (it != semantic.end())
			{
				semanticIndex = atoi(&*it);
				semantic = semantic.substr(0, it - semantic.begin());
			}

			auto attrib = std::make_shared<EGL2ShaderAttribute>();
			attrib->setName(nameAttribute.get());
			attrib->setLocation(location);
			attrib->setSemantic(semantic);
			attrib->setSemanticIndex(semanticIndex);

			_activeAttributes.push_back(attrib);
		}
	}
}

void
EGL2ShaderObject::_initActiveUniform() noexcept
{
	GLint numUniform = 0;
	GLint maxUniformLength = 0;

	glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &numUniform);
	glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLength);

	if (numUniform == 0)
		return;

	auto nameUniform = std::make_unique<GLchar[]>(maxUniformLength + 1);
	nameUniform[maxUniformLength] = 0;

	for (GLint i = 0; i < numUniform; ++i)
	{
		GLint size;
		GLenum type;

		glGetActiveUniform(_program, (GLuint)i, maxUniformLength, 0, &size, &type, nameUniform.get());

		if (std::strstr(nameUniform.get(), ".") != 0 || nameUniform[0] == '_')
			continue;

		GLint location = glGetUniformLocation(_program, nameUniform.get());
		if (location == -1)
			continue;

		auto uniform = std::make_shared<EGL2ShaderUniform>();
		uniform->setName(nameUniform.get());
		uniform->setLocation(location);

		if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE)
		{
			uniform->setType(ShaderVariantType::Texture);
		}
		else
		{
			bool isArray = uniform->getName().find("[0]") != std::string::npos;

			if (type == GL_BOOL)
				uniform->setType(ShaderVariantType::Bool);
			else if (type == GL_INT)
				uniform->setType(ShaderVariantType::Int);
			else if (type == GL_INT_VEC2)
				uniform->setType(ShaderVariantType::Int2);
			else if (type == GL_FLOAT)
			{
				if (isArray)
					uniform->setType(ShaderVariantType::FloatArray);
				else
					uniform->setType(ShaderVariantType::Float);
			}
			else if (type == GL_FLOAT_VEC2)
			{
				if (isArray)
					uniform->setType(ShaderVariantType::Float2Array);
				else
					uniform->setType(ShaderVariantType::Float2);
			}
			else if (type == GL_FLOAT_VEC3)
			{
				if (isArray)
					uniform->setType(ShaderVariantType::Float3Array);
				else
					uniform->setType(ShaderVariantType::Float3);
			}
			else if (type == GL_FLOAT_VEC4)
			{
				if (isArray)
					uniform->setType(ShaderVariantType::Float4Array);
				else
					uniform->setType(ShaderVariantType::Float4);
			}
			else if (type == GL_FLOAT_MAT3)
				uniform->setType(ShaderVariantType::Float3x3);
			else if (type == GL_FLOAT_MAT4)
				uniform->setType(ShaderVariantType::Float4x4);
			else
				assert(false);
		}

		_activeUniforms.push_back(uniform);
	}
}

void
EGL2ShaderObject::_updateShaderUniform(ShaderUniformPtr it) noexcept
{
	assert(it);

	auto uniform = std::dynamic_pointer_cast<EGL2ShaderUniform>(it);

	auto type = uniform->getType();
	if (type != ShaderVariantType::Texture)
	{
		if (!uniform->needUpdate())
			return;
		uniform->needUpdate(false);
	}

	auto location = uniform->getLocation();

	switch (type)
	{
	case ShaderVariantType::Bool:
	{
		glUniform1i(location, uniform->getBool());
		break;
	}
	case ShaderVariantType::Int:
	{
		glUniform1i(location, uniform->getInt());
		break;
	}
	case ShaderVariantType::Int2:
	{
		glUniform2iv(location, 1, uniform->getInt2().ptr());
		break;
	}
	case ShaderVariantType::Float:
	{
		glUniform1f(location, uniform->getFloat());
		break;
	}
	case ShaderVariantType::Float2:
	{
		glUniform2fv(location, 1, uniform->getFloat2().ptr());
		break;
	}
	case ShaderVariantType::Float3:
	{
		glUniform3fv(location, 1, uniform->getFloat3().ptr());
		break;
	}
	case ShaderVariantType::Float4:
	{
		glUniform4fv(location, 1, uniform->getFloat4().ptr());
		break;
	}
	case ShaderVariantType::Float3x3:
	{
		glUniformMatrix3fv(location, 1, GL_FALSE, uniform->getFloat3x3().ptr());
		break;
	}
	case ShaderVariantType::Float4x4:
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, uniform->getFloat4x4().ptr());
		break;
	}
	case ShaderVariantType::FloatArray:
	{
		glUniform1fv(location, uniform->getFloatArray().size(), uniform->getFloatArray().data());
		break;
	}
	case ShaderVariantType::Float2Array:
	{
		glUniform2fv(location, uniform->getFloat2Array().size(), (GLfloat*)uniform->getFloat2Array().data());
		break;
	}
	case ShaderVariantType::Texture:
	{
		glUniform1i(location, uniform->getInt());
		break;
	}
	default:
		assert(false);
		break;
	}
}

void
EGL2ShaderObject::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2ShaderObject::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END