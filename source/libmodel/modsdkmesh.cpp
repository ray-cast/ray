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
#include "modsdkmesh.h"

_NAME_BEGIN

#define SDKMESH_FILE_VERSION 101
#define MAX_VERTEX_ELEMENTS 32
#define MAX_VERTEX_STREAMS 16
#define MAX_FRAME_NAME 100
#define MAX_MESH_NAME 100
#define MAX_SUBSET_NAME 100
#define MAX_MATERIAL_NAME 100
#define MAX_TEXTURE_NAME MAX_PATH
#define MAX_MATERIAL_PATH MAX_PATH
#define INVALID_FRAME ((std::size_t)-1)
#define INVALID_MESH ((std::size_t)-1)
#define INVALID_MATERIAL ((std::size_t)-1)
#define INVALID_SUBSET ((std::size_t)-1)
#define INVALID_ANIMATION_DATA ((std::size_t)-1)
#define ERROR_RESOURCE_VALUE 1
#define INVALID_SAMPLER_SLOT ((std::size_t)-1)
#define MAX_VERTEX_BUFFER 255

#define USAGE_POSITION 0
#define USAGE_NORMAL 3
#define USAGE_TANGENT 6
#define USAGE_TEXCOORD 5

struct D3DVERTEXELEMENT9
{
	std::uint16_t   Stream;     // Stream index
	std::uint16_t   Offset;     // Offset in the stream in bytes
	std::uint8_t    Type;       // Data type
	std::uint8_t    Method;     // Processing method
	std::uint8_t    Usage;      // Semantics
	std::uint8_t    UsageIndex; // Semantic index
};

struct SDKMESH_HEADER
{
	//Basic Info and sizes
	std::uint32_t Version;
	std::uint8_t IsBigEndian;
	std::uint64_t HeaderSize;
	std::uint64_t NonBufferDataSize;
	std::uint64_t BufferDataSize;

	//Stats
	std::uint32_t NumVertexBuffers;
	std::uint32_t NumIndexBuffers;
	std::uint32_t NumMeshes;
	std::uint32_t NumTotalSubsets;
	std::uint32_t NumFrames;
	std::uint32_t NumMaterials;

	//Offsets to Data
	std::uint64_t VertexStreamHeadersOffset;
	std::uint64_t IndexStreamHeadersOffset;
	std::uint64_t MeshDataOffset;
	std::uint64_t SubsetDataOffset;
	std::uint64_t FrameDataOffset;
	std::uint64_t MaterialDataOffset;
};

struct SDKMESH_VERTEX_BUFFER_HEADER
{
	std::uint64_t NumVertices;
	std::uint64_t SizeBytes;
	std::uint64_t StrideBytes;
	D3DVERTEXELEMENT9 Decl[MAX_VERTEX_ELEMENTS];
	std::uint64_t DataOffset;
};

struct SDKMESH_INDEX_BUFFER_HEADER
{
	std::uint64_t NumIndices;
	std::uint64_t SizeBytes;
	std::uint32_t IndexType;
	std::uint64_t DataOffset;
};

struct SDKMESH_MESH
{
	char Name[MAX_MESH_NAME];
	std::uint8_t NumVertexBuffers;
	std::uint32_t VertexBuffers[MAX_VERTEX_STREAMS];
	std::uint32_t IndexBuffer;
	std::uint32_t NumSubsets;
	std::uint32_t NumFrameInfluences; //aka bones

	Vector3 BoundingBoxCenter;
	Vector3 BoundingBoxExtents;

	union
	{
		std::uint64_t SubsetOffset;	//Offset to list of subsets (This also forces the union to 64bits)
		std::uint32_t* pSubsets;	    //Pointer to list of subsets
	};
	union
	{
		std::uint64_t FrameInfluenceOffset;  //Offset to list of frame influences (This also forces the union to 64bits)
		std::uint32_t* pFrameInfluences;      //Pointer to list of frame influences
	};
};

struct SDKMESH_MATERIAL
{
	char    Name[MAX_MATERIAL_NAME];

	// Use MaterialInstancePath
	char    MaterialInstancePath[MAX_MATERIAL_PATH];

	// Or fall back to d3d8-type materials
	char    DiffuseTexture[MAX_TEXTURE_NAME];
	char    NormalTexture[MAX_TEXTURE_NAME];
	char    SpecularTexture[MAX_TEXTURE_NAME];

	Vector4 Diffuse;
	Vector4 Ambient;
	Vector4 Specular;
	Vector4 Emissive;
	float Power;

	std::uint64_t Force64_1;			//Force the union to 64bits
	std::uint64_t Force64_2;			//Force the union to 64bits
	std::uint64_t Force64_3;			//Force the union to 64bits
	std::uint64_t Force64_4;			//Force the union to 64bits
	std::uint64_t Force64_5;		    //Force the union to 64bits
	std::uint64_t Force64_6;			//Force the union to 64bits
};

SDKMeshHandler::SDKMeshHandler() noexcept
{
}

SDKMeshHandler::~SDKMeshHandler() noexcept
{
}

bool
SDKMeshHandler::doCanRead(StreamReader& stream) const noexcept
{
	SDKMESH_HEADER hdr;
	if (stream.read((char*)&hdr, sizeof(hdr)))
	{
		return hdr.Version == SDKMESH_FILE_VERSION;
	}

	return false;
}

bool
SDKMeshHandler::doCanSave(ModelType type) const noexcept
{
	return true;
}

bool
SDKMeshHandler::doLoad(StreamReader& stream, Model& model) noexcept
{
	SDKMESH_HEADER hdr;

	if (!stream.read((char*)&hdr, sizeof(hdr)))
		return false;

	std::vector<SDKMESH_VERTEX_BUFFER_HEADER> vbs;
	std::vector<SDKMESH_INDEX_BUFFER_HEADER> ibs;
	std::vector<SDKMESH_MESH> meshes;
	std::vector<SDKMESH_MATERIAL> materials;

	vbs.resize(hdr.NumVertexBuffers);
	ibs.resize(hdr.NumIndexBuffers);
	meshes.resize(hdr.NumMeshes);
	materials.resize(hdr.NumMaterials);

	if (!stream.seekg(hdr.VertexStreamHeadersOffset, ios_base::beg)) return false;
	if (!stream.read((char*)vbs.data(), sizeof(SDKMESH_VERTEX_BUFFER_HEADER) * hdr.NumVertexBuffers)) return false;

	if (!stream.seekg(hdr.IndexStreamHeadersOffset, ios_base::beg)) return false;
	if (!stream.read((char*)ibs.data(), sizeof(SDKMESH_INDEX_BUFFER_HEADER) * hdr.NumIndexBuffers)) return false;

	if (!stream.seekg(hdr.MeshDataOffset, ios_base::beg)) return false;
	if (!stream.read((char*)meshes.data(), sizeof(SDKMESH_MESH) * hdr.NumMeshes)) return false;

	if (!stream.seekg(hdr.MaterialDataOffset, ios_base::beg)) return false;
	if (!stream.read((char*)materials.data(), sizeof(SDKMESH_MATERIAL) * hdr.NumMaterials)) return false;

	Float3Array vertices;
	Float3Array normals;
	Float4Array tangets;
	Float2Array texcoord;
	UintArray faces;

	MeshPropertyPtr root = nullptr;

	for (auto& it : materials)
	{
		auto material = std::make_shared<MaterialProperty>();

		material->set(MATKEY_NAME, it.Name);
		material->set(MATKEY_COLOR_DIFFUSE, it.Diffuse);
		material->set(MATKEY_COLOR_AMBIENT, it.Ambient);
		material->set(MATKEY_COLOR_SPECULAR, Vector3(0.5, 0.5, 0.5));
		material->set(MATKEY_SHININESS, it.Power);

		if (it.DiffuseTexture[0] != 0)
		{
			material->set(MATKEY_TEXTURE_DIFFUSE(0), it.DiffuseTexture);
			material->set(MATKEY_TEXTURE_AMBIENT(0), it.DiffuseTexture);
		}

		if (it.NormalTexture[0] != 0)
		{
			material->set(MATKEY_TEXTURE_NORMALS(0), it.NormalTexture);
		}

		if (it.SpecularTexture[0] != 0)
		{
			material->set(MATKEY_TEXTURE_SPECULAR(0), it.SpecularTexture);
		}

		model.addMaterial(std::move(material));
	}

	for (auto& mesh : meshes)
	{
		for (std::size_t i = 0; i < mesh.NumVertexBuffers; i++)
		{
			stream.seekg(vbs[i].DataOffset, ios_base::beg);

			for (std::size_t j = 0; j < vbs[i].NumVertices; j++)
			{
				std::uint8_t offset = 0;
				std::uint8_t buffer[MAX_VERTEX_BUFFER];
				stream.read((char*)buffer, vbs[i].StrideBytes);

				for (std::size_t element = 0; element < MAX_VERTEX_ELEMENTS; element++)
				{
					if (vbs[i].Decl[element].Stream != 0)
					{
						offset = 0;
						break;
					}

					if (vbs[i].Decl[element].Usage == USAGE_POSITION)
					{
						vertices.push_back(*(Vector3*)((char*)&buffer + offset));
						offset += sizeof(Vector3);
					}
					else if (vbs[i].Decl[element].Usage == USAGE_NORMAL)
					{
						normals.push_back(*(Vector3*)((char*)&buffer + offset));
						offset += sizeof(Vector3);
					}
					else if (vbs[i].Decl[element].Usage == USAGE_TANGENT)
					{
						tangets.push_back(float4(*(Vector3*)((char*)&buffer + offset), 1.0));
						offset += sizeof(Vector3);
					}
					else if (vbs[i].Decl[element].Usage == USAGE_TEXCOORD)
					{
						texcoord.push_back(*(Vector2*)((char*)&buffer + offset));
						offset += sizeof(Vector2);
					}
				}
			}
		}

		stream.seekg(ibs[mesh.IndexBuffer].DataOffset, ios_base::beg);

		auto sizeOfData = ibs[mesh.IndexBuffer].SizeBytes / ibs[mesh.IndexBuffer].NumIndices;

		for (std::size_t j = 0; j < ibs[mesh.IndexBuffer].NumIndices; j++)
		{
			std::uint32_t buffer = 0;
			stream.read((char*)&buffer, sizeOfData);

			faces.push_back(buffer);
		}

		MeshPropertyPtr subset = std::make_shared<MeshProperty>();
		subset->setVertexArray(std::move(vertices));
		subset->setNormalArray(std::move(normals));
		subset->setTexcoordArray(std::move(texcoord));
		subset->setTangentArray(std::move(tangets));
		subset->setFaceArray(std::move(faces));

		if (root)
			root->addChild(subset);
		else
			root = subset;
	}

	model.addMesh(std::move(root));

	return true;
}

bool
SDKMeshHandler::doSave(StreamWrite& stream, Model& model) noexcept
{
	return false;
}

_NAME_END