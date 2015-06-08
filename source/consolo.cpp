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
#include <ray/consolo.h>
#include <iostream>

_NAME_BEGIN

icstream cin;
ocstream cout;

consolebuf::consolebuf() noexcept
{
}

consolebuf::~consolebuf() noexcept
{
}

bool
consolebuf::open(const char* filename, const ios_base::openmode mode) noexcept
{
    return true;
}

bool
consolebuf::open(const wchar_t* filename, const ios_base::openmode mode) noexcept
{
    return true;
}

streamsize
consolebuf::read(char_type* str, std::streamsize cnt) noexcept
{
    return std::cin.read(str, cnt).gcount();
}

streamsize
consolebuf::write(const char_type* str, std::streamsize cnt) noexcept
{
    std::cout.write(str, cnt);
    return cnt;
}

streamoff
consolebuf::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
{
    std::cin.seekg(pos, dir);
    return pos;
}

streamoff
consolebuf::tellg() noexcept
{
    return std::cin.tellg();
}

streamsize
consolebuf::size() noexcept
{
    return 0;
}

bool
consolebuf::is_open() const noexcept
{
    return true;
}

int
consolebuf::flush() noexcept
{
    std::cout.flush();
    return true;
}

void
consolebuf::close() noexcept
{
    std::cin.clear();
    std::cout.clear();
}

void
consolebuf::lock() noexcept
{
}

void
consolebuf::unlock() noexcept
{
}

void
consolebuf::copy(streambuf& other) noexcept
{
    std::vector<char> buffer;
    buffer.resize(other.size());
    other.read(buffer.data(), other.size());
    this->write(buffer.data(), other.size());
}

icstream::icstream() noexcept
    : istream(&_console)
{
}

icstream::icstream(FILE* file) noexcept
    : istream(&_console)
{
}

icstream::icstream(const char* filename, const ios_base::openmode mode) noexcept
    : istream(&_console)
{
    this->open(filename);
}

icstream::icstream(const char* filename, const ios_base::open_mode mode) noexcept
    : istream(&_console)
{
    this->open(filename);
}

icstream::icstream(const wchar_t* filename, const ios_base::openmode mode) noexcept
    : istream(&_console)
{
    this->open(filename);
}

icstream::icstream(const wchar_t* filename, const ios_base::open_mode mode) noexcept
    : istream(&_console)
{
    this->open(filename);
}

icstream::icstream(const std::string& filename, const ios_base::openmode mode) noexcept
    : istream(&_console)
{
    this->open(filename);
}

icstream::icstream(const std::string& filename, const ios_base::open_mode mode) noexcept
    : istream(&_console)
{
    this->open(filename);
}

icstream::icstream(const std::wstring& filename, const ios_base::openmode mode) noexcept
    : istream(&_console)
{
    this->open(filename);
}

icstream::icstream(const std::wstring& filename, const ios_base::open_mode mode) noexcept
    : istream(&_console)
{
    this->open(filename);
}

istream*
icstream::clone() const
{
    return new icstream;
}

icstream::~icstream() noexcept
{
}

ocstream::ocstream() noexcept
    : ostream(&_console)
{
}

ocstream::ocstream(FILE* file) noexcept
    : ostream(&_console)
{
}

ocstream::ocstream(const char* filename, const ios_base::openmode mode) noexcept
    : ostream(&_console)
{
    this->open(filename);
}

ocstream::ocstream(const char* filename, const ios_base::open_mode mode) noexcept
    : ostream(&_console)
{
    this->open(filename);
}

ocstream::ocstream(const wchar_t* filename, const ios_base::openmode mode) noexcept
    : ostream(&_console)
{
    this->open(filename);
}

ocstream::ocstream(const wchar_t* filename, const ios_base::open_mode mode) noexcept
    : ostream(&_console)
{
    this->open(filename);
}

ocstream::ocstream(const std::string& filename, const ios_base::openmode mode) noexcept
    : ostream(&_console)
{
    this->open(filename);
}

ocstream::ocstream(const std::string& filename, const ios_base::open_mode mode) noexcept
    : ostream(&_console)
{
    this->open(filename);
}

ocstream::ocstream(const std::wstring& filename, const ios_base::openmode mode) noexcept
    : ostream(&_console)
{
    this->open(filename);
}

ocstream::ocstream(const std::wstring& filename, const ios_base::open_mode mode) noexcept
    : ostream(&_console)
{
}

ocstream::~ocstream() noexcept
{
}

ostream*
ocstream::clone() const
{
    return new ocstream;
}

_NAME_END