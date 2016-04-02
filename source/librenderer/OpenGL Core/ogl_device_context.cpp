// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include "ogl_swapchain.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_texture.h"
#include "ogl_framebuffer.h"
#include "ogl_input_layout.h"
#include "ogl_graphics_data.h"
#include "ogl_sampler.h"
#include "ogl_descriptor.h"
#include "ogl_render_pipeline.h"

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
OGLDeviceContext::setup(const GraphicsContextDesc& desc) noexcept
{
	if (_initOpenGL)
		return false;

	if (!desc.getSwapchain())
		return false;

	_glcontext = desc.getSwapchain()->downcast<OGLSwapchain>();
	if (_glcontext)
	{
		this->initDebugControl();
		this->initStateSystem();

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
OGLDeviceContext::setScissor(const Scissor& scissor, std::size_t i) noexcept
{
	if (_scissor[i] != scissor)
	{
		glScissorIndexed(i, scissor.left, scissor.top, scissor.width, scissor.height);
		_scissor[i] = scissor;
	}
}

const Scissor&
OGLDeviceContext::getScissor(std::size_t i) const noexcept
{
	return _scissor[i];
}

void
OGLDeviceContext::setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept
{
	assert(pipeline);

	if (_pipeline != pipeline)
	{
		auto& pipelineDesc = pipeline->getGraphicsPipelineDesc();

		auto glstate = pipelineDesc.getGraphicsState()->downcast<OGLGraphicsState>();
		if (_state != glstate)
		{
			glstate->apply(_stateCaptured);

			_state = glstate;
			_stateCaptured = glstate->getGraphicsStateDesc();
		}

		auto glshader = pipelineDesc.getGraphicsProgram()->downcast<OGLShaderObject>();
		if (_shaderObject != glshader)
		{
			glUseProgram(glshader->getInstanceID());
			_shaderObject = glshader;
		}

		_needUpdateLayout = true;
	}
}

GraphicsPipelinePtr 
OGLDeviceContext::getRenderPipeline() const noexcept
{
	return _pipeline;
}

void 
OGLDeviceContext::setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept
{
	_descriptorSet = descriptorSet->downcast<OGLDescriptorSet>();
	_descriptorSet->bindProgram(_shaderObject);
}

GraphicsDescriptorSetPtr 
OGLDeviceContext::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

void
OGLDeviceContext::setInputLayout(GraphicsInputLayoutPtr inputLayout) noexcept
{
	assert(inputLayout);
	_inputLayout = inputLayout->downcast<OGLInputLayout>();
	_needUpdateLayout = true;
}

GraphicsInputLayoutPtr
OGLDeviceContext::getInputLayout() const noexcept
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
		if (data && data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer)
			_ibo = data->downcast<OGLGraphicsData>();
		else
			_ibo = nullptr;

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
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);

	if (_vbo != data)
	{
		if (data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer)
			_vbo = data->downcast<OGLGraphicsData>();
		else
			_vbo = nullptr;
		_needUpdateVbo = true;
	}
}

GraphicsDataPtr
OGLDeviceContext::getVertexBufferData() const noexcept
{
	return _vbo;
}

void
OGLDeviceContext::drawRenderBuffer(const GraphicsIndirect& renderable) noexcept
{
	assert(_vbo && _inputLayout);

	if (_inputLayout->getGraphicsInputLayoutDesc().getIndexType() != GraphicsIndexType::GraphicsIndexTypeNone)
	{
		if (!_ibo)
			return;

		if (_ibo->downcast<OGLGraphicsData>()->size() < _inputLayout->getGraphicsInputLayoutDesc().getIndexSize() * (renderable.startIndice + renderable.numIndices))
			return;
	}

	if (_needUpdateLayout)
	{
		if (_inputLayout)
			_inputLayout->bindLayout(_shaderObject);
		_needUpdateLayout = false;
	}

	if (_needUpdateVbo)
	{
		if (_vbo)
			_inputLayout->bindVbo(_vbo, 0);
		_needUpdateVbo = false;
	}

	if (_needUpdateIbo)
	{
		if (_ibo)
			_inputLayout->bindIbo(_ibo);
		_needUpdateIbo = false;
	}

	auto primitiveType = _stateCaptured.getRasterState().primitiveType;
	if (_ibo)
	{
		GLenum drawType = OGLTypes::asVertexType(primitiveType);
		GLenum indexType = OGLTypes::asIndexType(_inputLayout->getGraphicsInputLayoutDesc().getIndexType());
		GLsizei numInstance = std::max(1, renderable.numInstances);
		GLvoid* offsetIndices = (GLchar*)(nullptr) + (_inputLayout->getGraphicsInputLayoutDesc().getIndexSize() * renderable.startIndice);
		glDrawElementsInstancedBaseVertexBaseInstance(drawType, renderable.numIndices, indexType, offsetIndices, numInstance, renderable.startVertice, renderable.startInstances);
	}
	else
	{
		GLsizei numInstance = std::max(1, renderable.numInstances);
		GLenum drawType = OGLTypes::asVertexType(primitiveType);
		glDrawArraysInstancedBaseInstance(drawType, renderable.startVertice, renderable.numVertices, numInstance, renderable.startInstances);
	}
}

void
OGLDeviceContext::drawRenderBuffer(const GraphicsIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	for (std::size_t i = first; i < first + count; i++)
		this->drawRenderBuffer(renderable[i]);
}

void
OGLDeviceContext::setRenderTexture(GraphicsFramebufferPtr target) noexcept
{
	if (_renderTexture != target)
	{
		if (target)
		{
			_renderTexture = target->downcast<OGLFramebuffer>();
			auto& renderTextureDesc = _renderTexture->getGraphicsFramebufferDesc();
			auto& textures = renderTextureDesc.getTextures();

			glBindFramebuffer(GL_FRAMEBUFFER, _renderTexture->getInstanceID());

			std::size_t size = textures.size();
			if (size > 0)
			{
				for (std::size_t index = 0; index < size; index++)
				{
					auto& textureDesc = textures[index]->getGraphicsTextureDesc();
					this->setViewport(Viewport(0, 0, textureDesc.getWidth(), textureDesc.getHeight()), index);
				}
			}
			else
			{
				auto depthTexture = renderTextureDesc.getSharedDepthTexture();
				if (depthTexture)
				{
					auto& textureDesc = depthTexture->getGraphicsTextureDesc();
					this->setViewport(Viewport(0, 0, textureDesc.getWidth(), textureDesc.getHeight()), 0);
				}
			}
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
			_renderTexture = nullptr;
		}
	}
}

void
OGLDeviceContext::blitRenderTexture(GraphicsFramebufferPtr src, const Viewport& v1, GraphicsFramebufferPtr dest, const Viewport& v2) noexcept
{
	assert(src);

	auto readFramebuffer = src->downcast<OGLFramebuffer>()->getInstanceID();
	auto drawFramebuffer = dest ? dest->downcast<OGLFramebuffer>()->getInstanceID() : GL_NONE;

	glBlitNamedFramebuffer(readFramebuffer, drawFramebuffer, v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

GraphicsFramebufferPtr
OGLDeviceContext::getRenderTexture() const noexcept
{
	return _renderTexture;
}

void
OGLDeviceContext::clearRenderTexture(GraphicsClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
{
	GLbitfield mode = 0;

	if (flags & GraphicsClearFlags::GraphicsClearFlagsColor)
	{
		mode |= GL_COLOR_BUFFER_BIT;

		if (_clearColor != color)
		{
			glClearColor(color.x, color.y, color.z, color.w);
			_clearColor = color;
		}
	}

	if (flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
	{
		mode |= GL_DEPTH_BUFFER_BIT;

		if (_clearDepth != depth)
		{
			glClearDepthf(depth);
			_clearDepth = depth;
		}
	}

	if (flags & GraphicsClearFlags::GraphicsClearFlagsStencil)
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
		auto depthWriteEnable = _stateCaptured.getDepthState().depthWriteEnable;
		if (!depthWriteEnable && flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
		{
			glDepthMask(GL_TRUE);
		}

		glClear(mode);

		if (!depthWriteEnable && flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
		{
			glDepthMask(GL_FALSE);
		}
	}
}

void
OGLDeviceContext::clearRenderTexture(GraphicsClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	if (flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
	{
		auto depthWriteEnable = _stateCaptured.getDepthState().depthWriteEnable;
		if (!depthWriteEnable && flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
		{
			glDepthMask(GL_TRUE);
		}

		GLfloat f = depth;
		glClearBufferfv(GL_DEPTH, 0, &f);

		if (!depthWriteEnable && flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
		{
			glDepthMask(GL_FALSE);
		}
	}

	if (flags & GraphicsClearFlags::GraphicsClearFlagsStencil)
	{
		GLint s = stencil;
		glClearBufferiv(GL_STENCIL, 0, &s);
	}

	if (flags & GraphicsClearFlags::GraphicsClearFlagsColor)
	{
		glClearBufferfv(GL_COLOR, i, color.ptr());
	}
}

void
OGLDeviceContext::discardRenderTexture() noexcept
{
	assert(_renderTexture);
	_renderTexture->discard();
}

void
OGLDeviceContext::readRenderTexture(GraphicsFramebufferPtr target, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(w && h && data);

	if (target)
	{
		GLenum format = OGLTypes::asTextureFormat(pfd);
		GLenum type = OGLTypes::asTextureType(pfd);

		if (format != GL_INVALID_ENUM && type != GL_INVALID_ENUM)
		{
			this->setRenderTexture(target);
			glReadPixels(0, 0, w, h, format, type, data);
		}
	}
}

void
OGLDeviceContext::present() noexcept
{
	if (_glcontext)
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

	_needUpdateLayout = false;
	_needUpdateVbo = false;
	_needUpdateIbo = false;

	_viewport.resize(_maxViewports);

	_stateDefault = std::make_shared<OGLGraphicsState>();
	_stateDefault->setup(GraphicsStateDesc());

	_stateCaptured = GraphicsStateDesc();
}

void
OGLDeviceContext::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLDeviceContext::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END