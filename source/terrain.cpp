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
#include <ray/terrain.h>

_NAME_BEGIN

Terrain::Terrain() noexcept
    : _radiusRender(10)
    , _radiusDestroy(15)
    , _radiusCreate(15)
{
}

Terrain::~Terrain() noexcept
{
}

void
Terrain::addObServer(TerrainObserverPtr observer) noexcept
{
    auto it = std::find(_observer.begin(), _observer.end(), observer);
    if (it == _observer.end())
    {
        _observer.push_back(observer);
    }
}

void
Terrain::removeObServer(TerrainObserverPtr observer) noexcept
{
    auto it = std::find(_observer.begin(), _observer.end(), observer);
    if (it != _observer.end())
    {
        _observer.erase(it);
    }
}

TerrainChunkPtr
Terrain::find(int p, int q)
{
    for (auto& it : _chunks)
    {
        auto chunk = it;
        if (chunk->getX() == p == chunk->getY() == q)
        {
            return chunk;
        }
    }

    return nullptr;
}

int
Terrain::convChunked(float x)
{
    return floorf(roundf(x) / _chunkSize);
}

void
Terrain::createChunks()
{
}

void
Terrain::destroyChunks()
{
    auto chunk = _chunks.begin();
    auto chunkEnd = _chunks.end();

    std::vector<ChunkIteraotr> destroyList;

    for (; chunk != chunkEnd; ++chunk)
    {
        bool destroy = true;

        for (auto& it : _observer)
        {
            auto pos = it->getPosition();

            int p = convChunked(pos.x);
            int q = convChunked(pos.z);

            if ((*chunk)->distance(p, q) < _radiusDestroy)
            {
                destroy = false;
            }
        }

        if (destroy)
        {
            destroyList.push_back(chunk);
        }
    }

    for (auto& it : destroyList)
    {
        _chunks.erase(it);
    }
}

void
Terrain::computeVisiable(TerrainObserverPtr observer, std::vector<TerrainChunkPtr>& visiable) noexcept
{
    assert(observer);

    Matrix4x4 view;
    view.makeLookAt(observer->getPosition(), observer->getLookat(), observer->getUpVector());

    Frustum fru;
    fru.extract(view);

    for (auto& it : _chunks)
    {
        auto pos = observer->getPosition();

        int p = convChunked(pos.x);
        int q = convChunked(pos.z);

        if (it->distance(p, q) > _radiusRender)
        {
            continue;
        }

        if (it->visiable(fru, _chunkSize))
        {
            visiable.push_back(it);
        }
    }
}

_NAME_END