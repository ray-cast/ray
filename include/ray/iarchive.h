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
#ifndef _H_IARCHIVE_H_
#define _H_IARCHIVE_H_

#include <ray/oarchive.h>

_NAME_BEGIN

class EXPORT iarchive : virtual public archive
{
protected:
	class isentry final
	{
	public:
		isentry(iarchive* _istr);
		~isentry() noexcept;

		operator bool() const noexcept;

	private:
		isentry(const isentry&) noexcept = delete;
		isentry& operator=(const isentry&) noexcept = delete;

	private:
		bool _ok;
		iarchive* _my_istr;
	};

public:
	iarchive(archivebuf* buf) noexcept;
	virtual ~iarchive() noexcept;

	bool hasAttr(const char* name) const noexcept;
	void clearAttrs() noexcept;
	void addAttrs() noexcept;
	void addAttrsInChildren() noexcept;
	void addAttrsInChildren(const std::string& key) noexcept;
	const std::vector<std::string>& getAttrList() const noexcept;

	bool getValue(const std::string& name, bool& result) noexcept;
	bool getValue(const std::string& name, int1& result) noexcept;
	bool getValue(const std::string& name, int2& result) noexcept;
	bool getValue(const std::string& name, int3& result) noexcept;
	bool getValue(const std::string& name, int4& result) noexcept;
	bool getValue(const std::string& name, uint1& result) noexcept;
	bool getValue(const std::string& name, uint2& result) noexcept;
	bool getValue(const std::string& name, uint3& result) noexcept;
	bool getValue(const std::string& name, uint4& result) noexcept;
	bool getValue(const std::string& name, float1& result) noexcept;
	bool getValue(const std::string& name, float2& result) noexcept;
	bool getValue(const std::string& name, float3& result) noexcept;
	bool getValue(const std::string& name, float4& result) noexcept;
	bool getValue(const std::string& name, std::string& result) noexcept;

	template<typename T>
	T getValue(const std::string& name)
	{
		assert(!name.empty());
		T value;
		this->getValue(name, value);
		return value;
	}

	template<typename T>
	bool getValue(std::pair<const std::string&, T&> value)
	{
		assert(!value.first.empty());

		auto& attributes = this->getAttrList();
		for (auto& it : attributes)
		{
			if (it == value.first)
			{
				this->getValue(it, value.second);
				return true;
			}
		}

		return false;
	}

	template<typename T>
	bool getValue(std::pair<const char*, T&> value)
	{
		assert(value.first);

		auto& attributes = this->getAttrList();
		for (auto& it : attributes)
		{
			if (it.compare(value.first) == 0)
			{
				this->getValue(it, value.second);
				return true;
			}
		}

		return false;
	}

	template<typename T>
	iarchive& operator >> (std::pair<const std::string&, T&> value)
	{
		this->getValue(value);
		return *this;
	}

	template<typename T>
	iarchive& operator >> (std::pair<const char*, T&> value)
	{
		this->getValue(value);
		return *this;
	}

	template<typename T>
	iarchive& operator >> (std::shared_ptr<T>& value)
	{
		if (this->setToFirstChild())
		{
			value->load(*this);
			this->setToParent();
		}
		return *this;
	}

private:
	iarchive(const iarchive&) noexcept = delete;
	iarchive& operator=(const iarchive&) noexcept = delete;
};

_NAME_END

#endif