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
#ifndef _H_GRAPHICS_PIPELINE_H_
#define _H_GRAPHICS_PIPELINE_H_

#include <ray/graphics_child.h>

_NAME_BEGIN

class EXPORT GraphicsPipelineLayoutDesc final
{
public:
	GraphicsPipelineLayoutDesc() noexcept;
	~GraphicsPipelineLayoutDesc() noexcept;

	void setGraphicsDescriptorSetLayout(const GraphicsDescriptorSetLayouts& descriptorSetLayouts) noexcept;
	const GraphicsDescriptorSetLayouts& getGraphicsDescriptorSetLayouts() const noexcept;

	void addGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr& descriptorSetLayout) noexcept;
	void removeGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr& descriptorSetLayout) noexcept;

private:
	GraphicsDescriptorSetLayouts _descriptorSetLayouts;
};

class EXPORT GraphicsPipelineDesc final
{
public:
	GraphicsPipelineDesc() noexcept;
	~GraphicsPipelineDesc() noexcept;

	void setGraphicsProgram(GraphicsProgramPtr program) noexcept;
	GraphicsProgramPtr getGraphicsProgram() const noexcept;

	void setGraphicsInputLayout(GraphicsInputLayoutPtr inputLayout) noexcept;
	GraphicsInputLayoutPtr getGraphicsInputLayout() const noexcept;

	void setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr descriptorSet) noexcept;
	GraphicsDescriptorSetLayoutPtr getGraphicsDescriptorSetLayout() const noexcept;

	void setGraphicsFramebufferLayout(GraphicsFramebufferLayoutPtr framebufferLayout) noexcept;
	GraphicsFramebufferLayoutPtr getGraphicsFramebufferLayout() const noexcept;

	void setGraphicsState(GraphicsStatePtr state) noexcept;
	GraphicsStatePtr getGraphicsState() const noexcept;

private:
	GraphicsStatePtr _state;
	GraphicsProgramPtr _program;
	GraphicsInputLayoutPtr _inputLayout;
	GraphicsFramebufferLayoutPtr _framebufferLayout;
	GraphicsDescriptorSetLayoutPtr _descriptorSetLayout;
};

class EXPORT GraphicsPipelineLayout : public GraphicsChild
{
	__DeclareSubInterface(GraphicsPipelineLayout, GraphicsChild)
public:
	GraphicsPipelineLayout() noexcept;
	virtual ~GraphicsPipelineLayout() noexcept;

	virtual const GraphicsPipelineLayoutDesc& getGraphicsPipelineLayoutDesc() const noexcept = 0;

private:
	GraphicsPipelineLayout(const GraphicsPipelineLayout&) noexcept = delete;
	GraphicsPipelineLayout& operator=(const GraphicsPipelineLayout&) noexcept = delete;
};

class EXPORT GraphicsPipeline : public GraphicsChild
{
	__DeclareSubInterface(GraphicsPipeline, GraphicsChild)
public:
	GraphicsPipeline() noexcept;
	virtual ~GraphicsPipeline() noexcept;

	virtual const GraphicsPipelineDesc& getGraphicsPipelineDesc() const noexcept = 0;

private:
	GraphicsPipeline(const GraphicsPipeline&) noexcept = delete;
	GraphicsPipeline& operator=(const GraphicsPipeline&) noexcept = delete;
};

_NAME_END

#endif