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
#if DELEGATE_MAX_ARGS >= 0

// class T0, class T1, class T2, ...
#define DELEGATE_TEMPLATE_PARAMS MAKE_PARAMS1(DELEGATE_MAX_ARGS, class T)

// T0, T1, T2, ...
#define DELEGATE_TEMPLATE_ARGS MAKE_PARAMS1(DELEGATE_MAX_ARGS, T)

// T0 Arg0, T1 Arg1, T2 Arg2, ...
#define DELEGATE_FUNCTION_PARAMS MAKE_PARAMS2(DELEGATE_MAX_ARGS, T, &arg)

// Arg0, Arg1, Arg2, ...
#define DELEGATE_FUNCTION_ARGS MAKE_PARAMS1(DELEGATE_MAX_ARGS, arg)

#if DELEGATE_MAX_ARGS == 0
#define DELEGATE_COMMA
#else
#define DELEGATE_COMMA ,
#endif

template <typename Result DELEGATE_COMMA DELEGATE_TEMPLATE_PARAMS>
class delegate<Result(DELEGATE_TEMPLATE_ARGS)>
{
public:
	typedef delegate<Result(DELEGATE_TEMPLATE_ARGS)> _Myt;

	typedef std::function<Result(DELEGATE_TEMPLATE_ARGS)> _MyFunction;
	typedef std::vector<_MyFunction*> DELEGATES;
	typedef typename DELEGATES::iterator iterator;
	typedef typename DELEGATES::const_iterator const_iterator;
	typedef typename DELEGATES::size_type size_type;

public:
	delegate() noexcept
		: _functions(nullptr)
	{
	}

	delegate(const _Myt& copy) noexcept
		: _functions(nullptr)
	{
		this->assign(copy);
	}

	~delegate() noexcept
	{
		this->clear();
		delete _functions;
	}

	void attach(_MyFunction* callback)
	{
		if (!_functions)
			_functions = new DELEGATES;

		_functions->push_back(callback);
	}

	Result run(DELEGATE_FUNCTION_PARAMS)
	{
		if (!_functions || _functions->empty())
			return InvalidCall<Result>();

		return Partial<Result>::run(*this DELEGATE_COMMA DELEGATE_FUNCTION_ARGS);
	}

	size_type size() const noexcept
	{
		return (_functions ? _functions->size() : 0);
	}

	bool find(const _MyFunction* callback) const
	{
		if (_functions)
		{
			for (auto& it : *_functions)
			{
				if (callback == it)
					return true;
			}
		}

		return false;
	}

	void remove(const _MyFunction* callback)
	{
		if (_functions)
		{
			auto it = _functions->begin();
			auto end = _functions->end();
			for (; it != end; ++it)
			{
				if (callback == (*it))
				{
					_functions->erase(it);
					break;
				}
			}
		}
	}

	bool empty() noexcept
	{
		return (_functions && !_functions->empty() ? false : true);
	}

	void clear() noexcept
	{
		if (!_functions) { return; }
		_functions->clear();
	}

	Result operator()(DELEGATE_FUNCTION_PARAMS)
	{
		return run(DELEGATE_FUNCTION_ARGS);
	}

	_Myt& operator+=(const _MyFunction* t1)
	{
		this->attach(t1);
		return *this;
	}

	_Myt& operator-=(const _MyFunction* t1)
	{
		this->remove(t1);
		return *this;
	}

private:
	template<typename T, typename S = void>
	struct Partial
	{
		static T run(const _Myt& self DELEGATE_COMMA DELEGATE_FUNCTION_PARAMS)
		{
			const_iterator it = self._functions->begin();
			const_iterator end = self._functions->end() - 1;

			for (; it != end; ++it)
				*(*it)(DELEGATE_FUNCTION_ARGS);

			return *(*it)(DELEGATE_FUNCTION_ARGS);
		}
	};

	template<typename S>
	struct Partial<void, S>
	{
		static void run(const _Myt& self DELEGATE_COMMA DELEGATE_FUNCTION_PARAMS)
		{
			for (auto& it : *self._functions)
				(*it)(DELEGATE_FUNCTION_ARGS);
		}
	};

private:
	DELEGATES* _functions;
};

template <typename Result DELEGATE_COMMA DELEGATE_TEMPLATE_PARAMS>
bool operator==(const delegate<Result(DELEGATE_TEMPLATE_ARGS)>& lhs, const delegate<Result(DELEGATE_TEMPLATE_ARGS)>& rhs) noexcept
{
	if (lhs._functions && rhs._functions)
	{
		if (lhs._function == rhs.function)
			return true;
		else
			return *lhs._functions == *rhs._functions;
	}
}

#undef DELEGATE_TEMPLATE_PARAMS
#undef DELEGATE_TEMPLATE_ARGS
#undef DELEGATE_FUNCTION_PARAMS
#undef DELEGATE_FUNCTION_ARGS
#undef DELEGATE_COMMA

#endif //DELEGATE_COMMA