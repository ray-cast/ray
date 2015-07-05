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
#include <ray/modpmd.h>

_NAME_BEGIN

bool
PMDHandler::doCanRead(istream& stream) const noexcept
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
PMDHandler::doLoad(Model& model, istream& stream) noexcept
{
	PMD _pmd;
	if (!stream.read((char*)&_pmd.Header, sizeof(_pmd.Header))) return false;

	// vertex
	if (!stream.read((char*)&_pmd.VertexCount, sizeof(_pmd.VertexCount))) return false;

	if (_pmd.VertexCount > 0)
	{
		_pmd.VertexList.resize(_pmd.VertexCount);

		if (!stream.read((char*)&_pmd.VertexList[0], (std::streamsize)(sizeof(PMD_Vertex)* _pmd.VertexCount))) return false;
	}

	// index
	if (!stream.read((char*)&_pmd.IndexCount, sizeof(_pmd.IndexCount))) return false;

	if (_pmd.IndexCount > 0)
	{
		_pmd.IndexList.resize(_pmd.IndexCount);

		if (!stream.read((char*)&_pmd.IndexList[0], (std::streamsize)(sizeof(PMD_Index)* _pmd.IndexCount))) return false;
	}

	// materal
	if (!stream.read((char*)&_pmd.MaterialCount, sizeof(_pmd.MaterialCount))) return false;

	if (_pmd.MaterialCount > 0)
	{
		_pmd.MaterialList.resize(_pmd.MaterialCount);

		if (!stream.read((char*)&_pmd.MaterialList[0], (std::streamsize)(sizeof(PMD_Material)* _pmd.MaterialCount))) return false;
	}

	// bone
	if (!stream.read((char*)&_pmd.BoneCount, sizeof(_pmd.BoneCount))) return false;

	if (_pmd.BoneCount > 0)
	{
		_pmd.BoneList.resize(_pmd.BoneCount);

		if (!stream.read((char*)&_pmd.BoneList[0], (std::streamsize)(sizeof(PMD_Bone)* _pmd.BoneCount))) return false;
	}

	// IK
	if (!stream.read((char*)&_pmd.IkCount, sizeof(_pmd.IkCount))) return false;

	if (_pmd.IkCount > 0)
	{
		_pmd.IkList.resize(_pmd.IkCount);

		for (std::size_t i = 0; i < (std::size_t)_pmd.IkCount; i++)
		{
			if (!stream.read((char*)&_pmd.IkList[i].IK, sizeof(_pmd.IkList[i].IK))) return false;
			if (!stream.read((char*)&_pmd.IkList[i].Target, sizeof(_pmd.IkList[i].Target))) return false;
			if (!stream.read((char*)&_pmd.IkList[i].LinkCount, sizeof(_pmd.IkList[i].LinkCount))) return false;
			if (!stream.read((char*)&_pmd.IkList[i].LoopCount, sizeof(_pmd.IkList[i].LoopCount))) return false;
			if (!stream.read((char*)&_pmd.IkList[i].LimitOnce, sizeof(_pmd.IkList[i].LimitOnce))) return false;

			_pmd.IkList[i].LinkList.resize(_pmd.IkList[i].LinkCount);

			if (!stream.read((char*)&_pmd.IkList[i].LinkList[0], (std::streamsize)(sizeof(PMD_Link)* _pmd.IkList[i].LinkCount))) return false;
		}
	}

	// Face
	if (!stream.read((char*)&_pmd.FaceCount, sizeof(_pmd.FaceCount))) return false;

	if (_pmd.FaceCount > 0)
	{
		_pmd.FaceList.resize(_pmd.FaceCount);

		for (std::size_t i = 0; i < (std::size_t)_pmd.FaceCount; i++)
		{
			if (!stream.read((char*)&_pmd.FaceList[i].Name, sizeof(_pmd.FaceList[i].Name))) return false;
			if (!stream.read((char*)&_pmd.FaceList[i].VertexCount, sizeof(_pmd.FaceList[i].VertexCount))) return false;
			if (!stream.read((char*)&_pmd.FaceList[i].Category, sizeof(_pmd.FaceList[i].Category))) return false;

			if (_pmd.FaceList[i].VertexCount > 0)
			{
				_pmd.FaceList[i].VertexList.resize(_pmd.FaceList[i].VertexCount);

				if (!stream.read((char*)&_pmd.FaceList[i].VertexList[0], (std::streamsize)(sizeof(PMD_FaceVertex)* _pmd.FaceList[i].VertexCount))) return false;
			}
		}
	}

	// frame window
	if (!stream.read((char*)&_pmd.FrameWindow.ExpressionListCount, sizeof(_pmd.FrameWindow.ExpressionListCount))) return false;

	if (_pmd.FrameWindow.ExpressionListCount > 0)
	{
		_pmd.FrameWindow.ExpressionList.resize(_pmd.FrameWindow.ExpressionListCount);

		if (!stream.read((char*)&_pmd.FrameWindow.ExpressionList[0], (std::streamsize)(sizeof(PMD_Expression)* _pmd.FrameWindow.ExpressionListCount))) return false;
	}

	if (!stream.read((char*)&_pmd.FrameWindow.NodeNameCount, sizeof(_pmd.FrameWindow.NodeNameCount))) return false;

	if (_pmd.FrameWindow.NodeNameCount > 0)
	{
		_pmd.FrameWindow.NodeNameList.resize(_pmd.FrameWindow.NodeNameCount);

		if (!stream.read((char*)&_pmd.FrameWindow.NodeNameList[0].Name, (std::streamsize)(sizeof(PMD_NodeName)* _pmd.FrameWindow.NodeNameCount))) return false;
	}

	if (!stream.read((char*)&_pmd.FrameWindow.BoneToNodeCount, sizeof(_pmd.FrameWindow.BoneToNodeCount))) return false;

	if (_pmd.FrameWindow.BoneToNodeCount > 0)
	{
		_pmd.FrameWindow.BoneToNodeList.resize(_pmd.FrameWindow.BoneToNodeCount);

		if (!stream.read((char*)&_pmd.FrameWindow.BoneToNodeList[0].Bone, (std::streamsize)(sizeof(PMD_BoneToNode)* _pmd.FrameWindow.BoneToNodeCount))) return false;
	}

	// description
	if (!stream.read((char*)&_pmd.HasDescription, sizeof(_pmd.HasDescription))) return false;

	if (_pmd.HasDescription)
	{
		if (!stream.read((char*)&_pmd.Description.ModelName, sizeof(_pmd.Description.ModelName))) return false;

		if (!stream.read((char*)&_pmd.Description.Comment, sizeof(_pmd.Description.Comment))) return false;

		for (PMD_BoneCount i = 0; i < _pmd.BoneCount; i++)
		{
			PMD_BoneName name;

			if (!stream.read((char*)&name.Name, sizeof(name))) return false;

			_pmd.Description.BoneName.push_back(name);
		}

		for (PMD_uint8_t i = 0; i < _pmd.FrameWindow.ExpressionListCount; i++)
		{
			PMD_FaceName name;

			if (!stream.read((char*)&name.Name, sizeof(name))) return false;

			_pmd.Description.FaceName.push_back(name);
		}

		for (PMD_uint8_t i = 0; i < _pmd.FrameWindow.NodeNameCount; i++)
		{
			PMD_NodeName name;

			if (!stream.read((char*)&name.Name, sizeof(name))) return false;

			_pmd.Description.FrameName.push_back(name);
		}
	}

	// toon
	_pmd.ToonCount = PMD_NUM_TOON;

	_pmd.ToonList.resize(_pmd.ToonCount);

	if (!stream.read((char*)&_pmd.ToonList[0].Name, (std::streamsize)(sizeof(PMD_Toon) * _pmd.ToonCount))) return false;

	// rigidbody
	if (!stream.read((char*)&_pmd.BodyCount, sizeof(_pmd.BodyCount))) return false;

	if (_pmd.BodyCount > 0)
	{
		_pmd.BodyList.resize(_pmd.BodyCount);

		if (!stream.read((char*)&_pmd.BodyList[0], (std::streamsize)(sizeof(PMD_Body)* _pmd.BodyCount))) return false;
	}

	// joint
	if (!stream.read((char*)&_pmd.JointCount, sizeof(_pmd.JointCount))) return false;

	if (_pmd.JointCount > 0)
	{
		_pmd.JointList.resize(_pmd.JointCount);

		if (!stream.read((char*)&_pmd.JointList[0], (std::streamsize)(sizeof(PMD_Body)* _pmd.JointCount))) return false;
	}

	for (std::size_t index = 0; index < _pmd.MaterialList.size(); index++)
	{
		auto& it = _pmd.MaterialList[index];

		auto material = std::make_shared<MaterialProperty>();
		material->set(MATKEY_COLOR_DIFFUSE, it.Diffuse);
		material->set(MATKEY_COLOR_AMBIENT, it.Ambient);
		material->set(MATKEY_COLOR_SPECULAR, it.Specular);
		material->set(MATKEY_OPACITY, it.Opacity);
		material->set(MATKEY_SHININESS, it.Shininess);

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

	PMD_Index* indices = _pmd.IndexList.data();
	PMD_Vertex* vertices = _pmd.VertexList.data();

	auto root = std::make_shared<MeshProperty>();
	auto mesh = root;

	for (std::size_t index = 0; index < _pmd.MaterialList.size(); index++)
	{
		auto& it = _pmd.MaterialList[index];

		Vector3Array points;
		Vector3Array normals;
		Vector2Array texcoords;
		UintArray faces;

		for (PMD_IndexCount i = 0; i < it.FaceVertexCount; i++, indices++)
		{
			PMD_Vertex& v = vertices[*indices];

			points.push_back(v.Position);
			normals.push_back(v.Normal);
			texcoords.push_back(v.UV);
			faces.push_back(i);
		}

		if (mesh->getParent())
		{
			mesh = std::make_shared<MeshProperty>();
			root->addChild(mesh);
		}

		mesh->setMaterialID(index);

		mesh->setVertexArray(points);
		mesh->setNormalArray(normals);
		mesh->setTexcoordArray(texcoords);
		mesh->setFaceArray(faces);
	}

	model.addMesh(root);

	return true;
}

bool
PMDHandler::doSave(Model& model, ostream& stream) noexcept
{
	return false;
}

_NAME_END