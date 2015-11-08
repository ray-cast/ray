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
#ifndef _H_OGL_RENDERER_H_
#define _H_OGL_RENDERER_H_

#include <ray/ogl_types.h>
#include <ray/render_command.h>

_NAME_BEGIN

class OGLRenderer final : public RenderDevice
{
public:
	OGLRenderer() noexcept;
	~OGLRenderer() noexcept;

	virtual bool open(RenderWindowPtr window) except;
	virtual void close() noexcept;

	virtual void renderBegin() noexcept;
	virtual void renderEnd() noexcept;

	virtual void setViewport(const Viewport& view, std::size_t i = 0) noexcept;
	virtual const Viewport& getViewport(std::size_t i = 0) const noexcept;

	virtual void setWireframeMode(bool enable) noexcept;
	virtual bool getWireframeMode() const noexcept;

	virtual void setRenderWindow(RenderWindowPtr window) except;
	virtual RenderWindowPtr getRenderWindow() const noexcept;

	virtual void setSwapInterval(SwapInterval interval) except;
	virtual SwapInterval getSwapInterval() const noexcept;

	virtual void setRenderState(RenderStatePtr state) noexcept;
	virtual RenderStatePtr getRenderState() const noexcept;

	virtual void setRenderBuffer(RenderBufferPtr buffer) noexcept;
	virtual void updateRenderBuffer(RenderBufferPtr buffer) noexcept;
	virtual void drawRenderBuffer(const RenderIndirect& renderable) noexcept;
	virtual void drawRenderBuffer(const RenderIndirects& renderable) noexcept;
	virtual RenderBufferPtr getRenderBuffer() const noexcept;

	virtual void setRenderTexture(RenderTexturePtr target) noexcept;
	virtual void setRenderTextureLayer(RenderTexturePtr texture, std::int32_t layer) noexcept;
	virtual void setMultiRenderTexture(MultiRenderTexturePtr target) noexcept;
	virtual void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept;
	virtual void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept;
	virtual void discardRenderTexture() noexcept;
	virtual void readRenderTexture(RenderTexturePtr src, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
	virtual void copyRenderTexture(RenderTexturePtr src, const Viewport& v1, RenderTexturePtr dest, const Viewport& v2) noexcept;
	virtual RenderTexturePtr getRenderTexture() const noexcept;
	virtual MultiRenderTexturePtr getMultiRenderTexture() const noexcept;

	virtual void setShaderObject(ShaderObjectPtr shader) noexcept;
	virtual ShaderObjectPtr getShaderObject() const noexcept;

	virtual bool createShaderVariant(ShaderVariant& constant) noexcept;
	virtual void destroyShaderVariant(ShaderVariant& constant) noexcept;
	virtual void updateShaderVariant(ShaderVariantPtr constant) noexcept;

private:

	void initDebugControl() noexcept;
	void initStateSystem() noexcept;
	void initCommandList() noexcept;

	void setShaderUniform(ShaderUniformPtr uniform, TexturePtr texture, TextureSamplePtr sample = nullptr) noexcept;
	void setShaderUniform(ShaderUniformPtr uniform, ShaderVariantPtr constant) noexcept;

	static void checkError() noexcept;
	static void checkFramebuffer() noexcept;
	static void GLAPIENTRY debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept;

private:
	OGLRenderer(const OGLRenderer&) noexcept = delete;
	OGLRenderer& operator=(const OGLRenderer&) noexcept = delete;

private:

	bool _initOpenGL;
	bool _enableWireframe;

	Vector4 _clearColor;
	GLfloat _clearDepth;
	GLint   _clearStencil;

	GLuint _defaultVAO;
	GLuint _maxViewports;
	GLuint _maxTextureUnits;

	RenderTexturePtr _renderTexture;
	MultiRenderTexturePtr _multiRenderTexture;

	ShaderObjectPtr _shaderObject;

	RenderWindowPtr _glcontext;
	RenderBufferPtr _renderBuffer;
	RenderStatePtr _state;
	RenderStatePtr _stateCaptured;

	GLuint _stateObjDraw;
	GLuint _stateObjDrawGeo;

	GLuint      _tokenBuffer;
	GLuint      _tokenCmdList;
	std::string _tokenData;

	RenderCommand _renderCommands;

	std::vector<GLint> _textureUnits;
	std::vector<Viewport> _viewport;
	std::vector<OGLShaderVariant> _constantBuffers;
};

_NAME_END

#endif