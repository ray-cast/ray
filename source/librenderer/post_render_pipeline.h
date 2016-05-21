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
#ifndef _H_POST_RENDER_PIPELINE_H_
#define _H_POST_RENDER_PIPELINE_H_

#include <ray/render_post_process.h>

_NAME_BEGIN

class EXPORT PostRenderPipeline final : public RenderPostProcess
{
public:
	PostRenderPipeline() noexcept;
	~PostRenderPipeline() noexcept;

	bool enableSSSS(bool enable) noexcept;
	bool isEnableSSSS() const noexcept;

public:
	virtual void onActivate(RenderPipeline& pipeline) noexcept;
	virtual void onDeactivate(RenderPipeline& pipeline) noexcept;

	virtual void onResolutionChangeBefore(RenderPipeline& pipeline) noexcept;
	virtual void onResolutionChangeAfter(RenderPipeline& pipeline) noexcept;

	virtual void onRenderPre(RenderPipeline& pipeline) noexcept;
	virtual void onRenderPost(RenderPipeline& pipeline) noexcept;

	virtual bool onRender(RenderPipeline& pipeline, RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr& swap) noexcept;

private:
	bool _enableSSSS;

	std::shared_ptr<class SSSS> _SSSS;

	GraphicsTexturePtr _deferredDepthMap;
	GraphicsTexturePtr _deferredDepthLinearMap;
	GraphicsTexturePtr _deferredGraphicMap;
	GraphicsTexturePtr _deferredNormalMap;
	GraphicsTexturePtr _deferredLightMap;
	GraphicsTexturePtr _deferredShadowMap;

	GraphicsTexturePtr _swapMap;
	GraphicsFramebufferPtr _swapView;
};

_NAME_END

#endif