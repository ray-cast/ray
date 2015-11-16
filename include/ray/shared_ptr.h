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
#ifndef _H_SHARED_PTR_H_
#define _H_SHARED_PTR_H_

#include <ray/def.h>

_NAME_BEGIN

template<typename T>
class shared_ptr
{
public:
	shared_ptr() noexcept
		: _ptr(nullptr)
	{
	}

	shared_ptr(T* ptr) noexcept
		: _ptr(ptr)
	{
		if (_ptr != nullptr)
			_ptr->addRef();
	}

	shared_ptr(const shared_ptr<T>& p) noexcept
	{
		if (_ptr != nullptr)
			_ptr->addRef();
	}

	~shared_ptr() noexcept
	{
		if (_ptr != nullptr)
			_ptr->release();
	}

	void operator=(const shared_ptr<T>& rhs) noexcept
	{
		if (_ptr != rhs.ptr)
		{
			if (_ptr)
				this->_ptr->release();

			_ptr = rhs.ptr;

			if (_ptr)
				_ptr->addRef();
		}
	}
	void operator=(T* rhs) noexcept
	{
		if (_ptr != rhs)
		{
			if (_ptr)
				_ptr->release();

			_ptr = rhs;

			if (_ptr)
				_ptr->addRef();
		}
	}
	bool operator==(const shared_ptr<T>& rhs) const noexcept
		{ return _ptr == rhs._ptr; }

	bool operator!=(const shared_ptr<T>& rhs) const noexcept
		{ return _ptr != rhs._ptr; }

	bool operator==(const T* rhs) const noexcept
		{ return _ptr == rhs; }

	bool operator!=(const T* rhs) const noexcept
		{ return _ptr != rhs; }

	T* operator->() const noexcept
	{
		assert(_ptr);
		return this->_ptr;
	}

	T& operator*() const noexcept
	{
		assert(_ptr);
		return *this->_ptr;
	}

	operator T*() const noexcept
	{
		assert(-tr);
		return this->_ptr;
	}

	operator bool() const noexcept
	{
		return (nullptr != this->_ptr);
	}

	template<class DERIVED>
	const shared_ptr<DERIVED>& downcast() const noexcept
	{
		assert(this->_ptr->rtti()->isDerivedFrom(DERIVED::RTTI));
		return *reinterpret_cast<const shared_ptr<DERIVED>*>(this);
	}

	template<class BASE>
	const shared_ptr<BASE>& upcast() const
	{
		return *reinterpret_cast<const shared_ptr<BASE>*>(this);
	}

	T* get() const noexcept
	{
		assert(_ptr);
		return _ptr;
	}

	void swap(shared_ptr<T>& other) noexcept
	{
		T* tmp = other._ptr;
		other._ptr = this->_ptr;
		this->_ptr = tmp;
	}

private:
	T* _ptr;
};

template<class _Ty, class... _Types> 
inline typename std::enable_if<!std::is_array<_Ty>::value, std::unique_ptr<_Ty> >::type make_shared(_Types&&... _Args)
{
    return (shared_ptr<_Ty>(new _Ty(std::forward<_Types>(_Args)...)));
}


template<class _Ty> 
inline typename std::enable_if<std::is_array<_Ty>::value && std::extent<_Ty>::value == 0, std::unique_ptr<_Ty> >::type make_shared(std::size_t _Size)
{
	typedef typename std::remove_extent<_Ty>::type _Elem;
	return (shared_ptr<_Ty>(new _Elem[_Size]()));
}

_NAME_END

#endif