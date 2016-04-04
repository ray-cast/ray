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
#include "egl3_render_pipeline.h"
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
	swapchain->setDevice(this->downcast<EGL3Device>());
	if (swapchain->setup(desc))
		return swapchain;
	return false;
}

GraphicsContextPtr
EGL3Device::createGraphicsContext(const GraphicsContextDesc& desc) noexcept
{
	auto context = std::make_shared<EGL3DeviceContext>();
	context->setDevice(this->downcast<EGL3Device>());
	if (context->setup(desc))
		return context;
	return false;
}

GraphicsInputLayoutPtr
EGL3Device::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	auto inputLayout = std::make_shared<EGL3InputLayout>();
	inputLayout->setDevice(this->downcast<EGL3Device>());
	if (inputLayout->setup(desc))
		return inputLayout;
	return nullptr;
}

GraphicsDataPtr
EGL3Device::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	auto data = std::make_shared<EGL3GraphicsData>();
	data->setDevice(this->downcast<EGL3Device>());
	if (data->setup(desc))
		return data;
	return nullptr;
}

GraphicsTexturePtr
EGL3Device::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<EGL3Texture>();
	texture->setDevice(this->downcast<EGL3Device>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

GraphicsSamplerPtr
EGL3Device::createSampler(const GraphicsSamplerDesc& desc) noexcept
{
	auto sampler = std::make_shared<EGL3Sampler>();
	sampler->setDevice(this->downcast<EGL3Device>());
	if (sampler->setup(desc))
		return sampler;
	return nullptr;
}

GraphicsFramebufferPtr
EGL3Device::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
{
	auto framebuffer = std::make_shared<EGL3Framebuffer>();
	framebuffer->setDevice(this->downcast<EGL3Device>());
	if (framebuffer->setup(desc))
		return framebuffer;
	return nullptr;
}

GraphicsStatePtr
EGL3Device::createRenderState(const GraphicsStateDesc& desc) noexcept
{
	auto state = std::make_shared<EGL3GraphicsState>();
	state->setDevice(this->downcast<EGL3Device>());
	if (state->setup(desc))
		return state;
	return nullptr;
}

GraphicsShaderPtr
EGL3Device::createShader(const GraphicsShaderDesc& desc) noexcept
{
	auto shader = std::make_shared<EGL3Shader>();
	shader->setDevice(this->downcast<EGL3Device>());
	if (shader->setup(desc))
		return shader;
	return nullptr;
}

GraphicsProgramPtr
EGL3Device::createProgram(const GraphicsProgramDesc& desc) noexcept
{
	auto program = std::make_shared<EGL3ShaderObject>();
	program->setDevice(this->downcast<EGL3Device>());
	if (program->setup(desc))
		return program;
	return nullptr;
}

GraphicsPipelinePtr
EGL3Device::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
{
	auto pipeline = std::make_shared<EGL3RenderPipeline>();
	pipeline->setDevice(this->downcast<EGL3Device>());
	if (pipeline->setup(desc))
		return pipeline;
	return nullptr;
}

GraphicsDescriptorSetPtr
EGL3Device::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
{
	auto descriptorSet = std::make_shared<EGL3DescriptorSet>();
	descriptorSet->setDevice(this->downcast<EGL3Device>());
	if (descriptorSet->setup(desc))
		return descriptorSet;
	return nullptr;
}

GraphicsDescriptorSetLayoutPtr
EGL3Device::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
{
	auto descriptorSetLayout = std::make_shared<EGL3DescriptorSetLayout>();
	descriptorSetLayout->setDevice(this->downcast<EGL3Device>());
	if (descriptorSetLayout->setup(desc))
		return descriptorSetLayout;
	return nullptr;
}

GraphicsFormat
EGL3Device::findCompatibleFormat(GraphicsPixelFormatDesc& desc) noexcept
{
	auto minR = desc.getMinRedBits();
	auto maxR = desc.getMaxRedBits();
	auto minG = desc.getMinGreenBits();
	auto maxG = desc.getMaxGreenBits();
	auto minB = desc.getMinBlueBits();
	auto maxB = desc.getMaxBlueBits();
	auto minA = desc.getMinAlphaBits();
	auto maxA = desc.getMaxAlphaBits();
	auto minDepth = desc.getMinDepthBits();
	auto maxDepth = desc.getMaxDepthBits();
	auto minStencil = desc.getMinStencilBits();
	auto maxStencil = desc.getMaxStencilBits();

	auto type = desc.getFormatType();
	switch (type)
	{
	case GraphicsFormatType::GraphicsFormatTypeRInt:
	{
		if (minR <= 32 && maxR >= 32)
			return GraphicsFormat::GraphicsFormatR32SInt;
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16SInt;
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8SInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRUInt:
	{
		if (minR <= 32 && maxR >= 32)
			return GraphicsFormat::GraphicsFormatR32UInt;
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16UInt;
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRSRGB:
	{
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8SRGB;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRSNorm:
	{
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16SNorm;
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8SNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRUNorm:
	{
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16UNorm;
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8UNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRSScaled:
	{
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16SScaled;
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8UScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRUScaled:
	{
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16UScaled;
		if (minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8UScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRFloat:
	{
		if (minR <= 32 && maxR >= 32)
			return GraphicsFormat::GraphicsFormatR32SFloat;
		if (minR <= 16 && maxR >= 16)
			return GraphicsFormat::GraphicsFormatR16SFloat;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGInt:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32)
			return GraphicsFormat::GraphicsFormatR32G32SInt;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16SInt;
		if (minR <= 8 && maxR >= 8 && minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8G8SInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGUInt:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32)
			return GraphicsFormat::GraphicsFormatR32G32UInt;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16UInt;
		if (minR <= 8 && maxR >= 8 && minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8G8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGSRGB:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8)
			return GraphicsFormat::GraphicsFormatR8G8SRGB;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGSNorm:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16SNorm;
		if (minR <= 8 && maxR >= 8 && minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8G8SNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGUNorm:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16UNorm;
		if (minR <= 8 && maxR >= 8 && minR <= 8 && maxR >= 8)
			return GraphicsFormat::GraphicsFormatR8G8UNorm;
		if (minR <= 4 && maxR >= 4 && minR <= 4 && maxR >= 4)
			return GraphicsFormat::GraphicsFormatR4G4UNormPack8;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGSScaled:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16SScaled;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8)
			return GraphicsFormat::GraphicsFormatR8G8SScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGUScaled:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16UScaled;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8)
			return GraphicsFormat::GraphicsFormatR8G8UScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGFloat:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32)
			return GraphicsFormat::GraphicsFormatR32G32SFloat;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16)
			return GraphicsFormat::GraphicsFormatR16G16SFloat;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBInt:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32 && minB <= 32 && maxB >= 32)
			return GraphicsFormat::GraphicsFormatR32G32B32SInt;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16SInt;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8SInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBUInt:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32 && minB <= 32 && maxB >= 32)
			return GraphicsFormat::GraphicsFormatR32G32B32UInt;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16UInt;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBSRGB:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8SRGB;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBSNorm:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16SNorm;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8SNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBUNorm:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16UNorm;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8UNorm;
		if (minR <= 5 && maxR >= 5 && minG <= 6 && maxG >= 6 && minB <= 5 && maxB >= 5)
			return GraphicsFormat::GraphicsFormatR5G6B5UNormPack16;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBSScaled:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16UScaled;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8SScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBUScaled:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16SScaled;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8UScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBFloat:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32 && minB <= 32 && maxB >= 32)
			return GraphicsFormat::GraphicsFormatR32G32B32SFloat;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16SFloat;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBAInt:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32 && minB <= 32 && maxB >= 32 && minA <= 32 && maxA >= 32)
			return GraphicsFormat::GraphicsFormatR32G32B32A32SInt;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16A16SInt;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8A8SInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBAUInt:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32 && minB <= 32 && maxB >= 32 && minA <= 32 && maxA >= 32)
			return GraphicsFormat::GraphicsFormatR32G32B32A32UInt;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16A16UInt;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8A8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBASRGB:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8A8SRGB;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBASNorm:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16SNorm;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8SNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBAUNorm:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16UNorm;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8UNorm;
		if (minR <= 4 && maxR >= 4 && minG <= 4 && maxG >= 4 && minB <= 4 && maxB >= 4 && minA <= 4 && maxA >= 4)
			return GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16;
		if (minR <= 5 && maxR >= 5 && minG <= 5 && maxG >= 5 && minB <= 5 && maxB >= 5 && minA <= 1 && maxA >= 1)
			return GraphicsFormat::GraphicsFormatR5G5B5A1UNormPack16;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBASScaled:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16SScaled;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8SScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBAUScaled:
	{
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16A16SScaled;
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatR8G8B8A8SScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeRGBAFloat:
	{
		if (minR <= 32 && maxR >= 32 && minG <= 32 && maxG >= 32 && minB <= 32 && maxB >= 32 && minA <= 32 && maxA >= 32)
			return GraphicsFormat::GraphicsFormatR32G32B32A32SFloat;
		if (minR <= 16 && maxR >= 16 && minG <= 16 && maxG >= 16 && minB <= 16 && maxB >= 16 && minA <= 16 && maxA >= 16)
			return GraphicsFormat::GraphicsFormatR16G16B16A16SFloat;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRInt:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8SInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRUInt:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRSRGB:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8SRGB;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRSNorm:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8SNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRUNorm:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8UNorm;
		if (minR <= 5 && maxR >= 5 && minG <= 6 && maxG >= 6 && minB <= 5 && maxB >= 5)
			return GraphicsFormat::GraphicsFormatB5G6R5UNormPack16;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRSScaled:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8SScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRUScaled:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8UScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRAInt:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8SInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRAUInt:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRASRGB:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8SRGB;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRASNorm:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8SNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRAUNorm:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8SRGB;
		if (minR <= 5 && maxR >= 5 && minG <= 5 && maxG >= 5 && minB <= 5 && maxB >= 5 && minA <= 1 && maxA >= 1)
			return GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16;
		if (minR <= 4 && maxR >= 4 && minG <= 4 && maxG >= 4 && minB <= 4 && maxB >= 4 && minA <= 4 && maxA >= 4)
			return GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRASScaled:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8SScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRAUScaled:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8A8UScaled;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRInt:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8)
			return GraphicsFormat::GraphicsFormatB8G8R8SInt;
		if (minR <= 10 && maxR >= 10 && minG <= 10 && maxG >= 10 && minB <= 10 && maxB >= 10 && minA <= 2 && maxA >= 2)
			return GraphicsFormat::GraphicsFormatA2B10G10R10SIntPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRUInt:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatA8B8G8R8UIntPack32;
		if (minR <= 10 && maxR >= 10 && minG <= 10 && maxG >= 10 && minB <= 10 && maxB >= 10 && minA <= 2 && maxA >= 2)
			return GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRSRGB:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRSNorm:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatA8B8G8R8SNormPack32;
		if (minR <= 10 && maxR >= 10 && minG <= 10 && maxG >= 10 && minB <= 10 && maxB >= 10 && minA <= 2 && maxA >= 2)
			return GraphicsFormat::GraphicsFormatA2B10G10R10SNormPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRUNorm:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatA8B8G8R8UNormPack32;
		if (minR <= 10 && maxR >= 10 && minG <= 10 && maxG >= 10 && minB <= 10 && maxB >= 10 && minA <= 2 && maxA >= 2)
			return GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRSScaled:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatA8B8G8R8SScaledPack32;
		if (minR <= 10 && maxR >= 10 && minG <= 10 && maxG >= 10 && minB <= 10 && maxB >= 10 && minA <= 2 && maxA >= 2)
			return GraphicsFormat::GraphicsFormatA2B10G10R10SScaledPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeABGRUScaled:
	{
		if (minR <= 8 && maxR >= 8 && minG <= 8 && maxG >= 8 && minB <= 8 && maxB >= 8 && minA <= 8 && maxA >= 8)
			return GraphicsFormat::GraphicsFormatA8B8G8R8UScaledPack32;
		if (minR <= 10 && maxR >= 10 && minG <= 10 && maxG >= 10 && minB <= 10 && maxB >= 10 && minA <= 2 && maxA >= 2)
			return GraphicsFormat::GraphicsFormatA2B10G10R10UScaledPack32;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeDepth:
	{
		if (minDepth <= 32 && maxDepth >= 32)
			return GraphicsFormat::GraphicsFormatD32_SFLOAT;
		if (minDepth <= 24 && maxDepth >= 24)
			return GraphicsFormat::GraphicsFormatX8_D24UNormPack32;
		if (minDepth <= 16 && maxDepth >= 16)
			return GraphicsFormat::GraphicsFormatD16UNorm;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeDepthStencil:
	{
		if (minDepth <= 32 && maxDepth >= 32 && minStencil <= 8 && maxStencil >= 8)
			return GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt;
		if (minDepth <= 24 && maxDepth >= 24 && minStencil <= 8 && maxStencil >= 8)
			return GraphicsFormat::GraphicsFormatD24UNorm_S8UInt;
		if (minDepth <= 16 && maxDepth >= 16 && minStencil <= 8 && maxStencil >= 8)
			return GraphicsFormat::GraphicsFormatD16UNorm_S8UInt;
	}
	break;
	case GraphicsFormatType::GraphicsFormatTypeBGRFloat:
	case GraphicsFormatType::GraphicsFormatTypeBGRAFloat:
	case GraphicsFormatType::GraphicsFormatTypeABGRFloat:
	default:
		GL_PLATFORM_ASSERT(false, "Can't support format");
		return GraphicsFormat::GraphicsFormatUndefined;
	}

	return GraphicsFormat::GraphicsFormatUndefined;
}

const GraphicsDeviceDesc&
EGL3Device::getGraphicsDeviceDesc() const noexcept
{
	return _deviceDesc;
}

_NAME_END