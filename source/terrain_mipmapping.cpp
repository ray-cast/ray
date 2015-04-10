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
#include <ray/terrain_mipmapping.h>

#include <iostream>

_NAME_BEGIN

Mipmapping::Mipmapping()
    : factor_(0.001f)
    , _size(0)
{
}

Mipmapping::~Mipmapping()
{
}

bool
Mipmapping::init(HeightMap* T)
{
    setMap(T);

    std::uint32_t size = getSize();

    if (!bits_.create(size, size))
    {
        return false;
    }

    pVariant_.resize(size, size);

    std::uint32_t x = size >> 1;
    std::uint32_t y = size >> 1;

    initVariant(getDepth(), x, y);

    return true;
}

float
Mipmapping::initVariant(std::uint8_t level, std::uint32_t x, std::uint32_t y)
{
    assert(0 != x && 0 != y);
    assert(x < getSize() && y < getSize());

    float var[10] = { 0 };

    int iter = 0;

    if (level > 1)
    {
        int size = 1 << (level - 2);

        var[iter++] = initVariant(level - 1, x + size, y + size);
        var[iter++] = initVariant(level - 1, x - size, y + size);
        var[iter++] = initVariant(level - 1, x - size, y - size);
        var[iter++] = initVariant(level - 1, x + size, y - size);
    }

    int size = 1 << (level - 1);

    float lu = getHeight(x - size, y + size);
    float ld = getHeight(x - size, y - size);
    float ru = getHeight(x + size, y + size);
    float rd = getHeight(x + size, y - size);

    float left = (lu + ld) * 0.5f;
    float right = (ru + rd) * 0.5f;
    float up = (ru + lu) * 0.5f;
    float down = (rd + ld) * 0.5f;
    float center = (lu + rd) * 0.5f;

    var[iter++] = abs(getHeight(x - size, y) - left);
    var[iter++] = abs(getHeight(x + size, y) - right);
    var[iter++] = abs(getHeight(x, y + size) - up);
    var[iter++] = abs(getHeight(x, y - size) - down);
    var[iter++] = abs(getHeight(x, y) - center);

    float max = var[0];
    for (int i = 0; i < 10; i++)
    {
        if (max < var[i])
        {
            max = var[i];
        }
    }

    pVariant_.resize((std::size_t)max);

    return max;
}

bool
Mipmapping::isDivide(const Quadnode& node, std::uint8_t depth)
{
    assert(0 < node.x && node.x < getSize());
    assert(0 < node.y && node.y < getSize());

    Vector3f vecl = getVertex(node.x, node.y);// -getPosition();

    bool Divid = depth > 1;

    unsigned int x = node.x;
    unsigned int y = node.y;
    unsigned int size = 1 << depth;

    float l = vecl.length2();
    float d = (float)size * getScale();
    std::uint32_t r = pVariant_[y * _size + x];

    if (d * r / l < factor_)
    {
        Divid = false;
    }

    std::uint32_t trans = y - size;
    if (trans != 0 && !bits_.isTrue(x, trans))
    {
        _adjct_act[TOP] = 0;
        Divid = false;
    }
    else
    {
        _adjct_act[TOP] = 1;
    }

    trans = y + size;
    if (trans < getSize() && !bits_.isTrue(x, trans))
    {
        _adjct_act[BOTTOM] = 0;
        Divid = false;
    }
    else
    {
        _adjct_act[BOTTOM] = 1;
    }

    trans = x - size;
    if (trans != 0 && !bits_.isTrue(trans, y))
    {
        _adjct_act[LEFT] = 0;
        Divid = false;
    }
    else
    {
        _adjct_act[LEFT] = 1;
    }

    trans = x + size;
    if (trans < getSize() && !bits_.isTrue(trans, y))
    {
        _adjct_act[RIGHT] = 0;
        Divid = false;
    }
    else
    {
        _adjct_act[RIGHT] = 1;
    }

    return Divid;
}

void
Mipmapping::renderNode(const Quadnode& node, std::uint8_t level)
{
    assert(0 != node.x && node.x < getSize());
    assert(0 != node.y && node.y < getSize());

    std::uint32_t x = node.x;
    std::uint32_t y = node.y;

    std::uint32_t i = 0;
    std::uint32_t pt[10];

    std::uint32_t lenght = getSize();
    std::uint32_t size = 1 << (level - 1);

    pt[i++] = lenght * y + x;

    pt[i++] = lenght * (y - size) + x - size;

    if (_adjct_act[TOP])
        pt[i++] = lenght * (y - size) + x;

    pt[i++] = lenght * (y - size) + x + size;

    if (_adjct_act[RIGHT])
        pt[i++] = lenght * y + x + size;

    pt[i++] = lenght * (y + size) + x + size;

    if (_adjct_act[BOTTOM])
        pt[i++] = lenght * (y + size) + x;

    pt[i++] = lenght * (y + size) + x - size;

    if (_adjct_act[LEFT])
        pt[i++] = lenght * y + x - size;

    pt[i++] = lenght * (y - size) + x - size;

    _build_trianlges += (i - 2);
    /*
    drawElement_.setCount(i);
    drawElement_.setIndex(pt);
    drawElement_.apply();*/
}

void
Mipmapping::dividNode(const Quadnode& node, std::uint8_t level)
{
    bits_.set(node.x, node.y, true);

    if (level < 2) return;

    unsigned int size = 1 << (level - 2);

    bits_.set(node.x + size, node.y + size, true);
    bits_.set(node.x - size, node.y - size, true);
    bits_.set(node.x + size, node.y - size, true);
    bits_.set(node.x - size, node.y + size, true);
}

void
Mipmapping::disableNode(const Quadnode& node, std::uint8_t level)
{
    bits_.set(node.x, node.y, true);

    if (level < 2) return;

    unsigned int size = 1 << (level - 2);

    bits_.set(node.x + size, node.y + size, false);
    bits_.set(node.x - size, node.y - size, false);
    bits_.set(node.x + size, node.y - size, false);
    bits_.set(node.x - size, node.y + size, false);
}

bool
Mipmapping::split(const Quadnode& node, std::uint8_t depth)
{
    if (!isVisible(node, depth))
    {
        dividNode(node, depth);
        return false;
    }

    if (isDivide(node, depth))
    {
        dividNode(node, depth);
        return true;
    }

    disableNode(node, depth);
    renderNode(node, depth);

    return false;
}

void
Mipmapping::render()
{
    _build_trianlges = 0;

    tessellate();
}

_NAME_END