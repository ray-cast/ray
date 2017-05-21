// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#ifndef _H_GRAPHICS_DEVICE_H_
#define _H_GRAPHICS_DEVICE_H_

#include <ray/graphics_types.h>

_NAME_BEGIN

class EXPORT GraphicsDeviceDesc final
{
public:
	GraphicsDeviceDesc() noexcept;
	~GraphicsDeviceDesc() noexcept;

	void setWindHandle(WindHandle hwnd) noexcept;
	WindHandle getWindHandle() const noexcept;

	void setDeviceType(GraphicsDeviceType type) noexcept;
	GraphicsDeviceType getDeviceType() const noexcept;

private:
	WindHandle _hwnd;
	GraphicsDeviceType _deviceType;
};

class EXPORT GraphicsDevice : public rtti::Interface
{
	__DeclareSubInterface(GraphicsDevice, rtti::Interface)
public:
	GraphicsDevice() noexcept;
	virtual ~GraphicsDevice() noexcept;

	virtual GraphicsSwapchainPtr createSwapchain(const GraphicsSwapchainDesc& desc) noexcept = 0;
	virtual GraphicsContextPtr createDeviceContext(const GraphicsContextDesc& desc) noexcept = 0;
	virtual GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept = 0;
	virtual GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept = 0;
	virtual GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept = 0;
	virtual GraphicsSamplerPtr createSampler(const GraphicsSamplerDesc& desc) noexcept = 0;
	virtual GraphicsFramebufferPtr createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept = 0;
	virtual GraphicsFramebufferLayoutPtr createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept = 0;
	virtual GraphicsStatePtr createRenderState(const GraphicsStateDesc& desc) noexcept = 0;
	virtual GraphicsShaderPtr createShader(const GraphicsShaderDesc& desc) noexcept = 0;
	virtual GraphicsProgramPtr createProgram(const GraphicsProgramDesc& desc) noexcept = 0;
	virtual GraphicsPipelinePtr createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept = 0;
	virtual GraphicsDescriptorSetPtr createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept = 0;
	virtual GraphicsDescriptorSetLayoutPtr createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept = 0;
	virtual GraphicsDescriptorPoolPtr createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept = 0;

	virtual void copyDescriptorSets(GraphicsDescriptorSetPtr& source, std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept = 0;

	virtual const GraphicsDeviceProperty& getGraphicsDeviceProperty() const noexcept = 0;
	virtual const GraphicsDeviceDesc& getGraphicsDeviceDesc() const noexcept = 0;

private:
	GraphicsDevice(const GraphicsDevice&) noexcept = delete;
	GraphicsDevice& operator=(const GraphicsDevice&) noexcept = delete;
};

class EXPORT GraphicsDevice2 : public GraphicsDevice
{
	__DeclareSubInterface(GraphicsDevice2, GraphicsDevice)
public:
	GraphicsDevice2() noexcept;
	virtual ~GraphicsDevice2() noexcept;

	virtual GraphicsCommandQueuePtr createCommandQueue(const GraphicsCommandQueueDesc& desc) noexcept = 0;
	virtual GraphicsCommandPoolPtr createCommandPool(const GraphicsCommandPoolDesc& desc) noexcept = 0;
	virtual GraphicsCommandListPtr createCommandList(const GraphicsCommandListDesc& desc) noexcept = 0;
	virtual GraphicsSemaphorePtr createSemaphore(const GraphicsSemaphoreDesc& desc) noexcept = 0;

private:
	GraphicsDevice2(const GraphicsDevice2&) noexcept = delete;
	GraphicsDevice2& operator=(const GraphicsDevice2&) noexcept = delete;
};

_NAME_END

#endif