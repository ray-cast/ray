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
#include <ray/stream.h>
#include <ray/except.h>

_NAME_BEGIN

stream::stream() noexcept
{
}

stream::~stream() noexcept
{
}

stream::operator void*() const noexcept
{
    return this->fail() ? 0 : (void*)this;
};

bool
stream::operator!() const noexcept
{
    return this->fail();
}

bool
stream::good() const noexcept
{
    return (this->rdstate() == (ios_base::iostate)ios_base::goodbit);
}

bool
stream::eof() const noexcept
{
    return (this->rdstate() == (ios_base::iostate)ios_base::eofbit);
}

bool
stream::fail() const noexcept
{
    return ((this->rdstate() & (ios_base::iostate)(ios_base::badbit | ios_base::failbit)) != 0);
}

bool
stream::bad() const noexcept
{
    return (this->rdstate() == (ios_base::iostate)ios_base::badbit);
}

ios_base::iostate
stream::exceptions() const noexcept
{
    return _my_except;
}

ios_base::iostate
stream::rdstate() const noexcept
{
    return _my_state;
}

streambuf*
stream::rdbuf() const noexcept
{
    return _strbuf;
}

void
stream::set_rdbuf(streambuf* buf) noexcept
{
    _strbuf = buf;
}

ios_base::fmtflags
stream::flags() const noexcept
{
    return _fmtfl;
}

void
stream::swap(stream& other) noexcept
{
    std::swap(this->_my_state, other._my_state);
    std::swap(this->_my_except, other._my_except);
    std::swap(this->_fmtfl, other._fmtfl);
}

void
stream::clear(ios_base::io_state _state)
{
    this->clear((ios_base::iostate)_state);
}

void
stream::clear(ios_base::iostate _state)
{
    this->clear(_state, false);
}

void
stream::clear(ios_base::iostate _state, bool _reraise)
{
    _my_state = (ios_base::iostate)(_state & ios_base::_Statmask);
    if ((_my_state & _my_except) == 0)
        return;
    else if (_reraise)
        throw;
    else if (_my_state & _my_except & ios_base::badbit)
        throw failure("ios_base::badbit set");
    else if (_my_state & _my_except & ios_base::failbit)
        throw failure("ios_base::failbit set");
    else
        throw failure("ios_base::eofbit set");
}

void
stream::setstate(ios_base::iostate _state) noexcept
{
    this->setstate(_state, false);
}

void
stream::setstate(ios_base::io_state _state) noexcept
{
    this->setstate((ios_base::iostate)_state);
}

void
stream::setstate(ios_base::iostate _state, bool _exreraise) noexcept
{
    if (_state != ios_base::goodbit)
        this->clear((ios_base::iostate)((int)this->rdstate() | (int)_state), _exreraise);
}

void
stream::setOpenMode(ios_base::openmode mode) noexcept
{
    _mode = mode;
}

ios_base::openmode
stream::getOpenMode() const noexcept
{
    return _mode;
}

void
stream::exceptions(ios_base::iostate _new) noexcept
{
    _my_except = (ios_base::iostate)((int)(_new)& (int)ios_base::_Statmask);
    this->clear(_my_state);
}

void
stream::_init(streambuf* _buf, ios_base::openmode mode) noexcept
{
    _strbuf = _buf;
    _fmtfl = (ios_base::fmtflags)(ios_base::skipws | ios_base::dec);
    _my_except = ios_base::goodbit;
    _mode = mode;
    this->clear(ios_base::goodbit);
}

_NAME_END