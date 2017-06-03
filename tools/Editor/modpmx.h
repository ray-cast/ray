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
#ifndef _H_MODEL_PMX_H_
#define _H_MODEL_PMX_H_

#include <ray/math.h>
#include <ray/fstream.h>

_NAME_BEGIN

#define PMX_VERSION_2_0   2.0
#define PMX_BONE_INDEX    1 << 0
#define PMX_BONE_MOVE     1 << 1
#define PMX_BONE_DISPLAY  1 << 2
#define PMX_BONE_OPERATOR 1 << 3
#define PMX_BONE_ROOT     1 << 4
#define PMX_BONE_IK       1 << 5
#define PMX_BONE_PARENT   1 << 8
#define PMX_BONE_AXIS     1 << 10
#define PMX_BONE_ROTATE   1 << 11

#define PMX_MATERIAL_DEFAULT 30

typedef Vector2 PMX_Vector2;
typedef Vector3 PMX_Vector3;
typedef Vector4 PMX_Vector4;

typedef Vector3 PMX_Color3;
typedef Vector4 PMX_Color4;
typedef float3x3 PMX_Float3x3;
typedef float4x4 PMX_Float4x4;

typedef wchar_t       PMX_char; // unicode encoding
typedef std::int8_t   PMX_int8_t;
typedef std::uint8_t  PMX_uint8_t;
typedef std::uint16_t PMX_uint16_t;
typedef std::uint32_t PMX_uint32_t;

typedef float PMX_Float;

enum PMXBone
{
	PMX_BDEF1,
	PMX_BDEF2,
	PMX_BDEF4,
	PMX_SDEF,
	PMX_QDEF
};

enum PMXMorphType
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

enum PMXShapeType
{
	ShapeTypeTriMesh,
	ShapeTypeRope
};

enum PMXShapeFlags
{
	ShapeFlagsB_Link,
	ShapeFlags_ClusterCreation,
	ShapeFlagsLinkCrossing
};

#pragma pack(push)
#pragma pack(push,1)

struct PMX_Header
{
	PMX_uint8_t magic[3];
	PMX_uint8_t offset;
	PMX_Float   version;
	PMX_uint8_t dataSize;
	PMX_uint8_t encode;
	PMX_uint8_t addUVCount;
	PMX_uint8_t sizeOfIndices;
	PMX_uint8_t sizeOfTexture;
	PMX_uint8_t sizeOfMaterial;
	PMX_uint8_t sizeOfBone;
	PMX_uint8_t sizeOfMorph;
	PMX_uint8_t sizeOfBody;
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
	PMX_uint8_t type;
	PMX_BoneWeight weight;
	PMX_Float   edge;
};

typedef PMX_uint8_t PMX_Index;

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
	PMX_uint32_t IndicesCount;
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
	PMX() noexcept;

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
	PMX_uint32_t numSoftBodies;

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

class PMXHandler final
{
public:
	PMXHandler() noexcept;
	~PMXHandler() noexcept;

	bool doCanRead(StreamReader& stream) const noexcept;

	bool doLoad(const std::string& path, PMX& pmx, std::string& error) noexcept;
	bool doLoad(StreamReader& stream, PMX& pmx) noexcept;

	bool doSave(StreamWrite& stream, const PMX& pmx) noexcept;
	bool doSave(const std::string& path, const PMX& pmx, std::string& error) noexcept;

private:
	PMXHandler(const PMXHandler&) = delete;
	PMXHandler& operator=(const PMXHandler&) = delete;
};

_NAME_END

#endif
