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
#include "shadow_render_framebuffer.h"
#include <ray/render_system.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>

_NAME_BEGIN

__ImplementSubInterface(ShadowRenderFramebuffer, RenderPipelineFramebuffer, "ShadowRenderFramebuffer")

ShadowRenderFramebuffer::ShadowRenderFramebuffer() noexcept
{
}

ShadowRenderFramebuffer::~ShadowRenderFramebuffer() noexcept
{
}

bool
ShadowRenderFramebuffer::setup()
{
	std::uint32_t shadowMapSize = 0;

	ShadowQuality shadowQuality = RenderSystem::instance()->getRenderSetting().shadowQuality;
	if (shadowQuality == ShadowQuality::ShadowQualityLow)
		shadowMapSize = LightShadowSize::LightShadowSizeLow;
	else if (shadowQuality == ShadowQuality::ShadowQualityMedium)
		shadowMapSize = LightShadowSize::LightShadowSizeMedium;
	else if (shadowQuality == ShadowQuality::ShadowQualityHigh)
		shadowMapSize = LightShadowSize::LightShadowSizeHigh;
	else if (shadowQuality == ShadowQuality::ShadowQualityVeryHigh)
		shadowMapSize = LightShadowSize::LightShadowSizeVeryHigh;
	else
		return false;

	GraphicsFormat depthLinearFormat;
	if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatR32SFloat))
		depthLinearFormat = GraphicsFormat::GraphicsFormatR32SFloat;
	else if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		depthLinearFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	GraphicsTextureDesc depthLinearDesc;
	depthLinearDesc.setWidth(shadowMapSize);
	depthLinearDesc.setHeight(shadowMapSize);
	depthLinearDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	depthLinearDesc.setTexFormat(depthLinearFormat);
	depthLinearDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_shadowDepthLinearMap = RenderSystem::instance()->createTexture(depthLinearDesc);
	if (!_shadowDepthLinearMap)
		return false;

	GraphicsFramebufferLayoutDesc shaodwMapLayoutDesc;
	shaodwMapLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, depthLinearFormat));
	_shadowDepthLinearViewLayout = RenderSystem::instance()->createFramebufferLayout(shaodwMapLayoutDesc);
	if (!_shadowDepthLinearViewLayout)
		return false;

	GraphicsFramebufferDesc shadowViewDesc;
	shadowViewDesc.setWidth(shadowMapSize);
	shadowViewDesc.setHeight(shadowMapSize);
	shadowViewDesc.addColorAttachment(GraphicsAttachmentBinding(_shadowDepthLinearMap, 0, 0));
	shadowViewDesc.setGraphicsFramebufferLayout(_shadowDepthLinearViewLayout);
	_shadowDepthLinearView = RenderSystem::instance()->createFramebuffer(shadowViewDesc);
	if (!_shadowDepthLinearView)
		return false;

	this->setFramebuffer(_shadowDepthLinearView);
	return true;
}

void
ShadowRenderFramebuffer::onRenderBefore() noexcept
{
}

void
ShadowRenderFramebuffer::onRenderPipeline(const CameraPtr& camera) noexcept
{
}

void
ShadowRenderFramebuffer::onRenderAfter() noexcept
{
}

void
ShadowRenderFramebuffer::onResolutionChange() noexcept
{
}

void
ShadowRenderFramebuffer::onResolutionChangeDPI() noexcept
{
}

_NAME_END