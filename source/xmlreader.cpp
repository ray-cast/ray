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
#include <ray/xmlreader.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <ray/utf8.h>

#include <sstream>
#include <tinyxml.h>

_NAME_BEGIN

XMLReader::XMLReader() noexcept
	: _document(nullptr)
	, _currentNode(nullptr)
{
}

XMLReader::~XMLReader() noexcept
{
}

bool
XMLReader::open(istream& stream) noexcept
{
	assert(0 == _document);
	assert(0 == _currentNode);

	std::size_t length = stream.size();
	if (length == 0)
		return false;

	std::string data;
	data.resize(length);

	if (!stream.read((char*)data.c_str(), length))
		return false;

	_document = std::make_unique<TiXmlDocument>();
	_document->Parse(data.c_str());

	if (!_document->Error())
	{
		_currentNode = _document->RootElement();
		if (_currentNode)
			return true;
	}

	return false;
}

bool
XMLReader::open(const std::string& filename) noexcept
{
	MemoryStream stream;
	IoServer::instance()->openFile(filename, stream);
	if (stream.is_open())
		return this->open(stream);
	return false;
}

void
XMLReader::close() noexcept
{
	if (_currentNode)
	{
		_currentNode = nullptr;
	}

	if (_document)
	{
		_document.reset();
		_document = nullptr;
	}
}

std::string
XMLReader::getCurrentNodeName() const noexcept
{
	if (_currentNode)
		return _currentNode->Value();
	else
		return "";
}

std::string
XMLReader::getCurrentNodePath() const noexcept
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

void
XMLReader::setToNode(const std::string& path) noexcept
{
}

bool
XMLReader::setToFirstChild(const std::string& name) noexcept
{
	assert(_currentNode);

	TiXmlElement* child = 0;
	if (name.empty())
	{
		child = _currentNode->FirstChildElement();
	}
	else
	{
		child = _currentNode->FirstChildElement(name.c_str());
	}

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
XMLReader::setToNextChild(const std::string& name) noexcept
{
	assert(_currentNode);

	TiXmlElement* next = 0;
	if (name.empty())
	{
		next = _currentNode->NextSiblingElement();
	}
	else
	{
		next = _currentNode->NextSiblingElement(name.c_str());
	}

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
XMLReader::setToParent() noexcept
{
	assert(_currentNode);

	TiXmlNode* parent = _currentNode->Parent();
	if (parent)
	{
		this->_currentNode = parent->ToElement();
		return true;
	}
	else
	{
		return false;
	}
}

bool
XMLReader::hasChild() const noexcept
{
	return _currentNode->NoChildren() ? false : true;
}

bool
XMLReader::hasAttr(const char* name) const noexcept
{
	assert(this->_currentNode);

	return _currentNode->Attribute(name) ? true : false;
}

std::vector<std::string>
XMLReader::getAttrs() const noexcept
{
	assert(this->_currentNode);

	std::vector<std::string> attrs;

	TiXmlAttribute* attr = this->_currentNode->FirstAttribute();
	while (0 != attr)
	{
		attrs.push_back(attr->Name());
		attr = attr->Next();
	}

	return attrs;
}

std::string
XMLReader::getText() const noexcept
{
	auto result = _currentNode->GetText();
	if (result)
		return result;
	return "";
}

bool
XMLReader::getValue(const std::string& name, bool& result) const noexcept
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
XMLReader::getValue(const std::string& name, int1& result) const noexcept
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
XMLReader::getValue(const std::string& name, int2& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		int i1 = 0;
		int i2 = 0;

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
XMLReader::getValue(const std::string& name, int3& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		int i1 = 0;
		int i2 = 0;
		int i3 = 0;

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
XMLReader::getValue(const std::string& name, int4& result) const noexcept
{
	std::string value;
	if (this->getValue(name, value))
	{
		int i1 = 0;
		int i2 = 0;
		int i3 = 0;
		int i4 = 0;

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
XMLReader::getValue(const std::string& name, float1& result) const noexcept
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
XMLReader::getValue(const std::string& name, float2& result) const noexcept
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
XMLReader::getValue(const std::string& name, float3& result) const noexcept
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
XMLReader::getValue(const std::string& name, float4& result) const noexcept
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
XMLReader::getValue(const std::string& name, std::string& result) const noexcept
{
	auto value = _currentNode->Attribute(name.c_str());
	if (value)
	{
		result = value;
		return true;
	}

	return false;
}

_NAME_END