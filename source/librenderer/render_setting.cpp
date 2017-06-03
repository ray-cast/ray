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
#include <ray/render_setting.h>

_NAME_BEGIN

void
RenderSetting::computeScatteringCoefficients() noexcept
{
	double n = 1.0003;
	double N = 2.545e+25;
	double Pn = 0.035;
	double rayleighConst = 8.0 * M_PI * M_PI * M_PI * (n * n - 1.0) * (n * n - 1.0) / (3.0 * N) * (6.0 + 3.0 * Pn) / (6.0 - 7.0 * Pn);

	double3 wave(680e-9, 550e-9, 440e-9);
	double3 lambda2 = wave * wave;
	double3 lambda4 = lambda2 * lambda2;
	double3 sctrCoeff = rayleighConst / lambda4;

	float4 mieTotalSctrCoeff = float4(this->density * 2e-5f);

	this->rayleighAngularSctrCoeff = float4(static_cast<float3>(4.0 / (16.0 * M_PI) * sctrCoeff), 0.0);
	this->rayleighExtinctionCoeff = float4(static_cast<float3>(sctrCoeff), 0.0f);
	this->mieAngularSctrCoeff = mieTotalSctrCoeff / float(4.0f * M_PI);
	this->mieExtinctionCoeff = mieTotalSctrCoeff * (1.f);
}

RenderSetting::RenderSetting() noexcept
	: window(nullptr)
	, width(0)
	, height(0)
	, dpi_w(0)
	, dpi_h(0)
	, deviceType(GraphicsDeviceType::GraphicsDeviceTypeOpenGL)
	, swapInterval(GraphicsSwapInterval::GraphicsSwapIntervalFree)
	, pipelineType(RenderPipelineType::RenderPipelineTypeDeferredLighting)
	, shadowMode(ShadowMode::ShadowModeSoft)
	, shadowQuality(ShadowQuality::ShadowQualityMedium)
	, enableSSDO(true)
	, enableAtmospheric(false)
	, enableSSR(false)
	, enableSSSS(false)
	, enableFog(false)
	, enableLightShaft(false)
	, enableDOF(false)
	, enableMotionBlur(false)
	, enableHDR(true)
	, enableColorGrading(false)
	, enableFXAA(true)
	, enableGlobalIllumination(false)
	, earthRadius(6360000.f, 6440000.f)
	, earthScaleHeight(7994.f, 2000.f)
	, minElevation(0.0f)
	, maxElevation(80000.0f)
	, rayleighAngularSctrCoeff(0)
	, rayleighExtinctionCoeff(0)
	, mieAngularSctrCoeff(0)
	, mieExtinctionCoeff(0)
	, mie(0.97f)
	, density(1.0f)
{
	computeScatteringCoefficients();
}

_NAME_END