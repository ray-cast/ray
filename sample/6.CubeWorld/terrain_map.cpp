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

TerrainMap::TerrainMap() noexcept
	: _mask(0)
{
}

TerrainMap::TerrainMap(std::size_t mask) noexcept
{
	this->create(mask);
}

TerrainMap::~TerrainMap() noexcept
{
	this->clear();
}

void
TerrainMap::create(std::size_t mask) noexcept
{
	_mask = mask;
	_count = 0;
	_data.resize(mask + 1);
}

void
TerrainMap::clear() noexcept
{
	_data.clear();
}

bool
TerrainMap::set(const TerrainData& data) noexcept
{
	std::size_t index = ray::hash_int(data.x, data.y, data.z) & _mask;

	auto* entry = &_data[index];

	int overwrite = 0;
	while (!entry->empty())
	{
		if (entry->x == data.x && entry->y == data.y && entry->z == data.z)
		{
			overwrite = 1;
			break;
		}

		index = (index + 1) & _mask;
		entry = &_data[index];
	}

	if (overwrite)
	{
		if (entry->instanceID != data.instanceID)
		{
			entry->instanceID = data.instanceID;
			return true;
		}
	}
	else if (data.instanceID)
	{
		*entry = data;

		_count++;
		if (_count * 2 > _mask)
			this->grow();

		return true;
	}

	return false;
}

bool
TerrainMap::get(TerrainData& data) noexcept
{
	std::size_t index = ray::hash_int(data.x, data.y, data.z) & _mask;

	TerrainData* entry = &_data[index];

	while (!entry->empty())
	{
		if (entry->x == data.x &&
			entry->y == data.y &&
			entry->z == data.z)
		{
			data.instanceID = entry->instanceID;
			return true;
		}

		index = (index + 1) & _mask;
		entry = &_data[index];
	}

	return false;
}

std::size_t
TerrainMap::count() const noexcept
{
	return _count;
}

void
TerrainMap::grow() noexcept
{
	TerrainMap map;
	map.create(_mask << 1 | 1);

	for (auto& it : _data)
	{
		if (!it.empty())
		{
			map.set(it);
		}
	}

	_mask = map._mask;
	_count = map._count;
	_data.swap(map._data);
}

TerrainDatas&
TerrainMap::data() noexcept
{
	return _data;
}