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
#include "egl2_device_context.h"
#include "egl2_state.h"
#include "egl2_shader.h"
#include "egl2_texture.h"
#include "egl2_input_layout.h"
#include "egl2_sampler.h"
#include "egl2_framebuffer.h"
#include "egl2_graphics_data.h"
#include "egl2_swapchain.h"
#include "egl2_render_pipeline.h"
#include "egl2_descriptor.h"

_NAME_BEGIN

__ImplementSubClass(EGL2DeviceContext, GraphicsContext, "EGL2DeviceContext")

EGL2DeviceContext::EGL2DeviceContext() noexcept
	: _initOpenGL(false)
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

EGL2DeviceContext::~EGL2DeviceContext() noexcept
{
	this->close();
}

bool
EGL2DeviceContext::setup(const GraphicsContextDesc& desc) noexcept
{
	if (_initOpenGL)
		return false;

	if (!desc.getSwapchain())
		return false;

	_glcontext = desc.getSwapchain()->downcast<EGL2Swapchain>();
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
EGL2DeviceContext::close() noexcept
{
	if (!_initOpenGL)
		return;

	if (_vbo)
	{
		_vbo.reset();
		_vbo = nullptr;
	}

	if (_ibo)
	{
		_ibo.reset();
		_ibo = nullptr;
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
EGL2DeviceContext::renderBegin() noexcept
{
	assert(_glcontext);
	_glcontext->setActive(true);
}

void
EGL2DeviceContext::renderEnd() noexcept
{
}

void
EGL2DeviceContext::setViewport(const Viewport& view, std::size_t i) noexcept
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

void
EGL2DeviceContext::setScissor(const Scissor& scissor, std::size_t i) noexcept
{
	if (_scissor != scissor)
	{
		glScissor(scissor.left, scissor.top, scissor.width, scissor.height);
		_scissor = scissor;
	}
}

const Scissor&
EGL2DeviceContext::getScissor(std::size_t i) const noexcept
{
	return _scissor;
}

void
EGL2DeviceContext::setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept
{
	assert(pipeline);

	if (_pipeline != pipeline)
	{
		auto& pipelineDesc = pipeline->getGraphicsPipelineDesc();

		auto glstate = pipelineDesc.getGraphicsState()->downcast<EGL2GraphicsState>();
		if (_state != glstate)
		{
			glstate->apply(_stateCaptured);

			_state = glstate;
			_stateCaptured = glstate->getGraphicsStateDesc();
		}

		auto glshader = pipelineDesc.getGraphicsProgram()->downcast<EGL2ShaderObject>();
		if (_shaderObject != glshader)
		{
			glUseProgram(glshader->getInstanceID());
			_shaderObject = glshader;
		}

		_needUpdateLayout = true;
	}
}

GraphicsPipelinePtr
EGL2DeviceContext::getRenderPipeline() const noexcept
{
	return _pipeline;
}

void
EGL2DeviceContext::setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept
{
	_descriptorSet = descriptorSet->downcast<EGL2DescriptorSet>();
	_descriptorSet->bindProgram(_shaderObject);
}

GraphicsDescriptorSetPtr
EGL2DeviceContext::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

const Viewport&
EGL2DeviceContext::getViewport(std::size_t i) const noexcept
{
	return _viewport;
}

void
EGL2DeviceContext::setInputLayout(GraphicsInputLayoutPtr inputLayout) noexcept
{
	assert(inputLayout);
	_inputLayout = inputLayout->downcast<EGL2InputLayout>();
	_needUpdateLayout = true;
}

GraphicsInputLayoutPtr
EGL2DeviceContext::getInputLayout() const noexcept
{
	return _inputLayout;
}

bool
EGL2DeviceContext::updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept
{
	auto _data = data->cast<EGL2GraphicsData>();
	_data->resize((const char*)str, cnt);
	return true;
}

void*
EGL2DeviceContext::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
{
	auto _data = data->cast<EGL2GraphicsData>();
	return _data->map(access);
}

void
EGL2DeviceContext::unmapBuffer(GraphicsDataPtr& data) noexcept
{
	auto _data = data->cast<EGL2GraphicsData>();
	_data->unmap();
}

GraphicsDataPtr
EGL2DeviceContext::getIndexBufferData() const noexcept
{
	return _ibo;
}

void
EGL2DeviceContext::setVertexBufferData(GraphicsDataPtr data) noexcept
{
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);

	if (_vbo != data)
	{
		if (data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer)
			_vbo = data->downcast<EGL2GraphicsData>();
		else
			_vbo = nullptr;
		_needUpdateVbo = true;
	}
}

void
EGL2DeviceContext::setIndexBufferData(GraphicsDataPtr data) noexcept
{
	if (_ibo != data)
	{
		if (data && data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer)
			_ibo = data->downcast<EGL2GraphicsData>();
		else
			_ibo = nullptr;

		_needUpdateIbo = true;
	}
}

GraphicsDataPtr
EGL2DeviceContext::getVertexBufferData() const noexcept
{
	return _vbo;
}

void
EGL2DeviceContext::drawRenderBuffer(const GraphicsIndirect& renderable) noexcept
{
	if (!_inputLayout || !_vbo)
		return;

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
		GLenum drawType = EGL2Types::asVertexType(primitiveType);
		GLenum indexType = _inputLayout->getGraphicsInputLayoutDesc().getIndexType();
		GLvoid* offsetIndices = (GLchar*)(nullptr) + (_inputLayout->getGraphicsInputLayoutDesc().getIndexSize() * renderable.startIndice);
		GL_CHECK(glDrawElements(drawType, renderable.numIndices, indexType, offsetIndices));
	}
	else
	{
		GLenum drawType = EGL2Types::asVertexType(primitiveType);
		GL_CHECK(glDrawArrays(drawType, renderable.startVertice, renderable.numVertices));
	}
}

void
EGL2DeviceContext::drawRenderBuffer(const GraphicsIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	for (std::size_t i = first; i < first + count; i++)
		this->drawRenderBuffer(renderable[i]);
}

void
EGL2DeviceContext::setRenderTexture(GraphicsFramebufferPtr target) noexcept
{
	if (_renderTexture != target)
	{
		if (target)
		{
			_renderTexture = target->downcast<EGL2Framebuffer>();
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
EGL2DeviceContext::blitRenderTexture(GraphicsFramebufferPtr src, const Viewport& v1, GraphicsFramebufferPtr dest, const Viewport& v2) noexcept
{
	GL_PLATFORM_LOG("Can't support blitRenderTexture");
}

GraphicsFramebufferPtr
EGL2DeviceContext::getRenderTexture() const noexcept
{
	return _renderTexture;
}

void
EGL2DeviceContext::clearRenderTexture(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	GLbitfield mode = 0;

	if (flags & GraphicsClearFlags::GraphicsClearFlagsColor)
	{
		mode |= GL_COLOR_BUFFER_BIT;

		if (_clearColor != color)
		{
			GL_CHECK(glClearColor(color.x, color.y, color.z, color.w));
			_clearColor = color;
		}
	}

	if (flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
	{
		mode |= GL_DEPTH_BUFFER_BIT;

		if (_clearDepth != depth)
		{
			GL_CHECK(glClearDepthf(depth));
			_clearDepth = depth;
		}
	}

	if (flags & GraphicsClearFlags::GraphicsClearFlagsStencil)
	{
		mode |= GL_STENCIL_BUFFER_BIT;

		if (_clearStencil != stencil)
		{
			GL_CHECK(glClearStencil(stencil));
			_clearStencil = stencil;
		}
	}

	if (mode != 0)
	{
		auto depthWriteEnable = _stateCaptured.getDepthState().depthWriteEnable;
		if (!depthWriteEnable && flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
		{
			GL_CHECK(glDepthMask(GL_TRUE));
		}

		GL_CHECK(glClear(mode));

		if (!depthWriteEnable && flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
		{
			glDepthMask(GL_FALSE);
		}
	}

	EGL2Check::checkError();
}

void
EGL2DeviceContext::clearRenderTexture(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
	this->clearRenderTexture(flags, color, depth, stencil);
}

void
EGL2DeviceContext::discardRenderTexture() noexcept
{
	GL_PLATFORM_LOG("Can't support discardRenderTexture");
}

void
EGL2DeviceContext::readRenderTexture(GraphicsFramebufferPtr target, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(w && h && data);

	if (target)
	{
		GLenum format = EGL2Types::asTextureFormat(pfd);
		GLenum type = EGL2Types::asTextureType(pfd);

		if (format != GL_INVALID_ENUM && type != GL_INVALID_ENUM)
		{
			this->setRenderTexture(target);
			glReadPixels(0, 0, w, h, format, type, data);
		}
	}
}

void
EGL2DeviceContext::present() noexcept
{
	assert(_glcontext);
	_glcontext->present();
}

void
EGL2DeviceContext::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
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
EGL2DeviceContext::initDebugControl() noexcept
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
EGL2DeviceContext::initStateSystem() noexcept
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

	_maxViewports = 4;

	_clearColor.set(0.0, 0.0, 0.0, 0.0);
	_clearDepth = 1.0;
	_clearStencil = 0;

	_viewport.left = 0;
	_viewport.top = 0;
	_viewport.width = 0;
	_viewport.height = 0;

	_enableWireframe = false;

	_stateDefault = std::make_shared<EGL2GraphicsState>();
	_stateCaptured = GraphicsStateDesc();
}

void
EGL2DeviceContext::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2DeviceContext::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END