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
#ifndef _H_DEFERRED_LIGHTING_PIPELINE_H_
#define _H_DEFERRED_LIGHTING_PIPELINE_H_

#include <ray/render_pipeline_controller.h>

_NAME_BEGIN

class DeferredLightingPipeline final : public RenderPipelineController
{
public:
	DeferredLightingPipeline() noexcept;
	~DeferredLightingPipeline() noexcept;

	bool setup(const RenderPipelinePtr& pipeline, bool enableMRSII = false) noexcept;
	void close() noexcept;

	void render3DEnvMap(const Camera* camera) noexcept;

	void renderOpaques(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;
	void renderOpaquesDepthLinear(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;
	void renderOpaquesShading(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;
	void renderOpaquesSpecificShading(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;

	void renderTransparentBack(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;
	void renderTransparentDepthLinearBack(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;
	void renderTransparentShadingBack(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;
	void renderTransparentSpecificShadingBack(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;

	void renderTransparentFront(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;
	void renderTransparentDepthLinearFront(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;
	void renderTransparentShadingFront(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;
	void renderTransparentSpecificShadingFront(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;

	void renderLights(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;
	void renderSunLight(RenderPipeline& pipeline, const Light& light) noexcept;
	void renderDirectionalLight(RenderPipeline& pipeline, const Light& light) noexcept;
	void renderPointLight(RenderPipeline& pipeline, const Light& light) noexcept;
	void renderSpotLight(RenderPipeline& pipeline, const Light& light) noexcept;
	void renderAmbientLight(RenderPipeline& pipeline, const Light& light) noexcept;
	void renderEnvironmentLight(RenderPipeline& pipeline, const Light& light) noexcept;

	void renderAmbientLights(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;
	void renderDirectLights(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;
	void renderIndirectSpotLight(RenderPipeline& pipeline, const Light& light) noexcept;
	void renderIndirectLights(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept;

	void copyRenderTexture(RenderPipeline& pipeline, const GraphicsTexturePtr& src, const GraphicsFramebufferPtr& dst) noexcept;
	void copyRenderTexture(RenderPipeline& pipeline, const GraphicsTexturePtr& src, const GraphicsFramebufferPtr& dst, const float4& viewport) noexcept;

private:
	void computeSpotVPLBuffers(RenderPipeline& pipeline, const Light& light) noexcept;
	void computeDepthDerivBuffer(RenderPipeline& pipeline, const GraphicsTexturePtr& src, const GraphicsFramebuffers& dst);
	void computeNormalDerivBuffer(RenderPipeline& pipeline, const GraphicsTexturePtr& src, const GraphicsFramebuffers& dst);
	void computeSubsplatStencil(RenderPipeline& pipeline, const GraphicsTexturePtr& depth, const GraphicsTexturePtr& normal, const GraphicsFramebuffers& dst);
	void computeUpsamplingMultiresBuffer(RenderPipeline& pipeline, GraphicsTexturePtr src, const GraphicsFramebuffers& srcviews, const GraphicsFramebufferPtr& dst);

private:
	bool initTextureFormat(RenderPipeline& pipeline) noexcept;

	bool setupSemantic(RenderPipeline& pipeline) noexcept;
	bool setupDeferredMaterials(RenderPipeline& pipeline) noexcept;

	bool setupMRSII(RenderPipeline& pipeline) noexcept;
	bool setupMRSIIMaterials(RenderPipeline& pipeline) noexcept;
	bool setupMRSIITextures(RenderPipeline& pipeline) noexcept;
	bool setupMRSIIRenderTextures(RenderPipeline& pipeline) noexcept;
	bool setupMRSIIRenderTextureLayouts(RenderPipeline& pipeline) noexcept;

	void destroySemantic() noexcept;
	void destroyDeferredMaterials() noexcept;

	void destroyMRSIIMaterials() noexcept;
	void destroyMRSIITextures() noexcept;
	void destroyMRSIIRenderTextures() noexcept;
	void destroyMRSIIRenderTextureLayouts() noexcept;

private:
	virtual void onRenderBefore() noexcept;
	virtual void onRenderPipeline(const Camera* camera) noexcept;
	virtual void onRenderAfter() noexcept;

	virtual void onResolutionChange() noexcept;

private:

	std::uint32_t _mrsiiDerivMipBase;
	std::uint32_t _mrsiiDerivMipCount;

	bool _enabledMRSSI;

	MaterialPtr _mrsii;
	MaterialTechPtr _mrsiiRsm2VPLsSpot;
	MaterialTechPtr _mrsiiRsm2VPLsDirectional;
	MaterialTechPtr _mrsiiGatherIndirect;
	MaterialTechPtr _mrsiiGatherIndirectDebug;
	MaterialTechPtr	_mrsiiDepthDerivate;
	MaterialTechPtr _mrsiiDepthDerivateMipmap;
	MaterialTechPtr _mrsiiNormalDerivate;
	MaterialTechPtr	_mrsiiNormalDerivateMipmap;
	MaterialTechPtr _mrsiiComputeSubsplatStencil;
	MaterialTechPtr _mrsiiUpsampling;
	MaterialTechPtr _mrsiiUpsamplingWithBlend;
	MaterialParamPtr _mrsiiColorMap;
	MaterialParamPtr _mrsiiNormalMap;
	MaterialParamPtr _mrsiiDepthLinearMap;
	MaterialParamPtr _mrsiiLightColor;
	MaterialParamPtr _mrsiiLightAttenuation;
	MaterialParamPtr _mrsiiLightOuterInner;
	MaterialParamPtr _mrsiiCountGridOffsetDelta;
	MaterialParamPtr _mrsiiLightView2EyeView;
	MaterialParamPtr _mrsiiOffset;
	MaterialParamPtr _mrsiiOffsetUpsampling;
	MaterialParamPtr _mrsiiMRT0;
	MaterialParamPtr _mrsiiMRT1;
	MaterialParamPtr _mrsiiVPLsBuffer;
	MaterialParamPtr _mrsiiMipmapLevel;
	MaterialParamPtr _mrsiiMipmapPass;
	MaterialParamPtr _mrsiiThreshold;

	GraphicsTexturePtr _mrsiiVPLsBufferMap;
	GraphicsTexturePtr _mrsiiDepthDerivMap;
	GraphicsTexturePtr _mrsiiNormalDerivMap;
	GraphicsTexturePtr _mrsiiSubsplatStencilMap;
	GraphicsTexturePtr _mrsiiGaterIndirectMap;

	GraphicsFramebufferPtr _mrsiiVPLsView;
	GraphicsFramebufferLayoutPtr _mrsiiVPLsViewLayout;

	GraphicsFramebuffers _mrsiiDepthDerivViews;
	GraphicsFramebuffers _mrsiiNormalDerivViews;
	GraphicsFramebuffers _mrsiiSubsplatStencilViews;
	GraphicsFramebuffers _mrsiiGaterIndirectViews;
	GraphicsFramebufferLayoutPtr _mrsiiDepthDerivViewLayout;
	GraphicsFramebufferLayoutPtr _mrsiiNormalDerivViewLayout;
	GraphicsFramebufferLayoutPtr _mrsiiSubsplatStencilViewLayout;
	GraphicsFramebufferLayoutPtr _mrsiiGaterIndirectViewLayout;

	GraphicsFormat _mrsiiDepthLinearFormat;
	GraphicsFormat _mrsiiNormalFormat;
	GraphicsFormat _mrsiiStencilFormat;
	GraphicsFormat _mrsiiLightFormat;

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
	MaterialTechPtr _deferredEnvironmentLighting;
	MaterialTechPtr _deferredShadingOpaques;
	MaterialTechPtr _deferredShadingTransparents;
	MaterialTechPtr _deferredDebugLayer;
	MaterialTechPtr _deferredCopyOnly;

	MaterialParamPtr _texMRT0;
	MaterialParamPtr _texMRT1;
	MaterialParamPtr _texMRT2;
	MaterialParamPtr _texMRT3;
	MaterialParamPtr _texDepth;
	MaterialParamPtr _texLight;
	MaterialParamPtr _texSource;
	MaterialParamPtr _texOpaque;

	MaterialParamPtr _eyePosition;
	MaterialParamPtr _clipInfo;
	MaterialParamPtr _scaleY;

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

	MaterialParamPtr _envBoxMax;
	MaterialParamPtr _envBoxMin;
	MaterialParamPtr _envBoxCenter;
	MaterialParamPtr _envDiffuse;
	MaterialParamPtr _envSpecular;
	MaterialParamPtr _envFactor;

	MaterialSemanticPtr _materialDeferredDepthMap;
	MaterialSemanticPtr _materialDeferredDepthLinearMap;
	MaterialSemanticPtr _materialDeferredLightMap;
	MaterialSemanticPtr _materialDeferredOpaqueShadingMap;

	RenderPipelinePtr _pipeline;
};

_NAME_END

#endif