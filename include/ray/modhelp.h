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
#ifndef _H_MODEL_HELP_H_
#define _H_MODEL_HELP_H_

#include <ray/modcfg.h>
#include <ray/modutil.h>

_NAME_BEGIN

enum TextureType
{
	TT_NONE,
	TT_DIFFUSE,
	TT_SPECULAR,
	TT_AMBIENT,
	TT_EMISSIVE,
	TT_HEIGHT,
	TT_NORMALS,
	TT_SHININESS,
	TT_OPACITY,
	TT_DISPLACEMENT,
	TT_LIGHTMAP,
	TT_REFLECTION,
};

#define MATKEY_NAME               "?mat.name",0,0
#define MATKEY_TWOSIDED           "$mat.twosided",0,0
#define MATKEY_SHADING_MODEL      "$mat.shadingm",0,0
#define MATKEY_ENABLE_WIREFRAME   "$mat.wireframe",0,0
#define MATKEY_BLEND_FUNC         "$mat.blend",0,0
#define MATKEY_OPACITY            "$mat.opacity",0,0
#define MATKEY_BUMPSCALING        "$mat.bumpscaling",0,0
#define MATKEY_SHININESS          "$mat.shininess",0,0
#define MATKEY_REFLECTIVITY       "$mat.reflectivity",0,0
#define MATKEY_SHININESS_STRENGTH "$mat.shinpercent",0,0
#define MATKEY_REFRACTI           "$mat.refracti",0,0
#define MATKEY_COLOR_DIFFUSE      "$clr.diffuse",0,0
#define MATKEY_COLOR_AMBIENT      "$clr.ambient",0,0
#define MATKEY_COLOR_SPECULAR     "$clr.specular",0,0
#define MATKEY_COLOR_EMISSIVE     "$clr.emissive",0,0
#define MATKEY_COLOR_TRANSPARENT  "$clr.transparent",0,0
#define MATKEY_COLOR_REFLECTIVE   "$clr.reflective",0,0

#define MATKEY_TEXTURE(type, N) "$tex.file", type, N

#define MATKEY_TEXTURE_DIFFUSE(N)      MATKEY_TEXTURE(TT_DIFFUSE, N)
#define MATKEY_TEXTURE_SPECULAR(N)     MATKEY_TEXTURE(TT_SPECULAR, N)
#define MATKEY_TEXTURE_AMBIENT(N)      MATKEY_TEXTURE(TT_AMBIENT, N)
#define MATKEY_TEXTURE_EMISSIVE(N)     MATKEY_TEXTURE(TT_EMISSIVE, N)
#define MATKEY_TEXTURE_NORMALS(N)      MATKEY_TEXTURE(TT_NORMALS, N)
#define MATKEY_TEXTURE_HEIGHT(N)       MATKEY_TEXTURE(TT_HEIGHT, N)
#define MATKEY_TEXTURE_SHININESS(N)    MATKEY_TEXTURE(TT_SHININESS, N)
#define MATKEY_TEXTURE_OPACITY(N)      MATKEY_TEXTURE(TT_OPACITY, N)
#define MATKEY_TEXTURE_DISPLACEMENT(N) MATKEY_TEXTURE(TT_DISPLACEMENT, N)
#define MATKEY_TEXTURE_LIGHTMAP(N)     MATKEY_TEXTURE(TT_LIGHTMAP, N)
#define MATKEY_TEXTURE_REFLECTION(N)   MATKEY_TEXTURE(TT_REFLECTION, N)

#define MAX_NUMBER_OF_COLOR_SETS 0x8
#define MAX_NUMBER_OF_TEXTURECOORDS 0x8

enum PropertyTypeInfo
{
	PTI_FLOAT = 0x01,
	PTI_STRING = 0x02,
	PTI_INTEGER = 0x04,
	PTI_BUFFER = 0x08,
};

enum MeshType
{
	MT_POINT,
	MT_LINE,
	MT_TRIANGLES,
	MT_FAN,
	MT_QUAD,
	MT_POLYGON,
};

class EXPORT AnimationProperty final
{
};

class EXPORT CameraProperty final
{
};

class EXPORT LightProperty final
{
};

class EXPORT TextureProperty final
{
public:
	TextureProperty() noexcept {};
	TextureProperty(const std::string& filename) noexcept
		:name(filename)
	{
	}

	TextureProperty(const char* filename, std::size_t length) noexcept
		: name(filename, length)
	{
	}

	std::string name;

	std::uint32_t width;
	std::uint32_t height;
};

class EXPORT MaterialProperty final
{
public:
	struct MaterialParam
	{
		std::string key;

		std::size_t type;
		std::size_t index;
		std::size_t length;
		std::size_t dataType;

		char* data;
	};

public:
	MaterialProperty() noexcept;
	~MaterialProperty() noexcept;

	bool set(const char* key, std::size_t type, std::size_t index, int value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, float value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, const Vector3& value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, const Vector4& value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, const char* value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, const unsigned char* value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, const std::string& value) noexcept;

	bool get(const char* key, std::size_t type, std::size_t index, int& value) const noexcept;
	bool get(const char* key, std::size_t type, std::size_t index, float& value) const noexcept;
	bool get(const char* key, std::size_t type, std::size_t index, Vector3& value) const noexcept;
	bool get(const char* key, std::size_t type, std::size_t index, Vector4& value) const noexcept;
	bool get(const char* key, std::size_t type, std::size_t index, std::string& value) const noexcept;
	bool get(const char* key, std::size_t type, std::size_t index, MaterialParam** out) const noexcept;

private:
	std::vector<MaterialParam*> _properties;
};

class EXPORT MeshProperty final : public Reference<MeshProperty>
{
public:
	MeshProperty() noexcept;
	~MeshProperty() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setParent(MeshPropertyPtr parent) noexcept;
	MeshPropertyPtr getParent() const noexcept;

	void addChild(MeshPropertyPtr child) noexcept;
	void removeChild(MeshPropertyPtr child) noexcept;
	void cleanupChildren() noexcept;
	MeshPropertyPtr findChild(const std::string& name, bool recurse = true) noexcept;

	std::size_t getChildCount() const noexcept;
	MeshPropertys& getChildren() noexcept;

	void setMeshType(MeshType type) noexcept;
	MeshType getMeshType() const noexcept;

	void setMaterialID(std::size_t index) noexcept;
	std::size_t getMaterialID() const noexcept;

	std::size_t getNumVertices() const noexcept;
	std::size_t getNumIndices() const noexcept;

	void setVertexArray(const Vector3Array& array) noexcept;
	void setNormalArray(const Vector3Array& array) noexcept;
	void setColorArray(const Vector3Array& array, std::size_t i = 0) noexcept;
	void setTexcoordArray(const Vector2Array& array, std::size_t i = 0) noexcept;
	void setFaceArray(const UintArray& array) noexcept;

	Vector3Array& getVertexArray() noexcept;
	Vector3Array& getNormalArray() noexcept;
	Vector3Array& getColorArray(std::size_t i = 0) noexcept;
	Vector2Array& getTexcoordArray(std::size_t i = 0) noexcept;
	UintArray& getFaceArray() noexcept;

	const Vector3Array& getVertexArray() const noexcept;
	const Vector3Array& getNormalArray() const noexcept;
	const Vector3Array& getColorArray(std::size_t i = 0) const noexcept;
	const Vector2Array& getTexcoordArray(std::size_t i = 0) const noexcept;
	const UintArray& getFaceArray() const noexcept;

	void makeCircle(float radius, std::uint32_t segments, float thetaStart = 0, float thetaLength = M_PI) noexcept;
	void makePlane(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
	void makePlane(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, unsigned char u, unsigned char v, float udir, float vdir, bool clear = true) noexcept;
	void makeFloor(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
	void makeNoise(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
	void makeCube(float width, float height, float depth, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, std::uint32_t depthSegments = 1) noexcept;
	void makeRing(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart = 0, float thetaLength = M_PI) noexcept;
	void makeSphere(float radius, std::uint32_t widthSegments = 8, std::uint32_t heightSegments = 6, float phiStart = 0.0, float phiLength = M_TWO_PI, float thetaStart = 0, float thetaLength = M_PI) noexcept;
	void makeVolumes(float fovy, float znear, float zfar) noexcept;
	void makeCone(float radius, float height, std::uint32_t segments, float thetaStart = 0, float thetaLength = M_TWO_PI) noexcept;

	void combieInstnace(const CombineInstance& instance) noexcept;
	void mergeVertices() noexcept;

	void computeCentroids() noexcept;
	void computeFaceNormals() noexcept;
	void computeVertexNormals() noexcept;
	void computeVertexNormals(std::size_t width, std::size_t height) noexcept;
	void computeMorphNormals() noexcept;
	void computeTangents() noexcept;
	void computeBoundingBox() noexcept;

	const Bound& getBoundingBox() const noexcept;
	const Bound& getBoundingBoxDownwards() const noexcept;

	void clear() noexcept;
	MeshPropertyPtr clone() noexcept;

private:

	std::string _name;
	std::size_t _materialID;

	MeshType _type;

	Vector3Array _vertices;
	Vector3Array _normals;
	Vector3Array _colors[MAX_NUMBER_OF_COLOR_SETS];
	Vector2Array _texcoords[MAX_NUMBER_OF_TEXTURECOORDS];
	Vector3Array _centroid;
	Vector3Array _facesNormal;

	UintArray _faces;

	Bound _boundingBox;
	Bound _boundingBoxChildren;

	MeshProperty* _parent;
	MeshPropertys _children;
};

_NAME_END

#endif