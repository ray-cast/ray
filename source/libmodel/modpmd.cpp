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
PMDHandler::doCanRead(const char* type) const noexcept
{
	return std::strncmp(type, "pmd", 3) == 0;
}

bool
PMDHandler::doLoad(StreamReader& stream, PMD& pmd) noexcept
{
	if (!stream.read((char*)&pmd.Header, sizeof(pmd.Header))) return false;

	if (!stream.read((char*)&pmd.numVertices, sizeof(pmd.numVertices))) return false;
	if (pmd.numVertices > 0)
	{
		pmd.VertexList.resize(pmd.numVertices);

		if (!stream.read((char*)&pmd.VertexList[0], (std::streamsize)(sizeof(PMD_Vertex)* pmd.numVertices))) return false;
	}

	if (!stream.read((char*)&pmd.numIndices, sizeof(pmd.numIndices))) return false;

	if (pmd.numIndices > 0)
	{
		pmd.IndexList.resize(pmd.numIndices);

		if (!stream.read((char*)&pmd.IndexList[0], (std::streamsize)(sizeof(PMD_Index)* pmd.numIndices))) return false;
	}

	if (!stream.read((char*)&pmd.numMaterials, sizeof(pmd.numMaterials))) return false;

	if (pmd.numMaterials > 0)
	{
		pmd.MaterialList.resize(pmd.numMaterials);

		if (!stream.read((char*)&pmd.MaterialList[0], (std::streamsize)(sizeof(PMD_Material)* pmd.numMaterials))) return false;
	}

	if (!stream.read((char*)&pmd.numBones, sizeof(pmd.numBones))) return false;

	if (pmd.numBones > 0)
	{
		pmd.BoneList.resize(pmd.numBones);

		if (!stream.read((char*)&pmd.BoneList[0], (std::streamsize)(sizeof(PMD_Bone)* pmd.numBones))) return false;
	}

	if (!stream.read((char*)&pmd.numIKs, sizeof(pmd.numIKs))) return false;

	if (pmd.numIKs > 0)
	{
		pmd.IkList.resize(pmd.numIKs);

		for (std::size_t i = 0; i < (std::size_t)pmd.numIKs; i++)
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

	if (!stream.read((char*)&pmd.numMorphs, sizeof(pmd.numMorphs))) return false;

	if (pmd.numMorphs > 0)
	{
		pmd.MorphList.resize(pmd.numMorphs);

		for (std::size_t i = 0; i < (std::size_t)pmd.numMorphs; i++)
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

	if (!stream.read((char*)&pmd.numExpression, sizeof(pmd.numExpression))) return false;

	if (pmd.numExpression > 0)
	{
		pmd.ExpressionList.resize(pmd.numExpression);

		if (!stream.read((char*)&pmd.ExpressionList[0], (std::streamsize)(sizeof(PMD_Expression)* pmd.numExpression))) return false;
	}

	if (!stream.read((char*)&pmd.numNodeNames, sizeof(pmd.numNodeNames))) return false;

	if (pmd.numNodeNames > 0)
	{
		pmd.NodeNameList.resize(pmd.numNodeNames);

		if (!stream.read((char*)&pmd.NodeNameList[0].Name, (std::streamsize)(sizeof(PMD_NodeName)* pmd.numNodeNames))) return false;
	}

	if (!stream.read((char*)&pmd.numNodeBones, sizeof(pmd.numNodeBones))) return false;

	if (pmd.numNodeBones > 0)
	{
		pmd.BoneToNodeList.resize(pmd.numNodeBones);

		if (!stream.read((char*)&pmd.BoneToNodeList[0].Bone, (std::streamsize)(sizeof(PMD_BoneToNode) * pmd.numNodeBones))) return false;
	}

	if (!stream.read((char*)&pmd.HasDescription, sizeof(pmd.HasDescription))) return false;

	if (pmd.HasDescription)
	{
		if (!stream.read((char*)&pmd.Description.ModelName, sizeof(pmd.Description.ModelName))) return false;

		if (!stream.read((char*)&pmd.Description.Comment, sizeof(pmd.Description.Comment))) return false;

		for (PMD_BoneCount i = 0; i < pmd.numBones; i++)
		{
			PMD_BoneName name;

			if (!stream.read((char*)&name.Name, sizeof(name))) return false;

			pmd.Description.BoneName.push_back(name);
		}

		for (PMD_uint8_t i = 0; i < pmd.numExpression; i++)
		{
			PMD_MorphName name;

			if (!stream.read((char*)&name.Name, sizeof(name))) return false;

			pmd.Description.FaceName.push_back(name);
		}

		for (PMD_uint8_t i = 0; i < pmd.numNodeNames; i++)
		{
			PMD_NodeName name;

			if (!stream.read((char*)&name.Name, sizeof(name))) return false;

			pmd.Description.FrameName.push_back(name);
		}
	}

	pmd.numToons = PMD_NUM_TOON;

	pmd.ToonList.resize(pmd.numToons);

	if (!stream.read((char*)&pmd.ToonList[0].Name, (std::streamsize)(sizeof(PMD_Toon) * pmd.numToons))) return false;

	if (!stream.read((char*)&pmd.numRigidbodys, sizeof(pmd.numRigidbodys))) return false;

	if (pmd.numRigidbodys > 0)
	{
		pmd.rigidbodys.resize(pmd.numRigidbodys);

		if (!stream.read((char*)&pmd.rigidbodys[0], (std::streamsize)(sizeof(PMD_Body)* pmd.numRigidbodys))) return false;
	}

	if (!stream.read((char*)&pmd.numJoints, sizeof(pmd.numJoints))) return false;

	if (pmd.numJoints > 0)
	{
		pmd.joints.resize(pmd.numJoints);

		if (!stream.read((char*)&pmd.joints[0], (std::streamsize)(sizeof(PMD_Joint) * pmd.numJoints))) return false;
	}

	return true;
}

bool
PMDHandler::doLoad(StreamReader& stream, Model& model) noexcept
{
	PMD pmd;
	if (!this->doLoad(stream, pmd))
		return false;

	for (std::size_t index = 0; index < pmd.MaterialList.size(); index++)
	{
		auto& it = pmd.MaterialList[index];

		auto material = std::make_shared<MaterialProperty>();
		material->set(MATKEY_COLOR_DIFFUSE, ray::math::srgb2linear(it.Diffuse));
		material->set(MATKEY_COLOR_AMBIENT, ray::math::srgb2linear(it.Ambient));
		material->set(MATKEY_COLOR_SPECULAR, ray::math::srgb2linear(it.Specular));
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
			weight.weight1 = v.Weight / 100.0f;
			weight.weight2 = 1.0f - weight.weight1;
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

	if (pmd.numBones > 1)
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
PMDHandler::doSave(StreamWrite& stream, const PMD& model) noexcept
{
	return false;
}

bool
PMDHandler::doSave(StreamWrite& stream, const Model& model) noexcept
{
	return false;
}

_NAME_END