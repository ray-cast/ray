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

	bool setup(RenderPipeline& pipeline) noexcept;
	void close() noexcept;

	void renderShadowMap(RenderPipeline& pipeline) noexcept;

private:
	virtual void onResolutionChangeBefore(RenderPipeline& pipeline) noexcept;
	virtual void onResolutionChangeAfter(RenderPipeline& pipeline) noexcept;

	virtual void onRenderPre(RenderPipeline& pipeline) noexcept;
	virtual void onRenderPipeline(RenderPipeline& pipeline, const CameraPtr& camera) noexcept;
	virtual void onRenderPost(RenderPipeline& pipeline) noexcept;

private:
	ShadowRenderPipeline(const ShadowRenderPipeline&) = delete;
	ShadowRenderPipeline& operator=(const ShadowRenderPipeline&) = delete;

private:
	MaterialPtr _softBlur;
	MaterialTechPtr _softGenShadowMap;
	MaterialTechPtr _softBlurShadowX;
	MaterialTechPtr _softBlurShadowY;
	MaterialParamPtr _softBlurShadowSource;
	MaterialParamPtr _softBlurShadowSourceInv;
	MaterialParamPtr _softClipConstant;

	GraphicsTextures _softShadowMaps;
	GraphicsTexturePtr _softShadowMapTemp;

	GraphicsFramebuffers _softShadowViews;
	GraphicsFramebufferPtr _softShadowViewTemp;
	GraphicsFramebufferLayoutPtr _softShadowViewLayout;

	float _shadowEsmFactor;
	float _shadowMapSize;
	GraphicsFormat _shadowFormat;
};

_NAME_END

#endif