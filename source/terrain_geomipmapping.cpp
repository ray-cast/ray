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
#include <ray/terrain_geomipmapping.h>

_NAME_BEGIN

Geomipmapping::Geomipmapping()
    :_detail(0)
    ,pMap_(0)
    ,factor_(0.001f)
{
}

bool
Geomipmapping::init(HeightMap* T)
{
    pMap_ = T;

    size_ = pMap_->getSize();

    _level = bitScanReverse(size_ - 1);

    detailSize_ = size_ >> MIN_DEPTH;

    _detail = new unsigned char[detailSize_ * detailSize_];

    if (!_detail)
    {
        return false;
    }

    memset(_detail, 0, detailSize_ * detailSize_);

    int x = size_ >> 1;
    int y = size_ >> 1;

    float cellX = pMap_->getVertex(1, 0).x - pMap_->getVertex(0, 0).x;
    float cellY = pMap_->getVertex(0, 1).z - pMap_->getVertex(0, 0).z;

    cellX > cellY ? cellSize_ = cellX : cellSize_ = cellY;

    root_ = Quadnode(x, y);
    
    return true;
}

bool
Geomipmapping::isVisible(const Quadnode& node, std::uint8_t level) const
{
    assert(node.x > 0 && node.x < size_);
    assert(node.y > 0 && node.y < size_);

    unsigned int size = 1 <<level;

    float R = size * cellSize_ * 1.414f;

    return _frustum.contains(pMap_->getVertex(node.x, node.y), R);
}

void
Geomipmapping::setDetail(std::uint32_t x, std::uint32_t y, std::uint8_t Detail)
{
    assert(x > 0 && x < size_);
    assert(y > 0 && y < size_);

    unsigned int xx = x >> MIN_DEPTH;
    unsigned int yy = y >> MIN_DEPTH;

    _detail[xx + yy * detailSize_] = Detail;
}

unsigned char
Geomipmapping::getDetail(unsigned int x, unsigned int y)
{
    assert(x > 0 && x < size_);
    assert(y > 0 && y < size_);

    unsigned int xx = x >> MIN_DEPTH;
    unsigned int yy = y >> MIN_DEPTH;

    return _detail[xx + yy * detailSize_];
}

std::uint8_t
Geomipmapping::evaluation(const Quadnode& node) const
{
    assert(node.x > 0 && node.x < size_);
    assert(node.y > 0 && node.y < size_);

    Vector3f vecl = pMap_->getVertex(node.x, node.y) - _frustum.getPosition();
    int l = (int)vecl.dot();

    std::uint8_t depth = MIN_DEPTH;

    for (std::uint8_t i = 1; i < MIN_DEPTH; i++)
    {
        double limit = LIMIT * pow((size_t)SCALE, i);
        if (l < limit)
        {
            depth = i;
            break;
        }
    }

    return depth;
}

bool 
Geomipmapping::computeEdge(const Quadnode& node, std::uint8_t level, std::uint8_t detail)
{
    unsigned int step = 2 <<(level - 1); //四周节点的距离

    bool Divid = true;

    std::uint32_t trans = node.y - step;
    if (trans >= 0 && getDetail(node.x, trans) > detail)
    {
        adjctAct_[TOP] = false;
    }
    else
    {
        adjctAct_[TOP] = true;    
    }

    trans = node.y + step;
    if (trans <= pMap_->getSize() && getDetail(node.x, trans) > detail)
    {
        adjctAct_[BOTTOM] = false;
    }
    else
    {
        adjctAct_[BOTTOM] = true;     
    }

    trans = node.x + step;
    if (trans <= pMap_->getSize() && getDetail(trans, node.y) > detail)
    {
        adjctAct_[RIGHT] = false;
    }
    else
    {
        adjctAct_[RIGHT] = true;    
    }

    trans = node.x - step;
    if (trans >= 0 && getDetail(trans, node.y) > detail)
    {
        adjctAct_[LEFT] = false;
    }
    else
    {
        adjctAct_[LEFT] = true;
    }

     return Divid;
}

void
Geomipmapping::renderTree(const Quadnode& node, std::uint8_t level)
{
    std::uint8_t depth = getDetail(node.x, node.y);
    std::uint32_t lenght = size_;
    std::uint32_t size = 1 << (level - 1);
    std::uint32_t count = (1 << (level - depth)) - 1;
    std::uint32_t step = 2 << (depth - 1);           //边缘的跨距
    std::uint32_t half = 1 << (depth - 1);           //边缘的距离

    Quadnode begin = Quadnode(node.x - size + half, node.y - size + half);
    Quadnode end   = Quadnode(begin.x + step * count, begin.y + step * count);

    computeEdge(node, level, depth);

    for (std::uint32_t y = begin.y; y <= end.y; y+=step)
    {
        for (std::uint32_t x = begin.x; x <= end.x; x += step)
        {
            std::uint32_t index = 0;
            std::uint32_t pt[10];

            pt[index++] = lenght * y + x;

            pt[index++] = lenght * (y - half) + x - half;

            if (y == begin.y)
            {
                if(adjctAct_[TOP])
                {
                    pt[index++] = lenght * (y - half) + x;
                }                    
            }
            else
            {
                pt[index++] = lenght * (y - half) + x;                
            }

            pt[index++] = lenght * (y - half) + x + half;

            if(x == end.x)
            {
                if(adjctAct_[RIGHT])
                {
                    pt[index++] = lenght * y + x + half;
                }
            }
            else
            {
                pt[index++] = lenght * y + x + half;
            }   

            pt[index++] = lenght * (y + half) + x + half;

            if(y == end.y)
            {
                if(adjctAct_[BOTTOM])
                {
                    pt[index++] = lenght * (y + half) + x;
                }
            }
            else
            {
                pt[index++] = lenght * (y + half) + x;
            }

            pt[index++] = lenght * (y + half) + x - half;

            if(x == begin.x)
            {
                if(adjctAct_[LEFT])
                {
                    pt[index++] = lenght * y + x - half;
                }
            }
            else
            {
                pt[index++] = lenght * y + x - half;
            }

            pt[index++] = lenght * (y - half) + x - half;

            buildTrianlges_ += (index - 2);

            /*drawElement_.setCount(index);
            drawElement_.setIndex(pt);
            drawElement_.apply();*/
        }
    }
}

void
Geomipmapping::computeVisible()
{
    cur_Nodes.push_back(root_);

    for (std::uint8_t i = _level; i > MIN_DEPTH; i--)
    {
        size_t size = cur_Nodes.size();

        for (size_t j = 0; j <size; j++)
        {
            Quadnode node = cur_Nodes[j];

            if (!isVisible(node, i))
                continue;

            int d = 1 <<(i - 2);

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
        cur_Nodes.swap(next_Nodes);
        next_Nodes.clear();
    }
}

void
Geomipmapping::render(Matrix4x4& viewproj)
{
    _frustum.extractFromMatrix(viewproj);
    
    buildTrianlges_ = 0;

    computeVisible();

    std::vector<Quadnode>::iterator it = cur_Nodes.begin();
    std::vector<Quadnode>::iterator end = cur_Nodes.end();

    for (; it != end; ++it)
    {
        setDetail(it->x, it->y, evaluation(*it));
    }

    it = cur_Nodes.begin();
    for (; it != end; ++it)
    {
        renderTree(*it, MIN_DEPTH);
    }

    cur_Nodes.clear();
}

_NAME_END