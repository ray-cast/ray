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
#include <ray/render_object_manager.h>
#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>

_NAME_BEGIN

Atmospheric::Setting::Setting() noexcept
	: earthRadius(6360000.f)
	, earthAtmTopHeight(80000.f)
	, minElevation(0.0f)
	, maxElevation(10000.0f)
	, particleScaleHeight(7994.f, 2000.f)
	, rayleighAngularSctrCoeff(0)
	, rayleighTotalSctrCoeff(0)
	, rayleighExtinctionCoeff(0)
	, mieAngularSctrCoeff(0)
	, mieExtinctionCoeff(0)
	, aerosolPhaseFuncG(0.97)
	, aerosolPhaseFuncG4(0)
	, aerosolAbsorbtionScale(0.1f)
	, aerosolDensityScale(1.0f)
{
}

Atmospheric::Atmospheric() noexcept
	: _needUpdateOpticalDepthAtmTop(true)
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
	float3 center = float3(0, -_setting.earthRadius, 0);
	float3 radius = float3(_setting.earthRadius, _setting.earthRadius + _setting.minElevation, _setting.earthRadius + _setting.maxElevation);
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

	const int numTestDirections = 5;
	for (int i = 0; i < numTestDirections; ++i)
	{
		for (int j = 0; j < numTestDirections; ++j)
		{
			float3 positionPS;
			positionPS.x = math::unorm2snorm((float)i / (numTestDirections - 1));
			positionPS.y = math::unorm2snorm((float)j / (numTestDirections - 1));
			positionPS.z = 0;

			float3 positionWS = camera.getViewProjectInverse() * positionPS;
			float3 direction = math::normalize(positionWS - eyePosition);

			float2 isecsWithBottomBoundSphere;
			computeRaySphereIntersection(eyePosition, direction, center, radius.y, isecsWithBottomBoundSphere);

			float nearIsecWithBottomSphere = isecsWithBottomBoundSphere.x > 0 ? isecsWithBottomBoundSphere.x : isecsWithBottomBoundSphere.y;
			if (nearIsecWithBottomSphere > 0)
			{
				float3 hitPointWS = eyePosition + direction * nearIsecWithBottomSphere;
				float3 hitPointCamSpace = camera.getView() * hitPointWS;

				zfar = std::max(zfar, hitPointCamSpace.z);
			}
			else
			{
				zfar = maxViewDistance;
			}
		}
	}

	const float4& viewport = camera.getPixelViewport();

	float4x4 project;
	project.makePerspective_fov_lh(camera.getAperture(), (float)viewport.z / viewport.w, znear, zfar);

	viewProjectInverse = math::inverse(project * camera.getView());
}

void
Atmospheric::computeScatteringCoefficients() noexcept
{
	double n = 1.0003;
	double N = 2.545e+25;
	double Pn = 0.035;
	double rayleighConst = 8.0 * M_PI * M_PI * M_PI * (n * n - 1.0) * (n * n - 1.0) / (3.0 * N) * (6.0 + 3.0 * Pn) / (6.0 - 7.0 * Pn);

	double3 wave(680e-9, 550e-9, 440e-9);
	double3 lambda2 = wave * wave;
	double3 lambda4 = lambda2 * lambda2;
	double3 sctrCoeff = rayleighConst / lambda4;

	float g = _setting.aerosolPhaseFuncG;
	float g2 = g * g;

	_setting.aerosolPhaseFuncG4.x = 3 * (1.0f - g2) / (2 * (2.0f + g2));
	_setting.aerosolPhaseFuncG4.y = 1.f + g2;
	_setting.aerosolPhaseFuncG4.z = -2.f * g;
	_setting.aerosolPhaseFuncG4.w = 1.f;

	_setting.rayleighTotalSctrCoeff = float4(sctrCoeff, 0.0f);
	_setting.rayleighAngularSctrCoeff = float4(3.0 / (16.0 * M_PI) * sctrCoeff, 0.0);
	_setting.rayleighExtinctionCoeff = _setting.rayleighTotalSctrCoeff;
	_setting.mieTotalSctrCoeff = float4(_setting.aerosolDensityScale * 2e-5f);
	_setting.mieAngularSctrCoeff = _setting.mieTotalSctrCoeff / (4.0f * M_PI);
	_setting.mieExtinctionCoeff = _setting.mieTotalSctrCoeff * (1.f + _setting.aerosolAbsorbtionScale);
}

float2 ChapmanOrtho(const float2 &f2x)
{
	static const float fConst = static_cast<float>(std::sqrt(M_PI / 2));
	float2 f2SqrtX = float2(sqrt(f2x.x), sqrt(f2x.y));
	return fConst * (float2(1.f, 1.f) / (2.f * f2SqrtX) + f2SqrtX);
}

float2 ChapmanRising(const float2 &f2X, float fCosChi)
{
	float2 f2ChOrtho = ChapmanOrtho(f2X);
	return f2ChOrtho / ((f2ChOrtho - float2(1, 1)) * fCosChi + float2(1, 1));
}

void
Atmospheric::computeDensityIntegralFromChapmanFunc(float fHeightAboveSurface, const float3& f3EarthCentreToPointDir, const float3 &f3RayDir, float2& densityIntegral) noexcept
{
	float fCosChi = math::dot(f3EarthCentreToPointDir, f3RayDir);
	float2 f2x = (fHeightAboveSurface + _setting.earthRadius) * float2(1.f / _setting.particleScaleHeight.x, 1.f / _setting.particleScaleHeight.y);
	float2 f2VerticalAirMass = _setting.particleScaleHeight * math::exp(-float2(fHeightAboveSurface, fHeightAboveSurface) / _setting.particleScaleHeight);
	if (fCosChi >= 0.f)
	{
		densityIntegral = f2VerticalAirMass * ChapmanRising(f2x, fCosChi);
	}
	else
	{
		float fSinChi = sqrt(1.f - fCosChi * fCosChi);
		float fh0 = (fHeightAboveSurface + _setting.earthRadius) * fSinChi - _setting.earthRadius;
		float2 f2VerticalAirMass0 = _setting.particleScaleHeight * math::exp(-float2(fh0, fh0) / _setting.particleScaleHeight);
		float2 f2x0 = float2(fh0 + _setting.earthRadius, fh0 + _setting.earthRadius) / _setting.particleScaleHeight;
		float2 f2ChOrtho_x0 = ChapmanOrtho(f2x0);
		float2 f2Ch = ChapmanRising(f2x, -fCosChi);
		densityIntegral = f2VerticalAirMass0 * (2.f * f2ChOrtho_x0) - f2VerticalAirMass*f2Ch;
	}
}

void
Atmospheric::computeSunColor(const float3& vDirectionOnSun, float4& f4SunColorAtGround, float4& f4AmbientLight) noexcept
{
	float zenithFactor = std::min(std::max(vDirectionOnSun.y, 0.0f), 1.0f);
	f4AmbientLight.x = zenithFactor*0.15f;
	f4AmbientLight.y = zenithFactor*0.1f;
	f4AmbientLight.z = std::max(0.05f, zenithFactor*0.25f);
	f4AmbientLight.w = 0.0f;

	float2 opticalDepthAtmosp;
	computeDensityIntegralFromChapmanFunc(0, float3::UnitY, vDirectionOnSun, opticalDepthAtmosp);

	float3 rlghExtCoeff = math::max((float3&)_setting.rayleighExtinctionCoeff, float3(1e-8f, 1e-8f, 1e-8f));
	float3 rlghOpticalDepth = rlghExtCoeff * opticalDepthAtmosp.x;

	float3 mieExtCoeff = math::max((float3&)_setting.mieExtinctionCoeff, float3(1e-8f, 1e-8f, 1e-8f));
	float3 mieOpticalDepth = mieExtCoeff * opticalDepthAtmosp.y;

	float3 totalExtinction = math::exp(-(rlghOpticalDepth + mieOpticalDepth));

	const float reflectance = 0.1f;
	f4SunColorAtGround.set(totalExtinction * reflectance);
}

void
Atmospheric::onActivate(RenderPipeline& pipeline) noexcept
{
	_sat = pipeline.createMaterial("sys:fx/atmospheric.fxml");

	_computeOpticalDepthAtmTop = _sat->getTech("ComputeOpticalDepthAtmTop");
	_computeInscatteredRadiance = _sat->getTech("ComputeInscatteredRadiance");

	_lightDirection = _sat->getParameter("lightDirection");
	_rayleighAngularSctrCoeff = _sat->getParameter("rayleighAngularSctrCoeff");
	_rayleighTotalSctrCoeff = _sat->getParameter("rayleighTotalSctrCoeff");
	_rayleighExtinctionCoeff = _sat->getParameter("rayleighExtinctionCoeff");
	_mieAngularSctrCoeff = _sat->getParameter("mieAngularSctrCoeff");
	_mieExtinctionCoeff = _sat->getParameter("mieExtinctionCoeff");
	_extraterrestrialSunColor = _sat->getParameter("extraterrestrialSunColor");
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
	netDensityDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
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

	this->computeScatteringCoefficients();

	_particleScaleHeight->uniform2f(_setting.particleScaleHeight);	
	_mieAngularSctrCoeff->uniform4f(_setting.mieAngularSctrCoeff);
	_mieExtinctionCoeff->uniform4f(_setting.mieExtinctionCoeff);	
	_rayleighAngularSctrCoeff->uniform4f(_setting.rayleighAngularSctrCoeff);
	_rayleighExtinctionCoeff->uniform4f(_setting.rayleighExtinctionCoeff);
	_earthAtmTopHeight->uniform1f(_setting.earthAtmTopHeight);
	_earthAtmTopRadius->uniform1f(_setting.earthAtmTopHeight + _setting.earthRadius);
	_earthRadius->uniform1f(_setting.earthRadius);
	_aerosolPhaseFuncG4->uniform4f(_setting.aerosolPhaseFuncG4);
	_tex2DOccludedNetDensityToAtmTop->uniformTexture(_occludedNetDensityMap);
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

	static auto direction = float3::UnitY;

	Quaternion rotate;
	rotate.makeRotate(float3::UnitX, 0.03);

	direction = math::normalize(math::rotate(rotate, direction));

	_lightDirection->uniform3f(-direction);

	float4 extraterrestrialSunColor = float4(10.0, 10.0, 10.0, 10.0) * 0.5f;
	_extraterrestrialSunColor->uniform4f(extraterrestrialSunColor);

	pipeline.setFramebuffer(source);
	pipeline.drawScreenQuad(*_computeInscatteredRadiance);

	return false;
}

_NAME_END