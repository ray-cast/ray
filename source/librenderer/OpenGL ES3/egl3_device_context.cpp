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
#include "egl3_device_context.h"
#include "egl3_state.h"
#include "egl3_shader.h"
#include "egl3_texture.h"
#include "egl3_framebuffer.h"
#include "egl3_layout.h"
#include "egl3_vbo.h"
#include "egl3_ibo.h"
#include "egl3_dibo.h"
#include "egl3_sampler.h"

_NAME_BEGIN

__ImplementSubClass(EGL3DeviceContext, GraphicsContext, "EGL3DeviceContext")

EGL3DeviceContext::EGL3DeviceContext() noexcept
	: _initOpenGL(false)
{
}

EGL3DeviceContext::~EGL3DeviceContext() noexcept
{
	this->close();
}

bool
EGL3DeviceContext::open(WindHandle window) except
{
	if (!_initOpenGL)
	{
		_glcontext = std::make_shared<EGL3Canvas>();
		_glcontext->open(window);
		_glcontext->setActive(true);

		this->initStateSystem();
        this->initDebugControl();

		_initOpenGL = true;
	}

	return _initOpenGL;
}

void
EGL3DeviceContext::close() noexcept
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

    _initOpenGL = false;
}

void
EGL3DeviceContext::renderBegin() noexcept
{
	assert(_glcontext);
	_glcontext->setActive(true);
}

void
EGL3DeviceContext::renderEnd() noexcept
{
	assert(_glcontext);
	_glcontext->setActive(false);
}

void
EGL3DeviceContext::setViewport(const Viewport& view, std::size_t i) noexcept
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
EGL3DeviceContext::getViewport(std::size_t i) const noexcept
{
	return _viewport;
}

void
EGL3DeviceContext::setWireframeMode(bool enable) noexcept
{
	_enableWireframe = enable;
}

bool
EGL3DeviceContext::getWireframeMode() const noexcept
{
	return _enableWireframe;
}

void
EGL3DeviceContext::setSwapInterval(SwapInterval interval) noexcept
{
	assert(_glcontext);
	_glcontext->setSwapInterval(interval);
}

SwapInterval
EGL3DeviceContext::getSwapInterval() const noexcept
{
	assert(_glcontext);
	return _glcontext->getSwapInterval();
}

void
EGL3DeviceContext::setGraphicsState(GraphicsStatePtr state) noexcept
{
	if (state)
	{
		auto oglState = state->downcast<EGL3GraphicsState>();
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
		_stateCaptured->apply(*_stateDefalut);
		assert(false);
	}
}

GraphicsStatePtr
EGL3DeviceContext::getGraphicsState() const noexcept
{
	return _state;
}

void
EGL3DeviceContext::setGraphicsLayout(GraphicsLayoutPtr layout) noexcept
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
EGL3DeviceContext::getGraphicsLayout() const noexcept
{
	return _inputLayout;
}

bool
EGL3DeviceContext::updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept
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
EGL3DeviceContext::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
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
EGL3DeviceContext::unmapBuffer(GraphicsDataPtr& data) noexcept
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
EGL3DeviceContext::setIndexBufferData(GraphicsDataPtr data) noexcept
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
EGL3DeviceContext::getIndexBufferData() const noexcept
{
	return _ibo;
}

void
EGL3DeviceContext::setVertexBufferData(GraphicsDataPtr data) noexcept
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
EGL3DeviceContext::getVertexBufferData() const noexcept
{
	return _vbo;
}

void
EGL3DeviceContext::drawRenderBuffer(const RenderIndirect& renderable) noexcept
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
EGL3DeviceContext::drawRenderBuffer(const RenderIndirects& renderable) noexcept
{
	assert(false);
}

void
EGL3DeviceContext::setGraphicsTexture(GraphicsTexturePtr texture, std::uint32_t slot) noexcept
{
	if (texture)
	{
		auto gltexture = texture->downcast<EGL3Texture>();

		GLuint textureID = gltexture->getInstanceID();
		GLenum textureDim = gltexture->getTarget();

		GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CHECK(glBindTexture(textureDim, textureID));
	}
	else
	{
		GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	}
}

void
EGL3DeviceContext::setGraphicsTexture(GraphicsTexturePtr textures[], std::uint32_t first, std::uint32_t count) noexcept
{
	for (std::uint32_t i = first; i < first + count; i++)
		this->setGraphicsTexture(textures[i], i);
}

void
EGL3DeviceContext::setGraphicsSampler(GraphicsSamplerPtr sampler, std::uint32_t slot) noexcept
{
	if (sampler)
	{
		auto glsampler = sampler->downcast<EGL3Sampler>();
		GLuint samplerID = glsampler->getInstanceID();
		GL_CHECK(glBindSampler(slot, samplerID));
	}
	else
	{
		GL_CHECK(glBindSampler(slot, 0));
	}
}

void
EGL3DeviceContext::setGraphicsSampler(GraphicsSamplerPtr samplers[], std::uint32_t first, std::uint32_t count) noexcept
{
	for (std::uint32_t i = first; i < first + count; i++)
		this->setGraphicsSampler(samplers[i], i);
}

void
EGL3DeviceContext::setRenderTexture(GraphicsRenderTexturePtr target) noexcept
{
	if (_renderTexture != target)
	{
		if (_renderTexture)
			_renderTexture->setActive(false);

		if (_multiRenderTexture)
			_multiRenderTexture->setActive(false);

		if (target)
		{
			_renderTexture = target->downcast<EGL3RenderTexture>();
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
EGL3DeviceContext::setMultiRenderTexture(GraphicsMultiRenderTexturePtr target) noexcept
{
	if (_multiRenderTexture != target)
	{
		if (_renderTexture)
			_renderTexture->setActive(false);

		if (_multiRenderTexture)
			_multiRenderTexture->setActive(false);

		if (target)
		{
			_multiRenderTexture = target->downcast<EGL3MultiRenderTexture>();
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
EGL3DeviceContext::setRenderTextureLayer(GraphicsRenderTexturePtr renderTexture, std::int32_t layer) noexcept
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
EGL3DeviceContext::blitRenderTexture(GraphicsRenderTexturePtr src, const Viewport& v1, GraphicsRenderTexturePtr dest, const Viewport& v2) noexcept
{
	assert(src);

	auto srcTarget = src->downcast<EGL3RenderTexture>()->getInstanceID();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, srcTarget);

	if (dest)
	{
		auto destTarget = dest->downcast<EGL3RenderTexture>()->getInstanceID();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destTarget);
	}
	else
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	if (_renderTexture)
	{
		_renderTexture->setActive(false);
		_renderTexture = nullptr;
	}

	if (_multiRenderTexture)
	{
		_multiRenderTexture->setActive(false);
		_multiRenderTexture = nullptr;
	}
}

GraphicsRenderTexturePtr
EGL3DeviceContext::getRenderTexture() const noexcept
{
	return _renderTexture;
}

GraphicsMultiRenderTexturePtr
EGL3DeviceContext::getMultiRenderTexture() const noexcept
{
	return _multiRenderTexture;
}

void
EGL3DeviceContext::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
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
EGL3DeviceContext::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
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
EGL3DeviceContext::discardRenderTexture() noexcept
{
	assert(_renderTexture || _multiRenderTexture);

	if (_renderTexture)
		_renderTexture->downcast<EGL3RenderTexture>()->discard();
	else if (_multiRenderTexture)
		_multiRenderTexture->downcast<EGL3MultiRenderTexture>()->discard();
}

void
EGL3DeviceContext::readRenderTexture(GraphicsRenderTexturePtr target, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(w && h && data);

	if (target)
	{
		GLenum format = EGL3Types::asEGL3Format(pfd);
		GLenum type = EGL3Types::asEGL3Type(pfd);

		if (format != GL_INVALID_ENUM && type != GL_INVALID_ENUM)
		{
			this->setRenderTexture(target);
			glReadPixels(0, 0, w, h, format, type, data);
		}
	}
}

void
EGL3DeviceContext::setShaderObject(ShaderObjectPtr shader) noexcept
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
EGL3DeviceContext::getShaderObject() const noexcept
{
	return _shaderObject;
}

void
EGL3DeviceContext::present() noexcept
{
	assert(_glcontext);
	_glcontext->present();
}

void
EGL3DeviceContext::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
{
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

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
EGL3DeviceContext::initDebugControl() noexcept
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
	// GL_CHECK(glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 6, ids, GL_FALSE));
#endif
}

void
EGL3DeviceContext::initStateSystem() noexcept
{
    GL_CHECK(glClearDepthf(1.0));
    GL_CHECK(glClearColor(0.0, 0.0, 0.0, 0.0));
    GL_CHECK(glClearStencil(0));

    GL_CHECK(glViewport(0, 0, 0, 0));

	GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

	GL_CHECK(glEnable(GL_DEPTH_TEST));
	GL_CHECK(glDepthMask(GL_TRUE));
	GL_CHECK(glDepthFunc(GL_LEQUAL));

	GL_CHECK(glEnable(GL_CULL_FACE));
	GL_CHECK(glCullFace(GL_BACK));
	GL_CHECK(glFrontFace(GL_CW));
	GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

	GL_CHECK(glDisable(GL_STENCIL_TEST));
	GL_CHECK(glStencilMask(0xFFFFFFFF));
	GL_CHECK(glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP));
	GL_CHECK(glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF));

	GL_CHECK(glDisable(GL_BLEND));
	GL_CHECK(glBlendEquation(GL_FUNC_ADD));
	GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    _maxTextureUnits = MAX_TEXTURE_UNIT;
    _maxViewports = 4;

    _clearColor.set(0.0, 0.0, 0.0, 0.0);
    _clearDepth = 1.0;
    _clearStencil = 0;

	_viewport.left = 0;
	_viewport.top = 0;
	_viewport.width = 0;
	_viewport.height = 0;

    _enableWireframe = false;

    _textureUnits.resize(_maxTextureUnits);

    _stateDefalut = std::make_shared<EGL3GraphicsState>();
    _stateCaptured = std::make_shared<EGL3GraphicsState>();

}

_NAME_END