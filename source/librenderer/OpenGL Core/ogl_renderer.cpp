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
#include "ogl_renderer.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_texture.h"
#include "ogl_buffer.h"
#include "ogl_commandlist.h"

_NAME_BEGIN

OGLRenderer::OGLRenderer() noexcept
	: _initOpenGL(false)
	, _maxTextureUnits(32)
	, _maxViewports(4)
	, _clearColor(0.0, 0.0, 0.0)
	, _clearDepth(0.0)
	, _clearStencil(0xFFFFFFFF)
	, _state(GL_NONE)
	, _renderTexture(GL_NONE)
	, _defaultVAO(GL_NONE)
	, _enableWireframe(false)
{
	_viewport.resize(_maxViewports);
	_stateCaptured = std::make_shared<OGLRenderState>();
}

OGLRenderer::~OGLRenderer() noexcept
{
	this->close();
}

bool
OGLRenderer::open(WindHandle window) except
{
	if (!_initOpenGL)
	{
		_glcontext = this->createRenderWindow();
		_glcontext->open(window);

		this->initDebugControl();
		this->initStateSystem();
		this->initCommandList();

		_initOpenGL = true;
	}

	return _initOpenGL;
}

void
OGLRenderer::close() noexcept
{
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

	if (_defaultVAO != GL_NONE)
	{
		glDeleteVertexArrays(1, &_defaultVAO);
		_defaultVAO = GL_NONE;
	}

	if (_glcontext)
	{
		_glcontext.reset();
		_glcontext = nullptr;
	}
}

void
OGLRenderer::renderBegin() noexcept
{
	this->setShaderObject(nullptr);
	this->setRenderTexture(nullptr);
	this->setRenderBuffer(nullptr);
}

void
OGLRenderer::renderEnd() noexcept
{
	assert(_glcontext);
	_glcontext->present();
}

void
OGLRenderer::setViewport(const Viewport& view, std::size_t i) noexcept
{
	if (_viewport[i] != view)
	{
		glViewportIndexedf(i, view.left, view.top, view.width, view.height);
		_viewport[i] = view;
	}
}

const Viewport&
OGLRenderer::getViewport(std::size_t i) const noexcept
{
	return _viewport[i];
}

void
OGLRenderer::setWireframeMode(bool enable) noexcept
{
	_enableWireframe = enable;
}

bool
OGLRenderer::getWireframeMode() const noexcept
{
	return _enableWireframe;
}

RenderWindowPtr
OGLRenderer::createRenderWindow() const noexcept
{
	return std::make_shared<OGLCanvas>();
}

void
OGLRenderer::setRenderWindow(RenderWindowPtr glcontext) except
{
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
OGLRenderer::getRenderWindow() const noexcept
{
	return _glcontext;
}

void
OGLRenderer::setSwapInterval(SwapInterval interval) except
{
	assert(_glcontext);
	_glcontext->setSwapInterval(interval);
}

SwapInterval
OGLRenderer::getSwapInterval() const noexcept
{
	assert(_glcontext);
	return _glcontext->getSwapInterval();
}

RenderStatePtr
OGLRenderer::createRenderState() noexcept
{
	return std::make_shared<OGLRenderState>();
}

void
OGLRenderer::setRenderState(RenderStatePtr state) noexcept
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
OGLRenderer::getRenderState() const noexcept
{
	return _state;
}

RenderBufferPtr
OGLRenderer::createRenderBuffer() noexcept
{
	return	std::make_shared<OGLRenderBuffer>();
}

IndexBufferDataPtr
OGLRenderer::createIndexBufferData() noexcept
{
	return std::make_shared<OGLIndexBuffer>();
}

VertexBufferDataPtr
OGLRenderer::createVertexBufferData() noexcept
{
	return std::make_shared<OGLVertexBuffer>();
}

void
OGLRenderer::setRenderBuffer(RenderBufferPtr buffer) noexcept
{
	if (_renderBuffer != buffer)
	{
		if (buffer)
			buffer->apply();
		_renderBuffer = buffer;
	}
}

void
OGLRenderer::updateRenderBuffer(RenderBufferPtr renderBuffer) noexcept
{
}

void
OGLRenderer::drawRenderBuffer(const RenderIndirect& renderable) noexcept
{
	assert(_renderBuffer && _stateCaptured);
	assert(_renderBuffer->getNumVertices() >= renderable.startVertice + renderable.numVertices);
	assert(_renderBuffer->getNumIndices() >= renderable.startIndice + renderable.numIndices);

	auto primitiveType = _stateCaptured->getRasterState().primitiveType;

	if (_enableWireframe)
	{
		if (primitiveType == GPU_POINT_OR_LINE ||
			primitiveType == GPU_TRIANGLE_OR_LINE ||
			primitiveType == GPU_FAN_OR_LINE)
		{
			primitiveType = GPU_LINE;
		}
	}

	GLenum drawType = OGLTypes::asOGLVertexType(primitiveType);
	auto ib = _renderBuffer->getIndexBuffer();
	if (ib)
	{
		GLenum indexType = OGLTypes::asOGLIndexType(ib->getIndexType());

		GLvoid* offsetIndices = (GLchar*)(_renderBuffer->getIndexBuffer()->getIndexSize() * renderable.startIndice);
		GLsizei numInstance = std::max(1, renderable.numInstances);
		glDrawElementsInstancedBaseVertexBaseInstance(drawType, renderable.numIndices, indexType, offsetIndices, numInstance, renderable.startVertice, renderable.startInstances);
	}
	else
	{
		GLsizei numInstance = std::max(1, renderable.numInstances);
		glDrawArraysInstancedBaseInstance(drawType, renderable.startVertice, renderable.numVertices, numInstance, renderable.startInstances);
	}
}

void
OGLRenderer::drawRenderBuffer(const RenderIndirects& renderable) noexcept
{
	assert(false);
}

RenderBufferPtr
OGLRenderer::getRenderBuffer() const noexcept
{
	return _renderBuffer;
}

TexturePtr
OGLRenderer::createTexture() noexcept
{	
	return std::make_shared<OGLTexture>();
}

void 
OGLRenderer::setTexture(TexturePtr texture, std::uint32_t slot) noexcept
{
	auto _texture = std::dynamic_pointer_cast<OGLTexture>(texture);
	if (_texture)
	{
		GLuint textureID = _texture->getInstanceID();
		GLenum textureDim = OGLTypes::asOGLTarget(_texture->getTexDim());

		glBindTextureUnit(slot, textureID);
	}
	else
	{
		glBindTextureUnit(slot, 0);
	}
}

RenderTexturePtr
OGLRenderer::createRenderTexture() noexcept
{	
	return std::make_shared<OGLRenderTexture>();
}

MultiRenderTexturePtr
OGLRenderer::createMultiRenderTexture() noexcept
{	
	return std::make_shared<OGLMultiRenderTexture>();
}

void
OGLRenderer::setRenderTexture(RenderTexturePtr target) noexcept
{
	if (_renderTexture != target)
	{
		if (target)
		{
			auto framebuffer = std::dynamic_pointer_cast<OGLRenderTexture>(target)->getInstanceID();

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

			this->setViewport(Viewport(0, 0, target->getWidth(), target->getHeight()));
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
OGLRenderer::setMultiRenderTexture(MultiRenderTexturePtr target) noexcept
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
OGLRenderer::setRenderTextureLayer(RenderTexturePtr renderTexture, std::int32_t layer) noexcept
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
OGLRenderer::blitRenderTexture(RenderTexturePtr src, const Viewport& v1, RenderTexturePtr dest, const Viewport& v2) noexcept
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
OGLRenderer::getRenderTexture() const noexcept
{
	return _renderTexture;
}

MultiRenderTexturePtr
OGLRenderer::getMultiRenderTexture() const noexcept
{
	return _multiRenderTexture;
}

void
OGLRenderer::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
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
OGLRenderer::clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
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
OGLRenderer::discardRenderTexture() noexcept
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
OGLRenderer::readRenderTexture(RenderTexturePtr target, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
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

ShaderPtr
OGLRenderer::createShader() noexcept
{	
	return std::make_shared<OGLShader>();
}

ShaderObjectPtr
OGLRenderer::createShaderObject() noexcept
{	
	return std::make_shared<OGLShaderObject>();
}

void
OGLRenderer::setShaderObject(ShaderObjectPtr shader) noexcept
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
			if (it->getType() == SPT_TEXTURE)
			{
				auto uniform = std::dynamic_pointer_cast<OGLShaderUniform>(it);
				auto bindingPoint = uniform->getBindingPoint();
				auto texture = std::dynamic_pointer_cast<OGLTexture>(uniform->getTexture());
				auto sampler = std::dynamic_pointer_cast<OGLTextureSampler>(uniform->getSampler());

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
OGLRenderer::getShaderObject() const noexcept
{
	return _shaderObject;
}

bool
OGLRenderer::createShaderVariant(ShaderVariant& constant) noexcept
{
	return false;
}

void
OGLRenderer::destroyShaderVariant(ShaderVariant& constant) noexcept
{
}

void
OGLRenderer::updateShaderVariant(ShaderVariantPtr constant) noexcept
{
}

void
OGLRenderer::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
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
OGLRenderer::initDebugControl() noexcept
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
OGLRenderer::initCommandList() noexcept
{
	initCommandListNV();
}

void
OGLRenderer::initStateSystem() noexcept
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

	glGenVertexArrays(1, &_defaultVAO);
	glBindVertexArray(_defaultVAO);

	glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_POSITION);
	glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_NORMAL);
	glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD);
	glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_DIFFUSE);
	glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_TANGENT);
	glEnableVertexAttribArray((GLuint)VertexAttrib::GPU_ATTRIB_BITANGENT);

	glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_DIFFUSE, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0);
	glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_TANGENT, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat((GLuint)VertexAttrib::GPU_ATTRIB_BITANGENT, 3, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_POSITION, (GLuint)VertexAttrib::GPU_ATTRIB_POSITION);
	glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_NORMAL, (GLuint)VertexAttrib::GPU_ATTRIB_NORMAL);
	glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD, (GLuint)VertexAttrib::GPU_ATTRIB_TEXCOORD);
	glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_DIFFUSE, (GLuint)VertexAttrib::GPU_ATTRIB_DIFFUSE);
	glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_TANGENT, (GLuint)VertexAttrib::GPU_ATTRIB_TANGENT);
	glVertexAttribBinding((GLuint)VertexAttrib::GPU_ATTRIB_BITANGENT, (GLuint)VertexAttrib::GPU_ATTRIB_BITANGENT);
}

_NAME_END