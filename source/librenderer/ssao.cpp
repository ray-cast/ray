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
#include <ray/ssao.h>
#include <ray/camera.h>

#include <ray/graphics_view.h>
#include <ray/graphics_texture.h>

_NAME_BEGIN

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
	this->setRenderQueue(RenderQueue::RenderQueueLighting);
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
SSAO::computeRawAO(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	_cameraProjInfo->assign(pipeline.getCamera()->getProjConstant());
	_cameraProjScale->assign(pipeline.getCamera()->getProjLength().y * _setting.radius);

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_ambientOcclusionPass);
}

void
SSAO::blurHorizontal(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	GraphicsTextureDesc textureDesc = source->getGraphicsTextureDesc();

	float2 direction(_setting.blurScale, 0.0f);
	direction.x /= textureDesc.getWidth();

	this->blurDirection(pipeline, source, dest, direction);
}

void
SSAO::blurVertical(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	GraphicsTextureDesc textureDesc = source->getGraphicsTextureDesc();

	float2 direction(0.0f, _setting.blurScale);
	direction.y /= textureDesc.getHeight();

	this->blurDirection(pipeline, source, dest, direction);
}

void
SSAO::blurDirection(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest, const float2& direction) noexcept
{
	_blurDirection->assign(direction);
	_blurSource->assign(source);

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_ambientOcclusionBlurPass);
}

void
SSAO::shading(RenderPipeline& pipeline, GraphicsTexturePtr ambient, GraphicsRenderTexturePtr dest) noexcept
{
	_occlusionAmbient->assign(ambient);

	pipeline.setRenderTexture(dest);
	pipeline.drawScreenQuad(_ambientOcclusionCopyPass);
}

void
SSAO::createSphereNoise()
{
	std::vector<float2> sphere;
	std::size_t numSample = _sampleNumber->getInt();

	for (std::size_t i = 0; i < numSample; i++)
	{
		float sampleAlpha = (i + 0.5) * (1.0 / numSample);
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
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_texAmbientMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16SFloat);
	_texBlurMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16SFloat);

	GraphicsRenderTextureDesc ambientViewDesc;
	ambientViewDesc.attach(_texAmbientMap);
	_texAmbientView = pipeline.createRenderTexture(ambientViewDesc);

	GraphicsRenderTextureDesc blurViewDesc;
	blurViewDesc.attach(_texBlurMap);
	_texBlurView = pipeline.createRenderTexture(blurViewDesc);

	_ambientOcclusion = pipeline.createMaterial("sys:fx\\ssao.fxml.o");
	_ambientOcclusionPass = _ambientOcclusion->getTech(RenderQueue::RenderQueuePostprocess)->getPass("ao");
	_ambientOcclusionBlurPass = _ambientOcclusion->getTech(RenderQueue::RenderQueuePostprocess)->getPass("blur");
	_ambientOcclusionCopyPass = _ambientOcclusion->getTech(RenderQueue::RenderQueuePostprocess)->getPass("copy");

	_cameraProjScale = _ambientOcclusion->getParameter("projScale");
	_cameraProjInfo = _ambientOcclusion->getParameter("projInfo");

	_occlusionRadius = _ambientOcclusion->getParameter("radius");
	_occlusionRadius2 = _ambientOcclusion->getParameter("radius2");
	_occlusionBias = _ambientOcclusion->getParameter("bias");
	_occlusionIntensity = _ambientOcclusion->getParameter("intensity");
	_occlusionAmbient = _ambientOcclusion->getParameter("texOcclusion");
	_occlusionSphere = _ambientOcclusion->getParameter("sphere[0]");

	_blurSource = _ambientOcclusion->getParameter("texSource");
	_blurFactor = _ambientOcclusion->getParameter("blurFactor");
	_blurSharpness = _ambientOcclusion->getParameter("blurSharpness");
	_blurDirection = _ambientOcclusion->getParameter("blurDirection");
	_blurGaussian = _ambientOcclusion->getParameter("blurGaussian");

	_blurRadius = _ambientOcclusion->getMacro("BLUR_RADIUS");
	_sampleNumber = _ambientOcclusion->getMacro("NUM_SAMPLE");

	_setting.blurRadius = _blurRadius->getInt();

	this->createSphereNoise();

	this->setSetting(_setting);
}

void
SSAO::onDeactivate(RenderPipeline& pipeline) except
{
}

void
SSAO::onRender(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) except
{
	this->computeRawAO(pipeline, source, _texAmbientView);

	if (_setting.blur)
	{
		this->blurHorizontal(pipeline, _texAmbientMap, _texBlurView);
		this->blurVertical(pipeline, _texBlurMap, _texAmbientView);
	}

	this->shading(pipeline, _texAmbientMap, dest);

	// pipeline.blitRenderTexture(_texAmbient, Viewport(0, 0, 1376, 768), dest, Viewport(0, 0, 1376, 768));
	// pipeline.blitRenderTexture(_texAmbient, Viewport(0, 0, 1376, 768), 0, Viewport(0, 0, 1376, 768));
}

_NAME_END