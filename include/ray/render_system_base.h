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
#ifndef _H_RENDER_SYSTEM_BASE_H_
#define _H_RENDER_SYSTEM_BASE_H_

#include <ray/render_pipeline_base.h>
#include <ray/render_setting.h>

_NAME_BEGIN

class EXPORT RenderSystem
{
public:
	RenderSystem() noexcept;
	virtual ~RenderSystem() noexcept;

	virtual void open(WindHandle window, std::size_t w, std::size_t h) except = 0;
	virtual void close() noexcept = 0;

	virtual void setRenderPipeline(RenderPipelinePtr pipeline) except = 0;
	virtual RenderPipelinePtr getRenderPipeline() const noexcept = 0;

	virtual void setRenderSetting(const RenderSetting& setting) except = 0;
	virtual const RenderSetting& getRenderSetting() const noexcept = 0;

	virtual void setWireframeMode(bool enable) noexcept = 0;
	virtual bool getWireframeMode() const noexcept = 0;

	virtual void setWindowResolution(std::size_t w, std::size_t h) except = 0;
	virtual void getWindowResolution(std::size_t& w, std::size_t& h) const noexcept = 0;

	virtual void setSwapInterval(SwapInterval interval) except = 0;
	virtual SwapInterval getSwapInterval() const noexcept = 0;

	virtual bool addRenderScene(RenderScenePtr scene) noexcept = 0;
	virtual void removeRenderScene(RenderScenePtr scene) noexcept = 0;

	virtual void renderBegin() noexcept = 0;
	virtual void renderScene() noexcept = 0;
	virtual void renderEnd() noexcept = 0;

private:
	RenderSystem(const RenderSystem&) noexcept = delete;
	RenderSystem& operator=(const RenderSystem&) noexcept = delete;
};

_NAME_END

#endif