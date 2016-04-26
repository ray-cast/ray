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
#include <ray/utf8.h>
#include <ray/string.h>
#include <iconv.h>

_NAME_BEGIN

std::size_t UTF8toGBK(char* dest, std::size_t maxLength, const char* data, std::size_t size)
{
	char inbuf[PATHLIMIT + 1] = { 0 };
	char outbuf[PATHLIMIT + 1] = { 0 };
	char *in = inbuf;
	char *out = outbuf;
	std::size_t in_size = (size_t)size;
	std::size_t out_size = (size_t)PATHLIMIT;

	memcpy(in, data, size);

	iconv_t ic = iconv_open("GBK", "UTF-8");
	iconv(ic, &in, &in_size, &out, &out_size);
	iconv_close(ic);

	out_size = PATHLIMIT - out_size;
	memcpy(dest, outbuf, out_size + 1);

	return out_size;
}

std::size_t UTF8toUNICODE(wchar_t* dest, std::size_t maxLength, const char* data, std::size_t size)
{
	char inbuf[PATHLIMIT + 1] = { 0 };
	char outbuf[PATHLIMIT + 1] = { 0 };
	char *in = inbuf;
	char *out = outbuf;
	std::size_t in_size = (size_t)size;
	std::size_t out_size = (size_t)PATHLIMIT;

	memcpy(in, data, size);

	UTF8toGBK(outbuf, PATHLIMIT, inbuf, size);

	in = outbuf;
	out = inbuf;

	iconv_t ic = iconv_open("WCHAR_T", "GBK");
	iconv(ic, &in, &in_size, &out, &out_size);
	iconv_close(ic);

	out_size = PATHLIMIT - out_size;
	memcpy(dest, inbuf, out_size);

	return out_size;
}

_NAME_END