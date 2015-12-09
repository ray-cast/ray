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
#include <ray/modpmx.h>

_NAME_BEGIN

bool
PMXHandler::doCanRead(StreamReader& stream) const noexcept
{
	PMX_Header hdr;

	if (stream.read((char*)&hdr, sizeof(hdr)))
	{
		if (hdr.Magic[0] == 'p' || hdr.Magic[0] == 'P' &&
			hdr.Magic[1] == 'm' || hdr.Magic[1] == 'M' &&
			hdr.Magic[2] == 'x' || hdr.Magic[2] == 'X')
		{
			if (hdr.Version == 2.0)
			{
				return true;
			}
		}
	}

	return false;
}

bool
PMXHandler::doLoad(Model& model, StreamReader& stream) noexcept
{
	PMX pmx;

	setlocale(LC_ALL, "");

	if (!stream.read((char*)&pmx.Header, sizeof(pmx.Header))) return false;
	if (!stream.read((char*)&pmx.Description.JapanModelLength, sizeof(pmx.Description.JapanModelLength))) return false;

	if (pmx.Description.JapanModelLength > 0)
	{
		pmx.Description.JapanModelName.resize(pmx.Description.JapanModelLength);

		if (!stream.read((char*)&pmx.Description.JapanModelName[0], pmx.Description.JapanModelLength)) return false;
	}

	if (!stream.read((char*)&pmx.Description.EnglishModelLength, sizeof(pmx.Description.EnglishModelLength))) return false;

	if (pmx.Description.EnglishModelLength > 0)
	{
		pmx.Description.EnglishCommentName.resize(pmx.Description.EnglishModelLength);

		if (!stream.read((char*)&pmx.Description.EnglishCommentName[0], pmx.Description.EnglishModelLength)) return false;
	}

	if (!stream.read((char*)&pmx.Description.JapanCommentLength, sizeof(pmx.Description.JapanCommentLength))) return false;

	if (pmx.Description.JapanCommentLength > 0)
	{
		pmx.Description.JapanCommentName.resize(pmx.Description.JapanCommentLength);

		if (!stream.read((char*)&pmx.Description.JapanCommentName[0], pmx.Description.JapanCommentLength)) return false;
	}

	if (!stream.read((char*)&pmx.Description.EnglishCommentLength, sizeof(pmx.Description.EnglishCommentLength))) return false;

	if (pmx.Description.EnglishCommentLength > 0)
	{
		pmx.Description.EnglishCommentName.resize(pmx.Description.EnglishCommentLength);

		if (!stream.read((char*)&pmx.Description.EnglishCommentName[0], pmx.Description.EnglishCommentLength)) return false;
	}

	if (!stream.read((char*)&pmx.VertexCount, sizeof(pmx.VertexCount))) return false;

	if (pmx.VertexCount > 0)
	{
		pmx.VertexList.resize(pmx.VertexCount);

		for (PMX_VertexCount i = 0; i < pmx.VertexCount; i++)
		{
			PMX_Vertex& vertex = pmx.VertexList[i];

			if (!stream.read((char*)&vertex.Position, sizeof(vertex.Position))) return false;
			if (!stream.read((char*)&vertex.Normal, sizeof(vertex.Normal))) return false;
			if (!stream.read((char*)&vertex.UV, sizeof(vertex.UV))) return false;

			if (pmx.Header.UVCount != 0)
			{
				if (!stream.read((char*)&vertex.addUV, (std::streamsize)pmx.Header.UVCount)) return false;
			}

			if (!stream.read((char*)&vertex.Type, sizeof(vertex.Type))) return false;

			switch (vertex.Type)
			{
			case PMX_BDEF1:
			{
				if (!stream.read((char*)&vertex.BoneWeight.Bone1, pmx.Header.BoneIndexSize)) return false;

				vertex.BoneWeight.Weight1 = 1.0f;
			}
			break;
			case PMX_BDEF2:
			{
				if (!stream.read((char*)&vertex.BoneWeight.Bone1, pmx.Header.BoneIndexSize)) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Bone2, pmx.Header.BoneIndexSize)) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Weight1, sizeof(vertex.BoneWeight.Weight2))) return false;

				vertex.BoneWeight.Weight2 = 1.0f - vertex.BoneWeight.Weight1;
			}
			break;
			case PMX_BDEF4:
			{
				if (!stream.read((char*)&vertex.BoneWeight.Bone1, pmx.Header.BoneIndexSize)) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Bone2, pmx.Header.BoneIndexSize)) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Bone3, pmx.Header.BoneIndexSize)) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Bone4, pmx.Header.BoneIndexSize)) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Weight1, sizeof(vertex.BoneWeight.Weight1))) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Weight2, sizeof(vertex.BoneWeight.Weight2))) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Weight3, sizeof(vertex.BoneWeight.Weight3))) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Weight4, sizeof(vertex.BoneWeight.Weight4))) return false;
			}
			break;
			case PMX_SDEF:
			{
				if (!stream.read((char*)&vertex.BoneWeight.Bone1, pmx.Header.BoneIndexSize)) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Bone2, pmx.Header.BoneIndexSize)) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Weight1, sizeof(vertex.BoneWeight.Weight1))) return false;
				if (!stream.read((char*)&vertex.BoneWeight.SDEF_C, sizeof(vertex.BoneWeight.SDEF_C))) return false;
				if (!stream.read((char*)&vertex.BoneWeight.SDEF_R0, sizeof(vertex.BoneWeight.SDEF_R0))) return false;
				if (!stream.read((char*)&vertex.BoneWeight.SDEF_R1, sizeof(vertex.BoneWeight.SDEF_R1))) return false;

				vertex.BoneWeight.Weight2 = 1.0f - vertex.BoneWeight.Weight1;
			}
			break;
			case PMX_QDEF:
			{
				if (!stream.read((char*)&vertex.BoneWeight.Bone1, pmx.Header.BoneIndexSize)) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Bone2, pmx.Header.BoneIndexSize)) return false;
				if (!stream.read((char*)&vertex.BoneWeight.Weight1, sizeof(vertex.BoneWeight.Weight1))) return false;

				vertex.BoneWeight.Weight2 = 1.0f - vertex.BoneWeight.Weight1;
			}
			default:
				return false;
			}

			if (!stream.read((char*)&vertex.edge, sizeof(vertex.edge))) return false;
		}
	}

	if (!stream.read((char*)&pmx.IndexCount, sizeof(pmx.IndexCount))) return false;

	if (pmx.IndexCount > 0)
	{
		pmx.IndexList.resize(pmx.IndexCount);

		if (pmx.Header.VertexIndexSize != sizeof(PMX_Index))
		{
			for (std::size_t i = 0; i < pmx.IndexCount; i++)
			{
				if (!stream.read((char*)&pmx.IndexList[i], (std::streamsize)pmx.Header.VertexIndexSize)) return false;
			}
		}
		else
		{
			if (!stream.read((char*)&pmx.IndexList[0], (std::streamsize)(sizeof(PMX_Index) * pmx.IndexCount))) return false;
		}
	}

	if (!stream.read((char*)&pmx.TextureCount, sizeof(pmx.TextureCount))) return false;

	if (pmx.TextureCount > 0)
	{
		pmx.TextureList.resize(pmx.TextureCount);

		for (PMX_TextureCount i = 0; i < pmx.TextureCount; i++)
		{
			PMX_Texture& texture = pmx.TextureList[i];

			if (!stream.read((char*)&texture.length, sizeof(texture.length))) return false;

			if (texture.length > 0)
			{
				texture.name.resize(texture.length);

				if (!stream.read((char*)&texture.name[0], texture.length)) return false;
			}
		}
	}

	if (!stream.read((char*)&pmx.MaterialCount, sizeof(pmx.MaterialCount))) return false;

	if (pmx.MaterialCount > 0)
	{
		pmx.MaterialList.resize(pmx.MaterialCount);

		for (PMX_MaterialCount i = 0; i < pmx.MaterialCount; i++)
		{
			PMX_Material& material = pmx.MaterialList[i];

			if (!stream.read((char*)&material.Name.length, sizeof(material.Name.length))) return false;

			if (material.Name.length > 0)
			{
				material.Name.name.resize(material.Name.length);

				if (!stream.read((char*)&material.Name.name[0], material.Name.length)) return false;
			}

			if (!stream.read((char*)&material.EnglishName.length, sizeof(material.EnglishName.length))) return false;

			if (material.EnglishName.length > 0)
			{
				material.EnglishName.name.resize(material.EnglishName.length);

				if (!stream.read((char*)&material.EnglishName.name[0], material.EnglishName.length)) return false;
			}

			if (!stream.read((char*)&material.Diffuse, sizeof(material.Diffuse))) return false;
			if (!stream.read((char*)&material.Opacity, sizeof(material.Opacity))) return false;
			if (!stream.read((char*)&material.Specular, sizeof(material.Specular))) return false;
			if (!stream.read((char*)&material.Shininess, sizeof(material.Shininess))) return false;
			if (!stream.read((char*)&material.Ambient, sizeof(material.Ambient))) return false;
			if (!stream.read((char*)&material.Flag, sizeof(material.Flag))) return false;
			if (!stream.read((char*)&material.EdgeColor, sizeof(material.EdgeColor))) return false;
			if (!stream.read((char*)&material.EdgeSize, sizeof(material.EdgeSize))) return false;
			if (!stream.read((char*)&material.TextureIndex, sizeof(material.TextureIndex))) return false;
			if (!stream.read((char*)&material.SphereTextureIndex, sizeof(material.SphereTextureIndex))) return false;
			if (!stream.read((char*)&material.ToonIndex, sizeof(material.ToonIndex))) return false;
			if (!stream.read((char*)&material.ToonFlag, sizeof(material.ToonFlag))) return false;
			if (!stream.read((char*)&material.SphereMode, sizeof(material.SphereMode))) return false;
			if (!stream.read((char*)&material.FaceVertexCount, sizeof(material.FaceVertexCount))) return false;
		}
	}

	if (!stream.read((char*)&pmx.BoneCount, sizeof(pmx.BoneCount))) return false;

	if (pmx.BoneCount > 0)
	{
		pmx.BoneList.resize(pmx.BoneCount);

		for (PMX_MaterialCount i = 0; i < pmx.BoneCount; i++)
		{
			PMX_Bone& bone = pmx.BoneList[i];

			if (!stream.read((char*)&bone.Name.length, sizeof(bone.Name.length))) return false;

			if (bone.Name.length > 0 && bone.Name.length < MAX_PATH)
			{
				bone.Name.name.resize(bone.Name.length >> 1);

				if (!stream.read((char*)&bone.Name.name[0], bone.Name.length)) return false;
			}

			if (!stream.read((char*)&bone.EnglishName.length, sizeof(bone.EnglishName.length))) return false;

			if (bone.EnglishName.length > 0 && bone.EnglishName.length < MAX_PATH)
			{
				bone.EnglishName.name.resize(bone.EnglishName.length >> 1);

				if (!stream.read((char*)&bone.EnglishName.name[0], bone.EnglishName.length)) return false;
			}

			if (!stream.read((char*)&bone.Position, sizeof(bone.Position))) return false;
			if (!stream.read((char*)&bone.Parent, pmx.Header.BoneIndexSize)) return false;
			if (!stream.read((char*)&bone.Level, sizeof(bone.Level))) return false;
			if (!stream.read((char*)&bone.Flag, sizeof(bone.Flag))) return false;

			if (bone.Flag & PMX_BONE_INDEX)
			{
				if (!stream.read((char*)&bone.ConnectedBoneIndex, pmx.Header.BoneIndexSize)) return false;
			}
			else
			{
				if (!stream.read((char*)&bone.Offset, sizeof(bone.Offset))) return false;
			}

			if (bone.Flag & PMX_BONE_PARENT)
			{
				if (!stream.read((char*)&bone.ProvidedParentBoneIndex, pmx.Header.BoneIndexSize)) return false;
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
				if (!stream.read((char*)&bone.IKTargetBoneIndex, pmx.Header.BoneIndexSize)) return false;
				if (!stream.read((char*)&bone.IKLoopCount, sizeof(bone.IKLoopCount))) return false;
				if (!stream.read((char*)&bone.IKLimitedRadian, sizeof(bone.IKLimitedRadian))) return false;
				if (!stream.read((char*)&bone.IKLinkCount, sizeof(bone.IKLinkCount))) return false;

				if (bone.IKLinkCount > 0)
				{
					bone.IKList.resize(bone.IKLinkCount);

					for (std::size_t j = 0; j < bone.IKLinkCount; j++)
					{
						if (!stream.read((char*)&bone.IKList[j].BoneIndex, pmx.Header.BoneIndexSize)) return false;
						if (!stream.read((char*)&bone.IKList[j].RotateLimited, (std::streamsize)sizeof(bone.IKList[j].RotateLimited))) return false;
						if (bone.IKList[j].RotateLimited)
						{
							if (!stream.read((char*)&bone.IKList[j].MaximumRadian, (std::streamsize)sizeof(bone.IKList[j].MaximumRadian))) return false;
							if (!stream.read((char*)&bone.IKList[j].MinimumRadian, (std::streamsize)sizeof(bone.IKList[j].MinimumRadian))) return false;
						}
					}
				}
			}
		}
	}

	if (!stream.read((char*)&pmx.MorphCount, sizeof(pmx.MorphCount))) return false;

	if (pmx.MorphCount > 0)
	{
		pmx.MorphList.resize(pmx.MorphCount);

		for (std::size_t i = 0; i < (std::size_t)pmx.MorphCount; i++)
		{
			if (!stream.read((char*)&pmx.MorphList[i].Length, sizeof(pmx.MorphList[i].Length))) return false;
			if (!stream.read((char*)&pmx.MorphList[i].Name, pmx.MorphList[i].Length)) return false;
			if (!stream.read((char*)&pmx.MorphList[i].Unknown, sizeof(pmx.MorphList[i].Unknown))) return false;
			if (!stream.read((char*)&pmx.MorphList[i].OffsetSize, sizeof(pmx.MorphList[i].OffsetSize))) return false;
			if (!stream.read((char*)&pmx.MorphList[i].MorphType, sizeof(pmx.MorphList[i].MorphType))) return false;
			if (!stream.read((char*)&pmx.MorphList[i].MorphCount, sizeof(pmx.MorphList[i].MorphCount))) return false;

			if (pmx.MorphList[i].MorphCount > 0)
			{
				if (pmx.MorphList[i].MorphType == 1)
				{
					pmx.MorphList[i].VertexList.resize(pmx.MorphList[i].MorphCount);

					for (std::size_t j = 0; j < (std::size_t)pmx.MorphList[i].MorphCount; j++)
					{
						if (!stream.read((char*)&pmx.MorphList[i].VertexList[j].Index, pmx.Header.MorphIndexSize)) return false;
						if (!stream.read((char*)&pmx.MorphList[i].VertexList[j].Offset, sizeof(pmx.MorphList[i].VertexList[j].Offset))) return false;
					}
				}
				else if (pmx.MorphList[i].MorphType == 2)
				{
					pmx.MorphList[i].BoneList.resize(pmx.MorphList[i].MorphCount);

					for (std::size_t j = 0; j < (std::size_t)pmx.MorphList[i].MorphCount; j++)
					{
						if (!stream.read((char*)&pmx.MorphList[i].BoneList[j].BoneIndex, pmx.Header.BoneIndexSize)) return false;
						if (!stream.read((char*)&pmx.MorphList[i].BoneList[j].Weight, sizeof(pmx.MorphList[i].BoneList[j].Weight))) return false;
						if (!stream.read((char*)&pmx.MorphList[i].BoneList[j].Position, sizeof(pmx.MorphList[i].BoneList[j].Position))) return false;
						if (!stream.read((char*)&pmx.MorphList[i].BoneList[j].Rotate, sizeof(pmx.MorphList[i].BoneList[j].Rotate))) return false;
					}
				}
				else if (pmx.MorphList[i].MorphType == 3)
				{
					for (std::size_t j = 0; j < (std::size_t)pmx.MorphList[i].MorphCount; j++)
					{
						if (!stream.read((char*)&pmx.MorphList[i].VertexList[j].Index, pmx.Header.MorphIndexSize)) return false;
						if (!stream.read((char*)&pmx.MorphList[i].VertexList[j].Offset, sizeof(pmx.MorphList[i].VertexList[j].Offset))) return false;
					}
				}
				else if (pmx.MorphList[i].MorphType == 8)
				{
					pmx.MorphList[i].MaterialList.resize(pmx.MorphList[i].MorphCount);

					for (std::size_t j = 0; j < (std::size_t)pmx.MorphList[i].MorphCount; j++)
					{
						if (!stream.read((char*)&pmx.MorphList[i].MaterialList[j].Index, pmx.Header.MorphIndexSize)) return false;
						if (!stream.read((char*)&pmx.MorphList[i].MaterialList[j].Diffuse, sizeof(pmx.MorphList[i].MaterialList[j].Diffuse))) return false;
						if (!stream.read((char*)&pmx.MorphList[i].MaterialList[j].Opacity, sizeof(pmx.MorphList[i].MaterialList[j].Opacity))) return false;
						if (!stream.read((char*)&pmx.MorphList[i].MaterialList[j].Specular, sizeof(pmx.MorphList[i].MaterialList[j].Specular))) return false;
						if (!stream.read((char*)&pmx.MorphList[i].MaterialList[j].Shininess, sizeof(pmx.MorphList[i].MaterialList[j].Shininess))) return false;
						if (!stream.read((char*)&pmx.MorphList[i].MaterialList[j].Ambient, sizeof(pmx.MorphList[i].MaterialList[j].Ambient))) return false;
						if (!stream.read((char*)&pmx.MorphList[i].MaterialList[j].EdgeColor, sizeof(pmx.MorphList[i].MaterialList[j].EdgeColor))) return false;
						if (!stream.read((char*)&pmx.MorphList[i].MaterialList[j].EdgeSize, sizeof(pmx.MorphList[i].MaterialList[j].EdgeSize))) return false;
						if (!stream.read((char*)&pmx.MorphList[i].MaterialList[j].Tex, sizeof(pmx.MorphList[i].MaterialList[j].Tex))) return false;
						if (!stream.read((char*)&pmx.MorphList[i].MaterialList[j].Unknown, sizeof(pmx.MorphList[i].MaterialList[j].Unknown))) return false;
						if (!stream.read((char*)&pmx.MorphList[i].MaterialList[j].Toon, sizeof(pmx.MorphList[i].MaterialList[j].Toon))) return false;
					}
				}
			}
		}
	}

	if (!stream.read((char*)&pmx.DisplaySlot.DisplayCount, sizeof(pmx.DisplaySlot.DisplayCount))) return false;

	/*if (pmx.DisplaySlot.DisplayCount > 0)
	{
		pmx.DisplaySlot.Slots.resize(pmx.DisplaySlot.DisplayCount);

		for (std::size_t i = 0; i < (std::size_t)pmx.DisplaySlot.DisplayCount; i++)
		{
			if (!stream.read((char*)&pmx.DisplaySlot.Slots[i].Type, sizeof(pmx.DisplaySlot.Slots[i].Type))) return false;

			if (pmx.DisplaySlot.Slots[i].Type == 0)
			{
				if (!stream.read((char*)&pmx.DisplaySlot.Slots[i].BoneIndex, sizeof(pmx.Header.BodyIndexSize)))
					return false;
			}

			else if (pmx.DisplaySlot.Slots[i].Type == 1)
			{
				if (!stream.read((char*)&pmx.DisplaySlot.Slots[i].MorphIndex, sizeof(pmx.Header.MorphIndexSize)))
					return false;
			}
			else
			{
				assert(false);
			}
		}
	}*/

	for (std::size_t index = 0; index < pmx.MaterialList.size(); index++)
	{
		auto& it = pmx.MaterialList[index];

		auto material = std::make_shared<MaterialProperty>();

		if ((it.Name.length >> 1) < MAX_PATH)
		{
			char name[MAX_PATH];
			auto length = wcstombs(nullptr, it.Name.name.c_str(), 0);
			if (length != -1 && length < MAX_PATH)
			{
				wcstombs(name, it.Name.name.c_str(), MAX_PATH);

				material->set(MATKEY_NAME, name);
			}
		}

		material->set(MATKEY_COLOR_DIFFUSE, it.Diffuse);
		material->set(MATKEY_COLOR_AMBIENT, it.Ambient);
		material->set(MATKEY_COLOR_SPECULAR, it.Specular);
		material->set(MATKEY_OPACITY, it.Opacity);
		material->set(MATKEY_SHININESS, it.Shininess);

		if (it.TextureIndex != std::numeric_limits<PMX_uint8_t>::max())
		{
			PMX_Texture& texture = pmx.TextureList[it.TextureIndex];

			std::wstring::size_type substr = texture.name.find_first_of(L"*");
			if (substr != std::string::npos)
			{
				texture.name.erase(texture.name.begin() + substr, texture.name.end());
			}

			if ((texture.length >> 1) < MAX_PATH)
			{
				char name[MAX_PATH];

				wcstombs(name, texture.name.c_str(), MAX_PATH);

				material->set(MATKEY_TEXTURE_DIFFUSE(0), name);
				material->set(MATKEY_TEXTURE_AMBIENT(0), name);
			}
		}

		if (it.SphereTextureIndex != std::numeric_limits<PMX_uint8_t>::max())
		{
			PMX_Texture& texture = pmx.TextureList[it.SphereTextureIndex];

			std::wstring::size_type substr = texture.name.find_first_of(L"*");
			if (substr != std::string::npos)
			{
				texture.name.erase(texture.name.begin() + substr, texture.name.end());
			}

			if ((texture.length >> 1) < MAX_PATH)
			{
				char name[MAX_PATH];

				wcstombs(name, texture.name.c_str(), MAX_PATH);

				material->set(MATKEY_TEXTURE_NORMALS(0), name);
			}
		}

		model.addMaterial(material);
	}

	PMX_Index* indices = pmx.IndexList.data();
	PMX_Vertex* vertices = pmx.VertexList.data();

	MeshPropertyPtr root = nullptr;

	for (std::size_t index = 0; index < pmx.MaterialList.size(); index++)
	{
		auto& it = pmx.MaterialList[index];

		Vector3Array points;
		Vector3Array normals;
		Vector2Array texcoords;
		VertexWeights weights;
		UintArray faces;

		for (PMX_IndexCount i = 0; i < it.FaceVertexCount; i++, indices++)
		{
			PMX_Vertex& v = vertices[*indices];

			points.push_back(v.Position);
			normals.push_back(v.Normal);
			texcoords.push_back(v.UV);
			faces.push_back(i);

			VertexWeight weight;
			weight.weight1 = v.BoneWeight.Weight1;
			weight.weight2 = v.BoneWeight.Weight2;
			weight.weight3 = v.BoneWeight.Weight3;
			weight.weight4 = v.BoneWeight.Weight4;
			weight.bone1 = v.BoneWeight.Bone1;
			weight.bone2 = v.BoneWeight.Bone2;
			weight.bone3 = v.BoneWeight.Bone3;
			weight.bone4 = v.BoneWeight.Bone4;

			weights.push_back(weight);
		}

		MeshPropertyPtr mesh = std::make_shared<MeshProperty>();
		mesh->setMaterialID(index);
		mesh->setVertexArray(points);
		mesh->setNormalArray(normals);
		mesh->setTexcoordArray(texcoords);
		mesh->setWeightArray(weights);
		mesh->setFaceArray(faces);

		if (root)
			root->addChild(mesh);
		else
			root = mesh;
	}

	if (pmx.BoneCount > 0)
	{
		Bones bones;
		InverseKinematics iks;

		auto& boneLists = pmx.BoneList;
		for (std::size_t boneIndex = 0; boneIndex < boneLists.size(); boneIndex++)
		{
			auto& it = boneLists[boneIndex];

			char name[MAX_PATH] = { 0 };
			if ((it.Name.length) > MAX_PATH)
				return false;

			if (!wcstombs(name, it.Name.name.data(), MAX_PATH))
				return false;

			Bone bone;

			bone.setName(name);
			bone.setPosition(it.Position);
			bone.setParent(it.Parent);
			bone.setChild(it.ConnectedBoneIndex);

			bones.push_back(bone);

			if (it.Flag & PMX_BONE_IK)
			{
				IKAttr attr;
				attr.IKBoneIndex = boneIndex;
				attr.IKTargetBoneIndex = it.IKTargetBoneIndex;
				attr.IKLimitedRadian = it.IKLimitedRadian;
				attr.IKLinkCount = it.IKLinkCount;
				attr.IKLoopCount = it.IKLoopCount;
				for (auto& bone : it.IKList)
				{
					IKChild child;
					child.BoneIndex = bone.BoneIndex;
					child.MinimumRadian = bone.MinimumRadian;
					child.MaximumRadian = bone.MaximumRadian;
					child.RotateLimited = bone.RotateLimited;

					attr.IKList.push_back(child);
				}

				iks.push_back(attr);
			}
		}

		root->setInverseKinematics(iks);
		root->setBoneArray(bones);
	}

	model.addMesh(root);

	return true;
}

bool
PMXHandler::doSave(Model& model, StreamWrite& stream) noexcept
{
	return false;
}

_NAME_END