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
				stream.seekg(0, std::ios_base::beg);
				return true;
			}
		}
	}

	stream.seekg(0, std::ios_base::beg);
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
PMXHandler::doLoad(StreamReader& stream) noexcept
{
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
				std::streamsize size = sizeof(vertex.position) + sizeof(vertex.normal) + sizeof(vertex.coord) + sizeof(vertex.addCoord);
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


			if (!stream.read((char*)&material.FaceVertexCount, sizeof(material.FaceVertexCount))) return false;
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

	return true;
}

bool
PMXHandler::doSave(StreamWrite& stream) noexcept
{
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
		pmx.textures.resize(pmx.numTextures);

		for (auto& texture : pmx.textures)
		{
			if (!stream.write((char*)&texture.length, sizeof(texture.length))) return false;
			if (!stream.write((char*)&texture.name, texture.length)) return false;
		}
	}

	if (!stream.write((char*)&pmx.numMaterials, sizeof(pmx.numMaterials))) return false;
	if (pmx.numMaterials)
	{
		pmx.materials.resize(pmx.numMaterials);

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


			if (!stream.write((char*)&material.FaceVertexCount, sizeof(material.FaceVertexCount))) return false;
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
					bone.IKList.resize(bone.IKLinkCount);

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
				morph.vertexList.resize(morph.morphCount);

				for (auto& vertex : morph.vertexList)
				{
					if (!stream.write((char*)&vertex.index, pmx.header.sizeOfVertex)) return false;
					if (!stream.write((char*)&vertex.offset, sizeof(vertex.offset))) return false;
				}
			}
			else if (morph.morphType == MorphType::MorphTypeBone)
			{
				morph.boneList.resize(morph.morphCount);

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
				morph.texcoordList.resize(morph.morphCount);

				for (auto& texcoord : morph.texcoordList)
				{
					if (!stream.write((char*)&texcoord.index, pmx.header.sizeOfVertex)) return false;
					if (!stream.write((char*)&texcoord.offset, sizeof(texcoord.offset))) return false;
				}
			}
			else if (morph.morphType == MorphType::MorphTypeMaterial)
			{
				morph.materialList.resize(morph.morphCount);

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

			displayFrame.elements.resize(displayFrame.elementsWithinFrame);
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

std::size_t
PMXHandler::getFace(std::size_t n) noexcept
{
	if (pmx.header.sizeOfVertex == 1)
		return *(std::uint8_t*)(pmx.indices.data() + n * pmx.header.sizeOfVertex);
	else if (pmx.header.sizeOfVertex == 2)
		return *(std::uint16_t*)(pmx.indices.data() + n * pmx.header.sizeOfVertex);
	else if (pmx.header.sizeOfVertex == 4)
		return *(std::uint32_t*)(pmx.indices.data() + n * pmx.header.sizeOfVertex);
	else
		return false;
}

void
PMXHandler::computeFaceNormals() noexcept
{
	_facesNormal.resize(pmx.numIndices / 3);

	for (std::size_t i = 0; i < pmx.numIndices; i += 3)
	{
		std::size_t f1 = getFace(i);
		std::size_t f2 = getFace(i + 1);
		std::size_t f3 = getFace(i + 2);

		const Vector3& a = pmx.vertices[f1].position;
		const Vector3& b = pmx.vertices[f2].position;
		const Vector3& c = pmx.vertices[f3].position;

		Vector3 edge1 = c - b;
		Vector3 edge2 = a - b;

		Vector3 normal = math::normalize(math::cross(edge1, edge2));

		_facesNormal[i / 3] = normal;
	}
}

void
PMXHandler::computeVertricesNormals() noexcept
{
	for (size_t i = 0; i < pmx.numVertices; i++)
	{
		pmx.vertices[i].normal = ray::float3::Zero;
	}

	for (size_t i = 0; i < pmx.numIndices; i += 3)
	{
		std::uint32_t a = getFace(i);
		std::uint32_t b = getFace(i + 1);
		std::uint32_t c = getFace(i + 2);

		pmx.vertices[a].normal += _facesNormal[i / 3];
		pmx.vertices[b].normal += _facesNormal[i / 3];
		pmx.vertices[c].normal += _facesNormal[i / 3];
	}

	for (size_t i = 0; i < pmx.numVertices; i++)
	{
		pmx.vertices[i].normal = math::normalize(pmx.vertices[i].normal);
	}
}

void
PMXHandler::computeBoundingBox(Bound& boundingBox) noexcept
{
	boundingBox.reset();

	for (size_t i = 0; i < pmx.numVertices; i++)
		boundingBox.encapsulate(pmx.vertices[i].position);
}

void
PMXHandler::computeLightmapPack() noexcept
{
	this->computeFaceNormals();

	Bound boundingBox;
	this->computeBoundingBox(boundingBox);

	pmx.header.addUVCount = 1;

	float2 minUV[3];
	float2 maxUV[3];

	minUV[0].set(FLT_MAX);
	minUV[1].set(FLT_MAX);
	minUV[2].set(FLT_MAX);
	maxUV[0].set(-FLT_MAX);
	maxUV[1].set(-FLT_MAX);
	maxUV[2].set(-FLT_MAX);

	for (size_t i = 0; i < pmx.numIndices; i += 3)
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
			uv[0] = pmx.vertices[a].position.yz();
			uv[1] = pmx.vertices[b].position.yz();
			uv[2] = pmx.vertices[c].position.yz();
		}
		else if (polyNormal.y > polyNormal.x && polyNormal.y > polyNormal.z)
		{
			flag = 2;
			uv[0] = pmx.vertices[a].position.xz();
			uv[1] = pmx.vertices[b].position.xz();
			uv[2] = pmx.vertices[c].position.xz();
		}
		else
		{
			uv[0] = pmx.vertices[a].position.xy();
			uv[1] = pmx.vertices[b].position.xy();
			uv[2] = pmx.vertices[c].position.xy();
		}

		for (int i = 0; i < 3; i++)
		{
			minUV[flag] = math::min(minUV[flag], uv[i]);
			maxUV[flag] = math::max(maxUV[flag], uv[i]);
		}
	}

	for (size_t i = 0; i < pmx.numIndices; i += 3)
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
			uv[0] = pmx.vertices[a].position.yz();
			uv[1] = pmx.vertices[b].position.yz();
			uv[2] = pmx.vertices[c].position.yz();
		}
		else if (polyNormal.y > polyNormal.x && polyNormal.y > polyNormal.z)
		{
			flag = 2;
			uv[0] = pmx.vertices[a].position.xz();
			uv[1] = pmx.vertices[b].position.xz();
			uv[2] = pmx.vertices[c].position.xz();
		}
		else
		{
			uv[0] = pmx.vertices[a].position.xy();
			uv[1] = pmx.vertices[b].position.xy();
			uv[2] = pmx.vertices[c].position.xy();
		}

		float2 deltaUV = maxUV[flag] - minUV[flag];

		uv[0] -= minUV[flag];
		uv[1] -= minUV[flag];
		uv[2] -= minUV[flag];

		uv[0] /= deltaUV;
		uv[1] /= deltaUV;
		uv[2] /= deltaUV;

		pmx.vertices[a].addCoord[0].set(uv[0]);
		pmx.vertices[b].addCoord[0].set(uv[1]);
		pmx.vertices[c].addCoord[0].set(uv[2]);
	}
}

LightMapNode*
PMXHandler::insertLightMapItem(LightMapNode* node, LightMapItem& item) noexcept
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

_NAME_END