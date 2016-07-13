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
#include "ogl_device_context.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_texture.h"
#include "ogl_framebuffer.h"
#include "ogl_input_layout.h"
#include "ogl_sampler.h"
#include "ogl_descriptor_set.h"
#include "ogl_pipeline.h"
#include "ogl_swapchain.h"
#include "ogl_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(OGLDeviceContext, GraphicsContext, "OGLDeviceContext")

OGLDeviceContext::OGLDeviceContext() noexcept
	: _clearColor(0.0f, 0.0f, 0.0f, 0.0f)
	, _clearDepth(1.0f)
	, _clearStencil(0)
	, _globalVao(GL_NONE)
	, _framebuffer(nullptr)
	, _program(nullptr)
	, _pipeline(nullptr)
	, _descriptorSet(nullptr)
	, _indexType(GL_UNSIGNED_INT)
	, _state(nullptr)
	, _glcontext(nullptr)
	, _needUpdatePipeline(false)
	, _needUpdateDescriptor(false)
	, _needUpdateVertexBuffers(false)
	, _needEnableDebugControl(false)
	, _needDisableDebugControl(false)
{
}

OGLDeviceContext::~OGLDeviceContext() noexcept
{
	this->close();
}

bool
OGLDeviceContext::setup(const GraphicsContextDesc& desc) noexcept
{
	assert(desc.getSwapchain());
	assert(desc.getSwapchain()->isInstanceOf<OGLSwapchain>());

	_glcontext = desc.getSwapchain()->downcast_pointer<OGLSwapchain>();
	_glcontext->setActive(true);

	if (!_glcontext->getActive())
		return false;

	if (!this->checkSupport())
		return false;

	if (GraphicsSystem::instance()->enableDebugControl())
		this->startDebugControl();

	if (!this->initStateSystem())
		return false;

	return true;
}

void
OGLDeviceContext::close() noexcept
{
	_framebuffer = nullptr;
	_program = nullptr;
	_pipeline = nullptr;
	_descriptorSet = nullptr;
	_state = nullptr;
	_glcontext = nullptr;
	_indexBuffer.reset();
	_vertexBuffers.clear();

	if (_globalVao)
	{
		glDeleteVertexArrays(1, &_globalVao);
		_globalVao = GL_NONE;
	}
}

void
OGLDeviceContext::renderBegin() noexcept
{
	assert(_glcontext);
	_glcontext->setActive(true);

	if (_needEnableDebugControl)
	{
		this->startDebugControl();
		_needEnableDebugControl = false;
	}

	if (_needDisableDebugControl)
	{
		this->stopDebugControl();
		_needDisableDebugControl = false;
	}
}

void
OGLDeviceContext::renderEnd() noexcept
{
}

void
OGLDeviceContext::setViewport(std::uint32_t i, const Viewport& view) noexcept
{
	assert(_glcontext->getActive());

	if (_viewports[i] != view)
	{
		glViewportIndexedf(i, view.left, view.top, view.width, view.height);
		_viewports[i] = view;
	}
}

const Viewport&
OGLDeviceContext::getViewport(std::uint32_t i) const noexcept
{
	return _viewports[i];
}

void
OGLDeviceContext::setScissor(std::uint32_t i, const Scissor& scissor) noexcept
{
	assert(_glcontext->getActive());

	if (_scissors[i] != scissor)
	{
		glScissorIndexed(i, scissor.left, scissor.top, scissor.width, scissor.height);
		_scissors[i] = scissor;
	}
}

const Scissor&
OGLDeviceContext::getScissor(std::uint32_t i) const noexcept
{
	return _scissors[i];
}
void
OGLDeviceContext::setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
{
	if (face & GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
	{
		if (_stateCaptured.getStencilFrontReadMask() != mask)
		{
			GLenum frontfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilFrontFunc());
			glStencilFuncSeparate(GL_FRONT, frontfunc, _stateCaptured.getStencilFrontRef(), mask);
			_stateCaptured.setStencilFrontReadMask(mask);
		}
	}
	if (face & GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit)
	{
		if (_stateCaptured.getStencilBackReadMask() != mask)
		{
			GLenum backfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilBackFunc());
			glStencilFuncSeparate(GL_BACK, backfunc, _stateCaptured.getStencilBackRef(), mask);
			_stateCaptured.setStencilBackReadMask(mask);
		}
	}
}

std::uint32_t
OGLDeviceContext::getStencilCompareMask(GraphicsStencilFaceFlagBits face) noexcept
{
	assert(face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit || face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit);

	if (face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
		return _stateCaptured.getStencilFrontReadMask();
	else
		return _stateCaptured.getStencilBackReadMask();
}

void
OGLDeviceContext::setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept
{
	if (face & GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
	{
		if (_stateCaptured.getStencilFrontRef() != reference)
		{
			GLenum frontfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilFrontFunc());
			glStencilFuncSeparate(GL_FRONT, frontfunc, reference, _stateCaptured.getStencilFrontReadMask());
			_stateCaptured.setStencilFrontRef(reference);
		}
	}
	if (face & GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit)
	{
		if (_stateCaptured.getStencilBackRef() != reference)
		{
			GLenum backfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilBackFunc());
			glStencilFuncSeparate(GL_BACK, backfunc, reference, _stateCaptured.getStencilBackReadMask());
			_stateCaptured.setStencilBackRef(reference);
		}
	}
}

std::uint32_t
OGLDeviceContext::getStencilReference(GraphicsStencilFaceFlagBits face) noexcept
{
	assert(face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit || face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit);

	if (face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
		return _stateCaptured.getStencilFrontRef();
	else
		return _stateCaptured.getStencilBackRef();
}

void
OGLDeviceContext::setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
{
	if (face & GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
	{
		if (_stateCaptured.getStencilFrontWriteMask() != mask)
		{
			glStencilMaskSeparate(GL_FRONT, mask);
			_stateCaptured.setStencilFrontWriteMask(mask);
		}
	}
	if (face & GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit)
	{
		if (_stateCaptured.getStencilBackWriteMask() != mask)
		{
			glStencilMaskSeparate(GL_BACK, mask);
			_stateCaptured.setStencilBackWriteMask(mask);
		}
	}
}

std::uint32_t
OGLDeviceContext::getStencilWriteMask(GraphicsStencilFaceFlagBits face) noexcept
{
	assert(face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit || face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit);

	if (face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
		return _stateCaptured.getStencilFrontWriteMask();
	else
		return _stateCaptured.getStencilBackWriteMask();
}

void
OGLDeviceContext::setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept
{
	assert(pipeline);
	assert(pipeline->isInstanceOf<OGLPipeline>());
	assert(_glcontext->getActive());

	auto glpipeline = pipeline->downcast_pointer<OGLPipeline>();
	if (_pipeline != glpipeline)
	{
		auto& pipelineDesc = pipeline->getGraphicsPipelineDesc();

		auto glstate = pipelineDesc.getGraphicsState()->downcast_pointer<OGLGraphicsState>();
		if (_state != glstate)
		{
			glstate->apply(_stateCaptured);
			_state = glstate;
		}

		auto glprogram = pipelineDesc.getGraphicsProgram()->downcast_pointer<OGLProgram>();
		if (_program != glprogram)
		{
			_program = glprogram;
			_program->apply();
		}

		if (_pipeline != glpipeline)
		{
			_pipeline = glpipeline;
			_pipeline->apply();
			_needUpdatePipeline = true;
		}
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
	assert(descriptorSet);
	assert(descriptorSet->isInstanceOf<OGLDescriptorSet>());
	assert(_glcontext->getActive());

	_descriptorSet = descriptorSet->downcast_pointer<OGLDescriptorSet>();
	_needUpdateDescriptor = true;
}

GraphicsDescriptorSetPtr
OGLDeviceContext::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

void
OGLDeviceContext::setVertexBufferData(std::uint32_t i, GraphicsDataPtr data, std::intptr_t offset) noexcept
{
	assert(data);
	assert(data->isInstanceOf<OGLGraphicsData>());
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
	assert(_vertexBuffers.size() > i);
	assert(_glcontext->getActive());

	auto vbo = data->downcast_pointer<OGLGraphicsData>();
	if (_vertexBuffers[i].vbo != vbo || _vertexBuffers[i].offset != offset)
	{
		_vertexBuffers[i].vbo = vbo;
		_vertexBuffers[i].offset = offset;
		_vertexBuffers[i].needUpdate = true;
		_needUpdateVertexBuffers = true;
	}
}

GraphicsDataPtr
OGLDeviceContext::getVertexBufferData(std::uint32_t i) const noexcept
{
	assert(_vertexBuffers.size() > i);
	return _vertexBuffers[i].vbo;
}

void
OGLDeviceContext::setIndexBufferData(GraphicsDataPtr data, GraphicsIndexType indexType) noexcept
{
	assert(data);
	assert(data->isInstanceOf<OGLGraphicsData>());
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
	assert(indexType == GraphicsIndexType::GraphicsIndexTypeUInt16 || indexType == GraphicsIndexType::GraphicsIndexTypeUInt32);
	assert(_glcontext->getActive());

	auto ibo = data->downcast_pointer<OGLGraphicsData>();
	if (_indexBuffer != ibo)
	{
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
		_indexBuffer = ibo;
	}

	_indexType = OGLTypes::asIndexType(indexType);
}

GraphicsDataPtr
OGLDeviceContext::getIndexBufferData() const noexcept
{
	return _indexBuffer;
}

void 
OGLDeviceContext::generateMipmap(GraphicsTexturePtr texture) noexcept
{
	assert(texture);
	assert(texture->isInstanceOf<OGLTexture>());

	auto gltexture = texture->downcast<OGLTexture>();
	auto textureID = gltexture->getInstanceID();
	auto textureTarget = gltexture->getTarget();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(textureTarget, textureID);
	glGenerateMipmap(textureTarget);
}

void
OGLDeviceContext::setFramebuffer(GraphicsFramebufferPtr target) noexcept
{
	assert(_glcontext->getActive());

	if (target)
	{
		auto framebuffer = target->downcast_pointer<OGLFramebuffer>();
		if (_framebuffer != framebuffer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getInstanceID());

			auto& framebufferDesc = framebuffer->getGraphicsFramebufferDesc();
			auto& colorAttachment = framebufferDesc.getColorAttachments();

			std::size_t viewportCount = std::max<std::size_t>(1, colorAttachment.size());
			for (std::size_t i = 0; i < viewportCount; i++)
			{
				this->setViewport(i, Viewport(0, 0, framebufferDesc.getWidth(), framebufferDesc.getHeight()));
			}

			_framebuffer = framebuffer;
		}
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		_framebuffer = nullptr;
	}
}

void
OGLDeviceContext::setFramebufferClear(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
}

void
OGLDeviceContext::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_glcontext->getActive());

	GLint buffer = 0;
	if (_framebuffer)
	{
		const auto& layoutDesc = _framebuffer->getGraphicsFramebufferDesc().getGraphicsFramebufferLayout()->getGraphicsFramebufferLayoutDesc();
		if (layoutDesc.getComponents().size() <= i)
			return;

		auto type = layoutDesc.getComponents().at(i).getAttachType();
		if (type == GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal)
		{
			if (!(flags & GraphicsClearFlagBits::GraphicsClearFlagColorBit))
				return;

			flags = GraphicsClearFlagBits::GraphicsClearFlagColorBit;	
			buffer = i;
		}
		else if (type == GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal || 
				 type == GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal)
		{
			if (!(flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit) &&
				!(flags & GraphicsClearFlagBits::GraphicsClearFlagStencilBit))
			{
				return;
			}
		}
	}

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
	{
		auto depthWriteEnable = _stateCaptured.getDepthWriteEnable();
		if (!depthWriteEnable)
		{
			glDepthMask(GL_TRUE);
		}

		if (flags & GraphicsClearFlagBits::GraphicsClearFlagStencilBit)
		{
			GLint s = stencil;
			GLfloat f = depth;
			glClearBufferfi(GL_DEPTH_STENCIL, buffer, f, s);
		}
		else
		{
			GLfloat f = depth;
			glClearBufferfv(GL_DEPTH, buffer, &f);
		}

		if (!depthWriteEnable)
		{
			glDepthMask(GL_FALSE);
		}
	}
	else if (flags & GraphicsClearFlagBits::GraphicsClearFlagStencilBit)
	{
		GLint s = stencil;
		glClearBufferiv(GL_STENCIL, buffer, &s);
	}
	else
	{
		auto colorWriteFlags = _stateCaptured.getColorBlends()[buffer].getColorWriteMask();
		if (colorWriteFlags != GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRGBABit)
		{
			glColorMaski(buffer, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}

		glClearBufferfv(GL_COLOR, buffer, color.ptr());

		if (colorWriteFlags != GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRGBABit)
		{
			GLboolean r = colorWriteFlags & GraphicsColorMaskFlagBits::GraphicsColorMaskFlagRedBit ? GL_TRUE : GL_FALSE;
			GLboolean g = colorWriteFlags & GraphicsColorMaskFlagBits::GraphicsColorMaskFlagGreendBit ? GL_TRUE : GL_FALSE;
			GLboolean b = colorWriteFlags & GraphicsColorMaskFlagBits::GraphicsColorMaskFlagBlurBit ? GL_TRUE : GL_FALSE;
			GLboolean a = colorWriteFlags & GraphicsColorMaskFlagBits::GraphicsColorMaskFlagAlphaBit ? GL_TRUE : GL_FALSE;
			glColorMaski(buffer, r, g, b, a);
		}
	}
}

void
OGLDeviceContext::blitFramebuffer(GraphicsFramebufferPtr src, const Viewport& v1, GraphicsFramebufferPtr dest, const Viewport& v2) noexcept
{
	assert(src);
	assert(src->isInstanceOf<OGLFramebuffer>());
	assert(!dest || (dest && dest->isInstanceOf<OGLFramebuffer>()));
	assert(_glcontext->getActive());

	auto readFramebuffer = src->downcast<OGLFramebuffer>()->getInstanceID();
	auto drawFramebuffer = dest ? dest->downcast<OGLFramebuffer>()->getInstanceID() : GL_NONE;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);

	glBlitFramebuffer(v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void
OGLDeviceContext::discardFramebuffer(std::uint32_t i) noexcept
{
	assert(_framebuffer);
	assert(_glcontext->getActive());

	const auto& layoutDesc = _framebuffer->getGraphicsFramebufferDesc().getGraphicsFramebufferLayout()->getGraphicsFramebufferLayoutDesc();
	if (layoutDesc.getComponents().size() > i)
	{
		auto& attachment = layoutDesc.getComponents().at(i);
		switch (attachment.getAttachType())
		{
		case GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal:
		{
			GLenum attachments = GL_COLOR_ATTACHMENT0 + i;
			glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);
		}
		break;
		case GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal:
		case GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal:
		{
			auto format = attachment.getAttachFormat();
			if (format == GraphicsFormat::GraphicsFormatS8UInt)
			{
				GLenum attachments = GL_STENCIL_ATTACHMENT;
				glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);
			}
			else if (format == GraphicsFormat::GraphicsFormatD16UNorm || format == GraphicsFormat::GraphicsFormatX8_D24UNormPack32 || format == GraphicsFormat::GraphicsFormatD32_SFLOAT)
			{
				GLenum attachments = GL_DEPTH_ATTACHMENT;
				glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);
			}
			else
			{
				GLenum attachments = GL_DEPTH_STENCIL_ATTACHMENT;
				glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);
			}
		}
		break;
		default:
			break;
		}
	}
}

GraphicsFramebufferPtr
OGLDeviceContext::getFramebuffer() const noexcept
{
	return _framebuffer;
}

void
OGLDeviceContext::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
{
	assert(_pipeline);
	assert(_glcontext->getActive());
	assert(startInstances == 0);

	if (_needUpdatePipeline || _needUpdateVertexBuffers)
	{
		_pipeline->bindVertexBuffers(_vertexBuffers, _needUpdatePipeline);
		_needUpdatePipeline = false;
		_needUpdateVertexBuffers = false;
	}

	if (_needUpdateDescriptor)
	{
		_descriptorSet->apply(*_program);
		_needUpdateDescriptor = false;
	}

	if (numVertices > 0)
	{
		GLenum drawType = OGLTypes::asVertexType(_stateCaptured.getPrimitiveType());
		glDrawArraysInstanced(drawType, startVertice, numVertices, numInstances);
	}
}

void
OGLDeviceContext::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
{
	assert(_pipeline);
	assert(_glcontext->getActive());
	assert(_indexBuffer);
	assert(_indexType == GL_UNSIGNED_INT || _indexType == GL_UNSIGNED_SHORT);
	assert(startInstances == 0);

	if (_needUpdatePipeline || _needUpdateVertexBuffers)
	{
		_pipeline->bindVertexBuffers(_vertexBuffers, _needUpdatePipeline);
		_needUpdatePipeline = false;
		_needUpdateVertexBuffers = false;
	}

	if (_needUpdateDescriptor)
	{
		_descriptorSet->apply(*_program);
		_needUpdateDescriptor = false;
	}
	
	if (numIndices > 0)
	{
		GLenum drawType = OGLTypes::asVertexType(_stateCaptured.getPrimitiveType());
		glDrawElementsInstancedBaseVertex(drawType, numIndices, _indexType, (GLbyte*)nullptr + startIndice, numInstances, startVertice);
	}
}

void
OGLDeviceContext::present() noexcept
{
	assert(_glcontext->getActive());
	_glcontext->present();
}

bool
OGLDeviceContext::checkSupport() noexcept
{
	if (!GLEW_ARB_draw_elements_base_vertex)
	{
		GL_PLATFORM_LOG("Can't support GL_ARB_draw_elements_base_vertex.");
		return false;
	}

	if (!GLEW_ARB_uniform_buffer_object)
	{
		GL_PLATFORM_LOG("Can't support GL_ARB_uniform_buffer_object.");
		return false;
	}

	if (!GLEW_ARB_texture_buffer_object)
	{
		GL_PLATFORM_LOG("Can't support GL_ARB_texture_buffer_object.");
		return false;
	}

	if (!GLEW_ARB_sampler_objects)
	{
		GL_PLATFORM_LOG("Can't support GL_ARB_sampler_objects.");
		return false;
	}

	if (!GLEW_ARB_framebuffer_object)
	{
		GL_PLATFORM_LOG("Can't support GL_ARB_framebuffer_object.");
		return false;
	}
	
	if (!GLEW_EXT_texture_filter_anisotropic)
	{
		GL_PLATFORM_LOG("Can't support GL_EXT_texture_filter_anisotropic.");
		return false;
	}

	if (!GLEW_ARB_separate_shader_objects)
	{
		GL_PLATFORM_LOG("Can't support GL_ARB_separate_shader_objects.");
		return false;
	}

	return true;
}

void
OGLDeviceContext::enableDebugControl(bool enable) noexcept
{
	if (enable)
	{
		_needEnableDebugControl = true;
		_needDisableDebugControl = false;
	}
	else
	{
		_needEnableDebugControl = false;
		_needDisableDebugControl = true;
	}
}

void
OGLDeviceContext::startDebugControl() noexcept
{
	if (GLEW_KHR_debug)
	{
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
	}
}

void
OGLDeviceContext::stopDebugControl() noexcept
{
	if (GLEW_KHR_debug)
	{
		glDisable(GL_DEBUG_OUTPUT);
		glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
}

bool
OGLDeviceContext::initStateSystem() noexcept
{
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

	glGenVertexArrays(1, &_globalVao);
	glBindVertexArray(_globalVao);
	
	auto& deviceProperties = this->getDevice()->getGraphicsDeviceProperty().getGraphicsDeviceProperties();
	_vertexBuffers.resize(deviceProperties.maxVertexInputBindings);
	_viewports.resize(deviceProperties.maxViewports, Viewport(0, 0, 0, 0));
	_scissors.resize(deviceProperties.maxViewports, Scissor(0, 0, 0, 0));

	GraphicsColorBlends blends(deviceProperties.maxFramebufferColorAttachments);
	_stateCaptured.setColorBlends(blends);

	return true;
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