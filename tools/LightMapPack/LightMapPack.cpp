// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include "LightMapPack.h"

#include <sstream>

_NAME_BEGIN

LightMapItem::LightMapItem() noexcept
{
}

LightMapItem::~LightMapItem() noexcept
{
}

LightMapPack::LightMapPack() noexcept
{
}

LightMapPack::~LightMapPack() noexcept
{
}

LightMapNode*
LightMapPack::insertLightMapItem(LightMapNode* node, LightMapItem& item) noexcept
{
	if (node->left && node->right)
	{
		auto next = this->insertLightMapItem(node->left, item);
		return next ? next : insertLightMapItem(node->right, item);
	}
	else
	{
		if (item.edge.x > node->rect.z || item.edge.y > node->rect.w)
		{
			return nullptr;
		}

		if (item.edge.x == node->rect.z && item.edge.y == node->rect.w)
		{
			float2 offset(node->rect.x, node->rect.y);
			*item.p1 += offset;
			*item.p2 += offset;
			*item.p3 += offset;
			*item.p4 += offset;
			return node;
		}

		node->left = new LightMapNode;
		node->right = new LightMapNode;

		float dw = node->rect.z - item.edge.x;
		float dh = node->rect.w - item.edge.y;

		if (dw > dh)
		{
			node->left->rect = float4(node->rect.x, node->rect.y + item.edge.y, item.edge.x, node->rect.w - item.edge.y);
			node->right->rect = float4(node->rect.x + item.edge.x, node->rect.y, node->rect.z - item.edge.x, node->rect.w);
		}
		else
		{
			node->left->rect = float4(node->rect.x + item.edge.x, node->rect.y, node->rect.z - item.edge.x, item.edge.y);
			node->right->rect = float4(node->rect.x, node->rect.y + item.edge.y, node->rect.z, node->rect.w - item.edge.y);
		}
	}

	float2 offset(node->rect.x, node->rect.y);
	item.offset = offset;
	*(item.p1) += offset;
	*(item.p2) += offset;
	*(item.p3) += offset;
	*(item.p4) += offset;

	return node;
}

bool
LightMapPack::load(const std::string& path) noexcept
{
	if (_lightMapListener)
		_lightMapListener->onMessage("loading model : " + path);

	if (path.empty())
	{
		if (_lightMapListener)
			_lightMapListener->onMessage("The input path cannot be empty");

		return false;
	}

	ray::ifstream stream;
	if (!stream.open(path))
	{
		if (_lightMapListener)
			_lightMapListener->onMessage("Failed to open the path : " + path);

		return false;
	}

	ray::PMXHandler model;
	if (!model.doCanRead(stream))
	{
		if (_lightMapListener)
			_lightMapListener->onMessage("File is not a valid model with : " + path);

		return false;
	}

	_model = std::make_unique<PMX>();
	if (!model.doLoad(stream, *_model))
	{
		if (_lightMapListener)
			_lightMapListener->onMessage("Non readable PMX file : " + path);

		return false;
	}

	if (_lightMapListener)
		_lightMapListener->onMessage("loaded model : " + path);

	return true;
}

bool 
LightMapPack::save(const std::string& path) noexcept
{
	assert(_model);

	ofstream stream;
	if (!stream.open(path))
	{
		if (_lightMapListener)
			_lightMapListener->onMessage("Failed to open the path : " + path);
		return false;
	}

	ray::PMXHandler model;
	if (!model.doSave(stream, *_model))
		return false;

	return true;
}

bool
LightMapPack::pack() noexcept
{
	for (std::size_t i = 0; i < _model->numIndices; i += 3)
	{
		std::uint32_t f1 = getFace(i);
		std::uint32_t f2 = getFace(i + 1);
		std::uint32_t f3 = getFace(i + 2);

		const Vector3& v1 = _model->vertices[f1].position;
		const Vector3& v2 = _model->vertices[f2].position;
		const Vector3& v3 = _model->vertices[f3].position;

		Vector3 edge1 = v3 - v2;
		Vector3 edge2 = v1 - v2;

		Vector3 normal = math::normalize(math::cross(edge1, edge2));

		if (math::length2(normal) > 0)
		{
			face.vertex_index[0] = f1;
			face.vertex_index[1] = f2;
			face.vertex_index[2] = f3;
			face.material_index = 0;

			faces.push_back(face);
		}
		else
		{
			if (_lightMassListener)
			{
				std::ostringstream stream;
				stream << "WARNING : Input mesh has zero-length normals. check faces " << i / 3;
				_lightMassListener->onMessage(stream.str());
			}
		}
	}

	return false;
}

std::uint32_t
LightMapPack::getFace(std::size_t n) noexcept
{
	assert(_model);

	std::uint8_t* data = (std::uint8_t*)_model->indices.data();

	if (_model->header.sizeOfIndices == 1)
		return *(std::uint8_t*)(data + n * _model->header.sizeOfIndices);
	else if (_model->header.sizeOfIndices == 2)
		return *(std::uint16_t*)(data + n * _model->header.sizeOfIndices);
	else if (_model->header.sizeOfIndices == 4)
		return *(std::uint32_t*)(data + n * _model->header.sizeOfIndices);
	else
		return false;
}

std::uint32_t
LightMapPack::getFace(std::size_t n, std::uint32_t firstIndex) noexcept
{
	assert(_model);

	std::uint8_t* data = (std::uint8_t*)_model->indices.data() + firstIndex;

	if (_model->header.sizeOfIndices == 1)
		return *(std::uint8_t*)(data + n * _model->header.sizeOfIndices);
	else if (_model->header.sizeOfIndices == 2)
		return *(std::uint16_t*)(data + n * _model->header.sizeOfIndices);
	else if (_model->header.sizeOfIndices == 4)
		return *(std::uint32_t*)(data + n * _model->header.sizeOfIndices);
	else
		return false;
}

/*
bool
LightMass::pack(const LightMassParams& params) noexcept
{
	if (_lightMassListener)
		_lightMassListener->onUvmapperStart();

	std::vector<Thekla::Atlas_Input_Vertex> vertices;

	for (std::size_t i = 0; i < _model->numVertices; i++)
	{
		if (math::length2(_model->vertices[i].normal) == 0)
		{
			if (_lightMassListener)
			{
				std::ostringstream stream;
				stream << "WARNING : Input mesh has zero-length normals. check vectex " << i;
				_lightMassListener->onMessage(stream.str());
			}
		}

		Thekla::Atlas_Input_Vertex v;
		v.position[0] = _model->vertices[i].position.x;
		v.position[1] = _model->vertices[i].position.y;
		v.position[2] = _model->vertices[i].position.z;
		v.normal[0] = _model->vertices[i].normal.x;
		v.normal[1] = _model->vertices[i].normal.y;
		v.normal[2] = _model->vertices[i].normal.z;
		v.uv[0] = _model->vertices[i].coord.x;
		v.uv[1] = _model->vertices[i].coord.y;
		v.first_colocal = i;

		vertices.push_back(v);
	}

	std::vector<Thekla::Atlas_Input_Face> faces;
	for (std::size_t i = 0; i < _model->numIndices; i += 3)
	{
		Thekla::Atlas_Input_Face face;

		std::uint32_t f1 = getFace(i);
		std::uint32_t f2 = getFace(i + 1);
		std::uint32_t f3 = getFace(i + 2);

		const Vector3& a = _model->vertices[f1].position;
		const Vector3& b = _model->vertices[f2].position;
		const Vector3& c = _model->vertices[f3].position;

		Vector3 edge1 = c - b;
		Vector3 edge2 = a - b;

		Vector3 normal = math::normalize(math::cross(edge1, edge2));

		if (math::length2(normal) > 0)
		{
			face.vertex_index[0] = f1;
			face.vertex_index[1] = f2;
			face.vertex_index[2] = f3;
			face.material_index = 0;

			faces.push_back(face);
		}
		else
		{
			if (_lightMassListener)
			{
				std::ostringstream stream;
				stream << "WARNING : Input mesh has zero-length normals. check faces " << i / 3;
				_lightMassListener->onMessage(stream.str());
			}
		}
	}

	Thekla::Atlas_Input_Mesh input_mesh;
	input_mesh.vertex_count = vertices.size();
	input_mesh.vertex_array = vertices.data();
	input_mesh.face_count = faces.size();
	input_mesh.face_array = faces.data();

	Thekla::Atlas_Options atlas_options;
	atlas_set_default_options(&atlas_options);

	atlas_options.packer_options.witness.packing_quality = 0;
	atlas_options.packer_options.witness.block_align = true;
	atlas_options.packer_options.witness.conservative = true;
	atlas_options.packer_options.witness.texel_area = 1;

	Thekla::Atlas_Error error = Thekla::Atlas_Error_Success;
	Thekla::Atlas_Output_Mesh * output_mesh = atlas_generate(&input_mesh, &atlas_options, &error);
	if (!output_mesh)
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Failed to generate light map.");

		return false;
	}

	_model->header.addUVCount = 1;

	for (std::size_t i = 0; i < output_mesh->vertex_count; i++)
	{
		std::uint32_t n = output_mesh->vertex_array[i].xref;
		_model->vertices[n].addCoord[0].x = output_mesh->vertex_array[i].uv[0] / output_mesh->atlas_width;
		_model->vertices[n].addCoord[0].y = output_mesh->vertex_array[i].uv[1] / output_mesh->atlas_height;
	}

	if (_lightMassListener)
		_lightMassListener->onUvmapperEnd();

	return true;
}*/

_NAME_END