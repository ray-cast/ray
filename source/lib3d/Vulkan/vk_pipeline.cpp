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
#include "vk_pipeline.h"
#include "vk_device.h"
#include "vk_shader.h"
#include "vk_input_layout.h"
#include "vk_framebuffer.h"
#include "vk_render_state.h"
#include "vk_descriptor_set.h"
#include "vk_system.h"

_NAME_BEGIN

__ImplementSubClass(VulkanPipeline, GraphicsPipeline, "VulkanPipeline")

VulkanPipeline::VulkanPipeline() noexcept
	: _vkPipeline(VK_NULL_HANDLE)
	, _vkPipelineCache(VK_NULL_HANDLE)
	, _vkPipelineLayout(VK_NULL_HANDLE)
{
}

VulkanPipeline::~VulkanPipeline() noexcept
{
	this->close();
}

bool
VulkanPipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
{
	assert(pipelineDesc.getGraphicsDescriptorSetLayout());
	//assert(pipelineDesc.getGraphicsFramebufferLayout());
	assert(pipelineDesc.getGraphicsProgram());
	assert(pipelineDesc.getGraphicsInputLayout());
	assert(pipelineDesc.getGraphicsState());
	assert(pipelineDesc.getGraphicsState()->isInstanceOf<VulkanRenderState>());
	assert(pipelineDesc.getGraphicsProgram()->isInstanceOf<VulkanProgram>());
	assert(pipelineDesc.getGraphicsInputLayout()->isInstanceOf<VulkanInputLayout>());
	assert(pipelineDesc.getGraphicsDescriptorSetLayout()->isInstanceOf<VulkanDescriptorSetLayout>());

	VkGraphicsPipelineCreateInfo pipeline;
	VkPipelineCacheCreateInfo pipelineCache;
	VkPipelineVertexInputStateCreateInfo vi;
	VkPipelineInputAssemblyStateCreateInfo ia;
	VkPipelineRasterizationStateCreateInfo rs;
	VkPipelineColorBlendStateCreateInfo cb;
	VkPipelineDepthStencilStateCreateInfo ds;
	VkPipelineViewportStateCreateInfo vp;
	VkPipelineTessellationStateCreateInfo ts;
	VkPipelineMultisampleStateCreateInfo ms;
	VkPipelineDynamicStateCreateInfo dynamicState;
	VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
	VkPipelineShaderStageCreateInfo shaderStages[GraphicsShaderStageFlagBits::GraphicsShaderStageBitCount];

	std::vector<VkVertexInputBindingDescription> vibs;
	std::vector<VkVertexInputAttributeDescription> vias;

	const auto& stateDesc = pipelineDesc.getGraphicsState()->getGraphicsStateDesc();
	const auto& inputLayoutDesc = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc();
	const auto& activeShaders = pipelineDesc.getGraphicsProgram()->downcast<VulkanProgram>()->getActiveShaders();

	memset(&pipeline, 0, sizeof(pipeline));
	memset(&pipelineCache, 0, sizeof(pipelineCache));
	memset(&vi, 0, sizeof(vi));
	memset(&ia, 0, sizeof(ia));
	memset(&rs, 0, sizeof(rs));
	memset(&cb, 0, sizeof(cb));
	memset(&ds, 0, sizeof(ds));
	memset(&vp, 0, sizeof(vp));
	memset(&ts, 0, sizeof(ts));
	memset(&ms, 0, sizeof(ms));
	memset(&dynamicState, 0, sizeof(dynamicState));
	memset(&dynamicStateEnables[0], 0, sizeof(dynamicStateEnables));
	memset(&shaderStages[0], 0, sizeof(shaderStages));

	std::uint32_t offset = 0;

	const auto& layouts = inputLayoutDesc.getVertexLayouts();
	for (auto& layout : layouts)
	{
		auto& attributes = pipelineDesc.getGraphicsProgram()->getActiveAttributes();
		for (auto& it : attributes)
		{
			if (it->getSemantic() == layout.getSemantic() &&
				it->getSemanticIndex() == layout.getSemanticIndex())
			{
				VkVertexInputAttributeDescription attr;
				attr.location = it->downcast<VulkanGraphicsAttribute>()->getBindingPoint();
				attr.binding = layout.getVertexSlot();
				attr.offset = offset + layout.getVertexOffset();
				attr.format = VulkanTypes::asGraphicsFormat(layout.getVertexFormat());

				vias.push_back(attr);
			}
		}

		offset += layout.getVertexOffset() + layout.getVertexSize();
	}

	const auto& bindings = inputLayoutDesc.getVertexBindings();
	for (auto& binding : bindings)
	{
		VkVertexInputBindingDescription vib;
		vib.binding = binding.getVertexSlot();
		vib.stride = binding.getVertexSize();

		auto divisor = binding.getVertexDivisor();
		if (divisor == GraphicsVertexDivisor::GraphicsVertexDivisorVertex)
			vib.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		else if (divisor == GraphicsVertexDivisor::GraphicsVertexDivisorInstance)
			vib.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		else
			continue;

		vibs.push_back(vib);
	}

	vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vi.vertexBindingDescriptionCount = vibs.size();
	vi.vertexAttributeDescriptionCount = vias.size();
	vi.pVertexBindingDescriptions = vibs.data();
	vi.pVertexAttributeDescriptions = vias.data();

	ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ia.topology = VulkanTypes::asPrimitiveTopology(stateDesc.getPrimitiveType());

	rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.cullMode = VulkanTypes::asCullMode(stateDesc.getCullMode());
	rs.frontFace =  VulkanTypes::asFrontFace(stateDesc.getFrontFace());
	rs.polygonMode = VulkanTypes::asPolygonMode(stateDesc.getPolygonMode());
	rs.rasterizerDiscardEnable = stateDesc.getRasterizerDiscardEnable();
	rs.depthBiasEnable = stateDesc.getDepthBiasEnable();
	rs.depthBiasConstantFactor = stateDesc.getDepthBias();
	rs.depthClampEnable = stateDesc.getDepthClampEnable();
	rs.depthBiasClamp = stateDesc.getDepthBiasClamp();
	rs.depthBiasSlopeFactor = stateDesc.getDepthSlopeScaleBias();
	
	std::vector<VkPipelineColorBlendAttachmentState> blends;
	for (auto& blend : stateDesc.getColorBlends())
	{
		VkPipelineColorBlendAttachmentState state;
		state.blendEnable = blend.getBlendEnable();
		state.srcColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
		state.dstColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		state.srcAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
		state.dstAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		state.alphaBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
		state.colorBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
		state.colorWriteMask = VkColorComponentFlagBits::VK_COLOR_COMPONENT_FLAG_BITS_MAX_ENUM;
		
		blends.push_back(state);
	}

	cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	cb.pNext = 0;
	cb.flags = 0;
	cb.blendConstants[0] = 1.0f;
	cb.blendConstants[1] = 1.0f;
	cb.blendConstants[2] = 1.0f;
	cb.blendConstants[3] = 1.0f;
	cb.logicOpEnable = false;
	cb.logicOp = VkLogicOp::VK_LOGIC_OP_AND;
	cb.attachmentCount = blends.size();
	cb.pAttachments = blends.data();

	ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	ds.depthTestEnable = stateDesc.getDepthEnable();
	ds.depthWriteEnable = stateDesc.getDepthWriteEnable();
	ds.depthCompareOp = VulkanTypes::asCompareOp(stateDesc.getDepthFunc());
	ds.depthBoundsTestEnable = stateDesc.getDepthBoundsEnable();
	ds.minDepthBounds = stateDesc.getDepthMin();
	ds.maxDepthBounds = stateDesc.getDepthMax();
	ds.stencilTestEnable = stateDesc.getStencilEnable();
	ds.front.compareMask = stateDesc.getStencilFrontReadMask();
	ds.front.compareOp = VulkanTypes::asCompareOp(stateDesc.getStencilFrontFunc());
	ds.front.depthFailOp = VulkanTypes::asStencilOp(stateDesc.getStencilFrontZFail());
	ds.front.failOp = VulkanTypes::asStencilOp(stateDesc.getStencilFrontFail());
	ds.front.passOp = VulkanTypes::asStencilOp(stateDesc.getStencilFrontPass());
	ds.front.reference = stateDesc.getStencilFrontRef();
	ds.front.writeMask = stateDesc.getStencilFrontWriteMask();
	ds.back.compareMask = stateDesc.getStencilBackReadMask();
	ds.back.compareOp = VulkanTypes::asCompareOp(stateDesc.getStencilBackFunc());
	ds.back.depthFailOp = VulkanTypes::asStencilOp(stateDesc.getStencilBackZFail());
	ds.back.failOp = VulkanTypes::asStencilOp(stateDesc.getStencilBackFail());
	ds.back.passOp = VulkanTypes::asStencilOp(stateDesc.getStencilBackPass());
	ds.back.reference = stateDesc.getStencilBackRef();
	ds.back.writeMask = stateDesc.getStencilBackWriteMask();

	vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vp.viewportCount = 1;
	vp.scissorCount = 1;

	ts.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

	ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	ms.sampleShadingEnable = stateDesc.getMultisampleEnable();

	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pDynamicStates = dynamicStateEnables;

	dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
	dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;

	for (const auto& shader : activeShaders)
	{
		shaderStages[pipeline.stageCount].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[pipeline.stageCount].module = shader->getShaderModule();
		shaderStages[pipeline.stageCount].stage = shader->getShaderStage();
		shaderStages[pipeline.stageCount].pName = "main";

		pipeline.stageCount++;
	}

	pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline.pNext = nullptr;
	pipeline.pVertexInputState = &vi;
	pipeline.pInputAssemblyState = &ia;
	pipeline.pRasterizationState = &rs;
	pipeline.pColorBlendState = &cb;
	pipeline.pDepthStencilState = &ds;
	pipeline.pViewportState = &vp;
	pipeline.pTessellationState = &ts;
	pipeline.pMultisampleState = &ms;
	pipeline.pStages = shaderStages;
	pipeline.pDynamicState = &dynamicState;
	pipeline.renderPass = VK_NULL_HANDLE; //pipelineDesc.getGraphicsFramebufferLayout()->downcast<VulkanFramebufferLayout>()->getRenderPass();

	VkDescriptorSetLayout descriptorSetLayout;
	descriptorSetLayout = pipelineDesc.getGraphicsDescriptorSetLayout()->downcast<VulkanDescriptorSetLayout>()->getDescriptorSetLayout();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(this->getDevice()->downcast<VulkanDevice>()->getDevice(), &pipelineLayoutCreateInfo, nullptr, &_vkPipelineLayout) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkCreatePipelineLayout() fail.");
		return false;
	}

	pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	if (vkCreatePipelineCache(this->getDevice()->downcast<VulkanDevice>()->getDevice(), &pipelineCache, nullptr, &_vkPipelineCache) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkCreatePipelineCache() fail.");
		return false;
	}

	pipeline.layout = _vkPipelineLayout;
	if (vkCreateGraphicsPipelines(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkPipelineCache, 1, &pipeline, nullptr, &_vkPipeline) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkCreateGraphicsPipelines() fail.");
		return false;
	}

	_pipelineDesc = pipelineDesc;
	return true;
}

void
VulkanPipeline::close() noexcept
{
	if (_vkPipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkPipeline, nullptr);
		_vkPipeline = VK_NULL_HANDLE;
	}

	if (_vkPipelineCache != VK_NULL_HANDLE)
	{
		vkDestroyPipelineCache(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkPipelineCache, nullptr);
		_vkPipelineCache = VK_NULL_HANDLE;
	}

	if (_vkPipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkPipelineLayout, nullptr);
		_vkPipelineLayout = VK_NULL_HANDLE;
	}
}

VkPipeline
VulkanPipeline::getPipeline() const noexcept
{
	return _vkPipeline;
}

VkPipelineLayout
VulkanPipeline::getPipelineLayout() const noexcept
{
	return _vkPipelineLayout;
}

VkPipelineBindPoint
VulkanPipeline::getPipelineBindPoint() const noexcept
{
	return VK_PIPELINE_BIND_POINT_GRAPHICS;
}

void
VulkanPipeline::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
VulkanPipeline::getDevice() noexcept
{
	return _device.lock();
}

const GraphicsPipelineDesc&
VulkanPipeline::getGraphicsPipelineDesc() const noexcept
{
	return _pipelineDesc;
}

_NAME_END