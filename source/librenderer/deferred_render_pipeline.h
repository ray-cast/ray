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

	bool setup(RenderPipeline& pipeline) noexcept;
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

private:
	bool initTextureFormat(RenderPipeline& pipeline) noexcept;

	bool setupSemantic(RenderPipeline& pipeline) noexcept;
	bool setupDeferredMaterials(RenderPipeline& pipeline) noexcept;
	bool setupDeferredTextures(RenderPipeline& pipeline) noexcept;
	bool setupDeferredRenderTextures(RenderPipeline& pipeline) noexcept;
	bool setupDeferredRenderTextureLayouts(RenderPipeline& pipeline) noexcept;
	bool setupShadowMaterial(RenderPipeline& pipeline) noexcept;
	bool setupShadowMap(RenderPipeline& pipeline) noexcept;

	void destroySemantic() noexcept;
	void destroyDeferredMaterials() noexcept;
	void destroyDeferredTextures() noexcept;
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
	MaterialTechPtr _deferredDepthOnly;
	MaterialTechPtr _deferredDepthLinear;
	MaterialTechPtr _deferredSunLight;
	MaterialTechPtr _deferredSunLightShadow;
	MaterialTechPtr _deferredDirectionalLight;
	MaterialTechPtr _deferredDirectionalLightShadow;
	MaterialTechPtr _deferredSpotLight;
	MaterialTechPtr _deferredSpotLightShadow;
	MaterialTechPtr _deferredPointLight;
	MaterialTechPtr _deferredAmbientLight;
	MaterialTechPtr _deferredShadingOpaques;
	MaterialTechPtr _deferredShadingTransparents;
	MaterialTechPtr _deferredDebugLayer;
	MaterialTechPtr _deferredCopyOnly;

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
	MaterialTechPtr _softGenShadowMap;
	MaterialTechPtr _softBlurOrthoShadowX;
	MaterialTechPtr _softBlurPerspectiveFovShadowX;
	MaterialTechPtr _softBlurShadowY;
	MaterialTechPtr _softLogBlurShadowX;
	MaterialTechPtr _softLogBlurShadowY;
	MaterialTechPtr _softConvOrthoLinearDepth;
	MaterialTechPtr _softConvPerspectiveFovLinearDepth;
	MaterialParamPtr _softShadowSource;
	MaterialParamPtr _softShadowSourceInv;
	MaterialParamPtr _softClipConstant;
	MaterialParamPtr _softOffset;
	MaterialParamPtr _softWeight;

	GraphicsTextures _softShadowMaps;
	GraphicsTexturePtr _softShadowMapTemp;
	GraphicsTexturePtr _softShadowDepthMapTemp;
	GraphicsFramebuffers _softShadowViews;
	GraphicsFramebufferPtr _softShadowViewTemp;
	GraphicsFramebufferPtr _softShadowDepthViewTemp;
	GraphicsFramebufferLayoutPtr _softShadowViewLayout;
	GraphicsFramebufferLayoutPtr _softShadowDepthViewLayout;

	float _shadowEsmFactor;
	std::uint32_t _shadowMapSize;
	GraphicsFormat _shadowFormat;
	GraphicsFormat _shadowDepthFormat;

	MaterialParamPtr _materialDepthMap;
	MaterialParamPtr _materialColorMap;
	MaterialParamPtr _materialNormalMap;
	MaterialParamPtr _materialDeferredDepthMap;
	MaterialParamPtr _materialDeferredDepthLinearMap;
	MaterialParamPtr _materialDeferredGraphicMap;
	MaterialParamPtr _materialDeferredNormalMap;
	MaterialParamPtr _materialDeferredLightMap;
	MaterialParamPtr _materialDeferredShadowMap;

	GraphicsFormat _deferredDepthFormat;
	GraphicsFormat _deferredDepthLinearFormat;
	GraphicsFormat _deferredGraphicsFormat;
	GraphicsFormat _deferredNormalFormat;
	GraphicsFormat _deferredLightFormat;
	GraphicsFormat _deferredShadingFormat;

	GraphicsTexturePtr _deferredDepthMap;
	GraphicsTexturePtr _deferredDepthLinearMap;
	GraphicsTexturePtr _deferredGraphicsMap;
	GraphicsTexturePtr _deferredNormalMap;
	GraphicsTexturePtr _deferredLightMap;
	GraphicsTexturePtr _deferredShadingMap;
	GraphicsTexturePtr _deferredSwapMap;

	GraphicsFramebufferLayoutPtr _deferredDepthViewLayout;
	GraphicsFramebufferLayoutPtr _deferredDepthLinearViewLayout;
	GraphicsFramebufferLayoutPtr _deferredGraphicsViewLayout;
	GraphicsFramebufferLayoutPtr _deferredNormalViewLayout;
	GraphicsFramebufferLayoutPtr _deferredLightingViewLayout;
	GraphicsFramebufferLayoutPtr _deferredShadingViewLayout;
	GraphicsFramebufferLayoutPtr _deferredGraphicsViewsLayout;

	GraphicsFramebufferPtr _deferredDepthView;
	GraphicsFramebufferPtr _deferredDepthLinearView;
	GraphicsFramebufferPtr _deferredGraphicsView;
	GraphicsFramebufferPtr _deferredNormalView;
	GraphicsFramebufferPtr _deferredLightingView;
	GraphicsFramebufferPtr _deferredShadingView;
	GraphicsFramebufferPtr _deferredSwapView;
	GraphicsFramebufferPtr _deferredGraphicsViews;
};

_NAME_END

#endif