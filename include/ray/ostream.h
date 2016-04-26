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
#ifndef _H_OSTREAM_H_
#define _H_OSTREAM_H_

#include <ray/stream.h>

_NAME_BEGIN

class EXPORT StreamWrite : public virtual StreamBase
{
public:
	StreamWrite(StreamBuf* buf) noexcept;
	virtual ~StreamWrite() noexcept;

	StreamWrite& write(const char* str, std::streamsize cnt) noexcept;
	StreamWrite& write(const char* str, streamsize size, streamsize cnt) noexcept;

	StreamWrite& seekg(ios_base::off_type pos) noexcept;
	StreamWrite& seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;

	StreamWrite& flush() noexcept;

	streamsize gcount() const noexcept;

	ios_base::pos_type tellg() noexcept;

	StreamWrite& operator << (const char* str) noexcept;
	StreamWrite& operator << (const std::string& str) noexcept;
	StreamWrite& operator << (char value) noexcept;
	StreamWrite& operator << (short value) noexcept;
	StreamWrite& operator << (int value) noexcept;
	StreamWrite& operator << (long long value) noexcept;
	StreamWrite& operator << (unsigned char value) noexcept;
	StreamWrite& operator << (unsigned short value) noexcept;
	StreamWrite& operator << (unsigned int value) noexcept;
	StreamWrite& operator << (unsigned long value) noexcept;
	StreamWrite& operator << (unsigned long long value) noexcept;
	StreamWrite& operator << (float value) noexcept;
	StreamWrite& operator << (double value) noexcept;
	StreamWrite& operator << (ios_base& (*function)(ios_base&)) noexcept;
	StreamWrite& operator << (StreamWrite& (*function)(StreamWrite&)) noexcept;

protected:
	class osentry
	{
	public:
		osentry(StreamWrite* _ostr);
		~osentry() noexcept;
		operator bool() const noexcept;

	private:
		osentry(const osentry&) noexcept = delete;
		const osentry& operator=(const osentry&) noexcept = delete;

	private:
		bool _ok;
		StreamWrite* _my_ostr;
	};

private:
	StreamWrite& operator=(const StreamWrite&) = delete;
	StreamWrite(const StreamWrite&) = delete;

private:

	streamsize _count;
};

_NAME_END

#endif
