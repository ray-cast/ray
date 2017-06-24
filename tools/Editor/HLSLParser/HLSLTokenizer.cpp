// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2017-2018.
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
#include "HLSLTokenizer.h"

const char* _reservedWords[] =
{
	"float",
	"float2",
	"float3",
	"float4",
	"float3x3",
	"float4x4",
	"half",
	"half2",
	"half3",
	"half4",
	"half3x3",
	"half4x4",
	"bool",
	"int",
	"int2",
	"int3",
	"int4",
	"uint",
	"uint2",
	"uint3",
	"uint4",
	"texture",
	"sampler2D",
	"samplerCUBE",
	"if",
	"else",
	"for",
	"while",
	"break",
	"true",
	"false",
	"void",
	"struct",
	"cbuffer",
	"tbuffer",
	"register",
	"return",
	"continue",
	"discard",
	"const",
	"packoffset",
	"uniform",
	"in",
	"inout",
};

HLSLTokenizer::HLSLTokenizer(const char* fileName, const char* buffer, size_t length) noexcept
{
	_buffer = buffer;
	_bufferEnd = buffer + length;
	_fileName = fileName;
	_lineNumber = 1;
	_tokenLineNumber = 1;
	_error = false;
	this->next();
}

void
HLSLTokenizer::next() noexcept
{
	while (skipWhitespace() || skipComment() || scanLineDirective()) {}

	if (_error || _buffer >= _bufferEnd)
	{
		_token = HLSLToken_EOF;
		return;
	}

	_tokenLineNumber = _lineNumber;

	if (_buffer[0] == '+' && _buffer[1] == '=') { _token = HLSLToken_PlusEqual; _buffer += 2; return; }
	if (_buffer[0] == '-' && _buffer[1] == '=') { _token = HLSLToken_MinusEqual; _buffer += 2; return; }
	if (_buffer[0] == '*' && _buffer[1] == '=') { _token = HLSLToken_TimesEqual; _buffer += 2; return; }
	if (_buffer[0] == '/' && _buffer[1] == '=') { _token = HLSLToken_DivideEqual; _buffer += 2; return; }
	if (_buffer[0] == '=' && _buffer[1] == '=') { _token = HLSLToken_EqualEqual; _buffer += 2; return; }
	if (_buffer[0] == '!' && _buffer[1] == '=') { _token = HLSLToken_NotEqual; _buffer += 2; return; }
	if (_buffer[0] == '<' && _buffer[1] == '=') { _token = HLSLToken_LessEqual; _buffer += 2; return; }
	if (_buffer[0] == '>' && _buffer[1] == '=') { _token = HLSLToken_GreaterEqual; _buffer += 2; return; }
	if (_buffer[0] == '&' && _buffer[1] == '&') { _token = HLSLToken_AndAnd; _buffer += 2; return; }
	if (_buffer[0] == '|' && _buffer[1] == '|') { _token = HLSLToken_BarBar; _buffer += 2; return; }
	if (_buffer[0] == '-' && _buffer[1] == '-') { _token = HLSLToken_MinusMinus; _buffer += 2; return; }
	if (_buffer[0] == '+' && _buffer[1] == '+') { _token = HLSLToken_PlusPlus; _buffer += 2; return; }
	if (_buffer[0] == '"' && _buffer[1] == '"') { _token = HLSLToken_Identifier; _buffer += 2; _tokenIdentifier.clear();  return; }
	if (ray::util::isOperatorSymbol(_buffer[0])) { _token = static_cast<HLSLToken>(_buffer[0]); ++_buffer; return; }

	if (this->scanString())
		return;

	if (this->scanNumber())
		return;

	if (this->scanIdentifier())
		return;

	_token = HLSLToken_Identifier;
}

bool
HLSLTokenizer::skipWhitespace() noexcept
{
	bool result = false;
	while (_buffer < _bufferEnd && ray::util::isSpaceOrNewLine(_buffer[0]))
	{
		result = true;
		if (_buffer[0] == '\n')
			++_lineNumber;
		++_buffer;
	}

	return result;
}

bool
HLSLTokenizer::skipComment() noexcept
{
	if (_buffer[0] != '/')
		return false;

	if (_buffer[1] == '/')
	{
		_buffer += 2;

		while (_buffer < _bufferEnd)
		{
			if (*(_buffer++) == '\n')
			{
				++_lineNumber;
				break;
			}
		}
	}
	else if (_buffer[1] == '*')
	{
		_buffer += 2;

		while (_buffer < _bufferEnd)
		{
			if (_buffer[0] == '\n')
				++_lineNumber;

			if (_buffer[0] == '*' && _buffer[1] == '/')
				break;

			++_buffer;
		}

		if (_buffer < _bufferEnd)
			_buffer += 2;
	}

	return true;
}

bool
HLSLTokenizer::scanNumber() noexcept
{
	if ((_buffer[0] == '+' || _buffer[0] == '-') && (_buffer[1] < '0' || _buffer[1] > '9'))
		return false;
	else if (_buffer[0] < '0' || _buffer[0] > '9')
		return false;

	bool hasSub = false;
	bool hasExporent = false;
	bool isFloatPointer = false;

	const char* start = _buffer;
	while (_buffer < _bufferEnd && (_buffer[0] >= '0' && _buffer[0] <= '9' || _buffer[0] == 'e' || _buffer[0] == '-' || _buffer[0] == 'f' || _buffer[0] == '.'))
	{
		if (_buffer[0] == '.')
			isFloatPointer = true;

		if (_buffer[0] == 'e')
			hasExporent = true;

		if (_buffer[0] == '-')
			hasSub = true;

		_buffer++;
	}

	if (hasExporent && hasSub)
		isFloatPointer = true;

	if (_buffer[0] != '\0' && !ray::util::isSpaceOrNewLine(_buffer[0]) && !ray::util::isOperatorSymbol(_buffer[0]))
		return false;

	if (isFloatPointer)
	{
		const char* end = nullptr;
		double value = ray::util::fast_atod(start, &end);
		if (end == start)
			return false;

		_token = HLSLToken_FloatLiteral;
		_tokenValueFloat = static_cast<float>(value);
		return true;
	}
	else
	{
		const char* end = nullptr;
		std::size_t value = ray::util::strtoul10(start, &end);

		_token = HLSLToken_IntLiteral;
		_tokenValueInteger = value;
		return true;
	}

	return false;
}

bool
HLSLTokenizer::scanString() noexcept
{
	const char* start = _buffer;

	if (_buffer[0] == '"')
	{
		++_buffer;

		while (_buffer < _bufferEnd && _buffer[0] != 0 && _buffer[0] != '"')
			++_buffer;

		++_buffer;

		if (_buffer > _bufferEnd)
		{
			_token = HLSLToken_EOF;
			return true;
		}

		_token = HLSLToken_Identifier;
		_tokenIdentifier.assign(start + 1, _buffer - start - 2);

		return true;
	}

	return false;
}

bool
HLSLTokenizer::scanIdentifier() noexcept
{
	const char* start = _buffer;

	while (_buffer < _bufferEnd && _buffer[0] != 0 && !ray::util::isOperatorSymbol(_buffer[0]) && !ray::util::isSpaceOrNewLine(_buffer[0]))
		++_buffer;

	std::size_t length = _buffer - start;
	_tokenIdentifier.assign(start, length);

	for (std::uint8_t i = 0; i < sizeof(_reservedWords) / sizeof(_reservedWords[0]); ++i)
	{
		if (ray::util::strcmp(_reservedWords[i], _tokenIdentifier.c_str()) == 0)
		{
			_token = static_cast<HLSLToken>(256 + i);
			return true;
		}
	}

	return false;
}

bool
HLSLTokenizer::scanLineDirective() noexcept
{
	if (_bufferEnd - _buffer > 5 && strncmp(_buffer, "#line", 5) == 0 && isspace(_buffer[5]))
	{
		_buffer += 5;

		while (_buffer < _bufferEnd && isspace(_buffer[0]))
		{
			if (_buffer[0] == '\n')
			{
				Error("Syntax error: expected line number after #line");
				return false;
			}
			++_buffer;
		}

		const char* iEnd = NULL;
		int lineNumber = ray::util::strtol10(_buffer, &iEnd);

		if (!isspace(*iEnd))
		{
			Error("Syntax error: expected line number after #line");
			return false;
		}

		_buffer = iEnd;
		while (_buffer < _bufferEnd && isspace(_buffer[0]))
		{
			char c = _buffer[0];
			++_buffer;
			if (c == '\n')
			{
				_lineNumber = lineNumber;
				return true;
			}
		}

		if (_buffer >= _bufferEnd)
		{
			_lineNumber = lineNumber;
			return true;
		}

		if (_buffer[0] != '"')
		{
			Error("Syntax error: expected '\"' after line number near #line");
			return false;
		}

		++_buffer;

		int i = 0;
		while (i + 1 < s_maxIdentifier && _buffer < _bufferEnd && _buffer[0] != '"')
		{
			if (_buffer[0] == '\n')
			{
				Error("Syntax error: expected '\"' before end of line near #line");
				return false;
			}

			_lineDirectiveFileName[i] = *_buffer;
			++_buffer;
			++i;
		}

		_lineDirectiveFileName[i] = 0;

		if (_buffer >= _bufferEnd)
		{
			Error("Syntax error: expected '\"' before end of file near #line");
			return false;
		}

		if (i + 1 >= s_maxIdentifier)
		{
			Error("Syntax error: file name too long near #line");
			return false;
		}

		++_buffer;

		while (_buffer < _bufferEnd && _buffer[0] != '\n')
		{
			if (!isspace(_buffer[0]))
			{
				Error("Syntax error: unexpected input after file name near #line");
				return false;
			}
			++_buffer;
		}

		++_buffer;

		_lineNumber = lineNumber;
		_fileName = _lineDirectiveFileName;

		return true;
	}

	return false;
}

HLSLToken
HLSLTokenizer::GetToken() const noexcept
{
	return _token;
}

float
HLSLTokenizer::GetFloat() const noexcept
{
	return _tokenValueFloat;
}

int
HLSLTokenizer::GetInt() const noexcept
{
	return _tokenValueInteger;
}

const char*
HLSLTokenizer::GetIdentifier() const noexcept
{
	return _tokenIdentifier.c_str();
}

int
HLSLTokenizer::GetLineNumber() const noexcept
{
	return _tokenLineNumber;
}

const char*
HLSLTokenizer::GetFileName() const noexcept
{
	return _fileName;
}

void
HLSLTokenizer::Error(const char* format, ...) noexcept
{
	if (_error)
		return;

	_error = true;

	char buffer[1024];
	va_list args;
	va_start(args, format);
	int result = vsnprintf(buffer, sizeof(buffer) - 1, format, args);
	va_end(args);

	printf("%s(%d) : %s", _fileName, _lineNumber, buffer);
}

void
HLSLTokenizer::GetTokenName(char buffer[s_maxIdentifier]) const noexcept
{
	if (_token == HLSLToken_FloatLiteral)
		std::sprintf(buffer, "%f", _tokenValueFloat);
	else if (_token == HLSLToken_IntLiteral)
		std::sprintf(buffer, "%o", _tokenValueInteger);
	else if (_token == HLSLToken_Identifier)
		std::strcpy(buffer, _tokenIdentifier.c_str());
	else
		GetTokenName(_token, buffer);
}

void
HLSLTokenizer::GetTokenName(HLSLToken token, char buffer[s_maxIdentifier]) noexcept
{
	if (token < HLSLToken_Begin)
	{
		buffer[0] = (char)token;
		buffer[1] = 0;
	}
	else if (token < HLSLToken_LessEqual)
	{
		ray::util::strcpy(buffer, _reservedWords[token - HLSLToken_Begin]);
	}
	else
	{
		switch (token)
		{
		case HLSLToken_PlusPlus:
			strcpy(buffer, "++");
			break;
		case HLSLToken_MinusMinus:
			strcpy(buffer, "--");
			break;
		case HLSLToken_PlusEqual:
			strcpy(buffer, "+=");
			break;
		case HLSLToken_MinusEqual:
			strcpy(buffer, "-=");
			break;
		case HLSLToken_TimesEqual:
			strcpy(buffer, "*=");
			break;
		case HLSLToken_DivideEqual:
			strcpy(buffer, "/=");
			break;
		case HLSLToken_FloatLiteral:
			strcpy(buffer, "float");
			break;
		case HLSLToken_IntLiteral:
			strcpy(buffer, "int");
			break;
		case HLSLToken_Identifier:
			strcpy(buffer, "identifier");
			break;
		case HLSLToken_EOF:
			strcpy(buffer, "<eof>");
		default:
			strcpy(buffer, "unknown");
			break;
		}
	}
}