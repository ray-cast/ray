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
#ifndef _H_FIMIC_H_
#define _H_FIMIC_H_

#include <ray/render_post_process.h>
#include <ray/timer.h>

_NAME_BEGIN

#define SAMPLE_COUNT 6
#define SAMPLE_LOG_SIZE 32
#define SAMPLE_LOG_COUNT SAMPLE_LOG_SIZE * SAMPLE_LOG_SIZE

class EXPORT FimicToneMapping final : public RenderPostProcess
{
public:
	struct EXPORT Setting
	{
		float bloomThreshold;
		float bloomIntensity;

		float lumKey;
		float lumDelta;
		float lumExposure;

		Setting() noexcept;
	};

public:
	FimicToneMapping() noexcept;
	~FimicToneMapping() noexcept;

	void setSetting(const Setting& setting) noexcept;
	const Setting& getSetting() const noexcept;

private:

	void sunLum(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept;
	void sunLumLog(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept;

	void measureLuminance(RenderPipeline& pipeline, GraphicsRenderTexturePtr source) noexcept;

	void generateBloom(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept;
	void generateToneMapping(RenderPipeline& pipeline, GraphicsTexturePtr bloom, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept;

	void blurh(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept;
	void blurv(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept;

private:

	void onActivate(RenderPipeline& pipeline) except;
	void onDeactivate(RenderPipeline& pipeline) except;

	void onRender(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept;

private:

	float _lumAdapt;

	TimerPtr _timer;

	Setting _setting;

	MaterialPtr _fimic;
	MaterialPassPtr _sunLum;
	MaterialPassPtr _sunLumLog;
	MaterialPassPtr _bloom;
	MaterialPassPtr _blurh;
	MaterialPassPtr _blurv;
	MaterialPassPtr _tone;

	MaterialParamPtr _bloomThreshold;
	MaterialParamPtr _bloomIntensity;
	MaterialParamPtr _bloomTexSizeInv;

	MaterialParamPtr _toneSource;
	MaterialParamPtr _toneBloom;
	MaterialParamPtr _toneLumAve;
	MaterialParamPtr _toneLumKey;
	MaterialParamPtr _toneLumExposure;
	MaterialParamPtr _toneBurnout;
	MaterialParamPtr _toneDefocus;

	GraphicsTexturePtr _texBloomMap[SAMPLE_COUNT];
	GraphicsTexturePtr _texSample4Map;
	GraphicsTexturePtr _texSample8Map;
	GraphicsTexturePtr _texSampleLogMap;
	GraphicsTexturePtr _texCombieMap;

	GraphicsRenderTexturePtr _texBloomView[SAMPLE_COUNT];
	GraphicsRenderTexturePtr _texSample4View;
	GraphicsRenderTexturePtr _texSample8View;
	GraphicsRenderTexturePtr _texSampleLogView;
	GraphicsRenderTexturePtr _texCombieView;
};

_NAME_END

#endif