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

class EXPORT FimicToneMapping final : public RenderPostProcess
{
public:
	struct EXPORT Setting
	{
		float bloomThreshold;
		float bloomIntensity;

		float exposure;

		Setting() noexcept;
	};

public:
	FimicToneMapping() noexcept;
	~FimicToneMapping() noexcept;

	void setSetting(const Setting& setting) noexcept;
	const Setting& getSetting() const noexcept;

private:
	void sumLum(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest, std::uint8_t level) noexcept;
	void sumLumLog(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept;
	void avgLuminance(RenderPipeline& pipeline, GraphicsTexturePtr adaptedLum, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept;

	void generateBloom(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept;
	void generateToneMapping(RenderPipeline& pipeline, GraphicsTexturePtr bloom, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept;

	void blurh(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest, std::uint32_t level) noexcept;
	void blurv(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest, std::uint32_t level) noexcept;
	void bloomCombine(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept;

private:

	void onActivate(RenderPipeline& pipeline) noexcept;
	void onDeactivate(RenderPipeline& pipeline) noexcept;

	bool onRender(RenderPipeline& pipeline, RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr swap) noexcept;

private:

	TimerPtr _timer;

	Setting _setting;

	MaterialPtr _fimic;
	MaterialTechPtr _sumLum;
	MaterialTechPtr _sumLumLog;
	MaterialTechPtr _avgLuminance;
	MaterialTechPtr _bloom;
	MaterialTechPtr _bloomCombine;
	MaterialTechPtr _blur;
	MaterialTechPtr _tone;

	MaterialParamPtr _bloomThreshold;
	MaterialParamPtr _bloomIntensity;
	MaterialParamPtr _bloomWeights;
	MaterialParamPtr _bloomFactors;

	MaterialParamPtr _toneBloom;
	MaterialParamPtr _toneLumAve;
	MaterialParamPtr _toneLumKey;
	MaterialParamPtr _toneLumExposure;
	MaterialParamPtr _toneBurnout;
	MaterialParamPtr _toneDefocus;
	MaterialParamPtr _delta;
	MaterialParamPtr _texLumAve;
	MaterialParamPtr _texSource;
	MaterialParamPtr _texSourceLevel;
	MaterialParamPtr _texSourceSizeInv;

	GraphicsTexturePtr _texBloomMap;
	GraphicsTexturePtr _texBloomTempMap;

	GraphicsTexturePtr _texSampleLogMap;
	GraphicsTexturePtr _texSampleLumMap;

	GraphicsFramebufferLayoutPtr _sampleBloomImageLayout;
	GraphicsFramebufferLayoutPtr _sampleLogImageLayout;

	GraphicsFramebuffers _texBloomView;
	GraphicsFramebuffers _texBloomTempView;

	GraphicsFramebufferPtr _texSampleLogView;
	GraphicsFramebufferPtr _texSampleLumView;
};

_NAME_END

#endif