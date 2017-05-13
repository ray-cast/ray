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
#include <ray/xmlreader.h>
#include <sstream>
#include <tinyxml.h>

_NAME_BEGIN

XmlBuf::XmlBuf() noexcept
	: _currentNode(nullptr)
	, _currentAttrNode(nullptr)
	, _document(nullptr)
{
}

XmlBuf::~XmlBuf() noexcept
{
	this->close();
}

bool
XmlBuf::open() noexcept
{
	assert(0 == _document);
	assert(0 == _currentNode);

	_document = std::make_unique<TiXmlDocument>();
	_document->SetValue("xml");

	_currentNode = _document->ToDocument();
	_currentAttrNode = nullptr;

	return true;
}

void
XmlBuf::close() noexcept
{
	if (_currentNode)
		_currentNode = nullptr;

	if (_currentAttrNode)
		_currentAttrNode = nullptr;

	if (_document)
	{
		_document.reset();
		_document = nullptr;
	}
}

bool
XmlBuf::is_open() const noexcept
{
	return _document.get() ? true : false;
}

bool
XmlBuf::load(StreamReader& stream) noexcept
{
	std::size_t length = stream.size();
	if (length == 0)
		return false;

	std::string data;
	data.resize(length);

	if (!stream.read((char*)data.c_str(), length))
		return false;

	_document->Parse(data.c_str());
	if (!_document->Error())
	{
		_currentNode = _document->ToDocument();
		_currentAttrNode = nullptr;
		return true;
	}

	return false;
}

bool
XmlBuf::save(StreamWrite& stream) noexcept
{
	TiXmlPrinter printer;
	if (_currentNode->Accept(&printer))
	{
		stream.write(printer.CStr(), printer.Size());
		return true;
	}

	return false;
}

std::string
XmlBuf::getCurrentNodeName() const noexcept
{
	if (_currentNode)
		return _currentNode->Value();
	else
		return "";
}

std::string
XmlBuf::getCurrentNodePath() const noexcept
{
	if (!_currentNode)
		return "";

	std::vector<std::string> components;

	TiXmlNode* node = _currentNode;
	while (node != _document.get())
	{
		components.push_back(node->Value());
		node = node->Parent();
	}

	std::string path;
	std::size_t size = components.size();
	if (size > 0)
	{
		for (int i = size - 1; i >= 0; i--)
		{
			path.append(components[i]);
			if (i > 0)
			{
				path.append("/");
			}
		}
	}

	return path;
}

bool
XmlBuf::addAttribute(const std::string& key, const std::string& value) noexcept
{
	auto element = _currentNode->ToElement();
	if (element)
	{
		if (!key.empty())
		{
			if (!element->Attribute(key.c_str()))
			{
				element->SetAttribute(key.c_str(), value.c_str());
				return true;
			}
		}
		else
		{
			if (element->FirstChild())
				return false;

			TiXmlText text(value.c_str());
			text.SetCDATA(true);
			return element->InsertEndChild(text) ? true : false;
		}
	}

	return false;
}

void
XmlBuf::setAttribute(const std::string& key, const std::string& value) noexcept
{
	auto element = _currentNode->ToElement();
	if (element)
		element->SetAttribute(key.c_str(), value.c_str());
}

void
XmlBuf::removeAttribute(const std::string& key) noexcept
{
	auto element = _currentNode->ToElement();
	if (element)
		element->RemoveAttribute(key.c_str());
}

bool
XmlBuf::addDeclaration(const std::string& version, const std::string& encoding, const std::string& standalone) noexcept
{
	TiXmlDeclaration declaration(version.c_str(), encoding.c_str(), standalone.c_str());
	TiXmlNode* node = nullptr;

	if (_currentNode)
	{
		auto parent = _currentNode->Parent();
		if (parent)
			node = parent->InsertEndChild(declaration);
		else
			node = _currentNode->InsertEndChild(declaration);

		_currentNode = node;
		return node ? true : false;
	}

	return false;
}

bool
XmlBuf::insert(const std::string& key) noexcept
{
	TiXmlElement element(key.c_str());
	TiXmlNode* node = nullptr;

	if (!_currentNode)
		node = _document->InsertEndChild(element);
	else
		node = _currentNode->InsertEndChild(element);

	if (node)
		_currentNode = node->ToElement();

	return node ? true : false;
}

bool
XmlBuf::insertToParent(const std::string& key) noexcept
{
	TiXmlElement element(key.c_str());
	TiXmlNode* node = nullptr;

	if (_currentNode)
	{
		auto parent = _currentNode->Parent();
		if (parent)
			node = parent->InsertEndChild(element);
		else
			node = _currentNode->InsertEndChild(element);

		if (node)
			_currentNode = node->ToElement();

		return node ? true : false;
	}

	return false;
}

bool
XmlBuf::setToNode(const std::string& path) noexcept
{
	return false;
}

bool
XmlBuf::setToFirstChild() noexcept
{
	assert(_currentNode);

	this->clearAttrs();

	TiXmlElement* child = 0;
	child = _currentNode->FirstChildElement();

	if (child)
		_currentNode = child;

	return child ? true : false;
}

bool
XmlBuf::setToFirstChild(const std::string& name) noexcept
{
	assert(_currentNode);

	this->clearAttrs();

	TiXmlElement* child = 0;
	if (name.empty())
		child = _currentNode->FirstChildElement();
	else
		child = _currentNode->FirstChildElement(name.c_str());

	if (child)
	{
		_currentNode = child;
		return true;
	}
	else
	{
		return false;
	}
}

bool
XmlBuf::setToNextChild() noexcept
{
	assert(_currentNode);

	this->clearAttrs();

	TiXmlElement* next = 0;
	next = _currentNode->NextSiblingElement();

	if (next)
	{
		_currentNode = next;
		return true;
	}
	else
	{
		this->setToParent();
		return false;
	}
}

bool
XmlBuf::setToNextChild(const std::string& name) noexcept
{
	assert(_currentNode);

	this->clearAttrs();

	TiXmlElement* next = 0;
	if (name.empty())
		next = _currentNode->NextSiblingElement();
	else
		next = _currentNode->NextSiblingElement(name.c_str());

	if (next)
	{
		_currentNode = next;
		return true;
	}
	else
	{
		this->setToParent();
		return false;
	}
}

bool
XmlBuf::setToParent() noexcept
{
	assert(_currentNode);

	this->clearAttrs();

	TiXmlNode* parent = _currentNode->Parent();
	if (parent)
	{
		_currentNode = parent->ToElement();
		return _currentNode ? true : false;
	}

	return false;
}

bool
XmlBuf::setToRoot() noexcept
{
	assert(_currentNode);

	this->clearAttrs();
	this->_currentNode = _document->ToDocument();
	return _currentNode ? true : false;
}

bool
XmlBuf::hasChild() const noexcept
{
	return _currentNode->NoChildren() ? false : true;
}

bool
XmlBuf::hasAttr(const char* name) const noexcept
{
	assert(_currentNode);
	auto element = _currentNode->ToElement();
	if (element)
		return element->Attribute(name) ? true : false;
	return false;
}

void
XmlBuf::clearAttrs() noexcept
{
	_attrNames.clear();
	_attrLists.clear();
}

bool
XmlBuf::addAttrs() noexcept
{
	assert(this->_currentNode);

	auto element = _currentNode->ToElement();
	if (element)
	{
		TiXmlAttribute* attr = element->FirstAttribute();
		while (attr)
		{
			_attrNames.push_back(attr->Name());
			_attrLists.push_back(attr);

			attr = attr->Next();
		}
	}

	return true;
}

bool
XmlBuf::addAttrsInChildren() noexcept
{
	auto child = _currentNode->FirstChildElement();
	if (child)
	{
		do
		{
			TiXmlAttribute* attr = child->FirstAttribute();
			if (attr)
			{
				do
				{
					_attrNames.push_back(attr->Name());
					_attrLists.push_back(attr);

					attr = attr->Next();
				} while (attr);
			}

			child = child->NextSiblingElement();
		} while (child);

		return true;
	}

	return false;
}

bool
XmlBuf::addAttrsInChildren(const std::string& key) noexcept
{
	assert(this->_currentNode);

	auto child = _currentNode->FirstChildElement();
	if (child)
	{
		do
		{
			if (key == child->Value())
			{
				TiXmlAttribute* attr = child->FirstAttribute();
				do
				{
					_attrNames.push_back(attr->Name());
					_attrLists.push_back(attr);

					attr = attr->Next();
				} while (attr);
			}

			child = child->NextSiblingElement();
		} while (child);

		return true;
	}

	return false;
}

const std::vector<std::string>&
XmlBuf::getAttrList() const noexcept
{
	return _attrNames;
}

bool
XmlBuf::getValue(const std::string& name, bool& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		if (value == "true" || value == "1")
		{
			result = true;
			return true;
		}
		else if (value == "false" || value == "0" || value == "nil" || value == "null")
		{
			result = false;
			return true;
		}
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, int1& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		result = atoi(value.c_str());
		return true;
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, int2& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		int1 i1 = 0;
		int1 i2 = 0;

		for (auto& it : value)
		{
			if (it == ',')
			{
				it = ' ';
			}
		}

		std::istringstream sin(value.c_str());

		sin >> i1 >> i2;
		result.set(i1, i2);
		return true;
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, int3& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		int1 i1 = 0;
		int1 i2 = 0;
		int1 i3 = 0;

		for (auto& it : value)
		{
			if (it == ',')
			{
				it = ' ';
			}
		}

		std::istringstream sin(value.c_str());

		sin >> i1 >> i2 >> i3;
		result.set(i1, i2, i3);
		return true;
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, int4& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		int1 i1 = 0;
		int1 i2 = 0;
		int1 i3 = 0;
		int1 i4 = 0;

		for (auto& it : value)
		{
			if (it == ',')
			{
				it = ' ';
			}
		}

		std::istringstream sin(value.c_str());

		sin >> i1 >> i2 >> i3 >> i4;
		result.set(i1, i2, i3, i4);
		return true;
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, uint1& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		result = atoi(value.c_str());
		return true;
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, uint2& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		uint i1 = 0;
		uint i2 = 0;

		for (auto& it : value)
		{
			if (it == ',')
			{
				it = ' ';
			}
		}

		std::istringstream sin(value.c_str());

		sin >> i1 >> i2;
		result.set(i1, i2);
		return true;
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, uint3& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		uint i1 = 0;
		uint i2 = 0;
		uint i3 = 0;

		for (auto& it : value)
		{
			if (it == ',')
			{
				it = ' ';
			}
		}

		std::istringstream sin(value.c_str());

		sin >> i1 >> i2 >> i3;
		result.set(i1, i2, i3);
		return true;
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, uint4& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		uint i1 = 0;
		uint i2 = 0;
		uint i3 = 0;
		uint i4 = 0;

		for (auto& it : value)
		{
			if (it == ',')
			{
				it = ' ';
			}
		}

		std::istringstream sin(value.c_str());

		sin >> i1 >> i2 >> i3 >> i4;
		result.set(i1, i2, i3, i4);
		return true;
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, float1& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		result = util::fast_atof(value.c_str());
		return true;
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, float2& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		float f1 = 0;
		float f2 = 0;

		for (auto& it : value)
		{
			if (it == ',')
			{
				it = ' ';
			}
		}

		std::istringstream sin(value.c_str());

		sin >> f1 >> f2;
		result.set(f1, f2);
		return true;
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, float3& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		float f1 = 0;
		float f2 = 0;
		float f3 = 0;

		for (auto& it : value)
		{
			if (it == ',')
			{
				it = ' ';
			}
		}

		std::istringstream sin(value);

		sin >> f1 >> f2 >> f3;
		result.set(f1, f2, f3);
		return true;
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, float4& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		float f1 = 0;
		float f2 = 0;
		float f3 = 0;
		float f4 = 0;

		for (auto& it : value)
		{
			if (it == ',')
			{
				it = ' ';
			}
		}

		std::istringstream sin(value.c_str());

		sin >> f1 >> f2 >> f3 >> f4;
		result.set(f1, f2, f3, f4);
		return true;
	}

	return false;
}

bool
XmlBuf::getValue(const std::string& name, std::string& result) const noexcept
{
	if (_attrLists.empty() || name.empty())
	{
		auto element = _currentNode->ToElement();
		if (element)
		{
			if (name.empty())
			{
				const TiXmlNode* child = element->FirstChild();
				if (child) 
				{
					const TiXmlText* childText = child->ToText();
					if (childText) {
						result = childText->Value();
						return true;
					}
				}
			}
			else
			{
				auto value = element->Attribute(name.c_str());
				if (value)
				{
					result = value;
					return true;
				}
			}
		}
	}

	for (auto& it : _attrLists)
	{
		if (it->Name() == name)
		{
			result = it->Value();
			return true;
		}
	}

	return false;
}

const char*
XmlBuf::errorString() const noexcept
{
	return _document->ErrorDesc();
}

XMLReader::XMLReader() noexcept
	: iarchive(&_xml)
{
}

XMLReader::~XMLReader() noexcept
{
}

XMLReader&
XMLReader::open(StreamReader& stream) noexcept
{
	const isentry ok(this);
	if (ok)
	{
		if (!_xml.open())
			this->setstate(ios_base::failbit);
		else
			this->clear(ios_base::goodbit);
	}

	return this->load(stream);
}

XMLReader&
XMLReader::close() noexcept
{
	const isentry ok(this);
	if (ok)
	{
		if (!this->fail())
			_xml.close();
	}

	return (*this);
}

bool
XMLReader::is_open() const noexcept
{
	return _xml.is_open();
}

XMLReader&
XMLReader::load(StreamReader& stream) noexcept
{
	const isentry ok(this);
	if (ok)
	{
		if (!_xml.load(stream))
			this->setstate(ios_base::failbit);
		else
			this->clear(ios_base::goodbit);
	}

	return (*this);
}

XMLWrite::XMLWrite() noexcept
	: oarchive(&_xml)
{
}

XMLWrite::~XMLWrite() noexcept
{
	this->close();
}

XMLWrite&
XMLWrite::open(const std::string& version, const std::string& encoding, const std::string& standalone) noexcept
{
	const osentry ok(this);
	if (ok)
	{
		if (_xml.open())
		{
			_xml.addDeclaration(version, encoding, standalone);
			this->clear(ios_base::goodbit);
			return (*this);
		}
	}

	this->setstate(ios_base::failbit);
	return (*this);
}

XMLWrite&
XMLWrite::close() noexcept
{
	const osentry ok(this);
	if (ok)
	{
		if (!this->fail())
			_xml.close();
	}

	return (*this);
}

XMLWrite&
XMLWrite::save(StreamWrite& ostream) noexcept
{
	assert(ostream);
	assert(ostream.good());

	const osentry ok(this);
	if (ok)
	{
		if (!this->fail() && !_xml.save(ostream))
			this->setstate(ios_base::failbit);
	}

	return (*this);
}

_NAME_END