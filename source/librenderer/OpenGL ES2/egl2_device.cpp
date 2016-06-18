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
#include "egl2_device.h"
#include "egl2_device_context.h"
#include "egl2_device_property.h"
#include "egl2_swapchain.h"
#include "egl2_state.h"
#include "egl2_shader.h"
#include "egl2_texture.h"
#include "egl2_framebuffer.h"
#include "egl2_input_layout.h"
#include "egl2_sampler.h"
#include "egl2_pipeline.h"
#include "egl2_descriptor.h"
#include "egl2_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(EGL2Device, GraphicsDevice, "EGL2Device")

EGL2Device::EGL2Device() noexcept
{
}

EGL2Device::~EGL2Device() noexcept
{
	this->close();
}

bool
EGL2Device::setup(const GraphicsDeviceDesc& desc) noexcept
{
	auto deviceProperty = std::make_shared<EGL2DeviceProperty>();
	if (!deviceProperty->setup(desc))
		return false;

	_deviceProperty = deviceProperty;
	_deviceDesc = desc;
	return true;
}

void
EGL2Device::close() noexcept
{
	_deviceProperty.reset();
}

GraphicsSwapchainPtr
EGL2Device::createSwapchain(const GraphicsSwapchainDesc& desc) noexcept
{
	auto swapchain = std::make_shared<EGL2Swapchain>();
	swapchain->setDevice(this->downcast_pointer<EGL2Device>());
	if (swapchain->setup(desc))
		return swapchain;
	return false;
}

GraphicsContextPtr
EGL2Device::createDeviceContext(const GraphicsContextDesc& desc) noexcept
{
	auto context = std::make_shared<EGL2DeviceContext>();
	context->setDevice(this->downcast_pointer<EGL2Device>());
	if (context->setup(desc))
	{
		_deviceContexts.push_back(context);
		return context;
	}
	return false;
}

GraphicsInputLayoutPtr
EGL2Device::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	auto inputLayout = std::make_shared<EGL2InputLayout>();
	inputLayout->setDevice(this->downcast_pointer<EGL2Device>());
	if (inputLayout->setup(desc))
		return inputLayout;
	return nullptr;
}

GraphicsDataPtr
EGL2Device::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	auto data = std::make_shared<EGL2GraphicsData>();
	data->setDevice(this->downcast_pointer<EGL2Device>());
	if (data->setup(desc))
		return data;
	return nullptr;
}

GraphicsTexturePtr
EGL2Device::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<EGL2Texture>();
	texture->setDevice(this->downcast_pointer<EGL2Device>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

GraphicsSamplerPtr
EGL2Device::createSampler(const GraphicsSamplerDesc& desc) noexcept
{
	auto sampler = std::make_shared<EGL2Sampler>();
	sampler->setDevice(this->downcast_pointer<EGL2Device>());
	if (sampler->setup(desc))
		return sampler;
	return nullptr;
}

GraphicsFramebufferPtr
EGL2Device::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
{
	auto framebuffer = std::make_shared<EGL2Framebuffer>();
	framebuffer->setDevice(this->downcast_pointer<EGL2Device>());
	if (framebuffer->setup(desc))
		return framebuffer;
	return nullptr;
}

GraphicsFramebufferLayoutPtr
EGL2Device::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
{
	auto framebufferLayout = std::make_shared<EGL2FramebufferLayout>();
	framebufferLayout->setDevice(this->downcast_pointer<EGL2Device>());
	if (framebufferLayout->setup(desc))
		return framebufferLayout;
	return nullptr;
}

GraphicsStatePtr
EGL2Device::createRenderState(const GraphicsStateDesc& desc) noexcept
{
	auto state = std::make_shared<EGL2GraphicsState>();
	state->setDevice(this->downcast_pointer<EGL2Device>());
	if (state->setup(desc))
		return state;
	return nullptr;
}

GraphicsShaderPtr
EGL2Device::createShader(const GraphicsShaderDesc& desc) noexcept
{
	auto shader = std::make_shared<EGL2Shader>();
	shader->setDevice(this->downcast_pointer<EGL2Device>());
	if (shader->setup(desc))
		return shader;
	return nullptr;
}

GraphicsProgramPtr
EGL2Device::createProgram(const GraphicsProgramDesc& desc) noexcept
{
	auto program = std::make_shared<EGL2Program>();
	program->setDevice(this->downcast_pointer<EGL2Device>());
	if (program->setup(desc))
		return program;
	return nullptr;
}

GraphicsPipelinePtr
EGL2Device::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
{
	auto pipeline = std::make_shared<EGL2Pipeline>();
	pipeline->setDevice(this->downcast_pointer<EGL2Device>());
	if (pipeline->setup(desc))
		return pipeline;
	return nullptr;
}

GraphicsDescriptorSetPtr
EGL2Device::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
{
	auto descriptorSet = std::make_shared<EGL2DescriptorSet>();
	descriptorSet->setDevice(this->downcast_pointer<EGL2Device>());
	if (descriptorSet->setup(desc))
		return descriptorSet;
	return nullptr;
}

GraphicsDescriptorSetLayoutPtr
EGL2Device::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
{
	auto descriptorSetLayout = std::make_shared<EGL2DescriptorSetLayout>();
	descriptorSetLayout->setDevice(this->downcast_pointer<EGL2Device>());
	if (descriptorSetLayout->setup(desc))
		return descriptorSetLayout;
	return nullptr;
}

GraphicsDescriptorPoolPtr
EGL2Device::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
{
	auto descriptorPool = std::make_shared<EGL2DescriptorPool>();
	descriptorPool->setDevice(this->downcast_pointer<EGL2Device>());
	if (descriptorPool->setup(desc))
		return descriptorPool;
	return nullptr;
}

void
EGL2Device::enableDebugControl(bool enable) noexcept
{
	for (auto& it : _deviceContexts)
	{
		auto deviceContext = it.lock();
		if (deviceContext)
			deviceContext->downcast<EGL2DeviceContext>()->enableDebugControl(enable);
	}
}

void
EGL2Device::copyDescriptorSets(GraphicsDescriptorSetPtr& source, std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept
{
	assert(source);
	source->downcast<EGL2DescriptorSet>()->copy(descriptorCopyCount, descriptorCopies);
}

const GraphicsDeviceProperty&
EGL2Device::getGraphicsDeviceProperty() const noexcept
{
	return *_deviceProperty;
}

const GraphicsDeviceDesc&
EGL2Device::getGraphicsDeviceDesc() const noexcept
{
	return _deviceDesc;
}

_NAME_END