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
#ifndef _H_TERRAIN_H_
#define _H_TERRAIN_H_

#include "terrain_chunk.h"
#include "terrain_item.h"

class TerrainThread
{
public:
    TerrainThread()
        : state(IDLE)
        , isQuitRequest(false)
    {
    }

    bool isQuitRequest;

    TerrainChunkPtr chunk;

    enum State
    {
        QUIT,
        IDLE,
        BUSY,
        DONE,
    };

    std::atomic<State> state;

    std::mutex mutex;
    std::condition_variable_any dispose;
    std::unique_ptr<std::thread> _thread;

private:
    TerrainThread(const TerrainThread&) = delete;
    TerrainThread& operator=(TerrainThread&) = delete;
};

class Terrain : public ray::GameController
{
public:
    Terrain() noexcept;
    ~Terrain() noexcept;

    void addItem(TerrainItemPtr item) noexcept;
    void removeItem(TerrainItemPtr item) noexcept;

    bool addBlockByMousePos(int x, int y) noexcept;
    bool removeBlockByMousePos(int x, int y) noexcept;

    void addObject(TerrainObjectPtr object) noexcept;
    void removeObject(TerrainObjectPtr object) noexcept;
    TerrainObjects& getObjects() noexcept;

    ray::GameComponentPtr clone() const noexcept;

private:

    void deleteChunks() noexcept;
    void createChunks() noexcept;
    void checkChunks() noexcept;
    void hitChunks() noexcept;

    TerrainChunkPtr findChunk(int x, int y, int z) noexcept;
    bool visiable(const ray::Frustum& fru, int x, int y, int z) noexcept;
    int chunked(float x) noexcept;
    float unchunk(int x) noexcept;

    void dispose(std::shared_ptr<TerrainThread> ctx) noexcept;

    TerrainChunkPtr hitTest(const ray::Vector3& translate, const ray::Vector3& view, ray::int3& out) noexcept;

    void onActivate() except;
    void onDeactivate() except;

    void onFrame() except;

private:

    ray::GameObjectPtr _player;

    std::size_t _maxInstances;

    float _dayTimer;

    int _size;
    int _scale;

    int _createRadius;
    int _renderRadius;
    int _deleteRadius;
    int _signRadius;

    float _hitRadius;

    std::size_t _maxChunks;

    std::vector<TerrainItemPtr> _itmes;
    std::vector<TerrainObjectPtr> _objects;
    std::vector<TerrainChunkPtr> _chunks;

    std::vector<std::shared_ptr<TerrainThread>> _threads;
};

#endif