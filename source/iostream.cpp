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
#include <ray/iostream.h>

_NAME_BEGIN

Stream::iosentry::iosentry(Stream* _istr)
    :_myIostr(_istr)
    , _ok(true)
{
    if (_myIostr->rdbuf() != 0)
        _myIostr->rdbuf()->lock();
}

Stream::iosentry::~iosentry() noexcept
{
    if (_myIostr->rdbuf() != 0)
        _myIostr->rdbuf()->unlock();
};

Stream::iosentry::operator bool() const noexcept
{
    return _ok ? true : false;
}

Stream::Stream(streambuf* buf) noexcept
    : StreamReader(buf)
    , StreamWrite(buf)
    , _count(0)
{
	StreamBase::_init(buf, ios_base::in | ios_base::out);
}

Stream::~Stream() noexcept
{
}

Stream&
Stream::read(char* str, std::streamsize cnt) noexcept
{
    assert(cnt != 0);

    try
    {
        ios_base::iostate state = ios_base::goodbit;

        const iosentry ok(this);

        if (ok)
        {
            _count = this->rdbuf()->read(str, cnt);

            if (_count != cnt)
                state |= ios_base::badbit;
        }

        this->setstate(state);
    }
    catch (...)
    {
        this->setstate(ios_base::badbit, true);
    }

    return (*this);
}

Stream&
Stream::read(char* str, streamsize size, streamsize cnt) noexcept
{
    return this->read(str, size * cnt);
}

Stream&
Stream::write(const char* str, std::streamsize cnt) noexcept
{
    assert(cnt != 0);

    try
    {
        ios_base::iostate state = ios_base::goodbit;

        const iosentry ok(this);

        if (ok)
        {
            if (this->rdbuf()->write(str, cnt) != cnt)
                state |= ios_base::badbit;
        }

        this->setstate(state);
    }
    catch (...)
    {
        this->setstate(ios_base::badbit, true);
    }

    return (*this);
}

Stream&
Stream::write(const char* str, streamsize size, streamsize cnt) noexcept
{
    return this->write(str, size * cnt);
}

Stream&
Stream::flush() noexcept
{
    if (this->rdbuf() != 0)
    {
        const iosentry ok(this);
        if (ok)
        {
            if (ok && this->rdbuf()->flush() == -1)
                this->setstate(ios_base::badbit, true);
        }
    }

    return (*this);
}

Stream&
Stream::seekg(ios_base::off_type pos) noexcept
{
    const iosentry ok(this);
    if (ok)
    {
        if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, ios_base::out) == ios_base::_BADOFF)
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

Stream&
Stream::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
{
    const iosentry ok(this);
    if (ok)
    {
        if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, dir) == ios_base::_BADOFF)
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

streamsize
Stream::size() noexcept
{
    const iosentry ok(this);
    if (ok)
    {
        if (!this->fail())
            return this->rdbuf()->size();
    }

    return (streamsize)ios_base::_BADOFF;
}

streamsize
Stream::gcount() const noexcept
{
    return _count;
}

streampos
Stream::tellg() noexcept
{
    const iosentry ok(this);

    if (!this->fail())
        return (this->rdbuf()->tellg());
    else
        return (ios_base::pos_type(ios_base::_BADOFF));
}

Stream& 
Stream::copy(StreamReader& other) noexcept
{
	_count = other.gcount();
	StreamBase::copy(other);
	return *this;
}

Stream&
Stream::copy(Stream& other) noexcept
{
    _count = other._count;
	StreamBase::copy(other);
    return *this;
}

_NAME_END