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
#ifndef _H_TERRAIN_CHUNK_H_
#define _H_TERRAIN_CHUNK_H_

#include "terrain_map.h"

class Terrain;
class TerrainChunk
{
public:
    TerrainChunk(Terrain& terrain) noexcept;
    ~TerrainChunk() noexcept;

    void init(std::size_t size, int x, int y, int z) noexcept;
    void realize() noexcept;
    void active(ray::GameObject* parent) noexcept;

    void dirt(bool dirt) noexcept;
    bool dirt() const noexcept;

    int distance(int x, int y, int z) noexcept;

    void getPosition(int& x, int& y, int &z) noexcept;

    bool set(ChunkX x, ChunkY y, ChunkZ z, ItemID id) noexcept;
    ItemID get(ChunkX x, ChunkY y, ChunkZ z) noexcept;

private:
    TerrainChunk(const TerrainChunk&) noexcept = delete;
    TerrainChunk& operator=(const TerrainChunk&) noexcept = delete;

private:

    bool _dirt;

    Terrain& _terrain;

    TerrainMapPtr _map;
    TerrainObjects _objects;
};

#endif
