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
#ifndef _H_NOISE_H_
#define _H_NOISE_H_

#include <ray/mathfwd.h>

_NAME_BEGIN

template<typename T>
class PerlinNoise2t
{
public:
    enum table
    {
        size = 256,
        mask = size - 1
    };

    PerlinNoise2t()
    {
        setup();
    }

    //求得柏林噪声
    T noise(T x, T y, T scale)
    {
        Vector2t<T> pos(x * scale, y * scale);

        //找到此点的方格子
        int iX0 = int(pos.x);
        int iX1 = iX0 + 1;
        int iY0 = int(pos.y);
        int iY1 = iY0 + 1;

        T X0 = (T)iX0;
        T X1 = (T)iX1;
        T Y0 = (T)iY0;
        T Y1 = (T)iY1;

        //获得此方格4个顶点的梯度值
        const Vector2t<T>& v0 = getVec(iX0, iY0);
        const Vector2t<T>& v1 = getVec(iX0, iY1);
        const Vector2t<T>& v2 = getVec(iX1, iY0);
        const Vector2t<T>& v3 = getVec(iX1, iY1);

        Vector2t<T> d0(pos.x - X0, pos.y - Y0);
        Vector2t<T> d1(pos.x - X0, pos.y - Y1);
        Vector2t<T> d2(pos.x - X1, pos.y - Y0);
        Vector2t<T> d3(pos.x - X1, pos.y - Y1);

        //通过内积算出方格4个顶点的权重
        T h0 = (d0.x * v0.x) + (d0.y * v0.y);
        T h1 = (d1.x * v1.x) + (d1.y * v1.y);
        T h2 = (d2.x * v2.x) + (d2.y * v2.y);
        T h3 = (d3.x * v3.x) + (d3.y * v3.y);

        T Sx = d0.x, Sy = d0.y;

        //柏林插值平滑函数f(x) = 6*x^5 - 15*x^4 + 10*x^3
        //通过柏林方程式平滑此点

        Sx = (6 * powf(d0.x, 5.0f)) - (15 * powf(d0.x, 4.0f)) + (10 * powf(d0.x, 3.0f));

        Sy = (6 * powf(d0.y, 5.0f)) - (15 * powf(d0.y, 4.0f)) + (10 * powf(d0.y, 3.0f));

        //在下2顶点的x轴上对权重点Sx进行线性插值
        T avgX0 = lerp(h0, h2, Sx);

        //在上2顶点的x轴上对权重点Sx进行线性插值
        T avgX1 = lerp(h1, h3, Sx);

        //在y轴上合并2个插值
        T result = avgX0 + (Sy*(avgX1 - avgX0));

        return result;
    }

    T noise(const Vector2t<T> pos, T scale)
    {
        return noise(pos.x, pos.y, scale);
    }

private:
    //初始化伪随即表
    void setup()
    {
        T step = 6.24f / size;
        T val = 0.0f;

        for (int i = 0; i < size; i++)
        {
            table_[i].x = sin(val);
            table_[i].y = cos(val);

            val += step;

            lut_[i] = (std::uint8_t)(rand() & mask);
        }
    }

    //获得梯度值
    const Vector2t<T>& getVec(int x, int y) const
    {
        std::uint8_t a = lut_[x & mask];
        std::uint8_t b = lut_[y & mask];
        std::uint8_t val = lut_[(a + b)& mask];

        assert(val <= mask && val >= 0);

        return table_[val];
    }

    Vector2t<T> table_[size]; //梯度值表
    std::uint8_t lut_[size]; //伪随机索引表
};

void EXPORT simplex_seed(unsigned int x);

float EXPORT simplex2(float x, float y, int octaves, float persistence, float lacunarity);
float EXPORT simplex3(float x, float y, float z, int octaves, float persistence, float lacunarity);

_NAME_END

#endif