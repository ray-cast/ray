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
#include "terrain_items.h"
#include <ray/physics_body_component.h>
#include <ray/physics_heightmap_component.h>
#include <ray/physics_mesh_component.h>
#include <ray/physics_box_component.h>

TerrainGrass::TerrainGrass() noexcept
{
    _grass = std::make_shared<Grass>();
    _grassObject = ray::GameObject::find<ray::GameObject>("grass");

    this->addItem(_grass);
}

TerrainGrass::~TerrainGrass() noexcept
{
    if (_object)
    {
        _object->destroy();
        _object = nullptr;
    }
}

bool
TerrainGrass::visiable(TerrainMapPtr map, int x, int y, int z) noexcept
{
    int f1 = map->get(x - 1, y, z);
    int f2 = map->get(x + 1, y, z);
    int f3 = map->get(x, y - 1, z);
    int f4 = map->get(x, y + 1, z);
    int f5 = map->get(x, y, z - 1);
    int f6 = map->get(x, y, z + 1);

    f1 = (f1 == 0 || f1 != _grass->getInstance()) ? 1 : 0;
    f2 = (f2 == 0 || f2 != _grass->getInstance()) ? 1 : 0;
    f3 = ((f3 == 0 || f3 != _grass->getInstance()) && y > 0) ? 1 : 0;
    f4 = (f4 == 0 || f4 != _grass->getInstance()) ? 1 : 0;
    f5 = (f5 == 0 || f5 != _grass->getInstance()) ? 1 : 0;
    f6 = (f6 == 0 || f6 != _grass->getInstance()) ? 1 : 0;

    return (f1 + f2 + f3 + f4 + f5 + f6) ? true : false;
}

bool
TerrainGrass::create(TerrainMapPtr map) noexcept
{
    int size = map->size();
    int half = size >> 1;

    int _x, _y, _z;
    map->getPosition(_x, _y, _z);

    int offsetX = _x * size;
    int offsetZ = _z * size;

    for (int x = 0; x < size; x++)
    {
        for (int z = 0; z < size; z++)
        {
            int dx = offsetX + x;
            int dz = offsetZ + z;

            float f = ray::simplex2(dx * 0.01f, dz * 0.01f, 4, 0.5f, 2);
            float g = ray::simplex2(-dx * 0.01f, -dz * 0.01f, 2, 0.9f, 2);

            int h = f * (g * size + half);

            for (int y = 0; y < h; y++)
            {
                map->set(x, y, z, _grass->getInstance());
            }
        }
    }

    return true;
}

bool
TerrainGrass::createObject(TerrainMapPtr map) noexcept
{
    int mx, my, mz;
    map->getPosition(mx, my, mz);

    int size = map->size();

    int offsetX = mx * size << 1;
    int offsetY = my * size << 1;
    int offsetZ = mz * size << 1;

    auto mesh = _grassObject->getComponent<ray::MeshComponent>();

    ray::CombineInstance combines;

    for (auto& it : map->getEntrys())
    {
        if (it.empty() || it.instanceID != _grass->getInstance())
            continue;

        int total = this->visiable(map, it.x, it.y, it.z);
        if (total)
        {
            int dx = it.x << 1;
            int dy = it.y << 1;
            int dz = it.z << 1;

            ray::CombineInstance::Instance instance;
            instance.mesh = mesh->getMesh();
            instance.transform.makeTranslate(dx, dy, dz);

            combines.push_back(instance);
        }
    }

    if (!combines.empty())
    {
        auto gameObject = _grassObject->clone();
        gameObject->setName(ray::format("chunk_%d_%d_%d") % offsetX % offsetY % offsetZ);
        gameObject->setTranslate(ray::Vector3(offsetX, offsetY, offsetZ));
        gameObject->getComponent<ray::MeshComponent>()->setCombieInstnace(combines);

        auto rigidbody = std::make_shared<ray::PhysicsBodyComponent>();
        gameObject->addComponent(rigidbody);
        gameObject->addComponent(std::make_shared<ray::PhysicsMeshComponent>());

        _object = gameObject;

        return true;
    }

    return false;
}

bool
TerrainGrass::active(ray::GameObjectPtr parent) noexcept
{
    if (parent)
    {
        _object->setParent(parent);
        _object->setActive(true);
        _object->getComponent<ray::MeshComponent>()->clear();
    }
    else
    {
        _object->setActive(false);
        _object->setParent(nullptr);
    }

    return true;
}

bool
TerrainGrass::update(TerrainMapPtr map, ChunkX x, ChunkY y, ChunkZ z, ItemID old, ItemID id) noexcept
{
    if (id == _grass->getInstance() || old == _grass->getInstance())
    {
        auto parent = _object->getParent();

        _object->destroy();
        _object = nullptr;

        this->createObject(map);
        this->active(parent);
    }

    return true;
}

TerrainObjectPtr
TerrainGrass::clone() noexcept
{
    auto result = std::make_shared<TerrainGrass>();
    result->_grass = this->_grass;
    return result;
}

TerrainTree::TerrainTree() noexcept
{
    _wood = std::make_shared<Wood>();
    _leaf = std::make_shared<Leaf>();

    _woodObject = ray::GameObject::find<ray::GameObject>("wood");
    _leafObject = ray::GameObject::find<ray::GameObject>("leaf");

    this->addItem(_wood);
    this->addItem(_leaf);
}

TerrainTree::~TerrainTree() noexcept
{
    for (auto& it : _objects)
    {
        if (it)
        {
            it->destroy();
            it = nullptr;
        }
    }
}

bool
TerrainTree::create(TerrainMapPtr map) noexcept
{
    int size = map->size();
    int half = size >> 1;

    int _x, _y, _z;
    map->getPosition(_x, _y, _z);

    int offsetX = _x * size;
    int offsetZ = _z * size;

    for (int x = 0; x < size; x++)
    {
        for (int z = 0; z < size; z++)
        {
            int dx = offsetX + x;
            int dz = offsetZ + z;

            if (ray::simplex2(dx, dz, 6, 0.5, 2) < 0.86)
                continue;

            float f = ray::simplex2(dx * 0.01f, dz * 0.01f, 4, 0.5f, 2);
            float g = ray::simplex2(-dx * 0.01f, -dz * 0.01f, 2, 0.9f, 2);

            int h = f * (g * size + half);

            for (int y = h + 3; y < h + 8; y++)
            {
                for (int ox = -3; ox <= 3; ox++)
                {
                    for (int oz = -3; oz <= 3; oz++)
                    {
                        int d = (ox * ox) + (oz * oz) + (y - (h + 4)) * (y - (h + 4));
                        if (d < 11)
                        {
                            map->set(x + ox, y, z + oz, _leaf->getInstance());
                        }
                    }
                }
            }

            for (int y = h; y < h + 7; y++)
            {
                map->set(x, y, z, _wood->getInstance());
            }
        }
    }

    return true;
}

bool
TerrainTree::createObject(TerrainMapPtr map) noexcept
{
    int mx, my, mz;
    map->getPosition(mx, my, mz);

    int size = map->size();

    int offsetX = mx * size << 1;
    int offsetY = my * size << 1;
    int offsetZ = mz * size << 1;

    auto mesh = _woodObject->getComponent<ray::MeshComponent>();

    ray::CombineInstance _woods;
    ray::CombineInstance _leafs;

    for (auto& it : map->getEntrys())
    {
        if (it.empty())
            continue;

        if (it.instanceID != _wood->getInstance() &&
            it.instanceID != _leaf->getInstance())
            continue;

        int dx = it.x << 1;
        int dy = it.y << 1;
        int dz = it.z << 1;

        ray::CombineInstance::Instance instance;
        instance.mesh = mesh->getMesh();
        instance.transform.makeTranslate(dx, dy, dz);

        if (it.instanceID == _wood->getInstance())
            _woods.push_back(instance);

        if (it.instanceID == _leaf->getInstance())
            _leafs.push_back(instance);
    }

    if (!_woods.empty())
    {
        auto gameObject = _woodObject->clone();
        gameObject->setName(ray::format("chunk_wood_%d_%d_%d") % offsetX % offsetY % offsetZ);
        gameObject->setTranslate(ray::Vector3(offsetX, offsetY, offsetZ));
        gameObject->getComponent<ray::MeshComponent>()->setCombieInstnace(_woods);

        auto rigidbody = std::make_shared<ray::PhysicsBodyComponent>();
        gameObject->addComponent(rigidbody);
        gameObject->addComponent(std::make_shared<ray::PhysicsMeshComponent>());

        _objects.push_back(gameObject);
    }

    if (!_leafs.empty())
    {
        auto gameObject = _leafObject->clone();
        gameObject->setName(ray::format("chunk_leaf_%d_%d_%d") % offsetX % offsetY % offsetZ);
        gameObject->setTranslate(ray::Vector3(offsetX, offsetY, offsetZ));
        gameObject->getComponent<ray::MeshComponent>()->setCombieInstnace(_leafs);

        auto rigidbody = std::make_shared<ray::PhysicsBodyComponent>();
        gameObject->addComponent(rigidbody);
        gameObject->addComponent(std::make_shared<ray::PhysicsMeshComponent>());

        _objects.push_back(gameObject);
    }

    if (!_objects.empty())
        return true;

    return false;
}

bool
TerrainTree::active(ray::GameObjectPtr parent) noexcept
{
    for (auto& it : _objects)
    {
        if (parent)
        {
            it->setParent(parent);
            it->setActive(true);
            it->getComponent<ray::MeshComponent>()->clear();
        }
        else
        {
            it->setActive(false);
            it->setParent(nullptr);
        }
    }

    return true;
}

bool
TerrainTree::update(TerrainMapPtr map, ChunkX x, ChunkY y, ChunkZ z, ItemID old, ItemID id) noexcept
{
    return true;
}

TerrainObjectPtr
TerrainTree::clone() noexcept
{
    auto result = std::make_shared<TerrainTree>();
    result->_leaf = this->_leaf;
    result->_wood = this->_wood;
    return result;
}

TerrainClound::TerrainClound() noexcept
{
    _clound = std::make_shared<Clound>();
    _cloundObject = ray::GameObject::find<ray::GameObject>("clound");
    this->addItem(_clound);
}

TerrainClound::~TerrainClound() noexcept
{
    for (auto& it : _objects)
    {
        if (it)
        {
            it->destroy();
            it = nullptr;
        }
    }
}

bool
TerrainClound::create(TerrainMapPtr map) noexcept
{
    int size = map->size();

    int _x, _y, _z;
    map->getPosition(_x, _y, _z);

    int offsetX = _x * size;
    int offsetZ = _z * size;

    for (int x = 0; x < size; x++)
    {
        for (int z = 0; z < size; z++)
        {
            int dx = offsetX + x;
            int dz = offsetZ + z;

            for (int y = 64; y < 72; y++)
            {
                if (ray::simplex3(dx * 0.01f, y * 0.1f, dz * 0.01f, 8, 0.5f, 2) > 0.75)
                {
                    map->set(x, y, z, _clound->getInstance());
                }
            }
        }
    }

    return true;
}

bool
TerrainClound::createObject(TerrainMapPtr map) noexcept
{
    int mx, my, mz;
    map->getPosition(mx, my, mz);

    int size = map->size();

    int offsetX = mx * size << 1;
    int offsetY = my * size << 1;
    int offsetZ = mz * size << 1;

    auto mesh = _cloundObject->getComponent<ray::MeshComponent>();

    ray::CombineInstance combines;

    for (auto& it : map->getEntrys())
    {
        if (it.empty())
            continue;

        if (it.instanceID != _clound->getInstance())
            continue;

        int dx = it.x << 1;
        int dy = it.y << 1;
        int dz = it.z << 1;

        ray::CombineInstance::Instance instance;
        instance.mesh = mesh->getMesh();
        instance.transform.makeTranslate(dx, dy, dz);

        combines.push_back(instance);
    }

    if (!combines.empty())
    {
        auto gameObject = _cloundObject->clone();
        gameObject->setName(ray::format("chunk_clound_%d_%d_%d") % offsetX % offsetY % offsetZ);
        gameObject->setTranslate(ray::Vector3(offsetX, offsetY, offsetZ));
        gameObject->getComponent<ray::MeshComponent>()->setCombieInstnace(combines);

        _objects.push_back(gameObject);
        return true;
    }

    return false;
}

bool
TerrainClound::active(ray::GameObjectPtr parent) noexcept
{
    for (auto& it : _objects)
    {
        if (parent)
        {
            it->setParent(parent);
            it->setActive(true);
            it->getComponent<ray::MeshComponent>()->clear();
        }
        else
        {
            it->setActive(false);
            it->setParent(nullptr);
        }
    }

    return true;
}

bool
TerrainClound::update(TerrainMapPtr map, ChunkX x, ChunkY y, ChunkZ z, ItemID old, ItemID id) noexcept
{
    return true;
}

TerrainObjectPtr
TerrainClound::clone() noexcept
{
    auto result = std::make_shared<TerrainClound>();
    result->_clound = this->_clound;
    return result;
}

TerrainWater::TerrainWater() noexcept
{
    _water = std::make_shared<Water>();
    _waterObject = ray::GameObject::find<ray::GameObject>("water");
    this->addItem(_water);
}

TerrainWater::~TerrainWater() noexcept
{
    for (auto& it : _objects)
    {
        if (it)
        {
            it->destroy();
            it = nullptr;
        }
    }
}

bool
TerrainWater::create(TerrainMapPtr map) noexcept
{
    int size = map->size();
    int half = size >> 1;

    int _x, _y, _z;
    map->getPosition(_x, _y, _z);

    int offsetX = _x * size;
    int offsetZ = _z * size;

    for (int x = 0; x < size; x++)
    {
        for (int z = 0; z < size; z++)
        {
            int dx = offsetX + x;
            int dz = offsetZ + z;

            float f = ray::simplex2(dx * 0.01f, dz * 0.01f, 4, 0.5f, 2);
            float g = ray::simplex2(-dx * 0.01f, -dz * 0.01f, 2, 0.9f, 2);

            int h = f * (g * size + half);

            if (h < 10)
            {
                for (int y = h; y < 10; y++)
                {
                    map->set(x, y, z, _water->getInstance());
                }
            }
        }
    }

    return true;
}

bool
TerrainWater::createObject(TerrainMapPtr map) noexcept
{
    int mx, my, mz;
    map->getPosition(mx, my, mz);

    int size = map->size();

    int offsetX = mx * size << 1;
    int offsetY = my * size << 1;
    int offsetZ = mz * size << 1;

    auto mesh = _waterObject->getComponent<ray::MeshComponent>();

    ray::CombineInstance _waters;

    for (auto& it : map->getEntrys())
    {
        if (it.empty())
            continue;

        if (it.instanceID != _water->getInstance())
            continue;

        int dx = it.x << 1;
        int dy = it.y << 1;
        int dz = it.z << 1;

        ray::CombineInstance::Instance instance;
        instance.mesh = mesh->getMesh();
        instance.transform.makeTranslate(dx, dy, dz);

        if (it.instanceID == _water->getInstance())
            _waters.push_back(instance);
    }

    if (!_waters.empty())
    {
        auto gameObject = _waterObject->clone();
        gameObject->setName(ray::format("chunk_water_%d_%d_%d") % offsetX % offsetY % offsetZ);
        gameObject->setTranslate(ray::Vector3(offsetX, offsetY, offsetZ));
        gameObject->getComponent<ray::MeshComponent>()->setCombieInstnace(_waters);

        auto rigidbody = std::make_shared<ray::PhysicsBodyComponent>();
        gameObject->addComponent(rigidbody);
        gameObject->addComponent(std::make_shared<ray::PhysicsMeshComponent>());

        _objects.push_back(gameObject);

        return true;
    }

    return false;
}

bool
TerrainWater::active(ray::GameObjectPtr parent) noexcept
{
    for (auto& it : _objects)
    {
        if (parent)
        {
            it->setParent(parent);
            it->setActive(true);
            it->getComponent<ray::MeshComponent>()->clear();
        }
        else
        {
            it->setActive(false);
            it->setParent(nullptr);
        }
    }

    return true;
}

bool
TerrainWater::update(TerrainMapPtr map, ChunkX x, ChunkY y, ChunkZ z, ItemID old, ItemID id) noexcept
{
    return false;
}

TerrainObjectPtr
TerrainWater::clone() noexcept
{
    auto result = std::make_shared<TerrainWater>();
    result->_water = this->_water;
    return result;
}