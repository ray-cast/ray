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
#ifndef _H_ERROR_H_
#define _H_ERROR_H_

#include <ray/except.h>

_NAME_BEGIN

class EXPORT runtime_state
{
public:
	enum _state { _Statmask = 0x17 };
	static const _state goodbit = (_state)0x0;
	static const _state eofbit = (_state)0x1;
	static const _state failbit = (_state)0x2;
	static const _state badbit = (_state)0x3;

	typedef int stateval;
	typedef int oldstate;

	runtime_state() noexcept
	{
		this->_Init();
	}

	runtime_state(stateval state) noexcept
	{
		this->_Init(state);
	}

	operator void*() const noexcept
	{
		return this->fail() ? 0 : (void*)this;
	};

	bool operator!() const noexcept
	{
		return this->fail();
	}

	bool good() const noexcept
	{
		return (this->rdstate() == (stateval)goodbit);
	}

	bool fail() const noexcept
	{
		return ((this->rdstate() & (stateval)(badbit | failbit)) != 0);
	}

	bool bad() const noexcept
	{
		return (this->rdstate() == (stateval)badbit);
	}

protected:
	void clear(stateval _state, bool _reraise)
	{
		_Mystate = (stateval)(_state & _Statmask);
		if ((_Mystate & _Myexcept) == 0)
			return;
		else if (_reraise)
			throw;
		else if (_Mystate & _Myexcept & badbit)
			throw failure("badbit set");
		else if (_Mystate & _Myexcept & failbit)
			throw failure("failbit set");
	}

	void clear(stateval _state = goodbit) noexcept
	{
		this->clear(_state, false);
	}

	stateval rdstate() const noexcept
	{
		return (this->_Mystate);
	}

	oldstate setstate(stateval _state, bool _exreraise) noexcept
	{
		stateval tmp = this->rdstate();

		if (_state != goodbit)
			this->clear((stateval)((int)this->rdstate() | (int)_state), _exreraise);

		return tmp;
	}

	oldstate setstate(stateval _state) noexcept
	{
		return this->setstate(_state, false);
	}

	void _Init(stateval _state = goodbit) noexcept
	{
		_Myexcept = goodbit;
		this->clear(_state);
	}

private:
	stateval _Mystate;
	stateval _Myexcept;
};

_NAME_END

#endif