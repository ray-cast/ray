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
	: StreamReader(&_file)
{
}

ifstream::ifstream(FILE* file) noexcept
	: StreamReader(&_file)
{
}

ifstream::ifstream(const char* filename) noexcept
	: StreamReader(&_file)
{
	this->open(filename);
}

ifstream::ifstream(const wchar_t* filename) noexcept
	: StreamReader(&_file)
{
	this->open(filename);
}

ifstream::ifstream(const std::string& filename) noexcept
	: StreamReader(&_file)
{
	this->open(filename);
}

ifstream::ifstream(const std::wstring& filename) noexcept
	: StreamReader(&_file)
{
	this->open(filename);
}

bool
ifstream::is_open() const noexcept
{
	return this->rdbuf()->is_open();
}

ifstream&
ifstream::open(const char* filename) noexcept
{
	const isentry ok(this);
	if (ok)
	{
		if (!_file.open(filename, this->getOpenMode()))
			this->setstate(ios_base::failbit);
		else
			this->clear(ios_base::goodbit);
	}

	return (*this);
}

ifstream&
ifstream::open(const wchar_t* filename) noexcept
{
	const isentry ok(this);
	if (ok)
	{
		if (!_file.open(filename, this->getOpenMode()))
			this->setstate(ios_base::failbit);
		else
			this->clear(ios_base::goodbit);
	}

	return (*this);
}

ifstream&
ifstream::open(const std::string& filename) noexcept
{
	return this->open(filename.c_str());
}

ifstream&
ifstream::open(const std::wstring& filename) noexcept
{
	return this->open(filename.c_str());
}

ifstream&
ifstream::close() noexcept
{
	const isentry ok(this);
	if (ok)
	{
		if (!_file.close())
			this->setstate(failbit);
	}

	return (*this);
}

ifstream::~ifstream() noexcept
{
}

ofstream::ofstream() noexcept
	: StreamWrite(&_file)
{
}

ofstream::ofstream(FILE* file) noexcept
	: StreamWrite(&_file)
{
}

ofstream::ofstream(const char* filename) noexcept
	: StreamWrite(&_file)
{
	this->open(filename);
}

ofstream::ofstream(const wchar_t* filename) noexcept
	: StreamWrite(&_file)
{
	this->open(filename);
}

ofstream::ofstream(const std::string& filename) noexcept
	: StreamWrite(&_file)
{
	this->open(filename);
}

ofstream::ofstream(const std::wstring& filename) noexcept
	: StreamWrite(&_file)
{
	this->open(filename);
}

ofstream::~ofstream() noexcept
{
}

bool
ofstream::is_open() const noexcept
{
	return this->rdbuf()->is_open();
}

ofstream&
ofstream::open(const char* filename) noexcept
{
	const osentry ok(this);
	if (ok)
	{
		if (!_file.open(filename, this->getOpenMode()))
			this->setstate(ios_base::failbit);
		else
			this->clear(ios_base::goodbit);
	}

	return (*this);
}

ofstream&
ofstream::open(const wchar_t* filename) noexcept
{
	const osentry ok(this);
	if (ok)
	{
		if (!_file.open(filename, this->getOpenMode()))
			this->setstate(ios_base::failbit);
		else
			this->clear(ios_base::goodbit);
	}

	return (*this);
}

ofstream&
ofstream::open(const std::string& filename) noexcept
{
	return this->open(filename.c_str());
}

ofstream&
ofstream::open(const std::wstring& filename) noexcept
{
	return this->open(filename.c_str());
}

ofstream&
ofstream::close() noexcept
{
	const osentry ok(this);
	if (ok)
	{
		if (!_file.close())
			this->setstate(failbit);
	}

	return (*this);
}

fstream::fstream() noexcept
	: Stream(&_file)
{
}

fstream::fstream(FILE* file) noexcept
	: Stream(&_file)
{
}

fstream::fstream(const char* filename) noexcept
	: Stream(&_file)
{
	this->open(filename);
}

fstream::fstream(const wchar_t* filename) noexcept
	: Stream(&_file)
{
	this->open(filename);
}

fstream::fstream(const std::string& filename) noexcept
	: Stream(&_file)
{
	this->open(filename);
}

fstream::fstream(const std::wstring& filename) noexcept
	: Stream(&_file)
{
	this->open(filename);
}

fstream::~fstream() noexcept
{
}

fstream&
fstream::open(const char* filename) noexcept
{
	const osentry ok(this);
	if (ok)
	{
		if (!_file.open(filename, this->getOpenMode()))
			this->setstate(ios_base::failbit);
		else
			this->clear(ios_base::goodbit);
	}

	return (*this);
}

fstream&
fstream::open(const wchar_t* filename) noexcept
{
	const osentry ok(this);
	if (ok)
	{
		if (!_file.open(filename, this->getOpenMode()))
			this->setstate(ios_base::failbit);
		else
			this->clear(ios_base::goodbit);
	}

	return (*this);
}

fstream&
fstream::open(const std::string& filename) noexcept
{
	return this->open(filename.c_str());
}

fstream&
fstream::open(const std::wstring& filename) noexcept
{
	return this->open(filename.c_str());
}

bool
fstream::is_open() const noexcept
{
	return this->rdbuf()->is_open();
}

fstream&
fstream::close() noexcept
{
	const isentry ok(this);
	if (ok)
	{
		if (!_file.close())
			this->setstate(failbit);
	}

	return (*this);
}

_NAME_END