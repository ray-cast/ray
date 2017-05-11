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
#ifndef _H_LIGHTMAP_PACK_H_
#define _H_LIGHTMAP_PACK_H_

#include "LightMapListener.h"
#include "modpmx.h"

_NAME_BEGIN

class LightMapItem final
{
public:
	LightMapItem() noexcept;
	~LightMapItem() noexcept;

	float2 edge, offset;
	float2 *p1, *p2, *p3, *p4;

	float area() const
	{
		return edge.x * edge.y;
	}

	bool operator < (const LightMapItem& a) const
	{
		return area() > a.area();
	}
};

struct LightMapNode
{
	float4 rect;

	LightMapNode* left;
	LightMapNode* right;

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

class LightMapPack
{
public:
	LightMapPack() noexcept;
	~LightMapPack() noexcept;

	bool load(const std::string& path) noexcept;
	bool save(const std::string& path) noexcept;

	bool pack() noexcept;

	std::uint32_t getFace(std::size_t n) noexcept;
	std::uint32_t getFace(std::size_t n, std::uint32_t firstIndex) noexcept;
private:
	LightMapNode* insertLightMapItem(LightMapNode* node, LightMapItem& item) noexcept;

private:
	std::unique_ptr<PMX> _model;

	LightMapListenerPtr _lightMapListener;
};

_NAME_END

#endif