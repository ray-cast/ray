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
#ifndef _H_EGL3_DEVICE_H_
#define _H_EGL3_DEVICE_H_

#include "egl3_canvas.h"

_NAME_BEGIN

class EGL3Device final : public GraphicsDevice
{
	__DeclareSubClass(EGL3Device, GraphicsDevice)
public:
	EGL3Device() noexcept;
	~EGL3Device() noexcept;
	
	virtual GraphicsContextPtr createGraphicsContext(WindHandle hwnd) noexcept;
	virtual GraphicsStatePtr createGraphicsState() noexcept;
	virtual GraphicsLayoutPtr createGraphicsLayout(const GraphicsLayoutDesc& desc) noexcept;
	virtual GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
	virtual TexturePtr createTexture() noexcept;
	virtual GraphicsSamplerPtr createGraphicsSampler() noexcept;
	virtual RenderTexturePtr createRenderTexture() noexcept;
	virtual MultiRenderTexturePtr createMultiRenderTexture() noexcept;
	virtual ShaderPtr createShader() noexcept;
	virtual ShaderObjectPtr createShaderObject() noexcept;

private:
	EGL3Device(const EGL3Device&) noexcept = delete;
	EGL3Device& operator=(const EGL3Device&) noexcept = delete;
};

_NAME_END

#endif