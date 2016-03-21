// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include "ogl_shader.h"

#define EXCLUDE_PSTDINT
#include <hlslcc.hpp>

_NAME_BEGIN

__ImplementSubClass(OGLShader, GraphicsShader, "OGLShader")
__ImplementSubClass(OGLShaderObject, GraphicsProgram, "OGLShaderObject")
__ImplementSubClass(OGLShaderAttribute, ShaderAttribute, "OGLShaderAttribute")
__ImplementSubClass(OGLShaderUniform, ShaderUniform, "OGLShaderUniform")

OGLShaderAttribute::OGLShaderAttribute() noexcept
	: _location(GL_NONE)
{
}

OGLShaderAttribute::~OGLShaderAttribute() noexcept
{
}

void 
OGLShaderAttribute::setLocation(GLint location) noexcept
{
	_location = location;
}

GLint 
OGLShaderAttribute::getLocation() const noexcept
{
	return _location;
}

OGLShaderUniform::OGLShaderUniform() noexcept
	: _program(GL_NONE)
{
}

OGLShaderUniform::~OGLShaderUniform() noexcept
{
}

void
OGLShaderUniform::setBindingProgram(GLuint program) noexcept
{
	_program = program;
}

GLuint
OGLShaderUniform::getBindingProgram() const noexcept
{
	return _program;
}

OGLShader::OGLShader() noexcept
	: _instance(GL_NONE)
{
}

OGLShader::~OGLShader() noexcept
{
	this->close();
}

bool
OGLShader::setup(const GraphicsShaderDesc& shaderDesc) noexcept
{
	assert(_instance == GL_NONE);

	GLenum shaderType = OGLTypes::asShaderType(shaderDesc.getType());
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
	if (shaderDesc.getType() == GraphicsShaderStage::GraphicsShaderStageGeometry)
		flags = HLSLCC_FLAG_GS_ENABLED;
	else if (shaderDesc.getType() == GraphicsShaderStage::GraphicsShaderStageTessControl)
		flags = HLSLCC_FLAG_TESS_ENABLED;
	else if (shaderDesc.getType() == GraphicsShaderStage::GraphicsShaderStageTessEvaluation)
		flags = HLSLCC_FLAG_TESS_ENABLED;

	if (!TranslateHLSLFromMem(shaderDesc.getByteCodes().data(), flags, GLLang::LANG_DEFAULT, 0, &dependency, &shader))
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

	_shaderDesc = shaderDesc;
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

GLuint
OGLShader::getInstanceID() const noexcept
{
	return _instance;
}

const GraphicsShaderDesc& 
OGLShader::getGraphicsShaderDesc() const noexcept
{
	return _shaderDesc;
}

void
OGLShader::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLShader::getDevice() noexcept
{
	return _device.lock();
}

OGLShaderObject::OGLShaderObject() noexcept
	: _program(GL_NONE)
{
}

OGLShaderObject::~OGLShaderObject() noexcept
{
	this->close();
}

bool
OGLShaderObject::setup(const GraphicsProgramDesc& programDesc) noexcept
{
	assert(_program == GL_NONE);

	if (programDesc.getShaders().empty())
		return false;

	_program = glCreateProgram();
	if (_program == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateProgram() fail");
		return false;
	}

	for (auto& shader : programDesc.getShaders())
	{
		auto glshader = shader->downcast<OGLShader>();
		if (glshader)
			glAttachShader(_program, glshader->getInstanceID());
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

	_initActiveAttribute();
	_initActiveUniform();
	_initActiveUniformBlock();

	_programDesc = programDesc;
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

	_activeAttributes.clear();
	_activeUniforms.clear();
}

GLuint
OGLShaderObject::getInstanceID() noexcept
{
	return _program;
}

const GraphicsProgramDesc& 
OGLShaderObject::getGraphicsProgramDesc() const noexcept
{
	return _programDesc;
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

void
OGLShaderObject::_initActiveAttribute() noexcept
{
	GLint numAttribute = 0;
	GLint maxAttribute = 0;

	glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &numAttribute);
	glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribute);

	if (numAttribute > 0)
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

			auto attrib = std::make_shared<OGLShaderAttribute>();
			attrib->setName(name);
			attrib->setLocation(location);
			attrib->setSemantic(semantic);
			attrib->setSemanticIndex(semanticIndex);

			_activeAttributes.push_back(attrib);
		}
	}
}

void
OGLShaderObject::_initActiveUniform() noexcept
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

		GLuint location = glGetUniformLocation(_program, nameUniform.get());
		if (location == GL_INVALID_INDEX)
			continue;

		auto uniform = std::make_shared<OGLShaderUniform>();
		uniform->setName(nameUniform.get());
		uniform->setBindingPoint(location);
		uniform->setBindingProgram(_program);

		if (type == GL_SAMPLER_2D || type == GL_SAMPLER_3D ||
			type == GL_SAMPLER_2D_SHADOW ||
			type == GL_SAMPLER_2D_ARRAY || type == GL_SAMPLER_CUBE ||
			type == GL_SAMPLER_2D_ARRAY_SHADOW || type == GL_SAMPLER_CUBE_SHADOW)
		{
			uniform->setType(GraphicsUniformType::GraphicsUniformTypeStorageImage);
		}
		else
		{
			bool isArray = uniform->getName().find("[0]") != std::string::npos;

			if (type == GL_BOOL)
				uniform->setType(GraphicsUniformType::GraphicsUniformTypeBool);
			else if (type == GL_INT)
				uniform->setType(GraphicsUniformType::GraphicsUniformTypeInt);
			else if (type == GL_INT_VEC2)
				uniform->setType(GraphicsUniformType::GraphicsUniformTypeInt2);
			else if (type == GL_FLOAT)
			{
				if (isArray)
					uniform->setType(GraphicsUniformType::GraphicsUniformTypeFloatArray);
				else
					uniform->setType(GraphicsUniformType::GraphicsUniformTypeFloat);
			}
			else if (type == GL_FLOAT_VEC2)
			{
				if (isArray)
					uniform->setType(GraphicsUniformType::GraphicsUniformTypeFloat2Array);
				else
					uniform->setType(GraphicsUniformType::GraphicsUniformTypeFloat2);
			}
			else if (type == GL_FLOAT_VEC3)
			{
				if (isArray)
					uniform->setType(GraphicsUniformType::GraphicsUniformTypeFloat3Array);
				else
					uniform->setType(GraphicsUniformType::GraphicsUniformTypeFloat3);
			}
			else if (type == GL_FLOAT_VEC4)
			{
				if (isArray)
					uniform->setType(GraphicsUniformType::GraphicsUniformTypeFloat4Array);
				else
					uniform->setType(GraphicsUniformType::GraphicsUniformTypeFloat4);
			}
			else if (type == GL_FLOAT_MAT3)
				uniform->setType(GraphicsUniformType::GraphicsUniformTypeFloat3x3);
			else if (type == GL_FLOAT_MAT4)
				uniform->setType(GraphicsUniformType::GraphicsUniformTypeFloat4x4);
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

					glGetActiveUniformName(_program, indices[j], maxUniformLength, &length, name.data());
					varlist[j].append(name.data(), length);
				}

				auto uniformblock = std::make_shared<OGLShaderUniform>();
				uniformblock->setName(nameUniformBlock.get());
				uniformblock->setType(GraphicsUniformType::GraphicsUniformTypeUniformBuffer);
				uniformblock->setBindingPoint(location);
				uniformblock->setBindingProgram(_program);

				_activeUniforms.push_back(uniformblock);
			}
		}
	}
}

void
OGLShaderObject::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLShaderObject::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END