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
#include <ray/light_probe_render_framebuffer.h>
#include <ray/render_system.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>

_NAME_BEGIN

__ImplementSubInterface(LightProbeRenderFramebuffer, RenderPipelineFramebuffer, "LightProbeRenderFramebuffer")

LightProbeRenderFramebuffer::LightProbeRenderFramebuffer() noexcept
{
}

LightProbeRenderFramebuffer::~LightProbeRenderFramebuffer() noexcept
{
}

bool
LightProbeRenderFramebuffer::setup()
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

	GraphicsFormat shadowDepthFormat;
	if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatD32_SFLOAT))
		shadowDepthFormat = GraphicsFormat::GraphicsFormatD32_SFLOAT;
	else if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatX8_D24UNormPack32))
		shadowDepthFormat = GraphicsFormat::GraphicsFormatX8_D24UNormPack32;
	else if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatD16UNorm))
		shadowDepthFormat = GraphicsFormat::GraphicsFormatD16UNorm;
	else
		return false;

	GraphicsFormat shadowColorFormat;
	GraphicsFormat shadowNormalFormat;

	if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		shadowColorFormat = shadowNormalFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	GraphicsTextureDesc shadowDepthDesc;
	shadowDepthDesc.setWidth(shadowMapSize);
	shadowDepthDesc.setHeight(shadowMapSize);
	shadowDepthDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	shadowDepthDesc.setTexFormat(shadowDepthFormat);
	shadowDepthDesc.setSamplerMinFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	shadowDepthDesc.setSamplerMagFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_shadowDepthMap = RenderSystem::instance()->createTexture(shadowDepthDesc);
	if (!_shadowDepthMap)
		return false;

	GraphicsTextureDesc shadowColorDesc;
	shadowColorDesc.setWidth(shadowMapSize);
	shadowColorDesc.setHeight(shadowMapSize);
	shadowColorDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	shadowColorDesc.setTexFormat(shadowColorFormat);
	shadowColorDesc.setSamplerMinFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	shadowColorDesc.setSamplerMagFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_shadowColorMap = RenderSystem::instance()->createTexture(shadowColorDesc);
	if (!_shadowColorMap)
		return false;

	GraphicsTextureDesc shadowNormalDesc;
	shadowNormalDesc.setWidth(shadowMapSize);
	shadowNormalDesc.setHeight(shadowMapSize);
	shadowNormalDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	shadowNormalDesc.setTexFormat(shadowNormalFormat);
	shadowNormalDesc.setSamplerMinFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	shadowNormalDesc.setSamplerMagFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_shadowNormalMap = RenderSystem::instance()->createTexture(shadowNormalDesc);
	if (!_shadowNormalMap)
		return false;

	GraphicsFramebufferLayoutDesc shaodwRSMMapLayoutDesc;
	shaodwRSMMapLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, shadowColorFormat));
	shaodwRSMMapLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, shadowNormalFormat));
	shaodwRSMMapLayoutDesc.addComponent(GraphicsAttachmentLayout(2, GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal, shadowDepthFormat));
	_shadowRSMViewLayout = RenderSystem::instance()->createFramebufferLayout(shaodwRSMMapLayoutDesc);
	if (!_shadowRSMViewLayout)
		return false;

	GraphicsFramebufferDesc shadowRSMViewDesc;
	shadowRSMViewDesc.setWidth(shadowMapSize);
	shadowRSMViewDesc.setHeight(shadowMapSize);
	shadowRSMViewDesc.addColorAttachment(GraphicsAttachmentBinding(_shadowColorMap, 0, 0));
	shadowRSMViewDesc.addColorAttachment(GraphicsAttachmentBinding(_shadowNormalMap, 0, 0));
	shadowRSMViewDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_shadowDepthMap, 0, 0));
	shadowRSMViewDesc.setGraphicsFramebufferLayout(_shadowRSMViewLayout);
	_shadowRSMView = RenderSystem::instance()->createFramebuffer(shadowRSMViewDesc);
	if (!_shadowRSMView)
		return false;

	this->setFramebuffer(_shadowRSMView);
	return true;
}

_NAME_END