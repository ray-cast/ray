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
#ifndef _H_GRAPHICS_DEVICE_H_
#define _H_GRAPHICS_DEVICE_H_

#include <ray/graphics_data.h>
#include <ray/graphics_layout.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_view.h>
#include <ray/graphics_sampler.h>
#include <ray/graphics_state.h>
#include <ray/graphics_shader.h>

_NAME_BEGIN

class EXPORT GraphicsDevice : public rtti::Interface
{
	__DeclareSubInterface(GraphicsDevice, rtti::Interface)
public:
	GraphicsDevice() noexcept;
	virtual ~GraphicsDevice() noexcept;

	virtual GraphicsContextPtr createGraphicsContext(WindHandle window) noexcept = 0;
	virtual GraphicsLayoutPtr createGraphicsLayout(const GraphicsLayoutDesc& desc) noexcept = 0;
	virtual GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept = 0;
	virtual GraphicsTexturePtr createGraphicsTexture(const GraphicsTextureDesc& desc) noexcept = 0;
	virtual GraphicsSamplerPtr createGraphicsSampler(const GraphicsSamplerDesc& desc) noexcept = 0;
	virtual GraphicsRenderTexturePtr createRenderTexture(const GraphicsRenderTextureDesc& desc) noexcept = 0;
	virtual GraphicsMultiRenderTexturePtr createMultiRenderTexture(const GraphicsMultiRenderTextureDesc& desc) noexcept = 0;
	virtual GraphicsStatePtr createGraphicsState(const GraphicsStateDesc& desc) noexcept = 0;
	virtual GraphicsShaderPtr createShader(const ShaderDesc& desc) noexcept = 0;
	virtual GraphicsProgramPtr createShaderProgram(const ShaderObjectDesc& desc) noexcept = 0;

private:
	GraphicsDevice(const GraphicsDevice&) noexcept = delete;
	GraphicsDevice& operator=(const GraphicsDevice&) noexcept = delete;
};

_NAME_END

#endif