// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include <ray/terrain_height_map.h>

_NAME_BEGIN

HeightMap::HeightMap()
{
}

HeightMap::HeightMap(int size, const AABB& world)
{
	init(size, world);
}

HeightMap::HeightMap(std::string szMapName, int size, const AABB& world)
{
	init(szMapName, size, world);
}

HeightMap::~HeightMap()
{
}

bool
HeightMap::init(int size, const AABB& world)
{
	assert((math::ceilToPowerOfTwo(size) == 0) || (math::ceilToPowerOfTwo(size) == 1));

	size_ = size;

	cellSize_ = world.size().x / size;

	world_ = world;

	computeVertexMap();
	computeNormalMap();

	return true;
}

bool
HeightMap::init(std::string szMapName, int size, const AABB& world)
{
	assert((math::ceilToPowerOfTwo(size) == 0) || (math::ceilToPowerOfTwo(size) == 1));

	size_ = size;

	world_ = world;

	assert(szMapName.empty());

	return true;
}

void
HeightMap::computeVertexMap()
{
	assert(!_vertices.empty());

	PerlinNoise2 PL;

	float invfre = 0.013f;

	for (std::uint32_t y = 0; y < size_; y++)
	{
		for (std::uint32_t x = 0; x < size_; x++)
		{
			float accum = 0;

			accum += PL.noise(x * invfre, y*invfre, 0.8f) * 1.0f;
			accum += PL.noise(x * invfre + 0.3f, y * invfre + 0.3f, 1.0f) * 0.9f;
			accum += PL.noise(x * invfre, y*invfre, 3.0f) * 0.17f;
			accum += PL.noise(x * invfre, y*invfre, 3.4f) * 0.12f;
			accum += PL.noise(x * invfre, y*invfre, 3.7f) * 0.1f;

			accum *= 0.1f;
			accum += 0.5f;

			accum *= world_.size().y;

			accum += world_.min.y;

			_vertices.push_back(makePosition((float)x, accum, (float)y));
			_texcoord.push_back(getTexture(x, y));
		}
	}
}

void
HeightMap::computeNormalMap()
{
	assert(!_vertices.empty());

	Vector3f left;
	Vector3f right;
	Vector3f up;
	Vector3f down;

	for (std::size_t y = 0; y < size_; ++y)
	{
		for (std::size_t x = 0; x < size_; ++x)
		{
			Vector3f cur = this->getVertex(x, y);

			if (x > 0)
				left = this->getVertex(x - 1, y) - cur;
			if (x + 1 < size_)
				right = this->getVertex(x + 1, y) - cur;
			if (y + 1 < size_)
				up = this->getVertex(x, y + 1) - cur;
			if (y > 0)
				down = this->getVertex(x, y - 1) - cur;

			Vector3f lu = math::cross(left, up);
			Vector3f ru = math::cross(up, right);
			Vector3f rd = math::cross(right, down);
			Vector3f ld = math::cross(down, left);

			int average = 0;

			if (x > 0 && y > 0)
			{
				math::normalize(ld);
				average++;
			}

			if (x > 0 && y + 1 < size_)
			{
				math::normalize(lu);
				average++;
			}

			if (y > 0 && x + 1 < size_)
			{
				math::normalize(rd);
				average++;
			}

			if (x + 1 < size_ && y + 1 < size_)
			{
				math::normalize(ru);
				average++;
			}

			Vector3f cur_normal = math::normalize((lu + ru + ld + rd) / (float)average);

			_normals.push_back(cur_normal);
		}
	}
}

_NAME_END