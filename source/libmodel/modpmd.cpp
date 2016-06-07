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
#include "modpmd.h"
#include <iconv.h>

_NAME_BEGIN

#define BONE_TYPE_ROTATE      0
#define BONE_TYPE_ROTATE_MOVE 1
#define BONE_TYPE_IK          2
#define BONE_TYPE_UNKNOWN     3
#define BONE_TYPE_LINK_HEAD   6
#define BONE_TYPE_NONE        7

#define PMD_NUM_TOON 10

enum Tex_Type
{
	TEXFLAG_NONE,
	TEXFLAG_TEXTURE,
	TEXFLAG_MAP,
	TEXFLAG_ADD
};

#pragma pack(push)
#pragma pack(push,1)

typedef Vector2 PMD_Vector2;
typedef Vector3 PMD_Vector3;
typedef Vector4 PMD_Vector4;

typedef Vector3 PMD_Color3;
typedef Vector4 PMD_Color4;

typedef char          PMD_char;
typedef std::int8_t   PMD_int8_t;
typedef std::uint8_t  PMD_uint8_t;
typedef std::uint16_t PMD_uint16_t;
typedef std::uint32_t PMD_uint32_t;

typedef float PMD_Float;

struct PMD_Header
{
	PMD_uint8_t Magic[3];      // 始终为Pmd

	PMD_Float   Version;       // 版本 1.0f

	PMD_uint8_t ModelName[20]; // 模型名称

	PMD_uint8_t Comment[256];  // 模型说明
};

struct PMD_BoneIndex
{
	PMD_uint16_t Bone1; // 骨骼1

	PMD_uint16_t Bone2; // 骨骼2
};

typedef PMD_uint32_t PMD_VertexCount;

struct PMD_Vertex
{
	PMD_Vector3 Position;     // 顶点坐标

	PMD_Vector3 Normal;       // 顶点法线

	PMD_Vector2 UV;           // 顶点贴图坐标

	PMD_BoneIndex Bone;       // 骨骼索引

	PMD_uint8_t  Weight;      // 权重

	PMD_uint8_t  NonEdgeFlag; // 无边标志（说明：当这个字节不为0时NonEdgeFlag=true）
};

typedef PMD_uint32_t PMD_IndexCount;

typedef PMD_uint16_t PMD_Index; //顶点索引

typedef PMD_uint32_t PMD_MaterialCount;

struct PMD_Material
{
	PMD_Color3   Diffuse;         // 漫射光

	PMD_Float    Opacity;         // 不透明度

	PMD_Float    Shininess;       // 发光度

	PMD_Color3   Specular;        // 镜面光

	PMD_Color3   Ambient;         // 环境光

	PMD_uint8_t  ToonIndex;       // 有符号，卡通着色纹理编号

	PMD_uint8_t  Edge;            // 是否带边，当该字节 > 0时为true

	PMD_uint32_t FaceVertexCount; // 顶点索引数

	PMD_char     TextureName[20]; // 包含了纹理名称，使用*分割  枚举：NONE、ADD、MUL
};

struct PMD_BoneName
{
	PMD_uint8_t Name[20];
};

typedef PMD_uint16_t PMD_BoneCount;

struct PMD_Bone
{
	PMD_BoneName Name;     // 骨骼名字

	PMD_uint16_t Parent;   // 父亲骨骼

	PMD_uint16_t Child;    // 连接到

	PMD_uint8_t  Type;     // 类型（枚举：Rotate,RotateMove,IK, Unknown, IKLink, RotateEffect, IKTo, Unvisible, Twist,RotateRatio）

	PMD_uint16_t IKCount;  // IK（反向运动学）数值

	PMD_Vector3  Position; // 骨骼位置
};

typedef PMD_uint16_t PMD_Link;
typedef PMD_uint16_t PMD_IKCount;

struct PMD_IK
{
	PMD_uint16_t IK;

	PMD_uint16_t Target;

	PMD_uint8_t  LinkCount;

	PMD_uint16_t LoopCount;

	PMD_Float    Weight;

	std::vector<PMD_Link> LinkList; // LinkList[LinkCount];
};

struct PMD_MorphVertex
{
	PMD_uint32_t Index;    // 皮肤对应的组成顶点

	PMD_Vector3  Offset;   // 顶点位移
};

struct PMD_MorphName
{
	PMD_uint8_t Name[20];
};

typedef PMD_uint16_t PMD_MorphCount;

struct PMD_Morph
{
	PMD_MorphName Name;        // 表情名字

	PMD_uint32_t VertexCount; // 表情含有的顶点数量

	PMD_uint8_t  Category;    // 分类

	std::vector<PMD_MorphVertex>  VertexList;    // FaceVertex[VertexCount];  // 表情包含的所有顶点信息
};

struct PMD_NodeName
{
	PMD_uint8_t Name[50];
};

struct PMD_BoneToNode
{
	PMD_uint16_t Bone;

	PMD_uint8_t  Node;
};

typedef PMD_uint16_t PMD_Expression;

struct PMD_FrameWindow
{
	PMD_uint8_t                 ExpressionListCount;

	std::vector<PMD_Expression> ExpressionList;   // ExpressionList[ExpressionListCount]; // 读取后减1，对应皮肤数量

	PMD_uint8_t                 NodeNameCount;

	std::vector<PMD_NodeName>   NodeNameList;     // NodeNameList[NodeNameCount];

	PMD_uint32_t                BoneToNodeCount;

	std::vector<PMD_BoneToNode> BoneToNodeList;   // BoneToNodeList[BoneToNodeCount];
};

struct PMD_ToonName
{
	PMD_uint8_t Name[20];
};

struct PMD_Description
{
	PMD_uint8_t ModelName[20];

	PMD_uint8_t Comment[256];

	std::vector<PMD_BoneName> BoneName;  // Name[BoneCount];

	std::vector<PMD_MorphName> FaceName;  // Name[PMD_FrameWindow.ExpressionListCount];

	std::vector<PMD_NodeName> FrameName; // Name[PMD_FrameWindow.NodeNameCount];
};

typedef PMD_uint16_t PMD_ToonCount;

struct PMD_Toon
{
	PMD_uint8_t Name[100];
};

typedef PMD_uint32_t PMD_BodyCount;

struct PMD_Body
{
	PMD_uint8_t  Name[20];

	PMD_uint16_t bone;

	PMD_uint8_t group;
	PMD_uint16_t groupMask;
	PMD_uint8_t shape; // 0:Circle 1:Square 2:Capsule

	PMD_Vector3 scale;
	PMD_Vector3 position;
	PMD_Vector3 rotate;

	PMD_Float mass;
	PMD_Float movementDecay; // movement reduction
	PMD_Float rotationDecay; // rotation reduction
	PMD_Float elasticity;    // recoil
	PMD_Float friction;      // strength of friction

	PMD_uint8_t physicsOperation; //0:Follow Bone (static), 1:Physics Calc. (dynamic), 2: Physics Calc. + Bone position matching
};

typedef PMD_uint32_t PMD_JointCount;

struct PMD_Joint
{
	PMD_uint8_t  Name[20];

	PMD_uint32_t relatedRigidBodyIndexA; //-1 if irrelevant
	PMD_uint32_t relatedRigidBodyIndexB;

	PMD_Vector3 position;
	PMD_Vector3 rotation;

	PMD_Vector3 movementLowerLimit;
	PMD_Vector3 movementUpperLimit;

	PMD_Vector3 rotationLowerLimit; //-> radian angle
	PMD_Vector3 rotationUpperLimit; //-> radian angle

	PMD_Vector3 springMovementConstant;
	PMD_Vector3 springRotationConstant;
};

#pragma pack(pop)

struct PMD
{
	PMD_Header                Header;

	PMD_VertexCount           VertexCount;

	std::vector<PMD_Vertex>   VertexList;

	PMD_IndexCount            IndexCount;

	std::vector<PMD_Index>    IndexList;

	PMD_MaterialCount         MaterialCount;

	std::vector<PMD_Material> MaterialList;

	PMD_BoneCount             BoneCount;

	std::vector<PMD_Bone>     BoneList;

	PMD_IKCount               IkCount;

	std::vector<PMD_IK>       IkList;

	PMD_MorphCount            MorphCount;

	std::vector<PMD_Morph>    MorphList;

	PMD_FrameWindow           FrameWindow;

	PMD_uint8_t               HasDescription;

	PMD_Description           Description;

	PMD_ToonCount             ToonCount;

	std::vector<PMD_Toon>     ToonList;

	PMD_BodyCount             BodyCount;

	std::vector<PMD_Body>     rigidbodys;

	PMD_JointCount            JointCount;

	std::vector<PMD_Joint>    joints;
};

PMDHandler::PMDHandler() noexcept
{
}

PMDHandler::~PMDHandler() noexcept
{
}

bool
PMDHandler::doCanRead(StreamReader& stream) const noexcept
{
	PMD_Header hdr;

	if (stream.read((char*)&hdr, sizeof(hdr)))
	{
		if (hdr.Magic[0] == 'p' || hdr.Magic[0] == 'P' &&
			hdr.Magic[1] == 'm' || hdr.Magic[1] == 'M' &&
			hdr.Magic[2] == 'd' || hdr.Magic[2] == 'D')
		{
			if (hdr.Version == 1.0f)
			{
				return true;
			}
		}
	}

	return false;
}

bool
PMDHandler::doLoad(Model& model, StreamReader& stream) noexcept
{
	PMD pmd;
	if (!stream.read((char*)&pmd.Header, sizeof(pmd.Header))) return false;

	// vertex
	if (!stream.read((char*)&pmd.VertexCount, sizeof(pmd.VertexCount))) return false;

	if (pmd.VertexCount > 0)
	{
		pmd.VertexList.resize(pmd.VertexCount);

		if (!stream.read((char*)&pmd.VertexList[0], (std::streamsize)(sizeof(PMD_Vertex)* pmd.VertexCount))) return false;
	}

	// index
	if (!stream.read((char*)&pmd.IndexCount, sizeof(pmd.IndexCount))) return false;

	if (pmd.IndexCount > 0)
	{
		pmd.IndexList.resize(pmd.IndexCount);

		if (!stream.read((char*)&pmd.IndexList[0], (std::streamsize)(sizeof(PMD_Index)* pmd.IndexCount))) return false;
	}

	// materal
	if (!stream.read((char*)&pmd.MaterialCount, sizeof(pmd.MaterialCount))) return false;

	if (pmd.MaterialCount > 0)
	{
		pmd.MaterialList.resize(pmd.MaterialCount);

		if (!stream.read((char*)&pmd.MaterialList[0], (std::streamsize)(sizeof(PMD_Material)* pmd.MaterialCount))) return false;
	}

	// bone
	if (!stream.read((char*)&pmd.BoneCount, sizeof(pmd.BoneCount))) return false;

	if (pmd.BoneCount > 0)
	{
		pmd.BoneList.resize(pmd.BoneCount);

		if (!stream.read((char*)&pmd.BoneList[0], (std::streamsize)(sizeof(PMD_Bone)* pmd.BoneCount))) return false;
	}

	// IK
	if (!stream.read((char*)&pmd.IkCount, sizeof(pmd.IkCount))) return false;

	if (pmd.IkCount > 0)
	{
		pmd.IkList.resize(pmd.IkCount);

		for (std::size_t i = 0; i < (std::size_t)pmd.IkCount; i++)
		{
			if (!stream.read((char*)&pmd.IkList[i].IK, sizeof(pmd.IkList[i].IK))) return false;
			if (!stream.read((char*)&pmd.IkList[i].Target, sizeof(pmd.IkList[i].Target))) return false;
			if (!stream.read((char*)&pmd.IkList[i].LinkCount, sizeof(pmd.IkList[i].LinkCount))) return false;
			if (!stream.read((char*)&pmd.IkList[i].LoopCount, sizeof(pmd.IkList[i].LoopCount))) return false;
			if (!stream.read((char*)&pmd.IkList[i].Weight, sizeof(pmd.IkList[i].Weight))) return false;

			pmd.IkList[i].LinkList.resize(pmd.IkList[i].LinkCount);

			if (!stream.read((char*)&pmd.IkList[i].LinkList[0], (std::streamsize)(sizeof(PMD_Link)* pmd.IkList[i].LinkCount))) return false;
		}
	}

	// Morph
	if (!stream.read((char*)&pmd.MorphCount, sizeof(pmd.MorphCount))) return false;

	if (pmd.MorphCount > 0)
	{
		pmd.MorphList.resize(pmd.MorphCount);

		for (std::size_t i = 0; i < (std::size_t)pmd.MorphCount; i++)
		{
			if (!stream.read((char*)&pmd.MorphList[i].Name, sizeof(pmd.MorphList[i].Name))) return false;
			if (!stream.read((char*)&pmd.MorphList[i].VertexCount, sizeof(pmd.MorphList[i].VertexCount))) return false;
			if (!stream.read((char*)&pmd.MorphList[i].Category, sizeof(pmd.MorphList[i].Category))) return false;

			if (pmd.MorphList[i].VertexCount > 0)
			{
				pmd.MorphList[i].VertexList.resize(pmd.MorphList[i].VertexCount);

				if (!stream.read((char*)&pmd.MorphList[i].VertexList[0], (std::streamsize)(sizeof(PMD_MorphVertex)* pmd.MorphList[i].VertexCount))) return false;
			}
		}
	}

	// frame window
	if (!stream.read((char*)&pmd.FrameWindow.ExpressionListCount, sizeof(pmd.FrameWindow.ExpressionListCount))) return false;

	if (pmd.FrameWindow.ExpressionListCount > 0)
	{
		pmd.FrameWindow.ExpressionList.resize(pmd.FrameWindow.ExpressionListCount);

		if (!stream.read((char*)&pmd.FrameWindow.ExpressionList[0], (std::streamsize)(sizeof(PMD_Expression)* pmd.FrameWindow.ExpressionListCount))) return false;
	}

	if (!stream.read((char*)&pmd.FrameWindow.NodeNameCount, sizeof(pmd.FrameWindow.NodeNameCount))) return false;

	if (pmd.FrameWindow.NodeNameCount > 0)
	{
		pmd.FrameWindow.NodeNameList.resize(pmd.FrameWindow.NodeNameCount);

		if (!stream.read((char*)&pmd.FrameWindow.NodeNameList[0].Name, (std::streamsize)(sizeof(PMD_NodeName)* pmd.FrameWindow.NodeNameCount))) return false;
	}

	if (!stream.read((char*)&pmd.FrameWindow.BoneToNodeCount, sizeof(pmd.FrameWindow.BoneToNodeCount))) return false;

	if (pmd.FrameWindow.BoneToNodeCount > 0)
	{
		pmd.FrameWindow.BoneToNodeList.resize(pmd.FrameWindow.BoneToNodeCount);

		if (!stream.read((char*)&pmd.FrameWindow.BoneToNodeList[0].Bone, (std::streamsize)(sizeof(PMD_BoneToNode)* pmd.FrameWindow.BoneToNodeCount))) return false;
	}

	// description
	if (!stream.read((char*)&pmd.HasDescription, sizeof(pmd.HasDescription))) return false;

	if (pmd.HasDescription)
	{
		if (!stream.read((char*)&pmd.Description.ModelName, sizeof(pmd.Description.ModelName))) return false;

		if (!stream.read((char*)&pmd.Description.Comment, sizeof(pmd.Description.Comment))) return false;

		for (PMD_BoneCount i = 0; i < pmd.BoneCount; i++)
		{
			PMD_BoneName name;

			if (!stream.read((char*)&name.Name, sizeof(name))) return false;

			pmd.Description.BoneName.push_back(name);
		}

		for (PMD_uint8_t i = 0; i < pmd.FrameWindow.ExpressionListCount; i++)
		{
			PMD_MorphName name;

			if (!stream.read((char*)&name.Name, sizeof(name))) return false;

			pmd.Description.FaceName.push_back(name);
		}

		for (PMD_uint8_t i = 0; i < pmd.FrameWindow.NodeNameCount; i++)
		{
			PMD_NodeName name;

			if (!stream.read((char*)&name.Name, sizeof(name))) return false;

			pmd.Description.FrameName.push_back(name);
		}
	}

	// toon
	pmd.ToonCount = PMD_NUM_TOON;

	pmd.ToonList.resize(pmd.ToonCount);

	if (!stream.read((char*)&pmd.ToonList[0].Name, (std::streamsize)(sizeof(PMD_Toon) * pmd.ToonCount))) return false;

	// rigidbody
	if (!stream.read((char*)&pmd.BodyCount, sizeof(pmd.BodyCount))) return false;

	if (pmd.BodyCount > 0)
	{
		pmd.rigidbodys.resize(pmd.BodyCount);

		if (!stream.read((char*)&pmd.rigidbodys[0], (std::streamsize)(sizeof(PMD_Body)* pmd.BodyCount))) return false;
	}

	// joint
	if (!stream.read((char*)&pmd.JointCount, sizeof(pmd.JointCount))) return false;

	if (pmd.JointCount > 0)
	{
		pmd.joints.resize(pmd.JointCount);

		if (!stream.read((char*)&pmd.joints[0], (std::streamsize)(sizeof(PMD_Joint) * pmd.JointCount))) return false;
	}

	for (std::size_t index = 0; index < pmd.MaterialList.size(); index++)
	{
		auto& it = pmd.MaterialList[index];

		auto material = std::make_shared<MaterialProperty>();
		material->set(MATKEY_COLOR_DIFFUSE, it.Diffuse);
		material->set(MATKEY_COLOR_AMBIENT, it.Ambient);
		material->set(MATKEY_COLOR_SPECULAR, it.Specular);
		material->set(MATKEY_OPACITY, it.Opacity);
		material->set(MATKEY_SHININESS, it.Shininess / 256.0f);

		std::string name = it.TextureName;
		std::string::size_type substr = name.find_first_of("*");
		if (substr != std::string::npos)
		{
			name.erase(name.begin() + substr, name.end());
		}

		material->set(MATKEY_TEXTURE_DIFFUSE(0), name);
		material->set(MATKEY_TEXTURE_AMBIENT(0), name);

		model.addMaterial(material);
	}

	PMD_Index* indices = pmd.IndexList.data();

	MeshPropertyPtr root = nullptr;

	for (auto& it : pmd.MaterialList)
	{
		Float3Array vertices;
		Float3Array normals;
		Float2Array texcoords;
		VertexWeights weights;
		UintArray faces;

		for (PMD_IndexCount i = 0; i < it.FaceVertexCount; i++, indices++)
		{
			PMD_Vertex& v = pmd.VertexList[*indices];

			vertices.push_back(v.Position);
			normals.push_back(v.Normal);
			texcoords.push_back(v.UV);
			faces.push_back(i);

			VertexWeight weight;
			weight.weight1 = v.Weight / 100.0;
			weight.weight2 = 1.0 - weight.weight1;
			weight.weight3 = 0.0f;
			weight.weight4 = 0.0f;
			weight.bone1 = v.Bone.Bone1;
			weight.bone2 = v.Bone.Bone2;
			weight.bone3 = 0;
			weight.bone4 = 0;

			weights.push_back(weight);
		}

		auto mesh = std::make_shared<MeshProperty>();
		mesh->setVertexArray(std::move(vertices));
		mesh->setNormalArray(std::move(normals));
		mesh->setTexcoordArray(std::move(texcoords));
		mesh->setWeightArray(std::move(weights));
		mesh->setFaceArray(std::move(faces));

		if (!root)
			root = mesh;

		model.addMesh(mesh);
	}

	if (pmd.BoneCount > 1)
	{
		Bones bones;
		InverseKinematics iks;

		for (auto& it : pmd.BoneList)
		{
			Bone bone;

			char inbuf[MAX_PATH + 1] = { 0 };
			char outbuf[MAX_PATH + 1] = { 0 };
			char *in = inbuf;
			char *out = outbuf;
			std::size_t in_size = (std::size_t)MAX_PATH;
			std::size_t out_size = (std::size_t)MAX_PATH;

			memcpy(in, it.Name.Name, sizeof(it.Name.Name));

			iconv_t ic = iconv_open("GBK", "SJIS");
			iconv(ic, &in, &in_size, &out, &out_size);
			iconv_close(ic);

			if (out_size != MAX_PATH)
				bone.setName(outbuf);
			else
				bone.setName(in);

			bone.setPosition(it.Position);
			bone.setParent(it.Parent);

			bones.push_back(bone);

			model.addBone(std::make_shared<Bone>(bone));
		}

		for (auto& it : pmd.IkList)
		{
			IKAttr attr;
			attr.boneIndex = it.IK;
			attr.targetBoneIndex = it.Target;
			attr.chainLength = it.LinkCount;
			attr.iterations = it.LoopCount;

			for (auto& bone : it.LinkList)
			{
				IKChild child;
				child.boneIndex = bone;
				child.angleWeight = it.Weight;
				child.minimumDegrees = RAD_TO_DEG(Vector3(-0.002f, 0.0f, 0.0f));
				child.maximumDegrees = RAD_TO_DEG(Vector3(-M_PI, 0.0f, 0.0f));
				child.rotateLimited = model.getBonesList()[bone]->getName().find("ひざ") != std::string::npos;

				attr.child.push_back(child);
			}

			iks.push_back(attr);

			model.addIK(std::make_shared<IKAttr>(attr));
		}
	}

	for (auto& it : pmd.rigidbodys)
	{
		char inbuf[MAX_PATH + 1] = { 0 };
		char outbuf[MAX_PATH + 1] = { 0 };
		char *in = inbuf;
		char *out = outbuf;
		std::size_t in_size = (std::size_t)MAX_PATH;
		std::size_t out_size = (std::size_t)MAX_PATH;

		memcpy(in, it.Name, sizeof(it.Name));

		iconv_t ic = iconv_open("GBK", "SJIS");
		iconv(ic, &in, &in_size, &out, &out_size);
		iconv_close(ic);

		auto body = std::make_shared<RigidbodyProperty>();
		if (out_size != MAX_PATH)
			body->name = outbuf;
		else
			body->name = in;
		body->bone = it.bone;
		body->group = it.group;
		body->groupMask = it.groupMask;
		body->shape = (ShapeType)it.shape;
		body->scale = it.scale;
		body->position = it.position + pmd.BoneList[it.bone].Position;
		body->rotation = it.rotate;
		body->mass = it.mass;
		body->movementDecay = it.movementDecay;
		body->rotationDecay = it.rotationDecay;
		body->elasticity = it.elasticity;
		body->friction = it.friction;
		body->physicsOperation = it.physicsOperation;

		model.addRigidbody(body);
	}

	for (auto& it : pmd.joints)
	{
		char inbuf[MAX_PATH + 1] = { 0 };
		char outbuf[MAX_PATH + 1] = { 0 };
		char *in = inbuf;
		char *out = outbuf;
		std::size_t in_size = (std::size_t)MAX_PATH;
		std::size_t out_size = (std::size_t)MAX_PATH;

		memcpy(in, it.Name, sizeof(it.Name));

		iconv_t ic = iconv_open("GBK", "SJIS");
		iconv(ic, &in, &in_size, &out, &out_size);
		iconv_close(ic);

		auto joint = std::make_shared<JointProperty>();
		if (out_size != MAX_PATH)
			joint->name = outbuf;
		else
			joint->name = in;
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

		model.addJoint(joint);
	}

	return true;
}

bool
PMDHandler::doSave(Model& model, StreamWrite& stream) noexcept
{
	return false;
}

_NAME_END