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
#include "ogl_core_device_context.h"
#include "ogl_core_texture.h"
#include "ogl_core_framebuffer.h"
#include "ogl_core_graphics_data.h"
#include "ogl_core_descriptor.h"
#include "ogl_core_pipeline.h"
#include "ogl_input_layout.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_sampler.h"
#include "ogl_swapchain.h"

_NAME_BEGIN

__ImplementSubClass(OGLCoreDeviceContext, GraphicsContext, "OGLCoreDeviceContext")

OGLCoreDeviceContext::OGLCoreDeviceContext() noexcept
	: _clearDepth(1.0f)
	, _clearStencil(0)
	, _inputLayout(GL_NONE)
	, _indexType(GL_UNSIGNED_INT)
	, _indexOffset(0)
	, _needUpdatePipeline(false)
	, _needUpdateDescriptor(false)
	, _needUpdateVertexBuffers(false)
	, _needEnableDebugControl(false)
	, _needDisableDebugControl(false)
{
}

OGLCoreDeviceContext::~OGLCoreDeviceContext() noexcept
{
	this->close();
}

bool
OGLCoreDeviceContext::setup(const GraphicsContextDesc& desc) noexcept
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
OGLCoreDeviceContext::close() noexcept
{
	_framebuffer.reset();
	_program.reset();
	_pipeline.reset();
	_descriptorSet.reset();
	_state.reset();
	_indexBuffer.reset();
	_glcontext.reset();
	_vertexBuffers.clear();

	if (_inputLayout != GL_NONE)
	{
		glDeleteVertexArrays(1, &_inputLayout);
		_inputLayout = GL_NONE;
	}
}

void
OGLCoreDeviceContext::renderBegin() noexcept
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
OGLCoreDeviceContext::renderEnd() noexcept
{
}

void
OGLCoreDeviceContext::setViewport(std::uint32_t i, const Viewport& view) noexcept
{
	assert(_glcontext->getActive());

	if (_viewports[i] != view)
	{
		glViewportIndexedf(i, view.left, view.top, view.width, view.height);
		_viewports[i] = view;
	}
}

const Viewport&
OGLCoreDeviceContext::getViewport(std::uint32_t i) const noexcept
{
	return _viewports[i];
}

void
OGLCoreDeviceContext::setScissor(std::uint32_t i, const Scissor& scissor) noexcept
{
	assert(_glcontext->getActive());

	if (_scissors[i] != scissor)
	{
		std::uint32_t height;
		if (_framebuffer)
			height = _framebuffer->getGraphicsFramebufferDesc().getHeight();
		else
			height = _glcontext->getGraphicsSwapchainDesc().getHeight();

		glScissorIndexed(i, scissor.left, height - scissor.height - scissor.top, scissor.width, scissor.height);
		_scissors[i] = scissor;
	}
}

const Scissor&
OGLCoreDeviceContext::getScissor(std::uint32_t i) const noexcept
{
	return _scissors[i];
}

void
OGLCoreDeviceContext::setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
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
OGLCoreDeviceContext::getStencilCompareMask(GraphicsStencilFaceFlagBits face) noexcept
{
	assert(face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit || face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit);

	if (face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
		return _stateCaptured.getStencilFrontReadMask();
	else
		return _stateCaptured.getStencilBackReadMask();
}

void
OGLCoreDeviceContext::setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept
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
OGLCoreDeviceContext::getStencilReference(GraphicsStencilFaceFlagBits face) noexcept
{
	assert(face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit || face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit);

	if (face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
		return _stateCaptured.getStencilFrontRef();
	else
		return _stateCaptured.getStencilBackRef();
}

void
OGLCoreDeviceContext::setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
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
OGLCoreDeviceContext::getStencilWriteMask(GraphicsStencilFaceFlagBits face) noexcept
{
	assert(face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit || face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceBackBit);

	if (face == GraphicsStencilFaceFlagBits::GraphicsStencilFaceFrontBit)
		return _stateCaptured.getStencilFrontWriteMask();
	else
		return _stateCaptured.getStencilBackWriteMask();
}

void
OGLCoreDeviceContext::setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept
{
	assert(pipeline);
	assert(pipeline->isInstanceOf<OGLCorePipeline>());
	assert(_glcontext->getActive());

	auto glpipeline = pipeline->downcast_pointer<OGLCorePipeline>();
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
OGLCoreDeviceContext::getRenderPipeline() const noexcept
{
	return _pipeline;
}

void
OGLCoreDeviceContext::setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept
{
	assert(descriptorSet);
	assert(descriptorSet->isInstanceOf<OGLCoreDescriptorSet>());
	assert(_glcontext->getActive());

	_descriptorSet = descriptorSet->downcast_pointer<OGLCoreDescriptorSet>();
	_needUpdateDescriptor = true;
}

GraphicsDescriptorSetPtr
OGLCoreDeviceContext::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

void
OGLCoreDeviceContext::setVertexBufferData(std::uint32_t i, GraphicsDataPtr data, std::intptr_t offset) noexcept
{
	assert(data);
	assert(data->isInstanceOf<OGLCoreGraphicsData>());
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
	assert(_vertexBuffers.size() > i);
	assert(_glcontext->getActive());

	auto vbo = data->downcast_pointer<OGLCoreGraphicsData>();
	if (_vertexBuffers[i].vbo != vbo || _vertexBuffers[i].offset != offset)
	{
		_vertexBuffers[i].vbo = vbo;
		_vertexBuffers[i].offset = offset;
		_vertexBuffers[i].needUpdate = true;
		_needUpdateVertexBuffers = true;
	}
}

GraphicsDataPtr
OGLCoreDeviceContext::getVertexBufferData(std::uint32_t i) const noexcept
{
	assert(_vertexBuffers.size() > i);
	return _vertexBuffers[i].vbo;
}

void
OGLCoreDeviceContext::setIndexBufferData(GraphicsDataPtr data, std::intptr_t offset, GraphicsIndexType indexType) noexcept
{
	assert(data);
	assert(data->isInstanceOf<OGLCoreGraphicsData>());
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
	assert(indexType == GraphicsIndexType::GraphicsIndexTypeUInt16 || indexType == GraphicsIndexType::GraphicsIndexTypeUInt32);
	assert(_glcontext->getActive());

	auto ibo = data->downcast_pointer<OGLCoreGraphicsData>();
	if (_indexBuffer != ibo)
	{
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
		_indexBuffer = ibo;
	}

	_indexType = OGLTypes::asIndexType(indexType);
	_indexOffset = offset;
}

GraphicsDataPtr
OGLCoreDeviceContext::getIndexBufferData() const noexcept
{
	return _indexBuffer;
}

void
OGLCoreDeviceContext::generateMipmap(GraphicsTexturePtr texture) noexcept
{
	assert(texture);
	assert(texture->isInstanceOf<OGLCoreTexture>());

	auto gltexture = texture->downcast<OGLCoreTexture>();
	auto textureID = gltexture->getInstanceID();
	glGenerateTextureMipmap(textureID);
}

void
OGLCoreDeviceContext::setFramebuffer(GraphicsFramebufferPtr target) noexcept
{
	assert(_glcontext->getActive());

	if (target)
	{
		auto framebuffer = target->downcast_pointer<OGLCoreFramebuffer>();
		if (_framebuffer != framebuffer)
		{
			_framebuffer = framebuffer;
			glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer->getInstanceID());

			auto& framebufferDesc = _framebuffer->getGraphicsFramebufferDesc();
			auto& colorAttachment = framebufferDesc.getColorAttachments();

			std::size_t viewportCount = std::max<std::size_t>(1, colorAttachment.size());
			for (std::size_t i = 0; i < viewportCount; i++)
			{
				this->setViewport(i, Viewport(0, 0, framebufferDesc.getWidth(), framebufferDesc.getHeight()));

				glScissorIndexed(i, _scissors[i].left, framebufferDesc.getHeight() - _scissors[i].height - _scissors[i].top, _scissors[i].width, _scissors[i].height);
			}
		}
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		
		if (_state)
		{
			_state->apply(_stateDefault);
			_state = nullptr;
		}

		_framebuffer = nullptr;
	}
}

void
OGLCoreDeviceContext::setFramebufferClear(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
}

void
OGLCoreDeviceContext::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
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

	if (_stateCaptured.getScissorTestEnable())
	{
		if (_framebuffer)
			glScissor(0, 0, _framebuffer->getGraphicsFramebufferDesc().getWidth(), _framebuffer->getGraphicsFramebufferDesc().getHeight());
		else
			glScissor(0, 0, _glcontext->getGraphicsSwapchainDesc().getWidth(), _glcontext->getGraphicsSwapchainDesc().getHeight());
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

	if (_stateCaptured.getScissorTestEnable())
	{
		if (_framebuffer)
		{
			std::size_t viewportCount = std::max<std::size_t>(1, _framebuffer->getGraphicsFramebufferDesc().getColorAttachments().size());
			for (std::size_t j = 0; j < viewportCount; j++)
			{
				glScissorIndexed(j, _scissors[j].left, _scissors[j].top, _scissors[j].width, _scissors[j].height);
			}
		}
		else
		{
			glScissorIndexed(0, _scissors[0].left, _scissors[0].top, _scissors[0].width, _scissors[0].height);
		}
	}
}

void
OGLCoreDeviceContext::blitFramebuffer(GraphicsFramebufferPtr src, const Viewport& v1, GraphicsFramebufferPtr dest, const Viewport& v2) noexcept
{
	assert(src);
	assert(src->isInstanceOf<OGLCoreFramebuffer>());
	assert(!dest || (dest && dest->isInstanceOf<OGLCoreFramebuffer>()));
	assert(_glcontext->getActive());

	auto readFramebuffer = src->downcast_pointer<OGLCoreFramebuffer>()->getInstanceID();
	auto drawFramebuffer = dest ? dest->downcast_pointer<OGLCoreFramebuffer>()->getInstanceID() : GL_NONE;

	glBlitNamedFramebuffer(readFramebuffer, drawFramebuffer, v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void
OGLCoreDeviceContext::discardFramebuffer(std::uint32_t i) noexcept
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
OGLCoreDeviceContext::getFramebuffer() const noexcept
{
	return _framebuffer;
}

void
OGLCoreDeviceContext::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
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
		glDrawArraysInstancedBaseInstance(drawType, startVertice, numVertices, numInstances, startInstances);
	}
}

void
OGLCoreDeviceContext::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
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
		GLbyte* offsetIndices = nullptr;
		if (_indexType == GL_UNSIGNED_INT)
			offsetIndices += _indexOffset + sizeof(std::uint32_t) * startIndice;
		else
			offsetIndices += _indexOffset + sizeof(std::uint16_t) * startIndice;

		GLenum drawType = OGLTypes::asVertexType(_stateCaptured.getPrimitiveType());
		glDrawElementsInstancedBaseVertexBaseInstance(drawType, numIndices, _indexType, offsetIndices, numInstances, startVertice, startInstances);
	}
}

void
OGLCoreDeviceContext::present() noexcept
{
	assert(_glcontext->getActive());
	_glcontext->present();
}

bool
OGLCoreDeviceContext::checkSupport() noexcept
{
	if (!GLEW_ARB_vertex_array_object)
	{
		GL_PLATFORM_LOG("Can't support vertex array object");
		return false;
	}

	if (!GL_ARB_vertex_attrib_binding)
	{
		GL_PLATFORM_LOG("Can't support vertex attrib binding");
		return false;
	}

	if (!GLEW_ARB_sampler_objects)
	{
		GL_PLATFORM_LOG("Can't support sampler object");
		return false;
	}

	if (!GLEW_ARB_framebuffer_object)
	{
		GL_PLATFORM_LOG("Can't support framebuffer object");
		return false;
	}

	if (!GLEW_ARB_uniform_buffer_object)
	{
		GL_PLATFORM_LOG("Can't support uniform buffer object");
		return false;
	}

	if (!GLEW_ARB_texture_buffer_object)
	{
		GL_PLATFORM_LOG("Can't support texture buffer object");
		return false;
	}

	if (!GL_ARB_buffer_storage)
	{
		GL_PLATFORM_LOG("Can't support buffer storage");
		return false;
	}

	if (!GLEW_ARB_texture_storage)
	{
		GL_PLATFORM_LOG("Can't support texture storage");
		return false;
	}

	if (!GLEW_EXT_texture_filter_anisotropic)
	{
		GL_PLATFORM_LOG("Can't support anisotropic sampler");
		return false;
	}

	if (!GLEW_ARB_separate_shader_objects)
	{
		GL_PLATFORM_LOG("Can't support separate shader objects");
		return false;
	}

	if (!GLEW_ARB_direct_state_access)
	{
		GL_PLATFORM_LOG("Can't support direct state access");
		return false;
	}

	return true;
}

void
OGLCoreDeviceContext::enableDebugControl(bool enable) noexcept
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
OGLCoreDeviceContext::startDebugControl() noexcept
{
	if (!GLEW_KHR_debug)
	{
		GL_PLATFORM_LOG("Can't support GL_KHR_debug.");
		return;
	}

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

void
OGLCoreDeviceContext::stopDebugControl() noexcept
{
	if (GLEW_KHR_debug)
	{
		glDisable(GL_DEBUG_OUTPUT);

		glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
}

bool
OGLCoreDeviceContext::initStateSystem() noexcept
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

	glGenVertexArrays(1, &_inputLayout);
	glBindVertexArray(_inputLayout);

	if (GLEW_NV_vertex_buffer_unified_memory)
	{
		glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
	}

	auto& deviceProperties = this->getDevice()->getGraphicsDeviceProperty().getGraphicsDeviceProperties();
	_vertexBuffers.resize(deviceProperties.maxVertexInputBindings);
	_viewports.resize(deviceProperties.maxViewports, Viewport(0, 0, 0, 0));
	_scissors.resize(deviceProperties.maxViewports, Scissor(0, 0, 0, 0));
	_clearColor.resize(deviceProperties.maxFramebufferColorAttachments, float4(0.0f, 0.0f, 0.0f, 0.0f));

	GraphicsColorBlends blends(deviceProperties.maxFramebufferColorAttachments);
	_stateCaptured.setColorBlends(blends);

	return true;
}

void
OGLCoreDeviceContext::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
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
OGLCoreDeviceContext::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLCoreDeviceContext::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END