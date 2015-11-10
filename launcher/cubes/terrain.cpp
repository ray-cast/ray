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
#include "terrain.h"
#include "terrain_items.h"

__ImplementSubClass(TerrainComponent, ray::GameController)

TerrainComponent::TerrainComponent() noexcept
	: _createRadius(2)
	, _deleteRadius(9)
	, _renderRadius(10)
	, _signRadius(10)
	, _hitRadius(100)
	, _size(32)
	, _scale(2)
	, _dayTimer(0)
{
	_maxItem = std::numeric_limits<ItemID>::max();
	_maxChunks = _deleteRadius  * _deleteRadius * _deleteRadius;

	this->addObject(std::make_shared<TerrainGrass>());
	this->addObject(std::make_shared<TerrainTree>());
	this->addObject(std::make_shared<TerrainClound>());
	this->addObject(std::make_shared<TerrainWater>());
}

TerrainComponent::~TerrainComponent() noexcept
{
}

bool
TerrainComponent::addBlockByMousePos(std::int32_t x, std::int32_t y) noexcept
{
	ray::Vector3 pos(x, y, 1);

	auto player = find<ray::GameObject>("first_person_camera");
	if (!player)
		return false;

	auto translate = player->getTranslate();

	auto world = player->getComponent<ray::CameraComponent>()->screenToWorld(pos);
	auto view = world - player->getTranslate();
	view.normalize();

	return this->addBlockByRaycast(translate, view);
}

bool
TerrainComponent::addBlockByRaycast(const ray::Vector3& translate, const ray::Vector3& view) noexcept
{
	ray::int3 out;
	auto chunk = this->getChunkByRaycast(translate, view, out);
	if (chunk)
	{
		ray::float3 cur(out.x, out.y, out.z);

		ray::int3 block = out;

		ChunkPosition x, y, z;
		chunk->getPosition(x, y, z);

		ChunkPosition ix = x;
		ChunkPosition iy = y;
		ChunkPosition iz = z;

		ItemID hw = 0;

		do
		{
			cur.x -= view.x * 0.5;
			cur.y -= view.y * 0.5;
			cur.z -= view.z * 0.5;

			block.x = roundf(cur.x);
			block.y = roundf(cur.y);
			block.z = roundf(cur.z);

			if (block.x >= _size) ix++;
			if (block.y >= _size) iy++;
			if (block.z >= _size) iz++;
			if (block.x < 0) ix--;
			if (block.y < 0) iy--;
			if (block.z < 0) iz--;

			if (ix != x || iy != y || iz != z)
			{
				chunk = this->getChunkByChunkPos(ix, iy, iz);
				x = ix;
				y = iy;
				z = iz;
			}

			if (chunk)
			{
				hw = chunk->get(block.x, block.y, block.z);
			}
		} while (hw);

		if (chunk && hw == 0)
		{
			chunk->set(block.x, block.y, block.z, 1);
			chunk->update();
			return true;
		}
	}

	return false;
}

bool
TerrainComponent::removeBlockByMousePos(std::int32_t x, std::int32_t y) noexcept
{
	ray::int3 out;
	auto chunk = getChunkByMousePos(x, y, out);
	if (chunk)
	{
		chunk->set(out.x, out.y, out.z, 0);
		chunk->update();
		return true;
	}

	return false;
}

bool
TerrainComponent::removeBlockByRaycast(const ray::Vector3& translate, const ray::Vector3& view) noexcept
{
	ray::int3 out;
	auto chunk = getChunkByRaycast(translate, view, out);
	if (chunk)
	{
		chunk->set(out.x, out.y, out.z, 0);
		chunk->update();
		return true;
	}

	return false;
}

TerrainChunkPtr
TerrainComponent::getChunkByChunkPos(ChunkPosition x, ChunkPosition y, ChunkPosition z) const noexcept
{
	for (auto& it : _chunks)
	{
		ChunkPosition _x, _y, _z;
		it->getPosition(_x, _y, _z);

		if (_x == x && _y == y && _z == z)
		{
			return it;
		}
	}

	return nullptr;
}

TerrainChunkPtr
TerrainComponent::getChunkByMousePos(std::int32_t x, std::int32_t y, ray::int3& out) const noexcept
{
	ray::Vector3 pos(x, y, 1);

	auto player = find<ray::GameObject>("first_person_camera");
	if (!player)
		return false;

	auto translate = player->getTranslate();

	auto world = player->getComponent<ray::CameraComponent>()->screenToWorld(pos);
	auto view = world - player->getTranslate();
	view.normalize();

	return this->getChunkByRaycast(translate, view, out);
}

TerrainChunkPtr
TerrainComponent::getChunkByRaycast(const ray::Vector3& translate, const ray::Vector3& view, ray::int3& out) const noexcept
{
	BlockPosition x = chunked(translate.x);
	BlockPosition y = chunked(translate.y);
	BlockPosition z = chunked(translate.z);

	BlockPosition px = 0;
	BlockPosition py = 0;
	BlockPosition pz = 0;

	float lastX = x;
	float lastY = y;
	float lastZ = z;

	float cur = 0;
	float step = view.length();
	if (std::isnan(step))
	{
		return nullptr;
	}

	auto chunk = this->getChunkByChunkPos(x, y, z);

	ray::Vector3 pos;
	pos.x = roundf(translate.x - unchunk(x));
	pos.y = roundf(translate.y - unchunk(y));
	pos.z = roundf(translate.z - unchunk(z));

	std::int32_t scaleSize = _size * _scale;

	while (cur < _hitRadius)
	{
		if (lastX != x || lastY != y || z != lastZ)
		{
			chunk = this->getChunkByChunkPos(x, y, z);

			lastX = x;
			lastY = y;
			lastZ = z;
		}

		if (chunk)
		{
			BlockPosition nx = roundf(pos.x * 0.5);
			BlockPosition ny = roundf(pos.y * 0.5);
			BlockPosition nz = roundf(pos.z * 0.5);
			if (nx != px || ny != py || nz != pz)
			{
				auto hw = chunk->get(nx, ny, nz);
				if (hw > 0)
				{
					out.x = nx;
					out.y = ny;
					out.z = nz;
					return chunk;
				}

				px = nx;
				py = ny;
				pz = nz;
			}
		}

		pos += view;
		cur += step;

		if (pos.x < 0) { pos.x = scaleSize; x--; }
		if (pos.y < 0) { pos.y = scaleSize; y--; }
		if (pos.z < 0) { pos.z = scaleSize; z--; }
		if (pos.x > scaleSize) { pos.x = 0; x++; }
		if (pos.y > scaleSize) { pos.y = 0; y++; }
		if (pos.z > scaleSize) { pos.z = 0; z++; }
	}

	return nullptr;
}

ChunkPosition
TerrainComponent::chunked(float x) const noexcept
{
	return  std::floor(x / _scale / _size);
}

float
TerrainComponent::unchunk(ChunkPosition x) const noexcept
{
	return x * _size * _scale;
}

bool
TerrainComponent::visiable(const ray::Frustum& fru, ChunkPosition x, ChunkPosition y, ChunkPosition z) const noexcept
{
	ray::AABB aabb;

	float dx = unchunk(x);
	float dy = unchunk(y);
	float dz = unchunk(z);

	ray::Vector3 min = ray::Vector3(dx - _size, dy - _size, dz - _size);
	ray::Vector3 max = ray::Vector3(dx + _size, dy + _size, dz + _size);

	aabb.encapsulate(min);
	aabb.encapsulate(max);

	return fru.contains(aabb);
}

void
TerrainComponent::addItem(TerrainItemPtr item) noexcept
{
	if (_itmes.size() < _maxItem)
	{
		_itmes.push_back(item);
		item->setInstance(_itmes.size());
	}
}

void
TerrainComponent::removeItem(TerrainItemPtr item) noexcept
{
}

void
TerrainComponent::addObject(TerrainObjectPtr object) noexcept
{
	_objects.push_back(object);
	for (auto& it : object->getItems())
	{
		this->addItem(it);
	}
}

void
TerrainComponent::removeObject(TerrainObjectPtr object) noexcept
{
}

TerrainObjects&
TerrainComponent::getObjects() noexcept
{
	return _objects;
}

void
TerrainComponent::deleteChunks() noexcept
{
	auto it = _chunks.begin();
	auto end = _chunks.end();

	for (; it != end; ++it)
	{
		bool destroy = true;

		auto translate = _player->getTranslate();

		auto x = chunked(translate.x);
		auto y = chunked(translate.y);
		auto z = chunked(translate.z);

		if ((*it)->distance(x, y, z) < _deleteRadius)
		{
			destroy = false;
		}

		if (destroy)
		{
			it = _chunks.erase(it);
			end = _chunks.end();
			if (it == end)
				break;
		}
	}
}

void
TerrainComponent::createChunks() noexcept
{
	if (_chunks.size() > _maxChunks)
		return;

	auto translate = _player->getTranslate();
	ChunkPosition x = chunked(translate.x);
	ChunkPosition y = 0;
	ChunkPosition z = chunked(translate.z);

	ray::Frustum fru;
	fru.extract(_player->getComponent<ray::CameraComponent>()->getViewProject());

	for (auto& it : _threads)
	{
		if (it->state != TerrainThread::IDLE)
			continue;

		ChunkPosition bestX = 0;
		ChunkPosition bestY = 0;
		ChunkPosition bestZ = 0;
		ChunkPosition start = std::numeric_limits<ChunkPosition>::max();
		ChunkPosition bestScore = start;

		for (std::int32_t iq = -_createRadius; iq <= _createRadius; iq++)
		{
			for (std::int32_t ip = -_createRadius; ip <= _createRadius; ip++)
			{
				ChunkPosition dx = x + iq;
				ChunkPosition dy = y;
				ChunkPosition dz = z + ip;

				auto chunk = this->getChunkByChunkPos(dx, dy, dz);
				if (chunk && !chunk->dirt())
					continue;

				std::int32_t invisiable = !this->visiable(fru, dx, dy, dz);
				std::int32_t distance = std::max(std::abs(iq), std::abs(ip));
				std::int32_t score = (invisiable << 24) | distance;
				if (score < bestScore)
				{
					bestScore = score;
					bestX = dx;
					bestZ = dz;
				}
			}
		}

		if (start == bestScore)
			return;

		auto chunk = this->getChunkByChunkPos(bestX, bestY, bestZ);
		if (!chunk)
		{
			chunk = std::make_shared<TerrainChunk>(*this);
			chunk->init(_size, bestX, bestY, bestZ);

			_chunks.push_back(chunk);
		}

		it->chunk = chunk;
		it->chunk->dirt(false);
		it->state = TerrainThread::BUSY;
		it->dispose.notify_one();
	}
}

void
TerrainComponent::checkChunks() noexcept
{
	for (auto& it : _threads)
	{
		if (it->state != TerrainThread::DONE)
			continue;

		if (it->chunk)
		{
			it->chunk->active(std::dynamic_pointer_cast<ray::GameObject>(this->getGameObject()->shared_from_this()));
			it->chunk = nullptr;
		}

		it->state = TerrainThread::IDLE;
	}
}

void
TerrainComponent::hitChunks() noexcept
{
	auto inputFeatures = this->getGameServer()->getFeature<ray::InputFeatures>();
	if (inputFeatures)
	{
		auto input = inputFeatures->getInput();
		if (input->getButtonDown(ray::InputButton::MOUSE0))
		{
			if (input->getKey(ray::InputKey::LCTRL) || input->isLockedCursor())
			{
				this->removeBlockByMousePos(input->getMousePosX(), input->getMousePosY());
			}
		}
		else if (input->getButtonDown(ray::InputButton::MOUSE2))
		{
			this->addBlockByMousePos(input->getMousePosX(), input->getMousePosY());
		}
	}
}

void
TerrainComponent::dispose(std::shared_ptr<TerrainThread> ctx) noexcept
{
	while (!ctx->isQuitRequest)
	{
		if (ctx->state != TerrainThread::BUSY)
		{
			ctx->mutex.lock();
			ctx->dispose.wait(ctx->mutex);
			ctx->mutex.unlock();
			continue;
		}

		if (ctx->chunk)
		{
			ctx->chunk->realize();
		}

		ctx->state = TerrainThread::DONE;
	}
}

ray::GameComponentPtr
TerrainComponent::clone() const noexcept
{
	return nullptr;
}

void
TerrainComponent::onActivate() except
{
	_player = find<ray::GameObject>("first_person_camera");
	if (_player)
	{
		ray::Vector3 translate = _player->getTranslate();

		auto x = chunked(translate.x);
		auto y = chunked(translate.y);
		auto z = chunked(translate.z);

		for (ChunkPosition i = x - 1; i < x + 1; i++)
		{
			for (ChunkPosition j = z - 1; j < z + 1; j++)
			{
				auto chunk = std::make_shared<TerrainChunk>(*this);
				chunk->init(_size, i, 0, j);
				chunk->realize();
				chunk->active(std::dynamic_pointer_cast<ray::GameObject>(this->getGameObject()->shared_from_this()));

				_chunks.push_back(chunk);
			}
		}

		for (int i = 0; i < MAX_THREADS; i++)
		{
			auto thread = std::make_shared<TerrainThread>();
			thread->_thread = std::make_unique<std::thread>(std::bind(&TerrainComponent::dispose, this, thread));

			_threads.push_back(thread);
		}
	}
}

void
TerrainComponent::onDeactivate() except
{
	_chunks.clear();
	_itmes.clear();

	for (auto& it : _threads)
	{
		it->isQuitRequest = true;

		if (it->_thread)
		{
			it->dispose.notify_one();

			it->_thread->join();
			it->_thread = nullptr;
		}
	}

	_threads.clear();
}

void
TerrainComponent::onFrame() except
{
	this->deleteChunks();
	this->checkChunks();
	this->createChunks();
	this->hitChunks();

	/*auto sun = this->find<ray::GameObject>("sun");
	if (sun)
	{
		float dayTimer = this->getGameServer()->getTimer()->elapsed() / 50;
		dayTimer -= int(dayTimer);

		_dayTimer += this->getGameServer()->getTimer()->delta() / 50;

		float sunY = cos(_dayTimer);
		float sunZ = sin(_dayTimer);

		auto pos = sun->getTranslate();
		pos.y = sunY * 50;
		pos.z = sunZ * 50;

		sun->setTranslate(pos);
	}*/
}