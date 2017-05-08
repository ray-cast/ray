// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#ifndef _H_FSTREAM_H_
#define _H_FSTREAM_H_

#include <ray/iostream.h>
#include <ray/filebuf.h>

_NAME_BEGIN

class EXPORT ifstream final : public StreamReader
{
public:
	ifstream() noexcept;
	ifstream(FILE* file) noexcept;
	ifstream(const char* filename) noexcept;
	ifstream(const wchar_t* filename) noexcept;
	ifstream(const std::string& filename) noexcept;
	ifstream(const std::wstring& filename) noexcept;
	~ifstream() noexcept;

	ifstream& open(const char* filename) noexcept;
	ifstream& open(const wchar_t* filename) noexcept;
	ifstream& open(const std::string& filename) noexcept;
	ifstream& open(const std::wstring& filename) noexcept;

	ifstream& close() noexcept;

	bool is_open() const noexcept;

private:

	filebuf _file;
};

class EXPORT ofstream final : public StreamWrite
{
public:
	ofstream() noexcept;
	ofstream(FILE* file) noexcept;
	ofstream(const char* filename) noexcept;
	ofstream(const wchar_t* filename) noexcept;
	ofstream(const std::string& filename) noexcept;
	ofstream(const std::wstring& filename) noexcept;
	~ofstream() noexcept;

	ofstream& open(const char* filename) noexcept;
	ofstream& open(const wchar_t* filename) noexcept;
	ofstream& open(const std::string& filename) noexcept;
	ofstream& open(const std::wstring& filename) noexcept;

	ofstream& close() noexcept;

	bool is_open() const noexcept;

private:

	filebuf _file;
};

class EXPORT fstream final : public Stream
{
public:
	fstream() noexcept;
	fstream(FILE* file) noexcept;
	fstream(const char* filename) noexcept;
	fstream(const wchar_t* filename) noexcept;
	fstream(const std::string& filename) noexcept;
	fstream(const std::wstring& filename) noexcept;
	~fstream() noexcept;

	fstream& open(const char* filename) noexcept;
	fstream& open(const wchar_t* filename) noexcept;
	fstream& open(const std::string& filename) noexcept;
	fstream& open(const std::wstring& filename) noexcept;

	fstream& close() noexcept;

	bool is_open() const noexcept;

private:

	filebuf _file;
};

_NAME_END

#endif