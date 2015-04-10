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
#ifndef _H_MODEL_PMX_H_
#define _H_MODEL_PMX_H_

#include <ray/model.h>

_NAME_BEGIN

#pragma pack(push)
#pragma pack(push,1)

enum
{
    PMX_BDEF1,
    PMX_BDEF2,
    PMX_BDEF4,
    PMX_SDEF,
    PMX_QDEF
};

#define PMX_BONE_INDEX    1 << 0
#define PMX_BONE_MOVE     1 << 1
#define PMX_BONE_IK       1 << 5
#define PMX_BONE_PARENT   1 << 8
#define PMX_BONE_AXIS     1 << 9
#define PMX_BONE_ROTATE   1 << 11

typedef Vector2 PMX_Vector2;
typedef Vector3 PMX_Vector3;
typedef Vector4 PMX_Vector4;

typedef Color3 PMX_Color3;
typedef Color4 PMX_Color4;

typedef char          PMX_char;
typedef std::int8_t   PMX_int8_t;
typedef std::uint8_t  PMX_uint8_t;
typedef std::uint16_t PMX_uint16_t;
typedef std::uint32_t PMX_uint32_t;

typedef float PMX_Float;

struct PMX_Header
{
    PMX_uint8_t Magic[3];  // 始终为PMX

    PMX_uint8_t Offset;    // 始终为 0x20

    PMX_Float   Version;   // 版本

    PMX_uint8_t DataSize;  // 始终 0x08

    PMX_uint8_t Encode;    // 0x00 UTF-16(LE) 0x01 UTF-8

    PMX_uint8_t UVCount;   // 追加的UV ( 1 ~ 4 )

    PMX_uint8_t VertexIndexSize;   // ( 1 or 2 or 4 )

    PMX_uint8_t TextureIndexSize;  // ( 1 or 2 or 4 )

    PMX_uint8_t MaterialIndexSize; // ( 1 or 2 or 4 )

    PMX_uint8_t BoneIndexSize;     // ( 1 or 2 or 4 )

    PMX_uint8_t MorphIndexSize;    // ( 1 or 2 or 4 )

    PMX_uint8_t BodyIndexSize;     // ( 1 or 2 or 4 )
};

struct PMX_Description
{
    PMX_uint32_t JapanModelLength;        // 模型名长度

    std::vector<PMX_char> JapanModelName;     // 模型名称

    PMX_uint32_t EnglishModelLength;      // 模型名长度

    std::vector<PMX_char> EnglishModelName;   // 模型名称

    PMX_uint32_t JapanCommentLength;      // 概要长度

    std::vector<PMX_char> JapanCommentName;   // 模型说明

    PMX_uint32_t EnglishCommentLength;    // 概要长度

    std::vector<PMX_char> EnglishCommentName; // 模型说明
};

struct PMX_BoneWeight
{
    PMX_uint16_t Bone1;    // 骨骼索引

    PMX_uint16_t Bone2;    // 骨骼索引

    PMX_uint16_t Bone3;    // 骨骼索引

    PMX_uint16_t Bone4;    // 骨骼索引

    PMX_Float Weight1;     // 权重

    PMX_Float Weight2;     // 权重

    PMX_Float Weight3;     // 权重

    PMX_Float Weight4;     // 权重

    PMX_Vector3 SDEF_C;

    PMX_Vector3 SDEF_R0;

    PMX_Vector3 SDEF_R1;
};

typedef PMX_uint32_t PMX_VertexCount;

struct PMX_Vertex
{
    PMX_Vector3 Position; // 顶点坐标

    PMX_Vector3 Normal;   // 顶点法线

    PMX_Vector2 UV;       // 顶点贴图坐标

    PMX_Vector4 addUV;    // 附加纹理坐标

    PMX_uint8_t Type;     // 变形方式 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF

    PMX_BoneWeight BoneWeight; // 骨骼

    PMX_Float   edge;      // 倍率
};

typedef PMX_uint32_t PMX_IndexCount;

typedef PMX_uint32_t PMX_Index; //顶点索引

typedef PMX_uint32_t PMX_TextureCount;

struct PMX_Name
{
    PMX_uint32_t length;

    std::wstring name;
};

typedef PMX_Name PMX_Texture;
typedef PMX_uint32_t PMX_MaterialCount;

struct PMX_Material
{
    PMX_Name    Name;

    PMX_Name    EnglishName;

    PMX_Color3  Diffuse;

    PMX_Float   Opacity;

    PMX_Color3  Specular;

    PMX_Float   Shininess;

    PMX_Color3  Ambient;

    PMX_uint8_t Flag;

    PMX_Vector4 EdgeColor;

    PMX_Float   EdgeSize;

    PMX_uint8_t TextureIndex;

    PMX_uint8_t SphereTextureIndex;

    PMX_uint16_t ToonIndex;

    PMX_uint8_t ToonFlag;

    PMX_uint32_t SphereMode;

    PMX_uint32_t FaceVertexCount;
};

typedef PMX_uint32_t PMX_BoneCount;

struct PMX_IK
{
    PMX_uint8_t BoneIndex;

    PMX_uint8_t RotateLimited;

    PMX_Vector3 MinimumRadian;

    PMX_Vector3 MaximumRadian;
};

struct PMX_Bone
{
    PMX_Name    Name;

    PMX_Name    EnglishName;

    PMX_Vector3 Position;

    PMX_int8_t  Parent;

    PMX_uint32_t Level;

    PMX_uint16_t Flag;

    PMX_uint8_t ConnectedBoneIndex;

    PMX_Vector3 Offset;

    PMX_uint8_t ProvidedParentBoneIndex;

    PMX_Float  ProvidedRatio;

    PMX_Vector3 AxisDirection;

    PMX_Vector3 DimentionXDirection;

    PMX_Vector3 DimentionZDirection;

    PMX_uint32_t KeyValue;

    PMX_uint8_t IKTargetBoneIndex;

    PMX_uint32_t IKLoopCount;

    PMX_Float IKLimitedRadian;

    PMX_uint32_t IKLinkCount;

    std::vector<PMX_IK> IKList;
};

struct PMX
{
    PMX_Header Header;

    PMX_Description Description;

    PMX_VertexCount VertexCount;

    std::vector<PMX_Vertex> VertexList;

    PMX_IndexCount IndexCount;

    std::vector<PMX_Index> IndexList;

    PMX_TextureCount TextureCount;

    std::vector<PMX_Texture> TextureList;

    PMX_MaterialCount MaterialCount;

    std::vector<PMX_Material> MaterialList;

    PMX_BoneCount BoneCount;

    std::vector<PMX_Bone> BoneList;
};

#pragma pack(pop)

class PMXHandler : public ModelHandler
{
public:
    virtual bool doCanRead(istream& stream) const noexcept;

    virtual bool doLoad(Model& model, istream& stream) noexcept;
    virtual bool doSave(Model& model, ostream& stream) noexcept;
};

_NAME_END

#endif
