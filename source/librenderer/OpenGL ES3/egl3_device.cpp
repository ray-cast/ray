// +---------------------------------------------------------------------
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
#include "egl3_device.h"
#include "egl3_device_context.h"
#include "egl3_canvas.h"
#include "egl3_state.h"
#include "egl3_shader.h"
#include "egl3_texture.h"
#include "egl3_framebuffer.h"
#include "egl3_layout.h"
#include "egl3_vbo.h"
#include "egl3_ibo.h"
#include "egl3_dibo.h"
#include "egl3_sampler.h"

_NAME_BEGIN

__ImplementSubClass(EGL3Device, GraphicsDevice, "EGL3Device")

EGL3Device::EGL3Device() noexcept
{
}

EGL3Device::~EGL3Device() noexcept
{
}

GraphicsContextPtr
EGL3Device::createGraphicsContext(WindHandle win) noexcept
{
	auto context = std::make_shared<EGL3DeviceContext>();
	context->open(win);
	return context;
}

GraphicsStatePtr
EGL3Device::createGraphicsState() noexcept
{
	return std::make_shared<EGL3GraphicsState>();
}

GraphicsLayoutPtr
EGL3Device::createGraphicsLayout(const GraphicsLayoutDesc& desc) noexcept
{
	auto layout = std::make_shared<EGL3GraphicsLayout>();
	if (layout->open(desc))
		return layout;
	return nullptr;
}

GraphicsDataPtr
EGL3Device::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	auto type = desc.getType();

	if (type == GraphicsStream::VBO)
		return std::make_shared<EGL3VertexBuffer>(desc);
	else if (type == GraphicsStream::IBO)
		return std::make_shared<EGL3IndexBuffer>(desc);
	else if (type == GraphicsStream::DIBO)
		return std::make_shared<EGL3DrawIndirectBuffer>(desc);

	return nullptr;
}

GraphicsTexturePtr
EGL3Device::createGraphicsTexture(const GraphicsTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<EGL3Texture>();
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

GraphicsSamplerPtr
EGL3Device::createGraphicsSampler(const GraphicsSamplerDesc& desc) noexcept
{
	auto sampler = std::make_shared<EGL3Sampler>();
	if (sampler->setup(desc))
		return sampler;
	return nullptr;
}

GraphicsRenderTexturePtr
EGL3Device::createRenderTexture(const GraphicsRenderTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<EGL3RenderTexture>();
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

GraphicsMultiRenderTexturePtr
EGL3Device::createMultiRenderTexture(const GraphicsMultiRenderTextureDesc& desc) noexcept
{
	auto texture = std::make_shared<EGL3MultiRenderTexture>();
	if (texture->setup(desc))
		return texture;
	return nullptr;
}

ShaderPtr
EGL3Device::createShader() noexcept
{
	return std::make_shared<EGL3Shader>();
}

ShaderObjectPtr
EGL3Device::createShaderObject() noexcept
{
	return std::make_shared<EGL3ShaderObject>();
}

_NAME_END