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
	std::uint32_t probeMapSize = 32;

	GraphicsFormat probeDepthFormat;
	if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatD32_SFLOAT))
		probeDepthFormat = GraphicsFormat::GraphicsFormatD32_SFLOAT;
	else if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatX8_D24UNormPack32))
		probeDepthFormat = GraphicsFormat::GraphicsFormatX8_D24UNormPack32;
	else if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatD16UNorm))
		probeDepthFormat = GraphicsFormat::GraphicsFormatD16UNorm;
	else
		return false;

	GraphicsFormat probeColorFormat;
	GraphicsFormat probeNormalFormat;

	if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8UNorm))
		probeColorFormat = probeNormalFormat = GraphicsFormat::GraphicsFormatR8G8B8UNorm;
	else
		return false;

	GraphicsTextureDesc probeDepthDesc;
	probeDepthDesc.setWidth(probeMapSize);
	probeDepthDesc.setHeight(probeMapSize);
	probeDepthDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	probeDepthDesc.setTexFormat(probeDepthFormat);
	probeDepthDesc.setSamplerMinFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	probeDepthDesc.setSamplerMagFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_probeDepthMap = RenderSystem::instance()->createTexture(probeDepthDesc);
	if (!_probeDepthMap)
		return false;

	GraphicsTextureDesc probeColorDesc;
	probeColorDesc.setWidth(probeMapSize);
	probeColorDesc.setHeight(probeMapSize);
	probeColorDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	probeColorDesc.setTexFormat(probeColorFormat);
	probeColorDesc.setLayerNums(6);
	probeColorDesc.setSamplerMinFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	probeColorDesc.setSamplerMagFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_probeColorMap = RenderSystem::instance()->createTexture(probeColorDesc);
	if (!_probeColorMap)
		return false;

	GraphicsTextureDesc probeNormalDesc;
	probeNormalDesc.setWidth(probeMapSize);
	probeNormalDesc.setHeight(probeMapSize);
	probeNormalDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	probeNormalDesc.setTexFormat(probeNormalFormat);
	probeNormalDesc.setLayerNums(6);
	probeNormalDesc.setSamplerMinFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	probeNormalDesc.setSamplerMagFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_probeNormalMap = RenderSystem::instance()->createTexture(probeNormalDesc);
	if (!_probeNormalMap)
		return false;

	GraphicsFramebufferLayoutDesc shaodwRSMMapLayoutDesc;
	shaodwRSMMapLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, probeColorFormat));
	shaodwRSMMapLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, probeNormalFormat));
	shaodwRSMMapLayoutDesc.addComponent(GraphicsAttachmentLayout(2, GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal, probeDepthFormat));
	_probeRSMViewLayout = RenderSystem::instance()->createFramebufferLayout(shaodwRSMMapLayoutDesc);
	if (!_probeRSMViewLayout)
		return false;

	GraphicsFramebufferDesc probeRSMViewDesc;
	probeRSMViewDesc.setWidth(probeMapSize);
	probeRSMViewDesc.setHeight(probeMapSize);
	probeRSMViewDesc.addColorAttachment(GraphicsAttachmentBinding(_probeColorMap, 0, 0));
	probeRSMViewDesc.addColorAttachment(GraphicsAttachmentBinding(_probeNormalMap, 0, 0));
	probeRSMViewDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_probeDepthMap, 0, 0));
	probeRSMViewDesc.setGraphicsFramebufferLayout(_probeRSMViewLayout);
	_probeRSMView = RenderSystem::instance()->createFramebuffer(probeRSMViewDesc);
	if (!_probeRSMView)
		return false;

	this->setFramebuffer(_probeRSMView);
	return true;
}

_NAME_END