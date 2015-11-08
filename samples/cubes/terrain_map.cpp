// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
#include "terrain_map.h"

TerrainMap::TerrainMap(TerrainComponent& terrain)
	: _mask(0)
	, _terrain(terrain)
{
}

TerrainMap::~TerrainMap()
{
	this->close();
}

void
TerrainMap::create(std::size_t size, ChunkPosition x, ChunkPosition y, ChunkPosition z, std::size_t mask)
{
	_dx = x;
	_dy = y;
	_dz = z;
	_size = size;
	_mask = mask;
	_count = 0;
	_data.resize(mask + 1);
}

void
TerrainMap::close()
{
	_data = std::vector<MapEntry>();
}

bool
TerrainMap::set(BlockPosition x, BlockPosition y, BlockPosition z, ItemID w)
{
	assert(x >= 0 || x < _size);
	assert(y >= 0 || y < _size);
	assert(z >= 0 || z < _size);

	std::size_t index = ray::hash_int(x, y, z) & _mask;

	auto* entry = &_data[index];

	int overwrite = 0;
	while (!entry->empty())
	{
		if (entry->x == x && entry->y == y && entry->z == z)
		{
			overwrite = 1;
			break;
		}

		index = (index + 1) & _mask;
		entry = &_data[index];
	}

	if (overwrite)
	{
		if (entry->instanceID != w)
		{
			entry->instanceID = w;
			return true;
		}
	}
	else if (w)
	{
		entry->x = x;
		entry->y = y;
		entry->z = z;
		entry->instanceID = w;

		_count++;
		if (_count * 2 > _mask)
		{
			this->grow();
		}

		return true;
	}

	return false;
}

ItemID
TerrainMap::get(BlockPosition x, BlockPosition y, BlockPosition z)
{
	if (x < 0 || x >= _size) return 0;
	if (y < 0 || y >= _size) return 0;
	if (z < 0 || z >= _size) return 0;

	std::size_t index = ray::hash_int(x, y, z) & _mask;

	MapEntry* entry = &_data[index];

	while (!entry->empty())
	{
		if (entry->x == x &&
			entry->y == y &&
			entry->z == z)
		{
			return entry->instanceID;
		}

		index = (index + 1) & _mask;
		entry = &_data[index];
	}

	return 0;
}

std::size_t
TerrainMap::size() const noexcept
{
	return _size;
}

std::size_t
TerrainMap::count() const noexcept
{
	return _count;
}

void
TerrainMap::getPosition(ChunkPosition& x, ChunkPosition& y, ChunkPosition& z)
{
	x = _dx;
	y = _dy;
	z = _dz;
}

void
TerrainMap::copy(TerrainMap* map)
{
	_dx = map->_dx;
	_dy = map->_dy;
	_dz = map->_dz;
	_size = map->_size;
	_count = map->_count;
	_mask = map->_mask;
	_data = map->_data;
}

void
TerrainMap::grow()
{
	TerrainMap map(_terrain);
	map.create(_size, _dx, _dy, _dz, _mask << 1 | 1);

	for (auto& it : _data)
	{
		if (!it.empty())
		{
			map.set(it.x, it.y, it.z, it.instanceID);
		}
	}

	_mask = map._mask;
	_size = map._size;
	_data.swap(map._data);
}

MapEntrys&
TerrainMap::getEntrys() noexcept
{
	return _data;
}