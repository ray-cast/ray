// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#ifndef _H_PARSE_H_
#define _H_PARSE_H_

#include <ray/math.h>
#include <sstream>

_NAME_BEGIN

template<typename T = std::size_t> inline
T parseInteger(const char* line)
{
	T i1 = 0;

	std::istringstream sin(line);
	if (std::strncmp(line, "0x", 2) == 0)
	{
		sin >> std::hex >> i1;
	}
	else
	{
		sin >> i1;
	}

	return i1;
}

template<typename T = std::size_t> inline
T parseInteger(const wchar_t* line)
{
	T i1 = 0;

	std::wistringstream sin(line);
	if (std::wcsncmp(line, L"0x", 2) == 0)
	{
		sin >> std::hex >> i1;
	}
	else
	{
		sin >> i1;
	}

	return i1;
}

template<> inline
bool parseInteger<bool>(const char* line)
{
	if (std::strcmp(line, "true") == 0)
		return true;

	if (std::strcmp(line, "1") == 0)
		return true;

	return false;
}

template<typename T>
T parseFloat(const std::string& line);

template<> inline
float parseFloat<float>(const std::string& line)
{
	float f1 = 0;

	std::istringstream sin(line);
	sin >> f1;

	return f1;
}

template<> inline
double parseFloat<double>(const std::string& line)
{
	double f1 = 0;

	std::istringstream sin(line);
	sin >> f1;

	return f1;
}

template<typename T = float> inline
Vector2t<T> parseFloat2(const std::string& line)
{
	std::string value = line;

	if (!value.empty())
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
		return float2(f1, f2);
	}

	return float2(0, 0);
}

template<typename T = float> inline
Vector3t<T> parseFloat3(const std::string& line)
{
	std::string value = line;

	if (!value.empty())
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

		std::istringstream sin(value.c_str());

		sin >> f1 >> f2 >> f3;
		return float3(f1, f2, f3);
	}

	return float3(0, 0, 0);
}

template<typename T = float> inline
Vector4t<T> parseFloat4(const std::string& line)
{
	std::string value = line;

	if (!value.empty())
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
		return float4(f1, f2, f3, f4);
	}

	return float4(0, 0, 0, 0);
}

_NAME_END

#endif