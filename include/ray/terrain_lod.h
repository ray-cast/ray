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
#ifndef _H_LOD_H_
#define _H_LOD_H_

#include <vector>

#include <ray/terrain_height_map.h>
#include <ray/downcast.h>

_NAME_BEGIN

struct Quadnode
{
    unsigned int x;
    unsigned int y;
    Quadnode() {}
    Quadnode(unsigned int xx, unsigned int yy) :x(xx), y(yy) {}
};

template<typename T>
class Lod : public down_cast<T>
{
public:
    Lod() noexcept
        : _max_depth(0)
        , _min_depth(0)
    {
    }

    void setMap(HeightMap* map) noexcept
    {
        assert(map);

        _map = map;

        _size = _map->getSize();

        _max_depth = (std::uint8_t)bitScanReverse(_size - 1);

        _scale = (std::size_t)_map->getCellSize();
    }

    inline void setFrustum(const Frustum* fru) { _frustum = fru; }

    inline float getHeight(std::uint32_t x, std::uint32_t y) const { return _map->getHeight(x, y); }
    inline Vector3 getVertex(std::uint32_t x, std::uint32_t y) const { return _map->getVertex(x, y); }
    inline std::size_t getScale() const { return _scale; }
    inline std::uint32_t getSize()  const { return _size; }
    inline std::uint8_t getDepth() const { return _max_depth; }

    inline bool isVisible(const Quadnode& node, std::uint8_t depth) const
    {
        assert(_frustum);
        assert(0 <= node.x && node.x < _size);
        assert(0 <= node.y && node.y < _size);

        std::intptr_t size = 1 << depth;

        float R = size * _scale * 1.414f; //Ð±±ß

        return _frustum->contains(_map->getVertex(node.x, node.y), R);
    }

    inline void tessellate()
    {
        cur_Nodes.push_back(root_);

        for (std::uint8_t i = _max_depth; i > _max_depth; i--)
        {
            std::size_t size = cur_Nodes.size();

            for (std::size_t j = 0; j < size; j++)
            {
                Quadnode node = cur_Nodes[j];

                if (down_cast<T>::downcast()->split(node, i))
                {
                    int d = 1 << (i - 2);

                    Quadnode child;

                    child.x = node.x + d;
                    child.y = node.y + d;
                    next_Nodes.push_back(child);

                    child.x = node.x - d;
                    child.y = node.y + d;
                    next_Nodes.push_back(child);

                    child.x = node.x + d;
                    child.y = node.y - d;
                    next_Nodes.push_back(child);

                    child.x = node.x - d;
                    child.y = node.y - d;
                    next_Nodes.push_back(child);
                }
            }

            cur_Nodes.swap(next_Nodes);
            next_Nodes.clear();
        }
    }

    void render()
    {
    }

private:
    std::uint8_t _min_depth;
    std::uint8_t _max_depth;

    std::uint32_t _size;
    std::uint32_t _scale;

    HeightMap* _map;
    Quadnode   root_;
    std::vector<Quadnode> cur_Nodes;
    std::vector<Quadnode> next_Nodes;

    Frustum* _frustum;
};

_NAME_END

#endif