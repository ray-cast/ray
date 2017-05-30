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
#include "LightMassParams.h"

#include <GL\glew.h>

_NAME_BEGIN

LightMapData::LightMapData() noexcept
	: width(0)
	, height(0)
	, channel(0)
	, data(nullptr)
{
}

LightMapData::LightMapData(std::uint16_t w, std::uint16_t h, std::uint16_t c) noexcept
	: width(w)
	, height(h)
	, channel(c)
	, data(std::make_unique<float[]>(w * h * c))
{
	std::memset(data.get(), 0, w * h * c * sizeof(float));
}

LightMapParams::LightMapParams() noexcept
	: width(1024)
	, height(1024)
	, margin(2)
{
}

LightModelDrawCall::LightModelDrawCall() noexcept
	: count(0)
	, instanceCount(1)
	, firstIndex(0)
	, baseVertex(0)
	, baseInstance(0)
{
}

LightModelSubset::LightModelSubset() noexcept
	: emissive(float3::Zero)
{
}

LightModelData::LightModelData() noexcept
	: vertices(nullptr)
	, indices(nullptr)
	, sizeofIndices(2)
	, sizeofVertices(0)
	, strideVertices(0)
	, strideTexcoord(0)
	, numVertices(0)
	, numIndices(0)
{
}

LightSampleParams::LightSampleParams() noexcept
	: hemisphereSize(64)
	, hemisphereNear(0.1)
	, hemisphereFar(100)
	, hemisphereWeights(nullptr)
	, environmentColor(float3::One)
	, interpolationPasses(1)
	, interpolationThreshold(1e-4)
{
}

LightMassParams::LightMassParams() noexcept
{
}

_NAME_END