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
	, _maxTextureUnits(32)
	, _maxViewports(4)
	, _clearColor(0.0, 0.0, 0.0)
	, _clearDepth(0.0)
	, _clearStencil(0xFFFFFFFF)
	, _state(GL_NONE)
	, _renderTexture(GL_NONE)
	, _enableWireframe(false)
	, _needUpdateLayout(false)
	, _needUpdateVbo(false)
	, _needUpdateIbo(false)
{
	_viewport.resize(_maxViewports);
	_stateCaptured = std::make_shared<OGLGraphicsState>();
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
			oglState->apply(*_stateCaptured);

			_stateCaptured->setBlendState(oglState->getBlendState());
			_stateCaptured->setDepthState(oglState->getDepthState());
			_stateCaptured->setRasterState(oglState->getRasterState());
			_stateCaptured->setStencilState(oglState->getStencilState());

			_state = oglState;
		}
		else
		{
			assert(false);
		}
	}
	else
	{
		assert(false);
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
	if (!_stateCaptured)
		return;

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

	auto primitiveType = _stateCaptured->getRasterState().primitiveType;
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
			_vbo->bind(_inputLayout);
		_needUpdateVbo = false;
	}

	if (_needUpdateIbo)
	{
		if (_ibo)
			_ibo->bind();
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
OGLDeviceContext::drawRenderBuffer(const RenderIndirects& renderable) noexcept
{
	assert(false);
}

void
OGLDeviceContext::setTexture(TexturePtr texture, std::uint32_t slot) noexcept
{
	auto gltexture = std::dynamic_pointer_cast<OGLTexture>(texture);
	if (gltexture)
	{
		GLuint textureID = gltexture->getInstanceID();
		glBindTextureUnit(slot, textureID);
	}
	else
	{
		glBindTextureUnit(slot, 0);
	}
}

void
OGLDeviceContext::setGraphicsSampler(GraphicsSamplerPtr sampler, std::uint32_t slot) noexcept
{
	auto glsampler = sampler->downcast<OGLSampler>();
	if (glsampler)
	{
		GLuint samplerID = glsampler->getInstanceID();
		glBindSampler(slot, samplerID);
	}
	else
	{
		glBindSampler(slot, 0);
	}
}

void
OGLDeviceContext::setRenderTexture(RenderTexturePtr target) noexcept
{
	if (_renderTexture != target)
	{
		if (target)
		{
			auto framebuffer = std::dynamic_pointer_cast<OGLRenderTexture>(target)->getInstanceID();

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

			this->setViewport(Viewport(0, 0, target->getWidth(), target->getHeight()), 0);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		}

		_renderTexture = target;
		_multiRenderTexture = nullptr;
	}
}

void
OGLDeviceContext::setMultiRenderTexture(MultiRenderTexturePtr target) noexcept
{
	assert(target);

	auto framebuffer = std::dynamic_pointer_cast<OGLMultiRenderTexture>(target)->getInstanceID();
	if (_multiRenderTexture != target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		auto& renderTextures = target->getRenderTextures();
		std::size_t size = renderTextures.size();
		for (std::size_t index = 0; index < size; index++)
		{
			auto renderTexture = renderTextures[index];
			this->setViewport(Viewport(0, 0, renderTexture->getWidth(), renderTexture->getHeight()), index);
		}

		_renderTexture = nullptr;
		_multiRenderTexture = target;
	}
}

void
OGLDeviceContext::setRenderTextureLayer(RenderTexturePtr renderTexture, std::int32_t layer) noexcept
{
	assert(renderTexture);

	if (renderTexture->getTexDim() == TextureDim::DIM_2D_ARRAY ||
		renderTexture->getTexDim() == TextureDim::DIM_CUBE)
	{
		auto texture = std::dynamic_pointer_cast<OGLTexture>(renderTexture->getResolveTexture());
		auto textureID = texture->getInstanceID();

		GLenum attachment = GL_COLOR_ATTACHMENT0;

		if (_multiRenderTexture)
		{
			for (auto& it : _multiRenderTexture->getRenderTextures())
			{
				if (it == renderTexture)
					break;
				attachment++;
			}
		}
		else if (_renderTexture != renderTexture)
		{
			this->setRenderTexture(renderTexture);
		}

		if (renderTexture->getTexDim() == TextureDim::DIM_2D_ARRAY)
			glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, textureID, 0, layer);
		else if (renderTexture->getTexDim() == TextureDim::DIM_CUBE)
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, textureID, 0);
	}
}

void
OGLDeviceContext::blitRenderTexture(RenderTexturePtr src, const Viewport& v1, RenderTexturePtr dest, const Viewport& v2) noexcept
{
	assert(src);

	auto srcTarget = std::dynamic_pointer_cast<OGLRenderTexture>(src)->getInstanceID();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, srcTarget);

	if (dest)
	{
		auto destTarget = std::dynamic_pointer_cast<OGLRenderTexture>(dest)->getInstanceID();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destTarget);
	}
	else
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	_renderTexture = GL_NONE;
	_multiRenderTexture = GL_NONE;
}

RenderTexturePtr
OGLDeviceContext::getRenderTexture() const noexcept
{
	return _renderTexture;
}

MultiRenderTexturePtr
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
		auto depthWriteMask = _stateCaptured->getDepthState().depthWriteMask;
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
		auto depthWriteMask = _stateCaptured->getDepthState().depthWriteMask;
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
	{
		GLenum attachment = GL_COLOR_ATTACHMENT0;
		glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachment);
	}
	else if (_multiRenderTexture)
	{
		GLenum attachments[24];
		GLenum attachment = GL_COLOR_ATTACHMENT0;

		auto& targets = _multiRenderTexture->getRenderTextures();
		auto size = targets.size();

		for (std::size_t i = 0; i < size; i++)
		{
			attachments[i] = attachment;
			attachment++;
		}

		glInvalidateFramebuffer(GL_FRAMEBUFFER, size, attachments);
	}
}

void
OGLDeviceContext::readRenderTexture(RenderTexturePtr target, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(target && w && h && data);

	auto framebuffer = std::dynamic_pointer_cast<OGLRenderTexture>(target)->getInstanceID();
	if (_renderTexture != target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		_renderTexture = target;
		_multiRenderTexture = nullptr;
	}

	GLenum format = OGLTypes::asOGLFormat(pfd);
	GLenum type = OGLTypes::asOGLType(pfd);

	glReadPixels(0, 0, w, h, format, type, data);
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

	/*if (_shaderObject)
	{
	auto& uniforms = _shaderObject->getActiveUniforms();

	for (auto& it : uniforms)
	{
	if (it->getType() == Texture)
	{
	auto uniform = std::dynamic_pointer_cast<OGLShaderUniform>(it);
	auto bindingPoint = uniform->getBindingPoint();
	auto texture = std::dynamic_pointer_cast<OGLTexture>(uniform->getTexture());
	auto sampler = std::dynamic_pointer_cast<OGLSampler>(uniform->getSampler());

	this->setTexture(texture, bindingPoint);

	if (sampler)
	{
	GLuint samplerID = sampler->getInstanceID();
	glBindSampler(GL_TEXTURE0 + bindingPoint, samplerID);
	}
	}
	}
	}*/
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
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

_NAME_END