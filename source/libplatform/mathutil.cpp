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
#include <ray/mathutil.h>

_NAME_BEGIN

unsigned int seed;

float cos_look[361];
float sin_look[361];
float tan_look[361];

float fastSin(float theta)
{
    theta = fmodf(theta, 360);
    if (theta < 0) theta += 360.0;
    int thet_aint = (int)theta;
    float thet_afrac = theta - thet_aint;

    return sin_look[thet_aint] + thet_afrac * (sin_look[thet_aint + 1] - sin_look[thet_aint]);
}

float fastCos(float theta)
{
    theta = fmodf(theta, 360);
    if (theta < 0) theta += 360.0;
    int thet_aint = (int)theta;
    float thet_afrac = theta - thet_aint;

    return cos_look[thet_aint] + thet_afrac * (cos_look[thet_aint + 1] - cos_look[thet_aint]);
}

void randomize()
{
    srand(seed);
}

int random(int min, int max)
{
    seed = 214013 * seed + 2531011;
    return    min + (seed ^ seed >> 15) % (max - min + 1);
}

float random(float min, float max)
{
    seed = 214013 * seed + 2531011;
    return min + (max - min) * (1.0f / 65535.0f) * (seed >> 16);
}

double random(double min, double max)
{
    seed = 214013 * seed + 2531011;
    return min + (max - min) * (1.0f / 65535.0f) * (seed >> 16);
}

_NAME_END