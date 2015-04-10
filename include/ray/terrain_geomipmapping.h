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
#ifndef _H_GEOMIPMAPPING_H_
#define _H_GEOMIPMAPPING_H_

#include <ray/terrain_height_map.h>

_NAME_BEGIN

class Geomipmapping
{
public:
    Geomipmapping();

    enum 
    {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
    };

    enum
    {
        MIN_DEPTH = 5,
        LIMIT = 48000,
        SCALE = 5
    };

    struct Quadnode
    {
        std::uint32_t x;
        std::uint32_t y;

        inline Quadnode() {}
        inline Quadnode(std::uint32_t xx, std::uint32_t yy) :x(xx), y(yy) {}
    };

    //初始化地形层次细节
    bool init(HeightMap* T);

    //计算地形的LOD
    void render(Matrix4x4& viewproj);

    std::size_t BuildTrianlges() { return buildTrianlges_; }

    void setFactor(float f) {factor_ = f;}

private:
    void  renderTree(const Quadnode& node, std::uint8_t level);
    bool  isVisible(const Quadnode& node, std::uint8_t level) const;
    bool  computeEdge(const Quadnode& node, std::uint8_t level, std::uint8_t detail);
    void  setDetail(std::uint32_t x, std::uint32_t y, std::uint8_t Detail);
    void  computeVisible();
    std::uint8_t getDetail(std::uint32_t x, std::uint32_t y);
    std::uint8_t evaluation(const Quadnode& node) const;

    std::uint8_t   _level;    //分割层数
    std::uint8_t*  _detail;

    std::uint32_t    size_;
    std::uint32_t    detailSize_;


    std::size_t    buildTrianlges_;

    bool            adjctAct_[4];
    float           factor_;
    float           cellSize_;
    HeightMap*      pMap_;
    Quadnode        root_;
    std::vector<Quadnode> cur_Nodes;
    std::vector<Quadnode> next_Nodes;
    Frustum  _frustum;
};

_NAME_END

#endif