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
		float blurRadius;
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
	void computeRawAO(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept;
	void blurHorizontal(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept;
	void blurVertical(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept;

	void createSphereNoise() noexcept;

private:
	void onActivate(RenderPipeline& pipeline) noexcept;
	void onDeactivate(RenderPipeline& pipeline) noexcept;

	bool onRender(RenderPipeline& pipeline, RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr& dest) noexcept;

private:

	Setting _setting;

	MaterialPtr _ambientOcclusion;

	MaterialTechPtr _ambientOcclusionPass;
	MaterialTechPtr _ambientOcclusionBlurXPass;
	MaterialTechPtr _ambientOcclusionBlurYPass;

	MaterialParamPtr _cameraProjScale;

	MaterialParamPtr _occlusionRadius;
	MaterialParamPtr _occlusionRadius2;
	MaterialParamPtr _occlusionBias;
	MaterialParamPtr _occlusionIntensity;
	MaterialParamPtr _occlusionAmbient;
	MaterialParamPtr _occlusionSphere;
	MaterialMacroPtr _occlusionSampleNumber;

	MaterialParamPtr _blurSource;
	MaterialParamPtr _blurFactor;
	MaterialParamPtr _blurSharpness;
	MaterialParamPtr _blurDirection;
	MaterialParamPtr _blurGaussian;
	MaterialMacroPtr _blurRadius;

	GraphicsTexturePtr _texBlurMap;
	GraphicsTexturePtr _texAmbientMap;

	GraphicsFramebufferLayoutPtr _framebufferLayout;

	GraphicsFramebufferPtr _texBlurView;
	GraphicsFramebufferPtr _texAmbientView;
};

_NAME_END

#endif