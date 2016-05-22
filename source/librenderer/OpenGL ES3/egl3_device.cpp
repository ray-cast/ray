// +---------------------------------------------------------------------
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
#include "egl3_device.h"
#include "egl3_device_context.h"
#include "egl3_swapchain.h"
#include "egl3_state.h"
#include "egl3_shader.h"
#include "egl3_texture.h"
#include "egl3_framebuffer.h"
#include "egl3_input_layout.h"
#include "egl3_sampler.h"
#include "egl3_pipeline.h"
#include "egl3_descriptor.h"
#include "egl3_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(EGL3Device, GraphicsDevice, "EGL3Device")

EGL3Device::EGL3Device() noexcept
{
}

EGL3Device::~EGL3Device() noexcept
{
	this->close();
}

bool
EGL3Device::setup(const GraphicsDeviceDesc& desc) noexcept
{
	_deviceDesc = desc;
	return true;
}

void
EGL3Device::close() noexcept
{
}

GraphicsSwapchainPtr
EGL3Device::createSwapchain(const GraphicsSwapchainDesc& desc) noexcept
{
	auto swapchain = std::make_shared<EGL3Swapchain>();
	swapchain->setDevice(this->downcast_pointer<EGL3Device>());
	if (swapchain->setup(desc))
		return swapchain;
	return false;
}

GraphicsContextPtr
EGL3Device::createDeviceContext(const GraphicsContextDesc& desc) noexcept
{
	auto context = std::make_shared<EGL3DeviceContext>();
	context->setDevice(this->downcast_pointer<EGL3Device>());
	if (context->setup(desc))
		return context;
	return false;
}

GraphicsInputLayoutPtr
EGL3Device::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	auto inputLayout = std::make_shared<EGL3InputLayout>();
	inputLayout->setDevice(this->downcast_pointer<EGL3Device>());
	if (inputLayout->setup(desc))
		return inputLayout;
	return nullptr;
}

GraphicsDataPtr
EGL3Device::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	auto data = std::make_shared<EGL3GraphicsData>();
	data->setDevice(this->downcast_pointer<EGL3Device>());
	if (data->setup(desc))
		return data;
	return nullptr;
}

GraphicsTexturePtr
EGL3Device::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<EGL3Texture>();
	texture->setDevice(this->downcast_pointer<EGL3Device>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

GraphicsSamplerPtr
EGL3Device::createSampler(const GraphicsSamplerDesc& desc) noexcept
{
	auto sampler = std::make_shared<EGL3Sampler>();
	sampler->setDevice(this->downcast_pointer<EGL3Device>());
	if (sampler->setup(desc))
		return sampler;
	return nullptr;
}

GraphicsFramebufferPtr
EGL3Device::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
{
	auto framebuffer = std::make_shared<EGL3Framebuffer>();
	framebuffer->setDevice(this->downcast_pointer<EGL3Device>());
	if (framebuffer->setup(desc))
		return framebuffer;
	return nullptr;
}

GraphicsFramebufferLayoutPtr
EGL3Device::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
{
	auto framebufferLayout = std::make_shared<EGL3FramebufferLayout>();
	framebufferLayout->setDevice(this->downcast_pointer<EGL3Device>());
	if (framebufferLayout->setup(desc))
		return framebufferLayout;
	return nullptr;
}

GraphicsStatePtr
EGL3Device::createRenderState(const GraphicsStateDesc& desc) noexcept
{
	auto state = std::make_shared<EGL3GraphicsState>();
	state->setDevice(this->downcast_pointer<EGL3Device>());
	if (state->setup(desc))
		return state;
	return nullptr;
}

GraphicsShaderPtr
EGL3Device::createShader(const GraphicsShaderDesc& desc) noexcept
{
	auto shader = std::make_shared<EGL3Shader>();
	shader->setDevice(this->downcast_pointer<EGL3Device>());
	if (shader->setup(desc))
		return shader;
	return nullptr;
}

GraphicsProgramPtr
EGL3Device::createProgram(const GraphicsProgramDesc& desc) noexcept
{
	auto program = std::make_shared<EGL3Program>();
	program->setDevice(this->downcast_pointer<EGL3Device>());
	if (program->setup(desc))
		return program;
	return nullptr;
}

GraphicsPipelinePtr
EGL3Device::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
{
	auto pipeline = std::make_shared<EGL3Pipeline>();
	pipeline->setDevice(this->downcast_pointer<EGL3Device>());
	if (pipeline->setup(desc))
		return pipeline;
	return nullptr;
}

GraphicsDescriptorSetPtr
EGL3Device::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
{
	auto descriptorSet = std::make_shared<EGL3DescriptorSet>();
	descriptorSet->setDevice(this->downcast_pointer<EGL3Device>());
	if (descriptorSet->setup(desc))
		return descriptorSet;
	return nullptr;
}

GraphicsDescriptorSetLayoutPtr
EGL3Device::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
{
	auto descriptorSetLayout = std::make_shared<EGL3DescriptorSetLayout>();
	descriptorSetLayout->setDevice(this->downcast_pointer<EGL3Device>());
	if (descriptorSetLayout->setup(desc))
		return descriptorSetLayout;
	return nullptr;
}

GraphicsDescriptorPoolPtr
EGL3Device::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
{
	auto descriptorPool = std::make_shared<EGL3DescriptorPool>();
	descriptorPool->setDevice(this->downcast_pointer<EGL3Device>());
	if (descriptorPool->setup(desc))
		return descriptorPool;
	return nullptr;
}

const GraphicsDeviceDesc&
EGL3Device::getGraphicsDeviceDesc() const noexcept
{
	return _deviceDesc;
}

_NAME_END