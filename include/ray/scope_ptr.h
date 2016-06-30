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
#ifndef _H_SCOPE_GUARD_H_
#define _H_SCOPE_GUARD_H_

#include <ray/def.h>

_NAME_BEGIN

template<typename _Ty>
class scope_base
{
public:
	typedef _Ty* pointer;
	typedef const _Ty* const_pointer;
	typedef _Ty& reference;
	typedef const _Ty& const_reference;

	scope_base() noexcept
	{
		this->_Init();
	};

	scope_base(pointer obj) noexcept
	{
		this->_Init(obj);
	}

	scope_base(const scope_base& copy)
	{
		this->_Init(copy.dismiss());
	}

	pointer dismiss() const noexcept
	{
		_mdismiss = true;
		return _Myptr;
	}

	pointer get() noexcept
	{
		return (_Myptr);
	}

	const_pointer get() const noexcept
	{
		return (_Myptr);
	}

	pointer operator->() noexcept
	{
		return (this->get());
	}

	const_pointer operator->() const noexcept
	{
		return (this->get());
	}

	reference operator*() noexcept
	{
		return *(this->get());
	}

	const_reference operator*() const noexcept
	{
		return *(this->get());
	}

	explicit operator pointer() noexcept
	{
		return (this->get());
	}

	operator bool() const noexcept
	{
		return (this->get() != nullptr);
	}

	scope_base& operator=(scope_base& other)
	{
		_mdismiss = other._mdismiss;
		_Myptr = other.dismiss();
		return *this;
	}

	bool operator!=(const_pointer other) noexcept
	{
		return _Myptr != other;
	}

protected:

	void _Init(pointer ptr = nullptr) noexcept
	{
		_Myptr = ptr;
		_mdismiss = false;
	}

	pointer _Myptr;
	mutable bool _mdismiss;
};

template<typename _Ty, typename S = void>
class scope_ptr : public scope_base<_Ty>
{
public:
	typedef scope_base<_Ty> _Mybase;
	typedef scope_ptr<_Ty, S> _Myt;
	typedef _Ty* pointer;
	typedef const _Ty* const_pointer;

	scope_ptr() noexcept
	{
	};

	scope_ptr(pointer obj) noexcept
		:_Mybase(obj)
	{
	}

	scope_ptr(const _Myt& other) noexcept
	{
		_Mybase::_Init(other.dismiss());
	}

	scope_ptr(_Myt&& other) noexcept
	{
		_Mybase::_Init(other.dismiss());
	}

	~scope_ptr() noexcept
	{
		this->_Destroy();
	}

	void reset(pointer ptr) noexcept
	{
		this->_Destroy();
		_Mybase::_Init(ptr);
	}

	void release() noexcept
	{
		this->_Destroy();
		_Mybase::_Init(nullptr);
	}

	scope_ptr& operator=(pointer other) noexcept
	{
		this->reset(other);
		return *this;
	}

	scope_ptr& operator=(_Myt&& other) noexcept
	{
		_Mybase::_Init(other.dismiss());
		return *this;
	}

private:

	void _Destroy() noexcept
	{
		if (!_Mybase::_mdismiss)
		{
			delete _Mybase::_Myptr;
			_Mybase::_Myptr = nullptr;
		}
	}
};

template<typename _Ty, typename S>
class scope_ptr<_Ty[], S> : public scope_base<_Ty>
{
public:
	typedef scope_base<_Ty> _Mybase;
	typedef scope_ptr<_Ty[], S> _Myt;
	typedef _Ty* pointer;
	typedef const _Ty* const_pointer;
	typedef _Ty& reference;
	typedef const _Ty& const_reference;

	scope_ptr() noexcept
	{
	};

	scope_ptr(pointer obj) noexcept
		:_Mybase(obj)
	{
	}

	scope_ptr(const _Myt& other) noexcept
	{
		this->_Init(other.dismiss());
	}

	scope_ptr(_Myt&& other) noexcept
	{
		this->_Init(other.dismiss());
	}

	~scope_ptr() noexcept
	{
		this->_Destroy();
	}

	void reset(pointer ptr) noexcept
	{
		this->_Destroy();
		this->_Init(ptr);
	}

	void release() noexcept
	{
		this->_Destroy();
		this->_Init(nullptr);
	}

	scope_ptr& operator=(pointer other) noexcept
	{
		this->reset(other);
		return *this;
	}

	scope_ptr& operator=(_Myt& other) noexcept
	{
		this->_Init(other.dismiss());
	}

	reference operator[](std::size_t n) noexcept
	{
		return _Mybase::_Myptr[n];
	}

private:

	void _Destroy() noexcept
	{
		if (!_Mybase::_mdismiss)
		{
			delete[] _Mybase::_Myptr;
			_Mybase::_Myptr = nullptr;
		}
	}
};

template<class _Ty, class... _Types> inline
typename std::enable_if<!std::is_array<_Ty>::value, scope_ptr<_Ty> >::type make_scope(_Types&&... _Args)
{
	return (scope_ptr<_Ty>(new _Ty(std::forward<_Types>(_Args)...)));
}

template<class _Ty> inline
typename std::enable_if<std::is_array<_Ty>::value && std::extent<_Ty>::value == 0, scope_ptr<_Ty>>::type make_scope(std::size_t _Size)
{
	typedef typename std::remove_extent<_Ty>::type _Elem;
	return (scope_ptr<_Ty>(new _Elem[_Size]()));
}

_NAME_END

#endif