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
#include "egl3_shader.h"

_NAME_BEGIN

__ImplementSubClass(EGL3Shader, GraphicsShader, "EGL3Shader")
__ImplementSubClass(EGL3ShaderObject, GraphicsProgram, "EGL3ShaderObject")

EGL3ShaderVariant::EGL3ShaderVariant() noexcept
	: _bindingProgram(0)
	, _location(0)
{
}

EGL3ShaderVariant::~EGL3ShaderVariant() noexcept
{
}

void
EGL3ShaderVariant::setLocation(GLint location) noexcept
{
	_location = location;
}

GLint
EGL3ShaderVariant::getLocation() const noexcept
{
	return _location;
}

void
EGL3ShaderVariant::setBindingProgram(GLuint program) noexcept
{
	_bindingProgram = program;
}

GLuint
EGL3ShaderVariant::getBindingProgram() const noexcept
{
	return _bindingProgram;
}

void
EGL3ShaderVariant::assign(bool value) noexcept
{
	GL_CHECK(glProgramUniform1i(_bindingProgram, _location, value));
}

void
EGL3ShaderVariant::assign(int value) noexcept
{
	GL_CHECK(glProgramUniform1i(_bindingProgram, _location, value));
}

void
EGL3ShaderVariant::assign(const int2& value) noexcept
{
	GL_CHECK(glProgramUniform2iv(_bindingProgram, _location, 1, value.ptr()));
}

void
EGL3ShaderVariant::assign(float value) noexcept
{
	GL_CHECK(glProgramUniform1f(_bindingProgram, _location, value));
}

void
EGL3ShaderVariant::assign(const float2& value) noexcept
{
	GL_CHECK(glProgramUniform2fv(_bindingProgram, _location, 1, value.ptr()));
}

void
EGL3ShaderVariant::assign(const float3& value) noexcept
{
	GL_CHECK(glProgramUniform3fv(_bindingProgram, _location, 1, value.ptr()));
}

void
EGL3ShaderVariant::assign(const float4& value) noexcept
{
	GL_CHECK(glProgramUniform4fv(_bindingProgram, _location, 1, value.ptr()));
}

void
EGL3ShaderVariant::assign(const float3x3& value) noexcept
{
	GL_CHECK(glProgramUniformMatrix3fv(_bindingProgram, _location, 1, GL_FALSE, value.ptr()));
}

void
EGL3ShaderVariant::assign(const float4x4& value) noexcept
{
	GL_CHECK(glProgramUniformMatrix4fv(_bindingProgram, _location, 1, GL_FALSE, value.ptr()));
}

void
EGL3ShaderVariant::assign(const std::vector<float>& value) noexcept
{
	GL_CHECK(glProgramUniform1fv(_bindingProgram, _location, value.size(), value.data()));
}

void
EGL3ShaderVariant::assign(const std::vector<float2>& value) noexcept
{
	GL_CHECK(glProgramUniform2fv(_bindingProgram, _location, value.size(), (GLfloat*)value.data()));
}

void
EGL3ShaderVariant::assign(const std::vector<float3>& value) noexcept
{
	GL_CHECK(glProgramUniform3fv(_bindingProgram, _location, value.size(), (GLfloat*)value.data()));
}

void
EGL3ShaderVariant::assign(const std::vector<float4>& value) noexcept
{
	GL_CHECK(glProgramUniform4fv(_bindingProgram, _location, value.size(), (GLfloat*)value.data()));
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
EGLShaderUniform::setBindingProgram(GLuint program) noexcept
{
	_value.setBindingProgram(program);
}

GLuint
EGLShaderUniform::getBindingProgram() const noexcept
{
	return _value.getBindingProgram();
}

EGL3Shader::EGL3Shader() noexcept
	: _instance(GL_NONE)
{
}

EGL3Shader::~EGL3Shader() noexcept
{
	this->close();
}

bool
EGL3Shader::setup(const ShaderDesc& desc) except
{
	assert(!_instance);

	GLenum shaderType = EGL3Types::asEGL3ShaderType(desc.getType());
	_instance = glCreateShader(shaderType);
	if (_instance == GL_NONE)
		throw failure(__TEXT("glCreateShader fail"));

	if (desc.getSource().empty())
		throw failure(__TEXT("This shader code cannot be null"));

	const GLchar* codes[1] =
	{
		desc.getSource().c_str()
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
EGL3Shader::close() noexcept
{
	if (_instance)
	{
		glDeleteShader(_instance);
		_instance = 0;
	}
}

GLuint
EGL3Shader::getInstanceID() const noexcept
{
	return _instance;
}

EGL3ShaderObject::EGL3ShaderObject() noexcept
	: _program(0)
	, _isActive(false)
{
}

EGL3ShaderObject::~EGL3ShaderObject() noexcept
{
	this->close();
}

bool
EGL3ShaderObject::setup(const ShaderObjectDesc& programDesc) except
{
	assert(!_program);

	_program = glCreateProgram();

	for (auto& shader : programDesc.getShaders())
	{
		auto glshader = std::make_shared<EGL3Shader>();
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

		glDeleteProgram(_program);
		throw failure(log.data());
	}

	_isActive = false;

	_initActiveAttribute();
	_initActiveUniform();
	_initActiveUniformBlock();

	return true;
}

void
EGL3ShaderObject::close() noexcept
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
EGL3ShaderObject::setActive(bool active) noexcept
{
	if (_isActive != active)
	{
		if (active)
			glUseProgram(_program);
		_isActive = active;
	}
}

bool
EGL3ShaderObject::getActive() noexcept
{
	return _isActive;
}

GLuint
EGL3ShaderObject::getInstanceID() const noexcept
{
	return _program;
}

ShaderUniforms&
EGL3ShaderObject::getActiveUniforms() noexcept
{
	return _activeUniforms;
}

ShaderAttributes&
EGL3ShaderObject::getActiveAttributes() noexcept
{
	return _activeAttributes;
}

void
EGL3ShaderObject::_initActiveAttribute() noexcept
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
EGL3ShaderObject::_initActiveUniform() noexcept
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

		GLuint location = glGetUniformLocation(_program, nameUniform.get());
		if (location == GL_INVALID_INDEX)
			continue;

		auto uniform = std::make_shared<EGLShaderUniform>();
		uniform->setName(nameUniform.get());
		uniform->setLocation(location);
		uniform->setBindingProgram(_program);

		if (type == GL_SAMPLER_2D || type == GL_SAMPLER_3D ||
			type == GL_SAMPLER_2D_SHADOW ||
			type == GL_SAMPLER_2D_ARRAY || type == GL_SAMPLER_CUBE ||
			type == GL_SAMPLER_2D_ARRAY_SHADOW || type == GL_SAMPLER_CUBE_SHADOW)
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
EGL3ShaderObject::_initActiveUniformBlock() noexcept
{
	GLint numUniformBlock = 0;
	GLint maxUniformBlockLength = 0;
	GLint maxUniformLength = 0;

	glGetProgramiv(_program, GL_ACTIVE_UNIFORM_BLOCKS, &numUniformBlock);
	glGetProgramiv(_program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxUniformBlockLength);
	glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLength);

	if (numUniformBlock)
	{
		auto nameUniformBlock = make_scope<GLchar[]>(maxUniformBlockLength + 1);
		nameUniformBlock[maxUniformBlockLength] = 0;

		for (GLint i = 0; i < numUniformBlock; ++i)
		{
			GLsizei lengthUniformBlock;
			glGetActiveUniformBlockName(_program, (GLuint)i, maxUniformBlockLength, &lengthUniformBlock, nameUniformBlock.get());

			GLuint location = glGetUniformBlockIndex(_program, nameUniformBlock.get());
			if (location == GL_INVALID_INDEX)
				continue;

			glUniformBlockBinding(_program, location, location);

			GLint size;
			GLint count;

			glGetActiveUniformBlockiv(_program, location, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
			glGetActiveUniformBlockiv(_program, location, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &count);

			if (count)
			{
				std::vector<GLint> indices(count);
				glGetActiveUniformBlockiv(_program, location, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices.data());

				std::vector<GLint> offset((std::size_t)count);
				std::vector<GLint> type((std::size_t)count);
				std::vector<GLint> datasize((std::size_t)count);
				std::vector<std::string> varlist((std::size_t)count);
				std::vector<GLchar> name(maxUniformLength);

				glGetActiveUniformsiv(_program, count, (GLuint*)&indices[0], GL_UNIFORM_OFFSET, &offset[0]);
				glGetActiveUniformsiv(_program, count, (GLuint*)&indices[0], GL_UNIFORM_TYPE, &type[0]);
				glGetActiveUniformsiv(_program, count, (GLuint*)&indices[0], GL_UNIFORM_SIZE, &datasize[0]);

				for (GLint j = 0; j < count; ++j)
				{
					GLsizei length = 0;

					glGetActiveUniform(_program, indices[j], maxUniformLength, &length, &datasize[j], (GLenum*)&type[j], name.data());
					varlist[j].append(name.data(), length);
				}

				auto uniformblock = std::make_shared<EGLShaderUniform>();
				uniformblock->setName(nameUniformBlock.get());
				uniformblock->setType(ShaderVariantType::Buffer);
				uniformblock->setLocation(location);

				_activeUniforms.push_back(uniformblock);
			}
		}
	}
}

_NAME_END