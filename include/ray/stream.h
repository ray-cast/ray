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
#ifndef _H_STREAM_H_
#define _H_STREAM_H_

#include <ray/streambuf.h>

_NAME_BEGIN

class EXPORT stream
{
public:
    stream() noexcept;
    virtual ~stream() noexcept;

    void setOpenMode(ios_base::openmode mode) noexcept;
    ios_base::openmode getOpenMode() const noexcept;

    bool good() const noexcept;
    bool eof() const noexcept;
    bool fail() const noexcept;
    bool bad() const noexcept;

    void clear(ios_base::io_state _state);
    void clear(ios_base::iostate _state = ios_base::goodbit);
    void clear(ios_base::iostate _state, bool _reraise);

    void setstate(ios_base::iostate _state) noexcept;
    void setstate(ios_base::io_state _state) noexcept;
    void setstate(ios_base::iostate _state, bool _exreraise) noexcept;
    ios_base::iostate rdstate() const noexcept;

    void exceptions(ios_base::iostate _new) noexcept;
    ios_base::iostate exceptions() const noexcept;

    streambuf* rdbuf() const noexcept;
    void set_rdbuf(streambuf* buf) noexcept;

    ios_base::fmtflags flags() const noexcept;
    void swap(stream& other) noexcept;

    bool operator!() const noexcept;
    operator void*() const noexcept;

protected:

    void _init(streambuf* _buf, ios_base::openmode mode) noexcept;

private:
    stream& operator=(const stream&) = delete;
    stream(const stream&) = delete;

private:
    streambuf* _strbuf;

    ios_base::fmtflags _fmtfl;
    ios_base::iostate _my_state;
    ios_base::iostate _my_except;
    ios_base::openmode _mode;
};

_NAME_END

#endif