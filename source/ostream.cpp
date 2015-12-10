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
#include <ray/ostream.h>

_NAME_BEGIN

StreamWrite::osentry::osentry(StreamWrite* _ostr)
    :_my_ostr(_ostr)
{
    if (_my_ostr->rdbuf() != 0)
        _my_ostr->rdbuf()->lock();
    _ok = _ostr->good();
}

StreamWrite::osentry::~osentry() noexcept
{
    if (_my_ostr->rdbuf() != 0)
        _my_ostr->rdbuf()->unlock();
}

StreamWrite::osentry::operator bool() const noexcept
{
    return (_ok ? true : false);
}

StreamWrite::StreamWrite(streambuf* buf) noexcept
{
    StreamBase::_init(buf, ios_base::out);
}

StreamWrite::~StreamWrite() noexcept
{
}

StreamWrite&
StreamWrite::write(const char* str, std::streamsize cnt) noexcept
{
    assert(cnt != 0);

    try
    {
        ios_base::iostate state = ios_base::goodbit;

        const osentry ok(this);

        if (ok)
        {
            _count = this->rdbuf()->write(str, cnt);
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

StreamWrite&
StreamWrite::write(const char* str, streamsize size, streamsize cnt) noexcept
{
    return this->write(str, size * cnt);
}

StreamWrite&
StreamWrite::flush() noexcept
{
    if (this->rdbuf() != 0)
    {
        const osentry ok(this);
        if (ok)
        {
            if (ok && this->rdbuf()->flush() == -1)
                this->setstate(ios_base::badbit, true);
        }
    }

    return (*this);
}

StreamWrite&
StreamWrite::seekg(ios_base::off_type pos) noexcept
{
    const osentry ok(this);
    if (ok)
    {
        if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, ios_base::out) == ios_base::_BADOFF)
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

StreamWrite&
StreamWrite::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
{
    const osentry ok(this);
    if (ok)
    {
        if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, dir) == ios_base::_BADOFF)
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

streamsize
StreamWrite::gcount() const noexcept
{
    return _count;
}

ios_base::pos_type
StreamWrite::tellg() noexcept
{
    const osentry ok(this);

    if (!this->fail())
        return (this->rdbuf()->tellg());
    else
        return (ios_base::pos_type(ios_base::_BADOFF));
}

StreamWrite&
StreamWrite::operator << (const char* str) noexcept
{
    return this->write(str, (std::streamsize)std::strlen(str));
}

StreamWrite&
StreamWrite::operator << (const std::string& str) noexcept
{
    return this->write(str.c_str(), (std::streamsize)str.size());
}

StreamWrite&
StreamWrite::operator << (char value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[PATHLIMIT];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, PATHLIMIT, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

StreamWrite&
StreamWrite::operator << (unsigned char value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[PATHLIMIT];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, PATHLIMIT, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

StreamWrite&
StreamWrite::operator << (short value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[PATHLIMIT];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, PATHLIMIT, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

StreamWrite&
StreamWrite::operator << (long long value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[PATHLIMIT];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, PATHLIMIT, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

StreamWrite&
StreamWrite::operator << (unsigned short value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[PATHLIMIT];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, PATHLIMIT, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

StreamWrite&
StreamWrite::operator << (int value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[PATHLIMIT];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, PATHLIMIT, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

StreamWrite&
StreamWrite::operator << (unsigned int value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[PATHLIMIT];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, PATHLIMIT, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

StreamWrite&
StreamWrite::operator << (unsigned long value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[PATHLIMIT];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, PATHLIMIT, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

StreamWrite&
StreamWrite::operator << (unsigned long long value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[PATHLIMIT];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, PATHLIMIT, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

StreamWrite&
StreamWrite::operator << (float value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[PATHLIMIT];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::ftoa10(buf, PATHLIMIT, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

StreamWrite&
StreamWrite::operator << (double value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[PATHLIMIT];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::dtoa10(buf, PATHLIMIT, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

StreamWrite&
StreamWrite::operator << (ios_base& (*function)(ios_base&)) noexcept
{    // call ios_base manipulator
    assert(function);
    (*function)(*(ios_base *)this);
    return (*this);
}

StreamWrite&
StreamWrite::operator << (StreamWrite& (*function)(StreamWrite&)) noexcept
{
    assert(function);
    (*function)(*this);
    return (*this);
}

_NAME_END