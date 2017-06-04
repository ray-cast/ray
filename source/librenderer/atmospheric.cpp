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
#include "atmospheric.h"
#include <ray/light.h>
#include <ray/camera.h>
#include <ray/material.h>
#include <ray/geometry.h>
#include <ray/render_pipeline.h>
#include <ray/render_pipeline_manager.h>
#include <ray/render_object_manager.h>
#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>

_NAME_BEGIN

Atmospheric::Setting::Setting() noexcept
	: earthRadius(6360000.f, 6440000.f)
	, earthScaleHeight(7994.f, 2000.f)
	, minElevation(0.0f)
	, maxElevation(80000.0f)
	, rayleighAngularSctrCoeff(0)
	, rayleighExtinctionCoeff(0)
	, mieAngularSctrCoeff(0)
	, mieExtinctionCoeff(0)
	, mie(0.76)
{
}

Atmospheric::Atmospheric() noexcept
	: _needUpdateOpticalDepthAtmTop(true)
{
}

Atmospheric::Atmospheric(Setting& setting) noexcept
	: _setting(setting)
{
}

Atmospheric::~Atmospheric() noexcept
{
}

void
Atmospheric::computeRaySphereIntersection(const float3& position, const float3& dir, const float3& center, float radius, float2& intersections) noexcept
{
	float3 origin = position - center;
	float B = math::dot(origin, dir);
	float C = math::dot(origin, origin) - radius * radius;
	float D = B * B - C;

	if (D < 0)
	{
		intersections = float2(-1, -1);
	}
	else
	{
		D = std::sqrt(D);
		intersections = float2(-B - D, -B + D);
	}
}

void
Atmospheric::computeViewProjectInverse(const Camera& camera, float4x4& viewProjectInverse) noexcept
{
	float3 center = float3(0, -_setting.earthRadius.x, 0);
	float3 radius = float3(_setting.earthRadius.x, _setting.earthRadius.x + _setting.minElevation, _setting.earthRadius.x + _setting.maxElevation);
	float3 eyePosition = camera.getTranslate();
	float3 cameraGlobalPos = eyePosition - center;
	float cameraElevationSqr = math::dot(cameraGlobalPos, cameraGlobalPos);
	float cameraElev = std::sqrt(cameraElevationSqr);

	double radius2 = radius.x * radius.x;
	double maxRadius2 = radius.z * radius.z;
	double maxViewDistance = std::sqrt(cameraElevationSqr - radius2) + std::sqrt(maxRadius2 - radius2);

	float znear = 50.0f;
	float zfar = 1000.0f;

	if (cameraElev > radius.z)
	{
		const float4x4& proj = camera.getProject();
		znear = (cameraElev - radius.z) / std::sqrt(1 + 1.0f / (proj.a1 * proj.a1) + 1.0f / (proj.b2 * proj.b2));
	}

	const std::uint8_t numTestDirections = 5;
	for (std::uint8_t i = 0; i < numTestDirections; ++i)
	{
		for (std::uint8_t j = 0; j < numTestDirections; ++j)
		{
			float3 proj;
			proj.x = math::unorm2snorm(static_cast<float>(i) / (numTestDirections - 1));
			proj.y = math::unorm2snorm(static_cast<float>(j) / (numTestDirections - 1));
			proj.z = 0;

			float3 world = camera.getViewProjectInverse() * proj;
			float3 direction = math::normalize(world - eyePosition);

			float2 intersection;
			computeRaySphereIntersection(eyePosition, direction, center, radius.y, intersection);

			intersection.x = intersection.x > 0 ? intersection.x : intersection.y;
			if (intersection.x > 0)
			{
				float3 hitPointWS = eyePosition + direction * intersection.x;
				float3 hitPointCamSpace = camera.getView() * hitPointWS;

				zfar = std::max(zfar, hitPointCamSpace.z);
			}
			else
			{
				zfar = static_cast<float>(maxViewDistance);
			}
		}
	}

	const float4& viewport = camera.getPixelViewport();

	float4x4 project;
	project.makePerspective_fov_lh(camera.getAperture(), (float)viewport.z / viewport.w, znear, zfar);

	viewProjectInverse = math::inverse(project * camera.getView());
}

void
Atmospheric::setScatteringCoefficients(const Setting& setting) noexcept
{
	if (_sat)
	{
		float g = setting.mie;
		float g2 = g * g;

		float4 mie4;
		mie4.x = 3 * (1.0f - g2) / (2 * (2.0f + g2));
		mie4.y = 1.f + g2;
		mie4.z = -2.f * g;
		mie4.w = 1.f;

		_particleScaleHeight->uniform2f(setting.earthScaleHeight);
		_mieAngularSctrCoeff->uniform4f(setting.mieAngularSctrCoeff);
		_mieExtinctionCoeff->uniform4f(setting.mieExtinctionCoeff);
		_rayleighAngularSctrCoeff->uniform4f(setting.rayleighAngularSctrCoeff);
		_rayleighExtinctionCoeff->uniform4f(setting.rayleighExtinctionCoeff);
		_earthAtmTopHeight->uniform1f(setting.earthRadius.y - setting.earthRadius.x);
		_earthAtmTopRadius->uniform1f(setting.earthRadius.y);
		_earthRadius->uniform1f(setting.earthRadius.x);
		_aerosolPhaseFuncG4->uniform4f(mie4);
		_tex2DOccludedNetDensityToAtmTop->uniformTexture(_occludedNetDensityMap);
	}

	_setting = setting;
}

const Atmospheric::Setting&
Atmospheric::getScatteringCoefficients() const noexcept
{
	return _setting;
}

void
Atmospheric::onActivate(RenderPipeline& pipeline) noexcept
{
	_sat = pipeline.createMaterial("sys:fx/atmospheric.fxml");

	_computeOpticalDepthAtmTop = _sat->getTech("ComputeOpticalDepthAtmTop");
	_computeInscatteredRadiance = _sat->getTech("ComputeInscatteredRadiance");

	_lightColor = _sat->getParameter("lightColor");
	_lightDirection = _sat->getParameter("lightDirection");
	_rayleighAngularSctrCoeff = _sat->getParameter("rayleighAngularSctrCoeff");
	_rayleighExtinctionCoeff = _sat->getParameter("rayleighExtinctionCoeff");
	_mieAngularSctrCoeff = _sat->getParameter("mieAngularSctrCoeff");
	_mieExtinctionCoeff = _sat->getParameter("mieExtinctionCoeff");
	_aerosolPhaseFuncG4 = _sat->getParameter("aerosolPhaseFuncG4");
	_earthRadius = _sat->getParameter("earthRadius");
	_earthAtmTopHeight = _sat->getParameter("earthAtmTopHeight");
	_earthAtmTopRadius = _sat->getParameter("earthAtmTopRadius");
	_particleScaleHeight = _sat->getParameter("particleScaleHeight");
	_tex2DOccludedNetDensityToAtmTop = _sat->getParameter("tex2DOccludedNetDensityToAtmTop");
	_matViewProjectInverse = _sat->getParameter("matViewProjectInverse");

	GraphicsTextureDesc netDensityDesc;
	netDensityDesc.setWidth(256);
	netDensityDesc.setHeight(256);
	netDensityDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	netDensityDesc.setTexFormat(GraphicsFormat::GraphicsFormatR32G32SFloat);
	netDensityDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	netDensityDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_occludedNetDensityMap = pipeline.createTexture(netDensityDesc);

	GraphicsFramebufferLayoutDesc netDensityToAtmTopLayoutDesc;
	netDensityToAtmTopLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR32G32SFloat));
	_netDensityToAtmTopLayout = pipeline.createFramebufferLayout(netDensityToAtmTopLayoutDesc);

	GraphicsFramebufferDesc netDensityToAtmTopDesc;
	netDensityToAtmTopDesc.setWidth(256);
	netDensityToAtmTopDesc.setHeight(256);
	netDensityToAtmTopDesc.addColorAttachment(GraphicsAttachmentBinding(_occludedNetDensityMap, 0, 0));
	netDensityToAtmTopDesc.setGraphicsFramebufferLayout(_netDensityToAtmTopLayout);
	_netDensityToAtmTopView = pipeline.createFramebuffer(netDensityToAtmTopDesc);

	this->setScatteringCoefficients(_setting);
}

void
Atmospheric::onDeactivate(RenderPipeline& pipeline) noexcept
{
}

bool
Atmospheric::onRender(RenderPipeline& pipeline, RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr swap) noexcept
{
	if (queue != RenderQueue::RenderQueueOpaqueSpecific)
		return false;

	float4x4 viewProjectInverse;
	computeViewProjectInverse(*pipeline.getCamera(), viewProjectInverse);

	_matViewProjectInverse->uniform4fmat(viewProjectInverse);

	if (_needUpdateOpticalDepthAtmTop)
	{
		pipeline.setFramebuffer(_netDensityToAtmTopView);
		pipeline.discardFramebuffer(0);
		pipeline.drawScreenQuad(*_computeOpticalDepthAtmTop);
		_needUpdateOpticalDepthAtmTop = false;
	}

	const auto& lighting = pipeline.getCamera()->getRenderDataManager()->getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lighting)
	{
		auto light = it->downcast<Light>();
		if (light->getLightType() == LightType::LightTypeSun)
		{
			_lightColor->uniform3f(light->getLightColor() * light->getLightIntensity());
			_lightDirection->uniform3f(light->getForward());

			pipeline.setFramebuffer(source);
			pipeline.drawScreenQuad(*_computeInscatteredRadiance);
		}
	}

	return false;
}

_NAME_END