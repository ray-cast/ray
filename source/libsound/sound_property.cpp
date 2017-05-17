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
#include <ray/sound_property.h>

_NAME_BEGIN

SoundProperty::SoundProperty() noexcept
{
}

SoundProperty::~SoundProperty() noexcept
{
	for (auto& it : _properties)
	{
		delete it->data;
		delete it;
	}
}

bool
SoundProperty::set(const char* key, std::size_t type, std::size_t index, int value) noexcept
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

	auto prop = std::make_unique<SoundParam>();

	prop->key = key;
	prop->type = type;
	prop->index = index;
	prop->length = sizeof(int);
	prop->dataType = SPTI_INTEGER;
	prop->data = new char[prop->length];

	std::memcpy(prop->data, &value, prop->length);

	_properties.push_back(prop.release());

	return true;
}

bool
SoundProperty::set(const char* key, std::size_t type, std::size_t index, unsigned int value) noexcept
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

	auto prop = std::make_unique<SoundParam>();

	prop->key = key;
	prop->type = type;
	prop->index = index;
	prop->length = sizeof(int);
	prop->dataType = SPTI_UNSIGNED_INTEGER;
	prop->data = new char[prop->length];

	std::memcpy(prop->data, &value, prop->length);

	_properties.push_back(prop.release());

	return true;
}

bool
SoundProperty::set(const char* key, std::size_t type, std::size_t index, float value) noexcept
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

	auto prop = std::make_unique<SoundParam>();

	prop->key = key;
	prop->type = type;
	prop->index = index;
	prop->length = sizeof(float);
	prop->dataType = SPTI_FLOAT;
	prop->data = new char[prop->length];

	std::memcpy(prop->data, &value, prop->length);

	_properties.push_back(prop.release());

	return true;
}

bool
SoundProperty::set(const char* key, std::size_t type, std::size_t index, const float3& value) noexcept
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

	auto prop = std::make_unique<SoundParam>();

	prop->key = key;
	prop->type = type;
	prop->index = index;
	prop->length = sizeof(float3);
	prop->dataType = SPTI_FLOAT | SPTI_BUFFER;
	prop->data = new char[prop->length];

	std::memcpy(prop->data, &value, prop->length);

	_properties.push_back(prop.release());

	return true;
}

bool
SoundProperty::set(const char* key, std::size_t type, std::size_t index, const float4& value) noexcept
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

	auto prop = std::make_unique<SoundParam>();

	prop->key = key;
	prop->type = type;
	prop->index = index;
	prop->length = sizeof(float4);
	prop->dataType = SPTI_FLOAT | SPTI_BUFFER;
	prop->data = new char[prop->length];

	std::memcpy(prop->data, &value, prop->length);

	_properties.push_back(prop.release());

	return true;
}

bool
SoundProperty::set(const char* key, std::size_t type, std::size_t index, const char* value) noexcept
{
	assert(nullptr != key);

	std::string str(value);
	return this->set(key, type, index, str);
}

bool
SoundProperty::set(const char* key, std::size_t type, std::size_t index, const unsigned char* value) noexcept
{
	assert(nullptr != key);

	std::string str((char*)value);
	return this->set(key, type, index, str);
}

bool
SoundProperty::set(const char* key, std::size_t type, std::size_t index, const std::string& value) noexcept
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

	auto prop = std::make_unique<SoundParam>();

	prop->key = key;
	prop->type = type;
	prop->index = index;
	prop->length = value.length();
	prop->dataType = SPTI_STRING;
	prop->data = new char[prop->length];

	std::memcpy(prop->data, value.data(), prop->length);

	_properties.push_back(prop.release());

	return true;
}

bool
SoundProperty::get(const char* key, std::size_t type, std::size_t index, int& value) const noexcept
{
	assert(nullptr != key);

	SoundParam* prop = nullptr;
	if (this->get(key, type, index, &prop))
	{
		if (prop->dataType == SPTI_INTEGER)
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
SoundProperty::get(const char* key, std::size_t type, std::size_t index, unsigned int& value) const noexcept
{
	assert(nullptr != key);

	SoundParam* prop = nullptr;
	if (this->get(key, type, index, &prop))
	{
		if (prop->dataType == SPTI_UNSIGNED_INTEGER)
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
SoundProperty::get(const char* key, std::size_t type, std::size_t index, float& value) const noexcept
{
	assert(nullptr != key);

	SoundParam* prop = nullptr;
	if (this->get(key, type, index, &prop))
	{
		if (prop->dataType == SPTI_FLOAT)
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
SoundProperty::get(const char* key, std::size_t type, std::size_t index, float3& value) const noexcept
{
	assert(nullptr != key);

	SoundParam* prop = nullptr;
	if (this->get(key, type, index, &prop))
	{
		if (prop->dataType & SPTI_FLOAT &&
			prop->dataType & SPTI_BUFFER)
		{
			if (prop->length == sizeof(float3))
			{
				std::memcpy(&value, prop->data, prop->length);
				return true;
			}
		}
	}

	return false;
}

bool
SoundProperty::get(const char* key, std::size_t type, std::size_t index, Vector4& value) const noexcept
{
	assert(nullptr != key);

	SoundParam* prop = nullptr;
	if (this->get(key, type, index, &prop))
	{
		if (prop->dataType & SPTI_FLOAT &&
			prop->dataType & SPTI_BUFFER)
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
SoundProperty::get(const char* key, std::size_t type, std::size_t index, std::string& value) const noexcept
{
	assert(nullptr != key);

	SoundParam* prop = nullptr;
	if (this->get(key, type, index, &prop))
	{
		if (prop->dataType == SPTI_STRING)
		{
			value.assign(prop->data, prop->length);
			return true;
		}
	}

	return false;
}

bool
SoundProperty::get(const char* key, std::size_t type, std::size_t index, SoundParam** out) const noexcept
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

_NAME_END