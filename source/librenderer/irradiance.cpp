// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include "irradiance.h"
#include <ray/material.h>
#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/render_pipeline.h>
#include <ray/SH.h>

#define PARABOLOID_SAMPLES 64
#define NUM_ORDER 3
#define NUM_ORDER_P2 4
#define NUM_RESULT_SAMPLES 32     // bilinear filtering of 64x64 texture results in 4xAA 4 free
#define NUM_RADIANCE_SAMPLES 32
#define MINORDER 2
#define MAXORDER 6

_NAME_BEGIN

bool isPow2(std::uint32_t i) noexcept
{
	return ((i&(i - 1)) == 0);
}

std::uint32_t nextPow2(std::uint32_t i) noexcept
{
	std::uint32_t d = 0x1;
	while (d < i)
		d <<= 1;
	return d;
}

EnvironmentIrradiance::EnvironmentIrradiance() noexcept
{
}

EnvironmentIrradiance::~EnvironmentIrradiance() noexcept
{
	this->close();
}

bool
EnvironmentIrradiance::setup(RenderPipeline& pipeline) noexcept
{
	_irradiance = pipeline.createMaterial("sys:fx/irradiance.fxml"); if (!_irradiance) return false;
	_irradianceParaboloid = _irradiance->getTech("ConvertHemisphere"); if (!_irradianceParaboloid) return false;
	_irradianceProjectDualParaboloidToSH = _irradiance->getTech("ProjectDualParaboloidToSH"); if (!_irradianceProjectDualParaboloidToSH) return false;

	_paraboloidCubeMapSampler = _irradiance->getParameter("paraboloidCubeMapSampler"); if (!_paraboloidCubeMapSampler) return false;
	_paraboloidSamplesInverse = _irradiance->getParameter("paraboloidSamplesInverse"); if (!_paraboloidSamplesInverse) return false;

	_sphericalHarmonicConvolveDE0 = _irradiance->getParameter("SHConvolveDE0"); if (!_sphericalHarmonicConvolveDE0) return false;
	_sphericalHarmonicConvolveDE1 = _irradiance->getParameter("SHConvolveDE1"); if (!_sphericalHarmonicConvolveDE1) return false;
	_sphericalHarmonicConvolveYlmDW0 = _irradiance->getParameter("SHConvolveYlmDW0"); if (!_sphericalHarmonicConvolveYlmDW0) return false;
	_sphericalHarmonicConvolveYlmDW1 = _irradiance->getParameter("SHConvolveYlmDW1"); if (!_sphericalHarmonicConvolveYlmDW1) return false;

	_paraboloidFrontMap = pipeline.createTexture(PARABOLOID_SAMPLES, PARABOLOID_SAMPLES, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16G16B16SFloat);
	if (!_paraboloidFrontMap)
		return false;

	_paraboloidBackMap = pipeline.createTexture(PARABOLOID_SAMPLES, PARABOLOID_SAMPLES, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16G16B16SFloat);
	if (!_paraboloidBackMap)
		return false;

	_irradianceSHCoefficientsMap = pipeline.createTexture(NUM_ORDER_P2, NUM_ORDER_P2, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16G16B16SFloat);
	if (!_irradianceSHCoefficientsMap)
		return false;

	GraphicsFramebufferDesc paraboloidDualDesc;
	paraboloidDualDesc.addColorAttachment(GraphicsAttachmentBinding(_paraboloidFrontMap, 0, 0));
	paraboloidDualDesc.addColorAttachment(GraphicsAttachmentBinding(_paraboloidBackMap, 0, 0));
	_paraboloidDualViews = pipeline.createFramebuffer(paraboloidDualDesc);
	if (!_paraboloidDualViews)
		return false;

	GraphicsFramebufferDesc irradianceSHCoefficientsDesc;
	irradianceSHCoefficientsDesc.addColorAttachment(GraphicsAttachmentBinding(_paraboloidFrontMap, 0, 0));
	irradianceSHCoefficientsDesc.addColorAttachment(GraphicsAttachmentBinding(_paraboloidBackMap, 0, 0));
	_irradianceSHCoefficientsView = pipeline.createFramebuffer(irradianceSHCoefficientsDesc);
	if (!_irradianceSHCoefficientsView)
		return false;

	return _buildDualParaboloidWeightTextures(pipeline, _paraboloidSHWeights, NUM_ORDER, NUM_RADIANCE_SAMPLES);
}

void
EnvironmentIrradiance::close() noexcept
{
	_irradiance.reset();
	_irradianceParaboloid.reset();
	_irradianceProjectDualParaboloidToSH.reset();

	_paraboloidSamplesInverse.reset();
	_paraboloidCubeMapSampler.reset();

	_sphericalHarmonicConvolveDE0.reset();
	_sphericalHarmonicConvolveDE1.reset();
	_sphericalHarmonicConvolveYlmDW0.reset();
	_sphericalHarmonicConvolveYlmDW1.reset();

	_paraboloidFrontMap.reset();
	_paraboloidBackMap.reset();
	_paraboloidDualMaps.reset();
	_paraboloidSHWeights[0].reset();
	_paraboloidSHWeights[1].reset();
	_paraboloidSphericalHarmonicMap.reset();
	_irradianceSHCoefficientsMap.reset();

	_paraboloidDualViews.reset();
	_irradianceSHCoefficientsView.reset();
}

void
EnvironmentIrradiance::renderParaboloidEnvMap(RenderPipeline& pipeline, GraphicsTexturePtr cubemap) noexcept
{
	assert(cubemap);

	_paraboloidCubeMapSampler->uniformTexture(cubemap);
	_paraboloidSamplesInverse->uniform1f(1.0f / PARABOLOID_SAMPLES);

	_sphericalHarmonicConvolveDE0->uniformTexture(_paraboloidFrontMap);
	_sphericalHarmonicConvolveDE1->uniformTexture(_paraboloidBackMap);
	_sphericalHarmonicConvolveYlmDW0->uniformTexture(_paraboloidSHWeights[0]);
	_sphericalHarmonicConvolveYlmDW1->uniformTexture(_paraboloidSHWeights[1]);

	pipeline.setFramebuffer(_paraboloidDualViews);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(*_irradianceParaboloid);

	pipeline.setFramebuffer(_irradianceSHCoefficientsView);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(*_irradianceProjectDualParaboloidToSH);
}

void
EnvironmentIrradiance::renderProjectParaboloidToSH(RenderPipeline& pipeline, GraphicsFramebufferPtr evalSHFunction, GraphicsFramebufferPtr dest) noexcept
{
}

void
EnvironmentIrradiance::renderEvaluateConvolvedSH(RenderPipeline& pipeline, GraphicsFramebufferPtr evalSHFunction, GraphicsFramebufferPtr dest) noexcept
{
}

bool
EnvironmentIrradiance::_paraboloidCoord(double3& vec, int face, const double2& uv) noexcept
{
	//  sphere direction is the reflection of the orthographic view direction (determined by
	//  face), reflected about the normal to the paraboloid at uv
	double r_sqr = uv.x * uv.x + uv.y * uv.y;

	if (r_sqr > 1.0)
		return false;

	double3 axis;
	if (face == 0)
		axis = double3(0.f, 0.f, -1.f);
	else
		axis = double3(0.f, 0.f, 1.f);

	// compute normal on the parabaloid at uv
	double3 N(uv.x, uv.y, 1.f);
	N = math::normalize(N);

	// reflect axis around N, to compute sphere direction
	double v_dot_n = math::dot(axis, N);
	double3 R = axis - 2 * v_dot_n * N;

	vec = R;
	return true;
}

bool
EnvironmentIrradiance::_buildDualParaboloidWeightTextures(RenderPipeline& pipeline, GraphicsTexturePtr textures[2], std::uint32_t order, std::uint32_t size) noexcept
{
	assert(textures || isPow2(size));

	std::uint32_t nSize = nextPow2(order);

	//  texels need to be weighted by solid angle
	//  compute differential solid angle at texel center (cos(theta)/r^2), and then normalize and scale by 4*PI
	double *d_omega;
	double sum_d_omega = 0.f;
	d_omega = new double[size * size];

	for (std::size_t t = 0; t < size; t++)
	{
		for (std::size_t s = 0; s < size; s++)
		{
			double x = ((s + 0.5) / double(size))*2. - 1.;
			double y = ((t + 0.5) / double(size))*2. - 1.;
			double r_sqr = x*x + y*y;

			std::size_t index = t*size + s;
			if (r_sqr > 1.)  // only count points inside the circle
			{
				d_omega[index] = 0.;
				continue;
			}

			double z = 0.5*(1. - r_sqr);
			double mag = sqrt(r_sqr + z*z);  // =0.5[1+(x*x + y*y)]

			double cosTheta = 1.;  // cos(theta) terms fall out, since dA is first projected
								   // orthographically onto the paraboloid ( dA' = dA / dot(zAxis, Np) ), then reflected
								   // and projected onto the unit sphere (dA'' = dA' dot(R,Np) / len^2)
								   // dot(zAxis, Np) == dot(R, Np), so dA'' = dA / len^2
			d_omega[index] = cosTheta / (mag*mag);  //  = 1 / (mag^2)
			sum_d_omega += d_omega[index];
		}
	}

	double d_omega_scale = 4.*M_PI / (2.f*sum_d_omega);

	for (std::uint8_t face = 0; face < 2; face++)
	{
		float *coefficients;
		coefficients = new float[size * size * size * size];
		std::memset(coefficients, 0, size*size*size*size*sizeof(float));

		for (std::size_t t = 0; t < size; t++)
		{
			for (std::size_t s = 0; s < size; s++)
			{
				double sd = ((s + 0.5) / double(size))*2.0 - 1.0;
				double td = ((t + 0.5) / double(size))*2.0 - 1.0;
				double2 stVec((float)sd, (float)td);

				double3 parabVec;
				if (!_paraboloidCoord(parabVec, face, stVec))
					continue;   //  skip if this texel is outside the paraboloid

				//  compute the N^2 spherical harmonic basis functions
				
				auto sh = math::ProjectOntoSH<NUM_ORDER * NUM_ORDER, double>(parabVec);

				std::size_t basis = 0;
				std::size_t index = t * size + s;

				for (int l = 0; l < (int)order; l++)
				{
					for (int m = -l; m <= l; m++, basis++)
					{
						std::size_t tiley = basis / order;
						std::size_t tilex = basis % order;
						double Ylm = sh[l*(l + 1) + m];

						std::size_t offset = ((tiley*size + t)*size*nSize) + tilex*size + s;
						float weight = (float)(Ylm * d_omega[index] * d_omega_scale);
						coefficients[offset] = weight;
					}
				}
			}
		}

		GraphicsTextureDesc textureDesc;
		textureDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
		textureDesc.setTexFormat(GraphicsFormat::GraphicsFormatR32SFloat);
		textureDesc.setSize(size*size, size*size);
		textureDesc.setStream(coefficients);

		_paraboloidSHWeights[face] = pipeline.createTexture(textureDesc);

		delete[] coefficients;
	}

	delete[] d_omega;
	return true;
}

_NAME_END