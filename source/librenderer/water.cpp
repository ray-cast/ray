// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
// | Author: Xiemoperor <Xiemoperor@gmail.com>
// +----------------------------------------------------------------------
#include <ray/water.h>
/*
using namespace ray;

Water::Water(StateMachine* state)
    :pMachine_(state)
    ,size_(200)
    ,pNormalTable_(0)
    ,pTexcoordTable_(0)
    ,pVertexTable_(0)
    ,position_(0.0f, 0.0f, 0.0f)
{
}

void
Water::rippleSpread()
{
    size_t width = size_;
    size_t height = size_;

    size_t count = width * height - width;

    for(size_t i = width; i <count; i++)
    {
        float sum = lastRipple_[i-1] +
                    lastRipple_[i+1] +
                    lastRipple_[i-width] +
                    lastRipple_[i+width];

        currentRipple_[i] = sum * 0.5f - currentRipple_[i];

        currentRipple_[i] -= currentRipple_[i] / 32.0f;
    }

    currentRipple_.swap(lastRipple_);
}

void
Water::dropStone(int x, int y, int stoneSize, float stoneWeight)
{
    if(((x + stoneSize) > size_) ||
       ((y + stoneSize) > size_) ||
       ((x - stoneSize) <0)     ||
       ((x - stoneSize) <0))
    {
       return;
    }

    for (int posx = x-stoneSize; posx <x+stoneSize; posx++)
    {
        for (int posy = y-stoneSize; posy <y+stoneSize; posy++)
        {
            int stoneArea = stoneSize * stoneSize; //* ¦Ð  // ¦Ð * r^2
            int dropArea = (posx-x)*(posx-x) + (posy-y)*(posy-y); //* ¦Ð // ¦Ð * r^2

            if (dropArea <stoneArea)
            {
                currentRipple_(posx, posy) = -stoneWeight;
            }
        }
    }
}

bool
Water::computeNormalMap()
{
    if (!pNormalTable_)
    {
        pNormalTable_ = new Vector3f[size_ * size_];
    }

    if (!pNormalTable_) { return false; }

    Vector3f *pTable = pNormalTable_;

    for (int y = 0; y <size_; y++)
    {
        for (int x = 0; x <size_; x++)
        {
            if (x == 0 ||
                y == 0 ||
                x == size_ - 1 ||
                y == size_ - 1)
            {
                if (x > 0 && x != size_-1)
                {
                    pTable->x = getPos(x+1,y).y - getPos(x-1,y).y;
                }
                else if(x == 0)
                {
                    pTable->x = getPos(x+1,y).y - getPos(x,y).y;
                }
                else
                {
                    pTable->x = getPos(x,y).y - getPos(x-1,y).y;
                }
                
                if (y > 0 && y != size_-1)
                {
                    pTable->z = getPos(x,y+1).y - getPos(x,y-1).y;
                }
                else if(y == 0)
                {
                    pTable->z = getPos(x,y+1).y - getPos(x,y).y;
                }
                else
                {
                    pTable->z = getPos(x,y).y - getPos(x,y-1).y;
                }

                pTable->y = -100;
                pTable->normalize();
                pTable++;
                continue;
            }

            Vector3f n;

            pTable->x = getPos(x+1,y).y - getPos(x-1,y).y;
            pTable->y = -100;
            pTable->z = getPos(x,y+1).y - getPos(x,y-1).y;
            pTable->normalize();
            pTable++;
        }
    }
    return true;
}

void
Water::init(const Vector3f& pos, int size)
{
    size_ = size;
    position_ = pos;
    currentRipple_.resize(size, size);
    lastRipple_.resize(size, size);

    computeTexcoordMap();

    dropStone(10, 10, 10, 2.0f);

    std::string filename = "data/";
    filename += "water9";
    filename += ".png";

    unsigned char *buffer = 0;
    unsigned int width = 0;
    unsigned int height = 0;
    if(!LoadPNG(filename, buffer, width, height))
    {
        assert(false);
    }

    texture_.setWrapMode(REPEAT);
    texture_.setFilterMode(LINEAR);
    texture_.createTexture(buffer, width, height, true);

    free(buffer);
}

bool
Water::computeTexcoordMap()
{
    assert(!pTexcoordTable_);

    pTexcoordTable_ = new Vector2f[size_ * size_];

    if (!pTexcoordTable_) { return false; }

    Vector2f *pTable = pTexcoordTable_;

    for (int y = 0; y <size_; y++)
    {
        for (int x = 0; x <size_; x++)
        {
            *pTable++ = getTexture(x, y);
        }
    }

    return true;
}

bool
Water::computeVertexMap()
{
    if (!pVertexTable_)
    {
        pVertexTable_ = new Vector3[size_ * size_];
    }

    if (!pVertexTable_) { return false; }

    Vector3 *pTable = pVertexTable_;

    for (int y = 0; y <size_; y++)
    {
        for (int x = 0; x <size_; x++)
        {
            *pTable++ = getPos(x, y);
        }
    }

    return true;
}

void 
Water::render(const Frustum& f)
{
    rippleSpread();

    computeNormalMap();
    computeVertexMap();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, pVertexTable_);
    glTexCoordPointer(2, GL_FLOAT, 0, pTexcoordTable_);
    glNormalPointer(GL_FLOAT, 0, pNormalTable_);

    size_t width  = size_;
    size_t height = size_;
    glPushMatrix();
    glTranslatef(-100, 0, -100);
        for(size_t i = 0; i <height - 1; i++)
        {
            size_t pt[200];
            size_t k = 0;
            for (size_t j = 0; j <width; j++)
            {
                pt[k++] = j + i * width;

                pt[k++] = j + (i+1) * width;
            }

            glDrawElements(GL_TRIANGLE_STRIP, width*2, UNSIGNED_INT, pt);
        }
    glPopMatrix();
}*/