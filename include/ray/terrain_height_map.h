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
#ifndef _H_HEIGHT_MAP_H_
#define _H_HEIGHT_MAP_H_

#include <ray/math.h>

#include <vector>

_NAME_BEGIN

class HeightMap
{
public:
    struct Vertex
    {
        Vector3f v;
        Vector2f uv;
        Vector3f n;
    };

    HeightMap();
    HeightMap(int size, const AABB& box);
    HeightMap(std::string szMapName, int size, const AABB& box);
    ~HeightMap();

    bool init(int size, const AABB& box);
    bool init(std::string szMapName, int size, const AABB& box);

    const std::uint32_t getSize() const { return size_; }
    const float       getCellSize() const { return cellSize_; }

    const Vector3f makePosition(float x, float y, float z) const
    {
        assert(0 <= x && x < size_);
        assert(0 <= z && z < size_);

        float posX = world_.min.x + x * cellSize_;
        float posZ = world_.min.z + z * cellSize_;

        return Vector3f(posX, (float)y, posZ);
    }

    const Vector3f& getVertex(std::size_t x, std::size_t y) const
    {
        return _vertices[y * size_ + x];
    }

    Vector3f& getVertex(std::size_t x, std::size_t y)
    {
        return _vertices[y * size_ + x];;
    }

    float getHeight(std::size_t x, std::size_t y) const
    {
        return getVertex(x, y).y;
    }

    const Vector2f getTexture(std::size_t x, std::size_t y) const
    {
        assert(x < size_);
        assert(y < size_);

        float u = (float)x / size_;
        float v = (float)y / size_;

        return Vector2f(u, v);
    }

    std::size_t getVertexStride()   const { return sizeof(Vertex); }
    std::size_t getTexCoordStride() const { return sizeof(Vertex); }
    std::size_t getNormalStride()   const { return sizeof(Vertex); }

    std::size_t getVertexSize()     const { return size_ * size_ * sizeof(Vector3f); }
    std::size_t getTexCoordSize()   const { return size_ * size_ * sizeof(Vector2f); }
    std::size_t getNormalSize()     const { return size_ * size_ * sizeof(Vector3f); }

    const float* getVertexTable() const { return _vertices[0].ptr(); }
    const float* getNormalTable() const { return _normals[0].ptr(); }
    const float* getTexCoordTable() const { return _texcoord[0].ptr(); }

private:
    void computeVertexMap();
    void computeNormalMap();

    Vector3Array _vertices;
    Vector3Array _normals;
    Vector2Array _texcoord;

    std::uint32_t size_;

    float cellSize_;

    AABB world_;
};

_NAME_END

#endif