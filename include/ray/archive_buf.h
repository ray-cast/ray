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
#ifndef _H_ARCHIVEBUF_H_
#define _H_ARCHIVEBUF_H_

#include <ray/math.h>
#include <ray/iostream.h>
#include <ray/except.h>

#if _HAS_CXX17
#	include <variant>
#endif

_NAME_BEGIN

#if _HAS_CXX17

class EXPORT archive_node final
{
public:
	using boolean_t = bool;
	using number_integer_t = std::int64_t;
	using number_unsigned_t = std::uint64_t;
	using number_float_t = double;
	using string_t = std::string;
	using object_t = std::list<std::pair<std::string, archive_node>>;
	using array_t = std::vector<archive_node>;
	using iterator = object_t::iterator;
	using reverse_iterator = object_t::reverse_iterator;
	using const_iterator = object_t::const_iterator;
	using const_reverse_iterator = object_t::const_reverse_iterator;

	using variant_t = std::variant<
		void*,
		boolean_t,
		number_integer_t,
		number_unsigned_t,
		number_float_t,
		std::unique_ptr<string_t>,
		std::unique_ptr<array_t>,
		std::unique_ptr<object_t>
	>;

	static const archive_node& nil;
	static const archive_node& nilRef;

	enum type_t
	{
		null,
		boolean,
		number_integer,
		number_unsigned,
		number_float,
		string,
		array,
		object
	};
public:
	archive_node();
	archive_node(type_t value);
	archive_node(boolean_t value);
	archive_node(number_integer_t value);
	archive_node(number_unsigned_t value);
	archive_node(number_float_t value);
	archive_node(string_t&& value);
	archive_node(const string_t& value);
	archive_node(const string_t::value_type* value);
	archive_node(archive_node&& value);
	~archive_node();

	archive_node& at(const string_t& key);
	archive_node& at(const string_t::value_type* key);
	archive_node& at(const std::size_t n);

	const archive_node& at(const string_t& key) const;
	const archive_node& at(const string_t::value_type* key) const;
	const archive_node& at(const std::size_t n) const;

	template<type_t type, typename = std::enable_if_t<type == type_t::boolean>>
	constexpr boolean_t get() const { return this->_get<type>(); }

	template<type_t type, typename = std::enable_if_t<type == type_t::number_integer>>
	constexpr number_integer_t get() const { return this->_get<type>(); }

	template<type_t type, typename = std::enable_if_t<type == type_t::number_integer>>
	constexpr number_integer_t get(number_integer_t min, number_integer_t max) const { return std::clamp(this->_get<type>(), min, max); }

	template<type_t type, typename = std::enable_if_t<type == type_t::number_unsigned>>
	constexpr number_unsigned_t get() const { return this->_get<type>(); }

	template<type_t type, typename = std::enable_if_t<type == type_t::number_unsigned>>
	constexpr number_unsigned_t get(number_unsigned_t min, number_unsigned_t max) const { return std::clamp(this->_get<type>(), min, max); }

	template<type_t type, typename = std::enable_if_t<type == type_t::number_float>>
	constexpr number_float_t get() const { return this->_get<type>(); }

	template<type_t type, typename = std::enable_if_t<type == type_t::string>>
	constexpr const string_t& get() const { return *this->_get<type>(); }

	template<type_t type, typename = std::enable_if_t<type == type_t::array>>
	constexpr const array_t& get() const { return *this->_get<type>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, bool>::value>>
	constexpr bool get() const { return this->get<archive_node::type_t::boolean>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, std::int32_t>::value>>
	constexpr std::int32_t get() const { return this->get<archive_node::type_t::number_integer>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, std::uint32_t>::value>>
	constexpr std::uint32_t get() const { return this->get<archive_node::type_t::number_unsigned>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, number_integer_t>::value>>
	constexpr number_integer_t get() const { return this->get<archive_node::type_t::number_integer>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, number_unsigned_t>::value>>
	constexpr number_unsigned_t get() const { return this->get<archive_node::type_t::number_unsigned>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, float>::value>>
	constexpr float get() const { return this->get<float, archive_node::type_t::number_float>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, double>::value>>
	constexpr double get() const { return this->get<double, archive_node::type_t::number_float>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, string_t>::value>>
	constexpr const string_t& get() const { return this->get<archive_node::type_t::string>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, array_t>::value>>
	constexpr const array_t& get() const { return this->get<archive_node::type_t::array>(); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, std::int32_t>::value>>
	constexpr std::int32_t get(std::int32_t min, std::int32_t max) const { return std::clamp(this->get<archive_node::type_t::number_integer>(), min, max); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, std::uint32_t>::value>>
	constexpr std::uint32_t get(std::uint32_t min, std::uint32_t max) const { return std::clamp(this->get<archive_node::type_t::number_unsigned>(), min, max); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, number_integer_t>::value>>
	constexpr number_integer_t get(number_integer_t min, number_integer_t max) const { return std::clamp(this->get<archive_node::type_t::number_integer>(), min, max); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, number_unsigned_t>::value>>
	constexpr number_unsigned_t get(number_unsigned_t min, number_unsigned_t max) const { return std::clamp(this->get<archive_node::type_t::number_unsigned>(), min, max); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, float>::value>>
	constexpr float get(float min, float max) const { return std::clamp(this->get<float, archive_node::type_t::number_float>(), min, max); }

	template<typename T, typename = std::enable_if_t<std::is_same<T, double>::value>>
	constexpr double get(double min, double max) const { return std::clamp(this->get<double, archive_node::type_t::number_float>(), min, max); }

	void push_back(const string_t& key, boolean_t value);
	void push_back(const string_t& key, const number_integer_t& value);
	void push_back(const string_t& key, const number_unsigned_t& value);
	void push_back(const string_t& key, const number_float_t& value);
	void push_back(const string_t& key, const string_t& value);
	void push_back(const string_t& key, const string_t::value_type* value);
	void push_back(const string_t& key, archive_node&& value);

	iterator begin() noexcept;
	iterator end() noexcept;

	const_iterator begin() const noexcept;
	const_iterator end() const noexcept;

	reverse_iterator rbegin() noexcept;
	reverse_iterator rend() noexcept;

	const_reverse_iterator rbegin() const noexcept;
	const_reverse_iterator rend() const noexcept;

	archive_node& front() noexcept;
	const archive_node& front() const noexcept;

	archive_node& back() noexcept;
	const archive_node& back() const noexcept;

	void emplace(type_t type) noexcept;

	type_t type() const noexcept;

	char* type_name() const noexcept;
	char* type_name(type_t type) const noexcept;

	bool is_null() const noexcept;
	bool is_boolean() const noexcept;
	bool is_integral() const noexcept;
	bool is_float() const noexcept;
	bool is_string() const noexcept;
	bool is_numeric() const noexcept;
	bool is_array() const noexcept;
	bool is_object() const noexcept;

	archive_node& operator=(boolean_t value);
	archive_node& operator=(number_integer_t value);
	archive_node& operator=(number_unsigned_t value);
	archive_node& operator=(number_float_t value);
	archive_node& operator=(string_t&& value);
	archive_node& operator=(const string_t& value);
	archive_node& operator=(archive_node&& value);

	archive_node& operator[](const char* key);
	archive_node& operator[](const string_t& key);
	archive_node& operator[](std::size_t n);

	const archive_node& operator[](const char* key) const;
	const archive_node& operator[](const string_t& key) const;
	const archive_node& operator[](std::size_t n) const;

private:
	template<typename T, type_t type>
	constexpr T get() const
	{
		switch (_data.index())
		{
		case type_t::boolean:
			return (T)std::get<type_t::boolean>(_data);
		case type_t::number_integer:
			return (T)std::get<type_t::number_integer>(_data);
		case type_t::number_unsigned:
			return (T)std::get<type_t::number_unsigned>(_data);
		case type_t::number_float:
			return (T)std::get<type_t::number_float>(_data);
		default:
			throw failure(string_t("type must be number, but is ") + this->type_name());
		}
	}

	template<type_t type>
	constexpr decltype(auto) _get() const
	{
		if (this->type() != type)
			throw failure(string_t("type must be ") + type_name(type) + " but is " + this->type_name());

		return std::get<type>(_data);
	}

private:
	archive_node(const archive_node& value);
	archive_node& operator=(const archive_node& value);

private:
	variant_t _data;
};

#endif

class archivebuf
{
public:
	archivebuf() noexcept;
	virtual ~archivebuf() noexcept;

	virtual std::string getCurrentNodeName() const noexcept = 0;
	virtual std::string getCurrentNodePath() const noexcept = 0;

	virtual bool addAttribute(const std::string& key, const std::string& value) noexcept = 0;
	virtual void setAttribute(const std::string& key, const std::string& value) noexcept = 0;
	virtual void removeAttribute(const std::string& key) noexcept = 0;

	virtual bool insert(const std::string& key) noexcept = 0;
	virtual bool insertToParent(const std::string& key) noexcept = 0;

	virtual bool setToNode(const std::string& path) noexcept = 0;
	virtual bool setToFirstChild() noexcept = 0;
	virtual bool setToFirstChild(const std::string& name) noexcept = 0;
	virtual bool setToNextChild() noexcept = 0;
	virtual bool setToNextChild(const std::string& name) noexcept = 0;
	virtual bool setToParent() noexcept = 0;
	virtual bool setToRoot() noexcept = 0;

	virtual bool hasChild() const noexcept = 0;

	virtual bool hasAttr(const char* name) const noexcept = 0;
	virtual void clearAttrs() noexcept = 0;
	virtual bool addAttrs() noexcept = 0;
	virtual bool addAttrsInChildren() noexcept = 0;
	virtual bool addAttrsInChildren(const std::string& key) noexcept = 0;
	virtual const std::vector<std::string>& getAttrList() const noexcept = 0;

	virtual bool getValue(const std::string& name, bool& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, int1& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, int2& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, int3& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, int4& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, uint1& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, uint2& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, uint3& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, uint4& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, float1& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, float2& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, float3& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, float4& result) const noexcept = 0;
	virtual bool getValue(const std::string& name, std::string& result) const noexcept = 0;

	virtual void lock() noexcept;
	virtual void unlock() noexcept;

private:
	archivebuf(const archivebuf&) noexcept = delete;
	archivebuf& operator=(const archivebuf&) noexcept = delete;
};

_NAME_END

#endif