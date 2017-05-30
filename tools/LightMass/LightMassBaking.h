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

_NAME_BEGIN

class LightMassBaking
{
public:
	LightMassBaking() noexcept;
	virtual ~LightMassBaking() noexcept;

	void setLightMassListener(LightMassListenerPtr pointer) noexcept;
	LightMassListenerPtr getLightMassListener() const noexcept;

	void setWorldTransform(const float4x4& transform) noexcept;
	const float4x4& getWorldTransform() const noexcept;

	bool isStopped() const noexcept;

	bool start() noexcept;
	void stop() noexcept;

protected:
	bool setup(const LightSampleParams& params) noexcept;
	void close() noexcept;

	void setRenderTarget(float lightmap[], int w, int h, int channels);
	void setGeometry(int positionsType, const void *positionsXYZ, int positionsStride, int lightmapCoordsType, const void *lightmapCoordsUV, int lightmapCoordsStride, int count, int indicesType, const void *indices);
	void setSamplePosition(std::size_t indicesTriangleBaseIndex);

	int leftOf(const float2& a, const float2& b, const float2& c);
	int convexClip(float2* poly, int nPoly, const float2* clip, int nClip, float2* res);

	std::uint32_t passStepSize();
	std::uint32_t passOffsetX();
	std::uint32_t passOffsetY();

	float* getLightmapPixel(int x, int y);
	void setLightmapPixel(int x, int y, float* in);

	bool hasConservativeTriangleRasterizerFinished();
	void moveToNextPotentialConservativeTriangleRasterizerPosition();
	bool trySamplingConservativeTriangleRasterizerPosition();
	bool findFirstConservativeTriangleRasterizerPosition();
	bool findNextConservativeTriangleRasterizerPosition();

	void beginProcessHemisphereBatch();
	bool finishProcessHemisphereBatch();

	bool updateHemisphereWeights(const HemisphereWeight<float>* weights);
	void updateSampleCamera(float3 pos, float3 dir, const float3& up, float l, float r, float b, float t, float n, float f);
	bool updateSampleHemisphere(int* viewport);

	bool beginSampleHemisphere(int* outViewport4);
	bool endSampleHemisphere();

	virtual void doSampleHemisphere(const Viewportt<int>& viewport, const float4x4& mvp) = 0;

	float getSampleProcess() noexcept;

	std::uint32_t loadShader(std::uint32_t type, const char *source);
	std::uint32_t loadProgram(std::uint32_t vs, std::uint32_t fs, const char **attributes, int attributeCount);

private:
	struct HemiCamera
	{
		float4x4 world;
		float4x4 view;
		float4x4 project;
		float4x4 viewProject;
	};

	struct HemiContext
	{
		struct
		{
			const std::uint8_t* positions;
			int positionsType;
			int positionsStride;
			const std::uint8_t* uvs;
			int uvsType;
			int uvsStride;
			const std::uint8_t* indices;
			int indicesType;
			std::size_t count;
		} mesh;

		struct
		{
			int pass;
			int passCount;

			struct
			{
				std::size_t baseIndex;
				float3 p[3];
				float2 uv[3];
			} triangle;

			struct
			{
				std::int16_t minx, miny;
				std::int16_t maxx, maxy;
				std::int16_t x, y;
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
			std::uint32_t size;
			float znear, zfar;
			struct { float r, g, b; } clearColor;

			std::uint32_t fbHemiCountX;
			std::uint32_t fbHemiCountY;
			std::uint32_t fbHemiIndex;
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

	bool _isStopped;

	HemiCamera _camera;

	std::unique_ptr<HemiContext> _ctx;
	std::vector<LightModelDrawCall> _drawcalls;

	LightMassListenerPtr _lightMassListener;
};

_NAME_END

#endif