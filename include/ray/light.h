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
#ifndef _H_LIGHT_H_
#define _H_LIGHT_H_

#include <ray/render_object.h>

_NAME_BEGIN

class EXPORT Light final : public RenderObject, public RenderListener
{
	__DeclareSubClass(Light, RenderObject)
public:
	Light() noexcept;
	~Light() noexcept;

	void setRange(float range) noexcept;
	void setIntensity(float intensity) noexcept;
	void setSpotInnerCone(float value) noexcept;
	void setSpotOuterCone(float value) noexcept;

	float getRange() const noexcept;
	float getIntensity() const noexcept;
	float getSpotInnerCone() const noexcept;
	float getSpotOuterCone() const noexcept;

	void setLightType(LightType type) noexcept;
	LightType getLightType() const noexcept;

	void setLightColor(const float3& color) noexcept;
	const float3& getLightColor() const noexcept;

	void setLightAttenuation(const float3& attenuation) noexcept;
	const float3& getLightAttenuation() const noexcept;

	void setShadow(bool enable) noexcept;
	bool getShadow() const noexcept;

	void setSoftShadow(bool enable) noexcept;
	bool getSoftShadow() const noexcept;

	void setSubsurfaceScattering(bool enable) noexcept;
	bool getSubsurfaceScattering() const noexcept;

	void setShadowMap(GraphicsTexturePtr texture) noexcept;
	GraphicsTexturePtr getShadowMap() const noexcept;

	void setShadowSize(float size) noexcept;
	float getShadowSize() const noexcept;

	CameraPtr getShadowCamera() const noexcept;

	RenderObjectPtr clone() const noexcept;

private:
	virtual void onSceneChangeBefor() noexcept;
	virtual void onSceneChangeAfter() noexcept;

private:

	bool setupShadowMap(float size) noexcept;
	void destroyShadowMap() noexcept;

	void _updateShadow() noexcept;
	void _updateBoundingBox() noexcept;

	void onAddRenderData(RenderDataManager& manager) noexcept;

	void onRenderObjectPre(RenderPipeline& pipeline) noexcept;
	void onRenderObjectPost(RenderPipeline& pipeline) noexcept;

	void onMoveAfter() noexcept;

private:
	Light(const Light&) noexcept = delete;
	Light& operator=(const Light&) noexcept = delete;

private:

	LightType _lightType;

	float _lightIntensity;

	float3 _lightColor;
	float3 _lightAttenuation;

	float _spotInnerCone;
	float _spotOuterCone;

	bool _shadow;
	bool _shadowSoftEnable;
	bool _subsurfaceScattering;
	std::size_t _shadowSize;
	CameraPtr _shadowCamera;
	GraphicsFormat _shaodwFormat;
	GraphicsTexturePtr _shaodwMap;
	GraphicsFramebufferPtr _shaodwView;
	GraphicsFramebufferLayoutPtr _shaodwViewLayout;

	RenderSceneWeakPtr _renderScene;
};

_NAME_END

#endif