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
#ifndef _H_ARCHIVE_H_
#define _H_ARCHIVE_H_

#include <ray/iosbase.h>
#include <ray/math.h>

_NAME_BEGIN

class EXPORT iarchive : virtual public ios_base
{
public:
	iarchive() noexcept;
	virtual ~iarchive() noexcept;

	virtual std::string getCurrentNodeName() const noexcept = 0;
	virtual std::string getCurrentNodePath() const noexcept = 0;

	virtual void setToNode(const std::string& path) noexcept = 0;
	virtual bool setToFirstChild(const std::string& name = "") noexcept = 0;
	virtual bool setToNextChild(const std::string& name = "") noexcept = 0;
	virtual bool setToParent() noexcept = 0;

	virtual bool hasAttr(const char* name) const noexcept = 0;
	virtual std::vector<std::string> getAttrs() const noexcept = 0;

	virtual std::string getString(const char* name) const noexcept = 0;
	virtual std::string getString(const std::string& name) const noexcept = 0;

	virtual std::string getText() const noexcept = 0;

	virtual int getInteger(const char* name) const noexcept = 0;
	virtual int getInteger(const std::string& name) const noexcept = 0;

	virtual bool getBoolean(const char* name) const noexcept = 0;
	virtual bool getBoolean(const std::string& name) const noexcept = 0;

	virtual float getFloat(const char* name) const noexcept = 0;
	virtual float getFloat(const std::string& name) const noexcept = 0;

	virtual float2 getFloat2(const char* name) const noexcept = 0;
	virtual float2 getFloat2(const std::string& name) const noexcept = 0;

	virtual float3 getFloat3(const char* name) const noexcept = 0;
	virtual float3 getFloat3(const std::string& name) const noexcept = 0;

	virtual float4 getFloat4(const char* name) const noexcept = 0;
	virtual float4 getFloat4(const std::string& name) const noexcept = 0;

	void getValue(const std::string& name, bool& result) const noexcept { result = this->getBoolean(name); }
	void getValue(const std::string& name, int& result) const noexcept { result = this->getInteger(name); }
	void getValue(const std::string& name, float& result) const noexcept { result = this->getFloat(name); }
	void getValue(const std::string& name, float2& result) const noexcept { result = this->getFloat2(name); }
	void getValue(const std::string& name, float3& result) const noexcept { result = this->getFloat3(name); }
	void getValue(const std::string& name, float4& result) const noexcept { result = this->getFloat4(name); }
	void getValue(const std::string& name, std::string& result) const noexcept { result = this->getString(name); }

	template<typename T>
	iarchive& operator >> (std::pair<const std::string&, T&> value)
	{
		auto attributes = this->getAttrs();
		for (auto& it : attributes)
		{
			if (it == value.first)
			{
				this->getValue(it, value.second);
				break;
			}
		}

		return *this;
	}

	template<typename T>
	iarchive& operator >> (std::pair<const char*, T&> value)
	{
		do
		{
			auto attributes = this->getAttrs();
			for (auto& it : attributes)
			{
				if (it.compare(value.first) == 0)
				{
					this->getValue(it, value.second);
					this->setToParent();
					this->setToFirstChild();
					return *this;
				}
			}
		} while (this->setToNextChild());

		this->setToFirstChild();
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

class EXPORT oarchive : virtual public ios_base
{
public:

	template<typename T>
	oarchive& operator << (std::pair<const std::string&, T&> value)
	{
		assert(false);
		return *this;
	}

	template<typename T>
	oarchive& operator << (std::pair<const char*, T&> value)
	{
		assert(false);
		return *this;
	}

	template<typename T>
	iarchive& operator << (std::shared_ptr<T>& value)
	{
		if (this->setToFirstChild())
		{
			value->save(*this);
			this->setToParent();
		}
		return *this;
	}

private:

	oarchive(const oarchive&) noexcept = delete;
	oarchive& operator=(const oarchive&) noexcept = delete;
};

class EXPORT ioarchive : public iarchive, public oarchive
{
public:

private:

	ioarchive(const ioarchive&) noexcept = delete;
	ioarchive& operator=(const ioarchive&) noexcept = delete;
};

_NAME_END

#endif