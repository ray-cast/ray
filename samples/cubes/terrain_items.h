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
#ifndef _H_TERRAIN_ITEMS_H_
#define _H_TERRAIN_ITEMS_H_

#include "terrain.h"

class TerrainGrass : public TerrainObject
{
public:
    TerrainGrass() noexcept;
    ~TerrainGrass() noexcept;

    bool visiable(TerrainMapPtr map, int x, int y, int z)noexcept;

    bool create(TerrainMapPtr map) noexcept;
    bool createObject(TerrainMapPtr map) noexcept;

    bool active(ray::GameObject* parent) noexcept;

    bool update(TerrainMapPtr map, ChunkX x, ChunkY y, ChunkZ z, ItemID id) noexcept;

    TerrainObjectPtr clone() noexcept;

private:
    class Grass : public TerrainItem {};

    std::shared_ptr<Grass> _grass;

    ray::GameObjectPtr _grassObject;
    ray::GameObjectPtr _object;
};

class TerrainTree : public TerrainObject
{
public:
    TerrainTree() noexcept;
    ~TerrainTree() noexcept;

    bool create(TerrainMapPtr map) noexcept;
    bool createObject(TerrainMapPtr map) noexcept;

    bool active(ray::GameObject* parent) noexcept;

    bool update(TerrainMapPtr map, ChunkX x, ChunkY y, ChunkZ z, ItemID id) noexcept;

    TerrainObjectPtr clone() noexcept;

private:
    class Wood : public TerrainItem {};
    class Leaf : public TerrainItem {};

    std::shared_ptr<Wood> _wood;
    std::shared_ptr<Leaf> _leaf;

    ray::GameObjectPtr _woodObject;
    ray::GameObjectPtr _leafObject;

    ray::GameObjects _objects;
};

class TerrainClound : public TerrainObject
{
public:
    TerrainClound() noexcept;
    ~TerrainClound() noexcept;

    bool create(TerrainMapPtr map) noexcept;
    bool createObject(TerrainMapPtr map) noexcept;

    bool active(ray::GameObject* parent) noexcept;

    bool update(TerrainMapPtr map, ChunkX x, ChunkY y, ChunkZ z, ItemID id) noexcept;

    TerrainObjectPtr clone() noexcept;

private:
    class Clound : public TerrainItem {};

    std::shared_ptr<Clound> _clound;
    ray::GameObjectPtr _cloundObject;

    ray::GameObjects _objects;
};

class TerrainWater : public TerrainObject
{
public:
    TerrainWater() noexcept;
    ~TerrainWater() noexcept;

    bool create(TerrainMapPtr map) noexcept;
    bool createObject(TerrainMapPtr map) noexcept;

    bool active(ray::GameObject* parent) noexcept;

    bool update(TerrainMapPtr map, ChunkX x, ChunkY y, ChunkZ z, ItemID id) noexcept;

    TerrainObjectPtr clone() noexcept;

private:
    class Water : public TerrainItem {};

    std::shared_ptr<Water> _water;
    ray::GameObjectPtr _waterObject;

    ray::GameObjects _objects;
};

#endif