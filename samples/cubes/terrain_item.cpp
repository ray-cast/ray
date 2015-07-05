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
#include "terrain_item.h"
#include "terrain_map.h"

void
TerrainItem::setInstance(ItemID instance) noexcept
{
	_instanceID = instance;
}

ItemID
TerrainItem::getInstance() const noexcept
{
	return _instanceID;
}

void
TerrainObject::addItem(TerrainItemPtr item) noexcept
{
	auto it = std::find(items.begin(), items.end(), item);
	if (it == items.end())
	{
		items.push_back(item);
	}
}

TerrainItems&
TerrainObject::getItems() noexcept
{
	return items;
}

bool
TerrainObject::visiable(TerrainMapPtr map, const MapEntry& it, VisiableFaces& faces) noexcept
{
	ItemID f1 = map->get(it.x - 1, it.y, it.z);
	ItemID f2 = map->get(it.x + 1, it.y, it.z);
	ItemID f3 = map->get(it.x, it.y - 1, it.z);
	ItemID f4 = map->get(it.x, it.y + 1, it.z);
	ItemID f5 = map->get(it.x, it.y, it.z - 1);
	ItemID f6 = map->get(it.x, it.y, it.z + 1);

	f1 = (f1 != it.instanceID) ? 1 : 0;
	f2 = (f2 != it.instanceID) ? 1 : 0;
	f3 = (f3 != it.instanceID) ? 1 : 0;
	f4 = (f4 != it.instanceID) ? 1 : 0;
	f5 = (f5 != it.instanceID) ? 1 : 0;
	f6 = (f6 != it.instanceID) ? 1 : 0;

	faces.left = f1;
	faces.right = f2;
	faces.bottom = f3;
	faces.top = f4;
	faces.front = f5;
	faces.back = f6;

	return (f1 + f2 + f3 + f4 + f5 + f6) ? true : false;
}

void
TerrainObject::makeCube(ray::MeshPropertyPtr data, const VisiableFaces& faces, float x, float y, float z, float n) noexcept
{
	static const float positions[6][4][3] = {
		{ { -1, -1, -1 },{ -1, -1, +1 },{ -1, +1, -1 },{ -1, +1, +1 } },
		{ { +1, -1, -1 },{ +1, -1, +1 },{ +1, +1, -1 },{ +1, +1, +1 } },
		{ { -1, +1, -1 },{ -1, +1, +1 },{ +1, +1, -1 },{ +1, +1, +1 } },
		{ { -1, -1, -1 },{ -1, -1, +1 },{ +1, -1, -1 },{ +1, -1, +1 } },
		{ { -1, -1, -1 },{ -1, +1, -1 },{ +1, -1, -1 },{ +1, +1, -1 } },
		{ { -1, -1, +1 },{ -1, +1, +1 },{ +1, -1, +1 },{ +1, +1, +1 } }
	};
	static const float normals[6][3] = {
		{ -1, 0, 0 },
		{ +1, 0, 0 },
		{ 0, +1, 0 },
		{ 0, -1, 0 },
		{ 0, 0, -1 },
		{ 0, 0, +1 }
	};
	static const float uvs[6][4][2] = {
		{ { 0, 0 },{ 1, 0 },{ 0, 1 },{ 1, 1 } },
		{ { 1, 0 },{ 0, 0 },{ 1, 1 },{ 0, 1 } },
		{ { 0, 1 },{ 0, 0 },{ 1, 1 },{ 1, 0 } },
		{ { 0, 0 },{ 0, 1 },{ 1, 0 },{ 1, 1 } },
		{ { 0, 0 },{ 0, 1 },{ 1, 0 },{ 1, 1 } },
		{ { 1, 0 },{ 1, 1 },{ 0, 0 },{ 0, 1 } }
	};
	static const float indices[6][6] = {
		{ 0, 3, 2, 0, 1, 3 },
		{ 0, 3, 1, 0, 2, 3 },
		{ 0, 3, 2, 0, 1, 3 },
		{ 0, 3, 1, 0, 2, 3 },
		{ 0, 3, 2, 0, 1, 3 },
		{ 0, 3, 1, 0, 2, 3 }
	};
	static const float flipped[6][6] = {
		{ 0, 1, 2, 1, 3, 2 },
		{ 0, 2, 1, 2, 3, 1 },
		{ 0, 1, 2, 1, 3, 2 },
		{ 0, 2, 1, 2, 3, 1 },
		{ 0, 1, 2, 1, 3, 2 },
		{ 0, 2, 1, 2, 3, 1 }
	};

	float s = 0.0625;
	float a = 0 + 1 / 2048.0;
	float b = s - 1 / 2048.0;
	int visiable[6] = { faces.left, faces.right, faces.top, faces.bottom, faces.front, faces.back };

	auto& vertex = data->getVertexArray();
	auto& normal = data->getNormalArray();
	auto& texcoord = data->getTexcoordArray();
	auto& face = data->getFaceArray();

	for (int i = 0; i < 6; i++) {
		if (visiable[i] == 0) {
			continue;
		}
		for (int j = 0; j < 6; j++) {
			int k = indices[i][j];

			ray::Vector3 v;
			v.x = x + n * positions[i][k][0];
			v.y = y + n * positions[i][k][1];
			v.z = z + n * positions[i][k][2];

			ray::Vector3 n;
			n.x = normals[i][0];
			n.y = normals[i][1];
			n.z = normals[i][2];

			ray::Vector2 uv;
			uv.x = (uvs[i][k][0] ? b : a);
			uv.y = (uvs[i][k][1] ? b : a);

			vertex.push_back(v);
			normal.push_back(n);
			texcoord.push_back(uv);
			face.push_back(vertex.size() - 1);
		}
	}
}