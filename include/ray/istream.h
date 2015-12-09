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
#ifndef _H_ISTREAM_H_
#define _H_ISTREAM_H_

#include <ray/ostream.h>

_NAME_BEGIN

class EXPORT StreamReader : public virtual StreamBase
{
public:
    StreamReader(streambuf* buf) noexcept;
    virtual ~StreamReader() noexcept;

    StreamReader& read(char* str, std::streamsize cnt) noexcept;
    StreamReader& read(char* str, streamsize size, streamsize cnt) noexcept;

    StreamReader& seekg(ios_base::off_type pos) noexcept;
    StreamReader& seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;

    StreamReader& flush() noexcept;

    StreamReader& copy(const StreamReader& other) noexcept;

    streamsize size() noexcept;

    streamoff tellg() noexcept;

    streamsize gcount() const noexcept;

    virtual StreamReader* clone() const noexcept;

protected:
	class isentry final
	{
	public:
		isentry(StreamReader* _istr);
		~isentry() noexcept;

		operator bool() const noexcept;

	private:
		isentry(const isentry&) noexcept = delete;
		const isentry& operator=(const isentry&) noexcept = delete;

	private:
		bool _ok;
		StreamReader* _my_istr;
	};

private:
    StreamReader& operator=(const StreamReader&) = delete;
    StreamReader(const StreamReader&) = delete;

private:

    streamsize _count;
};

_NAME_END

#endif