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
#include <ray/graphics_device.h>
#include <ray/graphics_shader.h>
#include <ray/graphics_pipeline.h>
#include <ray/graphics_descriptor.h>

_NAME_BEGIN

__ImplementSubClass(MaterialPass, rtti::Interface, "MaterialPass")

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

	GraphicsDescriptorSetLayoutDesc descriptorSetLayoutDesc;
	descriptorSetLayoutDesc.setUniformComponents(_program->getActiveUniforms());
	descriptorSetLayoutDesc.setUniformBlockComponents(_program->getActiveUniformBlocks());
	_descriptorSetLayout = _program->getDevice()->createDescriptorSetLayout(descriptorSetLayoutDesc);
	if (!_descriptorSetLayout)
		return false;

	GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.setGraphicsState(_state);
	pipelineDesc.setGraphicsProgram(_program);
	pipelineDesc.setGraphicsInputLayout(_inputLayout);
	pipelineDesc.setGraphicsDescriptorSetLayout(_descriptorSetLayout);
	_pipeline = _program->getDevice()->createRenderPipeline(pipelineDesc);
	if (!_pipeline)
		return false;

	GraphicsDescriptorPoolDesc  descriptorPoolDesc;
	descriptorPoolDesc.setMaxSets(1);
	for (auto& activeUniform : _program->getActiveUniforms())
	{
		auto type = activeUniform->getType();
		if (type == GraphicsUniformType::GraphicsUniformTypeStorageImage ||
			type == GraphicsUniformType::GraphicsUniformTypeSamplerImage ||
			type == GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler)
		{
			descriptorPoolDesc.addGraphicsDescriptorPoolComponent(GraphicsDescriptorPoolComponent(activeUniform->getType(), 1));
		}
	}

	for (auto& activeUniformBlock : _program->getActiveUniformBlocks())
	{
		descriptorPoolDesc.addGraphicsDescriptorPoolComponent(GraphicsDescriptorPoolComponent(activeUniformBlock->getType(), 1));
	}		

	_descriptorPool = _program->getDevice()->createDescriptorPool(descriptorPoolDesc);

	GraphicsDescriptorSetDesc descriptorSetDesc;
	descriptorSetDesc.setGraphicsDescriptorSetLayout(_descriptorSetLayout);
	descriptorSetDesc.setGraphicsDescriptorPool(_descriptorPool);
	_descriptorSet = _program->getDevice()->createDescriptorSet(descriptorSetDesc);
	if (!_descriptorSet)
		return false;

	const auto& activeUniformSets = _descriptorSet->getGraphicsUniformSets();
	for (const auto& activeUniformSet : activeUniformSets)
	{
		auto activeUniform = activeUniformSet->getGraphicsUniform();
		auto uniformName = activeUniform->getName();
		if (activeUniform->getType() == GraphicsUniformType::GraphicsUniformTypeStorageImage ||
			activeUniform->getType() == GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler ||
			activeUniform->getType() == GraphicsUniformType::GraphicsUniformTypeSamplerImage)
		{
			auto pos = uniformName.find_first_of("_X_");
			if (pos != std::string::npos)
				uniformName = uniformName.substr(0, pos);
		}
		else
		{
			auto pos = uniformName.find_first_of('[');
			if (pos != std::string::npos)
				uniformName = uniformName.substr(0, pos);
		}
		
		auto param = material.getParameter(uniformName);
		if (!param)
			continue;

		if (param->getType() == activeUniform->getType())
		{
			param->addGraphicsUniform(activeUniformSet);
			_parameters.push_back(param);
		}
	}

	return true;
}

void
MaterialPass::close() noexcept
{
	if (_descriptorSet)
	{
		auto& activeUniformSets = _descriptorSet->getGraphicsUniformSets();
		for (auto& activeUniformSet : activeUniformSets)
		{
			auto activeUniform = activeUniformSet->getGraphicsUniform();
			auto param = this->getParameter(activeUniform->getName());
			if (param)
				param->removeGraphicsUniform(activeUniformSet);
		}
	}

	_parameters.clear();

	_pipeline.reset();
	_descriptorSet.reset();
	_descriptorSetLayout.reset();
}

void
MaterialPass::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
MaterialPass::getName() const noexcept
{
	return _name;
}

const MaterialParams& 
MaterialPass::getParameters() const noexcept
{
	return _parameters;
}

MaterialParamPtr
MaterialPass::getParameter(const std::string& name) const noexcept
{
	assert(!name.empty());

	for (auto& it : _parameters)
	{
		if (it->getName() == name)
		{
			return it;
		}
	}

	return nullptr;
}

void
MaterialPass::setGraphicsState(GraphicsStatePtr state) noexcept
{
	_state = state;
}

void
MaterialPass::setGraphicsProgram(GraphicsProgramPtr program) noexcept
{
	_program = program;
}

void
MaterialPass::setGraphicsInputLayout(GraphicsInputLayoutPtr inputLayout) noexcept
{
	_inputLayout = inputLayout;
}

void
MaterialPass::setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr descriptorPool) noexcept
{
	_descriptorPool = descriptorPool;
}

void
MaterialPass::setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr descriptorSetLayout) noexcept
{
	_descriptorSetLayout = descriptorSetLayout;
}

GraphicsStatePtr
MaterialPass::getGraphicsState() const noexcept
{
	return _state;
}

GraphicsProgramPtr
MaterialPass::getGraphicsProgram() const noexcept
{
	return _program;
}

GraphicsInputLayoutPtr
MaterialPass::getGraphicsInputLayout() const noexcept
{
	return _inputLayout;
}

GraphicsDescriptorPoolPtr
MaterialPass::getGraphicsDescriptorPool() const noexcept
{
	return _descriptorPool;
}

GraphicsDescriptorSetLayoutPtr
MaterialPass::getGraphicsDescriptorSetLayout() const noexcept
{
	return _descriptorSetLayout;
}

GraphicsPipelinePtr
MaterialPass::getRenderPipeline() const noexcept
{
	return _pipeline;
}

GraphicsDescriptorSetPtr 
MaterialPass::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

_NAME_END