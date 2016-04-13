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
#ifndef _H_MATERIAL_PASS_H_
#define _H_MATERIAL_PASS_H_

#include <ray/render_types.h>

_NAME_BEGIN

class EXPORT MaterialPassDesc final
{
public:
	MaterialPassDesc() noexcept;
	~MaterialPassDesc() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;
	
	void setMaterialPass(RenderPass pass) noexcept;
	RenderPass getMaterialPass() const noexcept;

	void setGraphicsState(GraphicsStatePtr& state) noexcept;
	void setGraphicsProgram(GraphicsProgramPtr& program) noexcept;
	void setGraphicsInputLayout(GraphicsInputLayoutPtr& inputLayout) noexcept;
	void setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr& pool) noexcept;
	void setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr& descriptorSetLayout) noexcept;

	GraphicsStatePtr getGraphicsState() const noexcept;
	GraphicsProgramPtr getGraphicsProgram() const noexcept;
	GraphicsInputLayoutPtr getGraphicsInputLayout() const noexcept;
	GraphicsDescriptorPoolPtr getGraphicsDescriptorPool() const noexcept;
	GraphicsDescriptorSetLayoutPtr getGraphicsDescriptorSetLayout() const noexcept;

private:
	std::string _name;

	RenderPass _pass;

	GraphicsStatePtr _state;
	GraphicsProgramPtr _program;
	GraphicsInputLayoutPtr _inputLayout;
	GraphicsDescriptorPoolPtr _descriptorPool;
	GraphicsDescriptorSetLayoutPtr _descriptorSetLayout;
};

class EXPORT MaterialPass final : public rtti::Interface
{
	__DeclareSubClass(MaterialPass, rtti::Interface)
public:
	MaterialPass() noexcept;
	~MaterialPass() noexcept;

	bool setup(Material& material) noexcept;
	void close() noexcept;
	
	const MaterialParams& getParameters() const noexcept;
	MaterialParamPtr getParameter(const std::string& name) const noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setRenderPass(RenderPass pass) noexcept;
	RenderPass getRenderPass() const noexcept;

	void setGraphicsState(GraphicsStatePtr state) noexcept;
	void setGraphicsProgram(GraphicsProgramPtr program) noexcept;
	void setGraphicsInputLayout(GraphicsInputLayoutPtr inputLayout) noexcept;
	void setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr pool) noexcept;
	void setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr descriptorSetLayout) noexcept;

	GraphicsStatePtr getGraphicsState() const noexcept;
	GraphicsProgramPtr getGraphicsProgram() const noexcept;
	GraphicsInputLayoutPtr getGraphicsInputLayout() const noexcept;
	GraphicsDescriptorPoolPtr getGraphicsDescriptorPool() const noexcept;
	GraphicsDescriptorSetLayoutPtr getGraphicsDescriptorSetLayout() const noexcept;
	GraphicsPipelinePtr getRenderPipeline() const noexcept;
	GraphicsDescriptorSetPtr getDescriptorSet() const noexcept;

private:
	MaterialPass(const MaterialPass&) = delete;
	MaterialPass& operator=(const MaterialPass&) = delete;

private:
	std::string _name;

	RenderPass _pass;
	MaterialParams _parameters;

	GraphicsStatePtr _state;
	GraphicsProgramPtr _program;
	GraphicsInputLayoutPtr _inputLayout;
	GraphicsDescriptorSetPtr _descriptorSet;
	GraphicsDescriptorPoolPtr _descriptorPool;
	GraphicsDescriptorSetLayoutPtr _descriptorSetLayout;
	GraphicsPipelinePtr _pipeline;
};

_NAME_END

#endif