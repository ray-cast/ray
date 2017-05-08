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
#include "LightMass.h"

_NAME_BEGIN

LightMass::LightMass() noexcept
	: _lightMassListener(std::make_shared<LightMassListener>())
{
}

LightMass::~LightMass() noexcept
{
}

std::uint32_t
LightMass::getFace(std::size_t n) noexcept
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
LightMass::getFace(std::size_t n, std::uint32_t firstIndex) noexcept
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

void
LightMass::computeFaceNormals() noexcept
{
	assert(_model);

	_facesNormal.resize(_model->numIndices / 3);

	for (std::size_t i = 0; i < _model->numIndices; i += 3)
	{
		std::size_t f1 = getFace(i);
		std::size_t f2 = getFace(i + 1);
		std::size_t f3 = getFace(i + 2);

		const Vector3& a = _model->vertices[f1].position;
		const Vector3& b = _model->vertices[f2].position;
		const Vector3& c = _model->vertices[f3].position;

		Vector3 edge1 = c - b;
		Vector3 edge2 = a - b;

		Vector3 normal = math::normalize(math::cross(edge1, edge2));

		_facesNormal[i / 3] = normal;
	}
}

void
LightMass::computeVertricesNormals() noexcept
{
	assert(_model);

	for (size_t i = 0; i < _model->numVertices; i++)
	{
		_model->vertices[i].normal = ray::float3::Zero;
	}

	for (size_t i = 0; i < _model->numIndices; i += 3)
	{
		std::uint32_t a = getFace(i);
		std::uint32_t b = getFace(i + 1);
		std::uint32_t c = getFace(i + 2);

		_model->vertices[a].normal += _facesNormal[i / 3];
		_model->vertices[b].normal += _facesNormal[i / 3];
		_model->vertices[c].normal += _facesNormal[i / 3];
	}

	for (size_t i = 0; i < _model->numVertices; i++)
	{
		_model->vertices[i].normal = math::normalize(_model->vertices[i].normal);
	}
}

void
LightMass::computeBoundingBox(Bound& boundingBox, std::uint32_t firstFace, std::uint32_t faceCount) noexcept
{
	assert(_model);

	boundingBox.reset();

	for (size_t i = 0; i < faceCount; i++)
	{
		std::uint32_t face = this->getFace(i, firstFace);
		boundingBox.encapsulate(_model->vertices[face].position);
	}
}

void
LightMass::computeLightmapPack() noexcept
{
	assert(_model);

	this->computeFaceNormals();

	_model->header.addUVCount = 1;

	float2 minUV[3];
	float2 maxUV[3];

	minUV[0].set(FLT_MAX);
	minUV[1].set(FLT_MAX);
	minUV[2].set(FLT_MAX);
	maxUV[0].set(-FLT_MAX);
	maxUV[1].set(-FLT_MAX);
	maxUV[2].set(-FLT_MAX);

	for (size_t i = 0; i < _model->numIndices; i += 3)
	{
		std::uint32_t a = getFace(i);
		std::uint32_t b = getFace(i + 1);
		std::uint32_t c = getFace(i + 2);

		float3 polyNormal = math::abs(_facesNormal[i / 3]);

		float2 uv[3];

		int flag = 0;
		if (polyNormal.x > polyNormal.y && polyNormal.x > polyNormal.z)
		{
			flag = 1;
			uv[0] = _model->vertices[a].position.yz();
			uv[1] = _model->vertices[b].position.yz();
			uv[2] = _model->vertices[c].position.yz();
		}
		else if (polyNormal.y > polyNormal.x && polyNormal.y > polyNormal.z)
		{
			flag = 2;
			uv[0] = _model->vertices[a].position.xz();
			uv[1] = _model->vertices[b].position.xz();
			uv[2] = _model->vertices[c].position.xz();
		}
		else
		{
			uv[0] = _model->vertices[a].position.xy();
			uv[1] = _model->vertices[b].position.xy();
			uv[2] = _model->vertices[c].position.xy();
		}

		for (int j = 0; j < 3; j++)
		{
			minUV[flag] = math::min(minUV[flag], uv[j]);
			maxUV[flag] = math::max(maxUV[flag], uv[j]);
		}
	}

	for (size_t i = 0; i < _model->numIndices; i += 3)
	{
		std::uint32_t a = getFace(i);
		std::uint32_t b = getFace(i + 1);
		std::uint32_t c = getFace(i + 2);

		float3 polyNormal = math::abs(_facesNormal[i / 3]);

		float2 uv[3];

		int flag = 0;
		if (polyNormal.x > polyNormal.y && polyNormal.x > polyNormal.z)
		{
			flag = 1;
			uv[0] = _model->vertices[a].position.yz();
			uv[1] = _model->vertices[b].position.yz();
			uv[2] = _model->vertices[c].position.yz();
		}
		else if (polyNormal.y > polyNormal.x && polyNormal.y > polyNormal.z)
		{
			flag = 2;
			uv[0] = _model->vertices[a].position.xz();
			uv[1] = _model->vertices[b].position.xz();
			uv[2] = _model->vertices[c].position.xz();
		}
		else
		{
			uv[0] = _model->vertices[a].position.xy();
			uv[1] = _model->vertices[b].position.xy();
			uv[2] = _model->vertices[c].position.xy();
		}

		float2 deltaUV = maxUV[flag] - minUV[flag];

		uv[0] -= minUV[flag];
		uv[1] -= minUV[flag];
		uv[2] -= minUV[flag];

		uv[0] /= deltaUV;
		uv[1] /= deltaUV;
		uv[2] /= deltaUV;

		_model->vertices[a].addCoord[0].set(uv[0]);
		_model->vertices[b].addCoord[0].set(uv[1]);
		_model->vertices[c].addCoord[0].set(uv[2]);
	}
}

LightMapNode*
LightMass::insertLightMapItem(LightMapNode* node, LightMapItem& item) noexcept
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
LightMass::load(const std::string& path) noexcept
{
	if (_lightMassListener)
		_lightMassListener->onMessage("loading model : " + path);

	if (path.empty())
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("The input path cannot be empty");

		return false;
	}

	ray::ifstream stream;
	if (!stream.open(path))
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Failed to open the path : " + path);

		return false;
	}

	ray::PMXHandler model;
	if (!model.doCanRead(stream))
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("File is not a valid model with : " + path);

		return false;
	}

	_model = std::make_unique<PMX>();
	if (!model.doLoad(stream, *_model))
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Non readable PMX file : " + path);

		return false;
	}

	if (_lightMassListener)
		_lightMassListener->onMessage("loaded model : " + path);

	return true;
}

bool
LightMass::save(const std::string& path) noexcept
{
	assert(_model);

	ofstream stream;
	if (!stream.open(path))
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Failed to open the path : " + path);
		return false;
	}

	ray::PMXHandler model;
	if (!model.doSave(stream, *_model))
		return false;

	return true;
}

bool 
LightMass::saveAsTGA(const std::string& path, float* data, std::uint32_t w, std::uint32_t h, std::uint32_t c)
{
	if (_lightMassBaking)
		return _lightMassBaking->saveAsTGA(path, data, w, h, c);
	return false;
}

bool 
LightMass::baking(const LightMassParams& params) noexcept
{
	assert(_model);

	assert(params.lightMap.data);
	assert(params.lightMap.width >= 0 && params.lightMap.height >= 0);
	assert(params.lightMap.channel == 1 || params.lightMap.channel == 2 || params.lightMap.channel == 3 || params.lightMap.channel == 4);

	LightBakingOptions option;

	option.baking = params.baking;
	option.lightMap = params.lightMap;

	option.model.vertices = (std::uint8_t*)_model->vertices.data();
	option.model.indices = _model->indices.data();

	option.model.sizeofVertices = sizeof(PMX_Vertex);
	option.model.sizeofIndices = _model->header.sizeOfIndices;

	option.model.strideVertices = offsetof(PMX_Vertex, position);
	option.model.strideTexcoord = offsetof(PMX_Vertex, coord);

	option.model.numVertices = _model->numVertices;
	option.model.numIndices = _model->numIndices;

	option.model.drawcalls.resize(_model->numMaterials);
	option.model.boundingBoxs.resize(_model->numMaterials);

	for (std::uint32_t i = 0; i < _model->numMaterials; i++)
	{
		std::uint32_t offsetFace = 0;

		for (int j = 0; j < i; j++)
			offsetFace += _model->materials[j].FaceCount * _model->header.sizeOfIndices;

		option.model.drawcalls[i].count = _model->materials[i].FaceCount;
		option.model.drawcalls[i].instanceCount = 1;
		option.model.drawcalls[i].firstIndex = offsetFace;
		option.model.drawcalls[i].baseInstance = 0;
		option.model.drawcalls[i].baseVertex = 0;
	}

	if (_lightMassListener)
		_lightMassListener->onMessage("Calculating the bounding box of the model.");

	for (std::uint32_t i = 0; i < _model->numMaterials; i++)
	{
		Bound bound;
		this->computeBoundingBox(bound, option.model.drawcalls[i].firstIndex, option.model.drawcalls[i].count);
		option.model.boundingBoxs[i] = bound;
	}

	if (_lightMassListener)
		_lightMassListener->onMessage("Calculated the bounding box of the model.");

	_lightMassBaking = std::make_shared<LightMassBaking>();
	_lightMassBaking->setLightMassListener(_lightMassListener);

	if (!_lightMassBaking->baking(option))
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Failed to baking the model");
	}

	return true;
}

void
LightMass::setLightMassListener(LightMassListenerPtr pointer) noexcept
{
	if (_lightMassListener != pointer)
	{
		if (_lightMassListener)
			_lightMassListener->onListenerChangeBefore();

		_lightMassListener = pointer;

		if (_lightMassListener)
			_lightMassListener->onListenerChangeAfter();
	}
}

LightMassListenerPtr 
LightMass::getLightMassListener() const noexcept
{
	return _lightMassListener;
}

_NAME_END