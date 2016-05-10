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
#include "vk_shader.h"
#include "vk_device.h"
#include "vk_system.h"

#include <GL/glew.h>

#define EXCLUDE_PSTDINT
#include <hlslcc.hpp>

#pragma warning (push)
#pragma warning (disable:4458)
#pragma warning (disable:4464)
#pragma warning (disable:4623)
#pragma warning (disable:5026)
#pragma warning (disable:5027)
#include <SPIRV/GlslangToSpv.h>
#pragma warning (pop)

_NAME_BEGIN

__ImplementSubClass(VulkanShader, GraphicsShader, "VulkanShader")
__ImplementSubClass(VulkanProgram, GraphicsProgram, "VulkanProgram")
__ImplementSubClass(VulkanGraphicsAttribute, GraphicsAttribute, "VulkanGraphicsAttribute")
__ImplementSubClass(VulkanGraphicsUniform, GraphicsUniform, "VulkanGraphicsUniform")
__ImplementSubClass(VulkanGraphicsUniformBlock, GraphicsUniformBlock, "VulkanGraphicsUniformBlock")

VulkanGraphicsAttribute::VulkanGraphicsAttribute() noexcept
	: _index(0)
	, _bindingPoint(GL_INVALID_INDEX)
	, _type(GraphicsFormat::GraphicsFormatUndefined)
{
}

VulkanGraphicsAttribute::~VulkanGraphicsAttribute() noexcept
{
}

void
VulkanGraphicsAttribute::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
VulkanGraphicsAttribute::getName() const noexcept
{
	return _name;
}

void
VulkanGraphicsAttribute::setType(GraphicsFormat type) noexcept
{
	_type = type;
}

GraphicsFormat
VulkanGraphicsAttribute::getType() const noexcept
{
	return _type;
}

void
VulkanGraphicsAttribute::setSemantic(const std::string& semantic) noexcept
{
	_semantic = semantic;
}

const std::string&
VulkanGraphicsAttribute::getSemantic() const noexcept
{
	return _semantic;
}

void
VulkanGraphicsAttribute::setSemanticIndex(std::uint8_t index) noexcept
{
	_index = index;
}

std::uint8_t
VulkanGraphicsAttribute::getSemanticIndex() const noexcept
{
	return _index;
}

void
VulkanGraphicsAttribute::setBindingPoint(GLuint bindingPoint) noexcept
{
	_bindingPoint = bindingPoint;
}

GLuint
VulkanGraphicsAttribute::getBindingPoint() const noexcept
{
	return _bindingPoint;
}

VulkanGraphicsUniform::VulkanGraphicsUniform() noexcept
	: _offset(0)
	, _bindingPoint(GL_INVALID_INDEX)
	, _type(GraphicsUniformType::GraphicsUniformTypeNone)
{
}

VulkanGraphicsUniform::~VulkanGraphicsUniform() noexcept
{
}

void
VulkanGraphicsUniform::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
VulkanGraphicsUniform::getName() const noexcept
{
	return _name;
}

void
VulkanGraphicsUniform::setType(GraphicsUniformType type) noexcept
{
	_type = type;
}

GraphicsUniformType
VulkanGraphicsUniform::getType() const noexcept
{
	return _type;
}

void
VulkanGraphicsUniform::setOffset(std::uint32_t offset) noexcept
{
	_offset = offset;
}

std::uint32_t
VulkanGraphicsUniform::getOffset() const noexcept
{
	return _offset;
}

void
VulkanGraphicsUniform::setBindingPoint(GLuint bindingPoint) noexcept
{
	_bindingPoint = bindingPoint;
}

GLuint
VulkanGraphicsUniform::getBindingPoint() const noexcept
{
	return _bindingPoint;
}

VulkanGraphicsUniformBlock::VulkanGraphicsUniformBlock() noexcept
	: _size(0)
	, _bindingPoint(GL_INVALID_INDEX)
	, _type(GraphicsUniformType::GraphicsUniformTypeUniformBuffer)
{
}

VulkanGraphicsUniformBlock::~VulkanGraphicsUniformBlock() noexcept
{
}

void
VulkanGraphicsUniformBlock::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
VulkanGraphicsUniformBlock::getName() const noexcept
{
	return _name;
}

void
VulkanGraphicsUniformBlock::setType(GraphicsUniformType type) noexcept
{
	_type = type;
}

GraphicsUniformType
VulkanGraphicsUniformBlock::getType() const noexcept
{
	return _type;
}

std::uint32_t
VulkanGraphicsUniformBlock::getOffset() const noexcept
{
	return 0;
}

void
VulkanGraphicsUniformBlock::setBlockSize(std::uint32_t size) noexcept
{
	_size = size;
}

std::uint32_t
VulkanGraphicsUniformBlock::getBlockSize() const noexcept
{
	return _size;
}

void
VulkanGraphicsUniformBlock::addGraphicsUniform(GraphicsUniformPtr uniform) noexcept
{
	_uniforms.push_back(uniform);
}

void
VulkanGraphicsUniformBlock::removeGraphicsUniform(GraphicsUniformPtr uniform) noexcept
{
	auto it = std::find(_uniforms.begin(), _uniforms.end(), uniform);
	if (it != _uniforms.end())
		_uniforms.erase(it);
}

const GraphicsUniforms&
VulkanGraphicsUniformBlock::getGraphicsUniforms() const noexcept
{
	return _uniforms;
}

void
VulkanGraphicsUniformBlock::setBindingPoint(GLuint bindingPoint) noexcept
{
	_bindingPoint = bindingPoint;
}

GLuint
VulkanGraphicsUniformBlock::getBindingPoint() const noexcept
{
	return _bindingPoint;
}

VulkanShader::VulkanShader() noexcept
	: _vkShader(VK_NULL_HANDLE)
{
}

VulkanShader::~VulkanShader() noexcept
{
	this->close();
}

bool
VulkanShader::setup(const GraphicsShaderDesc& shaderDesc) noexcept
{
	assert(_vkShader == VK_NULL_HANDLE);

	if (shaderDesc.getByteCodes().empty())
		return false;

	const char* codes = shaderDesc.getByteCodes().data();

	std::string conv;
	if (shaderDesc.getLanguage() == GraphicsShaderLang::GraphicsShaderLangHLSLbytecodes)
	{
		HlslByteCodes2GLSL(shaderDesc.getStage(), shaderDesc.getByteCodes().data(), conv);
		codes = conv.data();
	}

	std::vector<std::uint32_t> bytecodes;
	if (!GLSLtoSPV(VulkanTypes::asShaderStage(shaderDesc.getStage()), codes, bytecodes))
	{
		VK_PLATFORM_LOG("Can't conv glsl to spv.");
		return false;
	}

	VkShaderModuleCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.pNext = nullptr;
	info.codeSize = bytecodes.size() * sizeof(std::uint32_t);
	info.pCode = bytecodes.data();
	info.flags = 0;

	if (vkCreateShaderModule(this->getDevice()->downcast<VulkanDevice>()->getDevice(), &info, nullptr, &_vkShader) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkCreateShaderModule() fail.");
		return false;
	}

	_shaderDesc = shaderDesc;
	_shaderDesc.setLanguage(GraphicsShaderLang::GraphicsShaderLangGLSL);
	_shaderDesc.setByteCodes(codes);
	return true;
}

void
VulkanShader::close() noexcept
{
	if (_vkShader != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkShader, nullptr);
		_vkShader = VK_NULL_HANDLE;
	}
}

VkShaderModule
VulkanShader::getShaderModule() const noexcept
{
	return _vkShader;
}

void
VulkanShader::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
VulkanShader::getDevice() noexcept
{
	return _device.lock();
}

const GraphicsShaderDesc&
VulkanShader::getGraphicsShaderDesc() const noexcept
{
	return _shaderDesc;
}

bool
VulkanShader::HlslByteCodes2GLSL(GraphicsShaderStage stage, const char* codes, std::string& out)
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
	if (!TranslateHLSLFromMem(codes, flags, GLLang::LANG_DEFAULT, nullptr, &dependency, &shader))
	{
		FreeGLSLShader(&shader);
		return false;
	}

	out = shader.sourceCode;
	FreeGLSLShader(&shader);
	return true;
}


bool
VulkanShader::GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv)
{
	TBuiltInResource resources;
	resources.maxLights = 32;
	resources.maxClipPlanes = 6;
	resources.maxTextureUnits = 32;
	resources.maxTextureCoords = 32;
	resources.maxVertexAttribs = 64;
	resources.maxVertexUniformComponents = 4096;
	resources.maxVaryingFloats = 64;
	resources.maxVertexTextureImageUnits = 32;
	resources.maxCombinedTextureImageUnits = 80;
	resources.maxTextureImageUnits = 32;
	resources.maxFragmentUniformComponents = 4096;
	resources.maxDrawBuffers = 32;
	resources.maxVertexUniformVectors = 128;
	resources.maxVaryingVectors = 8;
	resources.maxFragmentUniformVectors = 16;
	resources.maxVertexOutputVectors = 16;
	resources.maxFragmentInputVectors = 15;
	resources.minProgramTexelOffset = -8;
	resources.maxProgramTexelOffset = 7;
	resources.maxClipDistances = 8;
	resources.maxComputeWorkGroupCountX = 65535;
	resources.maxComputeWorkGroupCountY = 65535;
	resources.maxComputeWorkGroupCountZ = 65535;
	resources.maxComputeWorkGroupSizeX = 1024;
	resources.maxComputeWorkGroupSizeY = 1024;
	resources.maxComputeWorkGroupSizeZ = 64;
	resources.maxComputeUniformComponents = 1024;
	resources.maxComputeTextureImageUnits = 16;
	resources.maxComputeImageUniforms = 8;
	resources.maxComputeAtomicCounters = 8;
	resources.maxComputeAtomicCounterBuffers = 1;
	resources.maxVaryingComponents = 60;
	resources.maxVertexOutputComponents = 64;
	resources.maxGeometryInputComponents = 64;
	resources.maxGeometryOutputComponents = 128;
	resources.maxFragmentInputComponents = 128;
	resources.maxImageUnits = 8;
	resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
	resources.maxCombinedShaderOutputResources = 8;
	resources.maxImageSamples = 0;
	resources.maxVertexImageUniforms = 0;
	resources.maxTessControlImageUniforms = 0;
	resources.maxTessEvaluationImageUniforms = 0;
	resources.maxGeometryImageUniforms = 0;
	resources.maxFragmentImageUniforms = 8;
	resources.maxCombinedImageUniforms = 8;
	resources.maxGeometryTextureImageUnits = 16;
	resources.maxGeometryOutputVertices = 256;
	resources.maxGeometryTotalOutputComponents = 1024;
	resources.maxGeometryUniformComponents = 1024;
	resources.maxGeometryVaryingComponents = 64;
	resources.maxTessControlInputComponents = 128;
	resources.maxTessControlOutputComponents = 128;
	resources.maxTessControlTextureImageUnits = 16;
	resources.maxTessControlUniformComponents = 1024;
	resources.maxTessControlTotalOutputComponents = 4096;
	resources.maxTessEvaluationInputComponents = 128;
	resources.maxTessEvaluationOutputComponents = 128;
	resources.maxTessEvaluationTextureImageUnits = 16;
	resources.maxTessEvaluationUniformComponents = 1024;
	resources.maxTessPatchComponents = 120;
	resources.maxPatchVertices = 32;
	resources.maxTessGenLevel = 64;
	resources.maxViewports = 16;
	resources.maxVertexAtomicCounters = 0;
	resources.maxTessControlAtomicCounters = 0;
	resources.maxTessEvaluationAtomicCounters = 0;
	resources.maxGeometryAtomicCounters = 0;
	resources.maxFragmentAtomicCounters = 8;
	resources.maxCombinedAtomicCounters = 8;
	resources.maxAtomicCounterBindings = 1;
	resources.maxVertexAtomicCounterBuffers = 0;
	resources.maxTessControlAtomicCounterBuffers = 0;
	resources.maxTessEvaluationAtomicCounterBuffers = 0;
	resources.maxGeometryAtomicCounterBuffers = 0;
	resources.maxFragmentAtomicCounterBuffers = 1;
	resources.maxCombinedAtomicCounterBuffers = 1;
	resources.maxAtomicCounterBufferSize = 16384;
	resources.maxTransformFeedbackBuffers = 4;
	resources.maxTransformFeedbackInterleavedComponents = 64;
	resources.maxCullDistances = 8;
	resources.maxCombinedClipAndCullDistances = 8;
	resources.maxSamples = 4;
	resources.limits.nonInductiveForLoops = 1;
	resources.limits.whileLoops = 1;
	resources.limits.doWhileLoops = 1;
	resources.limits.generalUniformIndexing = 1;
	resources.limits.generalAttributeMatrixVectorIndexing = 1;
	resources.limits.generalVaryingIndexing = 1;
	resources.limits.generalSamplerIndexing = 1;
	resources.limits.generalVariableIndexing = 1;
	resources.limits.generalConstantMatrixVectorIndexing = 1;

	EShLanguage stage = EShLangVertex;
	if (shader_type == VK_SHADER_STAGE_VERTEX_BIT)
		stage = EShLangVertex;
	else if (shader_type == VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
		stage = EShLangTessControl;
	else if (shader_type == VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
		stage = EShLangTessEvaluation;
	else if (shader_type == VK_SHADER_STAGE_GEOMETRY_BIT)
		stage = EShLangGeometry;
	else if (shader_type == VK_SHADER_STAGE_FRAGMENT_BIT)
		stage = EShLangFragment;
	else if (shader_type == VK_SHADER_STAGE_COMPUTE_BIT)
		stage = EShLangCompute;

	glslang::TShader shader(stage);
	glslang::TProgram program;

	const char *shaderStrings[1];
	shaderStrings[0] = pshader;
	shader.setStrings(shaderStrings, 1);

	EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
	if (!shader.parse(&resources, 330, false, messages))
	{
		VK_PLATFORM_LOG(shader.getInfoLog());
		VK_PLATFORM_LOG(shader.getInfoDebugLog());
		return false;
	}

	program.addShader(&shader);

	if (!program.link(messages))
	{
		VK_PLATFORM_LOG(program.getInfoLog());
		VK_PLATFORM_LOG(program.getInfoDebugLog());
		return false;
	}

	glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);
	return true;
}

VulkanProgram::VulkanProgram() noexcept
{
}

VulkanProgram::~VulkanProgram() noexcept
{
}

bool
VulkanProgram::setup(const GraphicsProgramDesc& programDesc) noexcept
{
	TBuiltInResource resources;
	resources.maxLights = 32;
	resources.maxClipPlanes = 6;
	resources.maxTextureUnits = 32;
	resources.maxTextureCoords = 32;
	resources.maxVertexAttribs = 64;
	resources.maxVertexUniformComponents = 4096;
	resources.maxVaryingFloats = 64;
	resources.maxVertexTextureImageUnits = 32;
	resources.maxCombinedTextureImageUnits = 80;
	resources.maxTextureImageUnits = 32;
	resources.maxFragmentUniformComponents = 4096;
	resources.maxDrawBuffers = 32;
	resources.maxVertexUniformVectors = 128;
	resources.maxVaryingVectors = 8;
	resources.maxFragmentUniformVectors = 16;
	resources.maxVertexOutputVectors = 16;
	resources.maxFragmentInputVectors = 15;
	resources.minProgramTexelOffset = -8;
	resources.maxProgramTexelOffset = 7;
	resources.maxClipDistances = 8;
	resources.maxComputeWorkGroupCountX = 65535;
	resources.maxComputeWorkGroupCountY = 65535;
	resources.maxComputeWorkGroupCountZ = 65535;
	resources.maxComputeWorkGroupSizeX = 1024;
	resources.maxComputeWorkGroupSizeY = 1024;
	resources.maxComputeWorkGroupSizeZ = 64;
	resources.maxComputeUniformComponents = 1024;
	resources.maxComputeTextureImageUnits = 16;
	resources.maxComputeImageUniforms = 8;
	resources.maxComputeAtomicCounters = 8;
	resources.maxComputeAtomicCounterBuffers = 1;
	resources.maxVaryingComponents = 60;
	resources.maxVertexOutputComponents = 64;
	resources.maxGeometryInputComponents = 64;
	resources.maxGeometryOutputComponents = 128;
	resources.maxFragmentInputComponents = 128;
	resources.maxImageUnits = 8;
	resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
	resources.maxCombinedShaderOutputResources = 8;
	resources.maxImageSamples = 0;
	resources.maxVertexImageUniforms = 0;
	resources.maxTessControlImageUniforms = 0;
	resources.maxTessEvaluationImageUniforms = 0;
	resources.maxGeometryImageUniforms = 0;
	resources.maxFragmentImageUniforms = 8;
	resources.maxCombinedImageUniforms = 8;
	resources.maxGeometryTextureImageUnits = 16;
	resources.maxGeometryOutputVertices = 256;
	resources.maxGeometryTotalOutputComponents = 1024;
	resources.maxGeometryUniformComponents = 1024;
	resources.maxGeometryVaryingComponents = 64;
	resources.maxTessControlInputComponents = 128;
	resources.maxTessControlOutputComponents = 128;
	resources.maxTessControlTextureImageUnits = 16;
	resources.maxTessControlUniformComponents = 1024;
	resources.maxTessControlTotalOutputComponents = 4096;
	resources.maxTessEvaluationInputComponents = 128;
	resources.maxTessEvaluationOutputComponents = 128;
	resources.maxTessEvaluationTextureImageUnits = 16;
	resources.maxTessEvaluationUniformComponents = 1024;
	resources.maxTessPatchComponents = 120;
	resources.maxPatchVertices = 32;
	resources.maxTessGenLevel = 64;
	resources.maxViewports = 16;
	resources.maxVertexAtomicCounters = 0;
	resources.maxTessControlAtomicCounters = 0;
	resources.maxTessEvaluationAtomicCounters = 0;
	resources.maxGeometryAtomicCounters = 0;
	resources.maxFragmentAtomicCounters = 8;
	resources.maxCombinedAtomicCounters = 8;
	resources.maxAtomicCounterBindings = 1;
	resources.maxVertexAtomicCounterBuffers = 0;
	resources.maxTessControlAtomicCounterBuffers = 0;
	resources.maxTessEvaluationAtomicCounterBuffers = 0;
	resources.maxGeometryAtomicCounterBuffers = 0;
	resources.maxFragmentAtomicCounterBuffers = 1;
	resources.maxCombinedAtomicCounterBuffers = 1;
	resources.maxAtomicCounterBufferSize = 16384;
	resources.maxTransformFeedbackBuffers = 4;
	resources.maxTransformFeedbackInterleavedComponents = 64;
	resources.maxCullDistances = 8;
	resources.maxCombinedClipAndCullDistances = 8;
	resources.maxSamples = 4;
	resources.limits.nonInductiveForLoops = 1;
	resources.limits.whileLoops = 1;
	resources.limits.doWhileLoops = 1;
	resources.limits.generalUniformIndexing = 1;
	resources.limits.generalAttributeMatrixVectorIndexing = 1;
	resources.limits.generalVaryingIndexing = 1;
	resources.limits.generalSamplerIndexing = 1;
	resources.limits.generalVariableIndexing = 1;
	resources.limits.generalConstantMatrixVectorIndexing = 1;

	glslang::TShader vs(EShLangVertex);
	glslang::TShader fs(EShLangFragment);
	glslang::TShader gs(EShLangGeometry);
	glslang::TShader cs(EShLangCompute);
	glslang::TShader tessControl(EShLangTessControl);
	glslang::TShader tessEvaluation(EShLangTessEvaluation);
	glslang::TShader* shaders[EShLangCount];

	shaders[EShLangVertex] = &vs;
	shaders[EShLangFragment] = &fs;
	shaders[EShLangGeometry] = &gs;
	shaders[EShLangCompute] = &cs;
	shaders[EShLangTessControl] = &tessControl;
	shaders[EShLangTessEvaluation] = &tessEvaluation;

	glslang::TProgram program;

	for (auto& it : programDesc.getShaders())
	{
		EShLanguage stage = EShLangVertex;

		auto shaderStage = it->downcast<VulkanShader>()->getGraphicsShaderDesc().getStage();
		if (shaderStage == GraphicsShaderStage::GraphicsShaderStageVertex)
			stage = EShLangVertex;
		else if (shaderStage == GraphicsShaderStage::GraphicsShaderStageFragment)
			stage = EShLangFragment;
		else if (shaderStage == GraphicsShaderStage::GraphicsShaderStageCompute)
			stage = EShLangCompute;
		else if (shaderStage == GraphicsShaderStage::GraphicsShaderStageGeometry)
			stage = EShLangGeometry;
		else if (shaderStage == GraphicsShaderStage::GraphicsShaderStageTessControl)
			stage = EShLangTessControl;
		else if (shaderStage == GraphicsShaderStage::GraphicsShaderStageTessEvaluation)
			stage = EShLangTessEvaluation;

		const char *shaderStrings[1];
		shaderStrings[0] = it->downcast<VulkanShader>()->getGraphicsShaderDesc().getByteCodes().c_str();

		shaders[stage]->setStrings(shaderStrings, 1);
		if (!shaders[stage]->parse(&resources, 330, false, (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules)))
		{
			VK_PLATFORM_LOG(shaders[stage]->getInfoLog());
			VK_PLATFORM_LOG(shaders[stage]->getInfoDebugLog());
			return false;
		}

		program.addShader(shaders[stage]);
	}

	if (!program.link(EShMessages::EShMsgDefault))
	{
		VK_PLATFORM_LOG(program.getInfoLog());
		VK_PLATFORM_LOG(program.getInfoDebugLog());
		return false;
	}

	program.buildReflection();

	std::size_t numUniforms = program.getNumLiveUniformVariables();
	for (std::size_t i = 0; i < numUniforms; i++)
	{
		auto name = program.getUniformName(i);
		auto index = program.getUniformIndex(name);
		auto type = program.getUniformType(i);
		auto offset = program.getUniformBufferOffset(i);

		auto uniformType = toGraphicsUniformType(name, type);
		if (uniformType == GraphicsUniformType::GraphicsUniformTypeSamplerImage ||
			uniformType == GraphicsUniformType::GraphicsUniformTypeStorageImage ||
			uniformType == GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler)
		{
			auto uniform = std::make_shared<VulkanGraphicsUniform>();
			uniform->setName(name);
			uniform->setType(uniformType);
			uniform->setBindingPoint(index);
			uniform->setOffset(offset);

			_activeUniforms.push_back(uniform);
		}
	}

	std::size_t numUniformBlock = program.getNumLiveUniformBlocks();
	for (std::size_t i = 0; i < numUniformBlock; i++)
	{
		auto index = program.getUniformBlockIndex(i);
		auto name = program.getUniformBlockName(i);
		auto size = program.getUniformBlockSize(i);

		if (index == -1)
			continue;

		auto uniformBlock = std::make_shared<VulkanGraphicsUniformBlock>();
		uniformBlock->setName(name);
		uniformBlock->setType(GraphicsUniformType::GraphicsUniformTypeUniformBuffer);
		uniformBlock->setBindingPoint(index);
		uniformBlock->setBlockSize(size);

		if (strncmp(name, "Globals", 7) == 0)
		{
			std::size_t numUniformsInBlock = program.getNumLiveUniformVariables();
			for (std::size_t uniformIndex = 0; uniformIndex < numUniformsInBlock; uniformIndex++)
			{
				auto uniformName = program.getUniformName(uniformIndex);
				auto uniformLocation = program.getUniformIndex(uniformName);
				auto unitoymType = program.getUniformType(uniformLocation);
				auto uniformOffset = program.getUniformBufferOffset(uniformLocation);
				auto type = toGraphicsUniformType(uniformName, unitoymType);

				if (type != GraphicsUniformType::GraphicsUniformTypeSamplerImage &&
					type != GraphicsUniformType::GraphicsUniformTypeStorageImage &&
					type != GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler)
				{
					auto uniform = std::make_shared<VulkanGraphicsUniform>();
					uniform->setName(uniformName);
					uniform->setType(type);
					uniform->setBindingPoint(uniformLocation);
					uniform->setOffset(uniformOffset);

					uniformBlock->addGraphicsUniform(uniform);
				}
			}
		}

		_activeUniformBlocks.push_back(uniformBlock);
	}

	_programDesc = programDesc;
	return true;
}

void
VulkanProgram::close() noexcept
{
	_activeAttributes.clear();
	_activeUniforms.clear();
	_activeUniformBlocks.clear();
}

const GraphicsUniforms&
VulkanProgram::getActiveUniforms() const noexcept
{
	return _activeUniforms;
}

const GraphicsUniformBlocks&
VulkanProgram::getActiveUniformBlocks() const noexcept
{
	return _activeUniformBlocks;
}

const GraphicsAttributes&
VulkanProgram::getActiveAttributes() const noexcept
{
	return _activeAttributes;
}

GraphicsUniformType
VulkanProgram::toGraphicsUniformType(const std::string& name, int type) noexcept
{
	if (type == GL_SAMPLER_2D || type == GL_SAMPLER_3D ||
		type == GL_SAMPLER_2D_SHADOW ||
		type == GL_SAMPLER_2D_ARRAY || type == GL_SAMPLER_CUBE ||
		type == GL_SAMPLER_2D_ARRAY_SHADOW || type == GL_SAMPLER_CUBE_SHADOW)
	{
		return GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler;
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
		else if (type == GL_UNSIGNED_INT)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeUIntArray;
			else
				return GraphicsUniformType::GraphicsUniformTypeUInt;
		}
		else if (type == GL_UNSIGNED_INT_VEC2)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeUInt2Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeUInt2;
		}
		else if (type == GL_UNSIGNED_INT_VEC3)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeUInt3Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeUInt3;
		}
		else if (type == GL_UNSIGNED_INT_VEC4)
		{
			if (isArray)
				return GraphicsUniformType::GraphicsUniformTypeUInt4Array;
			else
				return GraphicsUniformType::GraphicsUniformTypeUInt4;
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
			VK_PLATFORM_ASSERT(false, "Invlid uniform type");
			return GraphicsUniformType::GraphicsUniformTypeNone;
		}
	}
}

const GraphicsProgramDesc&
VulkanProgram::getGraphicsProgramDesc() const noexcept
{
	return _programDesc;
}

void
VulkanProgram::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
VulkanProgram::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END