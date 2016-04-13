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
#ifndef _H_DEFERRED_RENDER_PIPELINE_H_
#define _H_DEFERRED_RENDER_PIPELINE_H_

#include <ray/render_pipeline_controller.h>

_NAME_BEGIN

class DeferredRenderPipeline final : public RenderPipelineController
{
public:
	DeferredRenderPipeline() noexcept;
	~DeferredRenderPipeline() noexcept;

	void setup(RenderPipeline& pipeline) noexcept;
	void close() noexcept;

	void renderShadowMaps(RenderPipeline& pipeline) noexcept;

	void render2DEnvMap(RenderPipeline& pipeline) noexcept;
	void render3DEnvMap(RenderPipeline& pipeline) noexcept;

	void renderOpaques(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept;
	void renderOpaquesDepthLinear(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept;
	void renderOpaquesShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept;
	void renderOpaquesSpecificShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept;

	void renderTransparent(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept;
	void renderTransparentDepthLinear(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept;
	void renderTransparentShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept;
	void renderTransparentSpecificShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept;

	void renderLights(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept;
	void renderSunLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept;
	void renderDirectionalLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept;
	void renderAmbientLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept;
	void renderPointLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept;
	void renderSpotLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept;

	void copyRenderTexture(RenderPipeline& pipeline, GraphicsTexturePtr src, GraphicsFramebufferPtr dst, const Viewport& view) noexcept;

	bool enableSSSS(bool enable) noexcept;
	bool isEnableSSSS() const noexcept;

private:
	bool setupSemantic(RenderPipeline& pipeline) noexcept;
	bool setupDeferredMaterials(RenderPipeline& pipeline) noexcept;
	bool setupDeferredRenderTextures(RenderPipeline& pipeline) noexcept;
	bool setupDeferredRenderTextureLayouts(RenderPipeline& pipeline) noexcept;
	bool setupShadowMaterial(RenderPipeline& pipeline) noexcept;
	bool setupShadowMap(RenderPipeline& pipeline) noexcept;

	void destroySemantic() noexcept;
	void destroyDeferredMaterials() noexcept;
	void destroyDeferredRenderTextures() noexcept;
	void destroyDeferredRenderTextureLayouts() noexcept;
	void destroyShadowMaterial() noexcept;
	void destroyShadowMap() noexcept;

private:
	virtual void onResolutionChangeBefore(RenderPipeline& pipeline) noexcept;
	virtual void onResolutionChangeAfter(RenderPipeline& pipeline) noexcept;

	virtual void onRenderPre(RenderPipeline& pipeline) noexcept;
	virtual void onRenderPipeline(RenderPipeline& pipeline, const CameraPtr& camera) noexcept;
	virtual void onRenderPost(RenderPipeline& pipeline) noexcept;

private:

	MaterialPtr _deferredLighting;
	MaterialPassPtr _deferredDepthOnly;
	MaterialPassPtr _deferredDepthLinear;
	MaterialPassPtr _deferredSunLight;
	MaterialPassPtr _deferredSunLightShadow;
	MaterialPassPtr _deferredDirectionalLight;
	MaterialPassPtr _deferredDirectionalLightShadow;
	MaterialPassPtr _deferredSpotLight;
	MaterialPassPtr _deferredSpotLightShadow;
	MaterialPassPtr _deferredPointLight;
	MaterialPassPtr _deferredAmbientLight;
	MaterialPassPtr _deferredShadingOpaques;
	MaterialPassPtr _deferredShadingTransparents;
	MaterialPassPtr _deferredDebugLayer;
	MaterialPassPtr _deferredCopyOnly;

	MaterialParamPtr _texMRT0;
	MaterialParamPtr _texMRT1;
	MaterialParamPtr _texDepth;
	MaterialParamPtr _texLight;
	MaterialParamPtr _texSource;
	MaterialParamPtr _texEnvironmentMap;

	MaterialParamPtr _eyePosition;
	MaterialParamPtr _clipInfo;
	MaterialParamPtr _projInfo;

	MaterialParamPtr _shadowDecal;
	MaterialParamPtr _shadowMap;
	MaterialParamPtr _shadowFactor;
	MaterialParamPtr _shadowView2LightView;
	MaterialParamPtr _shadowView2LightViewProject;

	MaterialParamPtr _lightColor;
	MaterialParamPtr _lightEyePosition;
	MaterialParamPtr _lightEyeDirection;
	MaterialParamPtr _lightAttenuation;
	MaterialParamPtr _lightOuterInner;

	MaterialPtr _softBlur;
	MaterialPassPtr  _softBlurShadowX;
	MaterialPassPtr  _softBlurShadowY;
	MaterialParamPtr _softBlurShadowSource;
	MaterialParamPtr _softBlurShadowSourceInv;

	GraphicsTextures _softShaodwMaps;
	GraphicsTexturePtr _softShaodwMapTemp;

	GraphicsFramebuffers _softShaodwViews;
	GraphicsFramebufferPtr _softShaodwViewTemp;
	GraphicsFramebufferLayoutPtr _softShaodwViewLayout;

	float _shaodwFactor;
	float2 _shaodwMapSize;
	GraphicsFormat _shadowFormat;

	MaterialVariantPtr _materialDepthMap;
	MaterialVariantPtr _materialColorMap;
	MaterialVariantPtr _materialNormalMap;
	MaterialVariantPtr _materialDeferredDepthMap;
	MaterialVariantPtr _materialDeferredDepthLinearMap;
	MaterialVariantPtr _materialDeferredGraphicMap;
	MaterialVariantPtr _materialDeferredNormalMap;
	MaterialVariantPtr _materialDeferredLightMap;
	MaterialVariantPtr _materialDeferredShadowMap;

	GraphicsTexturePtr _deferredDepthMap;
	GraphicsTexturePtr _deferredDepthLinearMap;
	GraphicsTexturePtr _deferredGraphicsMap;
	GraphicsTexturePtr _deferredNormalMap;
	GraphicsTexturePtr _deferredLightMap;
	GraphicsTexturePtr _deferredShadingMap;
	GraphicsTexturePtr _deferredSwapMap;
	GraphicsTexturePtr _deferredFinalMap;

	GraphicsFramebufferLayoutPtr _deferredDepthViewLayout;
	GraphicsFramebufferLayoutPtr _deferredDepthLinearViewLayout;
	GraphicsFramebufferLayoutPtr _deferredGraphicsViewLayout;
	GraphicsFramebufferLayoutPtr _deferredNormalViewLayout;
	GraphicsFramebufferLayoutPtr _deferredLightingViewLayout;
	GraphicsFramebufferLayoutPtr _deferredShadingViewLayout;
	GraphicsFramebufferLayoutPtr _deferredSwapViewLayout;
	GraphicsFramebufferLayoutPtr _deferredFinalViewLayout;
	GraphicsFramebufferLayoutPtr _deferredGraphicsViewsLayout;

	GraphicsFramebufferPtr _deferredDepthView;
	GraphicsFramebufferPtr _deferredDepthLinearView;
	GraphicsFramebufferPtr _deferredGraphicsView;
	GraphicsFramebufferPtr _deferredNormalView;
	GraphicsFramebufferPtr _deferredLightingView;
	GraphicsFramebufferPtr _deferredShadingView;
	GraphicsFramebufferPtr _deferredSwapView;
	GraphicsFramebufferPtr _deferredFinalView;
	GraphicsFramebufferPtr _deferredGraphicsViews;

	RenderPostProcessPtr _SSSS;
};

_NAME_END

#endif