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
#include <ray/bone.h>

_NAME_BEGIN

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
#define MATKEY_EFFECT             "$mat.effect",0,0
#define MATKEY_COLOR_DIFFUSE      "$clr.diffuse",0,0
#define MATKEY_COLOR_AMBIENT      "$clr.ambient",0,0
#define MATKEY_COLOR_SPECULAR     "$clr.specular",0,0
#define MATKEY_COLOR_EMISSIVE     "$clr.emissive",0,0
#define MATKEY_COLOR_SPHEREMAP    "$clr.spheremap",0,0
#define MATKEY_COLOR_TRANSPARENT  "$clr.transparent",0,0
#define MATKEY_COLOR_REFLECTIVE   "$clr.reflective",0,0

#define MATKEY_TEXTURE(type, N) "$tex.file", type, N

#define MATKEY_TEXTURE_DIFFUSE(N)      MATKEY_TEXTURE(TextureTypeDiffuse, N)
#define MATKEY_TEXTURE_SPECULAR(N)     MATKEY_TEXTURE(TextureTypeSpecular, N)
#define MATKEY_TEXTURE_AMBIENT(N)      MATKEY_TEXTURE(TextureTypeAmbient, N)
#define MATKEY_TEXTURE_EMISSIVE(N)     MATKEY_TEXTURE(TextureTypeEmissive, N)
#define MATKEY_TEXTURE_NORMALS(N)      MATKEY_TEXTURE(TextureTypeNormals, N)
#define MATKEY_TEXTURE_HEIGHT(N)       MATKEY_TEXTURE(TextureTypeHeight, N)
#define MATKEY_TEXTURE_SHININESS(N)    MATKEY_TEXTURE(TextureTypeShininess, N)
#define MATKEY_TEXTURE_OPACITY(N)      MATKEY_TEXTURE(TextureTypeOpacity, N)
#define MATKEY_TEXTURE_DISPLACEMENT(N) MATKEY_TEXTURE(TextureTypeDisplacement, N)
#define MATKEY_TEXTURE_LIGHTMAP(N)     MATKEY_TEXTURE(TextureTypeLightmap, N)
#define MATKEY_TEXTURE_REFLECTION(N)   MATKEY_TEXTURE(TextureTypeReflection, N)

enum ShapeType
{
	ShapeTypeCircle,
	ShapeTypeSquare,
	ShapeTypeCapsule
};

class EXPORT CameraProperty final
{
};

class EXPORT LightProperty final
{
};

class EXPORT RigidbodyProperty
{
public:
	std::string name;

	std::uint32_t bone;

	std::uint8_t group;
	std::uint16_t groupMask;
	ShapeType shape;

	float3 scale;
	float3 position;
	float3 rotation;

	float mass;
	float movementDecay;
	float rotationDecay;
	float elasticity;
	float friction;

	std::uint8_t physicsOperation; //0:Follow Bone (static), 1:Physics Calc. (dynamic), 2: Physics Calc. + Bone position matching
};

class EXPORT JointProperty
{
public:
	std::string name;

	float3 position;
	float3 rotation;

	std::uint32_t bodyIndexA;
	std::uint32_t bodyIndexB;

	float3 movementLowerLimit;
	float3 movementUpperLimit;
	float3 rotationLowerLimit;
	float3 rotationUpperLimit;

	float3 springMovementConstant;
	float3 springRotationConstant;
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

struct EXPORT MeshSubset final
{
	std::uint32_t indicesOffset;
	std::uint32_t indicesCount;
};

class EXPORT MeshProperty final : public std::enable_shared_from_this<MeshProperty>
{
public:
	MeshProperty() noexcept;
	~MeshProperty() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setParent(MeshPropertyPtr parent) noexcept;
	MeshPropertyPtr getParent() const noexcept;

	void addChild(MeshPropertyPtr& child) noexcept;
	void addChild(MeshPropertyPtr&& child) noexcept;
	void removeChild(MeshPropertyPtr& child) noexcept;
	void removeChild(MeshPropertyPtr&& child) noexcept;
	void cleanupChildren() noexcept;
	MeshPropertyPtr findChild(const std::string& name, bool recurse = true) noexcept;

	std::size_t getChildCount() const noexcept;
	MeshPropertys& getChildren() noexcept;
	const MeshPropertys& getChildren() const noexcept;

	std::size_t getNumVertices() const noexcept;
	std::size_t getNumIndices() const noexcept;

	void setVertexArray(const Float3Array& array) noexcept;
	void setNormalArray(const Float3Array& array) noexcept;
	void setColorArray(const Float4Array& array) noexcept;
	void setTangentArray(const Float4Array& array) noexcept;
	void setTexcoordArray(const Float2Array& array, std::size_t n = 0) noexcept;
	void setWeightArray(const VertexWeights& array) noexcept;
	void setFaceArray(const UintArray& array) noexcept;
	void setBindposes(const Float4x4Array& array) noexcept;

	void setVertexArray(Float3Array&& array) noexcept;
	void setNormalArray(Float3Array&& array) noexcept;
	void setColorArray(Float4Array&& array) noexcept;
	void setTangentArray(Float4Array&& array) noexcept;
	void setTexcoordArray(Float2Array&& array, std::size_t n = 0) noexcept;
	void setWeightArray(VertexWeights&& array) noexcept;
	void setFaceArray(UintArray&& array) noexcept;
	void setBindposes(Float4x4Array&& array) noexcept;

	Float3Array& getVertexArray() noexcept;
	Float3Array& getNormalArray() noexcept;
	Float4Array& getTangentArray() noexcept;
	Float4Array& getColorArray() noexcept;
	Float2Array& getTexcoordArray(std::size_t n = 0) noexcept;
	VertexWeights& getWeightArray() noexcept;
	UintArray& getFaceArray() noexcept;
	Float4x4Array& getBindposes() noexcept;

	const Float3Array& getVertexArray() const noexcept;
	const Float3Array& getNormalArray() const noexcept;
	const Float4Array& getTangentArray() const noexcept;
	const Float4Array& getColorArray() const noexcept;
	const Float2Array& getTexcoordArray(std::size_t n = 0) const noexcept;
	const VertexWeights& getWeightArray() const noexcept;
	const Bones& getBoneArray(const Bones& array) const noexcept;
	const UintArray& getFaceArray() const noexcept;
	const Float4x4Array& getBindposes() const noexcept;
	const std::size_t getTexcoordNums() const noexcept;

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

	void combineMeshes(const CombineMesh instances[], std::size_t numInstance, bool merge) noexcept;
	void combineMeshes(const CombineMeshes& instances, bool merge) noexcept;

	void mergeVertices() noexcept;

	void computeFaceNormals() noexcept;
	void computeVertexNormals() noexcept;
	void computeVertexNormals(std::size_t width, std::size_t height) noexcept;
	void computeMorphNormals() noexcept;
	void computeTangents(std::uint8_t texSlot = 0) noexcept;
	void computeTangentQuats(Float4Array& tangentQuat) const noexcept;
	void computeBoundingBox() noexcept;
	void computePlanarUnwrap(std::vector<float2>& lightmap) noexcept;

	const BoundingBox& getBoundingBox() const noexcept;
	const BoundingBox& getBoundingBoxDownwards() const noexcept;

	void clear() noexcept;
	MeshPropertyPtr clone() noexcept;

private:
	std::string _name;

	Float3Array _vertices;
	Float3Array _normals;
	Float4Array _colors;
	Float2Array _texcoords[8];
	Float4Array _tangent;
	Float3Array _facesNormal;
	VertexWeights _weights;
	Float4x4Array _bindposes;

	UintArray _faces;

	Bones _bones;

	BoundingBox _boundingBox;
	BoundingBox _boundingBoxChildren;

	MeshProperty* _parent;
	MeshPropertys _children;
};

_NAME_END

#endif