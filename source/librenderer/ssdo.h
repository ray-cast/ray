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
#ifndef _H_SSDO_H_
#define _H_SSDO_H_

#include <ray/render_post_process.h>

_NAME_BEGIN

class SSDO : public RenderPostProcess
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
	SSDO() noexcept;
	~SSDO() noexcept;

	void setSetting(const Setting& set) noexcept;
	const Setting& getSetting() const noexcept;

private:
	void computeRawAO(RenderPipeline& pipeline, const GraphicsTexturePtr& source, const GraphicsFramebufferPtr& dest) noexcept;
	void blurHorizontal(RenderPipeline& pipeline, const GraphicsTexturePtr& source, const GraphicsFramebufferPtr& dest) noexcept;
	void blurVertical(RenderPipeline& pipeline, const GraphicsTexturePtr& source, const GraphicsFramebufferPtr& dest) noexcept;
	void applySSDO(RenderPipeline& pipeline, const GraphicsTexturePtr& source, const GraphicsFramebufferPtr& dest) noexcept;

private:
	void onActivate(RenderPipeline& pipeline) except;
	void onDeactivate(RenderPipeline& pipeline) noexcept;

	bool onRender(RenderPipeline& pipeline, RenderQueue queue, const GraphicsFramebufferPtr& source, const GraphicsFramebufferPtr& swap) noexcept;

private:

	Setting _setting;

	MaterialPtr _ambientOcclusion;

	MaterialTechPtr _ambientOcclusionPass;
	MaterialTechPtr _ambientOcclusionBlurPass;
	MaterialTechPtr _ambientOcclusionApply;

	MaterialParamPtr _occlusionSourceInv;
	MaterialParamPtr _occlusionProjectConstant;
	MaterialMacroPtr _occlusionSampleNumber;
	MaterialParamPtr _occlusionParams1;
	MaterialParamPtr _occlusionParams2;

	MaterialParamPtr _blurSource;
	MaterialParamPtr _blurFactor;
	MaterialParamPtr _blurSharpness;
	MaterialParamPtr _blurDirection;
	MaterialMacroPtr _blurRadius;

	GraphicsTexturePtr _texAmbientMap;
	GraphicsTexturePtr _texAmbientTempMap;

	GraphicsFramebufferLayoutPtr _framebufferLayout;

	GraphicsFramebufferPtr _texAmbientView;
	GraphicsFramebufferPtr _texAmbientTempView;
};

_NAME_END

#endif