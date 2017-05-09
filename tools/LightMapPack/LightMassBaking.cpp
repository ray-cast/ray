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
		const float *transform;
		const unsigned char *positions;
		int positionsType;
		int positionsStride;
		const unsigned char *uvs;
		int uvsType;
		int uvsStride;
		const unsigned char *indices;
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
		int width;
		int height;
		int channels;
		float *data;

#ifdef LM_DEBUG_INTERPOLATION
		unsigned char *debug;
#endif
	} lightmap;

	struct
	{
		unsigned int size;
		float zNear, zFar;
		struct { float r, g, b; } clearColor;

		unsigned int fbHemiCountX;
		unsigned int fbHemiCountY;
		unsigned int fbHemiIndex;
		int2* fbHemiToLightmapLocation;
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
			int2* fbHemiToLightmapLocation;
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

#ifdef LM_DEBUG_INTERPOLATION
		LM_FREE(ctx->lightmap.debug);
#endif

		free(hemisphere.transfer.fbHemiToLightmapLocation);
		free(hemisphere.fbHemiToLightmapLocation);
	}
};

static inline int lm_leftOf(float2 a, float2 b, float2 c)
{
	float2 lv = b - a;
	float2 rv = c - b;

	float x = lv.x * rv.y - lv.y * rv.x;
	return x < 0 ? -1 : x > 0;
}

static int lm_convexClip(float2* poly, int nPoly, const float2* clip, int nClip, float2* res)
{
	int nRes = nPoly;
	int dir = lm_leftOf(clip[0], clip[1], clip[2]);
	for (int i = 0, j = nClip - 1; i < nClip && nRes; j = i++)
	{
		if (i != 0)
			for (nPoly = 0; nPoly < nRes; nPoly++)
				poly[nPoly] = res[nPoly];
		nRes = 0;
		float2 v0 = poly[nPoly - 1];
		int side0 = lm_leftOf(clip[j], clip[i], v0);
		if (side0 != -dir)
			res[nRes++] = v0;
		for (int k = 0; k < nPoly; k++)
		{
			float2 v1 = poly[k], x;
			int side1 = lm_leftOf(clip[j], clip[i], v1);
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

static unsigned int lm_passStepSize(lm_context *ctx)
{
	unsigned int shift = ctx->meshPosition.passCount / 3 - (ctx->meshPosition.pass - 1) / 3;
	unsigned int step = (1 << shift);
	assert(step > 0);
	return step;
}

static unsigned int lm_passOffsetX(lm_context *ctx)
{
	if (!ctx->meshPosition.pass)
		return 0;
	int passType = (ctx->meshPosition.pass - 1) % 3;
	unsigned int halfStep = lm_passStepSize(ctx) >> 1;
	return passType != 1 ? halfStep : 0;
}

static unsigned int lm_passOffsetY(lm_context *ctx)
{
	if (!ctx->meshPosition.pass)
		return 0;
	int passType = (ctx->meshPosition.pass - 1) % 3;
	unsigned int halfStep = lm_passStepSize(ctx) >> 1;
	return passType != 0 ? halfStep : 0;
}

static bool lm_hasConservativeTriangleRasterizerFinished(lm_context *ctx)
{
	return ctx->meshPosition.rasterizer.y >= ctx->meshPosition.rasterizer.maxy;
}

static void lm_moveToNextPotentialConservativeTriangleRasterizerPosition(lm_context *ctx)
{
	unsigned int step = lm_passStepSize(ctx);
	ctx->meshPosition.rasterizer.x += step;
	while (ctx->meshPosition.rasterizer.x >= ctx->meshPosition.rasterizer.maxx)
	{
		ctx->meshPosition.rasterizer.x = ctx->meshPosition.rasterizer.minx + lm_passOffsetX(ctx);
		ctx->meshPosition.rasterizer.y += step;
		if (lm_hasConservativeTriangleRasterizerFinished(ctx))
			break;
	}
}

static float *lm_getLightmapPixel(lm_context *ctx, int x, int y)
{
	assert(x >= 0 && x < ctx->lightmap.width && y >= 0 && y < ctx->lightmap.height);
	return ctx->lightmap.data + (y * ctx->lightmap.width + x) * ctx->lightmap.channels;
}

static void lm_setLightmapPixel(lm_context *ctx, int x, int y, float *in)
{
	assert(x >= 0 && x < ctx->lightmap.width && y >= 0 && y < ctx->lightmap.height);
	float *p = ctx->lightmap.data + (y * ctx->lightmap.width + x) * ctx->lightmap.channels;
	for (int j = 0; j < ctx->lightmap.channels; j++)
		*p++ = *in++;
}

static bool lm_trySamplingConservativeTriangleRasterizerPosition(lm_context *ctx)
{
	if (lm_hasConservativeTriangleRasterizerFinished(ctx))
		return false;

	// check if lightmap pixel was already set
	float *pixelValue = lm_getLightmapPixel(ctx, ctx->meshPosition.rasterizer.x, ctx->meshPosition.rasterizer.y);
	for (int j = 0; j < ctx->lightmap.channels; j++)
		if (pixelValue[j] != 0.0f)
			return false;

	// try to interpolate from neighbors:
	if (ctx->meshPosition.pass > 0)
	{
		float *neighbors[4];
		int neighborCount = 0;
		int neighborsExpected = 0;
		int d = (int)lm_passStepSize(ctx) / 2;
		int dirs = ((ctx->meshPosition.pass - 1) % 3) + 1;
		if (dirs & 1) // check x-neighbors with distance d
		{
			neighborsExpected += 2;
			if (ctx->meshPosition.rasterizer.x - d >= ctx->meshPosition.rasterizer.minx &&
				ctx->meshPosition.rasterizer.x + d <  ctx->meshPosition.rasterizer.maxx)
			{
				neighbors[neighborCount++] = lm_getLightmapPixel(ctx, ctx->meshPosition.rasterizer.x - d, ctx->meshPosition.rasterizer.y);
				neighbors[neighborCount++] = lm_getLightmapPixel(ctx, ctx->meshPosition.rasterizer.x + d, ctx->meshPosition.rasterizer.y);
			}
		}
		if (dirs & 2) // check y-neighbors with distance d
		{
			neighborsExpected += 2;
			if (ctx->meshPosition.rasterizer.y - d >= ctx->meshPosition.rasterizer.miny &&
				ctx->meshPosition.rasterizer.y + d <  ctx->meshPosition.rasterizer.maxy)
			{
				neighbors[neighborCount++] = lm_getLightmapPixel(ctx, ctx->meshPosition.rasterizer.x, ctx->meshPosition.rasterizer.y - d);
				neighbors[neighborCount++] = lm_getLightmapPixel(ctx, ctx->meshPosition.rasterizer.x, ctx->meshPosition.rasterizer.y + d);
			}
		}
		if (neighborCount == neighborsExpected) // are all interpolation neighbors available?
		{
			// calculate average neighbor pixel value
			float avg[4] = { 0 };
			for (int i = 0; i < neighborCount; i++)
				for (int j = 0; j < ctx->lightmap.channels; j++)
					avg[j] += neighbors[i][j];
			float ni = 1.0f / neighborCount;
			for (int j = 0; j < ctx->lightmap.channels; j++)
				avg[j] *= ni;

			// check if error from average pixel to neighbors is above the interpolation threshold
			bool interpolate = true;
			for (int i = 0; i < neighborCount; i++)
			{
				bool zero = true;
				for (int j = 0; j < ctx->lightmap.channels; j++)
				{
					if (neighbors[i][j] != 0.0f)
						zero = false;
					if (fabs(neighbors[i][j] - avg[j]) > ctx->interpolationThreshold)
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
				lm_setLightmapPixel(ctx, ctx->meshPosition.rasterizer.x, ctx->meshPosition.rasterizer.y, avg);
#ifdef LM_DEBUG_INTERPOLATION
				// set interpolated pixel to green in debug output
				ctx->lightmap.debug[(ctx->meshPosition.rasterizer.y * ctx->lightmap.width + ctx->meshPosition.rasterizer.x) * 3 + 1] = 255;
#endif
				return false;
			}
		}
	}

	float2 pixel[16];
	pixel[0].set(ctx->meshPosition.rasterizer.x, ctx->meshPosition.rasterizer.y);
	pixel[1].set(ctx->meshPosition.rasterizer.x + 1, ctx->meshPosition.rasterizer.y);
	pixel[2].set(ctx->meshPosition.rasterizer.x + 1, ctx->meshPosition.rasterizer.y + 1);
	pixel[3].set(ctx->meshPosition.rasterizer.x, ctx->meshPosition.rasterizer.y + 1);

	float2 res[16];
	int nRes = lm_convexClip(pixel, 4, ctx->meshPosition.triangle.uv, 3, res);
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
			float2 uv = math::barycentric(ctx->meshPosition.triangle.uv[0], ctx->meshPosition.triangle.uv[1], ctx->meshPosition.triangle.uv[2], centroid);

			if (math::isfinite(uv))
			{
				float3 p0 = ctx->meshPosition.triangle.p[0];
				float3 p1 = ctx->meshPosition.triangle.p[1];
				float3 p2 = ctx->meshPosition.triangle.p[2];
				float3 v1 = p1 - p0;
				float3 v2 = p2 - p0;
				ctx->meshPosition.sample.position = p0 + v2 * uv.x + v1 * uv.y;
				ctx->meshPosition.sample.direction = math::normalize(math::cross(v1, v2));

				if (math::isfinite(ctx->meshPosition.sample.position) &&
					math::isfinite(ctx->meshPosition.sample.direction) &&
					math::length2(ctx->meshPosition.sample.direction) > 0.5f) // don't allow 0.0f. should always be ~1.0f
				{
					float3 up = float3::UnitY;
					if (std::abs(math::dot(up, ctx->meshPosition.sample.direction)) > 0.8f)
						up = float3::UnitZ;

					float3 side = math::normalize(math::cross(up, ctx->meshPosition.sample.direction));
					up = math::normalize(math::cross(side, ctx->meshPosition.sample.direction));
					int rx = ctx->meshPosition.rasterizer.x % 3;
					int ry = ctx->meshPosition.rasterizer.y % 3;

					const float baseAngle = 0.03f * M_PI;
					const float baseAngles[3][3] = {
						{ baseAngle, baseAngle + 1.0f / 3.0f, baseAngle + 2.0f / 3.0f },
						{ baseAngle + 1.0f / 3.0f, baseAngle + 2.0f / 3.0f, baseAngle },
						{ baseAngle + 2.0f / 3.0f, baseAngle, baseAngle + 1.0f / 3.0f }
					};
					float phi = 2.0f * M_PI * baseAngles[ry][rx] + 0.1f * ((float)rand() / (float)RAND_MAX);
					ctx->meshPosition.sample.up = math::normalize(side * cosf(phi) + up * sinf(phi));

					return true;
				}
			}
		}
	}
	return false;
}

static bool lm_findFirstConservativeTriangleRasterizerPosition(lm_context *ctx)
{
	while (!lm_trySamplingConservativeTriangleRasterizerPosition(ctx))
	{
		lm_moveToNextPotentialConservativeTriangleRasterizerPosition(ctx);
		if (lm_hasConservativeTriangleRasterizerFinished(ctx))
			return false;
	}
	return true;
}

static bool lm_findNextConservativeTriangleRasterizerPosition(lm_context *ctx)
{
	lm_moveToNextPotentialConservativeTriangleRasterizerPosition(ctx);
	return lm_findFirstConservativeTriangleRasterizerPosition(ctx);
}

static void lm_beginProcessHemisphereBatch(lm_context *ctx)
{
	if (!ctx->hemisphere.fbHemiIndex)
		return;

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(ctx->hemisphere.vao);

	int fbRead = 0;
	int fbWrite = 1;

	int outHemiSize = ctx->hemisphere.size / 2;
	glBindFramebuffer(GL_FRAMEBUFFER, ctx->hemisphere.fb[fbWrite]);
	glViewport(0, 0, outHemiSize * ctx->hemisphere.fbHemiCountX, outHemiSize * ctx->hemisphere.fbHemiCountY);
	glUseProgram(ctx->hemisphere.firstPass.programID);
	glUniform1i(ctx->hemisphere.firstPass.hemispheresTextureID, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ctx->hemisphere.fbTexture[fbRead]);
	glUniform1i(ctx->hemisphere.firstPass.weightsTextureID, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ctx->hemisphere.firstPass.weightsTexture);
	glActiveTexture(GL_TEXTURE0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(ctx->hemisphere.downsamplePass.programID);
	glUniform1i(ctx->hemisphere.downsamplePass.hemispheresTextureID, 0);
	while (outHemiSize > 1)
	{
		std::swap(fbRead, fbWrite);
		outHemiSize /= 2;
		glBindFramebuffer(GL_FRAMEBUFFER, ctx->hemisphere.fb[fbWrite]);
		glViewport(0, 0, outHemiSize * ctx->hemisphere.fbHemiCountX, outHemiSize * ctx->hemisphere.fbHemiCountY);
		glBindTexture(GL_TEXTURE_2D, ctx->hemisphere.fbTexture[fbRead]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (fbWrite == 0) // copy to other fb if we end up in fb 0, so that fb 0 can be written to while the data is async transferred!
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		std::swap(fbRead, fbWrite);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, ctx->hemisphere.fb[fbRead]);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ctx->hemisphere.fb[fbWrite]);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(
			0, 0, ctx->hemisphere.fbHemiCountX, ctx->hemisphere.fbHemiCountY,
			0, 0, ctx->hemisphere.fbHemiCountX, ctx->hemisphere.fbHemiCountY,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, ctx->hemisphere.fb[fbWrite]);
	}

	// start GPU->CPU transfer of downsampled hemispheres
	glBindBuffer(GL_PIXEL_PACK_BUFFER, ctx->hemisphere.transfer.pbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ctx->hemisphere.fb[1]);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
	glReadPixels(0, 0, ctx->hemisphere.fbHemiCountX, ctx->hemisphere.fbHemiCountY, GL_RGBA, GL_FLOAT, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	std::swap(ctx->hemisphere.transfer.fbHemiToLightmapLocation, ctx->hemisphere.fbHemiToLightmapLocation);

	ctx->hemisphere.transfer.fbHemiCount = ctx->hemisphere.fbHemiIndex;
	ctx->hemisphere.transfer.pboTransferStarted = true;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);

	ctx->hemisphere.fbHemiIndex = 0;
}

static void lm_finishProcessHemisphereBatch(lm_context *ctx)
{
	if (!ctx->hemisphere.transfer.pboTransferStarted)
		return;

	glBindBuffer(GL_PIXEL_PACK_BUFFER, ctx->hemisphere.transfer.pbo);
	float *hemi = (float*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

	assert(hemi);
	if (!hemi)
	{
		fprintf(stderr, "Fatal error! Could not map hemisphere buffer!\n");
		exit(-1);
	}

	unsigned int hemiIndex = 0;
	for (unsigned int hy = 0; hy < ctx->hemisphere.fbHemiCountY; hy++)
	{
		for (unsigned int hx = 0; hx < ctx->hemisphere.fbHemiCountX; hx++)
		{
			float *c = hemi + (hy * ctx->hemisphere.fbHemiCountX + hx) * 4;
			float validity = c[3];

			int2 lmUV = ctx->hemisphere.transfer.fbHemiToLightmapLocation[hy * ctx->hemisphere.fbHemiCountX + hx];
			float *lm = ctx->lightmap.data + (lmUV.y * ctx->lightmap.width + lmUV.x) * ctx->lightmap.channels;
			if (!lm[0] && validity > 0.9)
			{
				float scale = 1.0f / validity;
				switch (ctx->lightmap.channels)
				{
				case 1:
					lm[0] = std::max((c[0] + c[1] + c[2]) * scale / 3.0f, FLT_MIN);
					break;
				case 2:
					lm[0] = std::max((c[0] + c[1] + c[2]) * scale / 3.0f, FLT_MIN);
					lm[1] = 1.0f; // do we want to support this format?
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

#ifdef LM_DEBUG_INTERPOLATION
				ctx->lightmap.debug[(lmUV.y * ctx->lightmap.width + lmUV.x) * 3 + 0] = 255;
#endif
			}

			if (++hemiIndex == ctx->hemisphere.transfer.fbHemiCount)
				goto done;
		}
	}
done:
	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	ctx->hemisphere.transfer.pboTransferStarted = false;
}

void 
LightMassBaking::updateSampleMatrices(float* view, float3 pos, float3 dir, const float3& up, float* proj, float l, float r, float b, float t, float n, float f)
{
	// view matrix: lookAt(pos, pos + dir, up)
	float3 side = math::cross(dir, up);
	//up = cross(side, dir);
	dir = -dir; pos = -pos;
	view[0] = side.x;             view[1] = up.x;             view[2] = dir.x;             view[3] = 0.0f;
	view[4] = side.y;             view[5] = up.y;             view[6] = dir.y;             view[7] = 0.0f;
	view[8] = side.z;             view[9] = up.z;             view[10] = dir.z;             view[11] = 0.0f;
	view[12] = math::dot(side, pos); view[13] = math::dot(up, pos); view[14] = math::dot(dir, pos); view[15] = 1.0f;

	// projection matrix: frustum(l, r, b, t, n, f)
	float ilr = 1.0f / (r - l), ibt = 1.0f / (t - b), ninf = -1.0f / (f - n), n2 = 2.0f * n;
	proj[0] = n2 * ilr;      proj[1] = 0.0f;          proj[2] = 0.0f;           proj[3] = 0.0f;
	proj[4] = 0.0f;          proj[5] = n2 * ibt;      proj[6] = 0.0f;           proj[7] = 0.0f;
	proj[8] = (r + l) * ilr; proj[9] = (t + b) * ibt; proj[10] = (f + n) * ninf; proj[11] = -1.0f;
	proj[12] = 0.0f;         proj[13] = 0.0f;         proj[14] = f * n2 * ninf;  proj[15] = 0.0f;
}

bool 
LightMassBaking::updateSampleHemisphere(int* viewport, float* view, float* proj)
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

static float3 lm_transform(const float *m, float3 v)
{
	if (!m)
		return v;
	float3 r;
	r.x = m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12];
	r.y = m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13];
	r.z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14];
	float d = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15];
	assert(std::abs(d - 1.0f) < 0.00001f); // could divide by d, but this shouldn't be a projection transform!
	return r;
}

static float lm_defaultWeights(float cos_theta, void *userdata)
{
	return 1.0f;
}

void lmSetHemisphereWeights(lm_context *ctx, lm_weight_func f, void *userdata)
{
	// hemisphere weights texture. bakes in material dependent attenuation behaviour.
	float *weights = (float*)calloc(2 * 3 * ctx->hemisphere.size * ctx->hemisphere.size, sizeof(float));
	float center = (ctx->hemisphere.size - 1) * 0.5f;
	double sum = 0.0;
	for (unsigned int y = 0; y < ctx->hemisphere.size; y++)
	{
		float dy = 2.0f * (y - center) / (float)ctx->hemisphere.size;
		for (unsigned int x = 0; x < ctx->hemisphere.size; x++)
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

	// normalize weights
	float weightScale = (float)(1.0 / sum);
	for (unsigned int i = 0; i < 2 * 3 * ctx->hemisphere.size * ctx->hemisphere.size; i++)
		weights[i] *= weightScale;

	// upload weight texture
	glBindTexture(GL_TEXTURE_2D, ctx->hemisphere.firstPass.weightsTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, 3 * ctx->hemisphere.size, ctx->hemisphere.size, 0, GL_RG, GL_FLOAT, weights);
	free(weights);
}

void
LightMassBaking::setMeshPosition(std::uint32_t indicesTriangleBaseIndex)
{
	_ctx->meshPosition.triangle.baseIndex = indicesTriangleBaseIndex;

	float2 uvMin = float2(FLT_MAX, FLT_MAX), uvMax = float2(-FLT_MAX, -FLT_MAX);
	float2 uvScale(_ctx->lightmap.width, _ctx->lightmap.height);

	for (int i = 0; i < 3; i++)
	{
		unsigned int vIndex;
		switch (_ctx->mesh.indicesType)
		{
		case GL_NONE:
			vIndex = _ctx->meshPosition.triangle.baseIndex + i;
			break;
		case GL_UNSIGNED_BYTE:
			vIndex = ((const unsigned char*)_ctx->mesh.indices + _ctx->meshPosition.triangle.baseIndex)[i];
			break;
		case GL_UNSIGNED_SHORT:
			vIndex = ((const unsigned short*)_ctx->mesh.indices + _ctx->meshPosition.triangle.baseIndex)[i];
			break;
		case GL_UNSIGNED_INT:
			vIndex = ((const unsigned int*)_ctx->mesh.indices + _ctx->meshPosition.triangle.baseIndex)[i];
			break;
		default:
			assert(false);
			break;
		}

		const void *pPtr = _ctx->mesh.positions + vIndex * _ctx->mesh.positionsStride;
		float3 p;
		switch (_ctx->mesh.positionsType)
		{
			// TODO: signed formats
		case GL_UNSIGNED_BYTE: {
			const unsigned char *uc = (const unsigned char*)pPtr;
			p = float3(uc[0], uc[1], uc[2]);
		} break;
		case GL_UNSIGNED_SHORT: {
			const unsigned short *us = (const unsigned short*)pPtr;
			p = float3(us[0], us[1], us[2]);
		} break;
		case GL_UNSIGNED_INT: {
			const unsigned int *ui = (const unsigned int*)pPtr;
			p = float3((float)ui[0], (float)ui[1], (float)ui[2]);
		} break;
		case GL_FLOAT: {
			p = *(const float3*)pPtr;
		} break;
		default: {
			assert(false);
		} break;
		}
		_ctx->meshPosition.triangle.p[i] = lm_transform(_ctx->mesh.transform, p);

		// decode and scale (to lightmap resolution) vertex lightmap texture coords
		const void *uvPtr = _ctx->mesh.uvs + vIndex * _ctx->mesh.uvsStride;
		float2 uv;
		switch (_ctx->mesh.uvsType)
		{
		case GL_UNSIGNED_BYTE: {
			const unsigned char *uc = (const unsigned char*)uvPtr;
			uv = float2(uc[0] / (float)UCHAR_MAX, uc[1] / (float)UCHAR_MAX);
		} break;
		case GL_UNSIGNED_SHORT: {
			const unsigned short *us = (const unsigned short*)uvPtr;
			uv = float2(us[0] / (float)USHRT_MAX, us[1] / (float)USHRT_MAX);
		} break;
		case GL_UNSIGNED_INT: {
			const unsigned int *ui = (const unsigned int*)uvPtr;
			uv = float2(ui[0] / (float)UINT_MAX, ui[1] / (float)UINT_MAX);
		} break;
		case GL_FLOAT: {
			uv = *(const float2*)uvPtr;
		} break;
		default: {
			assert(false);
		} break;
		}

		_ctx->meshPosition.triangle.uv[i] = uv * uvScale;

		uvMin = math::min(uvMin, _ctx->meshPosition.triangle.uv[i]);
		uvMax = math::max(uvMax, _ctx->meshPosition.triangle.uv[i]);
	}

	float2 bbMin = math::floor(uvMin);
	float2 bbMax = math::ceil(uvMax);

	_ctx->meshPosition.rasterizer.minx = std::max((int)bbMin.x - 1, 0);
	_ctx->meshPosition.rasterizer.miny = std::max((int)bbMin.y - 1, 0);
	_ctx->meshPosition.rasterizer.maxx = std::min((int)bbMax.x + 1, _ctx->lightmap.width);
	_ctx->meshPosition.rasterizer.maxy = std::min((int)bbMax.y + 1, _ctx->lightmap.height);
	assert(_ctx->meshPosition.rasterizer.minx < _ctx->meshPosition.rasterizer.maxx && _ctx->meshPosition.rasterizer.miny < _ctx->meshPosition.rasterizer.maxy);

	_ctx->meshPosition.rasterizer.x = _ctx->meshPosition.rasterizer.minx + lm_passOffsetX(_ctx.get());
	_ctx->meshPosition.rasterizer.y = _ctx->meshPosition.rasterizer.miny + lm_passOffsetY(_ctx.get());

	// try moving to first valid sample position
	if (_ctx->meshPosition.rasterizer.x <= _ctx->meshPosition.rasterizer.maxx &&
		_ctx->meshPosition.rasterizer.y <= _ctx->meshPosition.rasterizer.maxy &&
		lm_findFirstConservativeTriangleRasterizerPosition(_ctx.get()))
		_ctx->meshPosition.hemisphere.side = 0; // we can start sampling the hemisphere
	else
		_ctx->meshPosition.hemisphere.side = 5; // no samples on this triangle! put hemisphere sampler into finished state
}

void lmImageDilate(const float *image, float *outImage, int w, int h, int c)
{
	assert(c > 0 && c <= 4);
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			float color[4];
			bool valid = false;
			for (int i = 0; i < c; i++)
			{
				color[i] = image[(y * w + x) * c + i];
				valid |= color[i] > 0.0f;
			}
			if (!valid)
			{
				int n = 0;
				const int dx[] = { -1, 0, 1,  0 };
				const int dy[] = { 0, 1, 0, -1 };
				for (int d = 0; d < 4; d++)
				{
					int cx = x + dx[d];
					int cy = y + dy[d];
					if (cx >= 0 && cx < w && cy >= 0 && cy < h)
					{
						float dcolor[4];
						bool dvalid = false;
						for (int i = 0; i < c; i++)
						{
							dcolor[i] = image[(cy * w + cx) * c + i];
							dvalid |= dcolor[i] > 0.0f;
						}
						if (dvalid)
						{
							for (int i = 0; i < c; i++)
								color[i] += dcolor[i];
							n++;
						}
					}
				}
				if (n)
				{
					float in = 1.0f / n;
					for (int i = 0; i < c; i++)
						color[i] *= in;
				}
			}
			for (int i = 0; i < c; i++)
				outImage[(y * w + x) * c + i] = color[i];
		}
	}
}

void lmImageSmooth(const float *image, float *outImage, int w, int h, int c)
{
	assert(c > 0 && c <= 4);
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			float color[4] = { 0 };
			int n = 0;
			for (int dy = -1; dy <= 1; dy++)
			{
				int cy = y + dy;
				for (int dx = -1; dx <= 1; dx++)
				{
					int cx = x + dx;
					if (cx >= 0 && cx < w && cy >= 0 && cy < h)
					{
						bool valid = false;
						for (int i = 0; i < c; i++)
							valid |= image[(cy * w + cx) * c + i] > 0.0f;
						if (valid)
						{
							for (int i = 0; i < c; i++)
								color[i] += image[(cy * w + cx) * c + i];
							n++;
						}
					}
				}
			}
			for (int i = 0; i < c; i++)
				outImage[(y * w + x) * c + i] = n ? color[i] / n : 0.0f;
		}
	}
}

struct LightMassContextGL
{
	LightMassContextGL() noexcept
		: program(0)
		, vs(0)
		, fs(0)
		, u_lightmap(0)
		, u_mvp(0)
		, lightmap(0)
		, vao(0)
		, vbo(0)
		, ibo(0)
	{
	}

	~LightMassContextGL() noexcept
	{
		glDeleteShader(vs);
		glDeleteShader(fs);
		glDeleteProgram(program);
		glDeleteTextures(1, &lightmap);
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
	}

	GLuint vs;
	GLuint fs;
	GLuint program;
	
	GLint u_lightmap;
	GLint u_mvp;

	GLuint lightmap;
	GLuint vao, vbo, ibo;
};

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
		if (!this->setupOpenGL(params.model))
		{
			if (_lightMassListener)
				_lightMassListener->onMessage("Could not initialize with OpenGL.");

			return false;
		}

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

		float4x4 view, proj;
		Viewportt<int> vp;

		std::uint32_t baseIndex = 0;

		while (this->beginSampleHemisphere(vp.ptr(), view.data(), proj.data()))
		{
			float4x4 mvp = proj * view;
			this->doSampleHemisphere(*_ctxGL, params, vp, mvp);

			if (baseIndex != _ctx->meshPosition.triangle.baseIndex)
			{
				if (_lightMassListener)
					_lightMassListener->onBakingProgressing(this->getSampleProcess());

				baseIndex = _ctx->meshPosition.triangle.baseIndex;
			}

			this->endSampleHemisphere();
		}

		if (params.lightMap.margin > 0)
		{
			std::unique_ptr<float[]> lightmapTemp = std::make_unique<float[]>(params.lightMap.width * params.lightMap.height * params.lightMap.channel);
			std::memset(lightmapTemp.get(), 0, params.lightMap.width * params.lightMap.height * params.lightMap.channel * sizeof(float));

			for (int j = 0; j < params.lightMap.margin; j++)
			{
				lmImageSmooth(params.lightMap.data, lightmapTemp.get(), params.lightMap.width, params.lightMap.height, params.lightMap.channel);
				lmImageDilate(lightmapTemp.get(), params.lightMap.data, params.lightMap.width, params.lightMap.height, params.lightMap.channel);
			}
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
LightMassBaking::setupOpenGL(const LightModelData& params) noexcept
{
	assert(params.vertices >= 0 && params.indices >= 0);
	assert(params.numVertices > 0 && params.numIndices > 0);
	assert(params.subsets.size() >= 1);
	assert(params.strideVertices < params.sizeofVertices && params.strideTexcoord < params.sizeofVertices);
	assert(params.sizeofVertices > 0);
	assert(params.sizeofIndices == 1 || params.sizeofIndices == 2 || params.sizeofIndices == 3);

	if (glewInit() != GLEW_OK)
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Could not initialize with OpenGL.");

		return false;
	}

	auto glcontext = std::make_unique<LightMassContextGL>();

	glGenBuffers(1, &glcontext->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, glcontext->vbo);
	glBufferData(GL_ARRAY_BUFFER, params.numVertices * params.sizeofVertices, params.vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &glcontext->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glcontext->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, params.numIndices * params.sizeofIndices, params.indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &glcontext->vao);
	glBindVertexArray(glcontext->vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, params.sizeofVertices, (char*)nullptr + params.strideVertices);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, params.sizeofVertices, (char*)nullptr + params.strideTexcoord);

	glBindBuffer(GL_ARRAY_BUFFER, glcontext->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glcontext->ibo);

	glBindVertexArray(0);

	std::uint8_t emissive[] = { 0, 0, 0, 255 };

	glGenTextures(1, &glcontext->lightmap);
	glBindTexture(GL_TEXTURE_2D, glcontext->lightmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, emissive);

	const char *vp =
		"#version 150 core\n"
		"in vec3 a_position;\n"
		"in vec2 a_texcoord;\n"
		"uniform mat4 u_mvp;\n"
		"out vec2 v_texcoord;\n"

		"void main()\n"
		"{\n"
		"v_texcoord = a_texcoord;\n"
		"gl_Position = u_mvp * vec4(a_position, 1.0);\n"
		"}\n";

	const char *fp =
		"#version 150 core\n"
		"in vec2 v_texcoord;\n"
		"uniform sampler2D u_lightmap;\n"
		"out vec4 o_color;\n"

		"void main()\n"
		"{\n"
		"o_color = vec4(texture(u_lightmap, v_texcoord).rgb, gl_FrontFacing ? 1.0 : 0.0);\n"
		"}\n";

	const char *attribs[] =
	{
		"a_position",
		"a_texcoord"
	};

	glcontext->vs = loadShader(GL_VERTEX_SHADER, vp);
	if (!glcontext->vs)
		return GL_NONE;

	glcontext->fs = loadShader(GL_FRAGMENT_SHADER, fp);
	if (!glcontext->fs)
		return GL_NONE;

	glcontext->program = loadProgram(glcontext->vs, glcontext->fs, attribs, 2);
	if (!glcontext->program)
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Failed to loading shader.");

		return false;
	}

	glcontext->u_mvp = glGetUniformLocation(glcontext->program, "u_mvp");
	glcontext->u_lightmap = glGetUniformLocation(glcontext->program, "u_lightmap");

	_ctxGL = std::move(glcontext);

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

	int w[] = { context->hemisphere.fbHemiCountX * context->hemisphere.size * 3, context->hemisphere.fbHemiCountX * context->hemisphere.size / 2 };
	int h[] = { context->hemisphere.fbHemiCountY * context->hemisphere.size,     context->hemisphere.fbHemiCountY * context->hemisphere.size / 2 };

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

	context->hemisphere.fbHemiToLightmapLocation = (int2*)calloc(context->hemisphere.fbHemiCountX * context->hemisphere.fbHemiCountY, sizeof(int2));
	context->hemisphere.transfer.fbHemiToLightmapLocation = (int2*)calloc(context->hemisphere.fbHemiCountX * context->hemisphere.fbHemiCountY, sizeof(int2));

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

#ifdef LM_DEBUG_INTERPOLATION
	if (_ctx->lightmap.debug)
		LM_FREE(_ctx->lightmap.debug);
	_ctx->lightmap.debug = (unsigned char*)LM_CALLOC(ctx->lightmap.width * ctx->lightmap.height, 3);
#endif
}

void 
LightMassBaking::setGeometry(const float4x4& world, int positionsType, const void *positionsXYZ, int positionsStride, int lightmapCoordsType, const void *lightmapCoordsUV, int lightmapCoordsStride, int count, int indicesType, const void *indices)
{
	_ctx->mesh.transform = world.data();
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

	this->setMeshPosition(0);
}

bool
LightMassBaking::beginSampleHemisphere(int* outViewport4, float* outView4x4, float* outProjection4x4)
{
	assert(_ctx->meshPosition.triangle.baseIndex < _ctx->mesh.count);

	while (!this->updateSampleHemisphere(outViewport4, outView4x4, outProjection4x4))
	{
		if (lm_findNextConservativeTriangleRasterizerPosition(_ctx.get()))
		{
			_ctx->meshPosition.hemisphere.side = 0;
		}
		else
		{
			if (_ctx->meshPosition.triangle.baseIndex + 3 < _ctx->mesh.count)
			{
				this->setMeshPosition(_ctx->meshPosition.triangle.baseIndex + 3);
			}
			else
			{
				lm_finishProcessHemisphereBatch(_ctx.get());
				lm_beginProcessHemisphereBatch(_ctx.get());
				lm_finishProcessHemisphereBatch(_ctx.get());

				if (++_ctx->meshPosition.pass == _ctx->meshPosition.passCount)
				{
					_ctx->meshPosition.pass = 0;
					_ctx->meshPosition.triangle.baseIndex = _ctx->mesh.count;

					return false;
				}

				this->setMeshPosition(0);
			}
		}
	}

	return true;
}

void
LightMassBaking::endSampleHemisphere()
{
	if (++_ctx->meshPosition.hemisphere.side == 5)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (++_ctx->hemisphere.fbHemiIndex == _ctx->hemisphere.fbHemiCountX * _ctx->hemisphere.fbHemiCountY)
		{
			lm_finishProcessHemisphereBatch(_ctx.get());
			lm_beginProcessHemisphereBatch(_ctx.get());
		}
	}
}

void 
LightMassBaking::doSampleHemisphere(const LightMassContextGL& ctxGL, const LightBakingOptions& params, const Viewportt<int>& vp, const float4x4& mvp)
{
	GLenum faceType = GL_UNSIGNED_INT;
	if (params.model.sizeofIndices == 1)
		faceType = GL_UNSIGNED_BYTE;
	else if (params.model.sizeofIndices == 2)
		faceType = GL_UNSIGNED_SHORT;

	glViewport(vp.left, vp.top, vp.width, vp.height);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(ctxGL.program);

	glUniform1i(ctxGL.u_lightmap, 0);
	glUniformMatrix4fv(ctxGL.u_mvp, 1, GL_FALSE, mvp.ptr());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ctxGL.lightmap);
	glBindVertexArray(ctxGL.vao);

	Frustum fru;
	fru.extract(mvp);

	for (auto& subset : params.model.subsets)
	{
		if (!fru.contains(subset.boundingBox.aabb()))
			continue;

		if (glDrawElementsInstancedBaseVertexBaseInstance)
		{
			glDrawElementsInstancedBaseVertexBaseInstance(
				GL_TRIANGLES,
				subset.drawcall.count,
				faceType,
				(char*)nullptr + subset.drawcall.firstIndex * params.model.sizeofIndices,
				subset.drawcall.instanceCount,
				subset.drawcall.baseVertex,
				subset.drawcall.baseInstance
			);
		}
		else
		{
			glDrawElements(GL_TRIANGLES, subset.drawcall.count, faceType, (char*)nullptr + subset.drawcall.firstIndex * params.model.sizeofIndices);
		}
	}
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
			_lightMassListener->onMessage("Could not create shader!\n");

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
			_lightMassListener->onMessage("Could not compile shader!");

			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			if (length)
			{
				std::string log((std::size_t)length, 0);
				glGetShaderInfoLog(shader, length, &length, (char*)log.data());

				if (_lightMassListener)
					_lightMassListener->onMessage(log);
			}
		}

		glDeleteShader(shader);
		return 0;
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
			_lightMassListener->onMessage("Could not create program!");

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