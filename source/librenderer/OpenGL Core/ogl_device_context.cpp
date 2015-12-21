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
#include "ogl_device_context.h"
#include "ogl_canvas.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_texture.h"
#include "ogl_framebuffer.h"
#include "ogl_layout.h"
#include "ogl_vbo.h"
#include "ogl_ibo.h"
#include "ogl_dibo.h"
#include "ogl_commandlist.h"
#include "ogl_sampler.h"

_NAME_BEGIN

__ImplementSubClass(OGLDeviceContext, GraphicsContext, "OGLDeviceContext")

OGLDeviceContext::OGLDeviceContext() noexcept
	: _initOpenGL(false)
{
}

OGLDeviceContext::~OGLDeviceContext() noexcept
{
	this->close();
}

bool
OGLDeviceContext::open(WindHandle window) except
{
	if (!_initOpenGL)
	{
		_glcontext = std::make_shared<OGLCanvas>();
		_glcontext->open(window);

		this->initDebugControl();
		this->initStateSystem();
		this->initCommandList();

		_initOpenGL = true;
	}

	return _initOpenGL;
}

void
OGLDeviceContext::close() noexcept
{
	if (_shaderObject)
	{
		_shaderObject.reset();
		_shaderObject = nullptr;
	}

	if (_renderTexture)
	{
		_renderTexture.reset();
		_renderTexture = nullptr;
	}

	if (_multiRenderTexture)
	{
		_multiRenderTexture.reset();
		_multiRenderTexture = nullptr;
	}

	if (_state)
	{
		_state.reset();
		_state = nullptr;
	}

	if (_glcontext)
	{
		_glcontext.reset();
		_glcontext = nullptr;
	}

	_initOpenGL = false;
}

void
OGLDeviceContext::renderBegin() noexcept
{
	assert(_glcontext);
	_glcontext->setActive(true);
}

void
OGLDeviceContext::renderEnd() noexcept
{
	assert(_glcontext);
	_glcontext->setActive(false);
}

void
OGLDeviceContext::setViewport(const Viewport& view, std::size_t i) noexcept
{
	if (_viewport[i] != view)
	{
		glViewportIndexedf(i, view.left, view.top, view.width, view.height);
		_viewport[i] = view;
	}
}

const Viewport&
OGLDeviceContext::getViewport(std::size_t i) const noexcept
{
	return _viewport[i];
}

void
OGLDeviceContext::setWireframeMode(bool enable) noexcept
{
	_enableWireframe = enable;
}

bool
OGLDeviceContext::getWireframeMode() const noexcept
{
	return _enableWireframe;
}

void
OGLDeviceContext::setSwapInterval(SwapInterval interval) noexcept
{
	assert(_glcontext);
	_glcontext->setSwapInterval(interval);
}

SwapInterval
OGLDeviceContext::getSwapInterval() const noexcept
{
	assert(_glcontext);
	return _glcontext->getSwapInterval();
}

void
OGLDeviceContext::setGraphicsState(GraphicsStatePtr state) noexcept
{
	if (state)
	{
		auto oglState = state->downcast<OGLGraphicsState>();
		if (oglState)
		{
			oglState->apply(_stateCaptured);

			_state = oglState;
			_stateCaptured = oglState->getGraphicsStateDesc();
		}
		else
		{
			assert(false);
		}
	}
	else
	{
		_stateDefalut->apply(_stateCaptured);
	}
}

GraphicsStatePtr
OGLDeviceContext::getGraphicsState() const noexcept
{
	return _state;
}

void
OGLDeviceContext::setGraphicsLayout(GraphicsLayoutPtr layout) noexcept
{
	if (_inputLayout != layout)
	{
		if (layout)
		{
			if (layout->isInstanceOf<OGLGraphicsLayout>())
				_inputLayout = layout->downcast<OGLGraphicsLayout>();
			else
				_inputLayout = nullptr;
		}
		else
		{
			_inputLayout = nullptr;
		}

		_needUpdateLayout = true;
	}
}

GraphicsLayoutPtr
OGLDeviceContext::getGraphicsLayout() const noexcept
{
	return _inputLayout;
}

bool
OGLDeviceContext::updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept
{
	if (data)
	{
		auto _data = data->cast<OGLGraphicsData>();
		_data->resize((const char*)str, cnt);
		return true;
	}

	return false;
}

void*
OGLDeviceContext::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
{
	if (data)
	{
		auto _data = data->cast<OGLGraphicsData>();
		return _data->map(access);
	}

	return nullptr;
}

void
OGLDeviceContext::unmapBuffer(GraphicsDataPtr& data) noexcept
{
	if (data)
	{
		auto _data = data->cast<OGLGraphicsData>();
		_data->unmap();
	}
}

void
OGLDeviceContext::setIndexBufferData(GraphicsDataPtr data) noexcept
{
	if (_ibo != data)
	{
		if (data)
		{
			if (data->isInstanceOf<OGLIndexBuffer>())
				_ibo = data->downcast<OGLIndexBuffer>();
			else
				_ibo = nullptr;
		}
		else
		{
			_ibo = nullptr;
		}

		_needUpdateIbo = true;
	}
}

GraphicsDataPtr
OGLDeviceContext::getIndexBufferData() const noexcept
{
	return _ibo;
}

void
OGLDeviceContext::setVertexBufferData(GraphicsDataPtr data) noexcept
{
	if (_vbo != data)
	{
		if (data)
		{
			if (data->isInstanceOf<OGLVertexBuffer>())
				_vbo = data->downcast<OGLVertexBuffer>();
			else
				_vbo = nullptr;
		}
		else
		{
			_vbo = nullptr;
		}

		_needUpdateVbo = true;
	}
}

GraphicsDataPtr
OGLDeviceContext::getVertexBufferData() const noexcept
{
	return _vbo;
}

void
OGLDeviceContext::drawRenderBuffer(const RenderIndirect& renderable) noexcept
{
	if (!_inputLayout || !_vbo)
		return;

	if (_vbo->size() < _inputLayout->getVertexSize() * (renderable.startVertice + renderable.numVertices))
		return;

	if (_inputLayout->getIndexType() != GL_NONE)
	{
		if (!_ibo)
			return;

		if (_ibo->size() < _inputLayout->getIndexSize() * (renderable.startIndice + renderable.numIndices))
			return;
	}

	auto primitiveType = _stateCaptured.getRasterState().primitiveType;
	if (_enableWireframe)
	{
		if (primitiveType == VertexType::PointOrLine ||
			primitiveType == VertexType::TriangleOrLine ||
			primitiveType == VertexType::FanOrLine)
		{
			primitiveType = VertexType::Line;
		}
	}

	if (_needUpdateLayout)
	{
		if (_inputLayout)
			_inputLayout->bindLayout();
		_needUpdateLayout = false;
	}

	if (_needUpdateVbo)
	{
		if (_vbo)
			_inputLayout->bindVbo(_vbo);
		_needUpdateVbo = false;
	}

	if (_needUpdateIbo)
	{
		if (_ibo)
			_inputLayout->bindIbo(_ibo);
		_needUpdateIbo = false;
	}

	if (_ibo)
	{
		GLenum drawType = OGLTypes::asOGLVertexType(primitiveType);
		GLenum indexType = _inputLayout->getIndexType();
		GLsizei numInstance = std::max(1, renderable.numInstances);
		GLvoid* offsetIndices = (GLchar*)(nullptr) + (_inputLayout->getIndexSize() * renderable.startIndice);
		glDrawElementsInstancedBaseVertexBaseInstance(drawType, renderable.numIndices, indexType, offsetIndices, numInstance, renderable.startVertice, renderable.startInstances);
	}
	else
	{
		GLsizei numInstance = std::max(1, renderable.numInstances);
		GLenum drawType = OGLTypes::asOGLVertexType(primitiveType);
		glDrawArraysInstancedBaseInstance(drawType, renderable.startVertice, renderable.numVertices, numInstance, renderable.startInstances);
	}
}

void
OGLDeviceContext::drawRenderBuffer(const RenderIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	for (std::size_t i = first; i < first + count; i++)
		this->drawRenderBuffer(renderable[i]);
}

void
OGLDeviceContext::setGraphicsTexture(GraphicsTexturePtr texture, std::uint32_t slot) noexcept
{
	if (texture)
	{
		auto gltexture = texture->downcast<OGLTexture>();
		GLuint textureID = gltexture->getInstanceID();
		glBindTextureUnit(slot, textureID);
	}
	else
	{
		glBindTextureUnit(slot, 0);
	}
}

void
OGLDeviceContext::setGraphicsTexture(GraphicsTexturePtr texture[], std::uint32_t first, std::uint32_t count) noexcept
{
	if (count < MAX_TEXTURE_UNIT)
	{
		GLuint textures[MAX_TEXTURE_UNIT];
		for (std::uint32_t i = 0; i < count; i++)
		{
			if (texture[i])
				textures[i] = std::dynamic_pointer_cast<OGLTexture>(texture[i])->getInstanceID();
			else
				textures[i] = GL_NONE;
		}

		glBindTextures(first, count, textures);
	}
}

void
OGLDeviceContext::setGraphicsSampler(GraphicsSamplerPtr sampler, std::uint32_t slot) noexcept
{
	if (sampler)
	{
		GLuint samplerID = sampler->downcast<OGLSampler>()->getInstanceID();
		glBindSampler(slot, samplerID);
	}
	else
	{
		glBindSampler(slot, 0);
	}
}

void
OGLDeviceContext::setGraphicsSampler(GraphicsSamplerPtr sampler[], std::uint32_t first, std::uint32_t count) noexcept
{
	if (count < MAX_SAMPLER_UNIT)
	{
		GLuint samplers[MAX_SAMPLER_UNIT];
		for (std::uint32_t i = 0; i < count; i++)
		{
			if (sampler[i])
				samplers[i] = sampler[i]->downcast<OGLSampler>()->getInstanceID();
			else
				samplers[i] = GL_NONE;
		}

		glBindSamplers(first, count, samplers);
	}
}

void
OGLDeviceContext::setRenderTexture(GraphicsRenderTexturePtr target) noexcept
{
	if (_renderTexture != target)
	{
		if (_renderTexture)
			_renderTexture->setActive(false);

		if (_multiRenderTexture)
			_multiRenderTexture->setActive(false);

		if (target)
		{
			_renderTexture = target->downcast<OGLRenderTexture>();
			_renderTexture->setActive(true);

			auto textureDesc = _renderTexture->getResolveTexture()->getGraphicsTextureDesc();
			this->setViewport(Viewport(0, 0, textureDesc.getWidth(), textureDesc.getHeight()), 0);
		}
		else
		{
			_renderTexture = nullptr;
			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		}

		_multiRenderTexture = nullptr;
	}
}

void
OGLDeviceContext::setMultiRenderTexture(GraphicsMultiRenderTexturePtr target) noexcept
{
	if (_multiRenderTexture != target)
	{
		if (_renderTexture)
			_renderTexture->setActive(false);

		if (_multiRenderTexture)
			_multiRenderTexture->setActive(false);

		if (target)
		{
			_multiRenderTexture = target->downcast<OGLMultiRenderTexture>();
			_multiRenderTexture->setActive(true);

			auto& framebuffers = _multiRenderTexture->getGraphicsMultiRenderTextureDesc().getRenderTextures();
			std::size_t size = framebuffers.size();
			for (std::size_t index = 0; index < size; index++)
			{
				auto framebuffer = framebuffers[index];
				auto textureDesc = framebuffer->getResolveTexture()->getGraphicsTextureDesc();
				this->setViewport(Viewport(0, 0, textureDesc.getWidth(), textureDesc.getHeight()), index);
			}
		}
		else
		{
			_multiRenderTexture = nullptr;
			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		}

		_renderTexture = nullptr;
	}
}

void
OGLDeviceContext::setRenderTextureLayer(GraphicsRenderTexturePtr renderTexture, std::int32_t layer) noexcept
{
	assert(renderTexture);

	if (_multiRenderTexture)
	{
		_multiRenderTexture->setLayer(renderTexture, layer);
	}
	else if (_renderTexture == renderTexture)
	{
		_renderTexture->setLayer(layer);
	}
	else
	{
		this->setRenderTexture(renderTexture);
		_renderTexture->setLayer(layer);
	}
}

void
OGLDeviceContext::blitRenderTexture(GraphicsRenderTexturePtr src, const Viewport& v1, GraphicsRenderTexturePtr dest, const Viewport& v2) noexcept
{
	assert(src);

	auto readFramebuffer = src->downcast<OGLRenderTexture>()->getInstanceID();
	auto drawFramebuffer = dest ? dest->downcast<OGLRenderTexture>()->getInstanceID() : 0;

	glBlitNamedFramebuffer(readFramebuffer, drawFramebuffer, v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

GraphicsRenderTexturePtr
OGLDeviceContext::getRenderTexture() const noexcept
{
	return _renderTexture;
}

GraphicsMultiRenderTexturePtr
OGLDeviceContext::getMultiRenderTexture() const noexcept
{
	return _multiRenderTexture;
}

void
OGLDeviceContext::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
{
	GLbitfield mode = 0;

	if (flags & ClearFlags::CLEAR_COLOR)
	{
		mode |= GL_COLOR_BUFFER_BIT;

		if (_clearColor != color)
		{
			glClearColor(color.x, color.y, color.z, color.w);
			_clearColor = color;
		}
	}

	if (flags & ClearFlags::CLEAR_DEPTH)
	{
		mode |= GL_DEPTH_BUFFER_BIT;

		if (_clearDepth != depth)
		{
			glClearDepthf(depth);
			_clearDepth = depth;
		}
	}

	if (flags & ClearFlags::CLEAR_STENCIL)
	{
		mode |= GL_STENCIL_BUFFER_BIT;

		if (_clearStencil != stencil)
		{
			glClearStencil(stencil);
			_clearStencil = stencil;
		}
	}

	if (mode != 0)
	{
		auto depthWriteMask = _stateCaptured.getDepthState().depthWriteMask;
		if (!depthWriteMask && flags & ClearFlags::CLEAR_DEPTH)
		{
			glDepthMask(GL_TRUE);
		}

		glClear(mode);

		if (!depthWriteMask && flags & ClearFlags::CLEAR_DEPTH)
		{
			glDepthMask(GL_FALSE);
		}
	}
}

void
OGLDeviceContext::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	if (flags & ClearFlags::CLEAR_DEPTH)
	{
		auto depthWriteMask = _stateCaptured.getDepthState().depthWriteMask;
		if (!depthWriteMask && flags & ClearFlags::CLEAR_DEPTH)
		{
			glDepthMask(GL_TRUE);
		}

		GLfloat f = depth;
		glClearBufferfv(GL_DEPTH, 0, &f);

		if (!depthWriteMask && flags & ClearFlags::CLEAR_DEPTH)
		{
			glDepthMask(GL_FALSE);
		}
	}

	if (flags & ClearFlags::CLEAR_STENCIL)
	{
		GLint s = stencil;
		glClearBufferiv(GL_STENCIL, 0, &s);
	}

	if (flags & ClearFlags::CLEAR_COLOR)
	{
		glClearBufferfv(GL_COLOR, i, color.ptr());
	}
}

void
OGLDeviceContext::discardRenderTexture() noexcept
{
	assert(_renderTexture || _multiRenderTexture);

	if (_renderTexture)
		_renderTexture->downcast<OGLRenderTexture>()->discard();
	else if (_multiRenderTexture)
		_multiRenderTexture->downcast<OGLMultiRenderTexture>()->discard();
}

void
OGLDeviceContext::readRenderTexture(GraphicsRenderTexturePtr target, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(w && h && data);

	if (target)
	{
		GLenum format = OGLTypes::asOGLFormat(pfd);
		GLenum type = OGLTypes::asOGLType(pfd);

		if (format != GL_INVALID_ENUM && type != GL_INVALID_ENUM)
		{
			this->setRenderTexture(target);
			glReadPixels(0, 0, w, h, format, type, data);
		}
	}
}

void
OGLDeviceContext::setShaderObject(ShaderObjectPtr shader) noexcept
{
	if (_shaderObject != shader)
	{
		if (_shaderObject)
			_shaderObject->setActive(false);

		_shaderObject = shader;

		if (_shaderObject)
			_shaderObject->setActive(true);
	}
}

ShaderObjectPtr
OGLDeviceContext::getShaderObject() const noexcept
{
	return _shaderObject;
}

void
OGLDeviceContext::present() noexcept
{
	assert(_glcontext);
	_glcontext->present();
}

void
OGLDeviceContext::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
{
	std::cerr << "source : ";
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		std::cerr << "GL_DEBUG_SOURCE_API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		std::cerr << "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		std::cerr << "GL_DEBUG_SOURCE_SHADER_COMPILER";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		std::cerr << "GL_DEBUG_SOURCE_THIRD_PARTY";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		std::cerr << "GL_DEBUG_SOURCE_APPLICATION";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		std::cerr << "GL_DEBUG_SOURCE_OTHER";
		break;
	}

	std::cerr << std::endl;

	std::cerr << "type : ";
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		std::cerr << "GL_DEBUG_TYPE_ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cerr << "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cerr << "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cerr << "GL_DEBUG_TYPE_PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cerr << "GL_DEBUG_TYPE_PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cerr << "GL_DEBUG_TYPE_OTHER";
		break;
	case GL_DEBUG_TYPE_MARKER:
		std::cerr << "GL_DEBUG_TYPE_MARKER";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		std::cerr << "GL_DEBUG_TYPE_PUSH_GROUP";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		std::cerr << "GL_DEBUG_TYPE_POP_GROUP";
		break;
	}

	std::cerr << std::endl;

	std::cerr << "id : " << id << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cerr << "notice";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cerr << "low";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cerr << "medium";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cerr << "high";
		break;
	}

	std::cerr << std::endl;

	std::cerr << "message : " << message << std::endl;
}

void
OGLDeviceContext::initDebugControl() noexcept
{
#if defined(_DEBUG) && !defined(__ANDROID__)
	// 131184 memory info
	// 131185 memory allocation info
	GLuint ids[] =
	{
		131076,
		131169,
		131184,
		131185,
		131218,
		131204
	};

	glEnable(GL_DEBUG_OUTPUT);

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(debugCallBack, this);
	// enable all
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
	// disable ids
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 6, ids, GL_FALSE);
#endif
}

void
OGLDeviceContext::initCommandList() noexcept
{
	initCommandListNV();

	GraphicsDataDesc dibo;
	dibo.setType(GraphicsStream::DIBO);
	dibo.setStreamSize(sizeof(DrawElementsIndirectCommand));
	dibo.setUsage(UsageFlags::MAP_WRITE_BIT | UsageFlags::IMMUTABLE_STORAGE);

	_drawIndirectBuffer = std::make_shared<OGLDrawIndirectBuffer>();
	_drawIndirectBuffer->open(dibo);
	_drawIndirectBuffer->bind();
}

void
OGLDeviceContext::initStateSystem() noexcept
{
	glClearDepth(1.0);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearStencil(0);

	glViewport(0, 0, 0, 0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glDisable(GL_STENCIL_TEST);
	glStencilMask(0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);

	glDisable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_maxTextureUnits = MAX_TEXTURE_UNIT;
	_maxViewports = 4;

	_clearColor.set(0.0, 0.0, 0.0, 0.0);
	_clearDepth = 1.0;
	_clearStencil = 0;

	_enableWireframe = false;

	_needUpdateLayout = false;
	_needUpdateVbo = false;
	_needUpdateIbo = false;

	_viewport.resize(_maxViewports);

	_stateDefalut = std::make_shared<OGLGraphicsState>();
	_stateDefalut->setup(GraphicsStateDesc());

	_stateCaptured = GraphicsStateDesc();
}

_NAME_END