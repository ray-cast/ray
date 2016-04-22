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
	: _clearColor(0.0f, 0.0f, 0.0f, 0.0f)
	, _clearDepth(0.0)
	, _clearStencil(0xFFFFFFFF)
	, _viewport(0, 0, 0, 0)
	, _state(GL_NONE)
	, _renderTexture(GL_NONE)
	, _stateDefault(std::make_shared<EGL2GraphicsState>())
{
}

EGL2DeviceContext::~EGL2DeviceContext() noexcept
{
	this->close();
}

bool
EGL2DeviceContext::setup(const GraphicsContextDesc& desc) noexcept
{
	assert(desc.getSwapchain());
	assert(desc.getSwapchain()->isInstanceOf<EGL2Swapchain>());

	_glcontext = desc.getSwapchain()->downcast<EGL2Swapchain>();
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
EGL2DeviceContext::close() noexcept
{
	_renderTexture.reset();
	_shaderObject.reset();
	_pipeline.reset();
	_descriptorSet.reset();
	_state.reset();
	_stateDefault.reset();
	_vbo.reset();
	_ibo.reset();
	_inputLayout.reset();
	_supportTextures.clear();
	_supportAttribute.clear();
	_glcontext.reset();
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
EGL2DeviceContext::setViewport(const Viewport& view) noexcept
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
EGL2DeviceContext::setScissor(const Scissor& scissor) noexcept
{
	if (_scissor != scissor)
	{
		glScissor(scissor.left, scissor.top, scissor.width, scissor.height);
		_scissor = scissor;
	}
}

const Scissor&
EGL2DeviceContext::getScissor() const noexcept
{
	return _scissor;
}

void
EGL2DeviceContext::setStencilCompare(GraphicsStencilFace face, GraphicsCompareFunc func) noexcept
{
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
	{
		if (_stateCaptured.getStencilFrontFunc() != func)
		{
			GLenum frontfunc = EGL2Types::asCompareFunction(func);
			glStencilFuncSeparate(GL_FRONT, frontfunc, _stateCaptured.getStencilFrontRef(), _stateCaptured.getStencilFrontReadMask());
			_stateCaptured.setStencilFrontFunc(func);
		}
	}
	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
	{
		if (_stateCaptured.getStencilBackFunc() != func)
		{
			GLenum backfunc = EGL2Types::asCompareFunction(func);
			glStencilFuncSeparate(GL_BACK, backfunc, _stateCaptured.getStencilFrontRef(), _stateCaptured.getStencilFrontReadMask());
			_stateCaptured.setStencilBackFunc(func);
		}
	}
}

GraphicsCompareFunc
EGL2DeviceContext::getStencilCompare(GraphicsStencilFace face) noexcept
{
	if (face == GraphicsStencilFace::GraphicsStencilFaceFront)
		return _stateCaptured.getStencilFrontFunc();
	if (face == GraphicsStencilFace::GraphicsStencilFaceBack)
		return _stateCaptured.getStencilBackFunc();
	return GraphicsCompareFunc::GraphicsCompareFuncNone;
}

void
EGL2DeviceContext::setStencilReference(GraphicsStencilFace face, std::uint32_t reference) noexcept
{
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
	{
		if (_stateCaptured.getStencilFrontRef() != reference)
		{
			GLenum frontfunc = EGL2Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
			glStencilFuncSeparate(GL_FRONT, frontfunc, reference, _stateCaptured.getStencilFrontReadMask());
			_stateCaptured.setStencilFrontRef(reference);
		}
	}
	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
	{
		if (_stateCaptured.getStencilBackRef() != reference)
		{
			GLenum backfunc = EGL2Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
			glStencilFuncSeparate(GL_BACK, backfunc, reference, _stateCaptured.getStencilFrontReadMask());
			_stateCaptured.setStencilBackRef(reference);
		}
	}
}

std::uint32_t
EGL2DeviceContext::getStencilReference(GraphicsStencilFace face) noexcept
{
	if (face == GraphicsStencilFace::GraphicsStencilFaceFront)
		return _stateCaptured.getStencilFrontRef();
	if (face == GraphicsStencilFace::GraphicsStencilFaceBack)
		return _stateCaptured.getStencilBackRef();
	return std::uint32_t(0);
}

void
EGL2DeviceContext::setStencilFrontWriteMask(GraphicsStencilFace face, std::uint32_t mask) noexcept
{
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
	{
		if (_stateCaptured.getStencilFrontWriteMask() != mask)
		{
			GLenum frontfunc = EGL2Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
			glStencilFuncSeparate(GL_FRONT, frontfunc, _stateCaptured.getStencilFrontRef(), mask);
			_stateCaptured.setStencilFrontWriteMask(mask);
		}
	}
	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
	{
		if (_stateCaptured.getStencilBackWriteMask() != mask)
		{
			GLenum backfunc = EGL2Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
			glStencilFuncSeparate(GL_BACK, backfunc, _stateCaptured.getStencilBackRef(), mask);
			_stateCaptured.setStencilBackWriteMask(mask);
		}
	}
}

std::uint32_t
EGL2DeviceContext::getStencilFrontWriteMask(GraphicsStencilFace face) noexcept
{
	if (face == GraphicsStencilFace::GraphicsStencilFaceFront)
		return _stateCaptured.getStencilFrontWriteMask();
	if (face == GraphicsStencilFace::GraphicsStencilFaceBack)
		return _stateCaptured.getStencilBackWriteMask();
	return std::uint32_t(0);
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

		auto glshader = pipelineDesc.getGraphicsProgram()->downcast<EGL2Program>();
		if (_shaderObject != glshader)
		{
			_shaderObject = glshader;
			_shaderObject->apply();
		}

		auto inputLayout = pipelineDesc.getGraphicsInputLayout()->downcast<EGL2InputLayout>();
		if (_inputLayout != inputLayout)
		{
			_inputLayout = inputLayout;
			_inputLayout->bindLayout(_shaderObject);

			_needUpdateLayout = true;
		}
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
	assert(descriptorSet);
	assert(descriptorSet->isInstanceOf<EGL2DescriptorSet>());
	assert(_glcontext->getActive());

	_descriptorSet = descriptorSet->downcast<EGL2DescriptorSet>();
	_descriptorSet->apply(_shaderObject);
}

GraphicsDescriptorSetPtr
EGL2DeviceContext::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

const Viewport&
EGL2DeviceContext::getViewport() const noexcept
{
	return _viewport;
}

bool
EGL2DeviceContext::updateBuffer(GraphicsDataPtr data, void* str, std::size_t cnt) noexcept
{
	assert(data);
	assert(data->isInstanceOf<EGL2GraphicsData>());
	assert(_glcontext->getActive());

	_needUpdateLayout = true;
	data->downcast<EGL2GraphicsData>()->resize((const char*)str, cnt);
	return true;
}

void*
EGL2DeviceContext::mapBuffer(GraphicsDataPtr data, std::uint32_t access) noexcept
{
	assert(data);
	assert(data->isInstanceOf<EGL2GraphicsData>());
	assert(_glcontext->getActive());

	_needUpdateLayout = true;
	return data->downcast<EGL2GraphicsData>()->map(access);
}

void
EGL2DeviceContext::unmapBuffer(GraphicsDataPtr data) noexcept
{
	assert(data);
	assert(data->isInstanceOf<EGL2GraphicsData>());
	assert(_glcontext->getActive());

	_needUpdateLayout = true;
	data->downcast<EGL2GraphicsData>()->unmap();
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
		_needUpdateLayout = true;
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
		_needUpdateLayout = true;
	}
}

GraphicsDataPtr
EGL2DeviceContext::getVertexBufferData() const noexcept
{
	return _vbo;
}

void
EGL2DeviceContext::drawRenderMesh(const GraphicsIndirect& renderable) noexcept
{
	if (!_inputLayout || !_vbo)
		return;

	if (_needUpdateLayout)
	{
		if (_vbo)
			_inputLayout->bindVbo(_vbo);

		if (_ibo)
			_inputLayout->bindIbo(_ibo);

		_needUpdateLayout = false;
	}

	auto primitiveType = _stateCaptured.getPrimitiveType();
	if (_ibo)
	{
		GLenum drawType = EGL2Types::asVertexType(primitiveType);
		GLenum indexType = EGL2Types::asIndexType(renderable.indexType);
		GLvoid* offsetIndices = (GLchar*)(nullptr) + (_ibo->getGraphicsDataDesc().getStride() * renderable.startIndice);
		GL_CHECK(glDrawElements(drawType, renderable.numIndices, indexType, offsetIndices));
	}
	else
	{
		GLenum drawType = EGL2Types::asVertexType(primitiveType);
		GL_CHECK(glDrawArrays(drawType, renderable.startVertice, renderable.numVertices));
	}
}

void
EGL2DeviceContext::drawRenderMesh(const GraphicsIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	for (std::size_t i = first; i < first + count; i++)
		this->drawRenderMesh(renderable[i]);
}

void
EGL2DeviceContext::setFramebuffer(GraphicsFramebufferPtr target) noexcept
{
	if (_renderTexture != target)
	{
		if (target)
		{
			_renderTexture = target->downcast<EGL2Framebuffer>();
			auto& framebufferDesc = _renderTexture->getGraphicsFramebufferDesc();

			glBindFramebuffer(GL_FRAMEBUFFER, _renderTexture->getInstanceID());

			this->setViewport(Viewport(0, 0, framebufferDesc.getWidth(), framebufferDesc.getHeight()));
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
			_renderTexture = nullptr;
		}
	}
}

void
EGL2DeviceContext::setFramebuffer(GraphicsFramebufferPtr target, const float4& color, float depth, std::int32_t stencil) noexcept
{
	this->setFramebuffer(target);
	this->clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsAll, color, depth, stencil);
}

void
EGL2DeviceContext::blitFramebuffer(GraphicsFramebufferPtr src, const Viewport& v1, GraphicsFramebufferPtr dest, const Viewport& v2) noexcept
{
	GL_PLATFORM_LOG("Can't support blitFramebuffer");
}

GraphicsFramebufferPtr
EGL2DeviceContext::getFramebuffer() const noexcept
{
	return _renderTexture;
}

void
EGL2DeviceContext::clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
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
		auto depthWriteEnable = _stateCaptured.getDepthWriteEnable();
		if (!depthWriteEnable && flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
		{
			GL_CHECK(glDepthMask(GL_TRUE));
		}

		GL_CHECK(glClear(mode));

		if (!depthWriteEnable && flags & GraphicsClearFlags::GraphicsClearFlagsDepth)
		{
			GL_CHECK(glDepthMask(GL_FALSE));
		}
	}
}

void
EGL2DeviceContext::discardFramebuffer() noexcept
{
	GL_PLATFORM_LOG("Can't support discardFramebuffer");
}

void
EGL2DeviceContext::readFramebuffer(GraphicsFramebufferPtr target, GraphicsFormat pfd, std::size_t w, std::size_t h, std::size_t bufsize, void* data) noexcept
{
	assert(w && h && data);

	if (target)
	{
		GLenum format = EGL2Types::asTextureFormat(pfd);
		GLenum type = EGL2Types::asTextureType(pfd);

		if (format != GL_INVALID_ENUM && type != GL_INVALID_ENUM)
		{
			this->setFramebuffer(target);
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

bool
EGL2DeviceContext::isTextureSupport(GraphicsFormat format) noexcept
{
	return std::find(_supportTextures.begin(), _supportTextures.end(), format) != _supportTextures.end();
}

bool
EGL2DeviceContext::isTextureDimSupport(GraphicsTextureDim dimension) noexcept
{
	return std::find(_supportTextureDims.begin(), _supportTextureDims.end(), dimension) != _supportTextureDims.end();
}

bool
EGL2DeviceContext::isVertexSupport(GraphicsFormat format) noexcept
{
	return std::find(_supportAttribute.begin(), _supportAttribute.end(), format) != _supportAttribute.end();
}

bool
EGL2DeviceContext::isShaderSupport(GraphicsShaderStage stage) noexcept
{
	return std::find(_supportShaders.begin(), _supportShaders.end(), stage) != _supportShaders.end();
}

bool
EGL2DeviceContext::checkSupport() noexcept
{
	if (EGL2Types::setup())
		return true;
	return false;
}

bool
EGL2DeviceContext::initDebugControl(const GraphicsContextDesc& desc) noexcept
{
#if !defined(__ANDROID__) && !defined(__AMD__)
	if (!desc.getDebugMode())
		return true;

	if (!EGL2Types::isSupportFeature(EGL2Features::EGL2_KHR_debug))
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

	return true;
}

bool
EGL2DeviceContext::initTextureSupports() noexcept
{
	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_required_internalformat))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4UNormPack8);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G6R5UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR5G6B5UNormPack16);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_read_format_bgra))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_ARM_rgba8) || EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_rgb8_rgba8))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_format_BGRA8888) || EGL2Types::isSupportFeature(EGL2Features::EGL2_APPLE_texture_format_BGRA8888))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_sRGB))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_NV_sRGB_formats))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3SRGBBlock);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_rg))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8UNorm);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_norm16))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_texture_half_float) || EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_color_buffer_half_float))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_texture_float) || EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_color_buffer_float))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_type_2_10_10_10_REV))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_APPLE_texture_packed_float))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_depth_texture))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD16UNorm);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_depth24))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatX8_D24UNormPack32);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_depth32))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD32_SFLOAT);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_texture_stencil8))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatS8UInt);
	}

	/*if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_packed_depth_stencil))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	}*/

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_compression_dxt1))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBUNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_compression_s3tc))
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC2UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3UNormBlock);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_KHR_texture_compression_astc_ldr))
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
EGL2DeviceContext::initTextureDimSupports() noexcept
{
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2D);
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2DArray);
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCube);
	
	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_cube_map_array))
		_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCubeArray);

	return true;
}

bool
EGL2DeviceContext::initVertexSupports() noexcept
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

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_vertex_type_10_10_10_2))
	{
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
	}

	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_vertex_half_float))
	{
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SFloat);
	}

	return true;
}

bool
EGL2DeviceContext::initShaderSupports() noexcept
{
	_supportShaders.push_back(GraphicsShaderStage::GraphicsShaderStageVertex);
	_supportShaders.push_back(GraphicsShaderStage::GraphicsShaderStageFragment);

	return true;
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