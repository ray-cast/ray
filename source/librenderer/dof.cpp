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
#include "dof.h"
#include <ray/material.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

DepthOfField::DepthOfField() noexcept
{
}

DepthOfField::~DepthOfField() noexcept
{
}

void
DepthOfField::onActivate(RenderPipeline& pipeline) noexcept
{
	_dof = pipeline.createMaterial("sys:fx/dof.glsl");

	_sample4 = _dof->getTech("sample");
	_blurh = _dof->getTech("blurh");
	_blurv = _dof->getTech("blurv");
	_computeNear = _dof->getTech("computeNear");
	_final = _dof->getTech("final");

	_texTemp = pipeline.createTexture(512, 512, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_texBlur = pipeline.createTexture(512, 512, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);

	_texColor = _dof->getParameter("texColor");
	_texShrunk = _dof->getParameter("texShrunk");
	_texBlured = _dof->getParameter("texBlured");
	_texSmall = _dof->getParameter("texSmall");
	_texLarge = _dof->getParameter("texLarge");
}

void
DepthOfField::onDeactivate(RenderPipeline& pipeline) noexcept
{
}

void
DepthOfField::blurh(RenderPipelineDevice& pipeline, const GraphicsFramebufferPtr& source, const GraphicsFramebufferPtr& swap) noexcept
{
	/*_texColor->assign(source->getResolveTexture());

	pipeline.setFramebuffer(dest);
	pipeline.drawScreenQuad(_blurh);*/
}

void
DepthOfField::blurv(RenderPipelineDevice& pipeline, const GraphicsFramebufferPtr& source, const GraphicsFramebufferPtr& swap) noexcept
{
	/*_texColor->assign(source->getResolveTexture());

	pipeline.setFramebuffer(dest);
	pipeline.drawScreenQuad(_blurv);*/
}

void
DepthOfField::computeNear(RenderPipelineDevice& pipeline, GraphicsFramebufferPtr source, GraphicsFramebufferPtr blured, GraphicsFramebufferPtr dest) noexcept
{
	/*_texShrunk->assign(source->getResolveTexture());
	_texBlured->assign(blured->getResolveTexture());

	pipeline.setFramebuffer(dest);
	pipeline.drawScreenQuad(_computeNear);*/
}

void
DepthOfField::shading(RenderPipelineDevice& pipeline, GraphicsFramebufferPtr color, GraphicsTexturePtr texSmall, GraphicsTexturePtr large)
{
	/*_texColor->assign(color->getGraphicsFramebufferDesc().getTextures()[0]);
	_texSmall->assign(texSmall);
	_texLarge->assign(large);

	pipeline.setFramebuffer(color);
	pipeline.drawScreenQuad(_final);*/
}

bool
DepthOfField::onRender(RenderPipeline& pipeline, RenderQueue queue, const GraphicsFramebufferPtr& source, const GraphicsFramebufferPtr& swap) noexcept
{
	/*_texColor->assign(source->getGraphicsFramebufferDesc().getTextures()[0]);

	pipeline.setFramebuffer(_texBlur);
	pipeline.drawScreenQuad(_sample4);

	this->blurh(pipeline, _texBlur, _texTemp);
	this->blurv(pipeline, _texTemp, _texBlur);

	this->computeNear(pipeline, source, _texBlur, _texTemp);

	this->shading(pipeline, source, _texTemp, _texBlur);*/
	return false;
}

_NAME_END