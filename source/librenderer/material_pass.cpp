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
	:_pass(RenderPass::RenderPassCustom)
{
}

MaterialPass::~MaterialPass() noexcept
{
	this->close();
}

void 
MaterialPass::setup(Material& material) except
{
	assert(_pipeline);

	auto& activeUniforms = _pipeline->getGraphicsPipelineDesc().getGraphicsProgram()->getActiveUniforms();
	for (auto& activeUniform : activeUniforms)
	{
		auto uniformName = activeUniform->getName();
		if (activeUniform->getType() == GraphicsUniformType::GraphicsUniformTypeStorageImage)
		{
			auto pos = uniformName.find_first_of("_X_");
			if (pos != std::string::npos)
				uniformName = uniformName.substr(0, pos);
		}

		auto param = material.getParameter(uniformName);
		if (param && param->getType() == activeUniform->getType())
		{
			auto uniform = std::make_shared<GraphicsUniform>(GraphicsShaderStage::GraphicsShaderStageNone, activeUniform->getBindingPoint(), activeUniform->getType());

			param->addShaderUniform(uniform);

			_uniforms.push_back(uniform);
			_parameters.push_back(param);
		}
		else
		{
			assert(false);
		}
	}

	GraphicsDescriptorSetLayoutDesc descriptorSetLayoutDesc;
	descriptorSetLayoutDesc.setUniformComponents(_uniforms);
	_descriptorSetLayout = _pipeline->getDevice()->createGraphicsDescriptorSetLayout(descriptorSetLayoutDesc);

	GraphicsDescriptorSetDesc descriptorSet;
	descriptorSet.setGraphicsDescriptorSetLayout(_descriptorSetLayout);
	_descriptorSet = _pipeline->getDevice()->createGraphicsDescriptorSet(descriptorSet);
}

void
MaterialPass::close() noexcept
{
	if (_pipeline)
	{
		auto& pipelineDesc = _pipeline->getGraphicsPipelineDesc();
		auto uniforms = pipelineDesc.getGraphicsProgram()->getActiveUniforms();

		for (auto& it : uniforms)
		{
			auto param = this->getParameter(it->getName());
			if (param)
			{
				for (auto& uniform : _uniforms)
				{
					if (it->getBindingPoint() == uniform->getBindingPoint())
						param->removeShaderUniform(uniform);
				}
			}
		}

		_pipeline.reset();
		_pipeline = nullptr;
	}

	_parameters.clear();
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

void
MaterialPass::setRenderPass(RenderPass pass) noexcept
{
	_pass = pass;
}

RenderPass
MaterialPass::getRenderPass() const noexcept
{
	return _pass;
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
MaterialPass::setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept
{
	_pipeline = pipeline;
}

GraphicsPipelinePtr
MaterialPass::getRenderPipeline() const noexcept
{
	return _pipeline;
}

void 
MaterialPass::setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept
{
	_descriptorSet = descriptorSet;
}

GraphicsDescriptorSetPtr 
MaterialPass::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

_NAME_END