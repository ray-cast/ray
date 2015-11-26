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
private:
    class IICALLBACK
    {
    public:
        virtual ~IICALLBACK() noexcept {}

        virtual Result invoke(DELEGATE_FUNCTION_PARAMS) const = 0;
        virtual std::unique_ptr<IICALLBACK> clone() const = 0;
    };

    template<typename T>
    class ICALLBACK : public IICALLBACK
    {
    public:
        ICALLBACK(const T& f) noexcept : _functor(f) {}
        ICALLBACK(const ICALLBACK& copy) noexcept : _functor(copy._functor) {}

        template<typename Function>
        static Result invoke(const Function& Func DELEGATE_COMMA DELEGATE_FUNCTION_PARAMS)
        {
            return (const_cast<Function&>(Func))(DELEGATE_FUNCTION_ARGS);
        }

        template<typename This, typename Function>
        static Result invoke(const std::pair<This, Function>& mf DELEGATE_COMMA DELEGATE_FUNCTION_PARAMS)
        {
            return ((*mf.first).*mf.second)(DELEGATE_FUNCTION_ARGS);
        }

        virtual Result invoke(DELEGATE_FUNCTION_PARAMS) const
        {
            return invoke(_functor DELEGATE_COMMA DELEGATE_FUNCTION_ARGS);
        }

        virtual std::unique_ptr<IICALLBACK> clone() const
        {
            return std::make_unique<ICALLBACK<T>>(_functor);
        }

        T _functor;
    };

public:
    typedef delegate<Result(DELEGATE_TEMPLATE_ARGS)> _Myt;

	typedef typename std::unique_ptr<IICALLBACK> bind;
    typedef std::vector<std::unique_ptr<IICALLBACK>> DELEGATES;
    typedef typename DELEGATES::iterator iterator;
    typedef typename DELEGATES::const_iterator const_iterator;
    typedef typename DELEGATES::size_type size_type;

public:
    delegate() noexcept
        : _functions(nullptr)
    {
    }

    template<typename _Functor>
    delegate(const _Functor& t1) noexcept
        : _functions(nullptr)
    {
        this->attach(t1);
    }

    template<typename _This, typename _Functor>
    delegate(_This t1, _Functor t2) noexcept
        :_functions(nullptr)
    {
        this->attach(t1, t2);
    }

    delegate(const _Myt& copy) noexcept
        : _functions(nullptr)
    {
        this->assign(copy);
    }

    ~delegate() noexcept
    {
        clear();
        delete _functions;
    }

    Result operator()(DELEGATE_FUNCTION_PARAMS)
    {
        return run(DELEGATE_FUNCTION_ARGS);
    }

    template<typename T>
    _Myt& operator+=(const T& t1)
    {
        this->attach(t1);
        return *this;
    }

    template<typename T>
    _Myt& operator+=(const _Myt& other)
    {
        this->attach(other);
        return *this;
    }

    template<typename T>
    _Myt& operator-=(const T& t1)
    {
        this->remove(t1);
        return *this;
    }

    _Myt& operator=(const _Myt& copy)
    {
        this->assign(copy);
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
                (*it)->invoke(DELEGATE_FUNCTION_ARGS);

            return (*it)->invoke(DELEGATE_FUNCTION_ARGS);
        }
    };

    template<typename S>
    struct Partial<void, S>
    {
        static void run(const _Myt& self DELEGATE_COMMA DELEGATE_FUNCTION_PARAMS)
        {
            for (auto& it : *self._functions)
                it->invoke(DELEGATE_FUNCTION_ARGS);
        }
    };
public:
    template<typename _Function>
    void attach(_Function t1)
    {
        if (!_functions)
            _functions = new DELEGATES;

        _functions->push_back(new ICALLBACK<_Function>(t1));
    }

    template<typename _This, typename _Functor>
    void attach(const _This& t1, const _Functor& t2)
    {
        this->attach(std::make_pair(t1, t2));
    }

    template<typename _This, typename _Functor>
    void attach(const std::pair<_This, _Functor>& pair)
    {
        if (!_functions)
            _functions = new DELEGATES;

        _functions->push_back(std::make_unique<ICALLBACK<std::pair<_This, _Functor>>>(pair));
    }

	void attach(typename _Myt::bind& callback)
	{
		assert(callback);

		if (!_functions)
			_functions = new DELEGATES;

		_functions->push_back(std::move(callback));
	}

    void attach(const _Myt& other)
    {
		if (other._functions)
		{
			if (!_functions)
				_functions = new DELEGATES;

			for (auto& it : *other._functions)
				_functions->push_back(it->clone());
		}
    }

    template<typename T>
    void assign(const T& t1)
    {
        this->clear();
        this->attach(t1);
    }

    template<typename _This, typename _Functor>
    void assign(const _This& t1, const _Functor& t2)
    {
        this->clear();
        this->attach(std::make_pair(t1, t2));
    }

    template<typename _This, typename _Functor>
    void assign(const std::pair<_This, _Functor>& pair)
    {
        this->clear();
        this->attach(pair);
    }

    void assign(const _Myt& copy)
    {
        assert(this != &copy);
        this->clear();
        this->attach(copy);
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

    template<typename T>
    void remove(const T& t1)
    {
        if (!_functions) { return; }

        const_iterator it = _functions->begin();
        const_iterator end = _functions->end();

        for (; it != end; ++it)
        {
            ICALLBACK<T>* p = dynamic_cast<ICALLBACK<T>*>(*it);

            if (p->_functor == t1)
            {
                delete p;

                _functions->erase(it);

                break;
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

	template<typename _This, typename _Functor>
	static typename _Myt::bind make(const _This& t1, const _Functor& t2)
	{
		return std::make_unique<ICALLBACK<std::pair<_This, _Functor>>>(std::make_pair(t1, t2));
	}

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