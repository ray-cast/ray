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
#ifndef _H_THREAD_LOCAL_H_
#define _H_THREAD_LOCAL_H_

#include <ray/memory.h>
#include <ray/trait.h>

_NAME_BEGIN

template<typename T>
class ThreadLocal
{
public:
	typedef ThreadLocal<T> _Myt;
	typedef typename trait::_typeaddition<T>::pointer pointer;
	typedef typename trait::_typeaddition<T>::const_pointer const_pointer;
	typedef typename trait::_typeaddition<T>::reference reference;
	typedef typename trait::_typeaddition<T>::const_reference const_reference;

	ThreadLocal() noexcept
	{
	}

	ThreadLocal(pointer local) noexcept
	{
		this->reset(local);
	}

	ThreadLocal(const ThreadLocal& local) noexcept
	{
		if (&local == this)
			return;
		*this = local;
	}

	~ThreadLocal() noexcept
	{
		this->release();
	}

	explicit operator bool() noexcept
	{
		return (this->get() != nullptr);
	}

	_Myt& operator=(const _Myt& other) noexcept
	{
		this->_Reset(other);
		return *this;
	}

	pointer get() noexcept
	{
		return this->_Get();
	}

	void reset(pointer local) noexcept
	{
		this->_Reset(local);
	}

	void release(bool force = false) noexcept
	{
		--_cnt;
		if (0 == _cnt || force)
		{
			delete _Myptr;
			_Myptr = nullptr;
		}
	}

	pointer operator->() noexcept
	{
		return this->_Get();
	}

private:

	void _Reset(pointer ptr) noexcept
	{
		if (ptr == _Myptr && _Myptr == nullptr)
			return;

		if (_Myptr)
			this->release(true);

		_Myptr = ptr;
		++_cnt;
	}

	void _Reset(const _Myt& other)
	{
		_Myptr = other._Myptr;
		_cnt = other._cnt;
		++_cnt;
	}

	pointer _Get() noexcept
	{
		return (_Myptr);
	}

	const_pointer _Get() const noexcept
	{
		return (_Myptr);
	}

	static __declspec(thread) T* _Myptr;
	static __declspec(thread) std::size_t _cnt;
};

template<typename T> __declspec(thread) T* ThreadLocal<T>::_Myptr;
template<typename T> __declspec(thread) std::size_t ThreadLocal<T>::_cnt;

_NAME_END

#endif
