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
#include "LightMassBaking.h"

#include <gl\glew.h>

_NAME_BEGIN

typedef float(*lm_weight_func)(float cos_theta, void *userdata);

struct lm_context
{
	struct
	{
		float4x4 transform;
		const unsigned char* positions;
		int positionsType;
		int positionsStride;
		const unsigned char* uvs;
		int uvsType;
		int uvsStride;
		const unsigned char* indices;
		int indicesType;
		unsigned int count;
	} mesh;

	struct
	{
		int pass;
		int passCount;

		struct
		{
			std::uint32_t baseIndex;
			float3 p[3];
			float2 uv[3];
		} triangle;

		struct
		{
			int minx, miny;
			int maxx, maxy;
			int x, y;
		} rasterizer;

		struct
		{
			float3 position;
			float3 direction;
			float3 up;
		} sample;

		struct
		{
			int side;
		} hemisphere;
	} meshPosition;

	struct
	{
		std::int32_t width;
		std::int32_t height;
		std::uint8_t channels;
		float *data;
	} lightmap;

	struct
	{
		unsigned int size;
		float zNear, zFar;
		struct { float r, g, b; } clearColor;

		unsigned int fbHemiCountX;
		unsigned int fbHemiCountY;
		unsigned int fbHemiIndex;
		std::unique_ptr<int2[]> fbHemiToLightmapLocation;
		GLuint fbTexture[2];
		GLuint fb[2];
		GLuint fbDepth;
		GLuint vao;
		struct
		{
			GLuint vs;
			GLuint fs;
			GLuint programID;
			GLuint hemispheresTextureID;
			GLuint weightsTextureID;
			GLuint weightsTexture;
		} firstPass;
		struct
		{
			GLuint vs;
			GLuint fs;
			GLuint programID;
			GLuint hemispheresTextureID;
		} downsamplePass;
		struct
		{
			GLuint pbo;
			bool pboTransferStarted;
			unsigned int fbHemiCount;
			std::unique_ptr<int2[]> fbHemiToLightmapLocation;
		} transfer;
	} hemisphere;

	float interpolationThreshold;

	~lm_context()
	{
		glDeleteTextures(1, &hemisphere.firstPass.weightsTexture);
		glDeleteBuffers(1, &hemisphere.transfer.pbo);

		glDeleteShader(hemisphere.downsamplePass.vs);
		glDeleteShader(hemisphere.downsamplePass.fs);
		glDeleteProgram(hemisphere.downsamplePass.programID);

		glDeleteShader(hemisphere.firstPass.vs);
		glDeleteShader(hemisphere.firstPass.fs);
		glDeleteProgram(hemisphere.firstPass.programID);

		glDeleteVertexArrays(1, &hemisphere.vao);
		glDeleteRenderbuffers(1, &hemisphere.fbDepth);
		glDeleteFramebuffers(2, hemisphere.fb);
		glDeleteTextures(2, hemisphere.fbTexture);
	}
};

int 
LightMassBaking::leftOf(const float2& a, const float2& b, const float2& c)
{
	float2 lv = b - a;
	float2 rv = c - b;

	float x = lv.x * rv.y - lv.y * rv.x;
	return x < 0 ? -1 : x > 0;
}

int 
LightMassBaking::convexClip(float2* poly, int nPoly, const float2* clip, int nClip, float2* res)
{
	int nRes = nPoly;
	int dir = leftOf(clip[0], clip[1], clip[2]);
	for (int i = 0, j = nClip - 1; i < nClip && nRes; j = i++)
	{
		if (i != 0)
			for (nPoly = 0; nPoly < nRes; nPoly++)
				poly[nPoly] = res[nPoly];
		nRes = 0;
		float2 v0 = poly[nPoly - 1];
		int side0 = leftOf(clip[j], clip[i], v0);
		if (side0 != -dir)
			res[nRes++] = v0;
		for (int k = 0; k < nPoly; k++)
		{
			float2 v1 = poly[k], x;
			int side1 = leftOf(clip[j], clip[i], v1);
			if (side0 + side1 == 0 && side0 && math::lineIntersection(clip[j], clip[i], v0, v1, x))
				res[nRes++] = x;
			if (k == nPoly - 1)
				break;
			if (side1 != -dir)
				res[nRes++] = v1;
			v0 = v1;
			side0 = side1;
		}
	}

	return nRes;
}

std::size_t
LightMassBaking::passStepSize()
{
	std::size_t shift = _ctx->meshPosition.passCount / 3 - (_ctx->meshPosition.pass - 1) / 3;
	std::size_t step = (1 << shift);
	assert(step > 0);
	return step;
}

std::size_t
LightMassBaking::passOffsetX()
{
	if (!_ctx->meshPosition.pass)
		return 0;
	int passType = (_ctx->meshPosition.pass - 1) % 3;
	unsigned int halfStep = passStepSize() >> 1;
	return passType != 1 ? halfStep : 0;
}

std::size_t
LightMassBaking::passOffsetY()
{
	if (!_ctx->meshPosition.pass)
		return 0;

	int passType = (_ctx->meshPosition.pass - 1) % 3;
	std::size_t halfStep = passStepSize() >> 1;
	return passType != 0 ? halfStep : 0;
}

bool 
LightMassBaking::hasConservativeTriangleRasterizerFinished()
{
	return _ctx->meshPosition.rasterizer.y >= _ctx->meshPosition.rasterizer.maxy;
}

void 
LightMassBaking::moveToNextPotentialConservativeTriangleRasterizerPosition()
{
	unsigned int step = passStepSize();

	_ctx->meshPosition.rasterizer.x += step;

	while (_ctx->meshPosition.rasterizer.x >= _ctx->meshPosition.rasterizer.maxx)
	{
		_ctx->meshPosition.rasterizer.x = _ctx->meshPosition.rasterizer.minx + this->passOffsetX();
		_ctx->meshPosition.rasterizer.y += step;

		if (this->hasConservativeTriangleRasterizerFinished())
			break;
	}
}

float*
LightMassBaking::getLightmapPixel(int x, int y)
{
	assert(x >= 0 && x < _ctx->lightmap.width && y >= 0 && y < _ctx->lightmap.height);
	return _ctx->lightmap.data + (y * _ctx->lightmap.width + x) * _ctx->lightmap.channels;
}

void 
LightMassBaking::setLightmapPixel(int x, int y, float* in)
{
	assert(x >= 0 && x < _ctx->lightmap.width && y >= 0 && y < _ctx->lightmap.height);
	float *p = _ctx->lightmap.data + (y * _ctx->lightmap.width + x) * _ctx->lightmap.channels;
	for (int j = 0; j < _ctx->lightmap.channels; j++)
		*p++ = *in++;
}

bool 
LightMassBaking::trySamplingConservativeTriangleRasterizerPosition()
{
	if (hasConservativeTriangleRasterizerFinished())
		return false;

	// check if lightmap pixel was already set
	float *pixelValue = getLightmapPixel(_ctx->meshPosition.rasterizer.x, _ctx->meshPosition.rasterizer.y);
	for (int j = 0; j < _ctx->lightmap.channels; j++)
		if (pixelValue[j] != 0.0f)
			return false;

	// try to interpolate from neighbors:
	if (_ctx->meshPosition.pass > 0)
	{
		float *neighbors[4];
		int neighborCount = 0;
		int neighborsExpected = 0;
		int d = (int)this->passStepSize() / 2;
		int dirs = ((_ctx->meshPosition.pass - 1) % 3) + 1;
		if (dirs & 1) // check x-neighbors with distance d
		{
			neighborsExpected += 2;
			if (_ctx->meshPosition.rasterizer.x - d >= _ctx->meshPosition.rasterizer.minx &&
				_ctx->meshPosition.rasterizer.x + d <  _ctx->meshPosition.rasterizer.maxx)
			{
				neighbors[neighborCount++] = this->getLightmapPixel(_ctx->meshPosition.rasterizer.x - d, _ctx->meshPosition.rasterizer.y);
				neighbors[neighborCount++] = this->getLightmapPixel(_ctx->meshPosition.rasterizer.x + d, _ctx->meshPosition.rasterizer.y);
			}
		}
		if (dirs & 2) // check y-neighbors with distance d
		{
			neighborsExpected += 2;
			if (_ctx->meshPosition.rasterizer.y - d >= _ctx->meshPosition.rasterizer.miny &&
				_ctx->meshPosition.rasterizer.y + d <  _ctx->meshPosition.rasterizer.maxy)
			{
				neighbors[neighborCount++] = this->getLightmapPixel(_ctx->meshPosition.rasterizer.x, _ctx->meshPosition.rasterizer.y - d);
				neighbors[neighborCount++] = this->getLightmapPixel(_ctx->meshPosition.rasterizer.x, _ctx->meshPosition.rasterizer.y + d);
			}
		}
		if (neighborCount == neighborsExpected) // are all interpolation neighbors available?
		{
			// calculate average neighbor pixel value
			float avg[4] = { 0 };
			for (int i = 0; i < neighborCount; i++)
				for (int j = 0; j < _ctx->lightmap.channels; j++)
					avg[j] += neighbors[i][j];
			float ni = 1.0f / neighborCount;
			for (int j = 0; j < _ctx->lightmap.channels; j++)
				avg[j] *= ni;

			// check if error from average pixel to neighbors is above the interpolation threshold
			bool interpolate = true;
			for (int i = 0; i < neighborCount; i++)
			{
				bool zero = true;
				for (int j = 0; j < _ctx->lightmap.channels; j++)
				{
					if (neighbors[i][j] != 0.0f)
						zero = false;
					if (fabs(neighbors[i][j] - avg[j]) > _ctx->interpolationThreshold)
						interpolate = false;
				}
				if (zero)
					interpolate = false;
				if (!interpolate)
					break;
			}

			// set interpolated value and return if interpolation is acceptable
			if (interpolate)
			{
				setLightmapPixel(_ctx->meshPosition.rasterizer.x, _ctx->meshPosition.rasterizer.y, avg);
				return false;
			}
		}
	}

	float2 pixel[16];
	pixel[0].set(_ctx->meshPosition.rasterizer.x, _ctx->meshPosition.rasterizer.y);
	pixel[1].set(_ctx->meshPosition.rasterizer.x + 1, _ctx->meshPosition.rasterizer.y);
	pixel[2].set(_ctx->meshPosition.rasterizer.x + 1, _ctx->meshPosition.rasterizer.y + 1);
	pixel[3].set(_ctx->meshPosition.rasterizer.x, _ctx->meshPosition.rasterizer.y + 1);

	float2 res[16];
	int nRes = convexClip(pixel, 4, _ctx->meshPosition.triangle.uv, 3, res);
	if (nRes > 0)
	{
		float2 centroid = res[0];
		float area = res[nRes - 1].x * res[0].y - res[nRes - 1].y * res[0].x;
		for (int i = 1; i < nRes; i++)
		{
			centroid = centroid + res[i];
			area += res[i - 1].x * res[i].y - res[i - 1].y * res[i].x;
		}
		centroid = centroid / (float)nRes;
		area = std::abs(area / 2.0f);

		if (area > 0.0f)
		{
			float2 uv = math::barycentric(_ctx->meshPosition.triangle.uv[0], _ctx->meshPosition.triangle.uv[1], _ctx->meshPosition.triangle.uv[2], centroid);

			if (math::isfinite(uv))
			{
				float3 p0 = _ctx->meshPosition.triangle.p[0];
				float3 p1 = _ctx->meshPosition.triangle.p[1];
				float3 p2 = _ctx->meshPosition.triangle.p[2];
				float3 v1 = p1 - p0;
				float3 v2 = p2 - p0;
				_ctx->meshPosition.sample.position = p0 + v2 * uv.x + v1 * uv.y;
				_ctx->meshPosition.sample.direction = math::normalize(math::cross(v1, v2));

				if (math::isfinite(_ctx->meshPosition.sample.position) &&
					math::isfinite(_ctx->meshPosition.sample.direction) &&
					math::length2(_ctx->meshPosition.sample.direction) > 0.5f) // don't allow 0.0f. should always be ~1.0f
				{
					float3 up = float3::UnitY;
					if (std::abs(math::dot(up, _ctx->meshPosition.sample.direction)) > 0.8f)
						up = float3::UnitZ;

					float3 side = math::normalize(math::cross(up, _ctx->meshPosition.sample.direction));
					up = math::normalize(math::cross(side, _ctx->meshPosition.sample.direction));
					int rx = _ctx->meshPosition.rasterizer.x % 3;
					int ry = _ctx->meshPosition.rasterizer.y % 3;

					const float baseAngle = 0.03f * M_PI;
					const float baseAngles[3][3] = {
						{ baseAngle, baseAngle + 1.0f / 3.0f, baseAngle + 2.0f / 3.0f },
						{ baseAngle + 1.0f / 3.0f, baseAngle + 2.0f / 3.0f, baseAngle },
						{ baseAngle + 2.0f / 3.0f, baseAngle, baseAngle + 1.0f / 3.0f }
					};
					float phi = 2.0f * M_PI * baseAngles[ry][rx] + 0.1f * ((float)rand() / (float)RAND_MAX);
					_ctx->meshPosition.sample.up = math::normalize(side * cosf(phi) + up * sinf(phi));

					return true;
				}
			}
		}
	}

	return false;
}

bool 
LightMassBaking::findFirstConservativeTriangleRasterizerPosition()
{
	while (!this->trySamplingConservativeTriangleRasterizerPosition())
	{
		this->moveToNextPotentialConservativeTriangleRasterizerPosition();
		if (this->hasConservativeTriangleRasterizerFinished())
			return false;
	}

	return true;
}

bool
LightMassBaking::findNextConservativeTriangleRasterizerPosition()
{
	this->moveToNextPotentialConservativeTriangleRasterizerPosition();
	return this->findFirstConservativeTriangleRasterizerPosition();
}

static float lm_defaultWeights(float cos_theta, void *userdata)
{
	return 1.0f;
}

void lmSetHemisphereWeights(lm_context *ctx, lm_weight_func f, void *userdata)
{
	float *weights = (float*)calloc(2 * 3 * ctx->hemisphere.size * ctx->hemisphere.size, sizeof(float));
	float center = (ctx->hemisphere.size - 1) * 0.5f;
	
	double sum = 0.0;

	for (std::uint32_t y = 0; y < ctx->hemisphere.size; y++)
	{
		float dy = 2.0f * (y - center) / (float)ctx->hemisphere.size;
		for (std::uint32_t x = 0; x < ctx->hemisphere.size; x++)
		{
			float dx = 2.0f * (x - center) / (float)ctx->hemisphere.size;
			float3 v = math::normalize(float3(dx, dy, 1.0f));

			float solidAngle = v.z * v.z * v.z;

			float *w0 = weights + 2 * (y * (3 * ctx->hemisphere.size) + x);
			float *w1 = w0 + 2 * ctx->hemisphere.size;
			float *w2 = w1 + 2 * ctx->hemisphere.size;

			// center weights
			w0[0] = solidAngle * f(v.z, userdata);
			w0[1] = solidAngle;

			// left/right side weights
			w1[0] = solidAngle * f(std::abs(v.x), userdata);
			w1[1] = solidAngle;

			// up/down side weights
			w2[0] = solidAngle * f(std::abs(v.y), userdata);
			w2[1] = solidAngle;

			sum += 3.0 * (double)solidAngle;
		}
	}

	float weightScale = (float)(1.0 / sum);
	for (unsigned int i = 0; i < 2 * 3 * ctx->hemisphere.size * ctx->hemisphere.size; i++)
		weights[i] *= weightScale;

	glBindTexture(GL_TEXTURE_2D, ctx->hemisphere.firstPass.weightsTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, 3 * ctx->hemisphere.size, ctx->hemisphere.size, 0, GL_RG, GL_FLOAT, weights);

	free(weights);
}

LightMassBaking::LightMassBaking() noexcept
{
}

LightMassBaking::~LightMassBaking() noexcept
{
}

void
LightMassBaking::setLightMassListener(LightMassListenerPtr pointer) noexcept
{
	_lightMassListener = pointer;
}

LightMassListenerPtr
LightMassBaking::getLightMassListener() const noexcept
{
	return _lightMassListener;
}

bool
LightMassBaking::baking(const LightBakingOptions& params) noexcept
{
	assert(params.lightMap.data);
	assert(params.lightMap.width >= 0 && params.lightMap.height >= 0);
	assert(params.lightMap.channel == 1 || params.lightMap.channel == 2 || params.lightMap.channel == 3 || params.lightMap.channel == 4);

	try
	{
		if (!this->setupBakeTools(params.baking))
		{
			if (_lightMassListener)
				_lightMassListener->onMessage("Could not initialize with BakeTools.");

			return false;
		}

		if (_lightMassListener)
			_lightMassListener->onBakingStart();

		GLenum faceType = GL_UNSIGNED_INT;
		if (params.model.sizeofIndices == 1)
			faceType = GL_UNSIGNED_BYTE;
		else if (params.model.sizeofIndices == 2)
			faceType = GL_UNSIGNED_SHORT;

		this->setRenderTarget(params.lightMap.data, params.lightMap.width, params.lightMap.height, params.lightMap.channel);
		this->setGeometry(float4x4::One,
			GL_FLOAT, params.model.vertices + params.model.strideVertices, params.model.sizeofVertices,
			GL_FLOAT, params.model.vertices + params.model.strideTexcoord, params.model.sizeofVertices,
			params.model.numIndices, faceType, params.model.indices);

		Viewportt<int> vp;

		std::uint32_t baseIndex = 0;

		while (this->beginSampleHemisphere(vp.ptr(), _view, _project))
		{
			_viewProject = _project * _view;

			this->doSampleHemisphere(params, vp, _viewProject);

			if (baseIndex != _ctx->meshPosition.triangle.baseIndex)
			{
				if (_lightMassListener)
					_lightMassListener->onBakingProgressing(this->getSampleProcess());

				baseIndex = _ctx->meshPosition.triangle.baseIndex;
			}

			if (!this->endSampleHemisphere())
				return false;
		}

		if (_lightMassListener)
			_lightMassListener->onBakingEnd();

		this->closeBakeTools();
	}
	catch (...)
	{
		this->closeBakeTools();
	}

	return true;
}

bool
LightMassBaking::setupBakeTools(const LightBakingParams& params)
{
	assert(params.hemisphereSize == 16 || params.hemisphereSize == 32 || params.hemisphereSize == 64 || params.hemisphereSize == 128 || params.hemisphereSize == 256 || params.hemisphereSize == 512);
	assert(params.hemisphereNear < params.hemisphereFar && params.hemisphereNear > 0.0f);
	assert(params.interpolationPasses >= 0 && params.interpolationPasses <= 8);
	assert(params.interpolationThreshold >= 0.0f);

	auto context = std::make_unique<lm_context>();

	context->meshPosition.passCount = 1 + 3 * params.interpolationPasses;
	context->interpolationThreshold = params.interpolationThreshold;
	context->hemisphere.size = params.hemisphereSize;
	context->hemisphere.zNear = params.hemisphereNear;
	context->hemisphere.zFar = params.hemisphereFar;
	context->hemisphere.clearColor.r = params.clearColor.x;
	context->hemisphere.clearColor.g = params.clearColor.y;
	context->hemisphere.clearColor.b = params.clearColor.z;
	context->hemisphere.fbHemiCountX = 1536 / (3 * context->hemisphere.size);
	context->hemisphere.fbHemiCountY = 512 / context->hemisphere.size;

	std::uint32_t w[] = { context->hemisphere.fbHemiCountX * context->hemisphere.size * 3, context->hemisphere.fbHemiCountX * context->hemisphere.size / 2 };
	std::uint32_t h[] = { context->hemisphere.fbHemiCountY * context->hemisphere.size,     context->hemisphere.fbHemiCountY * context->hemisphere.size / 2 };

	glGenTextures(2, context->hemisphere.fbTexture);
	glGenFramebuffers(2, context->hemisphere.fb);
	glGenRenderbuffers(1, &context->hemisphere.fbDepth);

	glBindRenderbuffer(GL_RENDERBUFFER, context->hemisphere.fbDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w[0], h[0]);
	glBindFramebuffer(GL_FRAMEBUFFER, context->hemisphere.fb[0]);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, context->hemisphere.fbDepth);

	for (int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, context->hemisphere.fbTexture[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w[i], h[i], 0, GL_RGBA, GL_FLOAT, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, context->hemisphere.fb[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, context->hemisphere.fbTexture[i], 0);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			if (_lightMassListener)
				_lightMassListener->onMessage("Could not create framebuffer!");

			glDeleteRenderbuffers(1, &context->hemisphere.fbDepth);
			glDeleteFramebuffers(2, context->hemisphere.fb);
			glDeleteTextures(2, context->hemisphere.fbTexture);

			return false;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenVertexArrays(1, &context->hemisphere.vao);

	{
		const char *vs =
			"#version 150 core\n"
			"const vec2 ps[4] = vec2[](vec2(1, -1), vec2(1, 1), vec2(-1, -1), vec2(-1, 1));\n"
			"void main()\n"
			"{\n"
			"gl_Position = vec4(ps[gl_VertexID], 0, 1);\n"
			"}\n";
		const char *fs =
			"#version 150 core\n"
			"uniform sampler2D hemispheres;\n"
			"uniform sampler2D weights;\n"

			"layout(pixel_center_integer) in vec4 gl_FragCoord;\n" // whole integer values represent pixel centers, GL_ARB_fragment_coord_conventions

			"out vec4 outColor;\n"

			"vec4 weightedSample(ivec2 h_uv, ivec2 w_uv, ivec2 quadrant)\n"
			"{\n"
			"vec4 sample = texelFetch(hemispheres, h_uv + quadrant, 0);\n"
			"vec2 weight = texelFetch(weights, w_uv + quadrant, 0).rg;\n"
			"return vec4(sample.rgb * weight.r, sample.a * weight.g);\n"
			"}\n"

			"vec4 threeWeightedSamples(ivec2 h_uv, ivec2 w_uv, ivec2 offset)\n"
			"{\n" // horizontal triple sum
			"vec4 sum = weightedSample(h_uv, w_uv, offset);\n"
			"offset.x += 2;\n"
			"sum += weightedSample(h_uv, w_uv, offset);\n"
			"offset.x += 2;\n"
			"sum += weightedSample(h_uv, w_uv, offset);\n"
			"return sum;\n"
			"}\n"

			"void main()\n"
			"{\n" // this is a weighted sum downsampling pass (alpha component contains the weighted valid sample count)
			"vec2 in_uv = (gl_FragCoord.xy - vec2(0.5)) * vec2(6.0, 2.0) + vec2(0.01);\n"
			"ivec2 h_uv = ivec2(in_uv);\n"
			"ivec2 w_uv = ivec2(mod(in_uv, vec2(textureSize(weights, 0))));\n" // there's no integer modulo :(
			"vec4 lb = threeWeightedSamples(h_uv, w_uv, ivec2(0, 0));\n"
			"vec4 rb = threeWeightedSamples(h_uv, w_uv, ivec2(1, 0));\n"
			"vec4 lt = threeWeightedSamples(h_uv, w_uv, ivec2(0, 1));\n"
			"vec4 rt = threeWeightedSamples(h_uv, w_uv, ivec2(1, 1));\n"
			"outColor = lb + rb + lt + rt;\n"
			"}\n";

		context->hemisphere.firstPass.vs = loadShader(GL_VERTEX_SHADER, vs);
		if (!context->hemisphere.firstPass.vs)
			return GL_NONE;

		context->hemisphere.firstPass.fs = loadShader(GL_FRAGMENT_SHADER, fs);
		if (!context->hemisphere.firstPass.fs)
			return GL_NONE;

		context->hemisphere.firstPass.programID = loadProgram(context->hemisphere.firstPass.vs, context->hemisphere.firstPass.fs, 0, 0);
		if (!context->hemisphere.firstPass.programID)
		{
			if (_lightMassListener)
				_lightMassListener->onMessage("Error loading the hemisphere first pass shader program... leaving!");

			glDeleteVertexArrays(1, &context->hemisphere.vao);
			glDeleteRenderbuffers(1, &context->hemisphere.fbDepth);
			glDeleteFramebuffers(2, context->hemisphere.fb);
			glDeleteTextures(2, context->hemisphere.fbTexture);
			
			return false;
		}

		context->hemisphere.firstPass.hemispheresTextureID = glGetUniformLocation(context->hemisphere.firstPass.programID, "hemispheres");
		context->hemisphere.firstPass.weightsTextureID = glGetUniformLocation(context->hemisphere.firstPass.programID, "weights");
	}

	{
		const char *vs =
			"#version 150 core\n"
			"const vec2 ps[4] = vec2[](vec2(1, -1), vec2(1, 1), vec2(-1, -1), vec2(-1, 1));\n"
			"void main()\n"
			"{\n"
			"gl_Position = vec4(ps[gl_VertexID], 0, 1);\n"
			"}\n";
		const char *fs =
			"#version 150 core\n"
			"uniform sampler2D hemispheres;\n"

			"out vec4 outColor;\n"

			"void main()\n"
			"{\n"
			"ivec2 h_uv = ivec2((gl_FragCoord.xy - vec2(0.5)) * 2.0 + vec2(0.01));\n"
			"vec4 lb = texelFetch(hemispheres, h_uv + ivec2(0, 0), 0);\n"
			"vec4 rb = texelFetch(hemispheres, h_uv + ivec2(1, 0), 0);\n"
			"vec4 lt = texelFetch(hemispheres, h_uv + ivec2(0, 1), 0);\n"
			"vec4 rt = texelFetch(hemispheres, h_uv + ivec2(1, 1), 0);\n"
			"outColor = lb + rb + lt + rt;\n"
			"}\n";

		context->hemisphere.downsamplePass.vs = loadShader(GL_VERTEX_SHADER, vs);
		if (!context->hemisphere.downsamplePass.vs)
			return GL_NONE;

		context->hemisphere.downsamplePass.fs = loadShader(GL_FRAGMENT_SHADER, fs);
		if (!context->hemisphere.downsamplePass.fs)
			return GL_NONE;

		context->hemisphere.downsamplePass.programID = loadProgram(context->hemisphere.downsamplePass.vs, context->hemisphere.downsamplePass.fs, 0, 0);
		if (!context->hemisphere.downsamplePass.programID)
		{
			if (_lightMassListener)
				_lightMassListener->onMessage("Error loading the hemisphere downsample pass shader program... leaving!");

			glDeleteProgram(context->hemisphere.firstPass.programID);
			glDeleteVertexArrays(1, &context->hemisphere.vao);
			glDeleteRenderbuffers(1, &context->hemisphere.fbDepth);
			glDeleteFramebuffers(2, context->hemisphere.fb);
			glDeleteTextures(2, context->hemisphere.fbTexture);
			
			return NULL;
		}

		context->hemisphere.downsamplePass.hemispheresTextureID = glGetUniformLocation(context->hemisphere.downsamplePass.programID, "hemispheres");
	}

	glGenBuffers(1, &context->hemisphere.transfer.pbo);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, context->hemisphere.transfer.pbo);
	glBufferData(GL_PIXEL_PACK_BUFFER, context->hemisphere.fbHemiCountX * context->hemisphere.fbHemiCountY * 4 * sizeof(float), 0, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	glGenTextures(1, &context->hemisphere.firstPass.weightsTexture);
	lmSetHemisphereWeights(context.get(), lm_defaultWeights, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	context->hemisphere.fbHemiToLightmapLocation = std::make_unique<int2[]>(context->hemisphere.fbHemiCountX * context->hemisphere.fbHemiCountY);
	context->hemisphere.transfer.fbHemiToLightmapLocation = std::make_unique<int2[]>(context->hemisphere.fbHemiCountX * context->hemisphere.fbHemiCountY);

	_ctx = std::move(context);

	return true;
}

void 
LightMassBaking::closeBakeTools()
{
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	_ctx.reset();
}

void
LightMassBaking::setRenderTarget(float *outLightmap, int w, int h, int c)
{
	_ctx->lightmap.data = outLightmap;
	_ctx->lightmap.width = w;
	_ctx->lightmap.height = h;
	_ctx->lightmap.channels = c;
}

void 
LightMassBaking::setGeometry(const float4x4& world, int positionsType, const void *positionsXYZ, int positionsStride, int lightmapCoordsType, const void *lightmapCoordsUV, int lightmapCoordsStride, int count, int indicesType, const void *indices)
{
	_ctx->mesh.transform = world;
	_ctx->mesh.positions = (const unsigned char*)positionsXYZ;
	_ctx->mesh.positionsType = positionsType;
	_ctx->mesh.positionsStride = positionsStride == 0 ? sizeof(float3) : positionsStride;
	_ctx->mesh.uvs = (const unsigned char*)lightmapCoordsUV;
	_ctx->mesh.uvsType = lightmapCoordsType;
	_ctx->mesh.uvsStride = lightmapCoordsStride == 0 ? sizeof(float2) : lightmapCoordsStride;
	_ctx->mesh.indicesType = indicesType;
	_ctx->mesh.indices = (const unsigned char*)indices;
	_ctx->mesh.count = count;

	_ctx->meshPosition.pass = 0;

	this->setSamplePosition(0);
}

void
LightMassBaking::setSamplePosition(std::uint32_t indicesTriangleBaseIndex)
{
	float2 uvMin(FLT_MAX, FLT_MAX);
	float2 uvMax(-FLT_MAX, -FLT_MAX);
	float2 uvScale(_ctx->lightmap.width, _ctx->lightmap.height);

	_ctx->meshPosition.triangle.baseIndex = indicesTriangleBaseIndex;

	for (int i = 0; i < 3; i++)
	{
		std::uint32_t index;
		if (_ctx->mesh.indicesType == GL_UNSIGNED_INT)
			index = ((const std::uint32_t*)_ctx->mesh.indices + _ctx->meshPosition.triangle.baseIndex)[i];
		else if (_ctx->mesh.indicesType == GL_UNSIGNED_SHORT)
			index = ((const std::uint16_t*)_ctx->mesh.indices + _ctx->meshPosition.triangle.baseIndex)[i];
		else if (_ctx->mesh.indicesType == GL_UNSIGNED_BYTE)
			index = ((const std::uint8_t*)_ctx->mesh.indices + _ctx->meshPosition.triangle.baseIndex)[i];
		else
			index = _ctx->meshPosition.triangle.baseIndex + i;

		float3 p;
		if (_ctx->mesh.positionsType == GL_FLOAT)
			p = *(const float3*)(_ctx->mesh.positions + index * _ctx->mesh.positionsStride);
		else if (_ctx->mesh.positionsType == GL_UNSIGNED_INT)
			p = float3((const std::uint32_t*)(_ctx->mesh.positions + index * _ctx->mesh.positionsStride));
		else if (_ctx->mesh.positionsType == GL_UNSIGNED_SHORT)
			p = float3((const std::uint16_t*)(_ctx->mesh.positions + index * _ctx->mesh.positionsStride));
		else
			p = float3((const std::uint8_t*)(_ctx->mesh.positions + index * _ctx->mesh.positionsStride));

		float2 uv;
		if (_ctx->mesh.uvsType == GL_FLOAT)
			uv = *(const float2*)(_ctx->mesh.uvs + index * _ctx->mesh.uvsStride);
		else if (_ctx->mesh.uvsType == GL_UNSIGNED_INT)
			uv = float2((const std::uint32_t*)(_ctx->mesh.uvs + index * _ctx->mesh.uvsStride)) / (float)UINT_MAX;
		else if (_ctx->mesh.uvsType == GL_UNSIGNED_SHORT)
			uv = float2((const std::uint16_t*)(_ctx->mesh.uvs + index * _ctx->mesh.uvsStride)) / (float)USHRT_MAX;
		else
			uv = float2((const std::uint8_t*)(_ctx->mesh.uvs + index * _ctx->mesh.uvsStride)) / (float)UCHAR_MAX;

		_ctx->meshPosition.triangle.uv[i] = uv * uvScale;
		_ctx->meshPosition.triangle.p[i] = _ctx->mesh.transform * p;

		uvMin = math::min(uvMin, _ctx->meshPosition.triangle.uv[i]);
		uvMax = math::max(uvMax, _ctx->meshPosition.triangle.uv[i]);
	}

	int2 bbMin((int2)math::floor(uvMin));
	int2 bbMax((int2)math::ceil(uvMax));

	_ctx->meshPosition.rasterizer.minx = math::max(bbMin.x - 1, 0);
	_ctx->meshPosition.rasterizer.miny = std::max(bbMin.y - 1, 0);
	_ctx->meshPosition.rasterizer.maxx = std::min(bbMax.x + 1, _ctx->lightmap.width);
	_ctx->meshPosition.rasterizer.maxy = std::min(bbMax.y + 1, _ctx->lightmap.height);
	_ctx->meshPosition.rasterizer.x = _ctx->meshPosition.rasterizer.minx + this->passOffsetX();
	_ctx->meshPosition.rasterizer.y = _ctx->meshPosition.rasterizer.miny + this->passOffsetY();

	assert(_ctx->meshPosition.rasterizer.minx < _ctx->meshPosition.rasterizer.maxx && _ctx->meshPosition.rasterizer.miny < _ctx->meshPosition.rasterizer.maxy);

	if (_ctx->meshPosition.rasterizer.x <= _ctx->meshPosition.rasterizer.maxx &&
		_ctx->meshPosition.rasterizer.y <= _ctx->meshPosition.rasterizer.maxy &&
		this->findFirstConservativeTriangleRasterizerPosition())
		_ctx->meshPosition.hemisphere.side = 0;
	else
		_ctx->meshPosition.hemisphere.side = 5;
}

void
LightMassBaking::beginProcessHemisphereBatch()
{
	if (!_ctx->hemisphere.fbHemiIndex)
		return;

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(_ctx->hemisphere.vao);

	int fbRead = 0;
	int fbWrite = 1;

	int outHemiSize = _ctx->hemisphere.size / 2;
	glBindFramebuffer(GL_FRAMEBUFFER, _ctx->hemisphere.fb[fbWrite]);

	glViewport(0, 0, outHemiSize * _ctx->hemisphere.fbHemiCountX, outHemiSize * _ctx->hemisphere.fbHemiCountY);

	glUseProgram(_ctx->hemisphere.firstPass.programID);
	glUniform1i(_ctx->hemisphere.firstPass.hemispheresTextureID, 0);
	glUniform1i(_ctx->hemisphere.firstPass.weightsTextureID, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _ctx->hemisphere.fbTexture[fbRead]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _ctx->hemisphere.firstPass.weightsTexture);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(_ctx->hemisphere.downsamplePass.programID);
	glUniform1i(_ctx->hemisphere.downsamplePass.hemispheresTextureID, 0);

	while (outHemiSize > 1)
	{
		std::swap(fbRead, fbWrite);
		outHemiSize /= 2;
		glBindFramebuffer(GL_FRAMEBUFFER, _ctx->hemisphere.fb[fbWrite]);
		glViewport(0, 0, outHemiSize * _ctx->hemisphere.fbHemiCountX, outHemiSize * _ctx->hemisphere.fbHemiCountY);
		glBindTexture(GL_TEXTURE_2D, _ctx->hemisphere.fbTexture[fbRead]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (fbWrite == 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		std::swap(fbRead, fbWrite);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _ctx->hemisphere.fb[fbRead]);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _ctx->hemisphere.fb[fbWrite]);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(
			0, 0, _ctx->hemisphere.fbHemiCountX, _ctx->hemisphere.fbHemiCountY,
			0, 0, _ctx->hemisphere.fbHemiCountX, _ctx->hemisphere.fbHemiCountY,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, _ctx->hemisphere.fb[fbWrite]);
	}

	glBindBuffer(GL_PIXEL_PACK_BUFFER, _ctx->hemisphere.transfer.pbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _ctx->hemisphere.fb[1]);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
	glReadPixels(0, 0, _ctx->hemisphere.fbHemiCountX, _ctx->hemisphere.fbHemiCountY, GL_RGBA, GL_FLOAT, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	_ctx->hemisphere.transfer.pboTransferStarted = true;
	_ctx->hemisphere.transfer.fbHemiCount = _ctx->hemisphere.fbHemiIndex;
	_ctx->hemisphere.transfer.fbHemiToLightmapLocation.swap(_ctx->hemisphere.fbHemiToLightmapLocation);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);

	_ctx->hemisphere.fbHemiIndex = 0;
}

bool
LightMassBaking::finishProcessHemisphereBatch()
{
	if (!_ctx->hemisphere.transfer.pboTransferStarted)
		return true;

	glBindBuffer(GL_PIXEL_PACK_BUFFER, _ctx->hemisphere.transfer.pbo);

	float *hemi = (float*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	if (!hemi)
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Could not map hemisphere buffer.");

		return false;
	}

	std::uint32_t hemiIndex = 0;
	for (std::uint32_t hy = 0; hy < _ctx->hemisphere.fbHemiCountY; hy++)
	{
		for (std::uint32_t hx = 0; hx < _ctx->hemisphere.fbHemiCountX; hx++)
		{
			float *c = hemi + (hy * _ctx->hemisphere.fbHemiCountX + hx) * 4;
			float validity = c[3];

			const int2& lmUV = _ctx->hemisphere.transfer.fbHemiToLightmapLocation[hy * _ctx->hemisphere.fbHemiCountX + hx];
			float *lm = _ctx->lightmap.data + (lmUV.y * _ctx->lightmap.width + lmUV.x) * _ctx->lightmap.channels;
			if (!lm[0] && validity > 0.9)
			{
				float scale = 1.0f / validity;
				switch (_ctx->lightmap.channels)
				{
				case 1:
					lm[0] = std::max((c[0] + c[1] + c[2]) * scale / 3.0f, FLT_MIN);
					break;
				case 2:
					lm[0] = std::max((c[0] + c[1] + c[2]) * scale / 3.0f, FLT_MIN);
					lm[1] = 1.0f;
					break;
				case 3:
					lm[0] = std::max(c[0] * scale, FLT_MIN);
					lm[1] = std::max(c[1] * scale, FLT_MIN);
					lm[2] = std::max(c[2] * scale, FLT_MIN);
					break;
				case 4:
					lm[0] = std::max(c[0] * scale, FLT_MIN);
					lm[1] = std::max(c[1] * scale, FLT_MIN);
					lm[2] = std::max(c[2] * scale, FLT_MIN);
					lm[3] = 1.0f;
					break;
				default:
					assert(false);
					break;
				}
			}

			if (++hemiIndex == _ctx->hemisphere.transfer.fbHemiCount)
				goto done;
		}
	}
done:
	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	_ctx->hemisphere.transfer.pboTransferStarted = false;

	return true;
}

void
LightMassBaking::updateSampleMatrices(float4x4& view, float3 pos, float3 dir, const float3& up, float4x4& proj, float l, float r, float b, float t, float n, float f)
{
	// view matrix: lookAt(pos, pos + dir, up)
	float3 side = math::cross(dir, up);
	//up = cross(side, dir);
	dir = -dir; pos = -pos;
	view.a1 = side.x; view.a2 = up.x; view.a3 = dir.x; view.a4 = 0.0f;
	view.b1 = side.y; view.b2 = up.y; view.b3 = dir.y; view.b4 = 0.0f;
	view.c1 = side.z; view.c2 = up.z; view.c3 = dir.z; view.c4 = 0.0f;
	view.d1 = math::dot(side, pos); view.d2 = math::dot(up, pos); view.d3 = math::dot(dir, pos); view.d4 = 1.0f;

	// projection matrix: frustum(l, r, b, t, n, f)
	float ilr = 1.0f / (r - l), ibt = 1.0f / (t - b), ninf = -1.0f / (f - n), n2 = 2.0f * n;
	proj.a1 = n2 * ilr;      proj.a2 = 0.0f;          proj.a3 = 0.0f;           proj.a4 = 0.0f;
	proj.b1 = 0.0f;          proj.b2 = n2 * ibt;      proj.b3 = 0.0f;           proj.b4 = 0.0f;
	proj.c1 = (r + l) * ilr; proj.c2 = (t + b) * ibt; proj.c3 = (f + n) * ninf; proj.c4 = -1.0f;
	proj.d1 = 0.0f;         proj.d2 = 0.0f;         proj.d3 = f * n2 * ninf;  proj.d4 = 0.0f;
}

bool
LightMassBaking::updateSampleHemisphere(int* viewport, float4x4& view, float4x4& proj)
{
	if (_ctx->meshPosition.hemisphere.side >= 5)
		return false;

	if (_ctx->meshPosition.hemisphere.side == 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _ctx->hemisphere.fb[0]);

		if (_ctx->hemisphere.fbHemiIndex == 0)
		{
			glClearColor(_ctx->hemisphere.clearColor.r, _ctx->hemisphere.clearColor.g, _ctx->hemisphere.clearColor.b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		_ctx->hemisphere.fbHemiToLightmapLocation[_ctx->hemisphere.fbHemiIndex].set(_ctx->meshPosition.rasterizer.x, _ctx->meshPosition.rasterizer.y);
	}

	int x = (_ctx->hemisphere.fbHemiIndex % _ctx->hemisphere.fbHemiCountX) * _ctx->hemisphere.size * 3;
	int y = (_ctx->hemisphere.fbHemiIndex / _ctx->hemisphere.fbHemiCountX) * _ctx->hemisphere.size;

	int size = _ctx->hemisphere.size;
	float zNear = _ctx->hemisphere.zNear;
	float zFar = _ctx->hemisphere.zFar;

	const float3& pos = _ctx->meshPosition.sample.position;
	const float3& dir = _ctx->meshPosition.sample.direction;
	const float3& up = _ctx->meshPosition.sample.up;
	float3 right = math::cross(dir, up);

	switch (_ctx->meshPosition.hemisphere.side)
	{
	case 0: // center
		viewport[0] = x;
		viewport[1] = y;
		viewport[2] = size;
		viewport[3] = size;
		this->updateSampleMatrices(view, pos, dir, up, proj, -zNear, zNear, -zNear, zNear, zNear, zFar);
		break;
	case 1: // right
		viewport[0] = size + x;
		viewport[1] = y;
		viewport[2] = size / 2;
		viewport[3] = size;
		this->updateSampleMatrices(view, pos, right, up, proj, -zNear, 0.0f, -zNear, zNear, zNear, zFar);
		break;
	case 2: // left
		viewport[0] = size + x + size / 2;
		viewport[1] = y;
		viewport[2] = size / 2;
		viewport[3] = size;
		this->updateSampleMatrices(view, pos, -right, up, proj, 0.0f, zNear, -zNear, zNear, zNear, zFar);
		break;
	case 3: // down
		viewport[0] = 2 * size + x;
		viewport[1] = y + size / 2;
		viewport[2] = size;
		viewport[3] = size / 2;
		this->updateSampleMatrices(view, pos, -up, dir, proj, -zNear, zNear, 0.0f, zNear, zNear, zFar);
		break;
	case 4: // up
		viewport[0] = 2 * size + x;
		viewport[1] = y;
		viewport[2] = size;
		viewport[3] = size / 2;
		this->updateSampleMatrices(view, pos, up, -dir, proj, -zNear, zNear, -zNear, 0.0f, zNear, zFar);
		break;
	default:
		assert(false);
		break;
	}

	return true;
}

bool
LightMassBaking::beginSampleHemisphere(int* outViewport4, float4x4& view, float4x4& proj)
{
	assert(_ctx->meshPosition.triangle.baseIndex < _ctx->mesh.count);

	while (!this->updateSampleHemisphere(outViewport4, view, proj))
	{
		if (this->findNextConservativeTriangleRasterizerPosition())
		{
			_ctx->meshPosition.hemisphere.side = 0;
		}
		else
		{
			if (_ctx->meshPosition.triangle.baseIndex + 3 < _ctx->mesh.count)
			{
				this->setSamplePosition(_ctx->meshPosition.triangle.baseIndex + 3);
			}
			else
			{
				if (!this->finishProcessHemisphereBatch())
					return false;

				this->beginProcessHemisphereBatch();

				if (!this->finishProcessHemisphereBatch())
					return false;

				if (++_ctx->meshPosition.pass == _ctx->meshPosition.passCount)
				{
					_ctx->meshPosition.pass = 0;
					_ctx->meshPosition.triangle.baseIndex = _ctx->mesh.count;

					return false;
				}

				this->setSamplePosition(0);
			}
		}
	}

	return true;
}

bool
LightMassBaking::endSampleHemisphere()
{
	if (++_ctx->meshPosition.hemisphere.side == 5)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (++_ctx->hemisphere.fbHemiIndex == _ctx->hemisphere.fbHemiCountX * _ctx->hemisphere.fbHemiCountY)
		{
			if (!this->finishProcessHemisphereBatch())
				return false;

			this->beginProcessHemisphereBatch();
		}
	}

	return true;
}

float
LightMassBaking::getSampleProcess() noexcept
{
	float passProgress = (float)_ctx->meshPosition.triangle.baseIndex / (float)_ctx->mesh.count;
	return ((float)_ctx->meshPosition.pass + passProgress) / (float)_ctx->meshPosition.passCount;
}

std::uint32_t
LightMassBaking::loadShader(std::uint32_t type, const char *source)
{
	GLuint shader = glCreateShader(type);
	if (shader == GL_NONE)
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Could not create shader.");

		return GL_NONE;
	}

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		if (_lightMassListener)
		{
			_lightMassListener->onMessage("Could not compile shader.");

			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			if (length)
			{
				std::string log(length, 0);
				glGetShaderInfoLog(shader, length, &length, (char*)log.data());

				if (_lightMassListener)
					_lightMassListener->onMessage(log);
			}
		}

		glDeleteShader(shader);
		return GL_NONE;
	}

	return shader;
}

std::uint32_t
LightMassBaking::loadProgram(std::uint32_t vs, std::uint32_t fs, const char **attributes, int attributeCount)
{
	assert(vs && fs);

	GLuint program = glCreateProgram();
	if (program == GL_NONE)
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Could not create program.");

		return GL_NONE;
	}

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	for (int i = 0; i < attributeCount; i++)
		glBindAttribLocation(program, i, attributes[i]);

	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		if (_lightMassListener)
		{
			_lightMassListener->onMessage("Could not link program!");

			GLint length = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
			if (length)
			{
				std::string log((std::size_t)length, 0);
				glGetProgramInfoLog(program, length, &length, (GLchar*)log.data());

				_lightMassListener->onMessage(log);
			}
		}

		glDeleteProgram(program);

		return GL_NONE;
	}

	return program;
}

_NAME_END