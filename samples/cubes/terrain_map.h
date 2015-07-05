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
#ifndef _H_TERRAIN_MAP_H_
#define _H_TERRAIN_MAP_H_

#include "terrain_types.h"

#define EMPTY_ENTRY (-1)

class MapEntry
{
public:
	MapEntry()
	{
		x = EMPTY_ENTRY;
		y = EMPTY_ENTRY;
		z = EMPTY_ENTRY;
		instanceID = 0;
	}

	bool empty()
	{
		return
			x == EMPTY_ENTRY ||
			y == EMPTY_ENTRY ||
			z == EMPTY_ENTRY ? true : false;
	}

	BlockPosition x;
	BlockPosition y;
	BlockPosition z;

	ItemID instanceID;
};

class TerrainMap final
{
public:
	TerrainMap();
	~TerrainMap();

	void create(std::size_t size, ChunkPosition x, ChunkPosition y, ChunkPosition z, std::size_t mask);
	void close();

	bool set(BlockPosition x, BlockPosition y, BlockPosition z, ItemID id);
	ItemID get(BlockPosition x, BlockPosition y, BlockPosition z);

	std::size_t size() const noexcept;
	std::size_t count() const noexcept;

	void getPosition(ChunkPosition& x, ChunkPosition& y, ChunkPosition &z);

	void copy(TerrainMap* map);

	MapEntrys& getEntrys() noexcept;

	void grow();

private:
	TerrainMap(const TerrainMap&) noexcept = delete;
	TerrainMap& operator=(const TerrainMap&) noexcept = delete;

private:

	ChunkPosition _dx;
	ChunkPosition _dy;
	ChunkPosition _dz;

	std::size_t _mask;
	std::size_t _size;
	std::size_t _count;

	std::vector<MapEntry> _data;
};

#endif