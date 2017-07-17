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
#ifndef _H_DEFERRED_LIGHTING_FRAMEBUFFERS_H_
#define _H_DEFERRED_LIGHTING_FRAMEBUFFERS_H_

#include <ray/render_pipeline_framebuffer.h>

_NAME_BEGIN

class EXPORT DeferredLightingFramebuffers : public RenderPipelineFramebuffer
{
	__DeclareSubInterface(RenderPipelineController, rtti::Interface)
public:
	DeferredLightingFramebuffers() noexcept;
	DeferredLightingFramebuffers(const GraphicsFramebufferPtr& framebuffer, const GraphicsFramebufferPtr& linearDepth) noexcept;
	virtual ~DeferredLightingFramebuffers() noexcept;

	bool setup(RenderSystem& pipeline, std::uint32_t width, std::uint32_t height);
	void close() noexcept;

	const GraphicsTexturePtr& getDeferredDepthMap() const noexcept { return _deferredDepthMap; }
	const GraphicsTexturePtr& getDeferredDepthLinearMap() const noexcept { return _deferredDepthLinearMap; }
	const GraphicsTexturePtr& getDeferredGbuffer1Map() const noexcept { return _deferredGbuffer1Map; }
	const GraphicsTexturePtr& getDeferredGbuffer2Map() const noexcept { return _deferredGbuffer2Map; }
	const GraphicsTexturePtr& getDeferredGbuffer3Map() const noexcept { return _deferredGbuffer3Map; }
	const GraphicsTexturePtr& getDeferredGbuffer4Map() const noexcept { return _deferredGbuffer4Map; }
	const GraphicsTexturePtr& getDeferredLightingMap() const noexcept { return _deferredLightingMap; }
	const GraphicsTexturePtr& getDeferredOpaqueShadingMap() const noexcept { return _deferredOpaqueShadingMap; }
	const GraphicsTexturePtr& getDeferredTransparentShadingMap() const noexcept { return _deferredTransparentShadingMap; }
	const GraphicsTexturePtr& getDeferredFinalShadingMap() const noexcept { return _deferredFinalShadingMap; }
	const GraphicsTexturePtr& getDeferredSwapMap() const noexcept { return _deferredSwapMap; }

	const GraphicsFramebufferPtr& getDeferredDepthView() const noexcept { return _deferredDepthView; }
	const GraphicsFramebufferPtr& getDeferredDepthLinearView() const noexcept { return _deferredDepthLinearView; }
	const GraphicsFramebufferPtr& getDeferredGbuffer1View() const noexcept { return _deferredGbuffer1View; }
	const GraphicsFramebufferPtr& getDeferredGbuffer2View() const noexcept { return _deferredGbuffer2View; }
	const GraphicsFramebufferPtr& getDeferredGbuffer3View() const noexcept { return _deferredGbuffer3View; }
	const GraphicsFramebufferPtr& getDeferredGbuffer4View() const noexcept { return _deferredGbuffer4View; }
	const GraphicsFramebufferPtr& getDeferredGbuffersView() const noexcept { return _deferredGbuffersView; }
	const GraphicsFramebufferPtr& getDeferredLightingView() const noexcept { return _deferredLightingView; }
	const GraphicsFramebufferPtr& getDeferredOpaqueShadingView() const noexcept { return _deferredOpaqueShadingView; }
	const GraphicsFramebufferPtr& getDeferredTransparentShadingViews() const noexcept { return _deferredTransparentShadingViews; }
	const GraphicsFramebufferPtr& getDeferredFinalShadingView() const noexcept { return _deferredFinalShadingView; }
	const GraphicsFramebufferPtr& getDeferredSwapView() const noexcept { return _deferredSwapView; }

private:
	DeferredLightingFramebuffers(const DeferredLightingFramebuffers&) noexcept = delete;
	DeferredLightingFramebuffers& operator=(const DeferredLightingFramebuffers&) noexcept = delete;

private:
	bool initTextureFormat(RenderSystem& pipeline) noexcept;

	bool setupDeferredTextures(RenderSystem& pipeline, std::uint32_t width, std::uint32_t height) noexcept;
	bool setupDeferredRenderTextures(RenderSystem& pipeline, std::uint32_t width, std::uint32_t height) noexcept;
	bool setupDeferredRenderTextureLayouts(RenderSystem& pipeline, std::uint32_t width, std::uint32_t height) noexcept;

	void destroyDeferredTextures() noexcept;
	void destroyDeferredRenderTextures() noexcept;
	void destroyDeferredRenderTextureLayouts() noexcept;

private:
	GraphicsFormat _deferredDepthFormat;
	GraphicsFormat _deferredDepthLinearFormat;
	GraphicsFormat _deferredGbuffer1Format;
	GraphicsFormat _deferredGbuffer2Format;
	GraphicsFormat _deferredGbuffer3Format;
	GraphicsFormat _deferredGbuffer4Format;
	GraphicsFormat _deferredTransparentFormat;
	GraphicsFormat _deferredLightFormat;
	GraphicsFormat _deferredShadingFormat;

	GraphicsTexturePtr _deferredDepthMap;
	GraphicsTexturePtr _deferredDepthLinearMap;
	GraphicsTexturePtr _deferredGbuffer1Map;
	GraphicsTexturePtr _deferredGbuffer2Map;
	GraphicsTexturePtr _deferredGbuffer3Map;
	GraphicsTexturePtr _deferredGbuffer4Map;
	GraphicsTexturePtr _deferredLightingMap;
	GraphicsTexturePtr _deferredOpaqueShadingMap;
	GraphicsTexturePtr _deferredTransparentShadingMap;
	GraphicsTexturePtr _deferredFinalShadingMap;
	GraphicsTexturePtr _deferredSwapMap;

	GraphicsFramebufferLayoutPtr _deferredDepthImageLayout;
	GraphicsFramebufferLayoutPtr _deferredDepthLinearImageLayout;
	GraphicsFramebufferLayoutPtr _deferredGbuffer1ImageLayout;
	GraphicsFramebufferLayoutPtr _deferredGbuffer2ImageLayout;
	GraphicsFramebufferLayoutPtr _deferredGbuffer3ImageLayout;
	GraphicsFramebufferLayoutPtr _deferredGbuffer4ImageLayout;
	GraphicsFramebufferLayoutPtr _deferredGbuffersImageLayout;
	GraphicsFramebufferLayoutPtr _deferredLightingImageLayout;
	GraphicsFramebufferLayoutPtr _deferredShadingImageLayout;
	GraphicsFramebufferLayoutPtr _deferredTransparentImagesLayout;

	GraphicsFramebufferPtr _deferredDepthView;
	GraphicsFramebufferPtr _deferredDepthLinearView;
	GraphicsFramebufferPtr _deferredGbuffer1View;
	GraphicsFramebufferPtr _deferredGbuffer2View;
	GraphicsFramebufferPtr _deferredGbuffer3View;
	GraphicsFramebufferPtr _deferredGbuffer4View;
	GraphicsFramebufferPtr _deferredGbuffersView;
	GraphicsFramebufferPtr _deferredLightingView;
	GraphicsFramebufferPtr _deferredOpaqueShadingView;
	GraphicsFramebufferPtr _deferredTransparentShadingViews;
	GraphicsFramebufferPtr _deferredFinalShadingView;
	GraphicsFramebufferPtr _deferredSwapView;
};

_NAME_END

#endif