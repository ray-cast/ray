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
#include <ray/game_object_manager.h>

TerrainGrass::TerrainGrass() noexcept
{
	_grass = std::make_shared<Grass>();
	_grassObject = ray::GameObjectManager::instance()->findObject("grass");

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
TerrainGrass::create(TerrainChunk& chunk) noexcept
{
	int size = chunk.size();
	int half = size >> 1;

	int _x, _y, _z;
	chunk.getPosition(_x, _y, _z);

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
				TerrainData data;
				data.x = x;
				data.y = y;
				data.z = z;
				data.instanceID = _grass->getInstance();
				chunk.set(data);
			}
		}
	}

	return true;
}

bool
TerrainGrass::createObject(TerrainChunk& chunk) noexcept
{
	auto mesh = std::make_shared<ray::MeshProperty>();

	for (auto& it : chunk.data())
	{
		if (it.empty() || it.instanceID != _grass->getInstance())
			continue;

		if (it.y <= 0)
			continue;

		VisiableFaces faces;
		int total = this->visiable(chunk, it, faces);
		if (total)
		{
			int dx = it.x << 1;
			int dy = it.y << 1;
			int dz = it.z << 1;

			this->makeCube(mesh, faces, dx, dy, dz, 1);
		}
	}

	if (mesh->getNumVertices())
	{
		int mx, my, mz;
		chunk.getPosition(mx, my, mz);

		int size = chunk.size();

		int offsetX = mx * size << 1;
		int offsetY = my * size << 1;
		int offsetZ = mz * size << 1;

		auto gameObject = _grassObject->clone();
		gameObject->setName(ray::format("chunk_%d_%d_%d") % offsetX % offsetY % offsetZ);
		gameObject->setTranslate(ray::Vector3(offsetX, offsetY, offsetZ));
		gameObject->getComponent<ray::MeshComponent>()->setMesh(mesh);

		_object = gameObject;

		return true;
	}

	return false;
}

bool
TerrainGrass::setActive(bool active) noexcept
{
	if (_object)
	{
		if (active)
		{
			_object->setActive(true);
		}
		else
		{
			_object->setActive(false);
			_object->setParent(nullptr);
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool
TerrainGrass::update(TerrainChunk& chunk) noexcept
{
	_object->destroy();
	_object = nullptr;

	this->createObject(chunk);
	this->setActive(true);

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

	_woodObject = ray::GameObjectManager::instance()->findObject("wood");
	_leafObject = ray::GameObjectManager::instance()->findObject("leaf");

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
TerrainTree::create(TerrainChunk& chunk) noexcept
{
	int size = chunk.size();
	int half = size >> 1;

	int _x, _y, _z;
	chunk.getPosition(_x, _y, _z);

	int offsetX = _x * size;
	int offsetZ = _z * size;

	for (int x = 3; x < size - 3; x++)
	{
		for (int z = 3; z < size - 3; z++)
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
							TerrainData data;
							data.x = x + ox;
							data.y = y;
							data.z = z + oz;
							data.instanceID = _leaf->getInstance();
							chunk.set(data);
						}
					}
				}
			}

			for (int y = h; y < h + 7; y++)
			{
				TerrainData data;
				data.x = x;
				data.y = y;
				data.z = z;
				data.instanceID = _wood->getInstance();
				chunk.set(data);
			}
		}
	}

	return true;
}

bool
TerrainTree::createObject(TerrainChunk& chunk) noexcept
{
	auto woods = std::make_shared<ray::MeshProperty>();
	auto leafs = std::make_shared<ray::MeshProperty>();

	for (auto& it : chunk.data())
	{
		if (it.empty())
			continue;

		if (it.instanceID != _wood->getInstance() &&
			it.instanceID != _leaf->getInstance())
			continue;

		VisiableFaces faces;
		int total = this->visiable(chunk, it, faces);
		if (total)
		{
			int dx = it.x << 1;
			int dy = it.y << 1;
			int dz = it.z << 1;

			if (it.instanceID == _wood->getInstance())
				this->makeCube(woods, faces, dx, dy, dz, 1);

			if (it.instanceID == _leaf->getInstance())
				this->makeCube(leafs, faces, dx, dy, dz, 1);
		}
	}

	int mx, my, mz;
	chunk.getPosition(mx, my, mz);

	int size = chunk.size();

	int offsetX = mx * size << 1;
	int offsetY = my * size << 1;
	int offsetZ = mz * size << 1;

	if (woods->getNumVertices())
	{
		auto gameObject = _woodObject->clone();
		gameObject->setName(ray::format("chunk_wood_%d_%d_%d") % offsetX % offsetY % offsetZ);
		gameObject->setTranslate(ray::Vector3(offsetX, offsetY, offsetZ));
		gameObject->getComponent<ray::MeshComponent>()->setMesh(woods);

		_objects.push_back(gameObject);
	}

	if (leafs->getNumVertices())
	{
		auto gameObject = _leafObject->clone();
		gameObject->setName(ray::format("chunk_leaf_%d_%d_%d") % offsetX % offsetY % offsetZ);
		gameObject->setTranslate(ray::Vector3(offsetX, offsetY, offsetZ));
		gameObject->getComponent<ray::MeshComponent>()->setMesh(leafs);

		_objects.push_back(gameObject);
	}

	if (!_objects.empty())
		return true;

	return false;
}

bool
TerrainTree::setActive(bool active) noexcept
{
	for (auto& it : _objects)
	{
		if (active)
		{
			it->setActive(true);
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
TerrainTree::update(TerrainChunk& chunk) noexcept
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
	_cloundObject = ray::GameObjectManager::instance()->findObject("clound");
	this->addItem(_clound);
}

TerrainClound::~TerrainClound() noexcept
{
	if (_object)
	{
		_object->destroy();
		_object = nullptr;
	}
}

bool
TerrainClound::create(TerrainChunk& chunk) noexcept
{
	int size = chunk.size();

	int _x, _y, _z;
	chunk.getPosition(_x, _y, _z);

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
					TerrainData data;
					data.x = x;
					data.y = y;
					data.z = z;
					data.instanceID = _clound->getInstance();
					chunk.set(data);
				}
			}
		}
	}

	return true;
}

bool
TerrainClound::createObject(TerrainChunk& chunk) noexcept
{
	auto mesh = std::make_shared<ray::MeshProperty>();

	for (auto& it : chunk.data())
	{
		if (it.empty())
			continue;

		if (it.instanceID != _clound->getInstance())
			continue;

		VisiableFaces faces;
		int total = this->visiable(chunk, it, faces);
		if (total)
		{
			int dx = it.x << 1;
			int dy = it.y << 1;
			int dz = it.z << 1;

			this->makeCube(mesh, faces, dx, dy, dz, 1);
		}
	}

	if (mesh->getNumVertices())
	{
		int mx, my, mz;
		chunk.getPosition(mx, my, mz);

		int size = chunk.size();

		int offsetX = mx * size << 1;
		int offsetY = my * size << 1;
		int offsetZ = mz * size << 1;

		auto gameObject = _cloundObject->clone();
		gameObject->setName(ray::format("chunk_%d_%d_%d") % offsetX % offsetY % offsetZ);
		gameObject->setTranslate(ray::Vector3(offsetX, offsetY, offsetZ));
		gameObject->getComponent<ray::MeshComponent>()->setMesh(mesh);

		_object = gameObject;

		return true;
	}

	return false;
}

bool
TerrainClound::setActive(bool active) noexcept
{
	if (_object)
	{
		if (active)
		{
			_object->setActive(true);
		}
		else
		{
			_object->setActive(false);
			_object->setParent(nullptr);
		}
	}

	return true;
}

bool
TerrainClound::update(TerrainChunk& chunk) noexcept
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
	_waterObject = ray::GameObjectManager::instance()->findObject("water");
	this->addItem(_water);
}

TerrainWater::~TerrainWater() noexcept
{
	if (_object)
	{
		_object->destroy();
		_object = nullptr;
	}
}

bool
TerrainWater::create(TerrainChunk& chunk) noexcept
{
	int size = chunk.size();
	int half = size >> 1;

	int _x, _y, _z;
	chunk.getPosition(_x, _y, _z);

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
					TerrainData data;
					data.x = x;
					data.y = y;
					data.z = z;
					data.instanceID = _water->getInstance();
					chunk.set(data);
				}
			}
		}
	}

	return true;
}

bool
TerrainWater::createObject(TerrainChunk& chunk) noexcept
{
	auto mesh = std::make_shared<ray::MeshProperty>();

	for (auto& it : chunk.data())
	{
		if (it.empty())
			continue;

		if (it.instanceID != _water->getInstance())
			continue;

		VisiableFaces faces;
		int total = this->visiable(chunk, it, faces);
		if (total)
		{
			int dx = it.x << 1;
			int dy = it.y << 1;
			int dz = it.z << 1;

			this->makeCube(mesh, faces, dx, dy, dz, 1);
		}
	}

	if (mesh->getNumVertices())
	{
		int mx, my, mz;
		chunk.getPosition(mx, my, mz);

		int size = chunk.size();

		int offsetX = mx * size << 1;
		int offsetY = my * size << 1;
		int offsetZ = mz * size << 1;

		auto gameObject = _waterObject->clone();
		gameObject->setName(ray::format("chunk_%d_%d_%d") % offsetX % offsetY % offsetZ);
		gameObject->setTranslate(ray::Vector3(offsetX, offsetY, offsetZ));
		gameObject->getComponent<ray::MeshComponent>()->setMesh(mesh);

		_object = gameObject;

		return true;
	}

	return false;
}

bool
TerrainWater::setActive(bool active) noexcept
{
	if (_object)
	{
		if (active)
		{
			_object->setActive(true);
		}
		else
		{
			_object->setActive(false);
			_object->setParent(nullptr);
		}
	}

	return true;
}

bool
TerrainWater::update(TerrainChunk& chunk) noexcept
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