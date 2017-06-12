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
#include "ssss.h"
#include <ray/camera.h>
#include <ray/light.h>
#include <ray/material.h>

#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

SSSS::SSSS() noexcept
	: _sssStrength(1.0f)
	, _sssCorrection(1.0f)
{
}

SSSS::~SSSS() noexcept
{
}

void
SSSS::setStrength(float strength) noexcept
{
	_sssStrength = strength;
}

void
SSSS::setCorrection(float correction) noexcept
{
	_sssCorrection = correction;
}

float
SSSS::getStrength() const noexcept
{
	return _sssStrength;
}

float
SSSS::getCorrection() const noexcept
{
	return _sssCorrection;
}

void
SSSS::applyGuassBlur(RenderPipeline& pipeline, GraphicsFramebufferPtr source, const GraphicsFramebufferPtr& swap) noexcept
{
	std::uint32_t widght = source->getGraphicsFramebufferDesc().getWidth();
	std::uint32_t height = source->getGraphicsFramebufferDesc().getHeight();

	float distance = 1.0f / (pipeline.getCamera()->getFar() - pipeline.getCamera()->getNear());

	_sssSource->uniformTexture(source->getGraphicsFramebufferDesc().getColorAttachment(0).getBindingTexture());
	_sssFactor->uniform3f(_sssStrength / widght, 0.0f, _sssCorrection * distance);

	pipeline.setFramebuffer(swap);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_sssGuassBlur);

	_sssSource->uniformTexture(swap->getGraphicsFramebufferDesc().getColorAttachment(0).getBindingTexture());
	_sssFactor->uniform3f(0.0, _sssStrength / height, _sssCorrection * distance);

	pipeline.setFramebuffer(source);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_sssGuassBlur);
}

void
SSSS::onActivate(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_ssss = pipeline.createMaterial("sys:fx/ssss.fxml"); if (!_ssss) return;
	_sssGuassBlur = _ssss->getTech("blur");

	_sssFactor = _ssss->getParameter("blurFactor");
	_sssSource = _ssss->getParameter("texSource");
}

void
SSSS::onDeactivate(RenderPipeline& pipeline) noexcept
{
}

bool
SSSS::onRender(RenderPipeline& pipeline, RenderQueue queue, const GraphicsFramebufferPtr& source, const GraphicsFramebufferPtr& swap) noexcept
{
	if (queue != RenderQueue::RenderQueuePostprocess)
		return false;

	this->applyGuassBlur(pipeline, source, swap);
	return false;
}

_NAME_END