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
#ifndef _H_EGL3_TEXTURE_H_
#define _H_EGL3_TEXTURE_H_

#include "egl_canvas.h"

_NAME_BEGIN

class EGL3TextureSample final : public TextureSample
{
public:
	EGL3TextureSample() noexcept;
	~EGL3TextureSample() noexcept;

	bool setup() except;
	void close() noexcept;

	GLuint getInstanceID() noexcept;
private:

	GLuint _sample;
};

class EGL3Texture final : public Texture
{
public:
	EGL3Texture() noexcept;
	~EGL3Texture() noexcept;

	bool setup() except;
	void close() noexcept;

	GLuint getInstanceID() noexcept;

private:

	static void applyTextureWrap(GLenum, TextureWrap wrap) noexcept;
	static void applyTextureFilter(GLenum target, TextureFilter filter) noexcept;
	static void applyTextureAnis(GLenum target, Anisotropy anis) noexcept;

private:

	GLuint _texture;
};

class EGL3RenderTexture final : public RenderTexture
{
public:
	EGL3RenderTexture() noexcept;
	~EGL3RenderTexture() noexcept;

	virtual bool setup(TexturePtr texture) except;
	virtual void setup(std::size_t w, std::size_t h, TextureDim dim, TextureFormat format) except;
	virtual void setup(std::size_t w, std::size_t h, std::size_t d, TextureDim dim, TextureFormat format) except;
	virtual void close() noexcept;

	void setLayer(GLuint layer) noexcept;
	GLuint getLayer() const noexcept;

	GLuint getInstanceID() noexcept;

private:
	void bindRenderTexture(TexturePtr texture, GLenum attachment) noexcept;
	void onSetRenderTextureAfter(RenderTexturePtr target) noexcept;

private:
	GLuint _fbo;
	GLuint _layer;
};

class EGL3MultiRenderTexture final : public MultiRenderTexture
{
public:
	EGL3MultiRenderTexture() noexcept;
	~EGL3MultiRenderTexture() noexcept;

	virtual bool setup() noexcept;
	virtual void close() noexcept;

	GLuint getInstanceID() noexcept;

private:
	void bindRenderTexture(RenderTexturePtr target, GLenum attachment) noexcept;

private:

	GLuint _fbo;
};

_NAME_END

#endif