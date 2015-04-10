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
PMXHandler::doCanRead(istream& stream) const noexcept
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
PMXHandler::doLoad(Model& model, istream& stream) noexcept
{
    PMX pmx;

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
                if (!stream.read((char*)&vertex.BoneWeight.Weight3, sizeof(vertex.BoneWeight.Weight3))) return false;

                vertex.BoneWeight.Weight2 = 1.0f - vertex.BoneWeight.Weight1;
                vertex.BoneWeight.Weight4 = 1.0f - vertex.BoneWeight.Weight3;
            }
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
            else
            {
                return false;
            }

            if (!stream.read((char*)&bone.EnglishName.length, sizeof(bone.EnglishName.length))) return false;

            if (bone.EnglishName.length > 0 && bone.EnglishName.length < MAX_PATH)
            {
                bone.EnglishName.name.resize(bone.EnglishName.length >> 1);

                if (!stream.read((char*)&bone.EnglishName.name[0], bone.EnglishName.length)) return false;
            }
            else
            {
                return false;
            }

            if (!stream.read((char*)&bone.Position, sizeof(bone.Position))) return false;
            if (!stream.read((char*)&bone.Parent, sizeof(bone.Parent))) return false;
            if (!stream.read((char*)&bone.Level, sizeof(bone.Level))) return false;
            if (!stream.read((char*)&bone.Flag, sizeof(bone.Flag))) return false;

            if (bone.Flag & PMX_BONE_INDEX)
            {
                if (!stream.read((char*)&bone.ConnectedBoneIndex, sizeof(bone.ConnectedBoneIndex))) return false;
            }
            else if (bone.Flag & PMX_BONE_MOVE)
            {
                if (!stream.read((char*)&bone.Offset, sizeof(bone.Offset))) return false;
            }
            else
            {
                if (!stream.read((char*)&bone.ConnectedBoneIndex, sizeof(bone.ConnectedBoneIndex))) return false;
            }

            if (bone.Flag & PMX_BONE_PARENT)
            {
                if (!stream.read((char*)&bone.ProvidedParentBoneIndex, sizeof(bone.ProvidedParentBoneIndex))) return false;
                if (!stream.read((char*)&bone.ProvidedRatio, sizeof(bone.ProvidedRatio))) return false;
            }

            if (bone.Flag & PMX_BONE_AXIS)
            {
                if (!stream.read((char*)&bone.AxisDirection, sizeof(bone.AxisDirection))) return false;
            }

            if (bone.Flag & PMX_BONE_IK)
            {
                if (!stream.read((char*)&bone.IKTargetBoneIndex, sizeof(bone.IKTargetBoneIndex))) return false;
                if (!stream.read((char*)&bone.IKLoopCount, sizeof(bone.IKLoopCount))) return false;
                if (!stream.read((char*)&bone.IKLimitedRadian, sizeof(bone.IKLimitedRadian))) return false;
                if (!stream.read((char*)&bone.IKLinkCount, sizeof(bone.IKLinkCount))) return false;

                if (bone.IKLinkCount > 0)
                {
                    bone.IKList.resize(bone.IKLinkCount);

                    for (std::size_t i = 0; i < bone.IKLinkCount; i++)
                    {
                        if (!stream.read((char*)&bone.IKList[0].BoneIndex, (std::streamsize)sizeof(bone.IKList[0].BoneIndex))) return false;
                        if (!stream.read((char*)&bone.IKList[0].RotateLimited, (std::streamsize)sizeof(bone.IKList[0].RotateLimited))) return false;
                        if (bone.IKList[0].RotateLimited)
                        {
                            if (!stream.read((char*)&bone.IKList[0].MaximumRadian, (std::streamsize)sizeof(bone.IKList[0].MaximumRadian))) return false;
                            if (!stream.read((char*)&bone.IKList[0].MinimumRadian, (std::streamsize)sizeof(bone.IKList[0].MinimumRadian))) return false;
                        }
                    }
                }
            }

            if (bone.Flag & PMX_BONE_ROTATE)
            {
                if (!stream.read((char*)&bone.DimentionXDirection, sizeof(bone.DimentionXDirection))) return false;
                if (!stream.read((char*)&bone.DimentionZDirection, sizeof(bone.DimentionZDirection))) return false;
            }
        }
    }

    PMX_Index* indices = pmx.IndexList.data();
    PMX_Vertex* vertices = pmx.VertexList.data();

    for (std::size_t index = 0; index < pmx.MaterialList.size(); index++)
    {
        auto& it = pmx.MaterialList[index];

        auto material = std::make_shared<MaterialProperty>();

        material->set(MATKEY_COLOR_DIFFUSE, it.Diffuse);
        material->set(MATKEY_COLOR_AMBIENT, it.Ambient);
        material->set(MATKEY_COLOR_SPECULAR, it.Specular);
        material->set(MATKEY_OPACITY, it.Opacity);
        material->set(MATKEY_SHININESS, it.Shininess);

        if (it.TextureIndex != std::numeric_limits<PMX_uint8_t>::max())
        {
            PMX_Texture& texture = pmx.TextureList[it.TextureIndex];

            std::wstring wname((wchar_t*)texture.name.data(), texture.length >> 1);
            std::wstring::size_type substr = wname.find_first_of(L"*");
            if (substr != std::string::npos)
            {
                wname.erase(wname.begin() + substr, wname.end());
            }

            char name[MAX_PATH];

            if ((wname.length() >> 1) > MAX_PATH) return false;

            wcstombs(name, wname.c_str(), wname.length() << 1);

            material->set(MATKEY_TEXTURE_DIFFUSE(0), name);
            material->set(MATKEY_TEXTURE_AMBIENT(0), name);
        }

        Vector3Array points;
        Vector3Array normals;
        Vector2Array texcoords;
        std::vector<Face> faces;

        for (PMX_IndexCount i = 0; i < it.FaceVertexCount; i++, indices++)
        {
            PMX_Vertex& v = vertices[*indices];

            points.push_back(v.Position);
            normals.push_back(v.Normal);
            texcoords.push_back(v.UV);
            faces.push_back(i);
        }

        auto mesh = std::make_shared<MeshProperty>();

        mesh->setMaterialID(index);

        mesh->setVertexArray(points);
        mesh->setNormalArray(normals);
        mesh->setTexcoordArray(texcoords);
        mesh->setFaceArray(faces);

        model.addMesh(mesh);
        model.addMaterial(material);
    }

    return true;
}

bool
PMXHandler::doSave(Model& model, ostream& stream) noexcept
{
    return false;
}

_NAME_END