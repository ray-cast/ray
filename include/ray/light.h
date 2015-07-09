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
#ifndef _H_LIGHT_H_
#define _H_LIGHT_H_

#include <ray/render_object.h>

_NAME_BEGIN

enum LightType
{
	LT_SUN,
	LT_POINT,
	LT_SPOT,
	LT_AREA,
	LT_HEMI_SPHERE
};

class EXPORT LightListener
{
public:
	LightListener() noexcept;
	virtual ~LightListener() noexcept;

	virtual void onRenderShadow(Light* light) noexcept = 0;
};

class EXPORT Light final : public RenderObject, public RenderListener
{
public:
	Light() noexcept;
	~Light() noexcept;

	void setup() noexcept;
	void close() noexcept;

	void setIntensity(float intensity) noexcept;
	void setRange(float range) noexcept;
	void setExponent(float value) noexcept;
	void setSpotAngle(float value) noexcept;
	void setSpotInnerCone(float value) noexcept;
	void setSpotOuterCone(float value) noexcept;

	float getIntensity() const noexcept;
	float getRange() const noexcept;
	float getExponent() const noexcept;
	float getSpotAngle() const noexcept;
	float getSpotInnerCone() const noexcept;
	float getSpotOuterCone() const noexcept;

	void setLightType(LightType type) noexcept;
	void setLightListener(LightListener* listener) noexcept;
	void setLightColor(const Vector3& color) noexcept;
	void setLightDirection(const Vector3& direction) noexcept;

	const Vector3& getLightColor() const noexcept;
	const Vector3& getLightDirection() const noexcept;

	LightType getLightType() const noexcept;
	LightListener* getLightListener() const noexcept;

	void setShadow(bool enable) noexcept;
	bool getShadow() const noexcept;

	void setupShadow(std::size_t size = 512) noexcept;
	void discardShadow() noexcept;
	void setTransform(const Matrix4x4& m) noexcept;

	CameraPtr getShadowCamera() const noexcept;
	TexturePtr getShadowMap() const noexcept;

	void setRenderScene(RenderScenePtr scene) noexcept;
	RenderScenePtr getRenderScene() const noexcept;

	LightPtr clone() const noexcept;

private:

	void _updateShadow() noexcept;

	void onWillRenderObject() noexcept;
	void onRenderObject() noexcept;

private:
	Light(const Light&) noexcept = delete;
	Light& operator=(const Light&) noexcept = delete;

private:

	LightType      _lightType;
	LightListener* _lightListener;

	float _lightRange;
	float _lightIntensity;

	Vector3 _lightColor;
	Vector3 _lightDirection;

	// spot
	float _spotExponent;
	float _spotAngle;
	float _spotInnerCone;
	float _spotOuterCone;

	// shaodw
	bool _shadow;
	std::size_t _shadowSize;
	CameraPtr _shadowCamera;
	Vector3 _shadowTranslate;
	Vector3 _shadowLookAt;

	RenderScene* _renderScene;
};

_NAME_END

#endif