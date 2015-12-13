// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
#ifndef _H_DEFERRED_LIGHTING_H_
#define _H_DEFERRED_LIGHTING_H_

#include <ray/render_pipeline_base.h>

_NAME_BEGIN

class DefaultRenderPipeline final : public RenderPipeline
{
public:
	DefaultRenderPipeline() except;
	~DefaultRenderPipeline() noexcept;

	void renderShadowMap(CameraPtr camera) noexcept;

	void render2DEnvMap(CameraPtr camera) noexcept;
	void render3DEnvMap(CameraPtr camera) noexcept;

	void renderCamera(CameraPtr camera) noexcept;

private:

	void renderOpaques(MultiRenderTexturePtr target) noexcept;
	void renderOpaquesDepthLinear(RenderTexturePtr target) noexcept;
	void renderOpaquesShading(RenderTexturePtr target, int layer = 0) noexcept;
	void renderOpaquesSpecificShading(RenderTexturePtr target, int layer = 0) noexcept;

	void renderTransparent(MultiRenderTexturePtr target) noexcept;
	void renderTransparentDepthLinear(RenderTexturePtr target) noexcept;
	void renderTransparentShading(RenderTexturePtr target, int layer = 0) noexcept;
	void renderTransparentSpecificShading(RenderTexturePtr target, int layer = 0) noexcept;

	void renderLights(RenderTexturePtr target) noexcept;
	void renderSunLight(const Light& light) noexcept;
	void renderDirectionalLight(const Light& light) noexcept;
	void renderAmbientLight(const Light& light) noexcept;
	void renderPointLight(const Light& light) noexcept;
	void renderSpotLight(const Light& light) noexcept;
	void renderHemiSphereLight(const Light& light) noexcept;
	void renderAreaLight(const Light& light) noexcept;

	void copyRenderTexture(RenderTexturePtr src, RenderTexturePtr dst, const Viewport& view) noexcept;

private:
	virtual void onActivate() except;
	virtual void onDeactivate() noexcept;

	virtual void onRenderPre(CameraPtr camera) noexcept;
	virtual void onRenderPipeline(CameraPtr camera) noexcept;
	virtual void onRenderPost(CameraPtr camera) noexcept;
private:

	CameraPtr _leftCamera;
	CameraPtr _rightCamera;
	CameraPtr _frontCamera;
	CameraPtr _backCamera;
	CameraPtr _topCamera;
	CameraPtr _bottomCamera;

	MaterialPtr _deferredLighting;
	MaterialPassPtr _deferredDepthOnly;
	MaterialPassPtr _deferredDepthLinear;
	MaterialPassPtr _deferredSunLight;
	MaterialPassPtr _deferredSunLightShadow;
	MaterialPassPtr _deferredDirectionalLight;
	MaterialPassPtr _deferredDirectionalLightShadow;
	MaterialPassPtr _deferredSpotLight;
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
	MaterialParamPtr _shadowChannel;
	MaterialParamPtr _shadowMap;
	MaterialParamPtr _shadowArrayMap;
	MaterialParamPtr _shadowFactor;
	MaterialParamPtr _shadowMatrix;
	MaterialParamPtr _shadowOffset;
	MaterialParamPtr _shadowWeight;

	MaterialParamPtr _lightColor;
	MaterialParamPtr _lightPosition;
	MaterialParamPtr _lightDirection;
	MaterialParamPtr _lightRange;
	MaterialParamPtr _lightIntensity;
	MaterialParamPtr _lightAttenuation;
	MaterialParamPtr _lightSpotInnerCone;
	MaterialParamPtr _lightSpotOuterCone;

	RenderTexturePtr _deferredDepthMap;
	RenderTexturePtr _deferredDepthLinearMap;
	RenderTexturePtr _deferredGraphicMap;
	RenderTexturePtr _deferredNormalMap;
	RenderTexturePtr _deferredLightMap;
	RenderTexturePtr _deferredShadingMap;

	MultiRenderTexturePtr _deferredGraphicMaps;
};

_NAME_END

#endif