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
#include "egl3_pipeline.h"
#include "egl3_swapchain.h"
#include "egl3_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(EGL3DeviceContext, GraphicsContext, "EGL3DeviceContext")

EGL3DeviceContext::EGL3DeviceContext() noexcept
	: _clearColor(0.0, 0.0, 0.0, 0.0)
	, _clearDepth(1.0)
	, _clearStencil(0)
	, _viewport(0, 0, 0, 0)
	, _startVertices(0)
	, _needUpdateLayout(true)
	, _needUpdateState(true)
{
}

EGL3DeviceContext::~EGL3DeviceContext() noexcept
{
	this->close();
}

bool
EGL3DeviceContext::setup(const GraphicsContextDesc& desc) noexcept
{
	assert(desc.getSwapchain());
	assert(desc.getSwapchain()->isInstanceOf<EGL3Swapchain>());

	_glcontext = desc.getSwapchain()->downcast<EGL3Swapchain>();
	_glcontext->setActive(true);

	if (_glcontext->getActive())
	{
		if (!this->checkSupport())
			return false;

		if (!this->initDebugControl(desc))
			return false;

		if (!this->initStateSystem())
			return false;

		if (!this->initTextureSupports())
			return false;

		if (!this->initTextureDimSupports())
			return false;

		if (!this->initVertexSupports())
			return false;

		if (!this->initShaderSupports())
			return false;

		return true;
	}

	return false;
}

void
EGL3DeviceContext::close() noexcept
{
	_framebuffer = nullptr;
	_shaderObject = nullptr;
	_pipeline = nullptr;
	_descriptorSet = nullptr;
	_state = nullptr;
	_vbo = nullptr;
	_ibo = nullptr;
	_inputLayout = nullptr;
	_glcontext = nullptr;
	_supportTextures.clear();
	_supportAttribute.clear();
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
EGL3DeviceContext::setViewport(std::uint32_t i, const Viewport& view) noexcept
{
	assert(_glcontext->getActive());

	if (_viewport != view)
	{
		GL_CHECK(glViewport(view.left, view.top, view.width, view.height));
		_viewport = view;
	}
}

const Viewport&
EGL3DeviceContext::getViewport(std::uint32_t i) const noexcept
{
	return _viewport;
}

void
EGL3DeviceContext::setScissor(std::uint32_t i, const Scissor& scissor) noexcept
{
	assert(_glcontext->getActive());

	if (_scissor != scissor)
	{
		GL_CHECK(glScissor(scissor.left, scissor.top, scissor.width, scissor.height));
		_scissor = scissor;
	}
}

const Scissor&
EGL3DeviceContext::getScissor(std::uint32_t i) const noexcept
{
	return _scissor;
}

void
EGL3DeviceContext::setStencilCompareMask(GraphicsStencilFace face, std::uint32_t mask) noexcept
{
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
	{
		if (_stateCaptured.getStencilFrontReadMask() != mask)
		{
			GLenum frontfunc = EGL3Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
			glStencilFuncSeparate(GL_FRONT, frontfunc, _stateCaptured.getStencilFrontRef(), mask);
			_stateCaptured.setStencilFrontReadMask(mask);
			_needUpdateState = true;
		}
	}
	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
	{
		if (_stateCaptured.getStencilBackReadMask() != mask)
		{
			GLenum backfunc = EGL3Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
			glStencilFuncSeparate(GL_BACK, backfunc, _stateCaptured.getStencilFrontRef(), mask);
			_stateCaptured.setStencilBackReadMask(mask);
			_needUpdateState = true;
		}
	}

}

std::uint32_t
EGL3DeviceContext::getStencilCompareMask(GraphicsStencilFace face) noexcept
{
	if (face == GraphicsStencilFace::GraphicsStencilFaceFront)
		return _stateCaptured.getStencilFrontReadMask();
	if (face == GraphicsStencilFace::GraphicsStencilFaceBack)
		return _stateCaptured.getStencilBackReadMask();
	return 0;
}

void
EGL3DeviceContext::setStencilReference(GraphicsStencilFace face, std::uint32_t reference) noexcept
{
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
	{
		if (_stateCaptured.getStencilFrontRef() != reference)
		{
			GLenum frontfunc = EGL3Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
			glStencilFuncSeparate(GL_FRONT, frontfunc, reference, _stateCaptured.getStencilFrontReadMask());
			_stateCaptured.setStencilFrontRef(reference);
			_needUpdateState = true;
		}
	}
	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
	{
		if (_stateCaptured.getStencilBackRef() != reference)
		{
			GLenum backfunc = EGL3Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
			glStencilFuncSeparate(GL_BACK, backfunc, reference, _stateCaptured.getStencilFrontReadMask());
			_stateCaptured.setStencilBackRef(reference);
			_needUpdateState = true;
		}
	}
}

std::uint32_t
EGL3DeviceContext::getStencilReference(GraphicsStencilFace face) noexcept
{
	if (face == GraphicsStencilFace::GraphicsStencilFaceFront)
		return _stateCaptured.getStencilFrontRef();
	if (face == GraphicsStencilFace::GraphicsStencilFaceBack)
		return _stateCaptured.getStencilBackRef();
	return 0;
}

void
EGL3DeviceContext::setStencilFrontWriteMask(GraphicsStencilFace face, std::uint32_t mask) noexcept
{
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
	{
		if (_stateCaptured.getStencilFrontWriteMask() != mask)
		{
			glStencilMaskSeparate(GL_FRONT, mask);
			_stateCaptured.setStencilFrontWriteMask(mask);
			_needUpdateState = true;
		}
	}
	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
	{
		if (_stateCaptured.getStencilBackWriteMask() != mask)
		{
			glStencilMaskSeparate(GL_BACK, mask);
			_stateCaptured.setStencilBackWriteMask(mask);
			_needUpdateState = true;
		}
	}
}

std::uint32_t
EGL3DeviceContext::getStencilFrontWriteMask(GraphicsStencilFace face) noexcept
{
	if (face == GraphicsStencilFace::GraphicsStencilFaceFront)
		return _stateCaptured.getStencilFrontWriteMask();
	if (face == GraphicsStencilFace::GraphicsStencilFaceBack)
		return _stateCaptured.getStencilBackWriteMask();
	return 0;
}

void
EGL3DeviceContext::setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept
{
	assert(pipeline);
	assert(pipeline->isInstanceOf<EGL3Pipeline>());
	assert(_glcontext->getActive());

	auto glpipeline = pipeline->downcast<EGL3Pipeline>();
	if (_pipeline != glpipeline)
	{
		auto& pipelineDesc = pipeline->getGraphicsPipelineDesc();

		auto glstate = pipelineDesc.getGraphicsState()->downcast<EGL3GraphicsState>();
		if (_state != glstate || _needUpdateState)
		{
			glstate->apply(_stateCaptured);

			_state = glstate;
			_needUpdateState = false;
		}

		auto glshader = pipelineDesc.getGraphicsProgram()->downcast<EGL3Program>();
		if (_shaderObject != glshader)
		{
			_shaderObject = glshader;
			_shaderObject->apply();
		}

		_pipeline = glpipeline;
		_pipeline->apply();

		_needUpdateLayout = true;
	}
}

GraphicsPipelinePtr
EGL3DeviceContext::getRenderPipeline() const noexcept
{
	if (_pipeline)
		return _pipeline->upcast_pointer<GraphicsPipeline>();
	return nullptr;
}

void
EGL3DeviceContext::setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept
{
	assert(descriptorSet);
	assert(descriptorSet->isInstanceOf<EGL3DescriptorSet>());
	assert(_glcontext->getActive());
	assert(_shaderObject);

	_descriptorSet = descriptorSet->downcast<EGL3DescriptorSet>();
	_descriptorSet->apply(*_shaderObject);
}

GraphicsDescriptorSetPtr
EGL3DeviceContext::getDescriptorSet() const noexcept
{
	if (_descriptorSet)
		return _descriptorSet->upcast_pointer<GraphicsDescriptorSet>();
	return nullptr;
}

void
EGL3DeviceContext::setVertexBufferData(GraphicsDataPtr data) noexcept
{
	assert(data);
	assert(data->isInstanceOf<EGL3GraphicsData>());
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
	assert(_glcontext->getActive());

	if (data)
	{
		auto vbo = data->downcast<EGL3GraphicsData>();
		if (_vbo != vbo)
		{
			_vbo = vbo;
			_needUpdateLayout = true;
		}
	}
	else
	{
		if (_vbo)
		{
			_vbo = nullptr;
			_needUpdateLayout = true;
		}
	}
}

GraphicsDataPtr
EGL3DeviceContext::getVertexBufferData() const noexcept
{
	if (_vbo)
		return _vbo->upcast_pointer<GraphicsData>();
	return nullptr;
}

void
EGL3DeviceContext::setIndexBufferData(GraphicsDataPtr data) noexcept
{
	assert(!data || data->isInstanceOf<EGL3GraphicsData>());
	assert(!data || data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
	assert(_glcontext->getActive());

	if (data)
	{
		auto ibo = data->downcast<EGL3GraphicsData>();
		if (_ibo != ibo)
		{
			_ibo = ibo;
			_needUpdateLayout = true;
		}
	}
	else
	{
		if (_ibo)
		{
			_ibo = nullptr;
			_needUpdateLayout = true;
		}
	}
}

GraphicsDataPtr
EGL3DeviceContext::getIndexBufferData() const noexcept
{
	if (_ibo)
		return _ibo->upcast_pointer<GraphicsData>();
	return nullptr;
}
void
EGL3DeviceContext::drawRenderMesh(const GraphicsIndirect& renderable) noexcept
{
	assert(_vbo);
	assert(_pipeline);
	assert(_glcontext->getActive());

	if (_needUpdateLayout || _startVertices != renderable.startVertice)
	{
		if (_vbo)
			_pipeline->bindVbo(*_vbo, renderable.startVertice);

		if (_ibo)
			_pipeline->bindIbo(*_ibo);

		_startVertices = renderable.startVertice;

		_needUpdateLayout = false;
	}

	if (_ibo)
	{
		GLenum drawType = EGL3Types::asVertexType(_stateCaptured.getPrimitiveType());
		GLenum indexType = EGL3Types::asIndexType(renderable.indexType);
		GLvoid* offsetIndices = (GLchar*)(nullptr) + (_ibo->getGraphicsDataDesc().getStride() * renderable.startIndice);
		GL_CHECK(glDrawElementsInstanced(drawType, renderable.numIndices, indexType, offsetIndices, renderable.numInstances));
	}
	else
	{
		GLenum drawType = EGL3Types::asVertexType(_stateCaptured.getPrimitiveType());
		GL_CHECK(glDrawArraysInstanced(drawType, 0, renderable.numVertices, renderable.numInstances));
	}
}

void
EGL3DeviceContext::drawRenderMesh(const GraphicsIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	for (std::size_t i = first; i < first + count; i++)
		this->drawRenderMesh(renderable[i]);
}

void
EGL3DeviceContext::setFramebuffer(GraphicsFramebufferPtr target) noexcept
{
	assert(_glcontext->getActive());

	if (target)
	{
		auto framebuffer = target->downcast<EGL3Framebuffer>();
		if (_framebuffer != framebuffer)
		{
			_framebuffer = framebuffer;
			glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer->getInstanceID());

			auto& framebufferDesc = _framebuffer->getGraphicsFramebufferDesc();
			this->setViewport(0, Viewport(0, 0, framebufferDesc.getWidth(), framebufferDesc.getHeight()));
		}
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		_framebuffer = nullptr;
	}
}

void
EGL3DeviceContext::blitFramebuffer(GraphicsFramebufferPtr src, const Viewport& v1, GraphicsFramebufferPtr dest, const Viewport& v2) noexcept
{
	assert(src);
	assert(src->isInstanceOf<EGL3Framebuffer>());
	assert(_glcontext->getActive());

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
}

GraphicsFramebufferPtr
EGL3DeviceContext::getFramebuffer() const noexcept
{
	if (_framebuffer)
		return _framebuffer->upcast_pointer<GraphicsFramebuffer>();
	return nullptr;
}

void
EGL3DeviceContext::clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_glcontext->getActive());

	GLbitfield mode = 0;

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagColorBit)
	{
		mode |= GL_COLOR_BUFFER_BIT;

		if (_clearColor != color)
		{
			glClearColor(color.x, color.y, color.z, color.w);
			_clearColor = color;
		}
	}

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
	{
		mode |= GL_DEPTH_BUFFER_BIT;

		if (_clearDepth != depth)
		{
			glClearDepthf(depth);
			_clearDepth = depth;
		}
	}

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagStencilBit)
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
		auto depthWriteEnable = _stateCaptured.getDepthWriteEnable();
		if (!depthWriteEnable && flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
		{
			glDepthMask(GL_TRUE);
		}

		GL_CHECK(glClear(mode));

		if (!depthWriteEnable && flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
		{
			glDepthMask(GL_FALSE);
		}
	}
}

void
EGL3DeviceContext::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_glcontext->getActive());

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
	{
		auto depthWriteEnable = _stateCaptured.getDepthWriteEnable();
		if (!depthWriteEnable && flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
		{
			glDepthMask(GL_TRUE);
		}

		GLfloat f = depth;
		glClearBufferfv(GL_DEPTH, 0, &f);

		if (!depthWriteEnable && flags & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
		{
			glDepthMask(GL_FALSE);
		}
	}

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagStencilBit)
	{
		GLint s = stencil;
		glClearBufferiv(GL_STENCIL, 0, &s);
	}

	if (flags & GraphicsClearFlagBits::GraphicsClearFlagColorBit)
	{
		glClearBufferfv(GL_COLOR, i, color.ptr());
	}
}

void
EGL3DeviceContext::discardFramebuffer(GraphicsAttachmentType attachments[], std::size_t i) noexcept
{
	assert(_glcontext->getActive());
}

void
EGL3DeviceContext::present() noexcept
{
	assert(_glcontext->getActive());
	_glcontext->present();
}

bool
EGL3DeviceContext::isTextureSupport(GraphicsFormat format) noexcept
{
	return std::find(_supportTextures.begin(), _supportTextures.end(), format) != _supportTextures.end();
}

bool
EGL3DeviceContext::isTextureDimSupport(GraphicsTextureDim dimension) noexcept
{
	return std::find(_supportTextureDims.begin(), _supportTextureDims.end(), dimension) != _supportTextureDims.end();
}

bool
EGL3DeviceContext::isVertexSupport(GraphicsFormat format) noexcept
{
	return std::find(_supportAttribute.begin(), _supportAttribute.end(), format) != _supportAttribute.end();
}

bool
EGL3DeviceContext::isShaderSupport(GraphicsShaderStageFlagBits stage) noexcept
{
	return std::find(_supportShaders.begin(), _supportShaders.end(), stage) != _supportShaders.end();
}

bool
EGL3DeviceContext::checkSupport() noexcept
{
	if (!EGL3Types::setup())
		return false;

	return true;
}

bool
EGL3DeviceContext::initDebugControl(const GraphicsContextDesc& desc) noexcept
{
#if !defined(__ANDROID__) && !defined(__AMD__)
	if (!desc.getDebugMode())
		return true;

	if (!EGL3Types::isSupportFeature(EGL3Features::EGL3_KHR_debug))
	{
		GL_PLATFORM_LOG("Can't support GL_KHR_debug.");
		return false;
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

	GL_CHECK(glEnable(GL_DEBUG_OUTPUT));

	GL_CHECK(glDebugMessageCallback(debugCallBack, this));
	// enable all
	GL_CHECK(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE));

	return true;
#else
	return true;
#endif
}

bool
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

	return true;
}

bool
EGL3DeviceContext::initTextureSupports() noexcept
{
	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_required_internalformat))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4UNormPack8);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G6R5UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR5G6B5UNormPack16);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_read_format_bgra))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_ARM_rgba8) || EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_rgb8_rgba8))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_format_BGRA8888) || EGL3Types::isSupportFeature(EGL3Features::EGL3_APPLE_texture_format_BGRA8888))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_sRGB))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_NV_sRGB_formats))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3SRGBBlock);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_rg))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32UInt);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_norm16))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_texture_half_float) || EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_color_buffer_half_float))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_texture_float) || EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_color_buffer_float))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_type_2_10_10_10_REV))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_APPLE_texture_packed_float))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_depth_texture))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD16UNorm);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_depth24))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatX8_D24UNormPack32);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_depth32))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD32_SFLOAT);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_texture_stencil8))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatS8UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD16UNorm_S8UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_compression_dxt1))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBUNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_compression_s3tc))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC2UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3UNormBlock);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_KHR_texture_compression_astc_ldr))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC4x4UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC4x4SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x4UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x4SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x5UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x5SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x5UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x5SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x6UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x6SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x5UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x5SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x6UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x6SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x8UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x8SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x5UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x5SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x6UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x6SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x8UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x8SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x10UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x10SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x10UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x10SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x12UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x12SRGBBlock);
	}

	return true;
}

bool
EGL3DeviceContext::initTextureDimSupports() noexcept
{
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2D);
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2DArray);
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCube);

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_cube_map_array))
		_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCubeArray);

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_texture_3D))
		_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim3D);

	return true;
}

bool
EGL3DeviceContext::initVertexSupports() noexcept
{
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_vertex_type_10_10_10_2))
	{
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
	}

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_OES_vertex_half_float))
	{
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SFloat);
	}

	return true;
}

bool
EGL3DeviceContext::initShaderSupports() noexcept
{
	_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageVertexBit);
	_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageFragmentBit);
	_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageComputeBit);

	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_geometry_shader))
		_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageGeometryBit);

	return true;
}

void
EGL3DeviceContext::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
{
	if (severity == GL_DEBUG_SEVERITY_LOW)
		return;

	if (severity == GL_DEBUG_SEVERITY_MEDIUM)
		return;

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