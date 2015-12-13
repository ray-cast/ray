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
#ifndef _H_EGL2_RENDERER_H_
#define _H_EGL2_RENDERER_H_

#include "egl2_canvas.h"

_NAME_BEGIN

class EGL2Renderer final : public RenderDevice
{
public:
	EGL2Renderer() noexcept;
	~EGL2Renderer() noexcept;

	virtual bool open(WindHandle window) except;
	virtual void close() noexcept;

	virtual void renderBegin() noexcept;
	virtual void renderEnd() noexcept;

	virtual void setViewport(const Viewport& view, std::size_t i = 0) noexcept;
	virtual const Viewport& getViewport(std::size_t i = 0) const noexcept;

	virtual void setWireframeMode(bool enable) noexcept;
	virtual bool getWireframeMode() const noexcept;

	virtual RenderWindowPtr createRenderWindow() const noexcept;
	virtual void setRenderWindow(RenderWindowPtr window) except;
	virtual RenderWindowPtr getRenderWindow() const noexcept;

	virtual void setSwapInterval(SwapInterval interval) except;
	virtual SwapInterval getSwapInterval() const noexcept;

	virtual RenderStatePtr createRenderState() noexcept;
	virtual void setRenderState(RenderStatePtr state) noexcept;
	virtual RenderStatePtr getRenderState() const noexcept;

	virtual RenderBufferPtr createRenderBuffer() noexcept;
	virtual IndexBufferDataPtr createIndexBufferData() noexcept;
	virtual VertexBufferDataPtr createVertexBufferData() noexcept;
	virtual void setRenderBuffer(RenderBufferPtr buffer) noexcept;
	virtual void updateRenderBuffer(RenderBufferPtr buffer) noexcept;
	virtual void drawRenderBuffer(const RenderIndirect& renderable) noexcept;
	virtual void drawRenderBuffer(const RenderIndirects& renderable) noexcept;
	virtual RenderBufferPtr getRenderBuffer() const noexcept;

	virtual TexturePtr createTexture() noexcept;
	virtual RenderTexturePtr createRenderTexture() noexcept;
	virtual MultiRenderTexturePtr createMultiRenderTexture() noexcept;
	virtual void setRenderTexture(RenderTexturePtr target) noexcept;
	virtual void setRenderTextureLayer(RenderTexturePtr texture, std::int32_t layer) noexcept;
	virtual void setMultiRenderTexture(MultiRenderTexturePtr target) noexcept;
	virtual void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept;
	virtual void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept;
	virtual void discardRenderTexture() noexcept;
	virtual void readRenderTexture(RenderTexturePtr src, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
	virtual void blitRenderTexture(RenderTexturePtr src, const Viewport& v1, RenderTexturePtr dest, const Viewport& v2) noexcept;
	virtual RenderTexturePtr getRenderTexture() const noexcept;
	virtual MultiRenderTexturePtr getMultiRenderTexture() const noexcept;

	virtual ShaderPtr createShader() noexcept;
	virtual ShaderObjectPtr createShaderObject() noexcept;
	virtual void setShaderObject(ShaderObjectPtr shader) noexcept;
	virtual ShaderObjectPtr getShaderObject() const noexcept;

	virtual bool createShaderVariant(ShaderVariant& constant) noexcept;
	virtual void destroyShaderVariant(ShaderVariant& constant) noexcept;
	virtual void updateShaderVariant(ShaderVariantPtr constant) noexcept;

private:

	void initDebugControl() noexcept;
	void initStateSystem() noexcept;

	static void GLAPIENTRY debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept;

private:
	EGL2Renderer(const EGL2Renderer&) noexcept = delete;
	EGL2Renderer& operator=(const EGL2Renderer&) noexcept = delete;

private:

	bool _initOpenGL;
	bool _enableWireframe;

	Vector4 _clearColor;
	GLfloat _clearDepth;
	GLint   _clearStencil;

	Viewport _viewport;

	GLuint _maxViewports;
	GLuint _maxTextureUnits;

	RenderTexturePtr _renderTexture;
	MultiRenderTexturePtr _multiRenderTexture;

	ShaderObjectPtr _shaderObject;

	RenderWindowPtr _glcontext;
	RenderBufferPtr _renderBuffer;
	RenderStatePtr _state;
	RenderStatePtr _stateCaptured;

	std::vector<GLint> _textureUnits;
};

_NAME_END

#endif