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
#include <ray/file.h>
#include <limits>

_NAME_BEGIN

struct _Iobuf
{
    int _file;
    int _flag;
    char *_ptr;
    char *_base;
    int   _charbuf;
    int   _bufsiz;
    streamsize   _cnt;
};

FILE* fmalloc(int file) noexcept
{
    if (file != EOF)
    {
        FILE* stream = (FILE*)::malloc(sizeof(FILE));
        if (stream != nullptr)
        {
            stream->_file = file;
            stream->_base = nullptr;
            stream->_ptr = nullptr;
            stream->_bufsiz = 0;
            stream->_flag = 0;

            return stream;
        }
    }

    return nullptr;
}

FILE* fopen(const char* filename, ios_base::openmode mode) noexcept
{
#if defined(__WINDOWS__)
    int flags = O_BINARY;
#else
    int flags = 0;
#endif

    if (mode & ios_base::in && mode & ios_base::out)
        flags |= O_RDWR;
    else if (mode & ios_base::in)
        flags |= O_RDONLY;
    else if (mode & ios_base::out)
        flags |= O_WRONLY | O_CREAT | O_TRUNC;

    if (mode & ios_base::app)     flags |= O_APPEND;
    if (mode & ios_base::trunc)   flags |= O_TRUNC;

    int access = fcntl::PP_DEFAULT;

#if defined(__WINDOWS__)
    access |= fcntl::PP_IRUSR | fcntl::PP_IWUSR;
#endif

    return fmalloc(fcntl::open(filename, flags, access));
}

FILE* fopen(const char* filename, ios_base::open_mode mode) noexcept
{
    return fopen(filename, (ios_base::openmode)mode);
}

FILE* fopen(const wchar_t* filename, ios_base::openmode mode) noexcept
{
#if defined(__WINDOWS__)
    int flags = O_BINARY;
#else
    int flags = 0;
#endif

    if (mode & ios_base::in && mode & ios_base::out)
        flags |= O_RDWR;
    else if (mode & ios_base::in)
        flags |= O_RDONLY;
    else if (mode & ios_base::out)
        flags |= O_WRONLY | O_CREAT;

    if (mode & ios_base::app)     flags |= O_APPEND;
    if (mode & ios_base::trunc)   flags |= O_TRUNC;

    int access = fcntl::PP_DEFAULT;

#if defined(__WINDOWS__)
    access |= fcntl::PP_IRUSR | fcntl::PP_IWUSR;
#endif

    return fmalloc(fcntl::open(filename, flags, access));
}

FILE* fopen(const wchar_t* filename, ios_base::open_mode mode) noexcept
{
    return fopen(filename, (ios_base::openmode)mode);
}

FILE* fopen(const std::string& filename, ios_base::openmode mode) noexcept
{
    return fopen(filename.c_str(), mode);
}

FILE* fopen(const std::string& filename, ios_base::open_mode mode) noexcept
{
    return fopen(filename.c_str(), (ios_base::openmode)mode);
}

FILE* fopen(const std::wstring& filename, ios_base::openmode mode) noexcept
{
    return fopen(filename.c_str(), mode);
}

FILE* fopen(const std::wstring& filename, ios_base::open_mode mode) noexcept
{
    return fopen(filename.c_str(), (ios_base::openmode)mode);
}

int fclose(FILE* stream) noexcept
{
    if (stream)
    {
        if (stream->_base)
        {
            free(stream->_base);
        }

        int ret = ::__close(stream->_file);

        free(stream);

        return ret;
    }

    return 0;
}

streamsize fread(void* buf, streamsize size, FILE* stream) noexcept
{
    if (size > std::numeric_limits<unsigned int>::max())
    {
        streamsize out = 0;
        streamsize number = size;

        unsigned int count = std::numeric_limits<unsigned int>::max();

        for (;;)
        {
            if (number > count)
            {
                number -= count;
            }
            else
            {
                count = (unsigned int)number;
            }

            if (count > 0)
            {
                int result = ::__read(stream->_file, buf, count);
                if (result != 0)
                {
                    out += result;
                    buf = (char*)buf + result;

                    if ((unsigned int)result != count)
                    {
                        break;
                    }
                }
            }
        }

        return out;
    }
    else
    {
        return (streamsize)::__read(stream->_file, buf, (unsigned int)size);
    }
}

streamsize fwrite(const void* buf, streamsize size, FILE* stream) noexcept
{
    if (size > std::numeric_limits<unsigned int>::max())
    {
        streamsize out = 0;
        streamsize number = size;

        unsigned int count = std::numeric_limits<unsigned int>::max();

        for (;;)
        {
            if (number > count)
            {
                number -= count;
            }
            else
            {
                count = (unsigned int)number;
            }

            if (count > 0)
            {
                int result = ::__write(stream->_file, buf, count);
                if (result != 0)
                {
                    out += result;
                    buf = (char*)buf + result;

                    if ((unsigned int)result != count)
                    {
                        break;
                    }
                }
            }
        }

        return out;
    }
    else
    {
        return (streamsize)::__write(stream->_file, buf, (unsigned int)size);
    }
}

int fgetc(FILE* stream) noexcept
{
    assert(stream);

    if (!stream->_base)
    {
        stream->_base = (char*)malloc(_INTERNAL_BUFSIZ);
    }

    if (stream->_base)
    {
        stream->_flag |= _IOMYBUF;
        stream->_bufsiz = _INTERNAL_BUFSIZ;
    }
    else
    {
        stream->_flag |= _IONBF;
        stream->_base = (char *)&(stream->_charbuf);
        stream->_bufsiz = 2;
    }

    stream->_ptr = stream->_base;
    stream->_cnt = fread(stream->_base, stream->_bufsiz, stream);

    if ((stream->_cnt == 0) || (stream->_cnt == -1))
    {
        stream->_flag |= stream->_cnt ? _IOERR : _IOEOF;
        stream->_cnt = 0;
        return EOF;
    }

#ifndef _UNICODE
    stream->_cnt--;
    return (0xff & *stream->_ptr++);
#else
	stream->_cnt -= sizeof(wchar_t);
    return (0xffff & *((wchar_t *)(*stream->_ptr++)));
#endif
}

streamoff ftell(FILE* stream) noexcept
{
#if defined(__WINDOWS__) || defined(__DOS__)
    return ::__tell(stream->_file);
#else
    return fseek(stream, 0, ios_base::cur);
#endif
}

streamoff fseek(FILE* stream, streamoff _off, ios_base::seekdir seek) noexcept
{
    // Just to check whether our enum maps one to one with the CRT constants
    static_assert(ios_base::beg == SEEK_SET &&
        ios_base::cur == SEEK_CUR &&
        ios_base::end == SEEK_END,
        "The seekdir does not match.");

    assert(seek == SEEK_SET || seek == SEEK_CUR || seek == SEEK_END);

    if (_off > std::numeric_limits<long>::max())
    {
        streamoff number = _off;

        long count = std::numeric_limits<long>::max();

        for (;;)
        {
            if (number > count)
            {
                number -= count;
            }
            else
            {
                count = (long)number;
            }

            if (count == 0)
            {
                break;
            }

			fcntl::seek(stream->_file, count, seek);
        }

        return ftell(stream);
    }
    else
    {
        return fcntl::seek(stream->_file, (long)_off, seek);
    }
}

IOFILE::IOFILE() noexcept
    :_stream(nullptr)
{
}

IOFILE::~IOFILE() noexcept
{
    this->close();
}

bool
IOFILE::is_open() const noexcept
{
    return _stream != nullptr;
}

IOFILE*
IOFILE::open(const char* filename, ios_base::openmode mode) noexcept
{
    if (_stream)
        fclose(_stream);

    _stream = fopen(filename, mode);
    if (_stream)
        return this;
    return nullptr;
}

IOFILE*
IOFILE::open(const wchar_t* filename, ios_base::openmode mode) noexcept
{
    if (_stream)
    {
        fclose(_stream);
    }

    _stream = fopen(filename, mode);
    if (_stream)
    {
        return this;
    }

    return nullptr;
}

IOFILE*
IOFILE::open(const char* filename, ios_base::open_mode mode) noexcept
{
    return this->open(filename, (ios_base::openmode)mode);
}

IOFILE*
IOFILE::open(const wchar_t* filename, ios_base::open_mode mode) noexcept
{
    return this->open(filename, (ios_base::openmode)mode);
}

IOFILE*
IOFILE::open(const std::string& filename, ios_base::openmode mode) noexcept
{
    return this->open(filename.c_str(), mode);
}

IOFILE*
IOFILE::open(const std::string& filename, ios_base::open_mode mode) noexcept
{
    return this->open(filename.c_str(), (ios_base::openmode)mode);
}

IOFILE*
IOFILE::open(const std::wstring& filename, ios_base::openmode mode) noexcept
{
    return this->open(filename.c_str(), mode);
}

IOFILE*
IOFILE::open(const std::wstring& filename, ios_base::open_mode mode) noexcept
{
    return this->open(filename.c_str(), (ios_base::openmode)mode);
}

int
IOFILE::getc() noexcept
{
    return fgetc(_stream);
}

streamoff
IOFILE::seek(streamoff _off, ios_base::seekdir seek) noexcept
{
    return fseek(_stream, _off, seek);
}

bool
IOFILE::close() noexcept
{
    if (_stream)
    {
        fclose(_stream);
        _stream = nullptr;
		return true;
    }

	return false;
}

streamoff
IOFILE::tell() const noexcept
{
    return ftell(_stream);
}

streamsize
IOFILE::read(void* buf, streamsize size) noexcept
{
    return fread(buf, size, _stream);
}

streamsize
IOFILE::write(const void* buf, streamsize size) noexcept
{
    return fwrite(buf, size, _stream);
}

int
IOFILE::flag() noexcept
{
    return _stream->_flag;
}

char*&
IOFILE::ptr()  noexcept
{
    return _stream->_ptr;
}

char*&
IOFILE::base() noexcept
{
    return _stream->_base;
}

streamsize&
IOFILE::cnt() noexcept
{
    return _stream->_cnt;
}

void
IOFILE::swap(IOFILE& other) noexcept
{
    std::swap(_stream, other._stream);
}

_NAME_END