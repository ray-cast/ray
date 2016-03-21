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
#include "ogl_device.h"
#include "ogl_device_context.h"
#include "ogl_canvas.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_texture.h"
#include "ogl_sampler.h"
#include "ogl_framebuffer.h"
#include "ogl_input_layout.h"
#include "ogl_vbo.h"
#include "ogl_ibo.h"
#include "ogl_render_pipeline.h"
#include "ogl_descriptor.h"
#include "ogl_canvas.h"

_NAME_BEGIN

__ImplementSubClass(OGLDevice, GraphicsDevice, "OGLDevice")

OGLDevice::OGLDevice() noexcept
{
}

OGLDevice::~OGLDevice() noexcept
{
	this->close();
}

bool
OGLDevice::open(WindHandle win) noexcept
{
	return true;
}

void
OGLDevice::close() noexcept
{
}

GraphicsSwapchainPtr 
OGLDevice::createGraphicsSwapchain(const GraphicsSwapchainDesc& desc) noexcept
{
	auto swapchain = std::make_shared<OGLCanvas>();
	swapchain->setDevice(this->downcast<OGLDevice>());
	if (swapchain->setup(desc))
		return swapchain;
	return false;
}

GraphicsContextPtr
OGLDevice::createGraphicsContext(const GraphicsContextDesc& desc) noexcept
{
	auto context = std::make_shared<OGLDeviceContext>();
	context->setDevice(this->downcast<OGLDevice>());
	if (context->setup(desc))
		return context;
	return false;
}

GraphicsInputLayoutPtr
OGLDevice::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	auto inputLayout = std::make_shared<OGLInputLayout>();
	inputLayout->setDevice(this->downcast<OGLDevice>());
	if (inputLayout->setup(desc))
		return inputLayout;
	return nullptr;
}

GraphicsDataPtr
OGLDevice::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	auto type = desc.getType();

	if (type == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer)
	{
		auto data = std::make_shared<OGLVertexBuffer>();
		data->setDevice(this->downcast<OGLDevice>());
		if (data->setup(desc))
			return data;
	}
	else if (type == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer)
	{
		auto data = std::make_shared<OGLIndexBuffer>();
		data->setDevice(this->downcast<OGLDevice>());
		if (data->setup(desc))
			return data;
	}
	else
	{
		assert(false);
	}

	return nullptr;
}

GraphicsTexturePtr
OGLDevice::createGraphicsTexture(const GraphicsTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<OGLTexture>();
	texture->setDevice(this->downcast<OGLDevice>());
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

GraphicsSamplerPtr
OGLDevice::createGraphicsSampler(const GraphicsSamplerDesc& desc) noexcept
{
	auto sampler = std::make_shared<OGLSampler>();
	sampler->setDevice(this->downcast<OGLDevice>());
	if (sampler->setup(desc))
		return sampler;
	return nullptr;
}

GraphicsRenderTexturePtr
OGLDevice::createRenderTexture(const GraphicsRenderTextureDesc& desc) noexcept
{
	auto framebuffer = std::make_shared<OGLRenderTexture>();
	framebuffer->setDevice(this->downcast<OGLDevice>());
	if (framebuffer->setup(desc))
		return framebuffer;
	return nullptr;
}

GraphicsStatePtr
OGLDevice::createRenderState(const GraphicsStateDesc& desc) noexcept
{
	auto state = std::make_shared<OGLGraphicsState>();
	state->setDevice(this->downcast<OGLDevice>());
	if (state->setup(desc))
		return state;
	return nullptr;
}

GraphicsShaderPtr
OGLDevice::createShader(const GraphicsShaderDesc& desc) noexcept
{
	auto shader = std::make_shared<OGLShader>();
	shader->setDevice(this->downcast<OGLDevice>());
	if (shader->setup(desc))
		return shader;
	return nullptr;
}

GraphicsProgramPtr
OGLDevice::createProgram(const GraphicsProgramDesc& desc) noexcept
{
	auto program = std::make_shared<OGLShaderObject>();
	program->setDevice(this->downcast<OGLDevice>());
	if (program->setup(desc))
		return program;
	return nullptr;
}

GraphicsPipelinePtr 
OGLDevice::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
{
	auto pipeline = std::make_shared<OGLRenderPipeline>();
	pipeline->setDevice(this->downcast<OGLDevice>());
	if (pipeline->setup(desc))
		return pipeline;
	return nullptr;
}

GraphicsDescriptorSetPtr 
OGLDevice::createGraphicsDescriptorSetPtr(const GraphicsDescriptorSetDesc& desc) noexcept
{
	auto descriptorSet = std::make_shared<OGLDescriptorSet>();
	descriptorSet->setDevice(this->downcast<OGLDevice>());
	if (descriptorSet->setup(desc))
		return descriptorSet;
	return nullptr;
}

GraphicsDescriptorSetLayoutPtr 
OGLDevice::createGraphicsDescriptorSetLayoutPtr(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
{
	auto descriptorSetLayout = std::make_shared<OGLDescriptorSetLayout>();
	descriptorSetLayout->setDevice(this->downcast<OGLDevice>());
	if (descriptorSetLayout->setup(desc))
		return descriptorSetLayout;
	return nullptr;
}

_NAME_END