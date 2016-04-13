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
#ifndef _H_SSSS_H_
#define _H_SSSS_H_

#include <ray/render_post_process.h>

_NAME_BEGIN

class SSSS : public RenderPostProcess
{
	__DeclareSubClass(SSSS, RenderPostProcess)
public:
	SSSS() noexcept;
	~SSSS() noexcept;

	void blurX(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept;
	void blurY(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept;

	void translucency(RenderPipeline& pipeline, LightPtr light, GraphicsTexturePtr shaodwmap, GraphicsFramebufferPtr dest) noexcept;
	void translucency(RenderPipeline& pipeline, GraphicsFramebufferPtr dest) noexcept;

private:

	void onActivate(RenderPipeline& pipeline) noexcept;
	void onDeactivate(RenderPipeline& pipeline) noexcept;

	bool onRender(RenderPipeline& pipeline, GraphicsFramebufferPtr source, GraphicsFramebufferPtr dest) noexcept;

private:

	float _sssScale;
	float _sssStrength;
	float _sssWidth;
	float _gaussianWidth;

	MaterialPtr _material;
	MaterialTechPtr _translucency;
	MaterialTechPtr _blur;

	MaterialParamPtr _sssStep;
	MaterialParamPtr _sssCorrection;
	MaterialParamPtr _sssSource;

	MaterialParamPtr _texDepthLinear;

	MaterialParamPtr _eyeProjInfo;

	MaterialParamPtr _lightColor;
	MaterialParamPtr _lightEyePosition;
	
	MaterialParamPtr _shadowMap;
	MaterialParamPtr _shadowFactor;
	MaterialParamPtr _shadowEye2LightView;
	MaterialParamPtr _shadowEye2LightViewProject;

	GraphicsTexturePtr _SSSSMap;
	GraphicsFramebufferPtr _SSSSView;
	GraphicsFramebufferLayoutPtr _SSSSViewLayout;
};

_NAME_END

#endif