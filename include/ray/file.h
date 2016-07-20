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
#ifndef _H_FILE_H_
#define _H_FILE_H_

#include <ray/iosbase.h>

_NAME_BEGIN

struct _Iobuf;
typedef _Iobuf FILE;

enum { _INTERNAL_BUFSIZ = 4096 };

extern FILE* fopen(const char* filename, ios_base::openmode mode) noexcept;
extern FILE* fopen(const char* filename, ios_base::open_mode mode) noexcept;
extern FILE* fopen(const wchar_t* filename, ios_base::openmode mode) noexcept;
extern FILE* fopen(const wchar_t* filename, ios_base::open_mode mode) noexcept;
extern FILE* fopen(const std::string& filename, ios_base::openmode mode) noexcept;
extern FILE* fopen(const std::string& filename, ios_base::open_mode mode) noexcept;
extern FILE* fopen(const std::wstring& filename, ios_base::openmode mode) noexcept;
extern FILE* fopen(const std::wstring& filename, ios_base::open_mode mode) noexcept;

extern int fflush(FILE* stream);
extern int fclose(FILE* stream) noexcept;

extern streamsize fread(void* buf, streamsize size, FILE* stream) noexcept;
extern streamsize fwrite(const void* buffer, streamsize size, FILE* stream) noexcept;

extern int fgetc(FILE* stream) noexcept;

extern streamoff ftell(FILE* stream) noexcept;
extern streamoff fseek(FILE* stream, streamoff _off, ios_base::seekdir seek) noexcept;
extern streamoff fgetpos(FILE* stream);
extern streamoff fsetpos(FILE* stream);

extern int faccess(const char* path, int mode) noexcept;
extern int faccess(const wchar_t* path, int mode) noexcept;

//extern int feof(FILE* stream);
//extern int ferror(FILE* stream);

class EXPORT IOFILE
{
public:
	IOFILE() noexcept;
	~IOFILE() noexcept;

	IOFILE* open(const char* filename, ios_base::openmode mode) noexcept;
	IOFILE* open(const wchar_t* filename, ios_base::openmode mode) noexcept;
	IOFILE* open(const char* filename, ios_base::open_mode mode) noexcept;
	IOFILE* open(const wchar_t* filename, ios_base::open_mode mode) noexcept;
	IOFILE* open(const std::string& filename, ios_base::openmode mode) noexcept;
	IOFILE* open(const std::string& filename, ios_base::open_mode mode) noexcept;
	IOFILE* open(const std::wstring& filename, ios_base::openmode mode) noexcept;
	IOFILE* open(const std::wstring& filename, ios_base::open_mode mode) noexcept;

	bool is_open() const noexcept;
	bool close() noexcept;

	int getc() noexcept;

	streamoff seek(streamoff _off, ios_base::seekdir seek) noexcept;
	streamoff tell() const noexcept;

	streamsize size() const noexcept;

	streamsize read(void* buf, streamsize size) noexcept;
	streamsize write(const void* buf, streamsize size) noexcept;

	int    flag() noexcept;
	char*& ptr()  noexcept;
	char*& base() noexcept;
	streamsize&   cnt()  noexcept;

	void swap(IOFILE& other) noexcept;

private:

	FILE* _stream;
};

_NAME_END

#endif