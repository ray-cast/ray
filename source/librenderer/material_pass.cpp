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
#include <ray/material_pass.h>
#include <ray/material.h>
#include <ray/material_semantic.h>
#include <ray/graphics_device.h>
#include <ray/graphics_shader.h>
#include <ray/graphics_pipeline.h>
#include <ray/graphics_descriptor.h>
#include <ray/graphics_input_layout.h>

_NAME_BEGIN

__ImplementSubClass(MaterialPass, rtti::Interface, "MaterialPass")

MaterialParamBinding::MaterialParamBinding() noexcept
{
}

MaterialParamBinding::~MaterialParamBinding() noexcept
{
}

void
MaterialParamBinding::setMaterialParam(MaterialParamPtr param) noexcept
{
	_param = param;
}

const MaterialParamPtr&
MaterialParamBinding::getMaterialParam() const noexcept
{
	return _param;
}

void
MaterialParamBinding::setGraphicsUniformSet(GraphicsUniformSetPtr uniformSet) noexcept
{
	_uniformSet = uniformSet;
}

const GraphicsUniformSetPtr&
MaterialParamBinding::getGraphicsUniformSet() const noexcept
{
	return _uniformSet;
}

void
MaterialParamBinding::uniform1b(bool value) noexcept
{
	_uniformSet->uniform1b(value);
}

void
MaterialParamBinding::uniform1i(std::int32_t i1) noexcept
{
	_uniformSet->uniform1i(i1);
}

void
MaterialParamBinding::uniform2i(const int2& value) noexcept
{
	_uniformSet->uniform2i(value);
}

void
MaterialParamBinding::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
{
	_uniformSet->uniform2i(i1, i2);
}

void
MaterialParamBinding::uniform3i(const int3& value) noexcept
{
	_uniformSet->uniform3i(value);
}

void
MaterialParamBinding::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
{
	_uniformSet->uniform3i(i1, i2, i3);
}

void
MaterialParamBinding::uniform4i(const int4& value) noexcept
{
	_uniformSet->uniform4i(value);
}

void
MaterialParamBinding::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
{
	_uniformSet->uniform4i(i1, i2, i3, i4);
}

void
MaterialParamBinding::uniform1ui(std::uint32_t ui1) noexcept
{
	_uniformSet->uniform1ui(ui1);
}

void
MaterialParamBinding::uniform2ui(const uint2& value) noexcept
{
	_uniformSet->uniform2ui(value);
}

void
MaterialParamBinding::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
{
	_uniformSet->uniform2ui(ui1, ui2);
}

void
MaterialParamBinding::uniform3ui(const uint3& value) noexcept
{
	_uniformSet->uniform3ui(value);
}

void
MaterialParamBinding::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
{
	_uniformSet->uniform3ui(ui1, ui2, ui3);
}

void
MaterialParamBinding::uniform4ui(const uint4& value) noexcept
{
	_uniformSet->uniform4ui(value);
}

void
MaterialParamBinding::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
{
	_uniformSet->uniform4ui(ui1, ui2, ui3, ui4);
}

void
MaterialParamBinding::uniform1f(float f1) noexcept
{
	_uniformSet->uniform1f(f1);
}

void
MaterialParamBinding::uniform2f(const float2& value) noexcept
{
	_uniformSet->uniform2f(value);
}

void
MaterialParamBinding::uniform2f(float f1, float f2) noexcept
{
	_uniformSet->uniform2f(f1, f2);
}

void
MaterialParamBinding::uniform3f(const float3& value) noexcept
{
	_uniformSet->uniform3f(value);
}

void
MaterialParamBinding::uniform3f(float f1, float f2, float f3) noexcept
{
	_uniformSet->uniform3f(f1, f2, f3);
}

void
MaterialParamBinding::uniform4f(const float4& value) noexcept
{
	_uniformSet->uniform4f(value);
}

void
MaterialParamBinding::uniform4f(float f1, float f2, float f3, float f4) noexcept
{
	_uniformSet->uniform4f(f1, f2, f3, f4);
}

void
MaterialParamBinding::uniform2fmat(const float2x2& value) noexcept
{
	_uniformSet->uniform2fmat(value);
}

void
MaterialParamBinding::uniform2fmat(const float* mat2) noexcept
{
	_uniformSet->uniform2fmat(mat2);
}

void
MaterialParamBinding::uniform3fmat(const float3x3& value) noexcept
{
	_uniformSet->uniform3fmat(value);
}

void
MaterialParamBinding::uniform3fmat(const float* mat3) noexcept
{
	_uniformSet->uniform3fmat(mat3);
}

void
MaterialParamBinding::uniform4fmat(const float4x4& value) noexcept
{
	_uniformSet->uniform4fmat(value);
}

void
MaterialParamBinding::uniform4fmat(const float* mat4) noexcept
{
	_uniformSet->uniform4fmat(mat4);
}

void
MaterialParamBinding::uniform1iv(std::size_t num, const std::int32_t* i1v) noexcept
{
	_uniformSet->uniform1iv(num, i1v);
}

void
MaterialParamBinding::uniform2iv(std::size_t num, const std::int32_t* i2v) noexcept
{
	_uniformSet->uniform2iv(num, i2v);
}

void
MaterialParamBinding::uniform3iv(std::size_t num, const std::int32_t* i3v) noexcept
{
	_uniformSet->uniform3iv(num, i3v);
}

void
MaterialParamBinding::uniform4iv(std::size_t num, const std::int32_t* i4v) noexcept
{
	_uniformSet->uniform4iv(num, i4v);
}

void
MaterialParamBinding::uniform1uiv(std::size_t num, const std::uint32_t* ui1v) noexcept
{
	_uniformSet->uniform1uiv(num, ui1v);
}

void
MaterialParamBinding::uniform2uiv(std::size_t num, const std::uint32_t* ui2v) noexcept
{
	_uniformSet->uniform2uiv(num, ui2v);
}

void
MaterialParamBinding::uniform3uiv(std::size_t num, const std::uint32_t* ui3v) noexcept
{
	_uniformSet->uniform3uiv(num, ui3v);
}

void
MaterialParamBinding::uniform4uiv(std::size_t num, const std::uint32_t* ui4v) noexcept
{
	_uniformSet->uniform4uiv(num, ui4v);
}

void
MaterialParamBinding::uniform1fv(std::size_t num, const float* f1v) noexcept
{
	_uniformSet->uniform1fv(num, f1v);
}

void
MaterialParamBinding::uniform2fv(std::size_t num, const float* f2v) noexcept
{
	_uniformSet->uniform2fv(num, f2v);
}

void
MaterialParamBinding::uniform3fv(std::size_t num, const float* f3v) noexcept
{
	_uniformSet->uniform3fv(num, f3v);
}

void
MaterialParamBinding::uniform4fv(std::size_t num, const float* f4v) noexcept
{
	_uniformSet->uniform4fv(num, f4v);
}

void
MaterialParamBinding::uniform2fmatv(std::size_t num, const float* mat2) noexcept
{
	_uniformSet->uniform2fmatv(num, mat2);
}

void
MaterialParamBinding::uniform3fmatv(std::size_t num, const float* mat3) noexcept
{
	_uniformSet->uniform3fmatv(num, mat3);
}

void
MaterialParamBinding::uniform4fmatv(std::size_t num, const float* mat4) noexcept
{
	_uniformSet->uniform4fmatv(num, mat4);
}

void
MaterialParamBinding::uniform1iv(const std::vector<int1>& value) noexcept
{
	_uniformSet->uniform1iv(value);
}

void
MaterialParamBinding::uniform2iv(const std::vector<int2>& value) noexcept
{
	_uniformSet->uniform2iv(value);
}

void
MaterialParamBinding::uniform3iv(const std::vector<int3>& value) noexcept
{
	_uniformSet->uniform3iv(value);
}

void
MaterialParamBinding::uniform4iv(const std::vector<int4>& value) noexcept
{
	_uniformSet->uniform4iv(value);
}

void
MaterialParamBinding::uniform1uiv(const std::vector<uint1>& value) noexcept
{
	_uniformSet->uniform1uiv(value);
}

void
MaterialParamBinding::uniform2uiv(const std::vector<uint2>& value) noexcept
{
	_uniformSet->uniform2uiv(value);
}

void
MaterialParamBinding::uniform3uiv(const std::vector<uint3>& value) noexcept
{
	_uniformSet->uniform3uiv(value);
}

void
MaterialParamBinding::uniform4uiv(const std::vector<uint4>& value) noexcept
{
	_uniformSet->uniform4uiv(value);
}

void
MaterialParamBinding::uniform1fv(const std::vector<float1>& value) noexcept
{
	_uniformSet->uniform1fv(value);
}

void
MaterialParamBinding::uniform2fv(const std::vector<float2>& value) noexcept
{
	_uniformSet->uniform2fv(value);
}

void
MaterialParamBinding::uniform3fv(const std::vector<float3>& value) noexcept
{
	_uniformSet->uniform3fv(value);
}

void
MaterialParamBinding::uniform4fv(const std::vector<float4>& value) noexcept
{
	_uniformSet->uniform4fv(value);
}

void
MaterialParamBinding::uniform2fmatv(const std::vector<float2x2>& value) noexcept
{
	_uniformSet->uniform2fmatv(value);
}

void
MaterialParamBinding::uniform3fmatv(const std::vector<float3x3>& value) noexcept
{
	_uniformSet->uniform3fmatv(value);
}

void
MaterialParamBinding::uniform4fmatv(const std::vector<float4x4>& value) noexcept
{
	_uniformSet->uniform4fmatv(value);
}

void
MaterialParamBinding::uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	_uniformSet->uniformTexture(texture, sampler);
}

void
MaterialParamBinding::uniformBuffer(GraphicsDataPtr ubo) noexcept
{
	_uniformSet->uniformBuffer(ubo);
}


MaterialSemanticBinding::MaterialSemanticBinding() noexcept
	: _semanticType(GlobalSemanticType::GlobalSemanticTypeNone)
{
}

MaterialSemanticBinding::~MaterialSemanticBinding() noexcept
{
}

void
MaterialSemanticBinding::setSemanticType(GlobalSemanticType semanticType) noexcept
{
	_semanticType = semanticType;
}

GlobalSemanticType
MaterialSemanticBinding::getSemanticType() const noexcept
{
	return _semanticType;
}

void
MaterialSemanticBinding::setGraphicsUniformSet(GraphicsUniformSetPtr uniformSet) noexcept
{
	_uniformSet = uniformSet;
}

const GraphicsUniformSetPtr&
MaterialSemanticBinding::getGraphicsUniformSet() const noexcept
{
	return _uniformSet;
}

MaterialPass::MaterialPass() noexcept
{
}

MaterialPass::~MaterialPass() noexcept
{
	this->close();
}

bool
MaterialPass::setup(Material& material) noexcept
{
	assert(_state);
	assert(_program);
	assert(_inputLayout);

	if (!_descriptorSetLayout)
	{
		GraphicsDescriptorSetLayoutDesc descriptorSetLayoutDesc;
		descriptorSetLayoutDesc.setUniformComponents(_program->getActiveParams());
		_descriptorSetLayout = _program->getDevice()->createDescriptorSetLayout(descriptorSetLayoutDesc);
		if (!_descriptorSetLayout)
			return false;
	}

	if (!_descriptorPool)
	{
		GraphicsDescriptorPoolDesc  descriptorPoolDesc;
		descriptorPoolDesc.setMaxSets(1);
		for (auto& activeUniform : _program->getActiveParams())
		{
			auto type = activeUniform->getType();
			if (type == GraphicsUniformType::GraphicsUniformTypeStorageImage ||
				type == GraphicsUniformType::GraphicsUniformTypeSamplerImage ||
				type == GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler ||
				type == GraphicsUniformType::GraphicsUniformTypeUniformBuffer)
			{
				descriptorPoolDesc.addGraphicsDescriptorPoolComponent(GraphicsDescriptorPoolComponent(activeUniform->getType(), 1));
			}
		}

		_descriptorPool = _program->getDevice()->createDescriptorPool(descriptorPoolDesc);
	}

	GraphicsDescriptorSetDesc descriptorSetDesc;
	descriptorSetDesc.setGraphicsDescriptorSetLayout(_descriptorSetLayout);
	descriptorSetDesc.setGraphicsDescriptorPool(_descriptorPool);
	_descriptorSet = _program->getDevice()->createDescriptorSet(descriptorSetDesc);
	if (!_descriptorSet)
		return false;

	const auto& activeUniformSets = _descriptorSet->getGraphicsUniformSets();
	for (const auto& activeUniformSet : activeUniformSets)
	{
		auto activeUniform = activeUniformSet->getGraphicsParam();

		auto param = material.getParameter(activeUniform->getName());
		if (!param)
			continue;

		if (param->getType() != activeUniform->getType())
		{
			assert(false);
			continue;
		}

		if (param->getSemanticType() != GlobalSemanticType::GlobalSemanticTypeNone)
		{
			MaterialSemanticBinding binding;
			binding.setSemanticType(param->getSemanticType());
			binding.setGraphicsUniformSet(activeUniformSet);
			_bindingSemantics.push_back(binding);
		}
		else
		{
			auto binding = std::make_unique<MaterialParamBinding>();
			binding->setGraphicsUniformSet(activeUniformSet);
			binding->setMaterialParam(param);

			param->addParamListener(binding.get());
			_bindingParams.push_back(std::move(binding));
		}
	}

	if (!_inputLayout)
	{
		GraphicsInputLayoutDesc inputLayoutDesc;

		std::size_t offset = 0;
		const auto& attributes = _program->getActiveAttributes();
		for (auto& attrib : attributes)
		{
			inputLayoutDesc.addVertexLayout(GraphicsVertexLayout(0, attrib->getSemantic(), attrib->getSemanticIndex(), attrib->getType(), offset));
			offset += GraphicsVertexLayout::getVertexSize(attrib->getType());
		}

		_inputLayout = _program->getDevice()->createInputLayout(inputLayoutDesc);
		if (!_inputLayout)
			return false;
	}

	if (!_pipeline)
	{
		GraphicsPipelineDesc pipelineDesc;
		pipelineDesc.setGraphicsState(_state);
		pipelineDesc.setGraphicsProgram(_program);
		pipelineDesc.setGraphicsInputLayout(_inputLayout);
		pipelineDesc.setGraphicsDescriptorSetLayout(_descriptorSetLayout);
		_pipeline = _program->getDevice()->createRenderPipeline(pipelineDesc);
		if (!_pipeline)
			return false;
	}

	return true;
}

void
MaterialPass::close() noexcept
{
	for (auto& binding : _bindingParams)
	{
		binding->getMaterialParam()->removeParamListener(binding.get());
	}

	_bindingSemantics.clear();
	_pipeline.reset();
	_descriptorSet.reset();
	_descriptorSetLayout.reset();
}

void
MaterialPass::setName(const std::string& name) noexcept
{
	_name = name;
}

void 
MaterialPass::setName(std::string&& name) noexcept
{
	_name = std::move(name);
}

const std::string&
MaterialPass::getName() const noexcept
{
	return _name;
}

void
MaterialPass::setGraphicsState(GraphicsStatePtr& state) noexcept
{
	_state = state;
}

void
MaterialPass::setGraphicsProgram(GraphicsProgramPtr& program) noexcept
{
	_program = program;
}

void
MaterialPass::setGraphicsInputLayout(GraphicsInputLayoutPtr& inputLayout) noexcept
{
	_inputLayout = inputLayout;
}

void
MaterialPass::setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr& descriptorPool) noexcept
{
	_descriptorPool = descriptorPool;
}

void
MaterialPass::setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr& descriptorSetLayout) noexcept
{
	_descriptorSetLayout = descriptorSetLayout;
}

void
MaterialPass::setGraphicsState(GraphicsStatePtr&& state) noexcept
{
	_state = std::move(state);
}

void
MaterialPass::setGraphicsProgram(GraphicsProgramPtr&& program) noexcept
{
	_program = std::move(program);
}

void
MaterialPass::setGraphicsInputLayout(GraphicsInputLayoutPtr&& inputLayout) noexcept
{
	_inputLayout = std::move(inputLayout);
}

void
MaterialPass::setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr&& descriptorPool) noexcept
{
	_descriptorPool = std::move(descriptorPool);
}

void
MaterialPass::setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr&& descriptorSetLayout) noexcept
{
	_descriptorSetLayout = std::move(descriptorSetLayout);
}

const GraphicsStatePtr&
MaterialPass::getGraphicsState() const noexcept
{
	return _state;
}

const GraphicsProgramPtr&
MaterialPass::getGraphicsProgram() const noexcept
{
	return _program;
}

const GraphicsInputLayoutPtr&
MaterialPass::getGraphicsInputLayout() const noexcept
{
	return _inputLayout;
}

const GraphicsDescriptorPoolPtr&
MaterialPass::getGraphicsDescriptorPool() const noexcept
{
	return _descriptorPool;
}

const GraphicsDescriptorSetLayoutPtr&
MaterialPass::getGraphicsDescriptorSetLayout() const noexcept
{
	return _descriptorSetLayout;
}

const GraphicsPipelinePtr&
MaterialPass::getRenderPipeline() const noexcept
{
	return _pipeline;
}

const GraphicsDescriptorSetPtr&
MaterialPass::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

MaterialPassPtr
MaterialPass::clone() const noexcept
{
	auto pass = std::make_shared<MaterialPass>();
	pass->_name = this->_name;
	pass->_state = this->_state;
	pass->_program = this->_program;
	pass->_descriptorPool = this->_descriptorPool;
	pass->_descriptorSetLayout = this->_descriptorSetLayout;
	pass->_inputLayout = this->_inputLayout;
	pass->_pipeline = this->_pipeline;
	return pass;
}

void
MaterialPass::update(const MaterialSemanticManager& semanticManager) noexcept
{
	for (auto& it : _bindingSemantics)
	{
		auto semanticType = it.getSemanticType();
		auto& semantic = semanticManager.getSemantic(semanticType);
		auto& uniform = it.getGraphicsUniformSet();
		this->updateSemantic(*uniform, *semantic);
	}
}

void 
MaterialPass::updateSemantic(GraphicsUniformSet& uniform, const MaterialSemantic& semantic) noexcept
{
	switch (semantic.getType())
	{
	case GraphicsUniformType::GraphicsUniformTypeBool:
		uniform.uniform1b(semantic.getBool());
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt:
		uniform.uniform1i(semantic.getInt());
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt2:
		uniform.uniform2i(semantic.getInt2());
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt3:
		uniform.uniform3i(semantic.getInt3());
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt4:
		uniform.uniform4i(semantic.getInt4());
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt:
		uniform.uniform1ui(semantic.getUInt());
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt2:
		uniform.uniform2ui(semantic.getUInt2());
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt3:
		uniform.uniform3ui(semantic.getUInt3());
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt4:
		uniform.uniform4ui(semantic.getUInt4());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat:
		uniform.uniform1f(semantic.getFloat());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat2:
		uniform.uniform2f(semantic.getFloat2());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat3:
		uniform.uniform3f(semantic.getFloat3());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat4:
		uniform.uniform4f(semantic.getFloat4());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat2x2:
		uniform.uniform2fmat(semantic.getFloat2x2());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
		uniform.uniform3fmat(semantic.getFloat3x3());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
		uniform.uniform4fmat(semantic.getFloat4x4());
		break;
	case GraphicsUniformType::GraphicsUniformTypeIntArray:
		uniform.uniform1iv(semantic.getIntArray());
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt2Array:
		uniform.uniform2iv(semantic.getInt2Array());
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt3Array:
		uniform.uniform3iv(semantic.getInt3Array());
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt4Array:
		uniform.uniform4iv(semantic.getInt4Array());
		break;
	case GraphicsUniformType::GraphicsUniformTypeUIntArray:
		uniform.uniform1uiv(semantic.getUIntArray());
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt2Array:
		uniform.uniform2uiv(semantic.getUInt2Array());
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt3Array:
		uniform.uniform3uiv(semantic.getUInt3Array());
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt4Array:
		uniform.uniform4uiv(semantic.getUInt4Array());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloatArray:
		uniform.uniform1fv(semantic.getFloatArray());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
		uniform.uniform2fv(semantic.getFloat2Array());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
		uniform.uniform3fv(semantic.getFloat3Array());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
		uniform.uniform4fv(semantic.getFloat4Array());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat2x2Array:
		uniform.uniform2fmatv(semantic.getFloat2x2Array());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat3x3Array:
		uniform.uniform3fmatv(semantic.getFloat3x3Array());
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat4x4Array:
		uniform.uniform4fmatv(semantic.getFloat4x4Array());
		break;
	case GraphicsUniformType::GraphicsUniformTypeSampler:
		break;
	case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
		uniform.uniformTexture(semantic.getTexture(), semantic.getTextureSampler());
		break;
	case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
		uniform.uniformTexture(semantic.getTexture(), semantic.getTextureSampler());
		break;
	case GraphicsUniformType::GraphicsUniformTypeStorageImage:
		uniform.uniformTexture(semantic.getTexture(), semantic.getTextureSampler());
		break;
	case GraphicsUniformType::GraphicsUniformTypeStorageTexelBuffer:
		uniform.uniformBuffer(semantic.getBuffer());
		break;
	case GraphicsUniformType::GraphicsUniformTypeStorageBuffer:
		uniform.uniformBuffer(semantic.getBuffer());
		break;
	case GraphicsUniformType::GraphicsUniformTypeStorageBufferDynamic:
		uniform.uniformBuffer(semantic.getBuffer());
		break;
	case GraphicsUniformType::GraphicsUniformTypeUniformTexelBuffer:
		uniform.uniformBuffer(semantic.getBuffer());
		break;
	case GraphicsUniformType::GraphicsUniformTypeUniformBuffer:
		uniform.uniformBuffer(semantic.getBuffer());
		break;
	case GraphicsUniformType::GraphicsUniformTypeUniformBufferDynamic:
		uniform.uniformBuffer(semantic.getBuffer());
		break;
	default:
		break;
	}
}

_NAME_END