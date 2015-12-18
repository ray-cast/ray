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
#include "egl2_device.h"
#include "egl2_device_context.h"
#include "egl2_canvas.h"
#include "egl2_state.h"
#include "egl2_shader.h"
#include "egl2_texture.h"
#include "egl2_layout.h"
#include "egl2_vbo.h"
#include "egl2_ibo.h"
#include "egl2_sampler.h"

_NAME_BEGIN

__ImplementSubClass(EGL2Device, GraphicsDevice, "EGL2Device")

EGL2Device::EGL2Device() noexcept
{
}

EGL2Device::~EGL2Device() noexcept
{
}

GraphicsContextPtr
EGL2Device::createGraphicsContext(WindHandle win) noexcept
{
	auto context = std::make_shared<EGL2DeviceContext>();
	context->open(win);
	return context;
}

GraphicsStatePtr
EGL2Device::createGraphicsState() noexcept
{
	return std::make_shared<EGL2GraphicsState>();
}

GraphicsLayoutPtr
EGL2Device::createGraphicsLayout(const GraphicsLayoutDesc& desc) noexcept
{
	auto layout = std::make_shared<EGL2GraphicsLayout>();
	if (layout->open(desc))
		return layout;
	return nullptr;
}

GraphicsDataPtr
EGL2Device::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	auto type = desc.getType();

	if (type == GraphicsStream::VBO)
		return std::make_shared<EGL2VertexBuffer>(desc);
	else if (type == GraphicsStream::IBO)
		return std::make_shared<EGL2IndexBuffer>(desc);
	return nullptr;
}

TexturePtr
EGL2Device::createTexture() noexcept
{
	return std::make_shared<EGL2Texture>();
}

GraphicsSamplerPtr
EGL2Device::createGraphicsSampler() noexcept
{
	return std::make_shared<EGL2Sampler>();
}

RenderTexturePtr
EGL2Device::createRenderTexture() noexcept
{
	return std::make_shared<EGL2RenderTexture>();
}

MultiRenderTexturePtr
EGL2Device::createMultiRenderTexture() noexcept
{
	return std::make_shared<EGL2MultiRenderTexture>();
}

ShaderPtr
EGL2Device::createShader() noexcept
{
	return std::make_shared<EGL2Shader>();
}

ShaderObjectPtr
EGL2Device::createShaderObject() noexcept
{
	return std::make_shared<EGL2ShaderObject>();
}

_NAME_END