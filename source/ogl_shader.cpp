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
#include <ray/ogl_shader.h>

_NAME_BEGIN

OGLShader::OGLShader() noexcept
	: _instance(GL_NONE)
{
}

OGLShader::~OGLShader() noexcept
{
	this->close();
}

bool
OGLShader::setup() except
{
	assert(!_instance);

	GLenum shaderType = OGLTypes::asOGLShaderType(this->getType());
	_instance = glCreateShader(shaderType);
	if (GL_NONE == _instance)
	{
		throw failure(__TEXT("glCreateShader fail"));
	}

	if (this->getSource().empty())
	{
		throw failure(__TEXT("This shader code cannot be null"));
	}

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

		throw failure(log);
	}

	return true;
}

void
OGLShader::close() noexcept
{
	if (_instance)
	{
		glDeleteShader(_instance);
		_instance = 0;
	}
}

std::size_t
OGLShader::getInstanceID() const noexcept
{
	return _instance;
}

OGLShaderObject::OGLShaderObject() noexcept
	: _program(0)
{
}

OGLShaderObject::~OGLShaderObject() noexcept
{
	this->close();
}

bool
OGLShaderObject::setup() except
{
	assert(!_program);

	_program = glCreateProgram();

	for (auto shader : _shaders)
	{
		auto oglShader = std::dynamic_pointer_cast<OGLShader>(shader);
		if (oglShader)
		{
			if (!shader->getInstanceID())
				shader->setup();

			glAttachShader(_program, oglShader->getInstanceID());
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
		throw failure(log.data());
	}

	_initActiveAttribute();
	_initActiveUniform();
	_initActiveUniformBlock();
	_initActiveSubroutine();

	return true;
}

void
OGLShaderObject::close() noexcept
{
	if (_program)
	{
		glDeleteProgram(_program);
		_program = 0;
	}

	_shaders.clear();

	_activeAttributes.clear();
	_activeUniforms.clear();
	_activeSubroutines.clear();
}

void
OGLShaderObject::addShader(ShaderPtr shader) noexcept
{
	_shaders.push_back(shader);
}

void
OGLShaderObject::removeShader(ShaderPtr shader) noexcept
{
	auto it = std::find(_shaders.begin(), _shaders.end(), shader);
	if (it != _shaders.end())
	{
		_shaders.erase(it);
	}
}

Shaders&
OGLShaderObject::getShaders() noexcept
{
	return _shaders;
}

std::size_t
OGLShaderObject::getInstanceID() noexcept
{
	return _program;
}

ShaderAttributes&
OGLShaderObject::getActiveAttributes() noexcept
{
	return _activeAttributes;
}

ShaderUniforms&
OGLShaderObject::getActiveUniforms() noexcept
{
	return _activeUniforms;
}

ShaderSubroutines&
OGLShaderObject::getActiveSubroutines() noexcept
{
	return _activeSubroutines;
}

void
OGLShaderObject::_initActiveAttribute() noexcept
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
			attrib->setLocation(location);

			_activeAttributes.push_back(attrib);
		}
	}
}

void
OGLShaderObject::_initActiveUniform() noexcept
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

		GLuint location = glGetUniformLocation(_program, nameUniform.get());
		if (location == GL_INVALID_INDEX)
			continue;

		auto uniform = std::make_shared<ShaderUniform>();
		uniform->setName(nameUniform.get());
		uniform->setLocation(location);
		uniform->setBindingProgram(_program);

		if (type == GL_SAMPLER_2D || type == GL_SAMPLER_3D ||
			type == GL_SAMPLER_2D_SHADOW ||
			type == GL_SAMPLER_2D_ARRAY || type == GL_SAMPLER_CUBE ||
			type == GL_SAMPLER_2D_ARRAY_SHADOW || type == GL_SAMPLER_CUBE_SHADOW)
		{
			uniform->setType(SPT_TEXTURE);
			uniform->setBindingPoint(numTexUnit++);
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
OGLShaderObject::_initActiveUniformBlock() noexcept
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
#if !defined(EGLAPI)
					glGetActiveUniformName(_program, indices[j], maxUniformLength, &length, name.data());
#else
					glGetActiveUniform(_program, indices[j], maxUniformLength, &length, &datasize[j], (GLenum*)&type[j], name.data());
#endif
					
					varlist[j].append(name.data(), length);
				}

				auto uniformblock = std::make_shared<ShaderUniform>();
				uniformblock->setName(nameUniformBlock.get());
				uniformblock->setType(ShaderVariantType::SPT_BUFFER);
				uniformblock->setLocation(location);

				_activeUniforms.push_back(uniformblock);
			}
		}
	}
}

void
OGLShaderObject::_initActiveSubroutine() noexcept
{
#if !defined(EGLAPI)
	GLint numSubroutines = 0;
	GLint maxSubroutines = 0;

	glGetProgramStageiv(_program, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINES, &numSubroutines);
	glGetProgramStageiv(_program, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINE_MAX_LENGTH, &maxSubroutines);

	if (numSubroutines)
	{
		auto nameSubroutines = make_scope<GLchar[]>(maxSubroutines + 1);
		nameSubroutines[maxSubroutines + 1] = 0;

		for (GLint i = 0; i < maxSubroutines; ++i)
		{
			GLenum type = GL_VERTEX_SHADER;
			glGetActiveSubroutineName(_program, type, i, maxSubroutines, 0, nameSubroutines.get());

			GLint location = glGetSubroutineIndex(_program, type, nameSubroutines.get());

			auto subroutines = std::make_shared<ShaderSubroutine>();
			subroutines->setName(nameSubroutines.get());
			subroutines->setLocation(location);

			_activeSubroutines.push_back(subroutines);
		}
	}
#endif
}

/*void
OGLShaderObject::onAttachSubroutine(std::shared_ptr<ShaderObject::Subroutine> subroutine) noexcept
{
	if (_program)
	{
		int index = -1;

		switch (subroutine->getShaderType())
		{
		case Shader::vertex:
			if (subroutine->isMultiple())
				index = glGetSubroutineUniformLocation(_program, GL_VERTEX_SHADER, subroutine->getName().c_str());
			else
				index = glGetSubroutineIndex(_program, GL_VERTEX_SHADER, subroutine->getName().c_str());
			break;
		case Shader::fragment:
			if (subroutine->isMultiple())
				index = glGetSubroutineUniformLocation(_program, GL_FRAGMENT_SHADER, subroutine->getName().c_str());
			else
				index = glGetSubroutineIndex(_program, GL_FRAGMENT_SHADER, subroutine->getName().c_str());
			break;
		case Shader::geometry:
			if (subroutine->isMultiple())
				index = glGetSubroutineUniformLocation(_program, GL_GEOMETRY_SHADER, subroutine->getName().c_str());
			else
				index = glGetSubroutineIndex(_program, GL_GEOMETRY_SHADER, subroutine->getName().c_str());
			break;
		}
	}

	for (auto& it : _subroutines)
	{
		switch (it->value())
		{
		case vertex:
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &it->getInstance());
			break;
		case fragment:
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &it->getInstance());
			break;
		case geometry:
			glUniformSubroutinesuiv(GL_GEOMETRY_SHADER, 1, &it->getInstance());
			break;
		}
	}
}*/

_NAME_END