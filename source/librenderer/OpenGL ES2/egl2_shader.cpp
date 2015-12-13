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
#include "egl2_texture.h"

_NAME_BEGIN

EGLShaderVariant::EGLShaderVariant() noexcept
	: _bindingProgram(0)
	, _bindingPoint(0)
	, _location(0)
{
}

EGLShaderVariant::~EGLShaderVariant() noexcept
{
}

void
EGLShaderVariant::setLocation(GLint location) noexcept
{
	_location = location;
}

GLint
EGLShaderVariant::getLocation() const noexcept
{
	return _location;
}

void
EGLShaderVariant::setBindingPoint(GLint unit) noexcept
{
	_bindingPoint = unit;
}

GLint
EGLShaderVariant::getBindingPoint() const noexcept
{
	return _bindingPoint;
}

void
EGLShaderVariant::setBindingProgram(GLuint program) noexcept
{
	_bindingProgram = program;
}

GLuint
EGLShaderVariant::getBindingProgram() const noexcept
{
	return _bindingProgram;
}

void
EGLShaderVariant::setType(ShaderVariantType type) noexcept
{
	if (_type != type)
	{
		if (_type == ShaderVariantType::SPT_FLOAT_ARRAY)
		{
			delete _value.farray;
			_value.farray = nullptr;
		}
		else if (_type == ShaderVariantType::SPT_FLOAT2_ARRAY)
		{
			delete _value.farray2;
			_value.farray2 = nullptr;
		}
		else if (_type == ShaderVariantType::SPT_FLOAT3X3)
		{
			delete _value.m3;
			_value.m3 = nullptr;
		}
		else if (_type == ShaderVariantType::SPT_FLOAT4X4)
		{
			delete _value.m4;
			_value.m4 = nullptr;
		}

		if (type == ShaderVariantType::SPT_FLOAT_ARRAY)
		{
			_value.farray = new std::vector<float>();
		}
		else if (type == ShaderVariantType::SPT_FLOAT2_ARRAY)
		{
			_value.farray2 = new std::vector<float2>();
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
EGLShaderVariant::getType() const noexcept
{
	return _type;
}

void
EGLShaderVariant::assign(bool value) noexcept
{
	this->setType(ShaderVariantType::SPT_BOOL);
	if (_value.b != value)
	{
		_value.b = value;
	}
}

void
EGLShaderVariant::assign(int value) noexcept
{
	this->setType(ShaderVariantType::SPT_INT);
	if (_value.i[0] != value)
	{
		_value.i[0] = value;
	}
}

void
EGLShaderVariant::assign(const int2& value) noexcept
{
	this->setType(ShaderVariantType::SPT_INT2);
	if (_value.i[0] != value.x ||
		_value.i[1] != value.y)
	{
		_value.i[0] = value.x;
		_value.i[1] = value.y;
	}
}

void
EGLShaderVariant::assign(float value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT);
	if (_value.f[0] != value)
	{
		_value.f[0] = value;
	}
}

void
EGLShaderVariant::assign(const float2& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT2);
	if (_value.f[0] != value.x ||
		_value.f[1] != value.y)
	{
		_value.f[0] = value.x;
		_value.f[1] = value.y;
	}
}

void
EGLShaderVariant::assign(const float3& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT3);
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
EGLShaderVariant::assign(const float4& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT4);
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
EGLShaderVariant::assign(const float3x3& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT3X3);
	*_value.m3 = value;
}

void
EGLShaderVariant::assign(const float4x4& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT4X4);
	*_value.m4 = value;
}

void
EGLShaderVariant::assign(const std::vector<float>& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT_ARRAY);
	*_value.farray = value;
}

void
EGLShaderVariant::assign(const std::vector<float2>& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT2_ARRAY);
	*_value.farray2 = value;
}

void
EGLShaderVariant::assign(const std::vector<float3>& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT3_ARRAY);
	*_value.farray3 = value;
}

void
EGLShaderVariant::assign(const std::vector<float4>& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT4_ARRAY);
	*_value.farray4 = value;
}

void
EGLShaderVariant::assign(TexturePtr texture, TextureSamplerPtr sampler) noexcept
{
	this->setType(ShaderVariantType::SPT_TEXTURE);
	if (_texture != texture)
	{
		_texture = texture;
		_textureSampler = sampler;
	}
}

bool
EGLShaderVariant::getBool() const noexcept
{
	assert(_type == ShaderVariantType::SPT_BOOL);
	return _value.b;
}

int
EGLShaderVariant::getInt() const noexcept
{
	assert(_type == ShaderVariantType::SPT_INT);
	return _value.i[0];
}

float
EGLShaderVariant::getFloat() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT);
	return _value.f[0];
}

const int2&
EGLShaderVariant::getInt2() const noexcept
{
	assert(_type == ShaderVariantType::SPT_INT2);
	return (int2&)_value.i;
}

const float2&
EGLShaderVariant::getFloat2() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT2);
	return (float2&)_value.f;
}

const float3&
EGLShaderVariant::getFloat3() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT3);
	return (float3&)_value.f;
}

const float4&
EGLShaderVariant::getFloat4() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT4);
	return (float4&)_value.f;
}

const float3x3&
EGLShaderVariant::getFloat3x3() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT3X3);
	return (float3x3&)*_value.m3;
}

const float4x4&
EGLShaderVariant::getFloat4x4() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT4X4);
	return (float4x4&)*_value.m4;
}

const std::vector<float>&
EGLShaderVariant::getFloatArray() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT_ARRAY);
	return *_value.farray;
}

const std::vector<float2>&
EGLShaderVariant::getFloat2Array() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT_ARRAY);
	return *_value.farray2;
}

TexturePtr
EGLShaderVariant::getTexture() const noexcept
{
	return _texture;
}

TextureSamplerPtr
EGLShaderVariant::getSampler() const noexcept
{
	return _textureSampler;
}

EGLShaderUniform::EGLShaderUniform() noexcept
	: ShaderUniform(&_value)
{
}

EGLShaderUniform::~EGLShaderUniform() noexcept
{
}

void
EGLShaderUniform::setName(const std::string& name) noexcept
{
	ShaderUniform::setName(name);
}

void
EGLShaderUniform::setType(ShaderVariantType type) noexcept
{
	ShaderUniform::setType(type);
}

void
EGLShaderUniform::setLocation(GLint location) noexcept
{
	_value.setLocation(location);
}

GLint
EGLShaderUniform::getLocation() const noexcept
{
	return _value.getLocation();
}

void
EGLShaderUniform::setBindingPoint(GLint unit) noexcept
{
	_value.setBindingPoint(unit);
}

GLint
EGLShaderUniform::getBindingPoint() const noexcept
{
	return _value.getBindingPoint();
}

void
EGLShaderUniform::setBindingProgram(GLuint program) noexcept
{
	_value.setBindingProgram(program);
}

GLuint
EGLShaderUniform::getBindingProgram() const noexcept
{
	return _value.getBindingProgram();
}

TexturePtr
EGLShaderUniform::getTexture() const noexcept
{
	return _value.getTexture();
}

TextureSamplerPtr
EGLShaderUniform::getSampler() const noexcept
{
	return _value.getSampler();
}

bool
EGLShaderUniform::getBool() const noexcept
{
	return _value.getBool();
}

int
EGLShaderUniform::getInt() const noexcept
{
	return _value.getInt();
}

float
EGLShaderUniform::getFloat() const noexcept
{
	return _value.getFloat();
}

const int2&
EGLShaderUniform::getInt2() const noexcept
{
	return _value.getInt2();
}

const float2&
EGLShaderUniform::getFloat2() const noexcept
{
	return _value.getFloat2();
}

const float3&
EGLShaderUniform::getFloat3() const noexcept
{
	return _value.getFloat3();
}

const float4&
EGLShaderUniform::getFloat4() const noexcept
{
	return _value.getFloat4();
}

const float3x3&
EGLShaderUniform::getFloat3x3() const noexcept
{
	return _value.getFloat3x3();
}

const float4x4&
EGLShaderUniform::getFloat4x4() const noexcept
{
	return _value.getFloat4x4();
}

const std::vector<float>&
EGLShaderUniform::getFloatArray() const noexcept
{
	return _value.getFloatArray();
}

const std::vector<float2>&
EGLShaderUniform::getFloat2Array() const noexcept
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
EGL2Shader::setup() except
{
	assert(!_instance);

	GLenum shaderType = EGL2Types::asEGL2ShaderType(this->getType());
	_instance = glCreateShader(shaderType);
	if (_instance == GL_NONE)
		throw failure(__TEXT("glCreateShader fail"));

	if (this->getSource().empty())
		throw failure(__TEXT("This shader code cannot be null"));

	const GLchar* codes[1] =
	{
		this->getSource().c_str()
	};

	glShaderSource(_instance, 1, codes, 0);
	glCompileShader(_instance);

	GLint result = GL_FALSE;
	glGetShaderiv(_instance, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result)
	{
		GLint length = 0;
		glGetShaderiv(_instance, GL_INFO_LOG_LENGTH, &length);

		std::string log((std::size_t)length, 0);
		glGetShaderInfoLog(_instance, length, &length, (char*)log.data());

		glDeleteShader(_instance);
		throw failure(log);
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

std::size_t
EGL2Shader::getInstanceID() const noexcept
{
	return _instance;
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
EGL2ShaderObject::setup() except
{
	assert(!_program);

	_program = glCreateProgram();

	for (auto& shader : _shaders)
	{
		auto glshader = std::dynamic_pointer_cast<EGL2Shader>(shader);
		if (glshader)
		{
			if (!glshader->getInstanceID())
				glshader->setup();

			glAttachShader(_program, glshader->getInstanceID());
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

		glDeleteProgram(_program);
		throw failure(log.data());
	}

	_isActive = false;

	glUseProgram(_program);

	_initActiveAttribute();
	_initActiveUniform();

	glUseProgram(GL_NONE);

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

void
EGL2ShaderObject::addShader(ShaderPtr shader) noexcept
{
	_shaders.push_back(shader);
}

void
EGL2ShaderObject::removeShader(ShaderPtr shader) noexcept
{
	auto it = std::find(_shaders.begin(), _shaders.end(), shader);
	if (it != _shaders.end())
		_shaders.erase(it);
}

const Shaders&
EGL2ShaderObject::getShaders() const noexcept
{
	return _shaders;
}

std::size_t
EGL2ShaderObject::getInstanceID() noexcept
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

			auto attrib = std::make_shared<ShaderAttribute>();
			attrib->setName(nameAttribute.get());
			//attrib->setLocation(location);

			_activeAttributes.push_back(attrib);
		}
	}
}

void
EGL2ShaderObject::_initActiveUniform() noexcept
{
	GLint numUniform = 0;
	GLint maxUniformLength = 0;
	GLint numTexUnit = 0;

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

		auto uniform = std::make_shared<EGLShaderUniform>();
		uniform->setName(nameUniform.get());
		uniform->setLocation(location);
		uniform->setBindingProgram(_program);

		if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE)
		{
			uniform->setType(SPT_TEXTURE);
			uniform->setBindingPoint(numTexUnit++);

			GL_CHECK(glUniform1i(location, uniform->getBindingPoint()));
		}
		else
		{
			bool isArray = uniform->getName().find("[0]") != std::string::npos;

			if (type == GL_BOOL)
				uniform->setType(SPT_BOOL);
			else if (type == GL_INT)
				uniform->setType(SPT_INT);
			else if (type == GL_INT_VEC2)
				uniform->setType(SPT_INT2);
			else if (type == GL_FLOAT)
			{
				if (isArray)
					uniform->setType(SPT_FLOAT_ARRAY);
				else
					uniform->setType(SPT_FLOAT);
			}
			else if (type == GL_FLOAT_VEC2)
			{
				if (isArray)
					uniform->setType(SPT_FLOAT2_ARRAY);
				else
					uniform->setType(SPT_FLOAT2);
			}
			else if (type == GL_FLOAT_VEC3)
			{
				if (isArray)
					uniform->setType(SPT_FLOAT3_ARRAY);
				else
					uniform->setType(SPT_FLOAT3);
			}
			else if (type == GL_FLOAT_VEC4)
			{
				if (isArray)
					uniform->setType(SPT_FLOAT4_ARRAY);
				else
					uniform->setType(SPT_FLOAT4);
			}
			else if (type == GL_FLOAT_MAT3)
				uniform->setType(SPT_FLOAT3X3);
			else if (type == GL_FLOAT_MAT4)
				uniform->setType(SPT_FLOAT4X4);
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

	auto uniform = std::dynamic_pointer_cast<EGLShaderUniform>(it);

	auto type = uniform->getType();
	if (type != ShaderVariantType::SPT_TEXTURE)
	{
		if (!uniform->needUpdate())
			return;
		uniform->needUpdate(false);
	}

	auto location = uniform->getLocation();
	auto program = uniform->getBindingProgram();

	switch (type)
	{
	case ShaderVariantType::SPT_BOOL:
	{
		glUniform1i(location, uniform->getBool());
		break;
	}
	case ShaderVariantType::SPT_INT:
	{
		glUniform1i(location, uniform->getInt());
		break;
	}
	case ShaderVariantType::SPT_INT2:
	{
		glUniform2iv(location, 1, uniform->getInt2().ptr());
		break;
	}
	case ShaderVariantType::SPT_FLOAT:
	{
		glUniform1f(location, uniform->getFloat());
		break;
	}
	case ShaderVariantType::SPT_FLOAT2:
	{
		glUniform2fv(location, 1, uniform->getFloat2().ptr());
		break;
	}
	case ShaderVariantType::SPT_FLOAT3:
	{
		glUniform3fv(location, 1, uniform->getFloat3().ptr());
		break;
	}
	case ShaderVariantType::SPT_FLOAT4:
	{
		glUniform4fv(location, 1, uniform->getFloat4().ptr());
		break;
	}
	case ShaderVariantType::SPT_FLOAT3X3:
	{
		glUniformMatrix3fv(location, 1, GL_FALSE, uniform->getFloat3x3().ptr());
		break;
	}
	case ShaderVariantType::SPT_FLOAT4X4:
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, uniform->getFloat4x4().ptr());
		break;
	}
	case ShaderVariantType::SPT_FLOAT_ARRAY:
	{
		glUniform1fv(location, uniform->getFloatArray().size(), uniform->getFloatArray().data());
		break;
	}
	case ShaderVariantType::SPT_FLOAT2_ARRAY:
	{
		glUniform2fv(location, uniform->getFloat2Array().size(), (GLfloat*)uniform->getFloat2Array().data());
		break;
	}
	case ShaderVariantType::SPT_TEXTURE:
	{
		auto texture = uniform->getTexture();
		if (texture)
		{
			auto bindingPoint = uniform->getBindingPoint();
			auto texture = std::dynamic_pointer_cast<EGL2Texture>(uniform->getTexture());

			if (texture)
			{
				GLuint textureID = texture->getInstanceID();
				GLenum textureDim = EGL2Types::asEGL2Target(texture->getTexDim());

				GL_CHECK(glActiveTexture(GL_TEXTURE0 + bindingPoint));
				GL_CHECK(glBindTexture(textureDim, textureID));
			}
			else
			{
				GL_CHECK(glActiveTexture(GL_TEXTURE0 + bindingPoint));
				GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
			}
		}
		break;
	}
	default:
		assert(false);
		break;
	}
}

_NAME_END