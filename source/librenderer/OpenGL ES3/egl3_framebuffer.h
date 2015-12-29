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
#ifndef _H_EGL3_FRAMEBUFFER_H_
#define _H_EGL3_FRAMEBUFFER_H_

#include "egl3_canvas.h"

_NAME_BEGIN

class EGL3RenderTexture final : public GraphicsRenderTexture
{
	__DeclareSubClass(EGL3RenderTexture, GraphicsRenderTexture)
public:
	EGL3RenderTexture() noexcept;
	~EGL3RenderTexture() noexcept;

	bool setup(const GraphicsRenderTextureDesc& framebufferDesc) except;
	void close() noexcept;

	void setLayer(GLuint layer) noexcept;
	GLuint getLayer() const noexcept;

	void setActive(bool active) noexcept;
	bool getActive() noexcept;

	void discard() noexcept;

	GLuint getInstanceID() noexcept;

	GraphicsTexturePtr getResolveTexture() const noexcept;
	const GraphicsRenderTextureDesc& getGraphicsRenderTextureDesc() const noexcept;

private:
	void bindRenderTexture(GraphicsTexturePtr texture, GLenum attachment) noexcept;

private:
	friend class EGL3Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL3RenderTexture(const EGL3RenderTexture&) noexcept = delete;
	EGL3RenderTexture& operator=(const EGL3RenderTexture&) noexcept = delete;

private:
	bool _isActive;

	GLuint _fbo;
	GLuint _layer;

	GraphicsDeviceWeakPtr _device;
	GraphicsRenderTextureDesc _framebufferDesc;
};

class EGL3MultiRenderTexture final : public GraphicsMultiRenderTexture
{
	__DeclareSubClass(EGL3MultiRenderTexture, GraphicsMultiRenderTexture)
public:
	EGL3MultiRenderTexture() noexcept;
	~EGL3MultiRenderTexture() noexcept;

	bool setup(const GraphicsMultiRenderTextureDesc& multiFramebufferDesc) noexcept;
	void close() noexcept;

	void setActive(bool active) noexcept;
	bool getActive() noexcept;

	void setLayer(GraphicsRenderTexturePtr texture, GLuint layer) noexcept;
	GLuint getLayer() const noexcept;

	void discard() noexcept;

	GLuint getInstanceID() noexcept;

	const GraphicsMultiRenderTextureDesc& getGraphicsMultiRenderTextureDesc() const noexcept;

private:
	void bindRenderTexture(GraphicsTexturePtr target, GLenum attachment) noexcept;

private:
	friend class EGL3Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL3MultiRenderTexture(const EGL3MultiRenderTexture&) noexcept = delete;
	EGL3MultiRenderTexture& operator=(const EGL3MultiRenderTexture&) noexcept = delete;

private:

	bool _isActive;
	GLuint _fbo;
	GraphicsDeviceWeakPtr _device;
	GraphicsMultiRenderTextureDesc _multiFramebufferDesc;
};

_NAME_END

#endif