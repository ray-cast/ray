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
#include <ray/jsonreader.h>
#include <ray/fstream.h>
#include <json/json.hpp>
#include <sstream>
#include <stack>

#if _HAS_CXX17

_NAME_BEGIN

namespace json
{
	void reader(archive_node& node, const nlohmann::json& json);

	void reader(archive_node& node, const std::vector<nlohmann::json>& arrays)
	{
		std::size_t index = 0;

		for (auto& object : arrays)
		{
			switch (object.type())
			{
			case nlohmann::json::value_t::boolean:
				node[index++] = object.get<nlohmann::json::boolean_t>();
				break;
			case nlohmann::json::value_t::number_float:
				node[index++] = object.get<nlohmann::json::number_float_t>();
				break;
			case nlohmann::json::value_t::number_integer:
				node[index++] = object.get<nlohmann::json::number_integer_t>();
				break;
			case nlohmann::json::value_t::number_unsigned:
				node[index++] = object.get<nlohmann::json::number_unsigned_t>();
				break;
			case nlohmann::json::value_t::string:
				node[index++] = object.get<nlohmann::json::string_t>();
				break;
			case nlohmann::json::value_t::object:
				reader(node[index++], object.get<nlohmann::json::object_t>());
				break;
			case nlohmann::json::value_t::array:
				node[index].emplace(archive_node::array);
				reader(node[index++], object.get<nlohmann::json::array_t>());
				break;
			}
		}
	}

	void reader(archive_node& node, const nlohmann::json& json)
	{
		for (auto& it = json.begin(); it != json.end(); ++it)
		{
			auto& value = it.value();

			switch (value.type())
			{
			case nlohmann::json::value_t::boolean:
				node.push_back(it.key(), value.get<nlohmann::json::boolean_t>());
				break;
			case nlohmann::json::value_t::number_float:
				node.push_back(it.key(), value.get<nlohmann::json::number_float_t>());
				break;
			case nlohmann::json::value_t::number_integer:
				node.push_back(it.key(), value.get<nlohmann::json::number_integer_t>());
				break;
			case nlohmann::json::value_t::number_unsigned:
				node.push_back(it.key(), value.get<nlohmann::json::number_unsigned_t>());
				break;
			case nlohmann::json::value_t::string:
				node.push_back(it.key(), value.get<nlohmann::json::string_t>());
				break;
			case nlohmann::json::value_t::object:
				reader(node[it.key()], value);
				break;
			case nlohmann::json::value_t::array:
			{
				node.push_back(it.key(), archive_node(archive_node::array));
				reader(node.back(), value.get<nlohmann::json::array_t>());
			}
			break;
			default:
				break;
			}
		}
	}

	archive_node reader(StreamReader& stream)
	{
		auto length = stream.size();
		if (length == 0 || length > std::numeric_limits<std::string::size_type>::max())
			return false;

		std::string data;
		data.resize((std::string::size_type)length);

		if (!stream.read((char*)data.c_str(), (std::string::size_type)length))
			return false;

		try
		{
			archive_node root;
			auto json = nlohmann::json::parse(data.begin(), data.end());
			reader(root, json);

			return root;
		}
		catch (const std::exception& e)
		{
			throw failure(e.what());
		}
	}

	archive_node reader(const std::string& path)
	{
		ifstream stream;
		if (!stream.open(path))
			return archive_node::null;
		return reader(stream);
	}

	bool writer(StreamWrite& stream, archive_node& root)
	{
		return false;
	}

	bool writer(const std::string& path, archive_node& root)
	{
		ofstream stream;
		if (!stream.open(path))
			return archive_node::null;
		return writer(stream, root);
	}
}

_NAME_END

#endif