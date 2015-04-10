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
#include <ray/fstream.h>

_NAME_BEGIN

ifstream::ifstream() noexcept
    : istream(&_file)
{
}

ifstream::ifstream(FILE* file) noexcept
    : istream(&_file)
{
}

ifstream::ifstream(const char* filename) noexcept
    : istream(&_file)
{
    this->open(filename);
}

ifstream::ifstream(const wchar_t* filename) noexcept
    : istream(&_file)
{
    this->open(filename);
}

ifstream::ifstream(const std::string& filename) noexcept
    : istream(&_file)
{
    this->open(filename);
}

ifstream::ifstream(const std::wstring& filename) noexcept
    : istream(&_file)
{
    this->open(filename);
}

ifstream::~ifstream() noexcept
{
}

ofstream::ofstream() noexcept
    : ostream(&_file)
{
}

ofstream::ofstream(FILE* file) noexcept
    : ostream(&_file)
{
}

ofstream::ofstream(const char* filename) noexcept
    : ostream(&_file)
{
    this->open(filename);
}

ofstream::ofstream(const wchar_t* filename) noexcept
    : ostream(&_file)
{
    this->open(filename);
}

ofstream::ofstream(const std::string& filename) noexcept
    : ostream(&_file)
{
    this->open(filename);
}

ofstream::ofstream(const std::wstring& filename) noexcept
    : ostream(&_file)
{
    this->open(filename);
}

ofstream::~ofstream() noexcept
{
}

fstream::fstream() noexcept
    : iostream(&_file)
{
}

fstream::fstream(FILE* file) noexcept
    : iostream(&_file)
{
}

fstream::fstream(const char* filename) noexcept
    : iostream(&_file)
{
    this->open(filename);
}

fstream::fstream(const wchar_t* filename) noexcept
    : iostream(&_file)
{
    this->open(filename);
}

fstream::fstream(const std::string& filename) noexcept
    : iostream(&_file)
{
    this->open(filename);
}

fstream::fstream(const std::wstring& filename) noexcept
    : iostream(&_file)
{
    this->open(filename);
}

fstream::~fstream() noexcept
{
}

_NAME_END