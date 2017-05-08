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
#include "modpmx.h"

_NAME_BEGIN

#define PMX_VERSION_2_0   2.0
#define PMX_BONE_INDEX    1 << 0
#define PMX_BONE_MOVE     1 << 1
#define PMX_BONE_IK       1 << 5
#define PMX_BONE_PARENT   1 << 8
#define PMX_BONE_AXIS     1 << 10
#define PMX_BONE_ROTATE   1 << 11

typedef Vector2 PMX_Vector2;
typedef Vector3 PMX_Vector3;
typedef Vector4 PMX_Vector4;

typedef Vector3 PMX_Color3;
typedef Vector4 PMX_Color4;
typedef float3x3 PMX_Float3x3;
typedef float4x4 PMX_Float4x4;

typedef wchar_t       PMX_char;
typedef std::int8_t   PMX_int8_t;
typedef std::uint8_t  PMX_uint8_t;
typedef std::uint16_t PMX_uint16_t;
typedef std::uint32_t PMX_uint32_t;

typedef float PMX_Float;

enum
{
	PMX_BDEF1,
	PMX_BDEF2,
	PMX_BDEF4,
	PMX_SDEF,
	PMX_QDEF
};

enum MorphType
{
	MorphTypeGroup = 0,
	MorphTypeVertex = 1,
	MorphTypeBone = 2,
	MorphTypeUV = 3,
	MorphTypeExtraUV1 = 4,
	MorphTypeExtraUV2 = 5,
	MorphTypeExtraUV3 = 6,
	MorphTypeExtraUV4 = 7,
	MorphTypeMaterial = 8
};

#pragma pack(push)
#pragma pack(push,1)

struct PMX_Header
{
	PMX_uint8_t magic[3];  // 始终为PMX
	PMX_uint8_t offset;    // 始终为 0x20
	PMX_Float   version;   // 版本
	PMX_uint8_t dataSize;  // 始终 0x08
	PMX_uint8_t encode;    // 0x00 UTF-16(LE) 0x01 UTF-8
	PMX_uint8_t addUVCount;     // 追加的UV ( 1 ~ 4 )
	PMX_uint8_t sizeOfVertex;   // ( 1 or 2 or 4 )
	PMX_uint8_t sizeOfTexture;  // ( 1 or 2 or 4 )
	PMX_uint8_t sizeOfMaterial; // ( 1 or 2 or 4 )
	PMX_uint8_t sizeOfBone;     // ( 1 or 2 or 4 )
	PMX_uint8_t sizeOfMorph;    // ( 1 or 2 or 4 )
	PMX_uint8_t sizeOfBody;     // ( 1 or 2 or 4 )
};

#pragma pack(pop)

struct PMX_Description
{
	PMX_uint32_t japanModelLength;
	PMX_uint32_t japanCommentLength;

	PMX_uint32_t englishModelLength;
	PMX_uint32_t englishCommentLength;

	std::vector<PMX_char> japanModelName;
	std::vector<PMX_char> japanCommentName;

	std::vector<PMX_char> englishModelName;
	std::vector<PMX_char> englishCommentName;
};

struct PMX_BoneWeight
{
	PMX_uint16_t bone1;
	PMX_uint16_t bone2;
	PMX_uint16_t bone3;
	PMX_uint16_t bone4;

	PMX_Float weight1;
	PMX_Float weight2;
	PMX_Float weight3;
	PMX_Float weight4;

	PMX_Vector3 SDEF_C;
	PMX_Vector3 SDEF_R0;
	PMX_Vector3 SDEF_R1;
};

struct PMX_Vertex
{
	PMX_Vector3 position;
	PMX_Vector3 normal;
	PMX_Vector2 coord;
	PMX_Vector4 addCoord[4];
	PMX_uint8_t type;	// 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF
	PMX_BoneWeight weight;
	PMX_Float   edge;
};

typedef PMX_uint8_t PMX_Index; //顶点索引

struct PMX_Name
{
	PMX_uint32_t length;
	PMX_char name[MAX_PATH];
};

struct PMX_Material
{
	PMX_Name name;
	PMX_Name nameEng;
	PMX_Color3  Diffuse;
	PMX_Float   Opacity;
	PMX_Color3  Specular;
	PMX_Float   Shininess;
	PMX_Color3  Ambient;
	PMX_uint8_t Flag;
	PMX_Vector4 EdgeColor;
	PMX_Float   EdgeSize;
	PMX_uint16_t TextureIndex;
	PMX_uint16_t SphereTextureIndex;
	PMX_uint8_t  SphereMode;
	PMX_uint8_t  ToonIndex;
	PMX_uint16_t ToneTexture;
	PMX_uint32_t memLength;
	PMX_char	 mem[MAX_PATH];
	PMX_uint32_t FaceCount;
};

struct PMX_IK
{
	PMX_uint16_t BoneIndex;
	PMX_uint8_t rotateLimited;
	PMX_Vector3 minimumRadian;
	PMX_Vector3 maximumRadian;
};

struct PMX_Bone
{
	PMX_Name name;
	PMX_Name nameEng;
	PMX_Vector3 position;
	PMX_uint16_t Parent;
	PMX_uint32_t Level;
	PMX_uint16_t Flag;
	PMX_uint16_t ConnectedBoneIndex;
	PMX_uint16_t ProvidedParentBoneIndex;
	PMX_Float  ProvidedRatio;
	PMX_Vector3 Offset;
	PMX_Vector3 AxisDirection;
	PMX_Vector3 DimentionXDirection;
	PMX_Vector3 DimentionZDirection;
	PMX_uint16_t IKTargetBoneIndex;
	PMX_uint32_t IKLoopCount;
	PMX_Float IKLimitedRadian;
	PMX_uint32_t IKLinkCount;
	std::vector<PMX_IK> IKList;
};

struct PMX_MorphVertex
{
	PMX_uint32_t index;
	PMX_Vector3  offset;
};

struct PMX_MorphTexcoord
{
	PMX_uint32_t index;
	PMX_Vector4  offset;
};

struct PMX_MorphBone
{
	PMX_uint32_t boneIndex;
	PMX_Vector3 position;
	PMX_Vector3 rotate;
};

struct PMX_MorphMaterial
{
	PMX_uint32_t index;
	PMX_uint8_t offset;
	PMX_Color4  diffuse;
	PMX_Float   shininess;
	PMX_Color3  specular;
	PMX_Color3  ambient;
	PMX_Color4  edgeColor;
	PMX_Float   edgeSize;
	PMX_Color4  tex;
	PMX_Color4  sphere;
	PMX_Color4  toon;
};

struct PMX_DisplayFrameElement
{
	PMX_uint8_t  target; // 1 : morph 0 : bone
	PMX_uint32_t index;
};

struct PMX_DisplayFrame
{
	PMX_Name name;
	PMX_Name nameEng;
	PMX_uint8_t type;
	PMX_uint32_t elementsWithinFrame;
	std::vector<PMX_DisplayFrameElement> elements;
};

struct PMX_Rigidbody
{
	PMX_Name name;
	PMX_Name nameEng;

	PMX_uint32_t bone;

	PMX_uint8_t group;
	PMX_uint16_t groupMask;
	PMX_uint8_t shape; // 0:Circle 1:Square 2:Capsule

	PMX_Vector3 scale;
	PMX_Vector3 position;
	PMX_Vector3 rotate;

	PMX_Float mass;
	PMX_Float movementDecay; // movement reduction
	PMX_Float rotationDecay; // rotation reduction
	PMX_Float elasticity;    // recoil
	PMX_Float friction;      // strength of friction

	PMX_uint8_t physicsOperation; //0:Follow Bone (static), 1:Physics Calc. (dynamic), 2: Physics Calc. + Bone position matching
};

struct PMX_Joint
{
	PMX_Name name;
	PMX_Name nameEng;

	PMX_uint8_t type; //0 : Spring 6DOF; in PMX 2.0 always set to 0 (included to give room for expansion)

	PMX_uint32_t relatedRigidBodyIndexA; //-1 if irrelevant
	PMX_uint32_t relatedRigidBodyIndexB;

	PMX_Vector3 position;
	PMX_Vector3 rotation;

	PMX_Vector3 movementLowerLimit;
	PMX_Vector3 movementUpperLimit;
	PMX_Vector3 rotationLowerLimit; //-> radian angle
	PMX_Vector3 rotationUpperLimit; //-> radian angle

	PMX_Vector3 springMovementConstant;
	PMX_Vector3 springRotationConstant;
};

struct PMX_Morph
{
	PMX_Name name;
	PMX_Name nameEng;

	PMX_uint8_t  control;
	PMX_uint8_t  morphType;
	PMX_uint32_t morphCount;

	PMX_uint16_t morphIndex;
	PMX_Float	 morphRate;

	std::vector<PMX_MorphVertex>  vertexList;
	std::vector<PMX_MorphBone> boneList;
	std::vector<PMX_MorphTexcoord> texcoordList;
	std::vector<PMX_MorphMaterial> materialList;
};

struct PMX
{
	PMX_Header header;
	PMX_Description description;

	PMX_uint32_t numVertices;
	PMX_uint32_t numIndices;
	PMX_uint32_t numTextures;
	PMX_uint32_t numMaterials;
	PMX_uint32_t numBones;
	PMX_uint32_t numMorphs;
	PMX_uint32_t numDisplayFrames;
	PMX_uint32_t numRigidbodys;
	PMX_uint32_t numJoints;

	std::vector<PMX_Vertex> vertices;
	std::vector<PMX_Index> indices;
	std::vector<PMX_Name> textures;
	std::vector<PMX_Material> materials;
	std::vector<PMX_Bone> bones;
	std::vector<PMX_Morph> morphs;
	std::vector<PMX_DisplayFrame> displayFrames;
	std::vector<PMX_Rigidbody> rigidbodys;
	std::vector<PMX_Joint> joints;
};

PMXHandler::PMXHandler() noexcept
{
}

PMXHandler::~PMXHandler() noexcept
{
}

bool
PMXHandler::doCanRead(StreamReader& stream) const noexcept
{
	PMX_Header hdr;

	if (stream.read((char*)&hdr, sizeof(hdr)))
	{
		if (hdr.magic[0] == 'p' || hdr.magic[0] == 'P' &&
			hdr.magic[1] == 'm' || hdr.magic[1] == 'M' &&
			hdr.magic[2] == 'x' || hdr.magic[2] == 'X')
		{
			if (hdr.version == 2.0)
			{
				return true;
			}
		}
	}

	return false;
}

bool 
PMXHandler::doCanSave(ModelType type) const noexcept
{
	if (type == ModelType::MT_PMX)
		return true;
	else
		return false;
}

bool
PMXHandler::doLoad(Model& model, StreamReader& stream) noexcept
{
	PMX pmx;

	setlocale(LC_ALL, "");

	if (!stream.read((char*)&pmx.header, sizeof(pmx.header))) return false;
	if (!stream.read((char*)&pmx.description.japanModelLength, sizeof(pmx.description.japanModelLength))) return false;

	if (pmx.description.japanModelLength > 0)
	{
		pmx.description.japanModelName.resize(pmx.description.japanModelLength);

		if (!stream.read((char*)&pmx.description.japanModelName[0], pmx.description.japanModelLength)) return false;
	}

	if (!stream.read((char*)&pmx.description.englishModelLength, sizeof(pmx.description.englishModelLength))) return false;

	if (pmx.description.englishModelLength > 0)
	{
		pmx.description.englishModelName.resize(pmx.description.englishModelLength);

		if (!stream.read((char*)&pmx.description.englishModelName[0], pmx.description.englishModelLength)) return false;
	}

	if (!stream.read((char*)&pmx.description.japanCommentLength, sizeof(pmx.description.japanCommentLength))) return false;

	if (pmx.description.japanCommentLength > 0)
	{
		pmx.description.japanCommentName.resize(pmx.description.japanCommentLength);

		if (!stream.read((char*)&pmx.description.japanCommentName[0], pmx.description.japanCommentLength)) return false;
	}

	if (!stream.read((char*)&pmx.description.englishCommentLength, sizeof(pmx.description.englishCommentLength))) return false;

	if (pmx.description.englishCommentLength > 0)
	{
		pmx.description.englishCommentName.resize(pmx.description.englishCommentLength);

		if (!stream.read((char*)&pmx.description.englishCommentName[0], pmx.description.englishCommentLength)) return false;
	}

	if (!stream.read((char*)&pmx.numVertices, sizeof(pmx.numVertices))) return false;

	if (pmx.numVertices > 0)
	{
		pmx.vertices.resize(pmx.numVertices);

		for (auto& vertex : pmx.vertices)
		{
			if (pmx.header.addUVCount == 0)
			{
				std::streamsize size = sizeof(vertex.position) + sizeof(vertex.normal) + sizeof(vertex.coord);
				if (!stream.read((char*)&vertex.position, size)) return false;
			}
			else
			{
				std::streamsize size = sizeof(vertex.position) + sizeof(vertex.normal) + sizeof(vertex.coord) + sizeof(vertex.addCoord[0]) * pmx.header.addUVCount;
				if (!stream.read((char*)&vertex.position, size)) return false;
			}

			if (!stream.read((char*)&vertex.type, sizeof(vertex.type))) return false;
			switch (vertex.type)
			{
			case PMX_BDEF1:
			{
				if (!stream.read((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
				vertex.weight.weight1 = 1.0f;
			}
			break;
			case PMX_BDEF2:
			{
				if (!stream.read((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight2))) return false;
				vertex.weight.weight2 = 1.0f - vertex.weight.weight1;
			}
			break;
			case PMX_BDEF4:
			{
				if (!stream.read((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&vertex.weight.bone3, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&vertex.weight.bone4, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;
				if (!stream.read((char*)&vertex.weight.weight2, sizeof(vertex.weight.weight2))) return false;
				if (!stream.read((char*)&vertex.weight.weight3, sizeof(vertex.weight.weight3))) return false;
				if (!stream.read((char*)&vertex.weight.weight4, sizeof(vertex.weight.weight4))) return false;
			}
			break;
			case PMX_SDEF:
			{
				if (!stream.read((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;
				if (!stream.read((char*)&vertex.weight.SDEF_C, sizeof(vertex.weight.SDEF_C))) return false;
				if (!stream.read((char*)&vertex.weight.SDEF_R0, sizeof(vertex.weight.SDEF_R0))) return false;
				if (!stream.read((char*)&vertex.weight.SDEF_R1, sizeof(vertex.weight.SDEF_R1))) return false;

				vertex.weight.weight2 = 1.0f - vertex.weight.weight1;
			}
			break;
			case PMX_QDEF:
			{
				if (!stream.read((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;

				vertex.weight.weight2 = 1.0f - vertex.weight.weight1;
			}
			default:
				return false;
			}

			if (!stream.read((char*)&vertex.edge, sizeof(vertex.edge))) return false;
		}
	}

	if (!stream.read((char*)&pmx.numIndices, sizeof(pmx.numIndices))) return false;

	if (pmx.numIndices > 0)
	{
		pmx.indices.resize(pmx.numIndices * pmx.header.sizeOfVertex);
		if (!stream.read((char*)pmx.indices.data(), pmx.indices.size())) return false;
	}

	if (!stream.read((char*)&pmx.numTextures, sizeof(pmx.numTextures))) return false;

	if (pmx.numTextures > 0)
	{
		pmx.textures.resize(pmx.numTextures);

		for (auto& texture : pmx.textures)
		{
			if (!stream.read((char*)&texture.length, sizeof(texture.length))) return false;
			if (!stream.read((char*)&texture.name, texture.length)) return false;
		}
	}

	if (!stream.read((char*)&pmx.numMaterials, sizeof(pmx.numMaterials))) return false;

	if (pmx.numMaterials > 0)
	{
		pmx.materials.resize(pmx.numMaterials);

		for (auto& material : pmx.materials)
		{
			if (!stream.read((char*)&material.name.length, sizeof(material.name.length))) return false;
			if (!stream.read((char*)&material.name.name, material.name.length)) return false;
			if (!stream.read((char*)&material.nameEng.length, sizeof(material.nameEng.length))) return false;
			if (!stream.read((char*)&material.nameEng.name, material.nameEng.length)) return false;
			if (!stream.read((char*)&material.Diffuse, sizeof(material.Diffuse))) return false;
			if (!stream.read((char*)&material.Opacity, sizeof(material.Opacity))) return false;
			if (!stream.read((char*)&material.Specular, sizeof(material.Specular))) return false;
			if (!stream.read((char*)&material.Shininess, sizeof(material.Shininess))) return false;
			if (!stream.read((char*)&material.Ambient, sizeof(material.Ambient))) return false;
			if (!stream.read((char*)&material.Flag, sizeof(material.Flag))) return false;
			if (!stream.read((char*)&material.EdgeColor, sizeof(material.EdgeColor))) return false;
			if (!stream.read((char*)&material.EdgeSize, sizeof(material.EdgeSize))) return false;
			if (!stream.read((char*)&material.TextureIndex, pmx.header.sizeOfTexture)) return false;
			if (!stream.read((char*)&material.SphereTextureIndex, pmx.header.sizeOfTexture)) return false;
			if (!stream.read((char*)&material.SphereMode, sizeof(material.SphereMode))) return false;
			if (!stream.read((char*)&material.ToonIndex, sizeof(material.ToonIndex))) return false;

			if (material.ToonIndex == 1)
			{
				if (!stream.read((char*)&material.ToneTexture, 1)) return false;
			}
			else
			{
				if (!stream.read((char*)&material.ToneTexture, pmx.header.sizeOfTexture)) return false;
			}
			
			if (!stream.read((char*)&material.memLength, sizeof(material.memLength))) return false;
			if (material.memLength > 0)
			{
				if (!stream.read((char*)&material.mem, material.memLength)) return false;
			}


			if (!stream.read((char*)&material.FaceCount, sizeof(material.FaceCount))) return false;
		}
	}

	if (!stream.read((char*)&pmx.numBones, sizeof(pmx.numBones))) return false;

	if (pmx.numBones > 0)
	{
		pmx.bones.resize(pmx.numBones);

		for (auto& bone : pmx.bones)
		{
			if (!stream.read((char*)&bone.name.length, sizeof(bone.name.length))) return false;
			if (!stream.read((char*)&bone.name.name, bone.name.length)) return false;
			if (!stream.read((char*)&bone.nameEng.length, sizeof(bone.nameEng.length))) return false;
			if (!stream.read((char*)&bone.nameEng.name, bone.nameEng.length)) return false;

			if (!stream.read((char*)&bone.position, sizeof(bone.position))) return false;
			if (!stream.read((char*)&bone.Parent, pmx.header.sizeOfBone)) return false;
			if (!stream.read((char*)&bone.Level, sizeof(bone.Level))) return false;
			if (!stream.read((char*)&bone.Flag, sizeof(bone.Flag))) return false;

			if (bone.Flag & PMX_BONE_INDEX)
			{
				if (!stream.read((char*)&bone.ConnectedBoneIndex, pmx.header.sizeOfBone)) return false;
			}
			else
			{
				if (!stream.read((char*)&bone.Offset, sizeof(bone.Offset))) return false;
			}

			if (bone.Flag & PMX_BONE_PARENT)
			{
				if (!stream.read((char*)&bone.ProvidedParentBoneIndex, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&bone.ProvidedRatio, sizeof(bone.ProvidedRatio))) return false;
			}

			if (bone.Flag & PMX_BONE_AXIS)
			{
				if (!stream.read((char*)&bone.AxisDirection, sizeof(bone.AxisDirection))) return false;
			}

			if (bone.Flag & PMX_BONE_ROTATE)
			{
				if (!stream.read((char*)&bone.DimentionXDirection, sizeof(bone.DimentionXDirection))) return false;
				if (!stream.read((char*)&bone.DimentionZDirection, sizeof(bone.DimentionZDirection))) return false;
			}

			if (bone.Flag & PMX_BONE_IK)
			{
				if (!stream.read((char*)&bone.IKTargetBoneIndex, pmx.header.sizeOfBone)) return false;
				if (!stream.read((char*)&bone.IKLoopCount, sizeof(bone.IKLoopCount))) return false;
				if (!stream.read((char*)&bone.IKLimitedRadian, sizeof(bone.IKLimitedRadian))) return false;
				if (!stream.read((char*)&bone.IKLinkCount, sizeof(bone.IKLinkCount))) return false;

				if (bone.IKLinkCount > 0)
				{
					bone.IKList.resize(bone.IKLinkCount);

					for (std::size_t j = 0; j < bone.IKLinkCount; j++)
					{
						if (!stream.read((char*)&bone.IKList[j].BoneIndex, pmx.header.sizeOfBone)) return false;
						if (!stream.read((char*)&bone.IKList[j].rotateLimited, (std::streamsize)sizeof(bone.IKList[j].rotateLimited))) return false;
						if (bone.IKList[j].rotateLimited)
						{
							if (!stream.read((char*)&bone.IKList[j].maximumRadian, (std::streamsize)sizeof(bone.IKList[j].maximumRadian))) return false;
							if (!stream.read((char*)&bone.IKList[j].minimumRadian, (std::streamsize)sizeof(bone.IKList[j].minimumRadian))) return false;
						}
					}
				}
			}
		}
	}

	if (!stream.read((char*)&pmx.numMorphs, sizeof(pmx.numMorphs))) return false;

	if (pmx.numMorphs > 0)
	{
		pmx.morphs.resize(pmx.numMorphs);

		for (auto& morph : pmx.morphs)
		{
			if (!stream.read((char*)&morph.name.length, sizeof(morph.name.length))) return false;
			if (!stream.read((char*)&morph.name.name, morph.name.length)) return false;
			if (!stream.read((char*)&morph.nameEng.length, sizeof(morph.nameEng.length))) return false;
			if (!stream.read((char*)&morph.nameEng.name, morph.nameEng.length)) return false;
			if (!stream.read((char*)&morph.control, sizeof(morph.control))) return false;
			if (!stream.read((char*)&morph.morphType, sizeof(morph.morphType))) return false;
			if (!stream.read((char*)&morph.morphCount, sizeof(morph.morphCount))) return false;

			if (morph.morphType == MorphType::MorphTypeGroup)
			{
				if (!stream.read((char*)&morph.morphIndex, pmx.header.sizeOfMorph)) return false;
				if (!stream.read((char*)&morph.morphRate, sizeof(morph.morphRate))) return false;
			}
			else if (morph.morphType == MorphType::MorphTypeVertex)
			{
				morph.vertexList.resize(morph.morphCount);

				for (auto& vertex : morph.vertexList)
				{
					if (!stream.read((char*)&vertex.index, pmx.header.sizeOfVertex)) return false;
					if (!stream.read((char*)&vertex.offset, sizeof(vertex.offset))) return false;
				}
			}
			else if (morph.morphType == MorphType::MorphTypeBone)
			{
				morph.boneList.resize(morph.morphCount);

				for (auto& bone : morph.boneList)
				{
					if (!stream.read((char*)&bone.boneIndex, pmx.header.sizeOfBone)) return false;
					if (!stream.read((char*)&bone.position, sizeof(bone.position))) return false;
					if (!stream.read((char*)&bone.rotate, sizeof(bone.rotate))) return false;
				}
			}
			else if (morph.morphType == MorphType::MorphTypeUV || morph.morphType == MorphType::MorphTypeExtraUV1 ||
				morph.morphType == MorphType::MorphTypeExtraUV2 || morph.morphType == MorphType::MorphTypeExtraUV3 ||
				morph.morphType == MorphType::MorphTypeExtraUV4)
			{
				morph.texcoordList.resize(morph.morphCount);

				for (auto& texcoord : morph.texcoordList)
				{
					if (!stream.read((char*)&texcoord.index, pmx.header.sizeOfVertex)) return false;
					if (!stream.read((char*)&texcoord.offset, sizeof(texcoord.offset))) return false;
				}
			}
			else if (morph.morphType == MorphType::MorphTypeMaterial)
			{
				morph.materialList.resize(morph.morphCount);

				for (auto& material : morph.materialList)
				{
					if (!stream.read((char*)&material.index, pmx.header.sizeOfMaterial)) return false;
					if (!stream.read((char*)&material.offset, sizeof(material.offset))) return false;
					if (!stream.read((char*)&material.diffuse, sizeof(material.diffuse))) return false;
					if (!stream.read((char*)&material.specular, sizeof(material.specular))) return false;
					if (!stream.read((char*)&material.shininess, sizeof(material.shininess))) return false;
					if (!stream.read((char*)&material.ambient, sizeof(material.ambient))) return false;
					if (!stream.read((char*)&material.edgeColor, sizeof(material.edgeColor))) return false;
					if (!stream.read((char*)&material.edgeSize, sizeof(material.edgeSize))) return false;
					if (!stream.read((char*)&material.tex, sizeof(material.tex))) return false;
					if (!stream.read((char*)&material.sphere, sizeof(material.sphere))) return false;
					if (!stream.read((char*)&material.toon, sizeof(material.toon))) return false;
				}
			}
		}
	}

	if (!stream.read((char*)&pmx.numDisplayFrames, sizeof(pmx.numDisplayFrames))) return false;

	if (pmx.numDisplayFrames > 0)
	{
		pmx.displayFrames.resize(pmx.numDisplayFrames);

		for (auto& displayFrame : pmx.displayFrames)
		{
			if (!stream.read((char*)&displayFrame.name.length, sizeof(displayFrame.name.length))) return false;
			if (!stream.read((char*)&displayFrame.name.name, displayFrame.name.length)) return false;
			if (!stream.read((char*)&displayFrame.nameEng.length, sizeof(displayFrame.nameEng.length))) return false;
			if (!stream.read((char*)&displayFrame.nameEng.name, displayFrame.nameEng.length)) return false;
			if (!stream.read((char*)&displayFrame.type, sizeof(displayFrame.type))) return false;
			if (!stream.read((char*)&displayFrame.elementsWithinFrame, sizeof(displayFrame.elementsWithinFrame))) return false;

			displayFrame.elements.resize(displayFrame.elementsWithinFrame);
			for (auto& element : displayFrame.elements)
			{
				if (!stream.read((char*)&element.target, sizeof(element.target))) return false;

				if (element.target == 0)
				{
					if (!stream.read((char*)&element.index, pmx.header.sizeOfBone))
						return false;
				}
				else if (element.target == 1)
				{
					if (!stream.read((char*)&element.index, pmx.header.sizeOfMorph))
						return false;
				}
			}
		}
	}

	if (!stream.read((char*)&pmx.numRigidbodys, sizeof(pmx.numRigidbodys))) return false;

	if (pmx.numRigidbodys > 0)
	{
		pmx.rigidbodys.resize(pmx.numRigidbodys);

		for (auto& rigidbody : pmx.rigidbodys)
		{
			if (!stream.read((char*)&rigidbody.name.length, sizeof(rigidbody.name.length))) return false;
			if (!stream.read((char*)&rigidbody.name.name, rigidbody.name.length)) return false;
			if (!stream.read((char*)&rigidbody.nameEng.length, sizeof(rigidbody.nameEng.length))) return false;
			if (!stream.read((char*)&rigidbody.nameEng.name, rigidbody.nameEng.length)) return false;

			if (!stream.read((char*)&rigidbody.bone, pmx.header.sizeOfBone)) return false;
			if (!stream.read((char*)&rigidbody.group, sizeof(rigidbody.group))) return false;
			if (!stream.read((char*)&rigidbody.groupMask, sizeof(rigidbody.groupMask))) return false;

			if (!stream.read((char*)&rigidbody.shape, sizeof(rigidbody.shape))) return false;

			if (!stream.read((char*)&rigidbody.scale, sizeof(rigidbody.scale))) return false;
			if (!stream.read((char*)&rigidbody.position, sizeof(rigidbody.position))) return false;
			if (!stream.read((char*)&rigidbody.rotate, sizeof(rigidbody.rotate))) return false;

			if (!stream.read((char*)&rigidbody.mass, sizeof(rigidbody.mass))) return false;
			if (!stream.read((char*)&rigidbody.movementDecay, sizeof(rigidbody.movementDecay))) return false;
			if (!stream.read((char*)&rigidbody.rotationDecay, sizeof(rigidbody.rotationDecay))) return false;
			if (!stream.read((char*)&rigidbody.elasticity, sizeof(rigidbody.elasticity))) return false;
			if (!stream.read((char*)&rigidbody.friction, sizeof(rigidbody.friction))) return false;
			if (!stream.read((char*)&rigidbody.physicsOperation, sizeof(rigidbody.physicsOperation))) return false;
		}
	}

	if (!stream.read((char*)&pmx.numJoints, sizeof(pmx.numJoints))) return false;

	if (pmx.numJoints > 0)
	{
		pmx.joints.resize(pmx.numJoints);

		for (auto& joint : pmx.joints)
		{
			if (!stream.read((char*)&joint.name.length, sizeof(joint.name.length))) return false;
			if (!stream.read((char*)&joint.name.name, joint.name.length)) return false;
			if (!stream.read((char*)&joint.nameEng.length, sizeof(joint.nameEng.length))) return false;
			if (!stream.read((char*)&joint.nameEng.name, joint.nameEng.length)) return false;

			if (!stream.read((char*)&joint.type, sizeof(joint.type))) return false;

			if (joint.type != 0)
				return false;

			if (!stream.read((char*)&joint.relatedRigidBodyIndexA, pmx.header.sizeOfBody)) return false;
			if (!stream.read((char*)&joint.relatedRigidBodyIndexB, pmx.header.sizeOfBody)) return false;

			if (!stream.read((char*)&joint.position, sizeof(joint.position))) return false;
			if (!stream.read((char*)&joint.rotation, sizeof(joint.rotation))) return false;

			if (!stream.read((char*)&joint.movementLowerLimit, sizeof(joint.movementLowerLimit))) return false;
			if (!stream.read((char*)&joint.movementUpperLimit, sizeof(joint.movementUpperLimit))) return false;

			if (!stream.read((char*)&joint.rotationLowerLimit, sizeof(joint.rotationLowerLimit))) return false;
			if (!stream.read((char*)&joint.rotationUpperLimit, sizeof(joint.rotationUpperLimit))) return false;

			if (!stream.read((char*)&joint.springMovementConstant, sizeof(joint.springMovementConstant))) return false;
			if (!stream.read((char*)&joint.springRotationConstant, sizeof(joint.springRotationConstant))) return false;
		}
	}

	for (auto& it : pmx.materials)
	{
		auto material = std::make_shared<MaterialProperty>();

		if ((it.name.length >> 1) < MAX_PATH)
		{
			char name[MAX_PATH];
			auto length = wcstombs(nullptr, it.name.name, 0);
			if (length < MAX_PATH)
			{
				wcstombs(name, it.name.name, MAX_PATH);

				material->set(MATKEY_NAME, name);
			}
		}

		material->set(MATKEY_COLOR_DIFFUSE, ray::math::srgb2linear(it.Diffuse));
		material->set(MATKEY_COLOR_AMBIENT, ray::math::srgb2linear(it.Ambient));
		material->set(MATKEY_COLOR_SPECULAR, ray::math::srgb2linear(it.Specular));
		material->set(MATKEY_OPACITY, it.Opacity);
		material->set(MATKEY_SHININESS, it.Shininess / 255.0f);

		std::uint32_t limits = 0;
		if (pmx.header.sizeOfTexture == 1)
			limits = std::numeric_limits<PMX_uint8_t>::max();
		else if (pmx.header.sizeOfTexture == 2)
			limits = std::numeric_limits<PMX_uint16_t>::max();
		else if (pmx.header.sizeOfTexture == 4)
			limits = std::numeric_limits<PMX_uint32_t>::max();

		if (it.TextureIndex < limits)
		{
			PMX_Name& texture = pmx.textures[it.TextureIndex];
			if ((texture.length >> 1) < MAX_PATH)
			{
				char name[MAX_PATH];

				wcstombs(name, texture.name, MAX_PATH);

				material->set(MATKEY_TEXTURE_DIFFUSE(0), name);
				material->set(MATKEY_TEXTURE_AMBIENT(0), name);
			}
		}

		if (it.SphereTextureIndex < limits)
		{
			PMX_Name& texture = pmx.textures[it.SphereTextureIndex];
			if ((texture.length >> 1) < MAX_PATH)
			{
				char name[MAX_PATH];

				wcstombs(name, texture.name, MAX_PATH);

				material->set(MATKEY_COLOR_SPHEREMAP(0), name);
			}
		}

		model.addMaterial(std::move(material));
	}

	PMX_Index* indices = pmx.indices.data();

	MeshPropertyPtr root = nullptr;

	for (auto& it : pmx.materials)
	{
		Float3Array vertices;
		Float3Array normals;
		Float2Array texcoords;
		VertexWeights weights;
		UintArray faces;

		for (std::uint32_t i = 0; i < it.FaceCount; i++, indices += pmx.header.sizeOfVertex)
		{
			std::uint32_t index = 0;

			if (pmx.header.sizeOfVertex == 1)
				index = *(std::uint8_t*)indices;
			else if (pmx.header.sizeOfVertex == 2)
				index = *(std::uint16_t*)indices;
			else if (pmx.header.sizeOfVertex == 4)
				index = *(std::uint32_t*)indices;
			else
				return false;

			PMX_Vertex& v = pmx.vertices[index];

			vertices.push_back(v.position);
			normals.push_back(v.normal);
			texcoords.push_back(v.coord);
			faces.push_back(i);

			if (pmx.bones.size() > 1)
			{
				VertexWeight weight;
				weight.weight1 = v.weight.weight1;
				weight.weight2 = v.weight.weight2;
				weight.weight3 = v.weight.weight3;
				weight.weight4 = v.weight.weight4;
				weight.bone1 = v.weight.bone1;
				weight.bone2 = v.weight.bone2;
				weight.bone3 = v.weight.bone3;
				weight.bone4 = v.weight.bone4;

				weights.push_back(weight);
			}
		}

		MeshPropertyPtr mesh = std::make_shared<MeshProperty>();
		mesh->setVertexArray(std::move(vertices));
		mesh->setNormalArray(std::move(normals));
		mesh->setTexcoordArray(std::move(texcoords));
		mesh->setWeightArray(std::move(weights));
		mesh->setFaceArray(std::move(faces));

		if (!root)
			root = mesh;

		model.addMesh(std::move(mesh));
	}

	if (pmx.numBones > 1)
	{
		std::size_t index = 0;
		for (auto& it : pmx.bones)
		{
			char name[MAX_PATH] = { 0 };
			if ((it.name.length) > MAX_PATH)
				return false;

			if (!wcstombs(name, it.name.name, MAX_PATH))
				return false;

			Bone bone;

			bone.setName(name);
			bone.setPosition(it.position);
			bone.setParent(it.Parent);

			model.addBone(std::make_shared<Bone>(bone));

			if (it.Flag & PMX_BONE_IK)
			{
				IKAttr attr;
				attr.boneIndex = index;
				attr.targetBoneIndex = it.IKTargetBoneIndex;
				attr.chainLength = it.IKLinkCount;
				attr.iterations = it.IKLoopCount;

				for (auto& ik : it.IKList)
				{
					IKChild child;
					child.boneIndex = ik.BoneIndex;
					child.angleWeight = RAD_TO_DEG(it.IKLimitedRadian) / 229.1831f;
					child.minimumDegrees = RAD_TO_DEG(ik.minimumRadian);
					child.maximumDegrees = RAD_TO_DEG(ik.maximumRadian);
					child.rotateLimited = ik.rotateLimited;

					attr.child.push_back(child);
				}

				model.addIK(std::make_shared<IKAttr>(attr));
			}

			index++;
		}
	}

	for (auto& it : pmx.rigidbodys)
	{
		char name[MAX_PATH] = { 0 };
		if ((it.name.length) > MAX_PATH)
			return false;

		if (!wcstombs(name, it.name.name, MAX_PATH))
			return false;

		auto body = std::make_shared<RigidbodyProperty>();
		body->name = name;
		body->bone = it.bone;
		body->group = it.group;
		body->groupMask = it.groupMask;
		body->shape = (ShapeType)it.shape;
		body->scale = it.scale;
		body->position = it.position;
		body->rotation = it.rotate;
		body->mass = it.mass;
		body->movementDecay = it.movementDecay;
		body->rotationDecay = it.rotationDecay;
		body->elasticity = it.elasticity;
		body->friction = it.friction;
		body->physicsOperation = it.physicsOperation;

		model.addRigidbody(std::move(body));
	}

	for (auto& it : pmx.joints)
	{
		char name[MAX_PATH] = { 0 };
		if ((it.name.length) > MAX_PATH)
			return false;

		if (!wcstombs(name, it.name.name, MAX_PATH))
			return false;

		auto joint = std::make_shared<JointProperty>();
		joint->name = name;
		joint->bodyIndexA = it.relatedRigidBodyIndexA;
		joint->bodyIndexB = it.relatedRigidBodyIndexB;
		joint->position = it.position;
		joint->rotation = it.rotation;
		joint->movementLowerLimit = it.movementLowerLimit;
		joint->movementUpperLimit = it.movementUpperLimit;
		joint->rotationLowerLimit = it.rotationLowerLimit;
		joint->rotationUpperLimit = it.rotationUpperLimit;
		joint->springMovementConstant = it.springMovementConstant;
		joint->springRotationConstant = it.springRotationConstant;

		model.addJoint(std::move(joint));
	}

	return true;
}

bool
PMXHandler::doSave(Model& model, StreamWrite& stream) noexcept
{
	PMX pmx;

	if (!stream.write((char*)&pmx.header, sizeof(pmx.header))) return false;

	if (!stream.write((char*)&pmx.description.japanModelLength, sizeof(pmx.description.japanModelLength))) return false;
	if (pmx.description.japanModelLength)
		if (!stream.write((char*)&pmx.description.japanModelName[0], pmx.description.japanModelLength)) return false;

	if (!stream.write((char*)&pmx.description.englishModelLength, sizeof(pmx.description.englishModelLength))) return false;
	if (pmx.description.englishModelLength)
		if (!stream.write((char*)&pmx.description.englishModelName[0], pmx.description.englishModelLength)) return false;

	if (!stream.write((char*)&pmx.description.japanCommentLength, sizeof(pmx.description.japanCommentLength))) return false;
	if (pmx.description.japanCommentLength)
		if (!stream.write((char*)&pmx.description.japanCommentName[0], pmx.description.japanCommentLength)) return false;

	if (!stream.write((char*)&pmx.description.englishCommentLength, sizeof(pmx.description.englishCommentLength))) return false;
	if (pmx.description.englishCommentLength)
		if (!stream.write((char*)&pmx.description.englishCommentName[0], pmx.description.englishCommentLength)) return false;

	if (!stream.write((char*)&pmx.numVertices, sizeof(pmx.numVertices))) return false;
	if (pmx.numVertices)
	{
		for (auto& vertex : pmx.vertices)
		{
			if (pmx.header.addUVCount > 0)
			{
				std::streamsize size = sizeof(vertex.position) + sizeof(vertex.normal) + sizeof(vertex.coord) + sizeof(vertex.addCoord[0]) * pmx.header.addUVCount;
				if (!stream.write((char*)&vertex.position, size)) return false;
			}
			else
			{
				std::streamsize size = sizeof(vertex.position) + sizeof(vertex.normal) + sizeof(vertex.coord);
				if (!stream.write((char*)&vertex.position, size)) return false;
			}

			if (!stream.write((char*)&vertex.type, sizeof(vertex.type))) return false;

			switch (vertex.type)
			{
			case PMX_BDEF1:
			{
				if (!stream.write((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
			}
			break;
			case PMX_BDEF2:
			{
				if (!stream.write((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight2))) return false;
			}
			break;
			case PMX_BDEF4:
			{
				if (!stream.write((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&vertex.weight.bone3, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&vertex.weight.bone4, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;
				if (!stream.write((char*)&vertex.weight.weight2, sizeof(vertex.weight.weight2))) return false;
				if (!stream.write((char*)&vertex.weight.weight3, sizeof(vertex.weight.weight3))) return false;
				if (!stream.write((char*)&vertex.weight.weight4, sizeof(vertex.weight.weight4))) return false;
			}
			break;
			case PMX_SDEF:
			{
				if (!stream.write((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;
				if (!stream.write((char*)&vertex.weight.SDEF_C, sizeof(vertex.weight.SDEF_C))) return false;
				if (!stream.write((char*)&vertex.weight.SDEF_R0, sizeof(vertex.weight.SDEF_R0))) return false;
				if (!stream.write((char*)&vertex.weight.SDEF_R1, sizeof(vertex.weight.SDEF_R1))) return false;
			}
			break;
			case PMX_QDEF:
			{
				if (!stream.write((char*)&vertex.weight.bone1, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&vertex.weight.bone2, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&vertex.weight.weight1, sizeof(vertex.weight.weight1))) return false;
			}
			default:
				return false;
			}

			if (!stream.write((char*)&vertex.edge, sizeof(vertex.edge))) return false;
		}
	}

	if (!stream.write((char*)&pmx.numIndices, sizeof(pmx.numIndices))) return false;
	if (pmx.numIndices)
		if (!stream.write((char*)pmx.indices.data(), pmx.indices.size())) return false;

	if (!stream.write((char*)&pmx.numTextures, sizeof(pmx.numTextures))) return false;
	if (pmx.numTextures)
	{
		for (auto& texture : pmx.textures)
		{
			if (!stream.write((char*)&texture.length, sizeof(texture.length))) return false;
			if (!stream.write((char*)&texture.name, texture.length)) return false;
		}
	}

	if (!stream.write((char*)&pmx.numMaterials, sizeof(pmx.numMaterials))) return false;
	if (pmx.numMaterials)
	{
		for (auto& material : pmx.materials)
		{
			if (!stream.write((char*)&material.name.length, sizeof(material.name.length))) return false;

			if (material.name.length)
				if (!stream.write((char*)&material.name.name, material.name.length)) return false;

			if (!stream.write((char*)&material.nameEng.length, sizeof(material.nameEng.length))) return false;

			if (material.nameEng.length)
				if (!stream.write((char*)&material.nameEng.name, material.nameEng.length)) return false;

			if (!stream.write((char*)&material.Diffuse, sizeof(material.Diffuse))) return false;
			if (!stream.write((char*)&material.Opacity, sizeof(material.Opacity))) return false;
			if (!stream.write((char*)&material.Specular, sizeof(material.Specular))) return false;
			if (!stream.write((char*)&material.Shininess, sizeof(material.Shininess))) return false;
			if (!stream.write((char*)&material.Ambient, sizeof(material.Ambient))) return false;
			if (!stream.write((char*)&material.Flag, sizeof(material.Flag))) return false;
			if (!stream.write((char*)&material.EdgeColor, sizeof(material.EdgeColor))) return false;
			if (!stream.write((char*)&material.EdgeSize, sizeof(material.EdgeSize))) return false;
			if (!stream.write((char*)&material.TextureIndex, pmx.header.sizeOfTexture)) return false;
			if (!stream.write((char*)&material.SphereTextureIndex, pmx.header.sizeOfTexture)) return false;
			if (!stream.write((char*)&material.SphereMode, sizeof(material.SphereMode))) return false;
			if (!stream.write((char*)&material.ToonIndex, sizeof(material.ToonIndex))) return false;

			if (material.ToonIndex == 1)
			{
				if (!stream.write((char*)&material.ToneTexture, 1)) return false;
			}
			else
			{
				if (!stream.write((char*)&material.ToneTexture, pmx.header.sizeOfTexture)) return false;
			}

			if (!stream.write((char*)&material.memLength, sizeof(material.memLength))) return false;
			if (material.memLength > 0)
			{
				if (!stream.write((char*)&material.mem, material.memLength)) return false;
			}


			if (!stream.write((char*)&material.FaceCount, sizeof(material.FaceCount))) return false;
		}
	}

	if (!stream.write((char*)&pmx.numBones, sizeof(pmx.numBones))) return false;
	if (pmx.numBones)
	{
		for (auto& bone : pmx.bones)
		{
			if (!stream.write((char*)&bone.name.length, sizeof(bone.name.length))) return false;
			if (bone.name.length)
				if (!stream.write((char*)&bone.name.name, bone.name.length)) return false;
			if (!stream.write((char*)&bone.nameEng.length, sizeof(bone.nameEng.length))) return false;
			if (bone.nameEng.length)
				if (!stream.write((char*)&bone.nameEng.name, bone.nameEng.length)) return false;

			if (!stream.write((char*)&bone.position, sizeof(bone.position))) return false;
			if (!stream.write((char*)&bone.Parent, pmx.header.sizeOfBone)) return false;
			if (!stream.write((char*)&bone.Level, sizeof(bone.Level))) return false;
			if (!stream.write((char*)&bone.Flag, sizeof(bone.Flag))) return false;

			if (bone.Flag & PMX_BONE_INDEX)
			{
				if (!stream.write((char*)&bone.ConnectedBoneIndex, pmx.header.sizeOfBone)) return false;
			}
			else
			{
				if (!stream.write((char*)&bone.Offset, sizeof(bone.Offset))) return false;
			}

			if (bone.Flag & PMX_BONE_PARENT)
			{
				if (!stream.write((char*)&bone.ProvidedParentBoneIndex, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&bone.ProvidedRatio, sizeof(bone.ProvidedRatio))) return false;
			}

			if (bone.Flag & PMX_BONE_AXIS)
			{
				if (!stream.write((char*)&bone.AxisDirection, sizeof(bone.AxisDirection))) return false;
			}

			if (bone.Flag & PMX_BONE_ROTATE)
			{
				if (!stream.write((char*)&bone.DimentionXDirection, sizeof(bone.DimentionXDirection))) return false;
				if (!stream.write((char*)&bone.DimentionZDirection, sizeof(bone.DimentionZDirection))) return false;
			}

			if (bone.Flag & PMX_BONE_IK)
			{
				if (!stream.write((char*)&bone.IKTargetBoneIndex, pmx.header.sizeOfBone)) return false;
				if (!stream.write((char*)&bone.IKLoopCount, sizeof(bone.IKLoopCount))) return false;
				if (!stream.write((char*)&bone.IKLimitedRadian, sizeof(bone.IKLimitedRadian))) return false;
				if (!stream.write((char*)&bone.IKLinkCount, sizeof(bone.IKLinkCount))) return false;

				if (bone.IKLinkCount > 0)
				{
					for (std::size_t j = 0; j < bone.IKLinkCount; j++)
					{
						if (!stream.write((char*)&bone.IKList[j].BoneIndex, pmx.header.sizeOfBone)) return false;
						if (!stream.write((char*)&bone.IKList[j].rotateLimited, (std::streamsize)sizeof(bone.IKList[j].rotateLimited))) return false;
						if (bone.IKList[j].rotateLimited)
						{
							if (!stream.write((char*)&bone.IKList[j].maximumRadian, (std::streamsize)sizeof(bone.IKList[j].maximumRadian))) return false;
							if (!stream.write((char*)&bone.IKList[j].minimumRadian, (std::streamsize)sizeof(bone.IKList[j].minimumRadian))) return false;
						}
					}
				}
			}
		}
	}

	if (!stream.write((char*)&pmx.numMorphs, sizeof(pmx.numMorphs))) return false;
	if (pmx.numMorphs)
	{
		for (auto& morph : pmx.morphs)
		{
			if (morph.name.length)
				if (!stream.write((char*)&morph.name.length, sizeof(morph.name.length))) return false;
			if (!stream.write((char*)&morph.name.name, morph.name.length)) return false;
			if (!stream.write((char*)&morph.nameEng.length, sizeof(morph.nameEng.length))) return false;
			if (morph.nameEng.length)
				if (!stream.write((char*)&morph.nameEng.name, morph.nameEng.length)) return false;
			if (!stream.write((char*)&morph.control, sizeof(morph.control))) return false;
			if (!stream.write((char*)&morph.morphType, sizeof(morph.morphType))) return false;
			if (!stream.write((char*)&morph.morphCount, sizeof(morph.morphCount))) return false;

			if (morph.morphType == MorphType::MorphTypeGroup)
			{
				if (!stream.write((char*)&morph.morphIndex, pmx.header.sizeOfMorph)) return false;
				if (!stream.write((char*)&morph.morphRate, sizeof(morph.morphRate))) return false;
			}
			else if (morph.morphType == MorphType::MorphTypeVertex)
			{
				for (auto& vertex : morph.vertexList)
				{
					if (!stream.write((char*)&vertex.index, pmx.header.sizeOfVertex)) return false;
					if (!stream.write((char*)&vertex.offset, sizeof(vertex.offset))) return false;
				}
			}
			else if (morph.morphType == MorphType::MorphTypeBone)
			{
				for (auto& bone : morph.boneList)
				{
					if (!stream.write((char*)&bone.boneIndex, pmx.header.sizeOfBone)) return false;
					if (!stream.write((char*)&bone.position, sizeof(bone.position))) return false;
					if (!stream.write((char*)&bone.rotate, sizeof(bone.rotate))) return false;
				}
			}
			else if (morph.morphType == MorphType::MorphTypeUV || morph.morphType == MorphType::MorphTypeExtraUV1 ||
				morph.morphType == MorphType::MorphTypeExtraUV2 || morph.morphType == MorphType::MorphTypeExtraUV3 ||
				morph.morphType == MorphType::MorphTypeExtraUV4)
			{
				for (auto& texcoord : morph.texcoordList)
				{
					if (!stream.write((char*)&texcoord.index, pmx.header.sizeOfVertex)) return false;
					if (!stream.write((char*)&texcoord.offset, sizeof(texcoord.offset))) return false;
				}
			}
			else if (morph.morphType == MorphType::MorphTypeMaterial)
			{
				for (auto& material : morph.materialList)
				{
					if (!stream.write((char*)&material.index, pmx.header.sizeOfMaterial)) return false;
					if (!stream.write((char*)&material.offset, sizeof(material.offset))) return false;
					if (!stream.write((char*)&material.diffuse, sizeof(material.diffuse))) return false;
					if (!stream.write((char*)&material.specular, sizeof(material.specular))) return false;
					if (!stream.write((char*)&material.shininess, sizeof(material.shininess))) return false;
					if (!stream.write((char*)&material.ambient, sizeof(material.ambient))) return false;
					if (!stream.write((char*)&material.edgeColor, sizeof(material.edgeColor))) return false;
					if (!stream.write((char*)&material.edgeSize, sizeof(material.edgeSize))) return false;
					if (!stream.write((char*)&material.tex, sizeof(material.tex))) return false;
					if (!stream.write((char*)&material.sphere, sizeof(material.sphere))) return false;
					if (!stream.write((char*)&material.toon, sizeof(material.toon))) return false;
				}
			}
		}
	}

	if (!stream.write((char*)&pmx.numDisplayFrames, sizeof(pmx.numDisplayFrames))) return false;
	if (pmx.numDisplayFrames)
	{
		for (auto& displayFrame : pmx.displayFrames)
		{
			if (!stream.write((char*)&displayFrame.name.length, sizeof(displayFrame.name.length))) return false;
			if (displayFrame.name.length)
				if (!stream.write((char*)&displayFrame.name.name, displayFrame.name.length)) return false;
			if (!stream.write((char*)&displayFrame.nameEng.length, sizeof(displayFrame.nameEng.length))) return false;
			if (displayFrame.nameEng.length)
				if (!stream.write((char*)&displayFrame.nameEng.name, displayFrame.nameEng.length)) return false;
			if (!stream.write((char*)&displayFrame.type, sizeof(displayFrame.type))) return false;
			if (!stream.write((char*)&displayFrame.elementsWithinFrame, sizeof(displayFrame.elementsWithinFrame))) return false;

			for (auto& element : displayFrame.elements)
			{
				if (!stream.write((char*)&element.target, sizeof(element.target))) return false;

				if (element.target == 0)
				{
					if (!stream.write((char*)&element.index, pmx.header.sizeOfBone))
						return false;
				}
				else if (element.target == 1)
				{
					if (!stream.write((char*)&element.index, pmx.header.sizeOfMorph))
						return false;
				}
			}
		}
	}

	if (!stream.write((char*)&pmx.numRigidbodys, sizeof(pmx.numRigidbodys))) return false;
	if (pmx.numRigidbodys)
	{
		for (auto& rigidbody : pmx.rigidbodys)
		{
			if (!stream.write((char*)&rigidbody.name.length, sizeof(rigidbody.name.length))) return false;
			if (rigidbody.name.length)
				if (!stream.write((char*)&rigidbody.name.name, rigidbody.name.length)) return false;
			if (!stream.write((char*)&rigidbody.nameEng.length, sizeof(rigidbody.nameEng.length))) return false;
			if (rigidbody.nameEng.length)
				if (!stream.write((char*)&rigidbody.nameEng.name, rigidbody.nameEng.length)) return false;

			if (!stream.write((char*)&rigidbody.bone, pmx.header.sizeOfBone)) return false;
			if (!stream.write((char*)&rigidbody.group, sizeof(rigidbody.group))) return false;
			if (!stream.write((char*)&rigidbody.groupMask, sizeof(rigidbody.groupMask))) return false;

			if (!stream.write((char*)&rigidbody.shape, sizeof(rigidbody.shape))) return false;

			if (!stream.write((char*)&rigidbody.scale, sizeof(rigidbody.scale))) return false;
			if (!stream.write((char*)&rigidbody.position, sizeof(rigidbody.position))) return false;
			if (!stream.write((char*)&rigidbody.rotate, sizeof(rigidbody.rotate))) return false;

			if (!stream.write((char*)&rigidbody.mass, sizeof(rigidbody.mass))) return false;
			if (!stream.write((char*)&rigidbody.movementDecay, sizeof(rigidbody.movementDecay))) return false;
			if (!stream.write((char*)&rigidbody.rotationDecay, sizeof(rigidbody.rotationDecay))) return false;
			if (!stream.write((char*)&rigidbody.elasticity, sizeof(rigidbody.elasticity))) return false;
			if (!stream.write((char*)&rigidbody.friction, sizeof(rigidbody.friction))) return false;
			if (!stream.write((char*)&rigidbody.physicsOperation, sizeof(rigidbody.physicsOperation))) return false;
		}
	}

	if (!stream.write((char*)&pmx.numJoints, sizeof(pmx.numJoints))) return false;
	if (pmx.numJoints)
	{
		for (auto& joint : pmx.joints)
		{
			if (!stream.write((char*)&joint.name.length, sizeof(joint.name.length))) return false;
			if (joint.name.length)
				if (!stream.write((char*)&joint.name.name, joint.name.length)) return false;
			if (!stream.write((char*)&joint.nameEng.length, sizeof(joint.nameEng.length))) return false;
			if (joint.nameEng.length)
				if (!stream.write((char*)&joint.nameEng.name, joint.nameEng.length)) return false;

			if (!stream.write((char*)&joint.type, sizeof(joint.type))) return false;

			if (joint.type != 0)
				return false;

			if (!stream.write((char*)&joint.relatedRigidBodyIndexA, pmx.header.sizeOfBody)) return false;
			if (!stream.write((char*)&joint.relatedRigidBodyIndexB, pmx.header.sizeOfBody)) return false;

			if (!stream.write((char*)&joint.position, sizeof(joint.position))) return false;
			if (!stream.write((char*)&joint.rotation, sizeof(joint.rotation))) return false;

			if (!stream.write((char*)&joint.movementLowerLimit, sizeof(joint.movementLowerLimit))) return false;
			if (!stream.write((char*)&joint.movementUpperLimit, sizeof(joint.movementUpperLimit))) return false;

			if (!stream.write((char*)&joint.rotationLowerLimit, sizeof(joint.rotationLowerLimit))) return false;
			if (!stream.write((char*)&joint.rotationUpperLimit, sizeof(joint.rotationUpperLimit))) return false;

			if (!stream.write((char*)&joint.springMovementConstant, sizeof(joint.springMovementConstant))) return false;
			if (!stream.write((char*)&joint.springRotationConstant, sizeof(joint.springRotationConstant))) return false;
		}
	}

	return true;
}

_NAME_END