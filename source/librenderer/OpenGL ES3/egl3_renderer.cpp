// +---------------------------------------------------------------------
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
#include "egl3_renderer.h"
#include "egl3_state.h"
#include "egl3_shader.h"
#include "egl3_texture.h"
#include "egl3_layout.h"
#include "egl3_vbo.h"
#include "egl3_ibo.h"
#include "egl3_dibo.h"
#include "egl3_sampler.h"

_NAME_BEGIN

EGL3Renderer::EGL3Renderer() noexcept
	: _initOpenGL(false)
	, _maxTextureUnits(32)
	, _maxViewports(4)
	, _clearColor(0.0, 0.0, 0.0)
	, _clearDepth(0.0)
	, _clearStencil(0xFFFFFFFF)
	, _viewport(0, 0, 0, 0)
	, _state(GL_NONE)
	, _renderTexture(GL_NONE)
	, _enableWireframe(false)
{
}

EGL3Renderer::~EGL3Renderer() noexcept
{
	this->close();
}

bool
EGL3Renderer::open(WindHandle window) except
{
	if (!_initOpenGL)
	{
		_glcontext = this->createRenderWindow();
		_glcontext->open(window);

		_textureUnits.resize(_maxTextureUnits);
		_stateCaptured = std::make_shared<EGL3RenderState>();

		this->initDebugControl();
		this->initStateSystem();

		_initOpenGL = true;
	}

	return _initOpenGL;
}

void
EGL3Renderer::close() noexcept
{
	if (!_initOpenGL)
		return;

	if (_renderBuffer)
	{
		_renderBuffer.reset();
		_renderBuffer = nullptr;
	}

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
EGL3Renderer::renderBegin() noexcept
{
	this->setShaderObject(nullptr);
	this->setRenderTexture(nullptr);
}

void
EGL3Renderer::renderEnd() noexcept
{
	assert(_glcontext);
	_glcontext->present();
}

void
EGL3Renderer::setViewport(const Viewport& view, std::size_t i) noexcept
{
	if (_viewport.left != view.left ||
		_viewport.top != view.top ||
		_viewport.width != view.width ||
		_viewport.height != view.height)
	{
		GL_CHECK(glViewport(view.left, view.top, view.width, view.height));
		_viewport = view;
	}
}

const Viewport&
EGL3Renderer::getViewport(std::size_t i) const noexcept
{
	return _viewport;
}

void
EGL3Renderer::setWireframeMode(bool enable) noexcept
{
	_enableWireframe = enable;
}

bool
EGL3Renderer::getWireframeMode() const noexcept
{
	return _enableWireframe;
}

RenderWindowPtr
EGL3Renderer::createRenderWindow() const noexcept
{
	return std::make_shared<EGL3Canvas>();
}

void
EGL3Renderer::setRenderWindow(RenderWindowPtr glcontext) except
{
	assert(glcontext);

	if (_glcontext != glcontext)
	{
		if (_glcontext)
			_glcontext->setActive(false);

		_glcontext = glcontext;

		if (_glcontext)
			_glcontext->setActive(true);
	}
}

RenderWindowPtr
EGL3Renderer::getRenderWindow() const noexcept
{
	return _glcontext;
}

void
EGL3Renderer::setSwapInterval(SwapInterval interval) except
{
	assert(_glcontext);
	_glcontext->setSwapInterval(interval);
}

SwapInterval
EGL3Renderer::getSwapInterval() const noexcept
{
	assert(_glcontext);
	return _glcontext->getSwapInterval();
}

RenderStatePtr
EGL3Renderer::createRenderState() noexcept
{
	return std::make_shared<EGL3RenderState>();
}

void
EGL3Renderer::setRenderState(RenderStatePtr state) noexcept
{
	assert(state);

	state->apply(*_stateCaptured);

	_stateCaptured->setBlendState(state->getBlendState());
	_stateCaptured->setDepthState(state->getDepthState());
	_stateCaptured->setRasterState(state->getRasterState());
	_stateCaptured->setStencilState(state->getStencilState());

	_state = state;
}

RenderStatePtr
EGL3Renderer::getRenderState() const noexcept
{
	return _state;
}

GraphicsLayoutPtr
EGL3Renderer::createGraphicsLayout(const GraphicsLayoutDesc& desc) noexcept
{
	auto layout = std::make_shared<EGL3GraphicsLayout>();
	if (layout->open(desc))
		return layout;
	return nullptr;
}

void
EGL3Renderer::setGraphicsLayout(GraphicsLayoutPtr layout) noexcept
{
	if (_inputLayout != layout)
	{
		if (layout)
		{
			if (layout->isInstanceOf<EGL3GraphicsLayout>())
				_inputLayout = layout->downcast<EGL3GraphicsLayout>();
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
EGL3Renderer::getGraphicsLayout() const noexcept
{
	return _inputLayout;
}

GraphicsDataPtr
EGL3Renderer::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	auto type = desc.getType();

	if (type == GraphicsStream::VBO)
		return std::make_shared<EGL3VertexBuffer>(desc);
	else if (type == GraphicsStream::IBO)
		return std::make_shared<EGL3IndexBuffer>(desc);
	else if (type == GraphicsStream::DIBO)
		return std::make_shared<EGL3DrawIndirectBuffer>(desc);

	return nullptr;
}

bool
EGL3Renderer::updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept
{
	if (data)
	{
		auto max = std::numeric_limits<GLsizeiptr>::max();
		if (cnt < max)
		{
			if (data->isInstanceOf<EGL3VertexBuffer>())
				this->setVertexBufferData(data);
			else if (data->isInstanceOf<EGL3IndexBuffer>())
				this->setIndexBufferData(data);
			else
				return false;

			auto _data = data->cast<EGL3GraphicsData>();
			_data->resize((const char*)str, cnt);
			return true;
		}
	}

	return false;
}

void*
EGL3Renderer::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
{
	if (data)
	{
		if (data->isInstanceOf<EGL3VertexBuffer>())
			this->setVertexBufferData(data);
		else if (data->isInstanceOf<EGL3IndexBuffer>())
			this->setIndexBufferData(data);
		else
			return nullptr;

		auto _data = data->cast<EGL3GraphicsData>();
		return _data->map(access);
	}

	return nullptr;
}

void
EGL3Renderer::unmapBuffer(GraphicsDataPtr& data) noexcept
{
	if (data)
	{
		if (data->isInstanceOf<EGL3VertexBuffer>())
			this->setVertexBufferData(data);
		else if (data->isInstanceOf<EGL3IndexBuffer>())
			this->setIndexBufferData(data);
		else
			return;

		auto _data = data->cast<EGL3GraphicsData>();
		_data->unmap();
	}
}

void
EGL3Renderer::setIndexBufferData(GraphicsDataPtr data) noexcept
{
	if (_ibo != data)
	{
		if (data)
		{
			if (data->isInstanceOf<EGL3IndexBuffer>())
				_ibo = data->downcast<EGL3IndexBuffer>();
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
EGL3Renderer::getIndexBufferData() const noexcept
{
	return _ibo;
}

void
EGL3Renderer::setVertexBufferData(GraphicsDataPtr data) noexcept
{
	if (_vbo != data)
	{
		if (data)
		{
			if (data->isInstanceOf<EGL3VertexBuffer>())
				_vbo = data->downcast<EGL3VertexBuffer>();
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
EGL3Renderer::getVertexBufferData() const noexcept
{
	return _vbo;
}

void
EGL3Renderer::drawRenderBuffer(const RenderIndirect& renderable) noexcept
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
		GLenum drawType = EGL3Types::asEGL3VertexType(primitiveType);
		GLenum indexType = _inputLayout->getIndexType();
		GLsizei numInstance = std::max(1, renderable.numInstances);
		GLvoid* offsetIndices = (GLchar*)(nullptr) + (_inputLayout->getIndexSize() * renderable.startIndice);
		GL_CHECK(glDrawElementsInstanced(drawType, renderable.numIndices, indexType, offsetIndices, numInstance));
	}
	else
	{
		GLsizei numInstance = std::max(1, renderable.numInstances);
		GLenum drawType = EGL3Types::asEGL3VertexType(primitiveType);
		GL_CHECK(glDrawArraysInstanced(drawType, renderable.startVertice, renderable.numVertices, numInstance));
	}
}

void
EGL3Renderer::drawRenderBuffer(const RenderIndirects& renderable) noexcept
{
	assert(false);
}

TexturePtr
EGL3Renderer::createTexture() noexcept
{
	auto result = std::make_shared<EGL3Texture>();
	return result;
}

void
EGL3Renderer::setTexture(TexturePtr texture, std::uint32_t slot) noexcept
{
	if (texture)
	{
		GLuint textureID = std::dynamic_pointer_cast<EGL3Texture>(texture)->getInstanceID();
		GLenum textureDim = EGL3Types::asEGL3Target(texture->getTexDim());

		GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CHECK(glBindTexture(textureDim, textureID));
	}
	else
	{
		GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	}
}

SamplerObjectPtr 
EGL3Renderer::createSamplerObject() noexcept
{
	return std::make_shared<EGL3Sampler>();
}

void 
EGL3Renderer::setSamplerObject(SamplerObjectPtr sampler, std::uint32_t slot) noexcept
{
	auto glsampler = sampler->downcast<EGL3Sampler>();
	if (glsampler)
	{
		GLuint samplerID = glsampler->getInstanceID();
		GL_CHECK(glBindSampler(slot, samplerID));
	}
	else
	{
		GL_CHECK(glBindSampler(slot, 0));
	}
}

RenderTexturePtr
EGL3Renderer::createRenderTexture() noexcept
{
	auto result = std::make_shared<EGL3RenderTexture>();
	return result;
}

MultiRenderTexturePtr
EGL3Renderer::createMultiRenderTexture() noexcept
{
	auto result = std::make_shared<EGL3MultiRenderTexture>();
	return result;
}

void
EGL3Renderer::setRenderTexture(RenderTexturePtr target) noexcept
{
	if (_renderTexture != target)
	{
		if (target)
		{
			auto framebuffer = std::dynamic_pointer_cast<EGL3RenderTexture>(target)->getInstanceID();

			GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

			this->setViewport(Viewport(0, 0, target->getWidth(), target->getHeight()));
		}
		else
		{
			GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE));
		}

		_renderTexture = target;
		_multiRenderTexture = nullptr;
	}
}

void
EGL3Renderer::setMultiRenderTexture(MultiRenderTexturePtr target) noexcept
{
	assert(target);

	auto framebuffer = std::dynamic_pointer_cast<EGL3MultiRenderTexture>(target)->getInstanceID();
	if (_multiRenderTexture != target)
	{
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

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
EGL3Renderer::setRenderTextureLayer(RenderTexturePtr renderTexture, std::int32_t layer) noexcept
{
	assert(renderTexture);

	if (renderTexture->getTexDim() == TextureDim::DIM_2D_ARRAY ||
		renderTexture->getTexDim() == TextureDim::DIM_CUBE)
	{
		auto texture = std::dynamic_pointer_cast<EGL3Texture>(renderTexture->getResolveTexture());
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
			GL_CHECK(glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, textureID, 0, layer));
		else if (renderTexture->getTexDim() == TextureDim::DIM_CUBE)
			GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, textureID, 0));
		else
			assert(false);
	}
}

void
EGL3Renderer::blitRenderTexture(RenderTexturePtr src, const Viewport& v1, RenderTexturePtr dest, const Viewport& v2) noexcept
{
	assert(src);

	auto srcTarget = std::dynamic_pointer_cast<EGL3RenderTexture>(src)->getInstanceID();
	GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, srcTarget));

	if (dest)
	{
		auto destTarget = std::dynamic_pointer_cast<EGL3RenderTexture>(dest)->getInstanceID();
		GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destTarget));
	}
	else
		GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));

	GL_CHECK(glBlitFramebuffer(v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST));

	_renderTexture = GL_NONE;
	_multiRenderTexture = GL_NONE;
}

RenderTexturePtr
EGL3Renderer::getRenderTexture() const noexcept
{
	return _renderTexture;
}

MultiRenderTexturePtr
EGL3Renderer::getMultiRenderTexture() const noexcept
{
	return _multiRenderTexture;
}

void
EGL3Renderer::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
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

		GL_CHECK(glClear(mode));

		if (!depthWriteMask && flags & ClearFlags::CLEAR_DEPTH)
		{
			glDepthMask(GL_FALSE);
		}
	}
}

void
EGL3Renderer::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	if (flags & ClearFlags::CLEAR_DEPTH)
	{
		auto depthWriteMask = _stateCaptured->getDepthState().depthWriteMask;
		if (!depthWriteMask && flags & ClearFlags::CLEAR_DEPTH)
		{
			glDepthMask(GL_TRUE);
		}

		GLfloat f = depth;
		GL_CHECK(glClearBufferfv(GL_DEPTH, 0, &f));

		if (!depthWriteMask && flags & ClearFlags::CLEAR_DEPTH)
		{
			glDepthMask(GL_FALSE);
		}
	}

	if (flags & ClearFlags::CLEAR_STENCIL)
	{
		GLint s = stencil;
		GL_CHECK(glClearBufferiv(GL_STENCIL, 0, &s));
	}

	if (flags & ClearFlags::CLEAR_COLOR)
	{
		GL_CHECK(glClearBufferfv(GL_COLOR, i, color.ptr()));
	}
}

void
EGL3Renderer::discardRenderTexture() noexcept
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
EGL3Renderer::readRenderTexture(RenderTexturePtr target, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(target && w && h && data);

	auto framebuffer = std::dynamic_pointer_cast<EGL3RenderTexture>(target)->getInstanceID();
	if (_renderTexture != target)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		_renderTexture = target;
		_multiRenderTexture = nullptr;
	}

	GLenum format = EGL3Types::asEGL3Format(pfd);
	GLenum type = EGL3Types::asEGL3Type(pfd);

	glReadPixels(0, 0, w, h, format, type, data);
}

ShaderPtr
EGL3Renderer::createShader() noexcept
{
	auto result = std::make_shared<EGL3Shader>();
	return result;
}

ShaderObjectPtr
EGL3Renderer::createShaderObject() noexcept
{
	auto result = std::make_shared<EGL3ShaderObject>();
	return result;
}

void
EGL3Renderer::setShaderObject(ShaderObjectPtr shader) noexcept
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
EGL3Renderer::getShaderObject() const noexcept
{
	return _shaderObject;
}

bool
EGL3Renderer::createShaderVariant(ShaderVariant& constant) noexcept
{
	return false;
}

void
EGL3Renderer::destroyShaderVariant(ShaderVariant& constant) noexcept
{
}

void
EGL3Renderer::updateShaderVariant(ShaderVariantPtr constant) noexcept
{
}

void
EGL3Renderer::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
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
EGL3Renderer::initDebugControl() noexcept
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

	GL_CHECK(glEnable(GL_DEBUG_OUTPUT));

	GL_CHECK(glDebugMessageCallback(debugCallBack, this));
	// enable all
	GL_CHECK(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE));
	// disable ids
	GL_CHECK(glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 6, ids, GL_FALSE));
#endif
}

void
EGL3Renderer::initStateSystem() noexcept
{
	GL_CHECK(glEnable(GL_DEPTH_TEST));
	GL_CHECK(glDepthMask(GL_TRUE));
	GL_CHECK(glDepthFunc(GL_LEQUAL));

	GL_CHECK(glEnable(GL_CULL_FACE));
	GL_CHECK(glCullFace(GL_BACK));
	GL_CHECK(glFrontFace(GL_CW));
	GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

	GL_CHECK(glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP));
	GL_CHECK(glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF));

	GL_CHECK(glBlendEquation(GL_FUNC_ADD));
	GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

_NAME_END