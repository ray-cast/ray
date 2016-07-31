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
#include <ray/modhelp.h>

_NAME_BEGIN

MaterialProperty::MaterialProperty() noexcept
{
}

MaterialProperty::~MaterialProperty() noexcept
{
	for (auto& it : _properties)
	{
		delete it->data;
		delete it;
	}
}

bool
MaterialProperty::set(const char* key, std::size_t type, std::size_t index, int value) noexcept
{
	assert(nullptr != key);

	auto it = _properties.begin();
	auto end = _properties.end();

	for (; it != end; ++it)
	{
		if ((*it)->key == key &&
			(*it)->type == type &&
			(*it)->index == index)
		{
			delete (*it);
			_properties.erase(it);
			break;
		}
	}

	auto prop = make_scope<MaterialParam>();
	prop->key = key;
	prop->type = type;
	prop->index = index;
	prop->length = sizeof(int);
	prop->dataType = PropertyTypeInfoInt;
	prop->data = new char[prop->length];

	std::memcpy(prop->data, &value, prop->length);

	_properties.push_back(prop.dismiss());

	return true;
}

bool
MaterialProperty::set(const char* key, std::size_t type, std::size_t index, float value) noexcept
{
	assert(nullptr != key);

	auto it = _properties.begin();
	auto end = _properties.end();

	for (; it != end; ++it)
	{
		if ((*it)->key == key &&
			(*it)->type == type &&
			(*it)->index == index)
		{
			delete (*it);
			_properties.erase(it);
			break;
		}
	}

	auto prop = make_scope<MaterialParam>();

	prop->key = key;
	prop->type = type;
	prop->index = index;
	prop->length = sizeof(float);
	prop->dataType = PropertyTypeInfoFloat;
	prop->data = new char[prop->length];

	std::memcpy(prop->data, &value, prop->length);

	_properties.push_back(prop.dismiss());

	return true;
}

bool
MaterialProperty::set(const char* key, std::size_t type, std::size_t index, const Vector3& value) noexcept
{
	assert(nullptr != key);

	auto it = _properties.begin();
	auto end = _properties.end();

	for (; it != end; ++it)
	{
		if ((*it)->key == key &&
			(*it)->type == type &&
			(*it)->index == index)
		{
			delete (*it);
			_properties.erase(it);
			break;
		}
	}

	auto prop = make_scope<MaterialParam>();

	prop->key = key;
	prop->type = type;
	prop->index = index;
	prop->length = sizeof(Vector3);
	prop->dataType = PropertyTypeInfoFloat | PropertyTypeInfoBuffer;
	prop->data = new char[prop->length];

	std::memcpy(prop->data, &value, prop->length);

	_properties.push_back(prop.dismiss());

	return true;
}

bool
MaterialProperty::set(const char* key, std::size_t type, std::size_t index, const Vector4& value) noexcept
{
	assert(nullptr != key);

	auto it = _properties.begin();
	auto end = _properties.end();

	for (; it != end; ++it)
	{
		if ((*it)->key == key &&
			(*it)->type == type &&
			(*it)->index == index)
		{
			delete (*it);
			_properties.erase(it);
			break;
		}
	}

	auto prop = make_scope<MaterialParam>();

	prop->key = key;
	prop->type = type;
	prop->index = index;
	prop->length = sizeof(Vector4);
	prop->dataType = PropertyTypeInfoFloat | PropertyTypeInfoBuffer;
	prop->data = new char[prop->length];

	std::memcpy(prop->data, &value, prop->length);

	_properties.push_back(prop.dismiss());

	return true;
}

bool
MaterialProperty::set(const char* key, std::size_t type, std::size_t index, const char* value) noexcept
{
	assert(nullptr != key);

	std::string str(value);
	return this->set(key, type, index, str);
}

bool
MaterialProperty::set(const char* key, std::size_t type, std::size_t index, const unsigned char* value) noexcept
{
	assert(nullptr != key);

	std::string str((char*)value);
	return this->set(key, type, index, str);
}

bool
MaterialProperty::set(const char* key, std::size_t type, std::size_t index, const std::string& value) noexcept
{
	assert(nullptr != key);

	auto it = _properties.begin();
	auto end = _properties.end();

	for (; it != end; ++it)
	{
		if ((*it)->key == key &&
			(*it)->type == type &&
			(*it)->index == index)
		{
			delete (*it);
			_properties.erase(it);
			break;
		}
	}

	auto prop = make_scope<MaterialParam>();

	prop->key = key;
	prop->type = type;
	prop->index = index;
	prop->length = value.length();
	prop->dataType = PropertyTypeInfoString;
	prop->data = new char[prop->length];

	std::memcpy(prop->data, value.data(), prop->length);

	_properties.push_back(prop.dismiss());

	return true;
}

bool
MaterialProperty::get(const char* key, std::size_t type, std::size_t index, int& value) const noexcept
{
	assert(nullptr != key);

	MaterialParam* prop = nullptr;
	if (this->get(key, type, index, &prop))
	{
		if (prop->dataType == PropertyTypeInfoInt)
		{
			if (prop->length == sizeof(int))
			{
				std::memcpy(&value, prop->data, prop->length);
				return true;
			}
		}
	}

	return false;
}

bool
MaterialProperty::get(const char* key, std::size_t type, std::size_t index, float& value) const noexcept
{
	assert(nullptr != key);

	MaterialParam* prop = nullptr;
	if (this->get(key, type, index, &prop))
	{
		if (prop->dataType == PropertyTypeInfoFloat)
		{
			if (prop->length == sizeof(float))
			{
				std::memcpy(&value, prop->data, prop->length);
				return true;
			}
		}
	}

	return false;
}

bool
MaterialProperty::get(const char* key, std::size_t type, std::size_t index, Vector3& value) const noexcept
{
	assert(nullptr != key);

	MaterialParam* prop = nullptr;
	if (this->get(key, type, index, &prop))
	{
		if (prop->dataType & PropertyTypeInfoFloat &&
			prop->dataType & PropertyTypeInfoBuffer)
		{
			if (prop->length == sizeof(Vector3))
			{
				std::memcpy(&value, prop->data, prop->length);
				return true;
			}
		}
	}

	return false;
}

bool
MaterialProperty::get(const char* key, std::size_t type, std::size_t index, Vector4& value) const noexcept
{
	assert(nullptr != key);

	MaterialParam* prop = nullptr;
	if (this->get(key, type, index, &prop))
	{
		if (prop->dataType & PropertyTypeInfoFloat &&
			prop->dataType & PropertyTypeInfoBuffer)
		{
			if (prop->length == sizeof(Vector4))
			{
				std::memcpy(&value, prop->data, prop->length);
				return true;
			}
		}
	}

	return false;
}

bool
MaterialProperty::get(const char* key, std::size_t type, std::size_t index, std::string& value) const noexcept
{
	assert(nullptr != key);

	MaterialParam* prop = nullptr;
	if (this->get(key, type, index, &prop))
	{
		if (prop->dataType == PropertyTypeInfoString)
		{
			value.assign(prop->data, prop->length);
			return true;
		}
	}

	return false;
}

bool
MaterialProperty::get(const char* key, std::size_t type, std::size_t index, MaterialParam** out) const noexcept
{
	assert(nullptr != key);
	assert(nullptr != out);

	for (auto& it : _properties)
	{
		if (it->key == key &&
			it->type == type &&
			it->index == index)
		{
			*out = it;
			return true;
		}
	}

	return false;
}

MeshProperty::MeshProperty() noexcept
	: _parent(nullptr)
{
}

MeshProperty::~MeshProperty() noexcept
{
}

void
MeshProperty::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
MeshProperty::getName() const noexcept
{
	return _name;
}

void
MeshProperty::setParent(MeshPropertyPtr parent) noexcept
{
	assert(this != parent.get());

	if (_parent != parent.get())
	{
		if (parent == nullptr)
		{
			auto it = _parent->_children.begin();
			auto end = _parent->_children.end();

			for (; it != end; ++it)
			{
				if ((*it).get() == this)
				{
					_parent->_children.erase(it);
					break;
				}
			}

			_parent = nullptr;
		}
		else
		{
			_parent = parent.get();
			_parent->_children.push_back(std::dynamic_pointer_cast<MeshProperty>(this->shared_from_this()));
		}
	}
}

MeshPropertyPtr
MeshProperty::getParent() const noexcept
{
	if (_parent)
		return std::dynamic_pointer_cast<MeshProperty>(_parent->shared_from_this());
	return nullptr;
}

void
MeshProperty::addChild(MeshPropertyPtr& entity) noexcept
{
	assert(entity);
	entity->setParent(std::dynamic_pointer_cast<MeshProperty>(this->shared_from_this()));
}

void
MeshProperty::addChild(MeshPropertyPtr&& entity) noexcept
{
	assert(entity);
	entity->setParent(std::dynamic_pointer_cast<MeshProperty>(this->shared_from_this()));
}

void
MeshProperty::removeChild(MeshPropertyPtr& entity) noexcept
{
	assert(entity);

	auto it = _children.begin();
	auto end = _children.end();
	for (; it != end; ++it)
	{
		if ((*it) == entity)
		{
			(*it)->setParent(nullptr);
			break;
		}
	}

	if (it != end)
	{
		_children.erase(it);
	}
}

void
MeshProperty::removeChild(MeshPropertyPtr&& entity) noexcept
{
	assert(entity);

	auto it = _children.begin();
	auto end = _children.end();
	for (; it != end; ++it)
	{
		if ((*it) == entity)
		{
			(*it)->setParent(nullptr);
			break;
		}
	}

	if (it != end)
	{
		_children.erase(it);
	}
}

void
MeshProperty::cleanupChildren() noexcept
{
	for (auto& it : _children)
	{
		it.reset();
	}

	_children.clear();
}

MeshPropertyPtr
MeshProperty::findChild(const std::string& name, bool recuse) noexcept
{
	for (auto& it : _children)
	{
		if (it->getName() == name)
		{
			return it;
		}
	}

	if (recuse)
	{
		for (auto& it : _children)
		{
			auto result = it->findChild(name, recuse);
			if (result)
			{
				return result;
			}
		}
	}

	return nullptr;
}

std::size_t
MeshProperty::getChildCount() const noexcept
{
	return _children.size();
}

MeshPropertys&
MeshProperty::getChildren() noexcept
{
	return _children;
}

const MeshPropertys&
MeshProperty::getChildren() const noexcept
{
	return _children;
}

std::size_t
MeshProperty::getNumVertices() const noexcept
{
	return _vertices.size();
}

std::size_t
MeshProperty::getNumIndices() const noexcept
{
	return _faces.size();
}

void
MeshProperty::setVertexArray(const Float3Array& array) noexcept
{
	_vertices = array;
}

void
MeshProperty::setNormalArray(const Float3Array& array) noexcept
{
	_normals = array;
}

void
MeshProperty::setColorArray(const Float4Array& array) noexcept
{
	_colors = array;
}

void
MeshProperty::setTangentArray(const Float4Array& array) noexcept
{
	_tangent = array;
}

void
MeshProperty::setTangentQuatArray(const Float4Array& array) noexcept
{
	_tangentQuat = array;
}

void
MeshProperty::setTexcoordArray(const Float2Array& array) noexcept
{
	_texcoords = array;
}

void
MeshProperty::setFaceArray(const UintArray& array) noexcept
{
	_faces = array;
}

void
MeshProperty::setBindposes(const Float4x4Array& array) noexcept
{
	_bindposes = array;
}

void
MeshProperty::setWeightArray(const VertexWeights& array) noexcept
{
	_weights = array;
}

void
MeshProperty::setVertexArray(Float3Array&& array) noexcept
{
	_vertices = std::move(array);
}

void
MeshProperty::setNormalArray(Float3Array&& array) noexcept
{
	_normals = std::move(array);
}

void
MeshProperty::setColorArray(Float4Array&& array) noexcept
{
	_colors = std::move(array);
}

void
MeshProperty::setTangentArray(Float4Array&& array) noexcept
{
	_tangent = std::move(array);
}

void
MeshProperty::setTangentQuatArray(Float4Array&& array) noexcept
{
	_tangentQuat = std::move(array);
}

void
MeshProperty::setTexcoordArray(Float2Array&& array) noexcept
{
	_texcoords = std::move(array);
}

void
MeshProperty::setFaceArray(UintArray&& array) noexcept
{
	_faces = std::move(array);
}

void
MeshProperty::setWeightArray(VertexWeights&& array) noexcept
{
	_weights = std::move(array);
}

void
MeshProperty::setBindposes(Float4x4Array&& array) noexcept
{
	_bindposes = std::move(array);
}

Float3Array&
MeshProperty::getVertexArray() noexcept
{
	return _vertices;
}

Float3Array&
MeshProperty::getNormalArray() noexcept
{
	return _normals;
}

Float4Array&
MeshProperty::getTangentArray() noexcept
{
	return _tangent;
}

Float4Array&
MeshProperty::getTangentQuatArray() noexcept
{
	return _tangentQuat;
}

Float4Array&
MeshProperty::getColorArray() noexcept
{
	return _colors;
}

std::vector<Vector2>&
MeshProperty::getTexcoordArray() noexcept
{
	return _texcoords;
}

VertexWeights&
MeshProperty::getWeightArray() noexcept
{
	return _weights;
}

UintArray&
MeshProperty::getFaceArray() noexcept
{
	return _faces;
}

Float4x4Array&
MeshProperty::getBindposes() noexcept
{
	return _bindposes;
}

const Float3Array&
MeshProperty::getVertexArray() const noexcept
{
	return _vertices;
}

const Float3Array&
MeshProperty::getNormalArray() const noexcept
{
	return _normals;
}

const Float4Array&
MeshProperty::getTangentArray() const noexcept
{
	return _tangent;
}

const Float4Array&
MeshProperty::getTangentQuatArray() const noexcept
{
	return _tangentQuat;
}

const Float4Array&
MeshProperty::getColorArray() const noexcept
{
	return _colors;
}

const Float2Array&
MeshProperty::getTexcoordArray() const noexcept
{
	return _texcoords;
}

const VertexWeights&
MeshProperty::getWeightArray() const noexcept
{
	return _weights;
}

const Float4x4Array&
MeshProperty::getBindposes() const noexcept
{
	return _bindposes;
}

const Bones&
MeshProperty::getBoneArray(const Bones& array) const noexcept
{
	return _bones;
}

const UintArray&
MeshProperty::getFaceArray() const noexcept
{
	return _faces;
}

const Bound&
MeshProperty::getBoundingBox() const noexcept
{
	return _boundingBox;
}

const Bound&
MeshProperty::getBoundingBoxDownwards() const noexcept
{
	return _boundingBoxChildren;
}

void
MeshProperty::clear() noexcept
{
	_vertices = Float3Array();
	_normals = Float3Array();
	_colors = Float4Array();
	_texcoords = Float2Array();
	_tangent = Float4Array();
	_facesNormal = Float3Array();
	_faces = UintArray();
}

MeshPropertyPtr
MeshProperty::clone() noexcept
{
	auto mesh = std::make_shared<MeshProperty>();
	mesh->setName(this->getName());
	mesh->setVertexArray(this->getVertexArray());
	mesh->setNormalArray(this->getNormalArray());
	mesh->setColorArray(this->getColorArray());
	mesh->setTexcoordArray(this->getTexcoordArray());
	mesh->setWeightArray(this->getWeightArray());
	mesh->setTangentArray(this->getTangentArray());
	mesh->setBindposes(this->getBindposes());
	mesh->setFaceArray(this->getFaceArray());
	mesh->_boundingBox = this->_boundingBox;
	mesh->_boundingBoxChildren = this->_boundingBoxChildren;

	for (auto& it : _children)
	{
		mesh->addChild(it->clone());
	}

	return mesh;
}

void
MeshProperty::makeCircle(float radius, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
{
	this->clear();

	for (std::uint32_t i = 0; i <= segments; i++)
	{
		Vector3 v;

		float segment = thetaStart + i / segments * thetaLength;

		v.x = radius * cos(segment);
		v.y = radius * sin(segment);
		v.z = 0;

		_vertices.push_back(v);

		_texcoords.emplace_back((v.x / radius + 1), (v.y / radius + 1) / 2);
	}

	for (std::uint32_t i = 1; i <= segments; i++)
	{
		std::uint32_t v1 = i;
		std::uint32_t v2 = i + 1;
		std::uint32_t v3 = 0;

		_faces.push_back(v1);
		_faces.push_back(v2);
		_faces.push_back(v3);

		_normals.push_back(Vector3::UnitZ);
		_normals.push_back(Vector3::UnitZ);
		_normals.push_back(Vector3::UnitZ);
	}

	this->computeBoundingBox();
}

void
MeshProperty::makePlane(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
{
	this->clear();

	float widthHalf = width * 0.5f;
	float heightHalf = height * 0.5f;

	std::uint32_t gridX = widthSegments;
	std::uint32_t gridY = heightSegments;

	std::uint32_t gridX1 = gridX + 1;
	std::uint32_t gridY1 = gridY + 1;

	float segmentWidth = width / gridX;
	float segmentHeight = height / gridY;

	const Vector3& normal = Vector3::UnitZ;

	for (std::uint32_t iz = 0; iz < gridY1; iz++)
	{
		for (std::uint32_t ix = 0; ix < gridX1; ix++)
		{
			float x = ix * segmentWidth - widthHalf;
			float z = iz * segmentHeight - heightHalf;

			_vertices.emplace_back(x, z, 0.0f);

			_normals.push_back(normal);
		}
	}

	for (std::uint32_t iy = 0; iy < gridY; iy++)
	{
		for (std::uint32_t ix = 0; ix < gridX; ix++)
		{
			_texcoords.emplace_back((float)ix / gridX, (float)(1 - (iy + 1)) / gridY);
			_texcoords.emplace_back((float)(ix + 1) / gridX, (float)(1 - (iy + 1)) / gridY);
			_texcoords.emplace_back((float)ix / gridX, (float)(1 - iy) / gridY);
			_texcoords.emplace_back((float)(ix + 1) / gridX, (float)(1 - iy) / gridY);

			std::int32_t a = static_cast<std::int32_t>(ix + gridX1 * iy);
			std::int32_t b = static_cast<std::int32_t>(ix + gridX1 * (iy + 1));
			std::int32_t c = static_cast<std::int32_t>(ix + gridX1 * (iy + 1) + 1);
			std::int32_t d = static_cast<std::int32_t>(ix + gridX1 * iy + 1);

			_faces.push_back(a);
			_faces.push_back(b);
			_faces.push_back(c);

			_faces.push_back(c);
			_faces.push_back(d);
			_faces.push_back(a);
		}
	}

	this->computeBoundingBox();
}

void
MeshProperty::makePlane(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, unsigned char u, unsigned char v, float udir, float vdir, bool clear) noexcept
{
	if (clear)
		this->clear();

	std::uint32_t gridX = widthSegments;
	std::uint32_t gridY = heightSegments;

	float widthHalf = width * 0.5f;
	float heightHalf = height * 0.5f;

	std::uint8_t w = 0;
	if ((u == 'x' && v == 'y') || (u == 'y' && v == 'x'))
	{
		w = 'z';
	}
	else if ((u == 'x' && v == 'z') || (u == 'z' && v == 'x'))
	{
		w = 'y';
		gridY = depthSegments;
	}
	else if ((u == 'z' && v == 'y') || (u == 'y' && v == 'z'))
	{
		w = 'x';
		gridX = depthSegments;
	}
	else
	{
		assert(false);
	}

	std::uint32_t gridX1 = gridX + 1;
	std::uint32_t gridY1 = gridY + 1;

	std::uint32_t offset = (std::uint32_t)_vertices.size();

	float segmentWidth = width / gridX;
	float segmentHeight = height / gridY;

	Vector3 n = Vector3::Zero;
	n.getComponent(w) = depth >= 0.0f ? 1.0f : -1.0f;

	for (std::uint32_t iy = 0; iy < gridY1; iy++)
	{
		for (std::uint32_t ix = 0; ix < gridX1; ix++)
		{
			Vector3 vec;
			vec.getComponent(u) = (ix * segmentWidth - widthHalf) * udir;
			vec.getComponent(v) = (iy * segmentHeight - heightHalf) * vdir;
			vec.getComponent(w) = depth;

			_vertices.push_back(vec);
			_normals.push_back(n);
		}
	}

	for (std::uint32_t iy = 0; iy < gridY; iy++)
	{
		for (std::uint32_t ix = 0; ix < gridX; ix++)
		{
			std::int32_t a = static_cast<std::int32_t>(ix + gridX1 * iy);
			std::int32_t b = static_cast<std::int32_t>(ix + gridX1 * (iy + 1));
			std::int32_t c = static_cast<std::int32_t>(ix + gridX1 * (iy + 1) + 1);
			std::int32_t d = static_cast<std::int32_t>(ix + gridX1 * iy + 1);

			_faces.push_back((std::int32_t)(a + offset));
			_faces.push_back((std::int32_t)(b + offset));
			_faces.push_back((std::int32_t)(c + offset));

			_faces.push_back((std::int32_t)(c + offset));
			_faces.push_back((std::int32_t)(d + offset));
			_faces.push_back((std::int32_t)(a + offset));

			_texcoords.emplace_back((float)ix / gridX, (float)(1 - (iy + 1)) / gridY);
			_texcoords.emplace_back((float)(ix + 1) / gridX, (float)(1 - (iy + 1)) / gridY);
			_texcoords.emplace_back((float)ix / gridX, (float)(1 - iy) / gridY);
			_texcoords.emplace_back((float)(ix + 1) / gridX, (float)(1 - iy) / gridY);
		}
	}
}

void
MeshProperty::makeFloor(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
{
	this->clear();
	this->makePlane(width, height, 0, widthSegments, 0, heightSegments, 'x', 'z', 1.0, 1.0);

	this->computeTangents();
	this->computeTangentQuats();
	this->computeBoundingBox();
}

void
MeshProperty::makeNoise(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
{
	this->clear();

	PerlinNoise2 PL;

	std::uint32_t gridX = widthSegments;
	std::uint32_t gridY = heightSegments;

	float widthHalf = width * 0.5f;
	float heightHalf = height * 0.5f;
	float widthSegment = width / widthSegments;
	float heightSegment = height / heightSegments;

	float invfre = 0.013f;

	auto makePosition = [&](float x, float y, float z) -> Vector3
	{
		float posX = -widthHalf + x * widthSegment;
		float posZ = -heightHalf + z * heightSegment;

		return Vector3(posX, y, posZ);
	};

	std::uint32_t gridX1 = gridX + 1;
	std::uint32_t gridY1 = gridY + 1;

	for (std::uint32_t y = 0; y < gridX1; y++)
	{
		for (std::uint32_t x = 0; x < gridY1; x++)
		{
			float accum = 0;

			accum += PL.noise(x * invfre, y * invfre, 0.8f) * 1.0f;
			accum += PL.noise(x * invfre, y * invfre, 3.0f) * 0.17f;
			//accum += PL.noise(x * invfre, y * invfre, 3.4f) * 0.12f;
			//accum += PL.noise(x * invfre, y * invfre, 3.7f) * 0.1f;
			//accum += PL.noise(x * invfre + 0.3f, y * invfre + 0.3f, 1.0f) * 0.9f;

			accum *= 0.1f;
			accum += 0.5f;

			accum *= width;

			accum -= heightHalf;

			_vertices.push_back(makePosition((float)x, accum, (float)y));
			_texcoords.push_back(Vector2((float)x, (float)y));
		}
	}

	for (std::uint32_t iy = 0; iy < gridX; iy++)
	{
		for (std::uint32_t ix = 0; ix < gridY; ix++)
		{
			_faces.push_back(ix + gridX1 * iy);
			_faces.push_back(ix + gridX1 * (iy + 1));
			_faces.push_back(ix + gridX1 * (iy + 1) + 1);

			_faces.push_back(ix + gridX1 * iy);
			_faces.push_back(ix + gridX1 * (iy + 1) + 1);
			_faces.push_back(ix + gridX1 * iy + 1);
		}
	}

	this->computeVertexNormals();
	this->computeTangents();
	this->computeTangentQuats();
	this->computeBoundingBox();
}

void
MeshProperty::makeCube(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments) noexcept
{
	this->clear();

	float widthHalf = width * 0.5f;
	float heightHalf = height * 0.5f;
	float depthHalf = depth * 0.5f;

	this->makePlane(depth, height, widthHalf, widthSegments, heightSegments, depthSegments, 'z', 'y', -1, -1, false); // px
	this->makePlane(depth, height, -widthHalf, widthSegments, heightSegments, depthSegments, 'z', 'y', 1, -1, false); // nx
	this->makePlane(width, depth, heightHalf, widthSegments, heightSegments, depthSegments, 'x', 'z', 1, 1, false); // py
	this->makePlane(width, depth, -heightHalf, widthSegments, heightSegments, depthSegments, 'x', 'z', 1, -1, false); // ny
	this->makePlane(width, height, depthHalf, widthSegments, heightSegments, depthSegments, 'x', 'y', 1, -1, false); // pz
	this->makePlane(width, height, -depthHalf, widthSegments, heightSegments, depthSegments, 'x', 'y', -1, -1, false); // nz

	this->computeTangents();
	this->computeTangentQuats();
	this->computeBoundingBox();
}

void
MeshProperty::makeRing(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart, float thetaLength) noexcept
{
	this->clear();

	innerRadius = innerRadius || 0;
	outerRadius = outerRadius || 50;

	thetaStart = thetaStart ? thetaStart : 0;
	thetaLength = thetaLength ? thetaLength : M_PI * 2;

	thetaSegments = thetaSegments ? std::max((std::uint32_t)3, thetaSegments) : 8;
	phiSegments = phiSegments ? std::max((std::uint32_t)3, phiSegments) : 8;

	for (size_t i = 0; i <= phiSegments; i++)
	{
		for (size_t j = 0; j <= thetaSegments; j++)
		{
		}
	}
}

void
MeshProperty::makeSphere(float radius, std::uint32_t widthSegments, std::uint32_t heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength) noexcept
{
	this->clear();

	std::vector<std::uint32_t> vertices;

	for (std::uint32_t y = 0; y <= heightSegments; y++)
	{
		for (std::uint32_t x = 0; x <= widthSegments; x++)
		{
			float u = (float)(x) / widthSegments;
			float v = (float)(y) / heightSegments;

			Vector3 vertex;
			vertex.x = -radius * sin(thetaStart + v * thetaLength) * cos(phiStart + u * phiLength);
			vertex.y =  radius * cos(thetaStart + v * thetaLength);
			vertex.z =  radius * sin(thetaStart + v * thetaLength) * sin(phiStart + u * phiLength);

			_vertices.push_back(vertex);
			_normals.push_back(math::normalize(vertex));
			_texcoords.emplace_back(u, 1 - v);

			vertices.push_back((std::uint32_t)_vertices.size() - 1);
		}
	}

	for (std::uint32_t y = 0; y < heightSegments; y++)
	{
		for (std::uint32_t x = 0; x < widthSegments; x++)
		{
			std::uint32_t v1 = vertices[y * (widthSegments + 1) + x];
			std::uint32_t v2 = vertices[y * (widthSegments + 1) + x + 1];
			std::uint32_t v3 = vertices[(y + 1) * (widthSegments + 1) + x];
			std::uint32_t v4 = vertices[(y + 1) * (widthSegments + 1) + x + 1];

			if (math::abs((_vertices)[v2].y) == radius)
			{
				_faces.push_back(v2);
				_faces.push_back(v3);
				_faces.push_back(v4);
			}
			else if (math::abs((_vertices)[v3].y) == radius)
			{
				_faces.push_back(v2);
				_faces.push_back(v1);
				_faces.push_back(v3);
			}
			else
			{
				_faces.push_back(v2);
				_faces.push_back(v3);
				_faces.push_back(v4);

				_faces.push_back(v2);
				_faces.push_back(v1);
				_faces.push_back(v3);
			}
		}
	}

	this->computeTangents();
	this->computeTangentQuats();
	this->computeBoundingBox();
}

void
MeshProperty::makeVolumes(float fovy, float znear, float zfar) noexcept
{
	this->clear();

	float tanFovy_2 = tan(fovy * M_PI / 360.0f);

	_vertices.emplace_back(tanFovy_2 * znear, tanFovy_2 * znear, -znear);
	_vertices.emplace_back(-tanFovy_2 * znear, tanFovy_2 * znear, -znear);
	_vertices.emplace_back(-tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
	_vertices.emplace_back(tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);

	_vertices.emplace_back(tanFovy_2 * znear, -tanFovy_2 * znear, -znear);
	_vertices.emplace_back(tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);
	_vertices.emplace_back(-tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);
	_vertices.emplace_back(-tanFovy_2 * znear, -tanFovy_2 * znear, -znear);

	_vertices.emplace_back(tanFovy_2 * znear, -tanFovy_2 * znear, -znear);
	_vertices.emplace_back(tanFovy_2 * znear, tanFovy_2 * znear, -znear);
	_vertices.emplace_back(tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
	_vertices.emplace_back(tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);

	_vertices.emplace_back(-tanFovy_2 * znear, -tanFovy_2 * znear, -znear);
	_vertices.emplace_back(-tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);
	_vertices.emplace_back(-tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
	_vertices.emplace_back(-tanFovy_2 * znear, tanFovy_2 * znear, -znear);

	_vertices.emplace_back(-tanFovy_2 * znear, tanFovy_2 * znear, -znear);
	_vertices.emplace_back(tanFovy_2 * znear, tanFovy_2 * znear, -znear);
	_vertices.emplace_back(tanFovy_2 * znear, -tanFovy_2 * znear, -znear);
	_vertices.emplace_back(-tanFovy_2 * znear, -tanFovy_2 * znear, -znear);

	_vertices.emplace_back(tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
	_vertices.emplace_back(-tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
	_vertices.emplace_back(-tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);
	_vertices.emplace_back(tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);

	this->computeBoundingBox();
}

void
MeshProperty::makeCone(float radius, float height, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
{
	this->clear();

	_vertices.emplace_back(0.0f, 0.0f, 0.0f);
	_vertices.emplace_back(0.0f, 0.0f, -height);

	_normals.emplace_back(0.0f, 0.0f, 0.0f);
	_normals.emplace_back(0.0f, 0.0f, -1.0f);

	_texcoords.emplace_back(0.0f, 0.0f);
	_texcoords.emplace_back(1.0f, 1.0f);

	float segment = thetaLength / segments;

	for (std::uint32_t i = 0; i <= segments; i++)
	{
		float sin;
		float cos;

		math::sinCos(&sin, &cos, thetaStart + i * segment);

		Vector3 v;
		v.x = radius * cos;
		v.y = -radius * sin;
		v.z = 0;

		_vertices.push_back(v);
		_normals.push_back(v);

		_texcoords.emplace_back((v.x / radius + 1), (v.y / radius + 1) / 2);
	}

	for (std::uint32_t i = 2; i <= segments + 1; i++)
	{
		std::uint32_t v1 = i;
		std::uint32_t v2 = 0;
		std::uint32_t v3 = i + 1;

		_faces.push_back(v1);
		_faces.push_back(v2);
		_faces.push_back(v3);
	}

	for (std::uint32_t i = 2; i <= segments + 1; i++)
	{
		std::uint32_t v1 = i;
		std::uint32_t v2 = 1;
		std::uint32_t v3 = i + 1;

		_faces.push_back(v3);
		_faces.push_back(v2);
		_faces.push_back(v1);
	}

	this->computeTangents();
	this->computeTangentQuats();
	this->computeBoundingBox();
}

void
MeshProperty::combineMeshes(const CombineMesh instances[], std::size_t numInstance, bool merge) noexcept
{
	this->clear();

	std::size_t offsetVertices = 0;
	std::size_t offsetIndices = 0;

	std::size_t maxIndices = 0;
	std::size_t maxVertices = 0;

	bool hasVertices = false;
	bool hasNormal = false;
	bool hasTangent = false;
	bool hasTangentQuat = false;
	bool hasTexcoord = false;
	bool hasFace = false;
	bool hasWeight = false;

	for (std::size_t i = 0; i < numInstance; i++)
	{
		auto mesh = instances[i].getMesh();
		if (mesh)
		{
			maxVertices += mesh->getNumVertices();
			maxIndices += mesh->getNumIndices();

			hasVertices |= !mesh->getVertexArray().empty();
			hasNormal |= !mesh->getNormalArray().empty();
			hasTangent |= !mesh->getTangentArray().empty();
			hasTangentQuat |= !mesh->getTangentQuatArray().empty();
			hasTexcoord |= !mesh->getTexcoordArray().empty();
			hasFace |= !mesh->getFaceArray().empty();
			hasWeight |= !mesh->getWeightArray().empty();

			if (!merge) break;
		}
	}

	if (hasVertices)
		this->_vertices.resize(maxVertices);

	if (hasNormal)
		this->_normals.resize(maxVertices);

	if (hasTangent)
		this->_tangent.resize(maxVertices);

	if (hasTangentQuat)
		this->_tangentQuat.resize(maxVertices);

	if (hasTexcoord)
		this->_texcoords.resize(maxVertices);

	if (hasWeight)
		this->_weights.resize(maxVertices);

	if (hasFace)
		this->_faces.resize(maxIndices);

	for (std::size_t i = 0; i < numInstance; i++)
	{
		auto mesh = instances[i].getMesh();
		if (!mesh)
			continue;

		std::size_t numVertex = mesh->getNumVertices();
		std::size_t numIndex = mesh->getNumIndices();

		std::vector<float3>* vertices_;
		std::vector<float3>* normals_;
		std::vector<float4>* tangent_;
		std::vector<float4>* tangentQuat_;
		std::vector<float4>* colors_;
		std::vector<float2>* texcoords_;
		std::vector<VertexWeight>* weights_;
		std::vector<float4x4>* bindposes_;

		UintArray* faces_;

		if (!merge && i != 0)
		{
			auto child = std::make_shared<MeshProperty>();
			child->getVertexArray().resize(mesh->getVertexArray().size());
			child->getNormalArray().resize(mesh->getNormalArray().size());
			child->getTangentArray().resize(mesh->getTangentArray().size());
			child->getTangentQuatArray().resize(mesh->getTangentQuatArray().size());
			child->getColorArray().resize(mesh->getColorArray().size());
			child->getTexcoordArray().resize(mesh->getTexcoordArray().size());
			child->getWeightArray().resize(mesh->getWeightArray().size());
			child->getFaceArray().resize(mesh->getFaceArray().size());
			child->getBindposes().resize(mesh->getBindposes().size());

			this->addChild(child);

			vertices_= &child->_vertices;
			normals_ = &child->_normals;
			tangent_ = &child->_tangent;
			tangentQuat_ = &child->_tangentQuat;
			colors_ = &child->_colors;
			texcoords_ = &child->_texcoords;
			weights_ = &child->_weights;
			bindposes_ = &child->_bindposes;
			faces_ = &child->_faces;
		}
		else
		{
			vertices_= &this->_vertices;
			normals_ = &this->_normals;
			tangent_ = &this->_tangent;
			tangentQuat_ = &this->_tangentQuat;
			colors_ = &this->_colors;
			texcoords_ = &this->_texcoords;
			weights_ = &this->_weights;
			bindposes_ = &this->_bindposes;
			faces_ = &this->_faces;
		}

		if (numVertex)
		{
			const auto& vertices = mesh->getVertexArray();
			if (!vertices.empty())
				std::memcpy((*vertices_).data() + offsetVertices, vertices.data(), numVertex * sizeof(Vector3));

			const auto& normals = mesh->getNormalArray();
			if (!normals.empty())
				std::memcpy((*normals_).data() + offsetVertices, normals.data(), numVertex * sizeof(Vector3));

			const auto& tangents = mesh->getTangentArray();
			if (!tangents.empty())
				std::memcpy((*tangent_).data() + offsetVertices, tangents.data(), numVertex * sizeof(Vector4));

			const auto& tangentQuats = mesh->getTangentQuatArray();
			if (!tangentQuats.empty())
				std::memcpy((*tangentQuat_).data() + offsetVertices, tangentQuats.data(), numVertex * sizeof(Vector4));

			const auto& colors = mesh->getColorArray();
			if (!colors.empty())
				std::memcpy((*colors_).data() + offsetVertices, colors.data(), numVertex * sizeof(Vector3));

			const auto& texcoords = mesh->getTexcoordArray();
			if (!texcoords.empty())
				std::memcpy((*texcoords_).data() + offsetVertices, texcoords.data(), numVertex * sizeof(Vector2));

			const auto& weights = mesh->getWeightArray();
			if (!weights.empty())
				std::memcpy((*weights_).data() + offsetVertices, weights.data(), numVertex * sizeof(VertexWeight));

			const auto& bindposes = mesh->getBindposes();
			for (auto& bindpose : bindposes)
				bindposes_->push_back(bindpose);

			const auto& transform = instances[i].getTransform();
			if (!math::isIdentity(transform))
			{
				if (math::isOnlyTranslate(transform))
				{
					auto translate = transform.getTranslate();

					for (std::size_t j = offsetIndices; j < vertices.size(); j++)
						(*vertices_)[j] += translate;
				}
				else
				{
					auto transformInverse = math::transformInverse(transform);
					auto transformInverseTranspose = (float3x3)math::transpose(transformInverse);

					for (std::size_t j = offsetIndices; j < vertices.size(); j++)
						(*vertices_)[j] *= transform;

					for (std::size_t j = offsetIndices; j < vertices.size(); j++)
						(*normals_)[j] *= transformInverseTranspose;
				}
			}
		}

		if (numIndex)
		{
			auto faces = mesh->getFaceArray();
			std::size_t size = faces.size();

			if (offsetIndices == 0)
			{
				(*faces_) = faces;
			}
			else
			{
				for (std::size_t j = 0; j < size; j++)
					(*faces_)[j + offsetIndices] = (faces[j] + offsetIndices);
			}
		}

		if (merge)
		{
			offsetVertices += numVertex;
			offsetIndices += numIndex;
		}
	}

	this->computeBoundingBox();
}

void
MeshProperty::combineMeshes(const CombineMeshes& instances, bool merge) noexcept
{
	this->combineMeshes(instances.data(), instances.size(), merge);
}

void
MeshProperty::mergeVertices() noexcept
{
	if (_vertices.empty())
		return;

	if (_normals.empty())
		this->computeVertexNormals();

	std::map<float, std::uint32_t> vectorMap;

	Float3Array changeVertex;
	Float3Array changeNormal;

	for (auto& it : _faces)
	{
		const Vector3& v = (_vertices)[it];
		const Vector3& n = (_normals)[it];

		float vkey = math::hash_float(v.x, v.y, v.z);
		float nkey = math::hash_float(n.z, n.y, n.x);
		float key = vkey + nkey;

		std::uint32_t value = vectorMap[key];
		if (value == 0)
		{
			changeVertex.push_back(v);
			changeNormal.push_back(n);

			std::size_t size = changeVertex.size();
			it = size - 1;
			vectorMap[key] = size;
		}
		else
		{
			it = value - 1;
		}
	}

	_vertices.swap(changeVertex);
	_normals.swap(changeNormal);
}

void
MeshProperty::computeFaceNormals() noexcept
{
	if (_vertices.empty() && _faces.empty())
		return;

	_facesNormal.resize(_vertices.size());

	std::size_t size = _faces.size();
	for (std::size_t i = 0; i < size; i += 3)
	{
		std::size_t f1 = _faces[i];
		std::size_t f2 = _faces[i + 1];
		std::size_t f3 = _faces[i + 2];

		const Vector3& a = _vertices[f1];
		const Vector3& b = _vertices[f2];
		const Vector3& c = _vertices[f3];

		Vector3 edge1 = c - b;
		Vector3 edge2 = a - b;

		Vector3 normal = math::normalize(math::cross(edge1, edge2));

		_facesNormal[f1] = normal;
		_facesNormal[f2] = normal;
		_facesNormal[f3] = normal;
	}
}

void
MeshProperty::computeVertexNormals() noexcept
{
	assert(!_vertices.empty() && !_faces.empty());

	Float3Array normal;
	normal.resize(_vertices.size());
	std::memset(normal.data(), 0, normal.size() * sizeof(float3));

	if (!_facesNormal.empty())
	{
		std::size_t size = _faces.size();
		for (size_t i = 0; i < size; i += 3)
		{
			std::uint32_t a = (_faces)[i];
			std::uint32_t b = (_faces)[i + 1];
			std::uint32_t c = (_faces)[i + 2];

			normal[a] += _facesNormal[a];
			normal[b] += _facesNormal[b];
			normal[c] += _facesNormal[c];
		}
	}
	else
	{
		std::size_t size = _faces.size();
		for (size_t i = 0; i < size; i += 3)
		{
			std::uint32_t f1 = (_faces)[i];
			std::uint32_t f2 = (_faces)[i + 1];
			std::uint32_t f3 = (_faces)[i + 2];

			Vector3& va = _vertices.at(f1);
			Vector3& vb = _vertices.at(f2);
			Vector3& vc = _vertices.at(f3);

			Vector3 edge1 = vb - va;
			Vector3 edge2 = vc - va;

			Vector3 n(math::normalize(math::cross(edge1, edge2)));

			normal[f1] += n;
			normal[f2] += n;
			normal[f3] += n;
		}
	}

	for (auto& it : normal)
		it = math::normalize(it);

	_normals.swap(normal);
}

void
MeshProperty::computeVertexNormals(std::size_t width, std::size_t height) noexcept
{
	Vector3 left;
	Vector3 right;
	Vector3 up;
	Vector3 down;

	auto getVertex = [&](std::size_t x, std::size_t y) -> Vector3
	{
		return (_vertices)[y * width + x];
	};

	for (std::size_t y = 0; y < width; ++y)
	{
		for (std::size_t x = 0; x < height; ++x)
		{
			Vector3 cur = getVertex(x, y);

			if (x > 0)
				left = getVertex(x - 1, y) - cur;

			if (x + 1 < width)
				right = getVertex(x + 1, y) - cur;

			if (y + 1 < height)
				up = getVertex(x, y + 1) - cur;

			if (y > 0)
				down = getVertex(x, y - 1) - cur;

			Vector3 lu = math::cross(left, up);
			Vector3 ru = math::cross(up, right);
			Vector3 rd = math::cross(right, down);
			Vector3 ld = math::cross(down, left);

			int average = 0;

			if (x > 0 && y > 0)
			{
				ld = math::normalize(ld);
				average++;
			}

			if (x > 0 && y + 1 < height)
			{
				lu = math::normalize(lu);
				average++;
			}

			if (y > 0 && x + 1 < width)
			{
				rd = math::normalize(rd);
				average++;
			}

			if (x + 1 < width && y + 1 < height)
			{
				ru = math::normalize(ru);
				average++;
			}

			_normals.push_back(math::normalize((lu + ru + ld + rd) / (float)average));
		}
	}
}

void
MeshProperty::computeMorphNormals() noexcept
{
}

void
MeshProperty::computeTangents() noexcept
{
	if (!_texcoords.empty())
	{
		std::vector<Vector3> tan1;
		std::vector<Vector3> tan2;

		tan1.resize(_vertices.size(), Vector3::Zero);
		tan2.resize(_vertices.size(), Vector3::Zero);

		std::size_t size = _faces.size();
		for (std::size_t i = 0; i < size; i += 3)
		{
			std::uint32_t f1 = (_faces)[i];
			std::uint32_t f2 = (_faces)[i + 1];
			std::uint32_t f3 = (_faces)[i + 2];

			auto& v1 = _vertices[f1];
			auto& v2 = _vertices[f2];
			auto& v3 = _vertices[f3];

			auto& w1 = _texcoords[f1];
			auto& w2 = _texcoords[f2];
			auto& w3 = _texcoords[f3];

			auto x1 = v2.x - v1.x;
			auto x2 = v3.x - v1.x;
			auto y1 = v2.y - v1.y;
			auto y2 = v3.y - v1.y;
			auto z1 = v2.z - v1.z;
			auto z2 = v3.z - v1.z;

			auto s1 = w2.x - w1.x;
			auto s2 = w3.x - w1.x;
			auto t1 = w2.y - w1.y;
			auto t2 = w3.y - w1.y;

			auto r = 1.0f / (s1 * t2 - s2 * t1);
			if (!std::isinf(r))
			{
				Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
				Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

				tan1[f1] += sdir;
				tan1[f2] += sdir;
				tan1[f3] += sdir;

				tan2[f1] += tdir;
				tan2[f2] += tdir;
				tan2[f3] += tdir;
			}
		}

		_tangent.resize(_normals.size());

		for (std::size_t i = 0; i < _normals.size(); i++)
		{
			auto& n = _normals[i];
			auto& t = tan1[i];

			float handedness = math::dot(math::cross(n, t), tan2[i]) < 0.0f ? 1.0f : -1.0f;

			_tangent[i] = float4(math::normalize(t - n * math::dot(n, t)), handedness);
		}
	}
	else
	{
		_tangent.resize(_normals.size());

		for (std::size_t i = 0; i < _normals.size(); i++)
		{
			float3 c1 = math::cross(_normals[i], float3(0.0, 0.0, 1.0));
			float3 c2 = math::cross(_normals[i], float3(0.0, 1.0, 0.0));

			float3 tangent;
			if (math::length(c1) > math::length(c2))
				tangent = c1;
			else
				tangent = c2;

			tangent = math::normalize(tangent);
			tangent = math::normalize(tangent - _normals[i] * math::dot(_normals[i], tangent));

			_tangent[i] = float4(tangent, 1.0);
		}
	}
}

void
MeshProperty::computeTangentQuats() noexcept
{
	assert(_tangent.size() > 1);
	assert(_tangent.size() == _normals.size());

	_tangentQuat.resize(_tangent.size());

	std::size_t numTangent = _tangent.size();
	for (std::size_t i = 0; i < numTangent; i++)
	{
		auto& normal = _normals[i];

		auto tangent = _tangent[i].xyz();
		auto binormal = math::cross(normal, tangent);

		Quaternion quat;
		quat.makeRotate(normal, binormal, tangent);

		if (quat.w < 0.0f)
			quat = -quat;

		if (_tangent[i].w < 0.0f)
			quat = -quat;

		_tangentQuat[i] = float4(quat.x, quat.y, quat.z, quat.w);
	}
}

void
MeshProperty::computeBoundingBox() noexcept
{
	_boundingBox.reset();

	if (!_vertices.empty())
		_boundingBox.encapsulate(_vertices.data(), _vertices.size());

	_boundingBoxChildren = _boundingBox;

	for (auto& it : _children)
	{
		it->computeBoundingBox();

		_boundingBoxChildren.encapsulate(it->getBoundingBox());
	}
}

_NAME_END