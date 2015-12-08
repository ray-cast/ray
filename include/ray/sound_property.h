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
#ifndef _H_SOUND_PROPERTY_H_
#define _H_SOUND_PROPERTY_H_

#include <ray/sound_types.h>

_NAME_BEGIN

struct SoundParam
{
	std::string key;

	std::size_t type;
	std::size_t index;
	std::size_t length;
	std::size_t dataType;

	char* data;
};

enum SoundPropertyTypeInfo
{
	SPTI_FLOAT = 0x01,
	SPTI_STRING = 0x02,
	SPTI_INTEGER = 0x04,
	SPTI_UNSIGNED_INTEGER = 0x04,
	SPTI_BUFFER = 0x08,
};

class EXPORT SoundProperty
{
public:
	SoundProperty() noexcept;
	~SoundProperty() noexcept;

	bool set(const char* key, std::size_t type, std::size_t index, int value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, unsigned int value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, float value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, const float3& value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, const float4& value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, const char* value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, const unsigned char* value) noexcept;
	bool set(const char* key, std::size_t type, std::size_t index, const std::string& value) noexcept;

	bool get(const char* key, std::size_t type, std::size_t index, int& value) const noexcept;
	bool get(const char* key, std::size_t type, std::size_t index, unsigned int& value) const noexcept;
	bool get(const char* key, std::size_t type, std::size_t index, float& value) const noexcept;
	bool get(const char* key, std::size_t type, std::size_t index, float3& value) const noexcept;
	bool get(const char* key, std::size_t type, std::size_t index, float4& value) const noexcept;
	bool get(const char* key, std::size_t type, std::size_t index, std::string& value) const noexcept;

private:
	bool get(const char* key, std::size_t type, std::size_t index, SoundParam** out) const noexcept;

private:

	std::vector<SoundParam*> _properties;
};

_NAME_END

#endif