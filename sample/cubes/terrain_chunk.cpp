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
	, _active(false)
{
}

TerrainChunk::~TerrainChunk() noexcept
{
	_objects.clear();
}

void
TerrainChunk::create(std::int32_t x, std::int32_t y, std::int32_t z, std::size_t size) noexcept
{
	_x = x;
	_y = y;
	_z = z;
	_size = size;
	_map = std::make_shared<TerrainMap>();
	_map->create(0x7FFF);

	auto objects = _terrain.getObjects();
	for (auto& it : objects)
	{
		auto object = it->clone();
		if (object->create(*this))
		{
			_objects.push_back(object);
		}
	}

	for (auto& it : _objects)
	{
		it->createObject(*this);
	}
}

std::size_t
TerrainChunk::distance(std::int32_t x, std::int32_t y, std::int32_t z) noexcept
{
	int dx = abs(_x - x);
	int dy = abs(_y - y);
	int dz = abs(_z - z);
	return std::max(std::max(dx, dy), dz);
}

void
TerrainChunk::getPosition(std::int32_t& x, std::int32_t& y, std::int32_t& z) noexcept
{
	x = _x;
	y = _y;
	z = _z;
}

bool
TerrainChunk::set(const TerrainData& data) noexcept
{
	assert(data.x >= 0 || data.x < _size);
	assert(data.y >= 0 || data.y < _size);
	assert(data.z >= 0 || data.z < _size);
	return _map->set(data);
}

bool
TerrainChunk::get(TerrainData& data) const noexcept
{
	if (data.x < 0 || data.x >= _size) return 0;
	if (data.y < 0 || data.y >= _size) return 0;
	if (data.z < 0 || data.z >= _size) return 0;
	return _map->get(data);
}

std::size_t
TerrainChunk::size() const noexcept
{
	return _size;
}

const TerrainDatas&
TerrainChunk::data() const noexcept
{
	return _map->data();
}

void
TerrainChunk::update() noexcept
{
	for (auto& it : _objects)
	{
		it->update(*this);
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
TerrainChunk::setActive(bool active) noexcept
{
	if (_active != active)
	{
		for (auto& it : _objects)
		{
			if (it)
			{
				it->setActive(active);
			}
		}

		_active = active;
	}
}

bool 
TerrainChunk::getActive() const noexcept
{
	return _active;
}