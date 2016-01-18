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
#ifndef _H_SSAO_H_
#define _H_SSAO_H_

#include <ray/render_post_process.h>

_NAME_BEGIN

struct int16x4
{
	int16x4()
	{
	}

	int16x4(float4 F)
	{
		x = FP32ToInt16SNORM(F.x);
		y = FP32ToInt16SNORM(F.y);
		z = FP32ToInt16SNORM(F.z);
		w = FP32ToInt16SNORM(F.w);
	}

	std::int16_t FP32ToInt16SNORM(float f)
	{
		return (std::int16_t)((1 << 15) * f);
	}

	std::int16_t x, y, z, w;
};

class SSAO : public RenderPostProcess
{
public:
	struct Setting
	{
		float radius;
		float bias;
		float intensity;

		bool blur;
		int blurRadius;
		float blurScale;
		float blurSharpness;

		Setting() noexcept;
	};

public:
	SSAO() noexcept;
	~SSAO() noexcept;

	void setSetting(const Setting& set) noexcept;
	const Setting& getSetting() const noexcept;

private:

	void computeRawAO(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept;
	void blurHorizontal(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept;
	void blurVertical(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept;
	void blurDirection(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest, const float2& direction) noexcept;
	void shading(RenderPipeline& pipeline, GraphicsRenderTexturePtr ambient, GraphicsRenderTexturePtr dest) noexcept;

	void createSphereNoise();

private:

	void onActivate(RenderPipeline& pipeline) except;
	void onDeactivate(RenderPipeline& pipeline) except;

	void onRender(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) except;

private:

	Setting _setting;

	MaterialPtr _ambientOcclusion;

	MaterialPassPtr _ambientOcclusionPass;
	MaterialPassPtr _ambientOcclusionBlurPass;
	MaterialPassPtr _ambientOcclusionCopyPass;

	MaterialParamPtr _cameraProjScale;
	MaterialParamPtr _cameraProjInfo;

	MaterialParamPtr _occlusionRadius;
	MaterialParamPtr _occlusionRadius2;
	MaterialParamPtr _occlusionBias;
	MaterialParamPtr _occlusionIntensity;
	MaterialParamPtr _occlusionAmbient;
	MaterialParamPtr _occlusionSphere;

	MaterialParamPtr _blurSource;
	MaterialParamPtr _blurFactor;
	MaterialParamPtr _blurSharpness;
	MaterialParamPtr _blurDirection;
	MaterialParamPtr _blurGaussian;

	MaterialVariantPtr _sampleNumber;
	MaterialVariantPtr _blurRadius;

	GraphicsRenderTexturePtr _texBlur;
	GraphicsRenderTexturePtr _texAmbient;
};

_NAME_END

#endif