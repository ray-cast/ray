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
#include "egl2_shader.h"

#define EXCLUDE_PSTDINT
#include <hlslcc.hpp>

#include <glsl/glsl_optimizer.h>

_NAME_BEGIN

__ImplementSubClass(EGL2Shader, GraphicsShader, "EGL2Shader")
__ImplementSubClass(EGL2Program, GraphicsProgram, "EGL2Program")
__ImplementSubClass(EGL2GraphicsAttribute, GraphicsAttribute, "EGL2GraphicsAttribute")
__ImplementSubClass(EGL2GraphicsUniform, GraphicsUniform, "EGL2GraphicsUniform")
__ImplementSubClass(EGL2GraphicsUniformBlock, GraphicsUniformBlock, "EGL2GraphicsUniformBlock")

EGL2GraphicsAttribute::EGL2GraphicsAttribute() noexcept
	: _index(0)
	, _bindingPoint(GL_INVALID_INDEX)
	, _type(GraphicsFormat::GraphicsFormatUndefined)
{
}

EGL2GraphicsAttribute::~EGL2GraphicsAttribute() noexcept
{
}

void
EGL2GraphicsAttribute::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
EGL2GraphicsAttribute::getName() const noexcept
{
	return _name;
}

void
EGL2GraphicsAttribute::setType(GraphicsFormat type) noexcept
{
	_type = type;
}

GraphicsFormat
EGL2GraphicsAttribute::getType() const noexcept
{
	return _type;
}

void
EGL2GraphicsAttribute::setSemantic(const std::string& semantic) noexcept
{
	_semantic = semantic;
}

const std::string&
EGL2GraphicsAttribute::getSemantic() const noexcept
{
	return _semantic;
}

void
EGL2GraphicsAttribute::setBindingPoint(std::uint32_t bindingPoint) noexcept
{
	_bindingPoint = bindingPoint;
}

std::uint32_t
EGL2GraphicsAttribute::getBindingPoint() const noexcept
{
	return _bindingPoint;
}

EGL2GraphicsUniform::EGL2GraphicsUniform() noexcept
	: _offset(0)
	, _bindingPoint(GL_INVALID_INDEX)
	, _type(GraphicsUniformType::GraphicsUniformTypeNone)
{
}

EGL2GraphicsUniform::~EGL2GraphicsUniform() noexcept
{
}

void
EGL2GraphicsUniform::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
EGL2GraphicsUniform::getName() const noexcept
{
	return _name;
}

void
EGL2GraphicsUniform::setSamplerName(const std::string& name) noexcept
{
	_samplerName = name;
}

const std::string&
EGL2GraphicsUniform::getSamplerName() const noexcept
{
	return _samplerName;
}

void
EGL2GraphicsUniform::setType(GraphicsUniformType type) noexcept
{
	_type = type;
}

GraphicsUniformType
EGL2GraphicsUniform::getType() const noexcept
{
	return _type;
}

void
EGL2GraphicsUniform::setOffset(std::uint32_t offset) noexcept
{
	_offset = offset;
}

std::uint32_t
EGL2GraphicsUniform::getOffset() const noexcept
{
	return _offset;
}

void
EGL2GraphicsUniform::setBindingPoint(GLuint bindingPoint) noexcept
{
	_bindingPoint = bindingPoint;
}

GLuint
EGL2GraphicsUniform::getBindingPoint() const noexcept
{
	return _bindingPoint;
}

EGL2GraphicsUniformBlock::EGL2GraphicsUniformBlock() noexcept
	: _size(0)
	, _bindingPoint(GL_INVALID_INDEX)
	, _type(GraphicsUniformType::GraphicsUniformTypeUniformBuffer)
{
}

EGL2GraphicsUniformBlock::~EGL2GraphicsUniformBlock() noexcept
{
}

void
EGL2GraphicsUniformBlock::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
EGL2GraphicsUniformBlock::getName() const noexcept
{
	return _name;
}

void
EGL2GraphicsUniformBlock::setType(GraphicsUniformType type) noexcept
{
	_type = type;
}

GraphicsUniformType
EGL2GraphicsUniformBlock::getType() const noexcept
{
	return _type;
}

std::uint32_t
EGL2GraphicsUniformBlock::getOffset() const noexcept
{
	return 0;
}

void
EGL2GraphicsUniformBlock::setBlockSize(std::uint32_t size) noexcept
{
	_size = size;
}

std::uint32_t
EGL2GraphicsUniformBlock::getBlockSize() const noexcept
{
	return _size;
}

void
EGL2GraphicsUniformBlock::addGraphicsUniform(GraphicsUniformPtr uniform) noexcept
{
	_uniforms.push_back(uniform);
}

void
EGL2GraphicsUniformBlock::removeGraphicsUniform(GraphicsUniformPtr uniform) noexcept
{
	auto it = std::find(_uniforms.begin(), _uniforms.end(), uniform);
	if (it != _uniforms.end())
		_uniforms.erase(it);
}

const GraphicsUniforms&
EGL2GraphicsUniformBlock::getGraphicsUniforms() const noexcept
{
	return _uniforms;
}

void
EGL2GraphicsUniformBlock::setBindingPoint(GLuint bindingPoint) noexcept
{
	_bindingPoint = bindingPoint;
}

GLuint
EGL2GraphicsUniformBlock::getBindingPoint() const noexcept
{
	return _bindingPoint;
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
EGL2Shader::setup(const GraphicsShaderDesc& shaderDesc) noexcept
{
	assert(_instance == GL_NONE);
	assert(shaderDesc.getByteCodes().size() > 0);
	assert(EGL2Types::asShaderStage(shaderDesc.getStage()) != GL_INVALID_ENUM);

	_instance = glCreateShader(EGL2Types::asShaderStage(shaderDesc.getStage()));
	if (_instance == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateShader() fail.");
		return false;
	}

	const char* codes = shaderDesc.getByteCodes().data();

	std::string conv;
	if (shaderDesc.getLanguage() == GraphicsShaderLang::GraphicsShaderLangHLSLbytecodes)
	{
		HlslByteCodes2GLSL(shaderDesc.getStage(), codes, conv);
		codes = conv.data();
	}

	glShaderSource(_instance, 1, &codes, 0);
	glCompileShader(_instance);

	GLint result = GL_FALSE;
	glGetShaderiv(_instance, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result)
	{
		GLint length = 0;
		glGetShaderiv(_instance, GL_INFO_LOG_LENGTH, &length);

		std::string log((std::size_t)length, 0);
		glGetShaderInfoLog(_instance, length, &length, (char*)log.data());

		GL_PLATFORM_LOG(log.c_str());
		return false;
	}

	_shaderDesc = shaderDesc;
	return true;
}

void
EGL2Shader::close() noexcept
{
	if (_instance != GL_NONE)
	{
		glDeleteShader(_instance);
		_instance = GL_NONE;
	}
}

GLuint
EGL2Shader::getInstanceID() const noexcept
{
	return _instance;
}

bool
EGL2Shader::HlslByteCodes2GLSL(GraphicsShaderStage stage, const char* codes, std::string& out)
{
	std::uint32_t flags = HLSLCC_FLAG_COMBINE_TEXTURE_SAMPLERS | HLSLCC_FLAG_INOUT_APPEND_SEMANTIC_NAMES | HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT;
	if (stage == GraphicsShaderStage::GraphicsShaderStageGeometry)
		flags = HLSLCC_FLAG_GS_ENABLED;
	else if (stage == GraphicsShaderStage::GraphicsShaderStageTessControl)
		flags = HLSLCC_FLAG_TESS_ENABLED;
	else if (stage == GraphicsShaderStage::GraphicsShaderStageTessEvaluation)
		flags = HLSLCC_FLAG_TESS_ENABLED;

	GLSLShader shader;
	GLSLCrossDependencyData dependency;
	if (!TranslateHLSLFromMem(codes, flags, GLLang::LANG_ES_300, nullptr, &dependency, &shader))
	{
		FreeGLSLShader(&shader);
		return false;
	}

	if (stage == GraphicsShaderStage::GraphicsShaderStageVertex || stage == GraphicsShaderStage::GraphicsShaderStageFragment)
	{
		glslopt_shader_type glslopt_type = glslopt_shader_type::kGlslOptShaderVertex;
		if (stage == GraphicsShaderStage::GraphicsShaderStageFragment)
			glslopt_type = glslopt_shader_type::kGlslOptShaderFragment;

		auto ctx = glslopt_initialize(glslopt_target::kGlslTargetOpenGLES30);
		if (ctx)
		{
			glslopt_shader* glslopt_shader = glslopt_optimize(ctx, glslopt_type, shader.sourceCode, 0);
			bool optimizeOk = glslopt_get_status(glslopt_shader);
			if (optimizeOk)
			{
				out = glslopt_get_output(glslopt_shader);
			}

			glslopt_cleanup(ctx);
		}
	}
	else
	{
		out = shader.sourceCode;
	}
	
	FreeGLSLShader(&shader);
	return true;
}

const GraphicsShaderDesc&
EGL2Shader::getGraphicsShaderDesc() const noexcept
{
	return _shaderDesc;
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

EGL2Program::EGL2Program() noexcept
	: _program(GL_NONE)
{
}

EGL2Program::~EGL2Program() noexcept
{
	this->close();
}

bool
EGL2Program::setup(const GraphicsProgramDesc& programDesc) noexcept
{
	assert(_program == GL_NONE);

	_program = glCreateProgram();

	for (auto& shader : programDesc.getShaders())
	{
		assert(shader->isInstanceOf<EGL2Shader>());

		auto glshader = shader->downcast<EGL2Shader>();
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

		GL_PLATFORM_LOG(log.c_str());
		return false;
	}

	_initActiveAttribute();
	_initActiveUniform();

	_programDesc = programDesc;
	return true;
}

void
EGL2Program::close() noexcept
{
	if (_program != GL_NONE)
	{
		glDeleteProgram(_program);
		_program = GL_NONE;
	}

	_activeAttributes.clear();
	_activeParams.clear();
}

void
EGL2Program::apply() noexcept
{
	glUseProgram(_program);
}

GLuint
EGL2Program::getInstanceID() const noexcept
{
	return _program;
}

const GraphicsParams&
EGL2Program::getActiveParams() const noexcept
{
	return _activeParams;
}

const GraphicsAttributes&
EGL2Program::getActiveAttributes() const noexcept
{
	return _activeAttributes;
}

void
EGL2Program::_initActiveAttribute() noexcept
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

			auto it = std::find_if(semantic.begin(), semantic.end(), [](char ch) { return ch >= '0' && ch <= '9'; });
			if (it != semantic.end())
			{
				semantic = semantic.substr(0, it - semantic.begin());
			}

			auto attrib = std::make_shared<EGL2GraphicsAttribute>();
			attrib->setName(nameAttribute.get());
			attrib->setBindingPoint(location);
			attrib->setSemantic(semantic);
			attrib->setType(toGraphicsFormat(type));

			_activeAttributes.push_back(attrib);
		}
	}
}

void
EGL2Program::_initActiveUniform() noexcept
{
	GLint numUniform = 0;
	GLint maxUniformLength = 0;

	glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &numUniform);
	glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLength);

	if (numUniform == 0)
		return;

	std::string nameUniform(maxUniformLength + 1, 0);

	GLint textureUnit = 0;
	for (GLint i = 0; i < numUniform; ++i)
	{
		GLint size;
		GLenum type;
		GLsizei length;
		glGetActiveUniform(_program, (GLuint)i, maxUniformLength, &length, &size, &type, (GLchar*)nameUniform.c_str());

		GLint location = glGetUniformLocation(_program, nameUniform.c_str());
		if (location == GL_INVALID_INDEX)
			continue;

		auto uniform = std::make_shared<EGL2GraphicsUniform>();
		uniform->setName(nameUniform.substr(0, std::min((std::size_t)length, nameUniform.find('['))));
		uniform->setBindingPoint(location);
		uniform->setType(toGraphicsUniformType(nameUniform, type));

		if (type == GL_SAMPLER_2D ||
			type == GL_SAMPLER_3D_OES ||
			type == GL_SAMPLER_CUBE ||
			type == GL_SAMPLER_CUBE_MAP_ARRAY_EXT)
		{
			auto pos = nameUniform.find_first_of("_X_");
			if (pos != std::string::npos)
			{
				uniform->setName(nameUniform.substr(0, pos));
				uniform->setSamplerName(nameUniform.substr(pos + 3));
			}

			glUniform1i(location, textureUnit);
			uniform->setBindingPoint(textureUnit);
			textureUnit++;
		}

		_activeParams.push_back(uniform);
	}
}

GraphicsFormat
EGL2Program::toGraphicsFormat(GLenum type) noexcept
{
	if (type == GL_BOOL)
		return GraphicsFormat::GraphicsFormatR8UInt;
	else if (type == GL_UNSIGNED_INT)
		return GraphicsFormat::GraphicsFormatR8UInt;
	else if (type == GL_INT)
		return GraphicsFormat::GraphicsFormatR8SInt;
	else if (type == GL_INT_VEC2)
		return GraphicsFormat::GraphicsFormatR8G8SInt;
	else if (type == GL_INT_VEC3)
		return GraphicsFormat::GraphicsFormatR8G8B8SInt;
	else if (type == GL_INT_VEC4)
		return GraphicsFormat::GraphicsFormatR8G8B8A8SInt;
	else if (type == GL_FLOAT)
		return GraphicsFormat::GraphicsFormatR32SFloat;
	else if (type == GL_FLOAT_VEC2)
		return GraphicsFormat::GraphicsFormatR32G32SFloat;
	else if (type == GL_FLOAT_VEC3)
		return GraphicsFormat::GraphicsFormatR32G32B32SFloat;
	else if (type == GL_FLOAT_VEC4)
		return GraphicsFormat::GraphicsFormatR32G32B32A32SFloat;
	else if (type == GL_FLOAT_MAT2)
		return GraphicsFormat::GraphicsFormatR32G32B32A32SFloat;
	else if (type == GL_FLOAT_MAT3)
		return GraphicsFormat::GraphicsFormatR32G32B32A32SFloat;
	else if (type == GL_FLOAT_MAT4)
		return GraphicsFormat::GraphicsFormatR32G32B32A32SFloat;
	else
	{
		GL_PLATFORM_ASSERT(false, "Invlid uniform type");
		return GraphicsFormat::GraphicsFormatUndefined;
	}
}

GraphicsUniformType
EGL2Program::toGraphicsUniformType(const std::string& name, GLenum type) noexcept
{
	if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE)
	{
		return GraphicsUniformType::GraphicsUniformTypeStorageImage;
	}
	else
	{
		bool isArray = name.find("[0]") != std::string::npos;

		if (type == GL_BOOL)
		{
			return GraphicsUniformType::GraphicsUniformTypeBool;
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
EGL2Program::getGraphicsProgramDesc() const noexcept
{
	return _programDesc;
}

void
EGL2Program::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2Program::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END