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
#ifndef _H_XMLREADER_H_
#define _H_XMLREADER_H_

#include <ray/ioarchive.h>

class TiXmlNode;
class TiXmlDocument;
class TiXmlElement;
class TiXmlAttribute;

_NAME_BEGIN

class xmlarchivebuf
{
public:
	xmlarchivebuf() noexcept;
	virtual ~xmlarchivebuf() noexcept;

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
	xmlarchivebuf(const xmlarchivebuf&) noexcept = delete;
	xmlarchivebuf& operator=(const xmlarchivebuf&) noexcept = delete;
};

class EXPORT xmlarchive : public ios_base
{
public:
	xmlarchive() noexcept;
	virtual ~xmlarchive() noexcept;

	void setOpenMode(ios_base::openmode mode) noexcept;
	ios_base::openmode getOpenMode() const noexcept;

	xmlarchivebuf* rdbuf() const noexcept;
	void set_rdbuf(xmlarchivebuf* buf) noexcept;

	std::string getCurrentNodeName() const noexcept;
	std::string getCurrentNodePath() const noexcept;

	bool setToNode(const std::string& path) noexcept;
	bool setToFirstChild() noexcept;
	bool setToFirstChild(const std::string& name) noexcept;
	bool setToNextChild() noexcept;
	bool setToNextChild(const std::string& name) noexcept;
	bool setToParent() noexcept;
	bool setToRoot() noexcept;

	bool hasChild() const noexcept;

protected:
	void _init(xmlarchivebuf* _buf, ios_base::openmode mode) noexcept;

private:
	xmlarchive& operator=(const xmlarchive&) = delete;
	xmlarchive(const xmlarchive&) = delete;

private:
	xmlarchivebuf* _strbuf;
	ios_base::openmode _mode;
};

class EXPORT oxmlarchive : virtual public xmlarchive
{
protected:
	class osentry final
	{
	public:
		osentry(oxmlarchive* _istr);
		~osentry() noexcept;

		operator bool() const noexcept;

	private:
		osentry(const osentry&) noexcept = delete;
		osentry& operator=(const osentry&) noexcept = delete;

	private:
		bool _ok;
		oxmlarchive* _my_istr;
	};

public:
	oxmlarchive(xmlarchivebuf* buf) noexcept;
	virtual ~oxmlarchive() noexcept;

	oxmlarchive& addAttribute(const std::string& key, const std::string& value) noexcept;
	oxmlarchive& setAttribute(const std::string& key, const std::string& value) noexcept;
	oxmlarchive& removeAttribute(const std::string& key) noexcept;

	oxmlarchive& addNode(const std::string& key) noexcept;
	oxmlarchive& addSubNode(const std::string& key) noexcept;

	template<typename T>
	oxmlarchive& operator << (std::pair<const std::string&, T&> value)
	{
		assert(false);
		return *this;
	}

	template<typename T>
	oxmlarchive& operator << (std::pair<const char*, T&> value)
	{
		assert(false);
		return *this;
	}

	template<typename T>
	oxmlarchive& operator << (std::shared_ptr<T>& value)
	{
		assert(false);
		return *this;
	}

private:

	oxmlarchive(const oxmlarchive&) noexcept = delete;
	oxmlarchive& operator=(const oxmlarchive&) noexcept = delete;
};

class EXPORT ixmlarchive : virtual public xmlarchive
{
protected:
	class isentry final
	{
	public:
		isentry(ixmlarchive* _istr);
		~isentry() noexcept;

		operator bool() const noexcept;

	private:
		isentry(const isentry&) noexcept = delete;
		isentry& operator=(const isentry&) noexcept = delete;

	private:
		bool _ok;
		ixmlarchive* _my_istr;
	};

public:
	ixmlarchive(xmlarchivebuf* buf) noexcept;
	virtual ~ixmlarchive() noexcept;

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
	ixmlarchive& operator >> (std::pair<const std::string&, T&> value)
	{
		this->getValue(value);
		return *this;
	}

	template<typename T>
	ixmlarchive& operator >> (std::pair<const char*, T&> value)
	{
		this->getValue(value);
		return *this;
	}

	template<typename T>
	ixmlarchive& operator >> (std::shared_ptr<T>& value)
	{
		if (this->setToFirstChild())
		{
			value->load(*this);
			this->setToParent();
		}
		return *this;
	}

private:
	ixmlarchive(const ixmlarchive&) noexcept = delete;
	ixmlarchive& operator=(const ixmlarchive&) noexcept = delete;
};

class XmlBuf final : public xmlarchivebuf
{
public:
	XmlBuf() noexcept;
	~XmlBuf() noexcept;

	virtual bool open() noexcept;
	virtual void close() noexcept;

	virtual bool is_open() const noexcept;

	virtual bool load(StreamReader& stream) noexcept;
	virtual bool save(StreamWrite& stream) noexcept;

	virtual std::string getCurrentNodeName() const noexcept;
	virtual std::string getCurrentNodePath() const noexcept;

	virtual bool addAttribute(const std::string& key, const std::string& value) noexcept;
	virtual void setAttribute(const std::string& key, const std::string& value) noexcept;
	virtual void removeAttribute(const std::string& key) noexcept;

	virtual bool addDeclaration(const std::string& version, const std::string& encoding, const std::string& standalone) noexcept;

	virtual bool insert(const std::string& key) noexcept;
	virtual bool insertToParent(const std::string& key) noexcept;

	virtual bool setToNode(const std::string& path) noexcept;
	virtual bool setToFirstChild() noexcept;
	virtual bool setToFirstChild(const std::string& name) noexcept;
	virtual bool setToNextChild() noexcept;
	virtual bool setToNextChild(const std::string& name) noexcept;
	virtual bool setToParent() noexcept;
	virtual bool setToRoot() noexcept;

	virtual bool hasChild() const noexcept;

	virtual bool hasAttr(const char* name) const noexcept;
	virtual void clearAttrs() noexcept;
	virtual bool addAttrs() noexcept;
	virtual bool addAttrsInChildren() noexcept;
	virtual bool addAttrsInChildren(const std::string& key) noexcept;
	virtual const std::vector<std::string>& getAttrList() const noexcept;

	virtual bool getValue(const std::string& name, bool& result) const noexcept;
	virtual bool getValue(const std::string& name, int1& result) const noexcept;
	virtual bool getValue(const std::string& name, int2& result) const noexcept;
	virtual bool getValue(const std::string& name, int3& result) const noexcept;
	virtual bool getValue(const std::string& name, int4& result) const noexcept;
	virtual bool getValue(const std::string& name, uint1& result) const noexcept;
	virtual bool getValue(const std::string& name, uint2& result) const noexcept;
	virtual bool getValue(const std::string& name, uint3& result) const noexcept;
	virtual bool getValue(const std::string& name, uint4& result) const noexcept;
	virtual bool getValue(const std::string& name, float1& result) const noexcept;
	virtual bool getValue(const std::string& name, float2& result) const noexcept;
	virtual bool getValue(const std::string& name, float3& result) const noexcept;
	virtual bool getValue(const std::string& name, float4& result) const noexcept;
	virtual bool getValue(const std::string& name, std::string& result) const noexcept;

	virtual const char* errorString() const noexcept;

private:
	XmlBuf(const XmlBuf&) noexcept = delete;
	XmlBuf& operator=(const XmlBuf&) noexcept = delete;

private:

	TiXmlNode* _currentNode;
	TiXmlElement* _currentAttrNode;
	std::vector<std::string> _attrNames;
	std::vector<TiXmlAttribute*> _attrLists;
	std::unique_ptr<TiXmlDocument> _document;
};

class EXPORT XMLReader final : public ixmlarchive
{
public:
	XMLReader() noexcept;
	~XMLReader() noexcept;

	XMLReader& open(StreamReader& stream) noexcept;
	XMLReader& close() noexcept;

	bool is_open() const noexcept;

	XMLReader& load(StreamReader& stream) noexcept;

private:
	XMLReader(const XMLReader&) noexcept = delete;
	XMLReader& operator=(const XMLReader&) noexcept = delete;

private:
	XmlBuf _xml;
};

class EXPORT XMLWrite final : public oxmlarchive
{
public:
	XMLWrite() noexcept;
	~XMLWrite() noexcept;

	XMLWrite& open(const std::string& version, const std::string& encoding, const std::string& standalone) noexcept;
	XMLWrite& close() noexcept;

	XMLWrite& save(StreamWrite& ostream) noexcept;

private:
	XMLWrite(const XMLWrite&) noexcept = delete;
	XMLWrite& operator=(const XMLWrite&) noexcept = delete;

private:
	XmlBuf _xml;
};

#if _HAS_CXX17
namespace xml
{
	EXPORT archivebuf reader(StreamReader& stream);
	EXPORT archivebuf reader(const std::string& path);

	EXPORT bool writer(StreamWrite& stream, archivebuf& root);
	EXPORT bool writer(const std::string& path, archivebuf& root);
}
#endif

_NAME_END

#endif