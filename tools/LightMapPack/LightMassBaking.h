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

#include "LightMassListener.h"

_NAME_BEGIN

struct LightMapItem
{
	float2 edge, offset;
	float2 *p1, *p2, *p3, *p4;

	float getArea() const
	{
		return edge.x*edge.y;
	}

	bool operator < (const LightMapItem& a) const
	{
		return getArea() > a.getArea();
	}
};


struct LightMapNode
{
	LightMapNode* left;
	LightMapNode* right;
	float4 rect;

	LightMapNode()
	{
		left = nullptr;
		right = nullptr;
		rect = float4(0.0, 0.0, 1.0, 1.0);
	}
	~LightMapNode()
	{
		delete left;
		delete right;
	}
};

struct LightMapData
{
	LightMapData() noexcept;

	std::uint16_t width;
	std::uint16_t height;
	std::uint8_t channel;
	std::uint8_t margin;

	float* data;
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

struct LightModelData
{
	LightModelData() noexcept;

	std::uint8_t* vertices;
	std::uint8_t* indices;

	std::uint8_t sizeofIndices;
	std::uint16_t sizeofVertices;

	std::uint32_t strideVertices;
	std::uint32_t strideTexcoord;

	std::uint32_t numVertices;
	std::uint32_t numIndices;

	std::vector<Bound> boundingBoxs;
	std::vector<LightModelDrawCall> drawcalls; // first : offset, second faceCount
};

struct LightBakingParams
{
	LightBakingParams() noexcept;

	int hemisphereSize;

	float hemisphereNear;
	float hemisphereFar;

	float3 clearColor;
	int interpolationPasses;
	float interpolationThreshold;
};

struct LightBakingOptions
{
	LightMapData lightMap;
	LightModelData model;
	LightBakingParams baking;
};

class LightMassBaking
{
public:
	LightMassBaking() noexcept;
	~LightMassBaking() noexcept;

	void setLightMassListener(LightMassListenerPtr pointer) noexcept;
	LightMassListenerPtr getLightMassListener() const noexcept;

	bool baking(const LightBakingOptions& options) noexcept;
	bool saveAsTGA(const std::string& path, float* data, std::uint32_t w, std::uint32_t h, std::uint32_t c);

private:
	bool initialize(const LightBakingOptions& options, struct LightMassContextGL&) noexcept;

private:
	LightMassListenerPtr _lightMassListener;
};

_NAME_END

#endif