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
#include <sstream>

#include <UVAtlas/UVAtlas.h>
#include <DirectXMesh/DirectXMesh.h>

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
	: _lightMassListener(std::make_shared<LightMassListener>())
{
}

LightMass::~LightMass() noexcept
{
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

bool 
LightMass::load(const std::string& path, PMX& pmx) noexcept
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

	if (!model.doLoad(stream, pmx))
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
LightMass::save(const std::string& path, const PMX& pmx) noexcept
{
	ofstream stream;
	if (!stream.open(path))
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Failed to open the path : " + path);
		return false;
	}

	ray::PMXHandler model;
	if (!model.doSave(stream, pmx))
		return false;

	return true;
}

bool 
LightMass::saveLightMass(const std::string& path, float* data, std::uint32_t w, std::uint32_t h, std::uint32_t c, std::uint32_t margin)
{
	assert(c == 1 || c == 3 || c == 4);

	bool isGreyscale = c == 1;
	bool hasAlpha = c == 4;

	TGAHeader header;
	header.id_length = 0;
	header.colormap_type = 0;
	header.image_type = isGreyscale ? 3 : 2;
	header.colormap_index = 0;
	header.colormap_length = 0;
	header.colormap_size = 0;
	header.x_origin = 0;
	header.y_origin = 0;
	header.width = w;
	header.height = h;
	header.pixel_size = c * 8;
	header.attributes = hasAlpha ? 8 : 0;

	if (margin > 0)
	{
		std::unique_ptr<float[]> lightmapTemp = std::make_unique<float[]>(w * h * c);
		std::memset(lightmapTemp.get(), 0, w * h * c * sizeof(float));

		for (std::uint32_t j = 0; j < margin; j++)
		{
			ImageSmooth(data, lightmapTemp.get(), w, h, c);
			ImageDilate(lightmapTemp.get(), data, w, h, c);
		}
	}

	auto temp = std::make_unique<std::uint8_t[]>(w * h * c);
	auto image = temp.get();

	float maxValue = 0.0f;
	for (std::uint32_t i = 0; i < w * h; i++)
		for (std::uint32_t j = 0; j < c; j++)
				maxValue = std::max(maxValue, data[i * c + j]);

	for (std::uint32_t i = 0; i < w * h * c; i++)
		image[i] = math::clamp<std::uint8_t>(data[i] * 255 / maxValue, 0, 255);

	for (std::uint32_t j = 0; j < h / 2; j++)
		for (std::uint32_t i = 0; i < w * c; i++)
			std::swap(image[i + j * (w * c)], image[(h - j - 1) * (w * c) + i]);

	if (!isGreyscale)
	{
		for (std::size_t i = 0; i < w * h * c; i += c)
			std::swap(image[i], image[i + 2]);
	}

	ofstream stream;
	if (stream.open(path))
	{
		stream.write((char*)&header, sizeof(header));
		stream.write((char*)image, w * h * c);
	}

	return true;
}

bool 
LightMass::pack(const LightMassParams& params, PMX& model) noexcept
{
	if (_lightMassListener)
		_lightMassListener->onUvmapperStart();

	std::vector<float3> atlasVertices;
	std::vector<std::uint32_t> atlasIndices;

	for (std::size_t i = 0; i < model.numVertices; i++)
		atlasVertices.push_back(model.vertices[i].position);

	std::unique_ptr<uint32_t[]> adj(new uint32_t[model.numIndices]);

	if (model.header.sizeOfIndices == 1)
		return false;
	else if (model.header.sizeOfIndices == 2)
		DirectX::GenerateAdjacencyAndPointReps((std::uint16_t*)model.indices.data(), model.numIndices / 3, (DirectX::XMFLOAT3*)atlasVertices.data(), model.numVertices, 0.0, nullptr, adj.get());
	else
		DirectX::GenerateAdjacencyAndPointReps((std::uint32_t*)model.indices.data(), model.numIndices / 3, (DirectX::XMFLOAT3*)atlasVertices.data(), model.numVertices, 0.0, nullptr, adj.get());

	std::vector<DirectX::UVAtlasVertex> vb;
	std::vector<uint8_t> ib;
	std::vector<uint32_t> remap;

	auto callback = [&](float percentComplete) -> HRESULT
	{
		if (_lightMassListener)
			_lightMassListener->onUvmapperProgressing(percentComplete);

		return true;
	};

	if (model.header.sizeOfIndices == 1)
		return false;
	else if (model.header.sizeOfIndices == 2)
	{
		HRESULT hr = DirectX::UVAtlasCreate(
			(DirectX::XMFLOAT3*)atlasVertices.data(),
			atlasVertices.size(),
			model.indices.data(),
			DXGI_FORMAT::DXGI_FORMAT_R16_UINT,
			model.numIndices / 3, 0, 0, params.lightMap.width, params.lightMap.height, 2.0, adj.get(), 0, 0, 
			callback, 
			0.1,
			DirectX::UVATLAS_GEODESIC_QUALITY,
			vb, ib, 0, &remap);
		if (FAILED(hr))
		{
			if (_lightMassListener)
				_lightMassListener->onMessage("Failed to UV Atlas.");
			return false;
		}
	}
	else
	{
		HRESULT hr = DirectX::UVAtlasCreate(
			(DirectX::XMFLOAT3*)atlasVertices.data(),
			atlasVertices.size(),
			model.indices.data(),
			DXGI_FORMAT::DXGI_FORMAT_R32_UINT,
			model.numIndices / 3, 0, 0, params.lightMap.width, params.lightMap.height, 2.0, adj.get(), 0, 0, 
			callback,
			0.1,
			DirectX::UVATLAS_GEODESIC_QUALITY,
			vb, ib, 0, &remap);
		if (FAILED(hr))
		{
			if (_lightMassListener)
				_lightMassListener->onMessage("Failed to UV Atlas.");
			return false;
		}
	}

	std::vector<PMX_Vertex> newVertices;
	newVertices.resize(vb.size());

	for (int i = 0; i < vb.size(); i++)
	{
		PMX_Vertex v = model.vertices[remap[i]];
		v.addCoord->x = vb[i].uv.x;
		v.addCoord->y = vb[i].uv.y;

		newVertices[i] = v;
	}

	model.header.addUVCount = 1;
	model.numVertices = newVertices.size();
	model.vertices = newVertices;
	model.indices = ib;

	return true;
}

bool 
LightMass::baking(const LightMassParams& params, const PMX& model) noexcept
{
	assert(params.lightMap.data);
	assert(params.lightMap.width >= 0 && params.lightMap.height >= 0);
	assert(params.lightMap.channel == 1 || params.lightMap.channel == 2 || params.lightMap.channel == 3 || params.lightMap.channel == 4);

	if (glewInit() != GLEW_OK)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Could not initialize with OpenGL.");

		return false;
	}

	LightBakingOptions option;
	option.baking = params.baking;
	option.lightMap = params.lightMap;

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

	auto lightMassBaking = std::make_shared<LightBakingAO>();
	lightMassBaking->open(option.model);
	lightMassBaking->setLightMassListener(_lightMassListener);

	if (!lightMassBaking->baking(option))
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

void 
LightMass::ImageDilate(const float *image, float *outImage, int w, int h, int c) noexcept
{
	assert(c > 0 && c <= 4);
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			float color[4];
			bool valid = false;
			for (int i = 0; i < c; i++)
			{
				color[i] = image[(y * w + x) * c + i];
				valid |= color[i] > 0.0f;
			}
			if (!valid)
			{
				int n = 0;
				const int dx[] = { -1, 0, 1,  0 };
				const int dy[] = { 0, 1, 0, -1 };
				for (int d = 0; d < 4; d++)
				{
					int cx = x + dx[d];
					int cy = y + dy[d];
					if (cx >= 0 && cx < w && cy >= 0 && cy < h)
					{
						float dcolor[4];
						bool dvalid = false;
						for (int i = 0; i < c; i++)
						{
							dcolor[i] = image[(cy * w + cx) * c + i];
							dvalid |= dcolor[i] > 0.0f;
						}
						if (dvalid)
						{
							for (int i = 0; i < c; i++)
								color[i] += dcolor[i];
							n++;
						}
					}
				}
				if (n)
				{
					float in = 1.0f / n;
					for (int i = 0; i < c; i++)
						color[i] *= in;
				}
			}
			for (int i = 0; i < c; i++)
				outImage[(y * w + x) * c + i] = color[i];
		}
	}
}

void 
LightMass::ImageSmooth(const float *image, float *outImage, int w, int h, int c) noexcept
{
	assert(c > 0 && c <= 4);
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			float color[4] = { 0 };
			int n = 0;
			for (int dy = -1; dy <= 1; dy++)
			{
				int cy = y + dy;
				for (int dx = -1; dx <= 1; dx++)
				{
					int cx = x + dx;
					if (cx >= 0 && cx < w && cy >= 0 && cy < h)
					{
						bool valid = false;
						for (int i = 0; i < c; i++)
							valid |= image[(cy * w + cx) * c + i] > 0.0f;
						if (valid)
						{
							for (int i = 0; i < c; i++)
								color[i] += image[(cy * w + cx) * c + i];
							n++;
						}
					}
				}
			}
			for (int i = 0; i < c; i++)
				outImage[(y * w + x) * c + i] = n ? color[i] / n : 0.0f;
		}
	}
}

_NAME_END