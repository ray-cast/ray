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
#include <ray/render_pipeline_framebuffer.h>

_NAME_BEGIN

__ImplementSubInterface(RenderPipelineFramebuffer, rtti::Interface, "RenderPipelineFramebuffer")

RenderPipelineFramebuffer::RenderPipelineFramebuffer() noexcept
{
}

RenderPipelineFramebuffer::~RenderPipelineFramebuffer() noexcept
{
}

RenderPipelineFramebuffer::RenderPipelineFramebuffer(const GraphicsFramebufferPtr& framebuffer) noexcept
	: _framebuffer(framebuffer)
{
}

void
RenderPipelineFramebuffer::setFramebuffer(GraphicsFramebufferPtr&& texture) noexcept
{
	_framebuffer = std::move(texture);
}

void
RenderPipelineFramebuffer::setFramebuffer(const GraphicsFramebufferPtr& texture) noexcept
{
	_framebuffer = texture;
}

const GraphicsFramebufferPtr&
RenderPipelineFramebuffer::getFramebuffer() const noexcept
{
	return _framebuffer;
}

void
RenderPipelineFramebuffer::onRenderPre() noexcept
{
}

void
RenderPipelineFramebuffer::onRenderPipeline(const CameraPtr& camera) noexcept
{
}

void
RenderPipelineFramebuffer::onRenderPost() noexcept
{
}

void
RenderPipelineFramebuffer::onResolutionChange() noexcept
{
}

void
RenderPipelineFramebuffer::onResolutionChangeDPI() noexcept
{
}

_NAME_END