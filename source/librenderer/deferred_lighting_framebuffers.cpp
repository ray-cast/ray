// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include <ray/deferred_lighting_framebuffers.h>
#include <ray/render_system.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>

_NAME_BEGIN

__ImplementSubInterface(DeferredLightingFramebuffers, rtti::Interface, "DeferredLightingFramebuffers")

DeferredLightingFramebuffers::DeferredLightingFramebuffers() noexcept
{
}

DeferredLightingFramebuffers::~DeferredLightingFramebuffers() noexcept
{
	this->close();
}

bool
DeferredLightingFramebuffers::setup(RenderSystem& pipeline, std::uint32_t width, std::uint32_t height)
{
	if (!this->initTextureFormat(pipeline))
		return false;

	if (!this->setupDeferredTextures(pipeline, width, height))
		return false;

	if (!this->setupDeferredRenderTextureLayouts(pipeline, width, height))
		return false;

	if (!this->setupDeferredRenderTextures(pipeline, width, height))
		return false;

	return true;
}

void
DeferredLightingFramebuffers::close() noexcept
{
	this->destroyDeferredTextures();
	this->destroyDeferredRenderTextures();
	this->destroyDeferredRenderTextureLayouts();
}

bool
DeferredLightingFramebuffers::initTextureFormat(RenderSystem& pipeline) noexcept
{
	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt))
		_deferredDepthFormat = GraphicsFormat::GraphicsFormatD24UNorm_S8UInt;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatD16UNorm_S8UInt))
		_deferredDepthFormat = GraphicsFormat::GraphicsFormatD16UNorm_S8UInt;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatX8_D24UNormPack32))
		_deferredDepthFormat = GraphicsFormat::GraphicsFormatX8_D24UNormPack32;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatD16UNorm))
		_deferredDepthFormat = GraphicsFormat::GraphicsFormatD16UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR32SFloat))
		_deferredDepthLinearFormat = GraphicsFormat::GraphicsFormatR32SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16SFloat))
		_deferredDepthLinearFormat = GraphicsFormat::GraphicsFormatR16SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredDepthLinearFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat))
		_deferredLightFormat = GraphicsFormat::GraphicsFormatR16G16B16A16SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm))
		_deferredLightFormat = GraphicsFormat::GraphicsFormatR16G16B16A16UNorm;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredLightFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredGbuffer1Format = _deferredGbuffer2Format = _deferredGbuffer3Format = _deferredGbuffer4Format = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat))
		_deferredShadingFormat = GraphicsFormat::GraphicsFormatR16G16B16A16SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm))
		_deferredShadingFormat = GraphicsFormat::GraphicsFormatR16G16B16A16UNorm;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredShadingFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat))
		_deferredTransparentFormat = GraphicsFormat::GraphicsFormatR16G16B16A16SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm))
		_deferredTransparentFormat = GraphicsFormat::GraphicsFormatR16G16B16A16UNorm;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredTransparentFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	return true;
}

bool
DeferredLightingFramebuffers::setupDeferredTextures(RenderSystem& pipeline, std::uint32_t width, std::uint32_t height) noexcept
{
	GraphicsTextureDesc _deferredDepthDesc;
	_deferredDepthDesc.setWidth(width);
	_deferredDepthDesc.setHeight(height);
	_deferredDepthDesc.setTexFormat(_deferredDepthFormat);
	_deferredDepthDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredDepthDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_deferredDepthDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_deferredDepthMap = pipeline.createTexture(_deferredDepthDesc);
	if (!_deferredDepthMap)
		return false;

	GraphicsTextureDesc _deferredDepthLinearDesc;
	_deferredDepthLinearDesc.setWidth(width);
	_deferredDepthLinearDesc.setHeight(height);
	_deferredDepthLinearDesc.setTexFormat(_deferredDepthLinearFormat);
	_deferredDepthLinearDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredDepthLinearDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_deferredDepthLinearDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_deferredDepthLinearMap = pipeline.createTexture(_deferredDepthLinearDesc);
	if (!_deferredDepthLinearMap)
		return false;

	GraphicsTextureDesc _deferredGbufferDesc;
	_deferredGbufferDesc.setWidth(width);
	_deferredGbufferDesc.setHeight(height);
	_deferredGbufferDesc.setTexFormat(_deferredGbuffer1Format);
	_deferredGbufferDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredGbufferDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_deferredGbufferDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_deferredGbuffer1Map = pipeline.createTexture(_deferredGbufferDesc);
	if (!_deferredGbuffer1Map)
		return false;

	GraphicsTextureDesc _deferredGbuffer2Desc;
	_deferredGbuffer2Desc.setWidth(width);
	_deferredGbuffer2Desc.setHeight(height);
	_deferredGbuffer2Desc.setTexFormat(_deferredGbuffer2Format);
	_deferredGbuffer2Desc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredGbuffer2Desc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_deferredGbuffer2Desc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_deferredGbuffer2Map = pipeline.createTexture(_deferredGbuffer2Desc);
	if (!_deferredGbuffer2Map)
		return false;

	GraphicsTextureDesc _deferredGbuffer3Desc;
	_deferredGbuffer3Desc.setWidth(width);
	_deferredGbuffer3Desc.setHeight(height);
	_deferredGbuffer3Desc.setTexFormat(_deferredGbuffer3Format);
	_deferredGbuffer3Desc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredGbuffer3Desc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_deferredGbuffer3Desc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_deferredGbuffer3Map = pipeline.createTexture(_deferredGbuffer3Desc);
	if (!_deferredGbuffer3Map)
		return false;

	GraphicsTextureDesc _deferredGbuffer4Desc;
	_deferredGbuffer4Desc.setWidth(width);
	_deferredGbuffer4Desc.setHeight(height);
	_deferredGbuffer4Desc.setTexFormat(_deferredGbuffer4Format);
	_deferredGbuffer4Desc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredGbuffer4Desc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_deferredGbuffer4Desc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_deferredGbuffer4Map = pipeline.createTexture(_deferredGbuffer4Desc);
	if (!_deferredGbuffer4Map)
		return false;

	GraphicsTextureDesc _deferredLightDesc;
	_deferredLightDesc.setWidth(width);
	_deferredLightDesc.setHeight(height);
	_deferredLightDesc.setTexFormat(_deferredLightFormat);
	_deferredLightDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredLightDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_deferredLightDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_deferredLightingMap = pipeline.createTexture(_deferredLightDesc);
	if (!_deferredLightingMap)
		return false;

	GraphicsTextureDesc _deferredShadingDesc;
	_deferredShadingDesc.setWidth(width);
	_deferredShadingDesc.setHeight(height);
	_deferredShadingDesc.setTexFormat(_deferredShadingFormat);
	_deferredShadingDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredShadingDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_deferredShadingDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_deferredOpaqueShadingMap = pipeline.createTexture(_deferredShadingDesc);
	if (!_deferredOpaqueShadingMap)
		return false;

	GraphicsTextureDesc _deferredTransparentDesc;
	_deferredTransparentDesc.setWidth(width);
	_deferredTransparentDesc.setHeight(height);
	_deferredTransparentDesc.setTexFormat(_deferredTransparentFormat);
	_deferredTransparentDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredTransparentDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_deferredTransparentDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_deferredTransparentShadingMap = pipeline.createTexture(_deferredTransparentDesc);
	if (!_deferredTransparentShadingMap)
		return false;

	_deferredFinalShadingMap = pipeline.createTexture(_deferredShadingDesc);
	if (!_deferredFinalShadingMap)
		return false;

	_deferredSwapMap = pipeline.createTexture(_deferredShadingDesc);
	if (!_deferredSwapMap)
		return false;

	return true;
}

bool
DeferredLightingFramebuffers::setupDeferredRenderTextureLayouts(RenderSystem& pipeline, std::uint32_t width, std::uint32_t height) noexcept
{
	GraphicsFramebufferLayoutDesc deferredDepthLayoutDesc;
	deferredDepthLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal, _deferredDepthFormat));
	_deferredDepthImageLayout = pipeline.createFramebufferLayout(deferredDepthLayoutDesc);
	if (!_deferredDepthImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredDepthLinearLayoutDesc;
	deferredDepthLinearLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredDepthLinearFormat));
	_deferredDepthLinearImageLayout = pipeline.createFramebufferLayout(deferredDepthLinearLayoutDesc);
	if (!_deferredDepthLinearImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredGraphicsLayoutDesc;
	deferredGraphicsLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredGbuffer1Format));
	_deferredGbuffer1ImageLayout = pipeline.createFramebufferLayout(deferredGraphicsLayoutDesc);
	if (!_deferredGbuffer1ImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredGbuffer2LayoutDesc;
	deferredGbuffer2LayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredGbuffer2Format));
	_deferredGbuffer2ImageLayout = pipeline.createFramebufferLayout(deferredGbuffer2LayoutDesc);
	if (!_deferredGbuffer2ImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredGbuffer3LayoutDesc;
	deferredGbuffer3LayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredGbuffer3Format));
	_deferredGbuffer3ImageLayout = pipeline.createFramebufferLayout(deferredGbuffer3LayoutDesc);
	if (!_deferredGbuffer3ImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredGbuffer4LayoutDesc;
	deferredGbuffer4LayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredGbuffer4Format));
	_deferredGbuffer4ImageLayout = pipeline.createFramebufferLayout(deferredGbuffer4LayoutDesc);
	if (!_deferredGbuffer4ImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredGbuffersImageLayoutDesc;
	deferredGbuffersImageLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredGbuffer1Format));
	deferredGbuffersImageLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredGbuffer2Format));
	deferredGbuffersImageLayoutDesc.addComponent(GraphicsAttachmentLayout(2, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredGbuffer3Format));
	deferredGbuffersImageLayoutDesc.addComponent(GraphicsAttachmentLayout(3, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredGbuffer4Format));
	deferredGbuffersImageLayoutDesc.addComponent(GraphicsAttachmentLayout(4, GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal, _deferredDepthFormat));
	_deferredGbuffersImageLayout = pipeline.createFramebufferLayout(deferredGbuffersImageLayoutDesc);
	if (!_deferredGbuffersImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredLightingLayoutDesc;
	deferredLightingLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredLightFormat));
	deferredLightingLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal, _deferredDepthFormat));
	_deferredLightingImageLayout = pipeline.createFramebufferLayout(deferredLightingLayoutDesc);
	if (!_deferredLightingImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredShadingLayoutDesc;
	deferredShadingLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredShadingFormat));
	deferredShadingLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal, _deferredDepthFormat));
	_deferredShadingImageLayout = pipeline.createFramebufferLayout(deferredShadingLayoutDesc);
	if (!_deferredShadingImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredTransparenImagesLayoutDesc;
	deferredTransparenImagesLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredShadingFormat));
	deferredTransparenImagesLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal, _deferredDepthFormat));
	_deferredTransparentImagesLayout = pipeline.createFramebufferLayout(deferredTransparenImagesLayoutDesc);
	if (!_deferredTransparentImagesLayout)
		return false;

	return true;
}

bool
DeferredLightingFramebuffers::setupDeferredRenderTextures(RenderSystem& pipeline, std::uint32_t width, std::uint32_t height) noexcept
{
	GraphicsFramebufferDesc deferredDepthDesc;
	deferredDepthDesc.setWidth(width);
	deferredDepthDesc.setHeight(height);
	deferredDepthDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_deferredDepthMap, 0, 0));
	deferredDepthDesc.setGraphicsFramebufferLayout(_deferredDepthImageLayout);
	_deferredDepthView = pipeline.createFramebuffer(deferredDepthDesc);
	if (!_deferredDepthView)
		return false;

	GraphicsFramebufferDesc deferredDepthLinearDesc;
	deferredDepthLinearDesc.setWidth(width);
	deferredDepthLinearDesc.setHeight(height);
	deferredDepthLinearDesc.addColorAttachment(GraphicsAttachmentBinding(_deferredDepthLinearMap, 0, 0));
	deferredDepthLinearDesc.setGraphicsFramebufferLayout(_deferredDepthLinearImageLayout);
	_deferredDepthLinearView = pipeline.createFramebuffer(deferredDepthLinearDesc);
	if (!_deferredDepthLinearView)
		return false;

	GraphicsFramebufferDesc deferredGbuffer1Desc;
	deferredGbuffer1Desc.setWidth(width);
	deferredGbuffer1Desc.setHeight(height);
	deferredGbuffer1Desc.addColorAttachment(GraphicsAttachmentBinding(_deferredGbuffer1Map, 0, 0));
	deferredGbuffer1Desc.setGraphicsFramebufferLayout(_deferredGbuffer1ImageLayout);
	_deferredGbuffer1View = pipeline.createFramebuffer(deferredGbuffer1Desc);
	if (!_deferredGbuffer1View)
		return false;

	GraphicsFramebufferDesc deferredGbuffer2Desc;
	deferredGbuffer2Desc.setWidth(width);
	deferredGbuffer2Desc.setHeight(height);
	deferredGbuffer2Desc.addColorAttachment(GraphicsAttachmentBinding(_deferredGbuffer2Map, 0, 0));
	deferredGbuffer2Desc.setGraphicsFramebufferLayout(_deferredGbuffer2ImageLayout);
	_deferredGbuffer2View = pipeline.createFramebuffer(deferredGbuffer2Desc);
	if (!_deferredGbuffer2View)
		return false;

	GraphicsFramebufferDesc deferredGbuffer3Desc;
	deferredGbuffer3Desc.setWidth(width);
	deferredGbuffer3Desc.setHeight(height);
	deferredGbuffer3Desc.addColorAttachment(GraphicsAttachmentBinding(_deferredGbuffer3Map, 0, 0));
	deferredGbuffer3Desc.setGraphicsFramebufferLayout(_deferredGbuffer3ImageLayout);
	_deferredGbuffer3View = pipeline.createFramebuffer(deferredGbuffer3Desc);
	if (!_deferredGbuffer3View)
		return false;

	GraphicsFramebufferDesc deferredGbuffer4Desc;
	deferredGbuffer4Desc.setWidth(width);
	deferredGbuffer4Desc.setHeight(height);
	deferredGbuffer4Desc.addColorAttachment(GraphicsAttachmentBinding(_deferredGbuffer4Map, 0, 0));
	deferredGbuffer4Desc.setGraphicsFramebufferLayout(_deferredGbuffer4ImageLayout);
	_deferredGbuffer4View = pipeline.createFramebuffer(deferredGbuffer4Desc);
	if (!_deferredGbuffer4View)
		return false;

	GraphicsFramebufferDesc deferredGbuffersDesc;
	deferredGbuffersDesc.setWidth(width);
	deferredGbuffersDesc.setHeight(height);
	deferredGbuffersDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_deferredDepthMap, 0, 0));
	deferredGbuffersDesc.addColorAttachment(GraphicsAttachmentBinding(_deferredGbuffer1Map, 0, 0));
	deferredGbuffersDesc.addColorAttachment(GraphicsAttachmentBinding(_deferredGbuffer2Map, 0, 0));
	deferredGbuffersDesc.addColorAttachment(GraphicsAttachmentBinding(_deferredGbuffer3Map, 0, 0));
	deferredGbuffersDesc.addColorAttachment(GraphicsAttachmentBinding(_deferredGbuffer4Map, 0, 0));
	deferredGbuffersDesc.setGraphicsFramebufferLayout(_deferredGbuffersImageLayout);
	_deferredGbuffersView = pipeline.createFramebuffer(deferredGbuffersDesc);
	if (!_deferredGbuffersView)
		return false;

	GraphicsFramebufferDesc deferredLightingDesc;
	deferredLightingDesc.setWidth(width);
	deferredLightingDesc.setHeight(height);
	deferredLightingDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_deferredDepthMap, 0, 0));
	deferredLightingDesc.addColorAttachment(GraphicsAttachmentBinding(_deferredLightingMap, 0, 0));
	deferredLightingDesc.setGraphicsFramebufferLayout(_deferredLightingImageLayout);
	_deferredLightingView = pipeline.createFramebuffer(deferredLightingDesc);
	if (!_deferredLightingView)
		return false;

	GraphicsFramebufferDesc deferredOpaqueShadingDesc;
	deferredOpaqueShadingDesc.setWidth(width);
	deferredOpaqueShadingDesc.setHeight(height);
	deferredOpaqueShadingDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_deferredDepthMap, 0, 0));
	deferredOpaqueShadingDesc.addColorAttachment(GraphicsAttachmentBinding(_deferredOpaqueShadingMap, 0, 0));
	deferredOpaqueShadingDesc.setGraphicsFramebufferLayout(_deferredShadingImageLayout);
	_deferredOpaqueShadingView = pipeline.createFramebuffer(deferredOpaqueShadingDesc);
	if (!_deferredOpaqueShadingView)
		return false;

	GraphicsFramebufferDesc deferredTransparentShadingDesc;
	deferredTransparentShadingDesc.setWidth(width);
	deferredTransparentShadingDesc.setHeight(height);
	deferredTransparentShadingDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_deferredDepthMap, 0, 0));
	deferredTransparentShadingDesc.addColorAttachment(GraphicsAttachmentBinding(_deferredTransparentShadingMap, 0, 0));
	deferredTransparentShadingDesc.setGraphicsFramebufferLayout(_deferredTransparentImagesLayout);
	_deferredTransparentShadingViews = pipeline.createFramebuffer(deferredTransparentShadingDesc);
	if (!_deferredTransparentShadingViews)
		return false;

	GraphicsFramebufferDesc deferredFinalShadingDesc;
	deferredFinalShadingDesc.setWidth(width);
	deferredFinalShadingDesc.setHeight(height);
	deferredFinalShadingDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_deferredDepthMap, 0, 0));
	deferredFinalShadingDesc.addColorAttachment(GraphicsAttachmentBinding(_deferredFinalShadingMap, 0, 0));
	deferredFinalShadingDesc.setGraphicsFramebufferLayout(_deferredShadingImageLayout);
	_deferredFinalShadingView = pipeline.createFramebuffer(deferredFinalShadingDesc);
	if (!_deferredFinalShadingView)
		return false;

	GraphicsFramebufferDesc deferredSwapDesc;
	deferredSwapDesc.setWidth(width);
	deferredSwapDesc.setHeight(height);
	deferredSwapDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_deferredDepthMap, 0, 0));
	deferredSwapDesc.addColorAttachment(GraphicsAttachmentBinding(_deferredSwapMap, 0, 0));
	deferredSwapDesc.setGraphicsFramebufferLayout(_deferredShadingImageLayout);
	_deferredSwapView = pipeline.createFramebuffer(deferredSwapDesc);
	if (!_deferredSwapView)
		return false;

	return true;
}

void
DeferredLightingFramebuffers::destroyDeferredRenderTextureLayouts() noexcept
{
	_deferredDepthImageLayout.reset();
	_deferredDepthLinearImageLayout.reset();
	_deferredGbuffer1ImageLayout.reset();
	_deferredGbuffer2ImageLayout.reset();
	_deferredGbuffer3ImageLayout.reset();
	_deferredGbuffer4ImageLayout.reset();
	_deferredLightingImageLayout.reset();
	_deferredShadingImageLayout.reset();
	_deferredGbuffersImageLayout.reset();
	_deferredTransparentImagesLayout.reset();
}

void
DeferredLightingFramebuffers::destroyDeferredTextures() noexcept
{
	_deferredDepthMap.reset();
	_deferredDepthLinearMap.reset();
	_deferredGbuffer1Map.reset();
	_deferredGbuffer2Map.reset();
	_deferredGbuffer3Map.reset();
	_deferredGbuffer4Map.reset();
	_deferredTransparentShadingMap.reset();
	_deferredLightingMap.reset();
	_deferredOpaqueShadingMap.reset();
	_deferredFinalShadingMap.reset();
	_deferredSwapMap.reset();
}

void
DeferredLightingFramebuffers::destroyDeferredRenderTextures() noexcept
{
	_deferredDepthView.reset();
	_deferredDepthLinearView.reset();
	_deferredGbuffer1View.reset();
	_deferredGbuffer2View.reset();
	_deferredGbuffer3View.reset();
	_deferredGbuffer4View.reset();
	_deferredGbuffersView.reset();
	_deferredLightingView.reset();
	_deferredOpaqueShadingView.reset();
	_deferredTransparentShadingViews.reset();
	_deferredFinalShadingView.reset();
	_deferredSwapView.reset();
}

_NAME_END