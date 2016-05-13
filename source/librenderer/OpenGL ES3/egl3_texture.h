// +----------------------------------------------------------------------
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
#ifndef _H_EGL3_TEXTURE_H_
#define _H_EGL3_TEXTURE_H_

#include "egl3_types.h"

_NAME_BEGIN

class EGL3Texture final : public GraphicsTexture
{
	__DeclareSubClass(EGL3Texture, GraphicsTexture)
public:
	EGL3Texture() noexcept;
	~EGL3Texture() noexcept;

	bool setup(const GraphicsTextureDesc& textureDesc) noexcept;
	void close() noexcept;

	bool map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, void** data) noexcept;
	void unmap() noexcept;

	GLenum getTarget() const noexcept;
	GLuint getInstanceID() const noexcept;

	const GraphicsTextureDesc& getGraphicsTextureDesc() const noexcept;

private:
	static bool applyMipmapLimit(GLenum target, std::uint32_t min, std::uint32_t count) noexcept;
	static bool applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept;
	static bool applySamplerFilter(GLenum target, GraphicsSamplerFilter filter) noexcept;
	static bool applySamplerAnis(GLenum target, GraphicsSamplerAnis anis) noexcept;

private:
	friend class EGL3Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	GLenum _target;
	GLuint _texture;
	GLuint _pbo;
	GLsizei _pboSize;
	GraphicsTextureDesc _textureDesc;
	GraphicsDeviceWeakPtr _device;
};

_NAME_END

#endif