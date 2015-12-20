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
#ifndef _H_OGL_DEVICE_CONTEXT_H_
#define _H_OGL_DEVICE_CONTEXT_H_

#include "ogl_types.h"

_NAME_BEGIN

class OGLDeviceContext final : public GraphicsContext
{
	__DeclareSubClass(OGLDeviceContext, GraphicsContext)
public:
	OGLDeviceContext() noexcept;
	~OGLDeviceContext() noexcept;

	bool open(WindHandle hwnd) except;
	void close() noexcept;

	void renderBegin() noexcept;
	void renderEnd() noexcept;

	void setWireframeMode(bool enable) noexcept;
	bool getWireframeMode() const noexcept;

	void setViewport(const Viewport& viewport, std::size_t i) noexcept;
	const Viewport& getViewport(std::size_t i) const noexcept;

	void setSwapInterval(SwapInterval interval) noexcept;
	SwapInterval getSwapInterval() const noexcept;

	void setGraphicsLayout(GraphicsLayoutPtr data) noexcept;
	GraphicsLayoutPtr getGraphicsLayout() const noexcept;

	bool updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept;
	void* mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept;
	void unmapBuffer(GraphicsDataPtr& data) noexcept;

	void setIndexBufferData(GraphicsDataPtr data) noexcept;
	GraphicsDataPtr getIndexBufferData() const noexcept;

	void setVertexBufferData(GraphicsDataPtr data) noexcept;
	GraphicsDataPtr getVertexBufferData() const noexcept;

	void setGraphicsTexture(GraphicsTexturePtr texture, std::uint32_t slot) noexcept;
	void setGraphicsTexture(GraphicsTexturePtr texture[], std::uint32_t first, std::uint32_t end) noexcept;

	void setGraphicsSampler(GraphicsSamplerPtr sampler, std::uint32_t slot) noexcept;
	void setGraphicsSampler(GraphicsSamplerPtr sampler[], std::uint32_t first, std::uint32_t count) noexcept;

	void setRenderTexture(GraphicsRenderTexturePtr target) noexcept;
	void setRenderTextureLayer(GraphicsRenderTexturePtr target, std::int32_t layer) noexcept;
	void setMultiRenderTexture(GraphicsMultiRenderTexturePtr target) noexcept;
	void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept;
	void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept;
	void discardRenderTexture() noexcept;
	void blitRenderTexture(GraphicsRenderTexturePtr src, const Viewport& v1, GraphicsRenderTexturePtr dest, const Viewport& v2) noexcept;
	void readRenderTexture(GraphicsRenderTexturePtr source, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
	GraphicsRenderTexturePtr getRenderTexture() const noexcept;
	GraphicsMultiRenderTexturePtr getMultiRenderTexture() const noexcept;

	void setGraphicsState(GraphicsStatePtr state) noexcept;
	GraphicsStatePtr getGraphicsState() const noexcept;

	void setShaderObject(ShaderObjectPtr shader) noexcept;
	ShaderObjectPtr getShaderObject() const noexcept;

	void drawRenderBuffer(const RenderIndirect& renderable) noexcept;
	void drawRenderBuffer(const RenderIndirects& renderable) noexcept;

	void present() noexcept;

private:
	void initDebugControl() noexcept;
	void initStateSystem() noexcept;
	void initCommandList() noexcept;

	static void GLAPIENTRY debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept;

private:
	OGLDeviceContext(const OGLDeviceContext&) noexcept = delete;
	OGLDeviceContext& operator=(const OGLDeviceContext&) noexcept = delete;

private:
	bool _initOpenGL;
	bool _enableWireframe;

	OGLCanvasPtr _glcontext;

	Vector4 _clearColor;
	GLfloat _clearDepth;
	GLint   _clearStencil;

	GLuint _maxViewports;
	GLuint _maxTextureUnits;

	OGLRenderTexturePtr _renderTexture;
	OGLMultiRenderTexturePtr _multiRenderTexture;

	ShaderObjectPtr _shaderObject;

	OGLGraphicsStatePtr _state;
	OGLGraphicsStatePtr _stateCaptured;
	OGLGraphicsStatePtr _stateDefalut;

	GLuint _stateObjDraw;
	GLuint _stateObjDrawGeo;

	GLuint      _tokenBuffer;
	GLuint      _tokenCmdList;
	std::string _tokenData;

	bool _needUpdateLayout;
	bool _needUpdateVbo;
	bool _needUpdateIbo;

	OGLVertexBufferPtr _vbo;
	OGLIndexBufferPtr _ibo;
	OGLGraphicsLayoutPtr _inputLayout;

	OGLDrawIndirectBufferPtr _drawIndirectBuffer;

	std::vector<Viewport> _viewport;
};

_NAME_END

#endif