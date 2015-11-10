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
#include <ray/dof.h>
#include <ray/material_maker.h>
#include <ray/render_factory.h>
#include <ray/render_texture.h>

_NAME_BEGIN

DepthOfField::DepthOfField() noexcept
{
	_dof = MaterialMaker("sys:fx/dof.glsl");

	_sample4 = _dof->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("sample");
	_blurh = _dof->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("blurh");
	_blurv = _dof->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("blurv");
	_computeNear = _dof->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("computeNear");
	_final = _dof->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("final");

	_texTemp = RenderFactory::createRenderTexture();
	_texTemp->setup(512, 512, TextureDim::DIM_2D, PixelFormat::R8G8B8);

	_texBlur = RenderFactory::createRenderTexture();
	_texBlur->setup(512, 512, TextureDim::DIM_2D, PixelFormat::R8G8B8);

	_texColor = _dof->getParameter("texColor");
	_texShrunk = _dof->getParameter("texShrunk");
	_texBlured = _dof->getParameter("texBlured");
	_texSmall = _dof->getParameter("texSmall");
	_texLarge = _dof->getParameter("texLarge");
}

DepthOfField::~DepthOfField() noexcept
{
}

void
DepthOfField::blurh(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
	_texColor->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.drawSceneQuad(_blurh);
}

void
DepthOfField::blurv(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
	_texColor->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.drawSceneQuad(_blurv);
}

void
DepthOfField::computeNear(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr blured, RenderTexturePtr dest) noexcept
{
	_texShrunk->assign(source->getResolveTexture());
	_texBlured->assign(blured->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.drawSceneQuad(_computeNear);
}

void
DepthOfField::shading(RenderPipeline& pipeline, RenderTexturePtr color, RenderTexturePtr texSmall, RenderTexturePtr large)
{
	_texColor->assign(color->getResolveTexture());
	_texSmall->assign(texSmall->getResolveTexture());
	_texLarge->assign(large->getResolveTexture());

	pipeline.setRenderTexture(color);
	pipeline.drawSceneQuad(_final);
}

void
DepthOfField::onRender(RenderPipeline& pipeline, RenderTexturePtr source) noexcept
{
	_texColor->assign(source->getResolveTexture());

	pipeline.setRenderTexture(_texBlur);
	pipeline.drawSceneQuad(_sample4);

	this->blurh(pipeline, _texBlur, _texTemp);
	this->blurv(pipeline, _texTemp, _texBlur);

	this->computeNear(pipeline, source, _texBlur, _texTemp);

	this->shading(pipeline, source, _texTemp, _texBlur);
}

_NAME_END