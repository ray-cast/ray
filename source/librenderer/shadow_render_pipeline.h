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
#ifndef _H_SHADOW_RENDER_PIPELINE_H_
#define _H_SHADOW_RENDER_PIPELINE_H_

#include <ray/render_pipeline_controller.h>

_NAME_BEGIN

class ShadowRenderPipeline final : public RenderPipelineController
{
	__DeclareSubClass(ShadowRenderPipeline, RenderPipelineController)
public:
	ShadowRenderPipeline() noexcept;
	virtual ~ShadowRenderPipeline() noexcept;

	bool setup(RenderPipelinePtr pipeline) noexcept;
	void close() noexcept;

	void setShadowMode(ShadowMode mode) noexcept;
	ShadowMode getShadowMode() const noexcept;

	void setShadowQuality(ShadowQuality quality) noexcept;
	ShadowQuality getShadowQuality() const noexcept;

private:
	void renderShadowMaps(const CameraPtr& camera) noexcept;
	void renderShadowMap(const Light& light, RenderQueue queue) noexcept;

private:
	bool setupShadowMaterial(RenderPipeline& pipeline) noexcept;
	bool setupShadowMaps(RenderPipeline& pipeline) noexcept;
	bool setupShadowSoftMaps(RenderPipeline& pipeline) noexcept;

	void destroyShadowMaterial() noexcept;
	void destroyShadowMaps() noexcept;

private:
	virtual void onRenderBefore() noexcept;
	virtual void onRenderPipeline(const CameraPtr& camera) noexcept;
	virtual void onRenderAfter() noexcept;

private:
	ShadowRenderPipeline(const ShadowRenderPipeline&) = delete;
	ShadowRenderPipeline& operator=(const ShadowRenderPipeline&) = delete;

private:
	ShadowMode _shadowMode;
	ShadowQuality _shadowQuality;

	MaterialPtr _shadowRender;
	MaterialTechPtr _shadowBlurOrthoShadowX;
	MaterialTechPtr _shadowBlurPerspectiveFovShadowX;
	MaterialTechPtr _shadowBlurShadowX[(std::uint8_t)LightType::LightTypeRangeSize];
	MaterialTechPtr _shadowBlurShadowY;
	MaterialTechPtr _shadowLogBlurShadowX;
	MaterialTechPtr _shadowLogBlurShadowY;
	MaterialTechPtr _shadowConvOrthoLinearDepth;
	MaterialTechPtr _shadowConvPerspectiveFovLinearDepth;
	MaterialParamPtr _shadowShadowSource;
	MaterialParamPtr _shadowShadowSourceInv;
	MaterialParamPtr _shadowClipConstant;
	MaterialParamPtr _shadowOffset;
	MaterialParamPtr _shadowWeight;

	GraphicsTexturePtr _shadowShadowDepthMapTemp;
	GraphicsTexturePtr _shadowShadowDepthLinearMapTemp;

	GraphicsFramebufferPtr _shadowShadowDepthViewTemp;
	GraphicsFramebufferPtr _shadowShadowDepthLinearViewTemp;

	GraphicsFramebufferLayoutPtr _shadowShadowDepthImageLayout;
	GraphicsFramebufferLayoutPtr _shadowShadowDepthLinearImageLayout;

	GraphicsFormat _shadowDepthFormat;
	GraphicsFormat _shadowDepthLinearFormat;

	RenderPipelinePtr _pipeline;
};

_NAME_END

#endif