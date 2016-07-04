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
#include "color_grading.h"
#include <ray/material.h>
#include <ray/graphics_framebuffer.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

ColorGrading::ColorGrading() noexcept
	: _enableGammaGrading(true)
	, _enableColorGrading(false)
{
}

ColorGrading::~ColorGrading() noexcept
{
}

void
ColorGrading::setGammaGrading(bool enable) noexcept
{
	_enableGammaGrading = enable;
}

void
ColorGrading::setColorGrading(bool enable) noexcept
{
	_enableColorGrading = enable;
}

bool
ColorGrading::getGammaGrading() const noexcept
{
	return _enableGammaGrading;
}

bool
ColorGrading::getColorGrading() const noexcept
{
	return _enableColorGrading;
}

void
ColorGrading::onActivate(RenderPipeline& pipeline) noexcept
{
	_material = pipeline.createMaterial("sys:fx/color_grading.fxml");
	_colorGrading = _material->getTech("ColorGrading");
	_texGrading = _material->getParameter("texColorGrading");
	_texSource = _material->getParameter("texSource");
	_gammGrading = _material->getParameter("colorGrading");

	if (_enableColorGrading)
	{
		if (pipeline.isTextureDimSupport(GraphicsTextureDim::GraphicsTextureDim3D))
			_texColorGrading = pipeline.createTexture("sys:media/color_grading.dds", GraphicsTextureDim::GraphicsTextureDim3D);
	}	

	_texGrading->uniformTexture(_texColorGrading);
}

void
ColorGrading::onDeactivate(RenderPipeline& pipeline) noexcept
{
}

bool
ColorGrading::onRender(RenderPipeline& pipeline, RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr swap) noexcept
{
	if (queue != RenderQueue::RenderQueuePostprocess)
		return false;

	_gammGrading->uniform2i(int2(_enableGammaGrading, _enableColorGrading));
	_texSource->uniformTexture(source->getGraphicsFramebufferDesc().getColorAttachment(0).getBindingTexture());

	pipeline.setFramebuffer(swap);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_colorGrading);

	return true;
}

_NAME_END