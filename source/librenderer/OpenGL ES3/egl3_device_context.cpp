// +---------------------------------------------------------------------
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
#include "egl3_device_context.h"
#include "egl3_state.h"
#include "egl3_shader.h"
#include "egl3_texture.h"
#include "egl3_framebuffer.h"
#include "egl3_input_layout.h"
#include "egl3_sampler.h"
#include "egl3_descriptor.h"
#include "egl3_render_pipeline.h"
#include "egl3_swapchain.h"
#include "egl3_graphics_data.h"

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
EGL3DeviceContext::setup(const GraphicsContextDesc& desc) noexcept
{
	if (_initOpenGL)
		return false;

	if (!desc.getSwapchain())
		return false;

	_glcontext = desc.getSwapchain()->downcast<EGL3Swapchain>();
	if (_glcontext)
	{
		_glcontext->setActive(true);
		this->initDebugControl();
		this->initStateSystem();

		_initOpenGL = true;
	}

	return _initOpenGL;
}

void
EGL3DeviceContext::close() noexcept
{
	if (!_initOpenGL)
		return;

	_shaderObject.reset();
	_renderTexture.reset();
	_state.reset();
	_glcontext.reset();

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
}

void
EGL3DeviceContext::setViewport(const Viewport& view, std::size_t i) noexcept
{
	if (_viewport != view)
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
EGL3DeviceContext::setScissor(const Scissor& scissor, std::size_t i) noexcept
{
	if (_scissor != scissor)
	{
		glScissor(scissor.left, scissor.top, scissor.width, scissor.height);
		_scissor = scissor;
	}
}

const Scissor&
EGL3DeviceContext::getScissor(std::size_t i) const noexcept
{
	return _scissor;
}

void
EGL3DeviceContext::setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept
{
	assert(pipeline);

	if (_pipeline != pipeline)
	{
		auto& pipelineDesc = pipeline->getGraphicsPipelineDesc();

		auto glstate = pipelineDesc.getGraphicsState()->downcast<EGL3GraphicsState>();
		if (_state != glstate)
		{
			glstate->apply(_stateCaptured);

			_state = glstate;
			_stateCaptured = glstate->getGraphicsStateDesc();
		}

		auto glshader = pipelineDesc.getGraphicsProgram()->downcast<EGL3ShaderObject>();
		if (_shaderObject != glshader)
		{
			glUseProgram(glshader->getInstanceID());
			_shaderObject = glshader;
		}

		_needUpdateLayout = true;
	}
}

GraphicsPipelinePtr
EGL3DeviceContext::getRenderPipeline() const noexcept
{
	return _pipeline;
}

void
EGL3DeviceContext::setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept
{
	_descriptorSet = descriptorSet->downcast<EGL3DescriptorSet>();
	_descriptorSet->bindProgram(_shaderObject);
}

GraphicsDescriptorSetPtr
EGL3DeviceContext::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

void
EGL3DeviceContext::setInputLayout(GraphicsInputLayoutPtr inputLayout) noexcept
{
	assert(inputLayout);
	_inputLayout = inputLayout->downcast<EGL3InputLayout>();
	_needUpdateLayout = true;
}

GraphicsInputLayoutPtr
EGL3DeviceContext::getInputLayout() const noexcept
{
	return _inputLayout;
}

bool
EGL3DeviceContext::updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept
{
	assert(data);
	auto _data = data->cast<EGL3GraphicsData>();
	_data->resize((const char*)str, cnt);
	return true;
}

void*
EGL3DeviceContext::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
{
	if (data)
	{
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
		auto _data = data->cast<EGL3GraphicsData>();
		_data->unmap();
	}
}

void
EGL3DeviceContext::setIndexBufferData(GraphicsDataPtr data) noexcept
{
	if (_ibo != data)
	{
		if (data && data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer)
			_ibo = data->downcast<EGL3GraphicsData>();
		else
			_ibo = nullptr;

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
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);

	if (_vbo != data)
	{
		if (data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer)
			_vbo = data->downcast<EGL3GraphicsData>();
		else
			_vbo = nullptr;
		_needUpdateVbo = true;
	}
}

GraphicsDataPtr
EGL3DeviceContext::getVertexBufferData() const noexcept
{
	return _vbo;
}

void
EGL3DeviceContext::drawRenderBuffer(const GraphicsIndirect& renderable) noexcept
{
	assert(_inputLayout || _vbo);

	if (_inputLayout->getGraphicsInputLayoutDesc().getIndexType() != GraphicsIndexType::GraphicsIndexTypeNone)
	{
		if (!_ibo)
			return;

		if (_ibo->size() < _inputLayout->getGraphicsInputLayoutDesc().getIndexSize() * (renderable.startIndice + renderable.numIndices))
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
			_inputLayout->bindVbo(_vbo);
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
		GLenum drawType = EGL3Types::asVertexType(primitiveType);
		GLenum indexType = EGL3Types::asIndexType(_inputLayout->getGraphicsInputLayoutDesc().getIndexType());
		GLsizei numInstance = std::max(1, renderable.numInstances);
		GLvoid* offsetIndices = (GLchar*)(nullptr) + (_inputLayout->getGraphicsInputLayoutDesc().getIndexSize() * renderable.startIndice);
		GL_CHECK(glDrawElementsInstanced(drawType, renderable.numIndices, indexType, offsetIndices, numInstance));
	}
	else
	{
		GLsizei numInstance = std::max(1, renderable.numInstances);
		GLenum drawType = EGL3Types::asVertexType(primitiveType);
		GL_CHECK(glDrawArraysInstanced(drawType, renderable.startVertice, renderable.numVertices, numInstance));
	}
}

void
EGL3DeviceContext::drawRenderBuffer(const GraphicsIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	for (std::size_t i = first; i < first + count; i++)
		this->drawRenderBuffer(renderable[i]);
}

void
EGL3DeviceContext::setRenderTexture(GraphicsFramebufferPtr target) noexcept
{
	if (_renderTexture != target)
	{
		if (target)
		{
			_renderTexture = target->downcast<EGL3Framebuffer>();
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
EGL3DeviceContext::blitRenderTexture(GraphicsFramebufferPtr src, const Viewport& v1, GraphicsFramebufferPtr dest, const Viewport& v2) noexcept
{
	assert(src);

	auto srcTarget = src->downcast<EGL3Framebuffer>()->getInstanceID();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, srcTarget);

	if (dest)
	{
		auto destTarget = dest->downcast<EGL3Framebuffer>()->getInstanceID();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destTarget);
	}
	else
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	_renderTexture = nullptr;
}

GraphicsFramebufferPtr
EGL3DeviceContext::getRenderTexture() const noexcept
{
	return _renderTexture;
}

void
EGL3DeviceContext::clearRenderTexture(GraphicsClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept
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

		GL_CHECK(glClear(mode));

		if (!depthWriteEnable && flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
		{
			glDepthMask(GL_FALSE);
		}
	}
}

void
EGL3DeviceContext::clearRenderTexture(GraphicsClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	if (flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
	{
		auto depthWriteEnable = _stateCaptured.getDepthState().depthWriteEnable;
		if (!depthWriteEnable && flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
		{
			glDepthMask(GL_TRUE);
		}

		GLfloat f = depth;
		GL_CHECK(glClearBufferfv(GL_DEPTH, 0, &f));

		if (!depthWriteEnable && flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
		{
			glDepthMask(GL_FALSE);
		}
	}

	if (flags & GraphicsClearFlags::GraphicsClearFlagsStencil)
	{
		GLint s = stencil;
		GL_CHECK(glClearBufferiv(GL_STENCIL, 0, &s));
	}

	if (flags & GraphicsClearFlags::GraphicsClearFlagsColor)
	{
		GL_CHECK(glClearBufferfv(GL_COLOR, i, color.ptr()));
	}
}

void
EGL3DeviceContext::discardRenderTexture() noexcept
{
	if (_renderTexture)
		_renderTexture->downcast<EGL3Framebuffer>()->discard();
}

void
EGL3DeviceContext::readRenderTexture(GraphicsFramebufferPtr target, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(w && h && data);

	if (target)
	{
		GLenum format = EGL3Types::asTextureFormat(pfd);
		GLenum type = EGL3Types::asTextureType(pfd);

		if (format != GL_INVALID_ENUM && type != GL_INVALID_ENUM)
		{
			this->setRenderTexture(target);
			glReadPixels(0, 0, w, h, format, type, data);
		}
	}
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

    _stateDefault = std::make_shared<EGL3GraphicsState>();
    _stateCaptured = GraphicsStateDesc();
}

void
EGL3DeviceContext::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3DeviceContext::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END