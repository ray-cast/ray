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
#include <ray/ssgi.h>
#include <ray/camera.h>
#include <ray/render_texture.h>

_NAME_BEGIN

SSGI::SSGI() noexcept
{
}

SSGI::~SSGI() noexcept
{
}

void
SSGI::setSetting(const Setting& set) noexcept
{
	_setting.radius = set.radius;
	_setting.bias = set.bias;
	_setting.intensity = set.intensity;
	_setting.blur = set.blur;
	_setting.blurRadius = set.blurRadius;
	_setting.blurScale = set.blurScale;
	_setting.blurSharpness = set.blurSharpness;

	_radius->assign(_setting.radius);
	_radius2->assign(_setting.radius * _setting.radius);

	_bias->assign(_setting.bias);
	_intensityDivR6->assign(_setting.intensity);

	_blurRadius->assign(_setting.blurRadius);
	_blurFactor->assign((float)1.0 / (2 * ((_setting.blurRadius + 1) / 2) * ((_setting.blurRadius + 1) / 2)));
	_blurSharpness->assign(_setting.blurSharpness);
}

const SSGI::Setting&
SSGI::getSetting() const noexcept
{
	return _setting;
}

void
SSGI::computeRawAO(RenderPipeline& pipeline, RenderTexturePtr dest) noexcept
{
	_projInfo->assign(pipeline.getCamera()->getProjConstant());
	_projScale->assign(pipeline.getCamera()->getProjLength().y * _setting.radius);
	_clipInfo->assign(pipeline.getCamera()->getClipConstant());

	pipeline.setRenderTexture(dest);
	pipeline.drawSceneQuad(_ambientOcclusionPass);
}

void
SSGI::blurHorizontal(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
	float2 direction(_setting.blurScale, 0.0f);
	direction.x /= source->getWidth();

	this->blurDirection(pipeline, source, dest, direction);
}

void
SSGI::blurVertical(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
	float2 direction(0.0f, _setting.blurScale);
	direction.y /= source->getHeight();

	this->blurDirection(pipeline, source, dest, direction);
}

void
SSGI::blurDirection(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest, const float2& direction) noexcept
{
	_blurDirection->assign(direction);
	_blurTexSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.drawSceneQuad(_ambientOcclusionBlurPass);
}

void
SSGI::shading(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr ao) noexcept
{
	_copyAmbient->assign(ao->getResolveTexture());

	pipeline.setRenderTexture(source);
	pipeline.drawSceneQuad(_ambientOcclusionCopyPass);
}

void
SSGI::onActivate(RenderPipeline& pipeline) except
{
	_texAmbient = pipeline.createRenderTexture();
	_texAmbient->setup(1376, 768, TextureDim::DIM_2D, TextureFormat::R16G16B16A16F);

	_texBlur = pipeline.createRenderTexture();
	_texBlur->setup(1376, 768, TextureDim::DIM_2D, TextureFormat::R16G16B16A16F);

	_ambientOcclusion = pipeline.createMaterial("sys:fx\\ssgi.glsl");
	_ambientOcclusionPass = _ambientOcclusion->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("ao");
	_ambientOcclusionBlurPass = _ambientOcclusion->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("blur");
	_ambientOcclusionCopyPass = _ambientOcclusion->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("copy");

	_radius = _ambientOcclusion->getParameter("radius");
	_radius2 = _ambientOcclusion->getParameter("radius2");
	_projScale = _ambientOcclusion->getParameter("projScale");
	_projInfo = _ambientOcclusion->getParameter("projInfo");
	_clipInfo = _ambientOcclusion->getParameter("clipInfo");
	_bias = _ambientOcclusion->getParameter("bias");
	_intensityDivR6 = _ambientOcclusion->getParameter("intensityDivR6");

	_blurTexSource = _ambientOcclusion->getParameter("texSource");
	_blurFactor = _ambientOcclusion->getParameter("blurFactor");
	_blurSharpness = _ambientOcclusion->getParameter("blurSharpness");
	_blurRadius = _ambientOcclusion->getParameter("blurRadius");
	_blurDirection = _ambientOcclusion->getParameter("blurDirection");
	_blurGaussian = _ambientOcclusion->getParameter("blurGaussian");

	_copyAmbient = _ambientOcclusion->getParameter("texAO");

	Setting setting;
	setting.radius = 1.0;
	setting.bias = 0.012;
	setting.intensity = 4;

	setting.blur = true;
	setting.blurRadius = 8;
	setting.blurScale = 2.5;
	setting.blurSharpness = 4;

	this->setSetting(setting);
}

void
SSGI::onDeactivate(RenderPipeline& pipeline) except
{
}

void
SSGI::onRender(RenderPipeline& pipeline, RenderTexturePtr source) noexcept
{
	this->computeRawAO(pipeline, _texAmbient);

	if (_setting.blur)
	{
		this->blurHorizontal(pipeline, _texAmbient, _texBlur);
		this->blurVertical(pipeline, _texBlur, _texAmbient);
	}

	this->shading(pipeline, source, _texAmbient);
}

_NAME_END