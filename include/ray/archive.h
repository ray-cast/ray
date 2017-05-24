// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#ifndef _H_ARCHIVE_H_
#define _H_ARCHIVE_H_

#include <ray/iosbase.h>
#include <ray/archive_buf.h>

_NAME_BEGIN

class EXPORT archive : public ios_base
{
public:
	using type_t = archivebuf::type_t;
	using boolean_t = archivebuf::boolean_t;
	using number_integer_t = archivebuf::number_integer_t;
	using number_unsigned_t = archivebuf::number_unsigned_t;
	using number_float_t = archivebuf::number_float_t;
	using number_float2_t = archivebuf::number_float2_t;
	using number_float3_t = archivebuf::number_float3_t;
	using number_float4_t = archivebuf::number_float4_t;
	using number_quaternion_t = archivebuf::number_quaternion_t;
	using string_t = archivebuf::string_t;
	using object_t = archivebuf::object_t;
	using array_t = archivebuf::array_t;
	using iterator = archivebuf::iterator;
	using reverse_iterator = archivebuf::reverse_iterator;
	using const_iterator = archivebuf::const_iterator;
	using const_reverse_iterator = archivebuf::const_reverse_iterator;

public:
	archive() noexcept;
	virtual ~archive() noexcept;

	void setOpenMode(ios_base::openmode mode) noexcept;
	ios_base::openmode getOpenMode() const noexcept;

	archivebuf* rdbuf() const noexcept;
	void set_rdbuf(archivebuf* buf) noexcept;

	std::size_t size() const noexcept;

	const char* type_name() const noexcept;
	const char* type_name(type_t type) const noexcept;

	const type_t type() const noexcept;

	bool is_null() const noexcept;
	bool is_boolean() const noexcept;
	bool is_integral() const noexcept;
	bool is_float() const noexcept;
	bool is_string() const noexcept;
	bool is_numeric() const noexcept;
	bool is_array() const noexcept;
	bool is_object() const noexcept;

	template<type_t type, typename = std::enable_if_t<type == type_t::boolean>>
	constexpr boolean_t get() const except { return this->rdbuf()->get<type>(); }

	template<type_t type, typename = std::enable_if_t<type == type_t::number_integer>>
	constexpr number_integer_t get() const except { return this->rdbuf()->get<type>(); }

	template<type_t type, typename = std::enable_if_t<type == type_t::number_unsigned>>
	constexpr number_unsigned_t get() const except { return this->rdbuf()->get<type>(); }

	template<type_t type, typename = std::enable_if_t<type == type_t::number_float>>
	constexpr number_float_t get() const except { return this->rdbuf()->get<number_float_t, type>(); }

	template<type_t type, typename = std::enable_if_t<type == type_t::string>>
	constexpr string_t& get() const except { return *this->rdbuf()->get<type>(); }

	template<type_t type, typename = std::enable_if_t<type == type_t::array>>
	constexpr array_t& get() const except { return *this->rdbuf()->get<type>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, boolean_t>::value>>
	constexpr boolean_t get() const except { return this->rdbuf()->get<type_t::boolean>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, number_integer_t>::value>>
	constexpr number_integer_t get() const except { return this->rdbuf()->get<type_t::number_integer>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, number_unsigned_t>::value>>
	constexpr number_unsigned_t get() const except { return this->rdbuf()->get<type_t::number_unsigned>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, number_float_t>::value>>
	constexpr number_float_t get() const except { return this->rdbuf()->get<type_t::number_float>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, string_t>::value>>
	constexpr string_t& get() const except { return this->rdbuf()->get<type_t::string>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, array_t>::value>>
	constexpr array_t& get() const except { return this->rdbuf()->get<type_t::array>(); }

protected:
	void _init(archivebuf* _buf, ios_base::openmode mode) noexcept;

private:
	archive& operator=(const archive&) = delete;
	archive(const archive&) = delete;

private:
	archivebuf* _strbuf;
	ios_base::openmode _mode;
};

_NAME_END

#endif