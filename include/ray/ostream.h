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

class EXPORT ostream : public virtual stream
{
public:
    explicit ostream(streambuf* buf, bool _isstd = false) noexcept;
    virtual ~ostream() noexcept;

    bool is_open() const noexcept;

    ostream& open(const char* filename) noexcept;
    ostream& open(const wchar_t* filename) noexcept;
    ostream& open(const std::string& filename) noexcept;
    ostream& open(const std::wstring& filename) noexcept;

    ostream& write(const char* str, std::streamsize cnt) noexcept;
    ostream& write(const char* str, streamsize size, streamsize cnt) noexcept;

    ostream& seekg(ios_base::off_type pos) noexcept;
    ostream& seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;

    ostream& flush() noexcept;
    ostream& close() noexcept;

    streamsize gcount() const noexcept;

    ios_base::pos_type tellg() noexcept;

    ostream& copy(ostream& other) noexcept;

    ostream& operator << (const char* str) noexcept;
    ostream& operator << (const std::string& str) noexcept;
    ostream& operator << (char value) noexcept;
    ostream& operator << (short value) noexcept;
    ostream& operator << (int value) noexcept;
    ostream& operator << (long long value) noexcept;
    ostream& operator << (unsigned char value) noexcept;
    ostream& operator << (unsigned short value) noexcept;
    ostream& operator << (unsigned int value) noexcept;
    ostream& operator << (unsigned long value) noexcept;
    ostream& operator << (unsigned long long value) noexcept;
    ostream& operator << (float value) noexcept;
    ostream& operator << (double value) noexcept;
    ostream& operator << (ios_base& (*function)(ios_base&)) noexcept;
    ostream& operator << (ostream& (*function)(ostream&)) noexcept;

private:
    ostream& operator=(const ostream&) = delete;
    ostream(const ostream&) = delete;

private:

    streamsize _count;
};

_NAME_END

#endif
