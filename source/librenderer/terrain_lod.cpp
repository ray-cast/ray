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
#include <ray/terrain_lod.h>

_NAME_BEGIN

TerrainLod::TerrainLod() noexcept
	: _minDepth(0)
	, _maxDepth(0)
{
}

void
TerrainLod::setMap(HeightMap* map) noexcept
{
	assert(map);
	_map = map;
	_size = _map->getSize();
	_scale = (std::size_t)_map->getCellSize();
	_maxDepth = 0;
	_maxDepth = (std::uint8_t)math::bitScanReverse(_size - 1);
}

void
TerrainLod::setFrustum(Frustum* fru)
{
	_frustum = fru;
}

float
TerrainLod::getHeight(std::uint32_t x, std::uint32_t y) const
{
	return _map->getHeight(x, y);
}

const Vector3&
TerrainLod::getVertex(std::uint32_t x, std::uint32_t y) const
{
	return _map->getVertex(x, y);
}

std::size_t
TerrainLod::getScale() const
{
	return _scale;
}

std::uint32_t
TerrainLod::getSize() const
{
	return _size;
}

std::uint8_t
TerrainLod::getDepth() const
{
	return _maxDepth;
}

bool
TerrainLod::isVisible(const Quadnode& node, std::uint8_t depth) const
{
	assert(_frustum);
	assert(node.x < _size);
	assert(node.y < _size);

	std::intptr_t size = 1 << depth;

	float R = size * _scale * 1.414f; //Ð±±ß

	return _frustum->contains(_map->getVertex(node.x, node.y), R);
}

void
TerrainLod::tessellate()
{
	_curNodes.push_back(_root);

	for (std::uint8_t i = _maxDepth; i > _maxDepth; i--)
	{
		std::size_t size = _curNodes.size();

		for (std::size_t j = 0; j < size; j++)
		{
			Quadnode node = _curNodes[j];

			if (split(node, i))
			{
				int d = 1 << (i - 2);

				Quadnode child;

				child.x = node.x + d;
				child.y = node.y + d;
				_nextNodes.push_back(child);

				child.x = node.x - d;
				child.y = node.y + d;
				_nextNodes.push_back(child);

				child.x = node.x + d;
				child.y = node.y - d;
				_nextNodes.push_back(child);

				child.x = node.x - d;
				child.y = node.y - d;
				_nextNodes.push_back(child);
			}
		}

		_curNodes.swap(_nextNodes);
		_nextNodes.clear();
	}
}

_NAME_END