// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#ifndef _H_TERRAIN_LOD_H_
#define _H_TERRAIN_LOD_H_

#include <ray/terrain_height_map.h>

_NAME_BEGIN

struct Quadnode
{
	std::uint32_t x;
	std::uint32_t y;
	Quadnode() {}
	Quadnode(std::uint32_t xx, std::uint32_t yy) :x(xx), y(yy) {}
};

class TerrainLod
{
public:
	TerrainLod() noexcept;

	void setMap(HeightMap* map) noexcept;
	void setFrustum(Frustum* fru);

	float getHeight(std::uint32_t x, std::uint32_t y) const;
	const Vector3& getVertex(std::uint32_t x, std::uint32_t y) const;
	std::size_t getScale() const;
	std::uint32_t getSize()  const;
	std::uint8_t getDepth() const;

	bool isVisible(const Quadnode& node, std::uint8_t depth) const;

	void tessellate();

protected:
	virtual bool split(const Quadnode& node, std::uint8_t level) = 0;

private:
	std::uint8_t _minDepth;
	std::uint8_t _maxDepth;

	std::uint32_t _size;
	std::uint32_t _scale;

	HeightMap* _map;
	Quadnode   _root;
	std::vector<Quadnode> _curNodes;
	std::vector<Quadnode> _nextNodes;

	Frustum* _frustum;
};

_NAME_END

#endif