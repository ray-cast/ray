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
#ifndef _H_CSTREAM_H_
#define _H_CSTREAM_H_

#include <ray/iostream.h>

_NAME_BEGIN

class consolebuf final : public StreamBuf
{
public:
    consolebuf() noexcept;
    virtual ~consolebuf() noexcept;

    bool open(const char* filename, ios_base::openmode mode) noexcept;
    bool open(const wchar_t* filename, ios_base::openmode mode) noexcept;

    streamsize read(char* str, std::streamsize cnt) noexcept;
    streamsize write(const char* str, std::streamsize cnt) noexcept;

    streamoff seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;
    streamoff tellg() noexcept;

    streamsize size() noexcept;

    bool is_open() const noexcept;

    int flush() noexcept;

    bool close() noexcept;

    void lock() noexcept;
    void unlock() noexcept;
};

class EXPORT icstream final : public StreamReader
{
public:
    icstream() noexcept;
    ~icstream() noexcept;

    virtual StreamReaderPtr clone() const noexcept;

private:

    consolebuf _console;
};

class EXPORT ocstream final : public StreamWrite
{
public:
    ocstream() noexcept;
    ~ocstream() noexcept;

    virtual StreamWrite* clone() const;

private:

    consolebuf _console;
};

extern EXPORT icstream cin;
extern EXPORT ocstream cout;

_NAME_END

#endif