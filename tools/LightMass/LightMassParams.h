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
#ifndef _H_LIGHTMASS_PARAMS_H_
#define _H_LIGHTMASS_PARAMS_H_

#include "LightMassTypes.h"

_NAME_BEGIN

class LightMapData
{
public:
	LightMapData() noexcept;
	LightMapData(std::uint16_t w, std::uint16_t h, std::uint16_t c) noexcept;

	std::uint16_t width;
	std::uint16_t height;
	std::uint8_t channel;
	std::unique_ptr<float[]> data;
};

struct LightMapParams
{
	LightMapParams() noexcept;

	std::uint16_t width;
	std::uint16_t height;
	std::uint8_t margin;
};

struct LightModelDrawCall
{
	LightModelDrawCall() noexcept;

	std::uint32_t count;
	std::uint32_t instanceCount;
	std::uint32_t firstIndex;
	std::uint32_t baseVertex;
	std::uint32_t baseInstance;
};

struct LightModelSubset
{
	LightModelSubset() noexcept;

	float3 emissive;
	LightModelDrawCall drawcall;
};

struct LightModelData
{
	LightModelData() noexcept;

	std::uint32_t numVertices;
	std::uint32_t numIndices;

	std::uint8_t sizeofIndices;
	std::uint16_t sizeofVertices;

	std::uint32_t strideVertices;
	std::uint32_t strideTexcoord;

	const std::uint8_t* vertices;
	const std::uint8_t* indices;

	std::vector<LightModelSubset> subsets;
};

struct LightSampleParams
{
	LightSampleParams() noexcept;

	std::uint32_t hemisphereSize;

	float hemisphereNear;
	float hemisphereFar;
	const HemisphereWeight<float>* hemisphereWeights;

	float3 environmentColor;
	int interpolationPasses;
	float interpolationThreshold;

	std::function<bool(float progress)> listener;
};

struct LightBakingParams
{
	LightModelData model;
	LightMapDataPtr lightMap;
	LightSampleParams baking;
};

struct LightMassParams
{
	LightMassParams() noexcept;

	LightModelData model;
	LightSampleParams baking;
};

_NAME_END

#endif