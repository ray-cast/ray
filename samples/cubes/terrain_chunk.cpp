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
#include "terrain_chunk.h"
#include "terrain.h"

TerrainChunk::TerrainChunk(TerrainComponent& terrain) noexcept
	: _terrain(terrain)
	, _dirt(false)
{
}

TerrainChunk::~TerrainChunk() noexcept
{
	_objects.clear();
}

void
TerrainChunk::init(std::size_t size, ChunkPosition x, ChunkPosition y, ChunkPosition z) noexcept
{
	_map = std::make_shared<TerrainMap>(_terrain);
	_map->create(size, x, y, z, 0x7FFF);
}

std::size_t
TerrainChunk::distance(ChunkPosition x, ChunkPosition y, ChunkPosition z) noexcept
{
	ChunkPosition _x, _y, _z;
	_map->getPosition(_x, _y, _z);

	int dx = abs(_x - x);
	int dy = abs(_y - y);
	int dz = abs(_z - z);

	return std::max(std::max(dx, dy), dz);
}

void
TerrainChunk::getPosition(ChunkPosition& x, ChunkPosition& y, ChunkPosition& z) noexcept
{
	_map->getPosition(x, y, z);
}

bool
TerrainChunk::set(BlockPosition x, BlockPosition y, BlockPosition z, ItemID id) noexcept
{
	return _map->set(x, y, z, id);
}

ItemID
TerrainChunk::get(BlockPosition x, BlockPosition y, BlockPosition z) noexcept
{
	return _map->get(x, y, z);
}

void
TerrainChunk::update() noexcept
{
	for (auto& it : _objects)
	{
		it->update(_map);
	}
}

void
TerrainChunk::realize() noexcept
{
	auto objects = _terrain.getObjects();
	for (auto& it : objects)
	{
		auto object = it->clone();
		if (object->create(_map))
		{
			_objects.push_back(object);
		}
	}

	for (auto& it : _objects)
	{
		it->createObject(_map);
	}
}

void
TerrainChunk::dirt(bool dirt) noexcept
{
	_dirt = dirt;
}

bool
TerrainChunk::dirt() const noexcept
{
	return _dirt;
}

void
TerrainChunk::active(ray::GameObjectPtr gameobj) noexcept
{
	for (auto& it : _objects)
	{
		if (it)
		{
			it->active(gameobj);
		}
	}
}