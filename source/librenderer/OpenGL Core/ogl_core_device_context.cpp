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
	: _clearColor(0.0f, 0.0f, 0.0f, 0.0f)
	, _clearDepth(1.0f)
	, _clearStencil(0)
	, _framebuffer(nullptr)
	, _shaderObject(nullptr)
	, _pipeline(nullptr)
	, _descriptorSet(nullptr)
	, _state(nullptr)
	, _vbo(nullptr)
	, _ibo(nullptr)
	, _glcontext(nullptr)
	, _needUpdateState(true)
	, _needUpdateLayout(true)
	, _needUpdateDescriptor(true)
{
	std::memset(&_viewport, 0, sizeof(_viewport));
	std::memset(&_scissor, 0, sizeof(_scissor));

	GraphicsColorBlends blends(4);
	_stateCaptured.setColorBlends(blends);
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

	_glcontext = desc.getSwapchain()->downcast<OGLSwapchain>();
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
OGLCoreDeviceContext::close() noexcept
{
	_framebuffer = nullptr;
	_shaderObject = nullptr;
	_pipeline = nullptr;
	_descriptorSet = nullptr;
	_state = nullptr;
	_vbo = nullptr;
	_ibo = nullptr;
	_glcontext = nullptr;
	_supportTextures.clear();
	_supportAttribute.clear();
}

void
OGLCoreDeviceContext::renderBegin() noexcept
{
	assert(_glcontext);
	_glcontext->setActive(true);
}

void
OGLCoreDeviceContext::renderEnd() noexcept
{
}

void
OGLCoreDeviceContext::setViewport(const Viewport& view) noexcept
{
	assert(_glcontext->getActive());

	if (_viewport != view)
	{
		glViewport(view.left, view.top, view.width, view.height);
		_viewport = view;
	}
}

const Viewport&
OGLCoreDeviceContext::getViewport() const noexcept
{
	return _viewport;
}

void
OGLCoreDeviceContext::setScissor(const Scissor& scissor) noexcept
{
	assert(_glcontext->getActive());

	if (_scissor != scissor)
	{
		glScissor(scissor.left, scissor.top, scissor.width, scissor.height);
		_scissor = scissor;
	}
}

const Scissor&
OGLCoreDeviceContext::getScissor() const noexcept
{
	return _scissor;
}

void
OGLCoreDeviceContext::setStencilCompareMask(GraphicsStencilFace face, std::uint32_t mask) noexcept
{
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
	{
		if (_stateCaptured.getStencilFrontReadMask() != mask)
		{
			GLenum frontfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilFrontFunc());
			glStencilFuncSeparate(GL_FRONT, frontfunc, _stateCaptured.getStencilFrontRef(), mask);
			_stateCaptured.setStencilFrontReadMask(mask);
			_needUpdateState = true;
		}
	}
	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
	{
		if (_stateCaptured.getStencilBackReadMask() != mask)
		{
			GLenum backfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilBackFunc());
			glStencilFuncSeparate(GL_BACK, backfunc, _stateCaptured.getStencilFrontRef(), mask);
			_stateCaptured.setStencilBackReadMask(mask);
			_needUpdateState = true;
		}
	}
	
}

std::uint32_t
OGLCoreDeviceContext::getStencilCompareMask(GraphicsStencilFace face) noexcept
{
	if (face == GraphicsStencilFace::GraphicsStencilFaceFront)
		return _stateCaptured.getStencilFrontReadMask();
	if (face == GraphicsStencilFace::GraphicsStencilFaceBack)
		return _stateCaptured.getStencilBackReadMask();
	return 0;
}

void
OGLCoreDeviceContext::setStencilReference(GraphicsStencilFace face, std::uint32_t reference) noexcept
{
	if (face & GraphicsStencilFace::GraphicsStencilFaceFront)
	{
		if (_stateCaptured.getStencilFrontRef() != reference)
		{
			GLenum frontfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilFrontFunc());
			glStencilFuncSeparate(GL_FRONT, frontfunc, reference, _stateCaptured.getStencilFrontReadMask());
			_stateCaptured.setStencilFrontRef(reference);
			_needUpdateState = true;
		}
	}
	if (face & GraphicsStencilFace::GraphicsStencilFaceBack)
	{
		if (_stateCaptured.getStencilBackRef() != reference)
		{
			GLenum backfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilBackFunc());
			glStencilFuncSeparate(GL_BACK, backfunc, reference, _stateCaptured.getStencilFrontReadMask());
			_stateCaptured.setStencilBackRef(reference);
			_needUpdateState = true;
		}
	}
}

std::uint32_t
OGLCoreDeviceContext::getStencilReference(GraphicsStencilFace face) noexcept
{
	if (face == GraphicsStencilFace::GraphicsStencilFaceFront)
		return _stateCaptured.getStencilFrontRef();
	if (face == GraphicsStencilFace::GraphicsStencilFaceBack)
		return _stateCaptured.getStencilBackRef();
	return 0;
}

void
OGLCoreDeviceContext::setStencilFrontWriteMask(GraphicsStencilFace face, std::uint32_t mask) noexcept
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
OGLCoreDeviceContext::getStencilFrontWriteMask(GraphicsStencilFace face) noexcept
{
	if (face == GraphicsStencilFace::GraphicsStencilFaceFront)
		return _stateCaptured.getStencilFrontWriteMask();
	if (face == GraphicsStencilFace::GraphicsStencilFaceBack)
		return _stateCaptured.getStencilBackWriteMask();
	return 0;
}

void
OGLCoreDeviceContext::setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept
{
	assert(pipeline);
	assert(pipeline->isInstanceOf<OGLCorePipeline>());
	assert(_glcontext->getActive());

	auto glpipeline = pipeline->downcast<OGLCorePipeline>();
	if (_pipeline != glpipeline || _needUpdateState)
	{
		auto& pipelineDesc = pipeline->getGraphicsPipelineDesc();

		auto glstate = pipelineDesc.getGraphicsState()->downcast<OGLGraphicsState>();
		if (_state != glstate || _needUpdateState)
		{
			glstate->apply(_stateCaptured);
			_state = glstate;
			_needUpdateState = false;
		}

		auto glshader = pipelineDesc.getGraphicsProgram()->downcast<OGLProgram>();
		if (_shaderObject != glshader)
		{
			_shaderObject = glshader;
			_shaderObject->apply();
		}

		if (_pipeline != glpipeline)
		{
			_pipeline = glpipeline;
			_pipeline->apply();

			_needUpdateLayout = true;
		}
	}
}

GraphicsPipelinePtr
OGLCoreDeviceContext::getRenderPipeline() const noexcept
{
	if (_pipeline)
		return _pipeline->upcast_pointer<GraphicsPipeline>();
	return nullptr;
}

void
OGLCoreDeviceContext::setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept
{
	assert(descriptorSet);
	assert(descriptorSet->isInstanceOf<OGLCoreDescriptorSet>());
	assert(_glcontext->getActive());
	assert(_shaderObject);

	_descriptorSet = descriptorSet->downcast<OGLCoreDescriptorSet>();
	_needUpdateDescriptor = true;
}

GraphicsDescriptorSetPtr
OGLCoreDeviceContext::getDescriptorSet() const noexcept
{
	if (_descriptorSet)
		return _descriptorSet->upcast_pointer<GraphicsDescriptorSet>();
	return nullptr;
}

void
OGLCoreDeviceContext::setVertexBufferData(GraphicsDataPtr data) noexcept
{
	assert(data);
	assert(data->isInstanceOf<OGLCoreGraphicsData>());
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);
	assert(_glcontext->getActive());

	if (data)
	{
		auto vbo = data->downcast<OGLCoreGraphicsData>();
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
OGLCoreDeviceContext::getVertexBufferData() const noexcept
{
	if (_vbo)
		return _vbo->upcast_pointer<GraphicsData>();
	return nullptr;
}

void
OGLCoreDeviceContext::setIndexBufferData(GraphicsDataPtr data) noexcept
{
	assert(!data || data->isInstanceOf<OGLCoreGraphicsData>());
	assert(!data || data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
	assert(_glcontext->getActive());

	if (data)
	{
		auto ibo = data->downcast<OGLCoreGraphicsData>();
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
OGLCoreDeviceContext::getIndexBufferData() const noexcept
{
	if (_ibo)
		return _ibo->upcast_pointer<GraphicsData>();
	return nullptr;
}

void
OGLCoreDeviceContext::drawRenderMesh(const GraphicsIndirect& renderable) noexcept
{
	assert(_vbo && _pipeline);
	assert(_glcontext->getActive());

	if (_needUpdateLayout)
	{
		if (_vbo)
			_pipeline->bindVbo(*_vbo, 0);

		if (_ibo)
			_pipeline->bindIbo(*_ibo);

		_needUpdateLayout = false;
	}

	if (_needUpdateDescriptor)
	{
		_descriptorSet->apply(*_shaderObject);
		_needUpdateDescriptor = false;
	}

	if (_ibo)
	{
		GLenum drawType = OGLTypes::asVertexType(_stateCaptured.getPrimitiveType());
		GLenum indexType = OGLTypes::asIndexType(renderable.indexType);
		GLvoid* offsetIndices = (GLchar*)(nullptr) + (_ibo->getGraphicsDataDesc().getStride() * renderable.startIndice);
		glDrawElementsInstancedBaseVertexBaseInstance(drawType, renderable.numIndices, indexType, offsetIndices, renderable.numInstances, renderable.startVertice, renderable.startInstances);
	}
	else
	{
		GLenum drawType = OGLTypes::asVertexType(_stateCaptured.getPrimitiveType());
		glDrawArraysInstancedBaseInstance(drawType, renderable.startVertice, renderable.numVertices, renderable.numInstances, renderable.startInstances);
	}
}

void
OGLCoreDeviceContext::drawRenderMesh(const GraphicsIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	assert(_glcontext->getActive());

	for (std::size_t i = first; i < first + count; i++)
		this->drawRenderMesh(renderable[i]);
}

void
OGLCoreDeviceContext::setFramebuffer(GraphicsFramebufferPtr target) noexcept
{
	assert(_glcontext->getActive());

	if (target)
	{
		auto framebuffer = target->downcast<OGLCoreFramebuffer>();
		if (_framebuffer != framebuffer)
		{
			_framebuffer = framebuffer;
			glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer->getInstanceID());

			auto& framebufferDesc = _framebuffer->getGraphicsFramebufferDesc();
			this->setViewport(Viewport(0, 0, framebufferDesc.getWidth(), framebufferDesc.getHeight()));
		}
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		_framebuffer = nullptr;
	}
}

void
OGLCoreDeviceContext::blitFramebuffer(GraphicsFramebufferPtr src, const Viewport& v1, GraphicsFramebufferPtr dest, const Viewport& v2) noexcept
{
	assert(src);
	assert(src->isInstanceOf<OGLCoreFramebuffer>());
	assert(!dest || (dest && dest->isInstanceOf<OGLCoreFramebuffer>()));
	assert(_glcontext->getActive());

	auto readFramebuffer = src->downcast<OGLCoreFramebuffer>()->getInstanceID();
	auto drawFramebuffer = dest ? dest->downcast<OGLCoreFramebuffer>()->getInstanceID() : GL_NONE;

	glBlitNamedFramebuffer(readFramebuffer, drawFramebuffer, v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

GraphicsFramebufferPtr
OGLCoreDeviceContext::getFramebuffer() const noexcept
{
	if (_framebuffer)
		return _framebuffer->upcast_pointer<GraphicsFramebuffer>();
	return nullptr;
}

void
OGLCoreDeviceContext::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
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
OGLCoreDeviceContext::discardFramebuffer(GraphicsAttachmentType attachments[], std::size_t i) noexcept
{
	assert(_glcontext->getActive());
	assert(_framebuffer);
	_framebuffer->discard(attachments, i);
}

void
OGLCoreDeviceContext::present() noexcept
{
	assert(_glcontext->getActive());
	_glcontext->present();
}

bool
OGLCoreDeviceContext::isTextureSupport(GraphicsFormat format) noexcept
{
	return std::find(_supportTextures.begin(), _supportTextures.end(), format) != _supportTextures.end();
}

bool
OGLCoreDeviceContext::isTextureDimSupport(GraphicsTextureDim dimension) noexcept
{
	return std::find(_supportTextureDims.begin(), _supportTextureDims.end(), dimension) != _supportTextureDims.end();
}

bool
OGLCoreDeviceContext::isVertexSupport(GraphicsFormat format) noexcept
{
	return std::find(_supportAttribute.begin(), _supportAttribute.end(), format) != _supportAttribute.end();
}

bool
OGLCoreDeviceContext::isShaderSupport(GraphicsShaderStage stage) noexcept
{
	return std::find(_supportShaders.begin(), _supportShaders.end(), stage) != _supportShaders.end();
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

bool
OGLCoreDeviceContext::initTextureSupports() noexcept
{
	if (GLEW_VERSION_2_0 || GLEW_EXT_texture)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4UNormPack8);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
	}

	if (GLEW_VERSION_2_0 || GLEW_EXT_texture || GLEW_EXT_bgra)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16);
	}

	if (GLEW_VERSION_2_0 || GLEW_EXT_texture || GLEW_EXT_abgr)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8UNormPack32);
	}

	if (GLEW_EXT_texture_integer)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SInt);
	}

	if (GLEW_EXT_bgra && GLEW_EXT_texture_integer)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SInt);
	}

	if (GLEW_EXT_abgr && GLEW_EXT_texture_integer)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8UIntPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SIntPack32);
	}

	if (GLEW_ARB_texture_float)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	}

	if (GLEW_EXT_packed_float)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);
	}

	if (GLEW_EXT_texture_shared_exponent)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32);
	}

	if (GLEW_ARB_texture_stencil8)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatS8UInt);
	}

	if (GLEW_ARB_depth_texture)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatX8_D24UNormPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD32_SFLOAT);
	}

	if (GLEW_ARB_depth_buffer_float || GLEW_NV_depth_buffer_float)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt);
	}

	if (GLEW_EXT_packed_depth_stencil || GLEW_ARB_framebuffer_object)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	}

	if (GLEW_ARB_texture_rgb10_a2ui)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32);
	}

	if (GLEW_EXT_texture_sRGB)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3SRGBBlock);
	}

	if (GLEW_EXT_bgra && GLEW_EXT_texture_sRGB)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SRGB);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	}

	if (GLEW_EXT_abgr && GLEW_EXT_texture_sRGB)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	}

	if (GLEW_ARB_texture_rg)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32UInt);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
	}

	if (GLEW_EXT_texture_snorm)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	}

	if (GLEW_EXT_bgra && GLEW_EXT_texture_snorm)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SNorm);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SNorm);
	}

	if (GLEW_EXT_bgra && GLEW_EXT_texture_snorm)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SNormPack32);
	}

	if (GLEW_EXT_texture_compression_s3tc)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBUNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC5UNormBlock);
	}

	if (GLEW_ARB_ES2_compatibility)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatR5G6B5UNormPack16);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G6R5UNormPack16);
	}

	if (GLEW_ARB_ES3_compatibility)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A1UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A8UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11SNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11G11UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11G11SNormBlock);
	}

	if (GLEW_ARB_ES3_compatibility && GL_EXT_texture_sRGB)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A1SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A8SRGBBlock);
	}

	if (GLEW_KHR_texture_compression_astc_ldr || GL_KHR_texture_compression_astc_hdr)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC4x4UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x4UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x5UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x5UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x6UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x5UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x6UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x8UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x5UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x6UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x8UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x10UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x10UNormBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x12UNormBlock);
	}

	if ((GLEW_KHR_texture_compression_astc_ldr || GL_KHR_texture_compression_astc_hdr) && GL_EXT_texture_sRGB)
	{
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC4x4SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x4SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x5SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x5SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x6SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x5SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x6SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x8SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x5SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x6SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x8SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x10SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x10SRGBBlock);
		_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x12SRGBBlock);
	}

	return true;
}

bool
OGLCoreDeviceContext::initTextureDimSupports() noexcept
{
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2D);
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2DArray);
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCube);

	if (GLEW_ARB_texture_cube_map_array)
		_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCubeArray);

	if (GLEW_ARB_texture_storage)
		_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim3D);

	return true;
}

bool
OGLCoreDeviceContext::initVertexSupports() noexcept
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

	if (GLEW_ARB_vertex_attrib_64bit || GLEW_EXT_vertex_attrib_64bit)
	{
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64SInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64UInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64SInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64UInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64SInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64UInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64A64SInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64A64UInt);
	}

	if (GLEW_ARB_vertex_type_10f_11f_11f_rev)
	{
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);
	}

	if (GLEW_ARB_vertex_type_2_10_10_10_rev)
	{
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);

		if (GLEW_ARB_vertex_array_bgra)
		{
			_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10SIntPack32);
			_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32);
			_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32);
		}
	}

	if (GLEW_ARB_vertex_array_bgra)
	{
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8SInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8UInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8SNorm);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8UNorm);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UInt);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SNorm);
		_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
	}

	return true;
}

bool
OGLCoreDeviceContext::initShaderSupports() noexcept
{
	_supportShaders.push_back(GraphicsShaderStage::GraphicsShaderStageVertex);
	_supportShaders.push_back(GraphicsShaderStage::GraphicsShaderStageFragment);

	if (GLEW_ARB_geometry_shader4)
		_supportShaders.push_back(GraphicsShaderStage::GraphicsShaderStageGeometry);

	if (GLEW_ARB_compute_shader)
		_supportShaders.push_back(GraphicsShaderStage::GraphicsShaderStageCompute);

	if (GLEW_ARB_tessellation_shader)
	{
		_supportShaders.push_back(GraphicsShaderStage::GraphicsShaderStageTessControl);
		_supportShaders.push_back(GraphicsShaderStage::GraphicsShaderStageTessEvaluation);
	}

	return true;
}

bool
OGLCoreDeviceContext::initDebugControl(const GraphicsContextDesc& desc) noexcept
{
	if (!desc.getDebugMode())
		return true;

	if (!GLEW_KHR_debug)
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

	glEnable(GL_DEBUG_OUTPUT);

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(debugCallBack, this);
	// enable all
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
	// disable ids
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 6, ids, GL_FALSE);

	return true;
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