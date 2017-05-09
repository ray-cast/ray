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
#ifndef _H_LIGHTMASS_BAKING_H_
#define _H_LIGHTMASS_BAKING_H_

#include "LightMassParams.h"
#include "LightMassListener.h"
#include "HemisphereWeight.h"

_NAME_BEGIN

class LightMassBaking
{
public:
	LightMassBaking() noexcept;
	virtual ~LightMassBaking() noexcept;

	virtual void setLightMassListener(LightMassListenerPtr pointer) noexcept;
	virtual LightMassListenerPtr getLightMassListener() const noexcept;

	virtual bool baking(const LightBakingOptions& options) noexcept;

protected:
	virtual bool setupBakeTools(const LightBakingParams& params);
	virtual void closeBakeTools();

	void setRenderTarget(float* lightmap, int w, int h, int channels);
	void setGeometry(const float4x4& world, int positionsType, const void *positionsXYZ, int positionsStride, int lightmapCoordsType, const void *lightmapCoordsUV, int lightmapCoordsStride, int count, int indicesType, const void *indices);
	void setSamplePosition(std::uint32_t indicesTriangleBaseIndex);

	bool updateHemisphereWeights(void* userdata);
	virtual float doGetHemisphereWeights(float cosTheta, void* userdata);

	int leftOf(const float2& a, const float2& b, const float2& c);
	int convexClip(float2* poly, int nPoly, const float2* clip, int nClip, float2* res);

	std::size_t passStepSize();
	std::size_t passOffsetX();
	std::size_t passOffsetY();

	float* getLightmapPixel(int x, int y);
	void setLightmapPixel(int x, int y, float* in);

	bool hasConservativeTriangleRasterizerFinished();
	void moveToNextPotentialConservativeTriangleRasterizerPosition();
	bool trySamplingConservativeTriangleRasterizerPosition();
	bool findFirstConservativeTriangleRasterizerPosition();
	bool findNextConservativeTriangleRasterizerPosition();

	virtual void beginProcessHemisphereBatch();
	virtual bool finishProcessHemisphereBatch();

	virtual void updateSampleMatrices(float4x4& view, float3 pos, float3 dir, const float3& up, float4x4& proj, float l, float r, float b, float t, float n, float f);
	virtual bool updateSampleHemisphere(int* viewport, float4x4& view, float4x4& proj);

	virtual bool beginSampleHemisphere(int* outViewport4, float4x4& view, float4x4& proj);
	virtual void doSampleHemisphere(const LightBakingOptions& params, const Viewportt<int>& viewport, const float4x4& mvp) = 0;
	virtual bool endSampleHemisphere();

	virtual float getSampleProcess() noexcept;

	std::uint32_t loadShader(std::uint32_t type, const char *source);
	std::uint32_t loadProgram(std::uint32_t vs, std::uint32_t fs, const char **attributes, int attributeCount);

private:
	float4x4 _view;
	float4x4 _project;
	float4x4 _viewProject;

	struct HemiContext
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
			std::size_t count;
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
			std::size_t size;
			float znear, zfar;
			struct { float r, g, b; } clearColor;

			std::size_t fbHemiCountX;
			std::size_t fbHemiCountY;
			std::size_t fbHemiIndex;
			std::unique_ptr<int2[]> fbHemiToLightmapLocation;
			std::uint32_t fbTexture[2];
			std::uint32_t fb[2];
			std::uint32_t fbDepth;
			std::uint32_t vao;
			struct
			{
				std::uint32_t vs;
				std::uint32_t fs;
				std::uint32_t programID;
				std::uint32_t hemispheresTextureID;
				std::uint32_t weightsTextureID;
				std::uint32_t weightsTexture;
			} firstPass;
			struct
			{
				std::uint32_t vs;
				std::uint32_t fs;
				std::uint32_t programID;
				std::uint32_t hemispheresTextureID;
			} downsamplePass;
			struct
			{
				std::uint32_t pbo;
				bool pboTransferStarted;
				std::size_t fbHemiCount;
				std::unique_ptr<int2[]> fbHemiToLightmapLocation;
			} transfer;
		} hemisphere;

		float interpolationThreshold;

		HemiContext() noexcept;
		~HemiContext() noexcept;
	};

	std::unique_ptr<HemiContext> _ctx;

	std::vector<LightModelDrawCall> _drawcalls;

	LightMassListenerPtr _lightMassListener;
};

_NAME_END

#endif