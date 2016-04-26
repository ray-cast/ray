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
#include <ray/graphics_pipeline.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsPipeline, GraphicsChild, "GraphicsPipeline")
__ImplementSubInterface(GraphicsPipelineLayout, GraphicsChild, "GraphicsPipelineLayout")

GraphicsPipelineLayoutDesc::GraphicsPipelineLayoutDesc() noexcept
{
}

GraphicsPipelineLayoutDesc::~GraphicsPipelineLayoutDesc() noexcept
{
}

void
GraphicsPipelineLayoutDesc::setGraphicsDescriptorSetLayout(const GraphicsDescriptorSetLayouts& descriptorSetLayout) noexcept
{
	_descriptorSetLayouts = descriptorSetLayout;
}

const GraphicsDescriptorSetLayouts&
GraphicsPipelineLayoutDesc::getGraphicsDescriptorSetLayouts() const noexcept
{
	return _descriptorSetLayouts;
}

void
GraphicsPipelineLayoutDesc::addGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr& descriptorSetLayout) noexcept
{
	auto it = std::find(_descriptorSetLayouts.begin(), _descriptorSetLayouts.end(), descriptorSetLayout);
	if (it == _descriptorSetLayouts.end())
		_descriptorSetLayouts.push_back(descriptorSetLayout);
}

void
GraphicsPipelineLayoutDesc::removeGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr& descriptorSetLayout) noexcept
{
	auto it = std::find(_descriptorSetLayouts.begin(), _descriptorSetLayouts.end(), descriptorSetLayout);
	if (it != _descriptorSetLayouts.end())
		_descriptorSetLayouts.erase(it);
}

GraphicsPipelineDesc::GraphicsPipelineDesc() noexcept
{
}

GraphicsPipelineDesc::~GraphicsPipelineDesc() noexcept
{
}

void
GraphicsPipelineDesc::setGraphicsProgram(GraphicsProgramPtr program) noexcept
{
	_program = program;
}

GraphicsProgramPtr
GraphicsPipelineDesc::getGraphicsProgram() const noexcept
{
	return _program;
}

void
GraphicsPipelineDesc::setGraphicsInputLayout(GraphicsInputLayoutPtr layout) noexcept
{
	_inputLayout = layout;
}

GraphicsInputLayoutPtr
GraphicsPipelineDesc::getGraphicsInputLayout() const noexcept
{
	return _inputLayout;
}

void
GraphicsPipelineDesc::setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr descriptorSetLayout) noexcept
{
	_descriptorSetLayout = descriptorSetLayout;
}

GraphicsDescriptorSetLayoutPtr
GraphicsPipelineDesc::getGraphicsDescriptorSetLayout() const noexcept
{
	return _descriptorSetLayout;
}

void
GraphicsPipelineDesc::setGraphicsFramebufferLayout(GraphicsFramebufferLayoutPtr framebufferLayout) noexcept
{
	_framebufferLayout = framebufferLayout;
}

GraphicsFramebufferLayoutPtr
GraphicsPipelineDesc::getGraphicsFramebufferLayout() const noexcept
{
	return _framebufferLayout;
}

void
GraphicsPipelineDesc::setGraphicsState(GraphicsStatePtr state) noexcept
{
	_state = state;
}

GraphicsStatePtr
GraphicsPipelineDesc::getGraphicsState() const noexcept
{
	return _state;
}

GraphicsPipelineLayout::GraphicsPipelineLayout() noexcept
{
}

GraphicsPipelineLayout::~GraphicsPipelineLayout() noexcept
{
}

GraphicsPipeline::GraphicsPipeline() noexcept
{
}

GraphicsPipeline::~GraphicsPipeline() noexcept
{
}

_NAME_END