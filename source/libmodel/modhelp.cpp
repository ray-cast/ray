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
#include <ray/memory.h>

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
	prop->dataType = PTI_INTEGER;
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
	prop->dataType = PTI_FLOAT;
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
	prop->dataType = PTI_FLOAT | PTI_BUFFER;
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
	prop->dataType = PTI_FLOAT | PTI_BUFFER;
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
	prop->dataType = PTI_STRING;
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
		if (prop->dataType == PTI_INTEGER)
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
		if (prop->dataType == PTI_FLOAT)
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
		if (prop->dataType & PTI_FLOAT &&
			prop->dataType & PTI_BUFFER)
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
		if (prop->dataType & PTI_FLOAT &&
			prop->dataType & PTI_BUFFER)
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
		if (prop->dataType == PTI_STRING)
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
	: _materialID(0)
	, _parent(nullptr)
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
MeshProperty::addChild(MeshPropertyPtr entity) noexcept
{
	assert(entity);
	entity->setParent(std::dynamic_pointer_cast<MeshProperty>(this->shared_from_this()));
}

void
MeshProperty::removeChild(MeshPropertyPtr entity) noexcept
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

void
MeshProperty::setMaterialID(std::size_t index) noexcept
{
	_materialID = index;
}

std::size_t
MeshProperty::getMaterialID() const noexcept
{
	return _materialID;
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
MeshProperty::setVertexArray(const Vector3Array& array) noexcept
{
	_vertices = array;
}

void
MeshProperty::setNormalArray(const Vector3Array& array) noexcept
{
	_normals = array;
}

void
MeshProperty::setColorArray(const Vector4Array& array) noexcept
{
	_colors = array;
}

void
MeshProperty::setTangentArray(const Vector3Array& array) noexcept
{
	_tangent = array;
}

void
MeshProperty::setTexcoordArray(const std::vector<Vector2>& array) noexcept
{
	_texcoords = array;
}

void
MeshProperty::setBoneArray(const Bones& array) noexcept
{
	_bones = array;
}

void
MeshProperty::setInverseKinematics(const InverseKinematics& iks) noexcept
{
	_iks = iks;
}

void
MeshProperty::setFaceArray(const UintArray& array) noexcept
{
	_faces = array;
}

void
MeshProperty::setWeightArray(const VertexWeights& array) noexcept
{
	_weights = array;
}

Vector3Array&
MeshProperty::getVertexArray() noexcept
{
	return _vertices;
}

Vector3Array&
MeshProperty::getNormalArray() noexcept
{
	return _normals;
}

Vector4Array&
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

Bones&
MeshProperty::getBoneArray() noexcept
{
	return _bones;
}

InverseKinematics&
MeshProperty::getInverseKinematics() noexcept
{
	return _iks;
}

UintArray&
MeshProperty::getFaceArray() noexcept
{
	return _faces;
}

const Vector3Array&
MeshProperty::getVertexArray() const noexcept
{
	return _vertices;
}

const Vector3Array&
MeshProperty::getNormalArray() const noexcept
{
	return _normals;
}

const Vector3Array&
MeshProperty::getTangentArray() const noexcept
{
	return _tangent;
}

const Vector4Array&
MeshProperty::getColorArray() const noexcept
{
	return _colors;
}

const Vector2Array&
MeshProperty::getTexcoordArray() const noexcept
{
	return _texcoords;
}

const VertexWeights&
MeshProperty::getWeightArray() const noexcept
{
	return _weights;
}

const Bones&
MeshProperty::getBoneArray(const Bones& array) const noexcept
{
	return _bones;
}

const InverseKinematics&
MeshProperty::getInverseKinematics() const noexcept
{
	return _iks;
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
	_vertices = Vector3Array();
	_normals = Vector3Array();
	_colors = Vector4Array();
	_texcoords = Vector2Array();
	_tangent = Vector3Array();
	_facesNormal = Vector3Array();
	_faces = UintArray();
}

MeshPropertyPtr
MeshProperty::clone() noexcept
{
	auto mesh = std::make_shared<MeshProperty>();
	mesh->setName(this->getName());
	mesh->setMaterialID(this->getMaterialID());
	mesh->setVertexArray(this->getVertexArray());
	mesh->setNormalArray(this->getNormalArray());
	mesh->setColorArray(this->getColorArray());
	mesh->setTexcoordArray(this->getTexcoordArray());
	mesh->setWeightArray(this->getWeightArray());
	mesh->setTangentArray(this->getTangentArray());
	mesh->setBoneArray(this->getBoneArray());
	mesh->setInverseKinematics(this->getInverseKinematics());
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

		_texcoords.push_back(Vector2((v.x / radius + 1), (v.y / radius + 1) / 2));
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

			_vertices.push_back(Vector3(x, z, 0));

			_normals.push_back(normal);
		}
	}

	for (std::uint32_t iy = 0; iy < gridY; iy++)
	{
		for (std::uint32_t ix = 0; ix < gridX; ix++)
		{
			_texcoords.push_back(Vector2((float)ix / gridX, (float)(1 - (iy + 1)) / gridY));
			_texcoords.push_back(Vector2((float)(ix + 1) / gridX, (float)(1 - (iy + 1)) / gridY));
			_texcoords.push_back(Vector2((float)ix / gridX, (float)(1 - iy) / gridY));
			_texcoords.push_back(Vector2((float)(ix + 1) / gridX, (float)(1 - iy) / gridY));

			std::int32_t a = static_cast<std::int32_t>(ix + gridX1 * iy);
			std::int32_t b = static_cast<std::int32_t>(ix + gridX1 * (iy + 1));
			std::int32_t c = static_cast<std::int32_t>(ix + gridX1 * (iy + 1) + 1);
			std::int32_t d = static_cast<std::int32_t>(ix + gridX1 * iy + 1);

			_faces.push_back(c);
			_faces.push_back(b);
			_faces.push_back(a);

			_faces.push_back(a);
			_faces.push_back(d);
			_faces.push_back(c);
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

			_texcoords.push_back(Vector2((float)ix / gridX, (float)(1 - (iy + 1)) / gridY));
			_texcoords.push_back(Vector2((float)(ix + 1) / gridX, (float)(1 - (iy + 1)) / gridY));
			_texcoords.push_back(Vector2((float)ix / gridX, (float)(1 - iy) / gridY));
			_texcoords.push_back(Vector2((float)(ix + 1) / gridX, (float)(1 - iy) / gridY));
		}
	}
}

void
MeshProperty::makeFloor(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
{
	this->clear();
	this->makePlane(width, height, 0, widthSegments, 0, heightSegments, 'x', 'z', 1.0, 1.0);
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
			accum += PL.noise(x * invfre, y * invfre, 3.4f) * 0.12f;
			accum += PL.noise(x * invfre, y * invfre, 3.7f) * 0.1f;
			accum += PL.noise(x * invfre + 0.3f, y * invfre + 0.3f, 1.0f) * 0.9f;

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

	std::vector<std::uint32_t> row;
	std::vector<std::vector<std::uint32_t>> vertices;
	std::vector<std::vector<std::uint32_t>> uvs;

	for (std::uint32_t y = 0; y <= heightSegments; y++)
	{
		for (std::uint32_t x = 0; x <= widthSegments; x++)
		{
			float u = (float)(x) / widthSegments;
			float v = (float)(y) / heightSegments;

			Vector3 vertex;
			vertex.x = -radius * cos(phiStart + u * phiLength) * sin(thetaStart + v * thetaLength);
			vertex.y = radius * cos(thetaStart + v * thetaLength);
			vertex.z = radius * sin(phiStart + u * phiLength) * sin(thetaStart + v * thetaLength);

			_vertices.push_back(vertex);

			row.push_back((std::uint32_t)_vertices.size() - 1);

			_texcoords.push_back(Vector2(u, 1 - v));
		}

		vertices.push_back(row);

		row.clear();
	}

	for (std::uint32_t y = 0; y < heightSegments; y++)
	{
		for (std::uint32_t x = 0; x < widthSegments; x++)
		{
			std::uint32_t v1 = vertices[y][(x + 1)];
			std::uint32_t v2 = vertices[y][x];
			std::uint32_t v3 = vertices[(y + 1)][x];
			std::uint32_t v4 = vertices[(y + 1)][(x + 1)];

			if (abs((_vertices)[v1].y) == radius)
			{
				_faces.push_back(v1);
				_faces.push_back(v3);
				_faces.push_back(v4);
			}
			else if (abs((_vertices)[v3].y) == radius)
			{
				_faces.push_back(v1);
				_faces.push_back(v2);
				_faces.push_back(v3);
			}
			else
			{
				_faces.push_back(v1);
				_faces.push_back(v3);
				_faces.push_back(v4);

				_faces.push_back(v1);
				_faces.push_back(v2);
				_faces.push_back(v3);
			}
		}
	}

	this->computeVertexNormals();
	this->computeBoundingBox();
}

void
MeshProperty::makeVolumes(float fovy, float znear, float zfar) noexcept
{
	this->clear();

	float tanFovy_2 = tan(fovy * M_PI / 360.0f);

	_vertices.push_back(Vector3(tanFovy_2 * znear, tanFovy_2 * znear, -znear));
	_vertices.push_back(Vector3(-tanFovy_2 * znear, tanFovy_2 * znear, -znear));
	_vertices.push_back(Vector3(-tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar));
	_vertices.push_back(Vector3(tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar));

	_vertices.push_back(Vector3(tanFovy_2 * znear, -tanFovy_2 * znear, -znear));
	_vertices.push_back(Vector3(tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar));
	_vertices.push_back(Vector3(-tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar));
	_vertices.push_back(Vector3(-tanFovy_2 * znear, -tanFovy_2 * znear, -znear));

	_vertices.push_back(Vector3(tanFovy_2 * znear, -tanFovy_2 * znear, -znear));
	_vertices.push_back(Vector3(tanFovy_2 * znear, tanFovy_2 * znear, -znear));
	_vertices.push_back(Vector3(tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar));
	_vertices.push_back(Vector3(tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar));

	_vertices.push_back(Vector3(-tanFovy_2 * znear, -tanFovy_2 * znear, -znear));
	_vertices.push_back(Vector3(-tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar));
	_vertices.push_back(Vector3(-tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar));
	_vertices.push_back(Vector3(-tanFovy_2 * znear, tanFovy_2 * znear, -znear));

	_vertices.push_back(Vector3(-tanFovy_2 * znear, tanFovy_2 * znear, -znear));
	_vertices.push_back(Vector3(tanFovy_2 * znear, tanFovy_2 * znear, -znear));
	_vertices.push_back(Vector3(tanFovy_2 * znear, -tanFovy_2 * znear, -znear));
	_vertices.push_back(Vector3(-tanFovy_2 * znear, -tanFovy_2 * znear, -znear));

	_vertices.push_back(Vector3(tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar));
	_vertices.push_back(Vector3(-tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar));
	_vertices.push_back(Vector3(-tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar));
	_vertices.push_back(Vector3(tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar));

	this->computeBoundingBox();
}

void
MeshProperty::makeCone(float radius, float height, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
{
	this->clear();

	_vertices.push_back(Vector3(0, 0, 0));
	_vertices.push_back(Vector3(0, 0, -height));

	_texcoords.push_back(Vector2(0, 0));
	_texcoords.push_back(Vector2(1, 1));

	float segment = thetaLength / segments;

	for (std::uint32_t i = 0; i <= segments; i++)
	{
		float sin;
		float cos;

		sinCos(&sin, &cos, thetaStart + i * segment);

		Vector3 v;
		v.x = radius * cos;
		v.y = -radius * sin;
		v.z = 0;

		_vertices.push_back(v);

		_texcoords.push_back(Vector2((v.x / radius + 1), (v.y / radius + 1) / 2));
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

	this->computeVertexNormals();
	this->computeBoundingBox();
}

void
MeshProperty::mergeMeshes(const CombineInstance& instance) noexcept
{
	this->clear();

	std::size_t offsetVertices = 0;
	std::size_t offsetIndices = 0;

	std::size_t maxIndices = 0;
	std::size_t maxVertices = 0;

	bool hasVertices = false;
	bool hasNormal = false;
	bool hasTexcoord = false;
	bool hasFace = false;

	auto& meshes = instance.getInstances();
	for (auto& it : meshes)
	{
		auto mesh = it.getMesh();
		if (mesh)
		{
			maxVertices += mesh->getNumVertices();
			maxIndices += mesh->getNumIndices();

			hasVertices |= !mesh->getVertexArray().empty();
			hasNormal |= !mesh->getNormalArray().empty();
			hasTexcoord |= !mesh->getTexcoordArray().empty();
			hasFace |= !mesh->getFaceArray().empty();
		}
	}

	if (hasVertices)
		this->_vertices.resize(maxVertices);

	if (hasNormal)
		this->_normals.resize(maxVertices);

	if (hasTexcoord)
		this->_texcoords.resize(maxVertices);

	if (hasFace)
		this->_faces.resize(maxIndices);

	for (auto& it : meshes)
	{
		auto mesh = it.getMesh();
		if (mesh)
		{
			std::size_t numVertex = mesh->getNumVertices();
			std::size_t numIndex = mesh->getNumIndices();

			auto transform = it.getTransform();

			if (numVertex)
			{
				if (transform.isOnlyTranslate())
				{
					auto translate = transform.getTranslate();

					auto& vertices = mesh->getVertexArray();
					for (std::size_t i = 0; i < vertices.size(); i++)
						_vertices[i + offsetIndices] = (vertices[i] + translate);

					auto& normals = mesh->getNormalArray();
					if (!normals.empty())
						std::memcpy(_normals.data() + offsetVertices, normals.data(), numVertex * sizeof(Vector3));
				}
				else
				{
					auto transformInverse = transform;
					transformInverse.inverse();

					auto transformInverseTranspose = Matrix4x4(transformInverse).transpose();

					for (auto& v : mesh->getVertexArray())
						_vertices.push_back(transform * v);

					for (auto& n : mesh->getNormalArray())
						_normals.push_back(transformInverseTranspose * n);
				}

				auto& colors = mesh->getColorArray();
				if (!colors.empty())
					std::memcpy(_colors[0].data() + offsetVertices, colors.data(), numVertex * sizeof(Vector3));

				auto& texcoords = mesh->getTexcoordArray();
				if (!texcoords.empty())
					std::memcpy(_texcoords[0].data() + offsetVertices, texcoords.data(), numVertex * sizeof(Vector2));
			}

			if (numIndex)
			{
				auto faces = mesh->getFaceArray();
				std::size_t size = faces.size();
				for (std::size_t i = 0; i < size; i++)
					_faces[i + offsetIndices] = (faces[i] + offsetIndices);
			}

			offsetVertices += numVertex;
			offsetIndices += numIndex;
		}
	}

	this->computeBoundingBox();
}

void
MeshProperty::mergeVertices() noexcept
{
	if (_vertices.empty())
		return;

	if (_normals.empty())
		this->computeVertexNormals();

	std::map<float, std::uint32_t> vectorMap;

	Vector3Array changeVertex;
	Vector3Array changeNormal;

	for (auto& it : _faces)
	{
		const Vector3& v = (_vertices)[it];
		const Vector3& n = (_normals)[it];

		float vkey = hash_float(v.x, v.y, v.z);
		float nkey = hash_float(n.z, n.y, n.x);
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

		Vector3 normal = edge1.cross(edge2);
		normal.normalize();

		_facesNormal[f1] = normal;
		_facesNormal[f2] = normal;
		_facesNormal[f3] = normal;
	}
}

void
MeshProperty::computeVertexNormals() noexcept
{
	assert(!_vertices.empty() && !_faces.empty());

	Vector3Array normal;
	normal.resize(_vertices.size());

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

			Vector3 edge1 = vc - vb;
			Vector3 edge2 = va - vb;

			Vector3 n = ~edge1.cross(edge2);

			normal[f1] += n;
			normal[f2] += n;
			normal[f3] += n;
		}
	}

	for (auto& it : normal)
		it.normalize();

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

			Vector3 lu = left.cross(up);
			Vector3 ru = up.cross(right);
			Vector3 rd = right.cross(down);
			Vector3 ld = down.cross(left);

			int average = 0;

			if (x > 0 && y > 0)
			{
				ld.normalize();
				average++;
			}

			if (x > 0 && y + 1 < height)
			{
				lu.normalize();
				average++;
			}

			if (y > 0 && x + 1 < width)
			{
				rd.normalize();
				average++;
			}

			if (x + 1 < width && y + 1 < height)
			{
				ru.normalize();
				average++;
			}

			Vector3 cur_normal = (lu + ru + ld + rd) / (float)average;

			cur_normal.normalize();

			_normals.push_back(cur_normal);
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
	std::vector<Vector3> tan1;
	std::vector<Vector3> tan2;

	tan1.resize(_vertices.size(), Vector3::Zero);
	tan2.resize(_vertices.size(), Vector3::Zero);

	std::size_t size = _faces.size();
	for (size_t i = 0; i < size; i += 3)
	{
		std::uint32_t f1 = (_faces)[i];
		std::uint32_t f2 = (_faces)[i + 1];
		std::uint32_t f3 = (_faces)[i + 2];

		auto& vA = _vertices[f1];
		auto& vB = _vertices[f2];
		auto& vC = _vertices[f3];

		auto& uvA = _texcoords[f1];
		auto& uvB = _texcoords[f2];
		auto& uvC = _texcoords[f3];

		auto x1 = vB.x - vA.x;
		auto x2 = vC.x - vA.x;
		auto y1 = vB.y - vA.y;
		auto y2 = vC.y - vA.y;
		auto z1 = vB.z - vA.z;
		auto z2 = vC.z - vA.z;

		auto s1 = uvB.x - uvA.x;
		auto s2 = uvC.x - uvA.x;
		auto t1 = uvB.y - uvA.y;
		auto t2 = uvC.y - uvA.y;

		auto r = 1.0 / (s1 * t2 - s2 * t1);

		Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
		Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

		tan1[f1] += sdir;
		tan1[f2] += sdir;
		tan1[f3] += sdir;

		tan2[f1] += tdir;
		tan2[f2] += tdir;
		tan2[f3] += tdir;
	}

	_tangent.resize(_vertices.size());

	for (std::size_t i = 0; i < _vertices.size(); i++)
	{
		auto n = _normals[i];
		auto t = tan1[i];

		_tangent[i] = (t - n * n.dot(t));
		_tangent[i].normalize();
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