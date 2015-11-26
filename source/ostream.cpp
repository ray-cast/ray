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
#include <ray/iolistener.h>

_NAME_BEGIN

class osentry
{
public:
    explicit osentry(ostream* _ostr)
        :_my_ostr(_ostr)
    {
        if (_my_ostr->rdbuf() != 0)
            _my_ostr->rdbuf()->lock();
        _ok = _ostr->good();
    }

    ~osentry() noexcept
    {
        if (_my_ostr->rdbuf() != 0)
            _my_ostr->rdbuf()->unlock();
    }

    explicit operator bool() const
    {
        return (_ok ? true : false);
    }

private:
    bool _ok;
    ostream* _my_ostr;
    osentry(const osentry&);
    const osentry& operator=(const osentry&);
};

ostream::ostream(streambuf* buf, bool _isstd) noexcept
{
    stream::_init(buf, ios_base::out);
}

ostream::~ostream() noexcept
{
}

bool
ostream::is_open() const noexcept
{
    return this->rdbuf()->is_open();
}

ostream&
ostream::open(const char* filename) noexcept
{
    const osentry ok(this);
    if (ok)
    {
        ios_base::openmode mode = getOpenMode();

        ioListenerPoster::onOpenBefor(filename, mode);

        if (!this->rdbuf()->open(filename, mode))
        {
            ioListenerPoster::onOpenFail(filename, mode);

            this->setstate(ios_base::failbit);
        }
        else
        {
            ioListenerPoster::onOpenSuccess(filename, mode);

            this->clear(ios_base::goodbit);
        }
    }

    return (*this);
}

ostream&
ostream::open(const wchar_t* filename) noexcept
{
    const osentry ok(this);
    if (ok)
    {
        ios_base::openmode mode = getOpenMode();

        ioListenerPoster::onOpenBefor(filename, mode);

        if (!this->rdbuf()->open(filename, mode))
        {
            ioListenerPoster::onOpenFail(filename, mode);

            this->setstate(ios_base::failbit);
        }
        else
        {
            ioListenerPoster::onOpenSuccess(filename, mode);

            this->clear(ios_base::goodbit);
        }
    }

    return (*this);
}

ostream&
ostream::open(const std::string& filename) noexcept
{
    return this->open(filename.c_str());
}

ostream&
ostream::open(const std::wstring& filename) noexcept
{
    return this->open(filename.c_str());
}

ostream&
ostream::write(const char* str, std::streamsize cnt) noexcept
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

ostream&
ostream::write(const char* str, streamsize size, streamsize cnt) noexcept
{
    return this->write(str, size * cnt);
}

ostream&
ostream::flush() noexcept
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

ostream&
ostream::seekg(ios_base::off_type pos) noexcept
{
    const osentry ok(this);
    if (ok)
    {
        if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, ios_base::out) == ios_base::_BADOFF)
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

ostream&
ostream::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
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
ostream::gcount() const noexcept
{
    return _count;
}

ios_base::pos_type
ostream::tellg() noexcept
{
    const osentry ok(this);

    if (!this->fail())
        return (this->rdbuf()->tellg());
    else
        return (ios_base::pos_type(ios_base::_BADOFF));
}

ostream&
ostream::close() noexcept
{
    const osentry ok(this);
    if (ok)
    {
        if (!this->fail())
        {
            this->rdbuf()->close();
        }
    }

    return (*this);
}

ostream&
ostream::copy(ostream& other) noexcept
{
    _count = other._count;
    stream::copy(other);
    return *this;
}

ostream&
ostream::operator << (const char* str) noexcept
{
    return this->write(str, (std::streamsize)std::strlen(str));
}

ostream&
ostream::operator << (const std::string& str) noexcept
{
    return this->write(str.c_str(), (std::streamsize)str.size());
}

ostream&
ostream::operator << (char value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[MAX_PATH];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, MAX_PATH, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

ostream&
ostream::operator << (unsigned char value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[MAX_PATH];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, MAX_PATH, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

ostream&
ostream::operator << (short value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[MAX_PATH];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, MAX_PATH, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

ostream&
ostream::operator << (long long value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[MAX_PATH];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, MAX_PATH, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

ostream&
ostream::operator << (unsigned short value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[MAX_PATH];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, MAX_PATH, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

ostream&
ostream::operator << (int value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[MAX_PATH];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, MAX_PATH, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

ostream&
ostream::operator << (unsigned int value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[MAX_PATH];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, MAX_PATH, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

ostream&
ostream::operator << (unsigned long value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[MAX_PATH];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, MAX_PATH, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

ostream&
ostream::operator << (unsigned long long value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[MAX_PATH];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::itoa10(buf, MAX_PATH, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

ostream&
ostream::operator << (float value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[MAX_PATH];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::ftoa10(buf, MAX_PATH, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

ostream&
ostream::operator << (double value) noexcept
{
    const osentry _Ok(this);
    if (_Ok)
    {
        char buf[MAX_PATH];

        if (this->flags() & ios_base::dec)
        {
            auto written = util::dtoa10(buf, MAX_PATH, value);
            if (written > 0)
            {
                return this->write(buf, written);
            }
        }
    }

    this->setstate(ios_base::failbit);
    return *this;
}

ostream&
ostream::operator << (ios_base& (*function)(ios_base&)) noexcept
{    // call ios_base manipulator
    assert(function);
    (*function)(*(ios_base *)this);
    return (*this);
}

ostream&
ostream::operator << (ostream& (*function)(ostream&)) noexcept
{
    assert(function);
    (*function)(*this);
    return (*this);
}

_NAME_END