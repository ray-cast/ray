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
#include <ray/ssao.h>
#include <ray/material_maker.h>
#include <ray/camera.h>
#include <ray/render_factory.h>
#include <ray/render_texture.h>

_NAME_BEGIN

#define NUM_SAMPLE 10

SSAO::Setting::Setting() noexcept
	: radius(1.0)
	, bias(0.002)
	, intensity(2)
	, blur(true)
	, blurRadius(6)
	, blurScale(2)
	, blurSharpness(8)
{
}

SSAO::SSAO() noexcept
{
	this->setRenderQueue(RenderQueue::RQ_OPAQUE);
}

SSAO::~SSAO() noexcept
{
}

void
SSAO::setSetting(const Setting& setting) noexcept
{
	const float blurSigma = _setting.blurRadius * 0.5;
	const float blurFalloff = 1.0 / (2.0 * blurSigma * blurSigma);

	_blurFactor->assign(blurFalloff);
	_blurRadius->assign(_setting.blurRadius);
	_blurSharpness->assign(_setting.blurSharpness);

	_occlusionRadius->assign(_setting.radius);
	_occlusionRadius2->assign(_setting.radius * _setting.radius);
	_occlusionBias->assign(_setting.bias);
	_occlusionIntensity->assign(_setting.intensity);

	_setting = setting;
}

const SSAO::Setting&
SSAO::getSetting() const noexcept
{
	return _setting;
}

void
SSAO::computeRawAO(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
	_cameraProjInfo->assign(pipeline.getCamera()->getProjConstant());
	_cameraProjScale->assign(pipeline.getCamera()->getProjLength().y * _setting.radius);
	_cameraClipInfo->assign(pipeline.getCamera()->getClipConstant());

	pipeline.setRenderTexture(dest);
	pipeline.clearRenderTexture(ClearFlags::CLEAR_COLOR, Vector4::Zero, 1.0, 0);
	pipeline.drawSceneQuad(_ambientOcclusionPass);
}

void
SSAO::blurHorizontal(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
	float2 direction(_setting.blurScale, 0.0f);
	direction.x /= source->getWidth();

	this->blurDirection(pipeline, source, dest, direction);
}

void
SSAO::blurVertical(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
	float2 direction(0.0f, _setting.blurScale);
	direction.y /= source->getHeight();

	this->blurDirection(pipeline, source, dest, direction);
}

void
SSAO::blurDirection(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest, const float2& direction) noexcept
{
	_blurDirection->assign(direction);
	_blurSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.clearRenderTexture(ClearFlags::CLEAR_COLOR, Vector4::Zero, 1.0, 0);
	pipeline.drawSceneQuad(_ambientOcclusionBlurPass);
}

void
SSAO::shading(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr ao) noexcept
{
	_occlusionAmbient->assign(ao->getResolveTexture());

	pipeline.setRenderTexture(source);
	pipeline.drawSceneQuad(_ambientOcclusionCopyPass);
}

void
SSAO::createSphereNoise()
{
	std::vector<float2> sphere;

	for (std::size_t i = 0; i < NUM_SAMPLE; i++)
	{
		float sampleAlpha = (i + 0.5) * (1.0 / NUM_SAMPLE);
		float angle = sampleAlpha * M_TWO_PI * 7;

		float2 rotate;
		sinCos(&rotate.y, &rotate.x, angle);

		sphere.push_back(rotate);
	}

	_occlusionSphere->assign(sphere);
}

void
SSAO::onActivate(RenderPipeline& pipeline) except
{
	std::size_t width, height;	
	pipeline.getWindowResolution(width, height);

	_texAmbient = RenderFactory::createRenderTexture();
	_texAmbient->setup(width, height, TextureDim::DIM_2D, PixelFormat::R16F);

	_texBlur = RenderFactory::createRenderTexture();
	_texBlur->setup(width, height, TextureDim::DIM_2D, PixelFormat::R16F);

	_ambientOcclusion = MaterialMaker("sys:fx\\ssao.glsl");
	_ambientOcclusionPass = _ambientOcclusion->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("ao");
	_ambientOcclusionBlurPass = _ambientOcclusion->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("blur");
	_ambientOcclusionCopyPass = _ambientOcclusion->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("copy");

	_cameraProjScale = _ambientOcclusion->getParameter("projScale");
	_cameraProjInfo = _ambientOcclusion->getParameter("projInfo");
	_cameraClipInfo = _ambientOcclusion->getParameter("clipInfo");

	_occlusionRadius = _ambientOcclusion->getParameter("radius");
	_occlusionRadius2 = _ambientOcclusion->getParameter("radius2");
	_occlusionBias = _ambientOcclusion->getParameter("bias");
	_occlusionIntensity = _ambientOcclusion->getParameter("intensity");
	_occlusionAmbient = _ambientOcclusion->getParameter("texOcclusion");
	_occlusionSphere = _ambientOcclusion->getParameter("sphere[0]");

	_blurSource = _ambientOcclusion->getParameter("texSource");
	_blurFactor = _ambientOcclusion->getParameter("blurFactor");
	_blurSharpness = _ambientOcclusion->getParameter("blurSharpness");
	_blurRadius = _ambientOcclusion->getParameter("blurRadius");
	_blurDirection = _ambientOcclusion->getParameter("blurDirection");
	_blurGaussian = _ambientOcclusion->getParameter("blurGaussian");

	this->createSphereNoise();

	this->setSetting(_setting);
}

void
SSAO::onDeactivate(RenderPipeline& pipeline) except
{
}

void
SSAO::onRender(RenderPipeline& pipeline, RenderTexturePtr source) except
{
	this->computeRawAO(pipeline, source, _texAmbient);

	if (_setting.blur)
	{
		this->blurHorizontal(pipeline, _texAmbient, _texBlur);
		this->blurVertical(pipeline, _texBlur, _texAmbient);
	}

	this->shading(pipeline, source, _texAmbient);

	//pipeline.copyRenderTexture(_texAmbient, Viewport(0, 0, 1376, 768), source, Viewport(0, 0, 1376, 768));
	//pipeline.copyRenderTexture(_texAmbient, Viewport(0, 0, 1376, 768), 0, Viewport(0, 0, 1376, 768));
}

_NAME_END