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
#ifndef _H_IOSBASE_H_
#define _H_IOSBASE_H_

#include <ray/iosfwd.h>

_NAME_BEGIN

typedef std::streamoff streamoff;
typedef std::streampos streampos;
typedef std::streamsize streamsize;

template<typename _Dummy>
class _Iosb
{
public:
	enum _FmtFlags { _Fmtmask = 0xffff, _Fmtzero = 0 };
	static const _FmtFlags skipws = (_FmtFlags)std::ios_base::skipws;
	static const _FmtFlags unitbuf = (_FmtFlags)std::ios_base::unitbuf;
	static const _FmtFlags uppercase = (_FmtFlags)std::ios_base::uppercase;
	static const _FmtFlags showbase = (_FmtFlags)std::ios_base::showbase;
	static const _FmtFlags showpoint = (_FmtFlags)std::ios_base::showpoint;
	static const _FmtFlags showpos = (_FmtFlags)std::ios_base::showpos;
	static const _FmtFlags left = (_FmtFlags)std::ios_base::left;
	static const _FmtFlags right = (_FmtFlags)std::ios_base::right;
	static const _FmtFlags internal = (_FmtFlags)std::ios_base::internal;
	static const _FmtFlags dec = (_FmtFlags)std::ios_base::dec;
	static const _FmtFlags oct = (_FmtFlags)std::ios_base::oct;
	static const _FmtFlags hex = (_FmtFlags)std::ios_base::hex;
	static const _FmtFlags scientific = (_FmtFlags)std::ios_base::scientific;
	static const _FmtFlags fixed = (_FmtFlags)std::ios_base::fixed;
	static const _FmtFlags boolalpha = (_FmtFlags)std::ios_base::boolalpha;

	enum _Iostate { _Statmask = 0x17 };

	static const _Iostate goodbit = (_Iostate)std::ios_base::goodbit;
	static const _Iostate eofbit = (_Iostate)std::ios_base::eofbit;
	static const _Iostate failbit = (_Iostate)std::ios_base::failbit;
	static const _Iostate badbit = (_Iostate)std::ios_base::badbit;

	enum _Openmode { _Openmask = 0xff };

	static const _Openmode in = (_Openmode)std::ios_base::in;
	static const _Openmode out = (_Openmode)std::ios_base::out;
	static const _Openmode ate = (_Openmode)std::ios_base::ate;
	static const _Openmode app = (_Openmode)std::ios_base::app;
	static const _Openmode trunc = (_Openmode)std::ios_base::trunc;
	static const _Openmode binary = (_Openmode)std::ios_base::binary;

	enum _Seekdir { _Seekmask = 0x3 };
	static const _Seekdir beg = (_Seekdir)std::ios_base::beg;
	static const _Seekdir cur = (_Seekdir)std::ios_base::cur;
	static const _Seekdir end = (_Seekdir)std::ios_base::end;

	enum { _Openprot = 0x40 };

	static const streamoff _BADOFF = (streamoff)-1;
};

template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::skipws;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::unitbuf;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::uppercase;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::showbase;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::showpoint;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::showpos;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::left;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::right;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::internal;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::dec;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::oct;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::hex;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::scientific;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::fixed;
template<typename _Dummy> const typename _Iosb<_Dummy>::_FmtFlags _Iosb<_Dummy>::boolalpha;

template<typename _Dummy> const typename _Iosb<_Dummy>::_Iostate _Iosb<_Dummy>::goodbit;
template<typename _Dummy> const typename _Iosb<_Dummy>::_Iostate _Iosb<_Dummy>::eofbit;
template<typename _Dummy> const typename _Iosb<_Dummy>::_Iostate _Iosb<_Dummy>::failbit;
template<typename _Dummy> const typename _Iosb<_Dummy>::_Iostate _Iosb<_Dummy>::badbit;

template<typename _Dummy> const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::in;
template<typename _Dummy> const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::out;
template<typename _Dummy> const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::ate;
template<typename _Dummy> const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::app;
template<typename _Dummy> const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::trunc;
template<typename _Dummy> const typename _Iosb<_Dummy>::_Openmode _Iosb<_Dummy>::binary;

template<typename _Dummy> const typename _Iosb<_Dummy>::_Seekdir _Iosb<_Dummy>::beg;
template<typename _Dummy> const typename _Iosb<_Dummy>::_Seekdir _Iosb<_Dummy>::cur;
template<typename _Dummy> const typename _Iosb<_Dummy>::_Seekdir _Iosb<_Dummy>::end;

template<typename _Dummy> const streamoff _Iosb<_Dummy>::_BADOFF;

class EXPORT ios_base : public _Iosb<int>
{
public:
	_INT_BITMASK(_Iostate, iostate);
	_INT_BITMASK(_Openmode, openmode);
	_INT_BITMASK(_Seekdir, seekdir);
	_INT_BITMASK(_FmtFlags, fmtflags);

	typedef unsigned int io_state, open_mode, seek_dir;

	typedef int int_type;
	typedef int pos_type;
	typedef int off_type;

public:
	ios_base() noexcept;
	~ios_base() noexcept;

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

	ios_base::fmtflags flags() const noexcept;

	bool operator!() const noexcept;
	operator bool() const noexcept;

protected:
	void _init() noexcept;

private:
	ios_base(const ios_base&) = delete;
	const ios_base& operator=(const ios_base&) = delete;

private:

	ios_base::fmtflags _fmtfl;

	ios_base::iostate _my_state;
	ios_base::iostate _my_except;
};

_NAME_END

#endif