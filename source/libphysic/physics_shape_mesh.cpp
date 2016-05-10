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
#include <ray/physics_shape_mesh.h>
#include "bullet_types.h"

_NAME_BEGIN

PhysicsShapeMesh::PhysicsShapeMesh() noexcept
	: _shape(nullptr)
	, _array(nullptr)
{
}

PhysicsShapeMesh::~PhysicsShapeMesh() noexcept
{
	this->close();
}

void
PhysicsShapeMesh::setup(const Vector3Array& vertices, const UintArray& face, const AABB& aabb) noexcept
{
	if (!_shape)
	{
		this->addMesh(vertices, face);

		btVector3 min;
		min.setX(aabb.min.x);
		min.setY(aabb.min.y);
		min.setZ(aabb.min.z);

		btVector3 max;
		max.setX(aabb.max.x);
		max.setY(aabb.max.y);
		max.setZ(aabb.max.z);

		_shape = new btBvhTriangleMeshShape(_array, true, min, max);
		_shape->setUserPointer(this);
	}
}

void
PhysicsShapeMesh::close() noexcept
{
	if (_array)
	{
		delete _array;
		_array = nullptr;
	}

	if (_shape)
	{
		delete _shape;
		_shape = nullptr;
	}
}

void
PhysicsShapeMesh::addMesh(const Vector3Array& vertices, const UintArray& face) noexcept
{
	btIndexedMesh mesh;
	mesh.m_vertexType = PHY_FLOAT;
	mesh.m_numVertices = vertices.size();
	mesh.m_vertexStride = sizeof(Vector3);
	mesh.m_vertexBase = (unsigned char*)vertices.data();
	mesh.m_indexType = PHY_INTEGER;
	mesh.m_numTriangles = face.size() / 3;
	mesh.m_triangleIndexStride = sizeof(std::uint32_t) * 3;
	mesh.m_triangleIndexBase = (unsigned char*)face.data();

	if (face.empty())
	{
		auto array = new btTriangleMesh;

		for (int i = 0; i < mesh.m_numVertices; i += 3)
		{
			btVector3 v1;
			v1.setX(vertices[i].x);
			v1.setY(vertices[i].y);
			v1.setZ(vertices[i].z);

			btVector3 v2;
			v2.setX(vertices[i + 1].x);
			v2.setY(vertices[i + 1].y);
			v2.setZ(vertices[i + 1].z);

			btVector3 v3;
			v3.setX(vertices[i + 2].x);
			v3.setY(vertices[i + 2].y);
			v3.setZ(vertices[i + 2].z);

			array->addTriangle(v1, v2, v3);
		}

		_array = array;
	}
	else
	{
		auto array = new btTriangleIndexVertexArray;
		array->addIndexedMesh(mesh);

		_array = array;
	}
}

btCollisionShape*
PhysicsShapeMesh::getCollisionShape() noexcept
{
	return _shape;
}

_NAME_END