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
#ifndef _H_OARCHIVE_H_
#define _H_OARCHIVE_H_

#include <ray/archive.h>

_NAME_BEGIN

class EXPORT oarchive : virtual public archive
{
protected:
	class osentry final
	{
	public:
		osentry(oarchive* _istr);
		~osentry() noexcept;

		operator bool() const noexcept;

	private:
		osentry(const osentry&) noexcept = delete;
		osentry& operator=(const osentry&) noexcept = delete;

	private:
		bool _ok;
		oarchive* _my_istr;
	};

public:
	oarchive(archivebuf* buf) noexcept;
	virtual ~oarchive() noexcept;

	void emplace(type_t type) noexcept;
	void clear() noexcept;

	archivebuf& at(const string_t& key) except;
	archivebuf& at(const string_t::value_type* key) except;
	archivebuf& at(const std::size_t n) except;

	const archivebuf& at(const string_t& key) const except;
	const archivebuf& at(const string_t::value_type* key) const except;
	const archivebuf& at(const std::size_t n) const except;

	oarchive& operator=(boolean_t value) except;
	oarchive& operator=(number_integer_t value) except;
	oarchive& operator=(number_unsigned_t value) except;
	oarchive& operator=(number_float_t value) except;
	oarchive& operator=(string_t&& value) except;
	oarchive& operator=(const string_t& value) except;
	oarchive& operator=(string_t::const_pointer& value) except;

	oarchive& operator[](std::size_t n) except;
	oarchive& operator[](const string_t& key) except;
	oarchive& operator[](string_t::const_pointer key) except;

	const oarchive& operator[](std::size_t n) const except;
	const oarchive& operator[](const string_t& key) const except;
	const oarchive& operator[](string_t::const_pointer key) const except;

	oarchive& operator << (archivebuf::boolean_t& argv)
	{
		this->operator=(argv);
		return *this;
	}

	template<typename T, std::enable_if_t<std::is_integral<T>::value || std::is_same<T, number_float_t>::value, int> = 0>
	oarchive& operator << (T argv)
	{
		this->operator=(argv);
		return *this;
	}

	template<typename T, std::enable_if_t<std::is_same<T, string_t>::value || std::is_same<T, string_t::const_pointer>::value, int> = 0>
	oarchive& operator << (const T& argv)
	{
		this->operator=(argv);
		return *this;
	}

	oarchive& operator << (const archivebuf::number_float2_t& argv)
	{
		if (!this->is_array())
			this->emplace(archivebuf::array);

		if (this->is_array())
		{
			for (std::uint8_t i = 0; i < 2; ++i)
				this->operator[](i) = argv[i];
		}
		else
		{
			throw failure(std::string("cannot use operator << with ") + this->type_name());
		}

		return *this;
	}

	oarchive& operator << (const archivebuf::number_float3_t& argv)
	{
		if (!this->is_array())
			this->emplace(archivebuf::array);

		if (this->is_array())
		{
			for (std::uint8_t i = 0; i < 3; ++i)
				this->operator[](i) = argv[i];
		}
		else
		{
			throw failure(std::string("cannot use operator << with ") + this->type_name());
		}

		return *this;
	}

	oarchive& operator << (const archivebuf::number_float4_t& argv)
	{
		if (!this->is_array())
			this->emplace(archivebuf::array);

		if (this->is_array())
		{
			for (std::uint8_t i = 0; i < 4; ++i)
				this->operator[](i) = argv[i];
		}
		else
		{
			throw failure(std::string("cannot use operator << with ") + this->type_name());
		}

		return *this;
	}

	oarchive& operator << (const archivebuf::number_quaternion_t& argv)
	{
		if (!this->is_array())
			this->emplace(archivebuf::array);

		if (this->is_array())
		{
			for (std::uint8_t i = 0; i < 4; ++i)
				this->operator[](i) = argv[i];
		}
		else
		{
			throw failure(std::string("cannot use operator << with ") + this->type_name());
		}

		return *this;
	}

private:

	oarchive(const oarchive&) noexcept = delete;
	oarchive& operator=(const oarchive&) noexcept = delete;
};

_NAME_END

#endif