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
#include "LightMassAmbientOcclusion.h"
#include "LightMassGlobalIllumination.h"

#include <GL\glew.h>

_NAME_BEGIN

#pragma pack(push)
#pragma pack(1)

struct TGAHeader
{
	std::uint8_t  id_length;
	std::uint8_t  colormap_type;
	std::uint8_t  image_type;
	std::uint16_t colormap_index;
	std::uint16_t colormap_length;
	std::uint8_t  colormap_size;
	std::uint16_t x_origin;
	std::uint16_t y_origin;
	std::uint16_t width;
	std::uint16_t height;
	std::uint8_t  pixel_size;
	std::uint8_t  attributes;
};

#pragma pack(pop)

LightMass::LightMass() noexcept
	: _initialize(false)
	, _lightMassListener(std::make_shared<LightMassListener>())
{
}

LightMass::LightMass(LightMassListenerPtr listener) noexcept
	: _initialize(false)
	, _lightMassListener(listener)
{
}

LightMass::~LightMass() noexcept
{
}

bool
LightMass::open() noexcept
{
	if (_initialize)
		return true;

	if (glewInit() != GLEW_OK)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Could not initialize with OpenGL.");

		return false;
	}

	_initialize = true;

	return true;
}

void 
LightMass::close() noexcept
{
	_initialize = false;
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

bool 
LightMass::baking(const LightMassParams& params, const PMX& model, LightMapData& map) noexcept
{
	assert(_initialize);
	assert(params.lightMap.width >= 0 && params.lightMap.height >= 0);

	LightBakingParams option;
	option.baking = params.baking;
	option.lightMap.width = map.width = params.lightMap.width;
	option.lightMap.height = map.height = params.lightMap.height;
	option.lightMap.channel = map.channel = params.enableGI ? 4 : 1;

	auto lightmap = std::make_unique<float[]>(params.lightMap.width * params.lightMap.height * option.lightMap.channel);
	std::memset(lightmap.get(), 0, params.lightMap.width * params.lightMap.height * sizeof(float));
	option.lightMap.data = std::move(lightmap);

	option.model.vertices = (std::uint8_t*)model.vertices.data();
	option.model.indices = model.indices.data();
	option.model.sizeofVertices = sizeof(PMX_Vertex);
	option.model.sizeofIndices = model.header.sizeOfIndices;
	option.model.strideVertices = offsetof(PMX_Vertex, position);
	option.model.strideTexcoord = offsetof(PMX_Vertex, addCoord[0]);
	option.model.numVertices = model.numVertices;
	option.model.numIndices = model.numIndices;
	option.model.subsets.resize(model.numMaterials);

	for (std::uint32_t i = 0; i < model.numMaterials; i++)
	{
		std::uint32_t offset = 0;

		for (std::uint32_t j = 0; j < i; j++)
			offset += model.materials[j].IndicesCount;

		option.model.subsets[i].emissive = math::srgb2linear(model.materials[i].Ambient) * model.materials[i].Shininess;

		option.model.subsets[i].drawcall.count = model.materials[i].IndicesCount;
		option.model.subsets[i].drawcall.instanceCount = 1;
		option.model.subsets[i].drawcall.firstIndex = offset;
		option.model.subsets[i].drawcall.baseInstance = 0;
		option.model.subsets[i].drawcall.baseVertex = 0;
	}

	if (_lightMassListener)
		_lightMassListener->onMessage("Calculating the bounding box of the model.");

	for (std::uint32_t i = 0; i < model.numMaterials; i++)
	{
		Bound bound;
		this->computeBoundingBox(model, bound, option.model.subsets[i].drawcall.firstIndex * option.model.sizeofIndices, option.model.subsets[i].drawcall.count);
		option.model.subsets[i].boundingBox = bound;
	}

	if (_lightMassListener)
		_lightMassListener->onMessage("Calculated the bounding box of the model.");

	if (params.enableGI)
	{
		auto lightMassBaking = std::make_shared<LightBakingGI>();
		lightMassBaking->open(option.model);
		lightMassBaking->setLightMassListener(_lightMassListener);

		if (!lightMassBaking->baking(option))
		{
			if (_lightMassListener)
				_lightMassListener->onMessage("Failed to baking the model");
		}
	}
	else
	{
		auto lightMassBaking = std::make_shared<LightBakingAO>();
		lightMassBaking->open(option.model);
		lightMassBaking->setLightMassListener(_lightMassListener);

		if (!lightMassBaking->baking(option))
		{
			if (_lightMassListener)
				_lightMassListener->onMessage("Failed to baking the model");
		}
	}

	map.data = std::move(option.lightMap.data);
	return true;
}

std::uint32_t
LightMass::getFace(const PMX& model, std::size_t n) noexcept
{
	std::uint8_t* data = (std::uint8_t*)model.indices.data();

	if (model.header.sizeOfIndices == 1)
		return *(std::uint8_t*)(data + n * model.header.sizeOfIndices);
	else if (model.header.sizeOfIndices == 2)
		return *(std::uint16_t*)(data + n * model.header.sizeOfIndices);
	else if (model.header.sizeOfIndices == 4)
		return *(std::uint32_t*)(data + n * model.header.sizeOfIndices);
	else
		return false;
}

std::uint32_t
LightMass::getFace(const PMX& model, std::size_t n, std::uint32_t firstIndex) noexcept
{
	std::uint8_t* data = (std::uint8_t*)model.indices.data() + firstIndex;

	if (model.header.sizeOfIndices == 1)
		return *(std::uint8_t*)(data + n * model.header.sizeOfIndices);
	else if (model.header.sizeOfIndices == 2)
		return *(std::uint16_t*)(data + n * model.header.sizeOfIndices);
	else if (model.header.sizeOfIndices == 4)
		return *(std::uint32_t*)(data + n * model.header.sizeOfIndices);
	else
		return false;
}

void
LightMass::computeBoundingBox(const PMX& model, Bound& boundingBox, std::uint32_t firstFace, std::uint32_t faceCount) noexcept
{
	boundingBox.reset();

	for (size_t i = 0; i < faceCount; i++)
	{
		std::uint32_t face = this->getFace(model, i, firstFace);
		boundingBox.encapsulate(model.vertices[face].position);
	}
}

_NAME_END