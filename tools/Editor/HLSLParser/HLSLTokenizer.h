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
#ifndef _H_HLSL_TOKENIZER_H_
#define _H_HLSL_TOKENIZER_H_

#include "HLSLTypes.h"

class HLSLTokenizer final
{
public:
	static const int s_maxIdentifier = 256;

public:
	HLSLTokenizer(const char* fileName, const char* buffer, std::size_t length) noexcept;

	void next() noexcept;

	HLSLToken GetToken() const noexcept;

	float GetFloat() const noexcept;
	int   GetInt() const noexcept;
	const char* GetIdentifier() const noexcept;
	const char* GetFileName() const noexcept;

	int GetLineNumber() const noexcept;

	void GetTokenName(char buffer[s_maxIdentifier]) const noexcept;

	void Error(const char* format, ...) noexcept;

	static void GetTokenName(HLSLToken token, char buffer[s_maxIdentifier]) noexcept;

private:
	bool skipWhitespace() noexcept;
	bool skipComment() noexcept;

	bool scanNumber() noexcept;
	bool scanLineDirective() noexcept;
	bool scanString() noexcept;
	bool scanIdentifier() noexcept;

private:
	const char* _fileName;
	const char* _buffer;
	const char* _bufferEnd;
	int         _lineNumber;
	bool        _error;

	HLSLToken   _token;
	std::string _tokenIdentifier;
	float       _tokenValueFloat;
	int         _tokenValueInteger;

	char        _lineDirectiveFileName[s_maxIdentifier];
	int         _tokenLineNumber;
};

#endif