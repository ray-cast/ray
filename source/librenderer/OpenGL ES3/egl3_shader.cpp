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
#include "egl3_shader.h"

#define EXCLUDE_PSTDINT
#include <hlslcc.hpp>

_NAME_BEGIN

__ImplementSubClass(EGL3Shader, GraphicsShader, "EGL3Shader")
__ImplementSubClass(EGL3ShaderObject, GraphicsProgram, "EGL3ShaderObject")
__ImplementSubClass(EGL3GraphicsAttribute, GraphicsAttribute, "EGL3GraphicsAttribute")
__ImplementSubClass(EGL3GraphicsUniform, GraphicsUniform, "EGL3GraphicsUniform")
__ImplementSubClass(EGL3GraphicsUniformBlock, GraphicsUniformBlock, "EGL3GraphicsUniformBlock")

EGL3GraphicsAttribute::EGL3GraphicsAttribute() noexcept
	: _index(0)
	, _bindingPoint(GL_INVALID_INDEX)
	, _type(GraphicsUniformType::GraphicsUniformTypeNone)
{
}

EGL3GraphicsAttribute::~EGL3GraphicsAttribute() noexcept
{
}

void
EGL3GraphicsAttribute::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
EGL3GraphicsAttribute::getName() const noexcept
{
	return _name;
}

void
EGL3GraphicsAttribute::setType(GraphicsUniformType type) noexcept
{
	_type = type;
}

GraphicsUniformType
EGL3GraphicsAttribute::getType() const noexcept
{
	return _type;
}

void
EGL3GraphicsAttribute::setSemantic(const std::string& semantic) noexcept
{
	_semantic = semantic;
}

const std::string&
EGL3GraphicsAttribute::getSemantic() const noexcept
{
	return _semantic;
}

void
EGL3GraphicsAttribute::setSemanticIndex(std::uint8_t index) noexcept
{
	_index = index;
}

std::uint8_t
EGL3GraphicsAttribute::getSemanticIndex() const noexcept
{
	return _index;
}

void
EGL3GraphicsAttribute::setBindingPoint(GLuint bindingPoint) noexcept
{
	_bindingPoint = bindingPoint;
}

GLuint
EGL3GraphicsAttribute::getBindingPoint() const noexcept
{
	return _bindingPoint;
}

EGL3GraphicsUniform::EGL3GraphicsUniform() noexcept
	: _offset(0)
	, _bindingPoint(GL_INVALID_INDEX)
	, _type(GraphicsUniformType::GraphicsUniformTypeNone)
{
}

EGL3GraphicsUniform::~EGL3GraphicsUniform() noexcept
{
}

void
EGL3GraphicsUniform::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
EGL3GraphicsUniform::getName() const noexcept
{
	return _name;
}

void
EGL3GraphicsUniform::setType(GraphicsUniformType type) noexcept
{
	_type = type;
}

GraphicsUniformType
EGL3GraphicsUniform::getType() const noexcept
{
	return _type;
}

void
EGL3GraphicsUniform::setOffset(std::uint32_t offset) noexcept
{
	_offset = offset;
}

std::uint32_t
EGL3GraphicsUniform::getOffset() const noexcept
{
	return _offset;
}

void
EGL3GraphicsUniform::setBindingPoint(GLuint bindingPoint) noexcept
{
	_bindingPoint = bindingPoint;
}

GLuint
EGL3GraphicsUniform::getBindingPoint() const noexcept
{
	return _bindingPoint;
}

EGL3GraphicsUniformBlock::EGL3GraphicsUniformBlock() noexcept
	: _size(0)
	, _bindingPoint(GL_INVALID_INDEX)
	, _type(GraphicsUniformType::GraphicsUniformTypeUniformBuffer)
{
}

EGL3GraphicsUniformBlock::~EGL3GraphicsUniformBlock() noexcept
{
}

void
EGL3GraphicsUniformBlock::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
EGL3GraphicsUniformBlock::getName() const noexcept
{
	return _name;
}

void
EGL3GraphicsUniformBlock::setType(GraphicsUniformType type) noexcept
{
	_type = type;
}

GraphicsUniformType
EGL3GraphicsUniformBlock::getType() const noexcept
{
	return _type;
}

void
EGL3GraphicsUniformBlock::setBlockSize(std::uint32_t size) noexcept
{
	_size = size;
}

std::uint32_t
EGL3GraphicsUniformBlock::getBlockSize() const noexcept
{
	return _size;
}

void
EGL3GraphicsUniformBlock::addGraphicsUniform(GraphicsUniformPtr uniform) noexcept
{
	_uniforms.push_back(uniform);
}

void
EGL3GraphicsUniformBlock::removeGraphicsUniform(GraphicsUniformPtr uniform) noexcept
{
	auto it = std::find(_uniforms.begin(), _uniforms.end(), uniform);
	if (it != _uniforms.end())
		_uniforms.erase(it);
}

const GraphicsUniforms&
EGL3GraphicsUniformBlock::getGraphicsUniforms() const noexcept
{
	return _uniforms;
}

void
EGL3GraphicsUniformBlock::setBindingPoint(GLuint bindingPoint) noexcept
{
	_bindingPoint = bindingPoint;
}

GLuint
EGL3GraphicsUniformBlock::getBindingPoint() const noexcept
{
	return _bindingPoint;
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
EGL3Shader::setup(const GraphicsShaderDesc& shaderDesc) noexcept
{
	assert(_instance == GL_NONE);

	GLenum shaderType = EGL3Types::asShaderType(shaderDesc.getType());
	if (shaderType == GL_INVALID_ENUM)
		return false;

	if (shaderDesc.getByteCodes().empty())
	{
		GL_PLATFORM_LOG("This shader code cannot be null.");
		return false;
	}

	GLSLShader shader;
	GLSLCrossDependencyData dependency;

	std::uint32_t flags = HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT | HLSLCC_FLAG_COMBINE_TEXTURE_SAMPLERS | HLSLCC_FLAG_INOUT_APPEND_SEMANTIC_NAMES;
	if (!TranslateHLSLFromMem(shaderDesc.getByteCodes().data(), flags, GLLang::LANG_ES_310, 0, &dependency, &shader))
	{
		GL_PLATFORM_LOG("Can't conv bytecodes to glsl.");
		return false;
	}

	_instance = glCreateShader(shaderType);
	if (_instance == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateShader() fail.");
		return false;
	}

	glShaderSource(_instance, 1, &shader.sourceCode, 0);
	glCompileShader(_instance);

	FreeGLSLShader(&shader);

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
EGL3Shader::close() noexcept
{
	if (_instance != GL_NONE)
	{
		glDeleteShader(_instance);
		_instance = GL_NONE;
	}
}

GLuint
EGL3Shader::getInstanceID() const noexcept
{
	return _instance;
}

const GraphicsShaderDesc&
EGL3Shader::getGraphicsShaderDesc() const noexcept
{
	return _shaderDesc;
}

void
EGL3Shader::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3Shader::getDevice() noexcept
{
	return _device.lock();
}

EGL3ShaderObject::EGL3ShaderObject() noexcept
	: _isActive(false)
	, _program(0)
{
}

EGL3ShaderObject::~EGL3ShaderObject() noexcept
{
	this->close();
}

bool
EGL3ShaderObject::setup(const GraphicsProgramDesc& programDesc) noexcept
{
	assert(_program == GL_NONE);

	_program = glCreateProgram();

	for (auto& shader : programDesc.getShaders())
	{
		auto glshader = shader->downcast<EGL3Shader>();
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

	_isActive = false;

	_initActiveAttribute();
	_initActiveUniform();
	_initActiveUniformBlock();

	_programDesc = programDesc;
	return true;
}

void
EGL3ShaderObject::close() noexcept
{
	if (_program != GL_NONE)
	{
		glDeleteProgram(_program);
		_program = GL_NONE;
	}

	_activeAttributes.clear();
	_activeUniforms.clear();
	_activeUniformBlocks.clear();
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

const GraphicsUniforms&
EGL3ShaderObject::getActiveUniforms() const noexcept
{
	return _activeUniforms;
}

const GraphicsUniformBlocks& 
EGL3ShaderObject::getActiveUniformBlocks() const noexcept
{
	return _activeUniformBlocks;
}

const GraphicsAttributes&
EGL3ShaderObject::getActiveAttributes() const noexcept
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

			auto attrib = std::make_shared<EGL3GraphicsAttribute>();
			attrib->setName(name);
			attrib->setBindingPoint(location);
			attrib->setSemantic(semantic);
			attrib->setSemanticIndex(semanticIndex);
			attrib->setType(toGraphicsUniformType(attrib->getName(), type));

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

		GLuint location = glGetUniformLocation(_program, nameUniform.get());
		if (location == GL_INVALID_INDEX)
			continue;

		auto uniform = std::make_shared<EGL3GraphicsUniform>();
		uniform->setName(nameUniform.get());
		uniform->setBindingPoint(location);
		uniform->setType(toGraphicsUniformType(uniform->getName(), type));

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

	if (numUniformBlock == 0)
		return;

	auto nameUniformBlock = make_scope<GLchar[]>(maxUniformBlockLength + 1);
	nameUniformBlock[maxUniformBlockLength] = 0;

	for (GLint i = 0; i < numUniformBlock; ++i)
	{
		GLsizei lengthUniformBlock = 0;
		glGetActiveUniformBlockName(_program, (GLuint)i, maxUniformBlockLength, &lengthUniformBlock, nameUniformBlock.get());
		if (lengthUniformBlock == 0)
			continue;

		GLuint location = glGetUniformBlockIndex(_program, nameUniformBlock.get());
		if (location == GL_INVALID_INDEX)
			continue;

		GLint count = 0;
		glGetActiveUniformBlockiv(_program, location, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &count);
		if (count == 0)
			continue;

		GLint size = 0;
		std::vector<GLint> indices(count);
		std::vector<GLint> offset((std::size_t)count);
		std::vector<GLint> type((std::size_t)count);
		std::vector<GLchar> name(maxUniformLength);

		glGetActiveUniformBlockiv(_program, location, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
		glGetActiveUniformBlockiv(_program, location, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices.data());
		glGetActiveUniformsiv(_program, count, (GLuint*)&indices[0], GL_UNIFORM_OFFSET, &offset[0]);

		auto uniformblock = std::make_shared<EGL3GraphicsUniformBlock>();
		uniformblock->setName(nameUniformBlock.get());
		uniformblock->setBindingPoint(location);
		uniformblock->setBlockSize(size);
		uniformblock->setType(GraphicsUniformType::GraphicsUniformTypeUniformBuffer);

		for (GLint j = 0; j < count; j++)
		{
			GLsizei length = 0;
			GLsizei size;
			GLenum type;
			glGetActiveUniform(_program, indices[j], maxUniformLength, &length, &size, &type, name.data());

			auto uniform = std::make_shared<EGL3GraphicsUniform>();
			uniform->setName(std::string(name.data(), length));
			uniform->setBindingPoint(indices[j]);
			uniform->setOffset(offset[j]);
			uniform->setType(toGraphicsUniformType(uniform->getName(), type));

			uniformblock->addGraphicsUniform(uniform);
		}

		_activeUniformBlocks.push_back(uniformblock);
	}
}

GraphicsUniformType
EGL3ShaderObject::toGraphicsUniformType(const std::string& name, GLenum type) noexcept
{
	if (type == GL_SAMPLER_2D || type == GL_SAMPLER_3D ||
		type == GL_SAMPLER_2D_SHADOW ||
		type == GL_SAMPLER_2D_ARRAY || type == GL_SAMPLER_CUBE ||
		type == GL_SAMPLER_2D_ARRAY_SHADOW || type == GL_SAMPLER_CUBE_SHADOW)
	{
		return GraphicsUniformType::GraphicsUniformTypeStorageImage;
	}
	else
	{
		bool isArray = name.find("[0]") != std::string::npos;

		if (type == GL_BOOL)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeBoolArray;
			else
				return GraphicsUniformType::GraphicsUniformTypeBool;
		}
		else if (type == GL_BOOL_VEC2)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeBool2Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeBool2;
		}
		else if (type == GL_BOOL_VEC3)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeBool3Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeBool3;
		}
		else if (type == GL_BOOL_VEC4)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeBool4Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeBool4;
		}
		else if (type == GL_INT)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeIntArray;
			else
				return GraphicsUniformType::GraphicsUniformTypeInt;
		}
		else if (type == GL_INT_VEC2)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeInt2Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeInt2;
		}
		else if (type == GL_INT_VEC3)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeInt3Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeInt3;
		}
		else if (type == GL_INT_VEC4)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeInt4Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeInt4;
		}
		else if (type == GL_FLOAT)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeFloatArray;
			else
				return GraphicsUniformType::GraphicsUniformTypeFloat;
		}
		else if (type == GL_FLOAT_VEC2)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeFloat2Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeFloat2;
		}
		else if (type == GL_FLOAT_VEC3)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeFloat3Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeFloat3;
		}
		else if (type == GL_FLOAT_VEC4)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeFloat4Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeFloat4;
		}
		else if (type == GL_FLOAT_MAT2)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeFloat2x2Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeFloat2x2;
		}
		else if (type == GL_FLOAT_MAT3)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeFloat3x3Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeFloat3x3;
		}
		else if (type == GL_FLOAT_MAT4)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeFloat4x4Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeFloat4x4;
		}
		else
		{
			GL_PLATFORM_ASSERT(false, "Invlid uniform type");
			return GraphicsUniformType::GraphicsUniformTypeNone;
		}
	}
}

const GraphicsProgramDesc&
EGL3ShaderObject::getGraphicsProgramDesc() const noexcept
{
	return _programDesc;
}

void
EGL3ShaderObject::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3ShaderObject::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END