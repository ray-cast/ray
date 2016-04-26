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

float* D3DXSHEvalDirection(float* out, std::uint32_t order, const Vector3 *dir) noexcept
{
	if ((order < MINORDER) || (order > MAXORDER))
		return out;

	out[0] = 0.5f / sqrt(M_PI);
	out[1] = -0.5f / sqrt(M_PI / 3.0f) * dir->y;
	out[2] = 0.5f / sqrt(M_PI / 3.0f) * dir->z;
	out[3] = -0.5f / sqrt(M_PI / 3.0f) * dir->x;
	if (order == 2)
		return out;

	out[4] = 0.5f / sqrt(M_PI / 15.0f) * dir->x * dir->y;
	out[5] = -0.5f / sqrt(M_PI / 15.0f) * dir->y * dir->z;
	out[6] = 0.25f / sqrt(M_PI / 5.0f) * (3.0f * dir->z * dir->z - 1.0f);
	out[7] = -0.5f / sqrt(M_PI / 15.0f) * dir->x * dir->z;
	out[8] = 0.25f / sqrt(M_PI / 15.0f) * (dir->x * dir->x - dir->y * dir->y);
	if (order == 3)
		return out;

	out[9] = -sqrt(70.0f / M_PI) / 8.0f * dir->y * (3.0f * dir->x * dir->x - dir->y * dir->y);
	out[10] = sqrt(105.0f / M_PI) / 2.0f * dir->x * dir->y * dir->z;
	out[11] = -sqrt(42.0 / M_PI) / 8.0f * dir->y * (-1.0f + 5.0f * dir->z * dir->z);
	out[12] = sqrt(7.0f / M_PI) / 4.0f * dir->z * (5.0f * dir->z * dir->z - 3.0f);
	out[13] = sqrt(42.0 / M_PI) / 8.0f * dir->x * (1.0f - 5.0f * dir->z * dir->z);
	out[14] = sqrt(105.0f / M_PI) / 4.0f * dir->z * (dir->x * dir->x - dir->y * dir->y);
	out[15] = -sqrt(70.0f / M_PI) / 8.0f * dir->x * (dir->x * dir->x - 3.0f * dir->y * dir->y);
	if (order == 4)
		return out;

	out[16] = 0.75f * sqrt(35.0f / M_PI) * dir->x * dir->y * (dir->x * dir->x - dir->y * dir->y);
	out[17] = 3.0f * dir->z * out[9];
	out[18] = 0.75f * sqrt(5.0f / M_PI) * dir->x * dir->y * (7.0f * dir->z * dir->z - 1.0f);
	out[19] = 0.375f * sqrt(10.0f / M_PI) * dir->y * dir->z * (3.0f - 7.0f * dir->z * dir->z);
	out[20] = 3.0f / (16.0f * sqrt(M_PI)) * (35.0f * dir->z * dir->z * dir->z * dir->z - 30.f * dir->z * dir->z + 3.0f);
	out[21] = 0.375f * sqrt(10.0f / M_PI) * dir->x * dir->z * (3.0f - 7.0f * dir->z * dir->z);
	out[22] = 0.375f * sqrt(5.0f / M_PI) * (dir->x * dir->x - dir->y * dir->y) * (7.0f * dir->z * dir->z - 1.0f);
	out[23] = 3.0 * dir->z * out[15];
	out[24] = 3.0f / 16.0f * sqrt(35.0f / M_PI) * (dir->x * dir->x * dir->x * dir->x - 6.0f * dir->x * dir->x * dir->y * dir->y + dir->y * dir->y * dir->y * dir->y);
	if (order == 5)
		return out;

	out[25] = -3.0f / 32.0f * sqrt(154.0f / M_PI) * dir->y * (5.0f * dir->x * dir->x * dir->x * dir->x - 10.0f * dir->x * dir->x * dir->y * dir->y + dir->y * dir->y * dir->y * dir->y);
	out[26] = 0.75f * sqrt(385.0f / M_PI) * dir->x * dir->y * dir->z * (dir->x * dir->x - dir->y * dir->y);
	out[27] = sqrt(770.0f / M_PI) / 32.0f * dir->y * (3.0f * dir->x * dir->x - dir->y * dir->y) * (1.0f - 9.0f * dir->z * dir->z);
	out[28] = sqrt(1155.0f / M_PI) / 4.0f * dir->x * dir->y * dir->z * (3.0f * dir->z * dir->z - 1.0f);
	out[29] = sqrt(165.0f / M_PI) / 16.0f * dir->y * (14.0f * dir->z * dir->z - 21.0f * dir->z * dir->z * dir->z * dir->z - 1.0f);
	out[30] = sqrt(11.0f / M_PI) / 16.0f * dir->z * (63.0f * dir->z * dir->z * dir->z * dir->z - 70.0f * dir->z * dir->z + 15.0f);
	out[31] = sqrt(165.0f / M_PI) / 16.0f * dir->x * (14.0f * dir->z * dir->z - 21.0f * dir->z * dir->z * dir->z * dir->z - 1.0f);
	out[32] = sqrt(1155.0f / M_PI) / 8.0f * dir->z * (dir->x * dir->x - dir->y * dir->y) * (3.0f * dir->z * dir->z - 1.0f);
	out[33] = sqrt(770.0f / M_PI) / 32.0f * dir->x * (dir->x * dir->x - 3.0f * dir->y * dir->y) * (1.0f - 9.0f * dir->z * dir->z);
	out[34] = 3.0f / 16.0f * sqrt(385.0f / M_PI) * dir->z * (dir->x * dir->x * dir->x * dir->x - 6.0 * dir->x * dir->x * dir->y * dir->y + dir->y * dir->y * dir->y * dir->y);
	out[35] = -3.0f / 32.0f * sqrt(154.0f / M_PI) * dir->x * (dir->x * dir->x * dir->x * dir->x - 10.0f * dir->x * dir->x * dir->y * dir->y + 5.0f * dir->y * dir->y * dir->y * dir->y);

	return out;
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
	paraboloidDualDesc.attach(_paraboloidFrontMap);
	paraboloidDualDesc.attach(_paraboloidBackMap);
	_paraboloidDualViews = pipeline.createFramebuffer(paraboloidDualDesc);
	if (!_paraboloidDualViews)
		return false;

	GraphicsFramebufferDesc irradianceSHCoefficientsDesc;
	irradianceSHCoefficientsDesc.attach(_paraboloidFrontMap);
	irradianceSHCoefficientsDesc.attach(_paraboloidBackMap);
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
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsColor, float4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(_irradianceParaboloid);

	pipeline.setFramebuffer(_irradianceSHCoefficientsView);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsColor, float4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(_irradianceProjectDualParaboloidToSH);
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
EnvironmentIrradiance::_paraboloidCoord(Vector3& vec, int face, const Vector2& uv) noexcept
{
	//  sphere direction is the reflection of the orthographic view direction (determined by
	//  face), reflected about the normal to the paraboloid at uv
	double r_sqr = uv.x * uv.x + uv.y * uv.y;

	if (r_sqr > 1.0)
		return false;

	Vector3 axis;
	if (face == 0)
		axis = Vector3(0.f, 0.f, -1.f);
	else
		axis = Vector3(0.f, 0.f, 1.f);

	// compute normal on the parabaloid at uv
	Vector3 N(uv.x, uv.y, 1.f);
	N = math::normalize(N);

	// reflect axis around N, to compute sphere direction
	float v_dot_n = math::dot(axis, N);
	Vector3 R = axis - 2 * v_dot_n*N;

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

	std::size_t s, t;
	//  paraboloids are symmetrical, so compute total diff. solid angle for one half, and double it.
	for (t = 0; t < size; t++)
	{
		for (s = 0; s < size; s++)
		{
			double x = ((s + 0.5) / double(size))*2. - 1.;
			double y = ((t + 0.5) / double(size))*2. - 1.;
			double r_sqr = x*x + y*y;

			int index = t*size + s;
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

	float* basisProj = new float[order*order];

	for (int face = 0; face < 2; face++)
	{
		float *coefficients;
		coefficients = new float[size * size * size * size];
		std::memset(coefficients, 0, size*size*size*size*sizeof(float));

		for (t = 0; t < size; t++)
		{
			for (s = 0; s < size; s++)
			{
				Vector3 parabVec;
				double sd = ((s + 0.5) / double(size))*2. - 1.;
				double td = ((t + 0.5) / double(size))*2. - 1.;
				Vector2 stVec((float)sd, (float)td);

				if (!_paraboloidCoord(parabVec, face, stVec))
					continue;   //  skip if this texel is outside the paraboloid

				//  compute the N^2 spherical harmonic basis functions
				D3DXSHEvalDirection(basisProj, order, &parabVec);

				int basis = 0;
				int index = t*size + s;
				for (int l = 0; l < (int)order; l++)
				{
					for (int m = -l; m <= l; m++, basis++)
					{
						int tiley = basis / order;
						int tilex = basis % order;
						double Ylm = basisProj[l*(l + 1) + m];

						int offset = ((tiley*size + t)*size*nSize) + tilex*size + s;
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

	delete[] basisProj;
	delete[] d_omega;
	return true;
}

_NAME_END