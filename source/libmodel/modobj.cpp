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
#include "modobj.h"

#include <fstream>
#include <sstream>

_NAME_BEGIN

ObjHandler::ObjHandler() noexcept
{
}

ObjHandler::~ObjHandler() noexcept
{
}

bool
ObjHandler::SearchFileHeaderForToken(StreamReader* stream, const char** tokens, unsigned int numTokens, unsigned int searchBytes, bool tokensSol)
{
	assert(nullptr != stream && nullptr != tokens && 0 != numTokens && 0 != searchBytes);

	if (stream->good())
	{
		// read 200 characters from the file
		std::unique_ptr<char[]> _buffer(new char[searchBytes + 1 /* for the '\0' */]);
		char* buffer = _buffer.get();

		const std::streamsize read = stream->read(buffer, searchBytes).gcount();
		if (!read)
			return false;

		for (std::streamsize i = 0; i < read; ++i)
			buffer[i] = static_cast<char>(::tolower(buffer[i]));

		// It is not a proper handling of unicode files here ...
		// ehm ... but it works in most cases.
		char* cur = buffer, *cur2 = buffer, *end = &buffer[read];
		while (cur != end)
		{
			if (*cur)
				*cur2++ = *cur;
			++cur;
		}
		*cur2 = '\0';

		for (unsigned int i = 0; i < numTokens; ++i)
		{
			assert(nullptr != tokens[i]);

			const char* r = strstr(buffer, tokens[i]);
			if (!r)
				continue;
			// We got a match, either we don't care where it is, or it happens to
			// be in the beginning of the file / line
			if (!tokensSol || r == buffer || r[-1] == '\r' || r[-1] == '\n')
			{
				return true;
			}
		}
	}
	return false;
}

bool
ObjHandler::doCanRead(StreamReader& stream) const noexcept
{
	static const char *pTokens[] = { "mtllib", "usemtl" };
	return SearchFileHeaderForToken(&stream, pTokens, 2);
}

bool 
ObjHandler::doCanSave(ModelType type) const noexcept
{
	return false;
}

bool
ObjHandler::doLoad(StreamReader& stream, Model& model) noexcept
{
	streamsize size = stream.gcount();
	if (size < 16)
		return false;

	std::unique_ptr<char> data(new char[(std::size_t)size]);
	stream.read(data.get(), size);

	_model = &model;

	return this->parser(data.get(), (std::size_t)size);
}

bool
ObjHandler::doSave(StreamWrite& stream, const Model& model) noexcept
{
	return false;
}

bool
ObjHandler::parser(char* data, std::size_t)
{
	std::ifstream infile(data);
	std::string line;

	std::string s1;

	Float3Array _vn;
	Float2Array _vt;
	Float3Array _v;

	std::vector<float> _fv;
	std::vector<float> _ft;
	std::vector<float> _fn;

	while (std::getline(infile, line))
	{
		switch (line[0])
		{
		case 'v':
		{
			if (line[1] == ' ')
			{
				_v.push_back(this->parseVector3(line));
				continue;
			}
			if (line[1] == 't')
			{
				_vt.push_back(this->parseVector2(line));
				continue;
			}
			if (line[1] == 'n')
			{
				_vn.push_back(this->parseVector3(line));
				continue;
			}
		}
		case 'p':
		case 'l':
		case 'f':
		{
			std::istringstream in(line);
			float a;

			for (std::size_t i = 0; i < 3; i++)
			{
				in >> s1;
				a = 0;
				std::size_t k;

				for (k = 0; s1[k] != '/'; k++)
					a = a * 10 + (s1[k] - 48);

				_fv.push_back(a);

				for (k = k + 1; s1[k] != '/'; k++)
					a = a * 10 + (s1[k] - 48);

				_ft.push_back(a);

				a = 0;
				for (k = k + 2; s1[k]; k++)
					a = a * 10 + (s1[k] - 48);

				_fn.push_back(a);
			}
		}
		continue;
		case '#':
		{
		}
		break;
		case 'u':
		{
		}
		break;
		case 'g':
		{
		}
		break;
		case 's':
		{
		}
		break;
		case 'o':
		{
		}
		break;
		default:
			break;
		}
	}

	return true;
}

Vector2
ObjHandler::parseVector2(const std::string& line)
{
	std::string s1;
	float f1, f2;

	std::istringstream sin(line);
	sin >> s1 >> f1 >> f2;

	return Vector2(f1, f2);
}

Vector3
ObjHandler::parseVector3(const std::string& line)
{
	std::string s1;
	float f1, f2, f3;

	std::istringstream sin(line);
	sin >> s1 >> f1 >> f2 >> f3;

	return Vector3(f1, f2, f3);
}

Vector4
ObjHandler::parseVector4(const std::string& line)
{
	std::string s1;
	float f1, f2, f3, f4;

	std::istringstream sin(line);
	sin >> s1 >> f1 >> f2 >> f3 >> f4;

	return Vector4(f1, f2, f3, f4);
}

_NAME_END