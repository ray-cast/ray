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

Terrain::Terrain() noexcept
    : _createRadius(2)
    , _deleteRadius(9)
    , _renderRadius(10)
    , _signRadius(10)
    , _hitRadius(5)
    , _size(32)
{
    _maxInstances = std::numeric_limits<ItemID>::max();
    _maxChunks = _deleteRadius  * _deleteRadius * _deleteRadius;
}

Terrain::~Terrain() noexcept
{
}

void
Terrain::onActivate() except
{
    _player = find<ray::GameObject>("first_person_camera");
    if (_player)
    {
        ray::Vector3 translate = _player->getTranslate();

        int x = chunked(translate.x);
        int y = chunked(translate.y);
        int z = chunked(translate.z);

        for (int i = x - 1; i < x + 1; i++)
        {
            for (int j = z - 1; j < z + 1; j++)
            {
                auto chunk = std::make_shared<TerrainChunk>(*this);
                chunk->init(_size, i, 0, j);
                chunk->realize();
                chunk->active(this->getGameObject());

                _chunks.push_back(chunk);
            }
        }

        for (std::size_t i = 0; i < 4; i++)
        {
            auto thread = std::make_shared<TerrainThread>();
            thread->_thread = std::make_unique<std::thread>(std::bind(&Terrain::dispose, this, thread));

            _threads.push_back(thread);
        }
    }
}

void
Terrain::onDeactivate() except
{
    _chunks.clear();
    _itmes.clear();

    if (!_threads.empty())
    {
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
}

void
Terrain::onFrame() except
{
    this->deleteChunks();
    this->checkChunks();

    if (_chunks.size() < _maxChunks)
        this->createChunks();

    this->hitChunks();
}

void
Terrain::addItem(TerrainItemPtr item) noexcept
{
    if (_itmes.size() + 1 < _maxInstances)
    {
        _itmes.push_back(item);
        item->setInstance(_itmes.size());
    }
}

void
Terrain::removeItem(TerrainItemPtr item) noexcept
{
}

void
Terrain::addObject(TerrainObjectPtr object) noexcept
{
    _objects.push_back(object);
    for (auto& it : object->getItems())
    {
        this->addItem(it);
    }
}

void
Terrain::removeObject(TerrainObjectPtr object) noexcept
{
}

TerrainObjects&
Terrain::getObjects() noexcept
{
    return _objects;
}

TerrainChunkPtr
Terrain::findChunk(int x, int y, int z) noexcept
{
    for (auto& it : _chunks)
    {
        if (it->distance(x, y, z) == 0)
        {
            return it;
        }
    }

    return nullptr;
}

void
Terrain::deleteChunks() noexcept
{
    auto it = _chunks.begin();
    auto end = _chunks.end();

    for (; it != end; ++it)
    {
        bool destroy = true;

        ray::Vector3 translate = _player->getTranslate();

        int x = chunked(translate.x);
        int y = chunked(translate.y);
        int z = chunked(translate.z);

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
Terrain::createChunks() noexcept
{
    auto& translate = _player->getTranslate();
    int x = chunked(translate.x);
    int y = 0;
    int z = chunked(translate.z);

    ray::Frustum fru;
    fru.extract(_player->getComponent<ray::CameraComponent>()->getViewProject());

    for (auto& it : _threads)
    {
        if (it->state != TerrainThread::IDLE)
            continue;

        int bestX = 0;
        int bestY = 0;
        int bestZ = 0;
        int start = std::numeric_limits<int>::max();
        int bestScore = start;

        for (int iq = -_createRadius; iq <= _createRadius; iq++)
        {
            for (int ip = -_createRadius; ip <= _createRadius; ip++)
            {
                int dx = x + iq;
                int dy = y;
                int dz = z + ip;

                auto chunk = this->findChunk(dx, dy, dz);
                if (chunk && !chunk->dirt())
                    continue;

                int invisiable = !this->visiable(fru, dx, dy, dz);
                int distance = std::max(std::abs(iq), std::abs(ip));
                int score = (invisiable << 24) | distance;
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

        auto chunk = this->findChunk(bestX, bestY, bestZ);
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
Terrain::checkChunks() noexcept
{
    for (auto& it : _threads)
    {
        if (it->state != TerrainThread::DONE)
            continue;

        if (it->chunk)
        {
            it->chunk->active(this->getGameObject());
            it->chunk = nullptr;
        }

        it->state = TerrainThread::IDLE;
    }
}

void
Terrain::hitChunks() noexcept
{
    auto inputFeatures = this->getGameServer()->getFeature<ray::InputFeatures>();
    if (inputFeatures)
    {
        auto input = inputFeatures->getInput();
        if (input && input->getButtonDown(ray::InputButton::MOUSE0))
        {
            auto player = find<ray::GameObject>("first_person_camera");
            auto translate = player->getTranslate();

            ray::Vector3 pos;
            pos.x = input->getMousePosX();
            pos.y = input->getMousePosY();
            pos.z = 1;

            auto world = player->getComponent<ray::CameraComponent>()->unproject(pos);
            auto view = world - player->getTranslate();
            view.normalize();

            ray::int3 out;
            auto chunk = hitTest(translate, view, out);
            if (chunk)
            {
                chunk->set(out.x, out.y, out.z, 0);
            }
        }
    }
}

TerrainChunkPtr
Terrain::hitTest(const ray::Vector3& translate, const ray::Vector3& view, ray::int3& out) noexcept
{
    int x = chunked(translate.x);
    int y = chunked(translate.y);
    int z = chunked(translate.z);

    int ox = x;
    int oy = y;
    int oz = z;

    float cur = 0;
    float step = view.length();

    ray::Vector3 pos;

    pos.x = (translate.x - unchunk(x)) * 0.5;
    pos.y = (translate.y - unchunk(y)) * 0.5;
    pos.z = (translate.z - unchunk(z)) * 0.5;

    auto chunk = this->findChunk(x, y, z);

    while (cur < _hitRadius)
    {
        if (x != ox || y != oy || z != oz)
            chunk = this->findChunk(ox, oy, oz);

        if (chunk)
        {
            int nx = roundf(pos.x);
            int ny = roundf(pos.y);
            int nz = roundf(pos.z);

            if (chunk->get(nx, ny, nz))
            {
                out.x = nx;
                out.y = ny;
                out.z = nz;
                return chunk;
            }

            pos.x += view.x;
            pos.y += view.y;
            pos.z += view.z;

            if (pos.x < 0) { pos.x = _size; ox--; }
            if (pos.y < 0) { pos.y = _size; oy--; }
            if (pos.z < 0) { pos.z = _size; oz--; }
            if (pos.x > _size) { pos.x = 0; ox++; }
            if (pos.y > _size) { pos.y = 0; oy++; }
            if (pos.z > _size) { pos.z = 0; oz++; }
        }

        cur += step;
    }

    return nullptr;
}

bool
Terrain::visiable(const ray::Frustum& fru, int x, int y, int z) noexcept
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
Terrain::dispose(std::shared_ptr<TerrainThread> ctx) noexcept
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

int
Terrain::chunked(float x) noexcept
{
    return  std::floorf(std::roundf(x) / (_size * 2));
}

float
Terrain::unchunk(int x) noexcept
{
    return x * _size * 2;
}

ray::GameComponentPtr
Terrain::clone() const noexcept
{
    return nullptr;
}