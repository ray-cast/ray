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
#ifndef _H_VK_RENDER_PIPELINE_H_
#define _H_VK_RENDER_PIPELINE_H_

#include "vk_types.h"

_NAME_BEGIN

class VulkanRenderPipeline final : public GraphicsPipeline
{
	__DeclareSubClass(VulkanRenderPipeline, GraphicsPipeline)
public:
	VulkanRenderPipeline() noexcept;
	virtual ~VulkanRenderPipeline() noexcept;

	bool setup(const GraphicsPipelineDesc& pipelineDesc) noexcept;
	void close() noexcept;

	VkPipeline getPipeline() const noexcept;
	VkPipelineLayout getPipelineLayout() const noexcept;

	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

	const GraphicsPipelineDesc& getGraphicsPipelineDesc() const noexcept;

private:
	VulkanRenderPipeline(const VulkanRenderPipeline&) noexcept = delete;
	VulkanRenderPipeline& operator=(const VulkanRenderPipeline&) noexcept = delete;

private:

	VkPipeline _vkPipeline;
	VkPipelineCache _vkPipelineCache;
	VkPipelineLayout _vkPipelineLayout;

	GraphicsPipelineDesc _pipelineDesc;
	GraphicsDeviceWeakPtr _device;
};

_NAME_END

#endif