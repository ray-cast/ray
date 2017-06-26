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
#ifndef _H_DEPTH_OF_FIELD_H_
#define _H_DEPTH_OF_FIELD_H_

#include <ray/render_post_process.h>

_NAME_BEGIN

class DepthOfField : public RenderPostProcess
{
public:
	DepthOfField() noexcept;
	~DepthOfField() noexcept;

	virtual void onActivate(RenderPipeline& pipeline) noexcept;
	virtual void onDeactivate(RenderPipeline& pipeline) noexcept;

	bool onRender(RenderPipeline& pipeline, RenderQueue queue, const GraphicsFramebufferPtr& source, const GraphicsFramebufferPtr& swap) noexcept;

	void blurh(RenderPipelineDevice& pipeline, const GraphicsFramebufferPtr& source, const GraphicsFramebufferPtr& swap) noexcept;
	void blurv(RenderPipelineDevice& pipeline, const GraphicsFramebufferPtr& source, const GraphicsFramebufferPtr& swap) noexcept;

	void computeNear(RenderPipelineDevice& pipeline, GraphicsFramebufferPtr shrunk, GraphicsFramebufferPtr blured, GraphicsFramebufferPtr dest) noexcept;

	void shading(RenderPipelineDevice& pipeline, GraphicsFramebufferPtr color, GraphicsTexturePtr texSmall, GraphicsTexturePtr large);

private:

	MaterialPtr _dof;
	MaterialTechPtr _sample4;
	MaterialTechPtr _blurv;
	MaterialTechPtr _blurh;
	MaterialTechPtr _computeNear;
	MaterialTechPtr _final;

	MaterialParamPtr _texColor;
	MaterialParamPtr _texDepth;
	MaterialParamPtr _texShrunk;
	MaterialParamPtr _texBlured;
	MaterialParamPtr _texSmall;
	MaterialParamPtr _texLarge;

	GraphicsTexturePtr _texTempMap;
	GraphicsTexturePtr _texBlurMap;

	GraphicsTexturePtr _texTemp;
	GraphicsTexturePtr _texBlur;
};

_NAME_END

#endif