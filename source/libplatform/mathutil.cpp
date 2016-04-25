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
#include <ray/math.h>
#include <time.h>

_NAME_BEGIN

namespace math
{
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
		srand(time(NULL));
	}

	void randomize(unsigned int seed)
	{
		srand(seed);
	}

	int random(int min, int max)
	{
		int seed = rand();
		return min + (seed ^ seed >> 15) % (max - min + 1);
	}

	float random(float min, float max)
	{
		int seed = rand();
		return min + (max - min) * (1.0f / RAND_MAX) * seed;
	}

	double random(double min, double max)
	{
		int seed = rand();
		return min + (max - min) * (1.0f / RAND_MAX) * seed;
	}

	std::uint32_t part1By1(std::uint32_t n)
	{
		n = (n ^ (n << 8)) & 0x00ff00ff;
		n = (n ^ (n << 4)) & 0x0f0f0f0f;
		n = (n ^ (n << 2)) & 0x33333333;
		n = (n ^ (n << 1)) & 0x55555555;
		return n;
	}

	std::uint32_t part1By2(std::uint32_t n)
	{
		n = (n ^ (n << 16)) & 0xff0000ff;
		n = (n ^ (n << 8)) & 0x0300f00f;
		n = (n ^ (n << 4)) & 0x030c30c3;
		n = (n ^ (n << 2)) & 0x09249249;
		return n;
	}

	std::uint32_t morton2(std::uint32_t x, std::uint32_t y)
	{
		return (part1By1(y) << 1) + part1By1(x);
	}

	std::uint32_t morton3(std::uint32_t x, std::uint32_t y, std::uint32_t z)
	{
		return (part1By2(z) << 2) + (part1By2(y) << 1) + part1By2(x);
	}

	bool isConvex(const float3 pt[], std::size_t n)
	{
		float angleSum = 0.0f;

		for (std::size_t i = 0; i < n; i++)
		{
			Vector3t<float> e1;
			if (i == 0)
			{
				e1 = pt[n - 1] - pt[i];
			}
			else
			{
				e1 = pt[i - 1] - pt[i];
			}

			Vector3t<float> e2;
			if (i == n - 1)
			{
				e2 = pt[0] - pt[i];
			}
			else
			{
				e2 = pt[i + 1] - pt[i];
			}

			e1 = normalize(e1);
			e2 = normalize(e2);

			float d = dot(e1, e2);

			float theta = safeAcos(d);

			angleSum += theta;
		}

		float convexAngleSum = (float)(n - 2) * M_PI;

		if (angleSum < convexAngleSum - (float)n * 0.0001f)
		{
			return false;
		}

		return true;
	}
}

_NAME_END