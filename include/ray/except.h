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
#ifndef _H_EXCEPT_H_
#define _H_EXCEPT_H_

#include <ray/err.h>

#if defined(_LINUX_)
#include <execinfo.h>
#elif defined(_WINDOWS_)
#include <ray/win_wk.h>
#endif

#include <sstream>

_NAME_BEGIN

class runtime_error : public std::exception
{
public:
    runtime_error(const error_code::int_type code) noexcept
        : _message()
        , _code(code)
    {
    }

    runtime_error(const error_code& code) noexcept
        : _message()
        , _code(code)
    {
    }

    runtime_error(const std::string& msg, const error_code& code) noexcept
        : _message(msg)
        , _code(code)
    {
        this->printStack();
    }

    runtime_error(const std::string& msg, const std::string& stack, const error_code& code) noexcept
        : _message(msg)
        , _stack(stack)
        , _code(code)
    {
    }

    virtual ~runtime_error() noexcept
    {
    }

    void printStack() noexcept
    {
#if defined(_WINDOWS_)
        StackWalker stack;
        _stack = stack.printStack();
#endif
    }

    const std::string& message() const noexcept
    {
        return _message;
    }

    const std::string& stack() const noexcept
    {
        return _stack;
    }

    const error_code& code() const noexcept
    {
        return (this->_code);
    }

    const char* what() const noexcept
    {
        _summary.append("[MESSAGE]: \n");
        _summary.append(_message);
        _summary.append("\n");
        _summary.append("[STACK INFO]: \n");
        _summary.append(_stack);
        _summary.append("\n");

        return _summary.c_str();
    }

private:

    std::string _message;
    std::string _stack;

    mutable std::string _summary;

    error_code _code;
};

template <typename T>
inline T InvalidCall()
{
    throw runtime_error(error_code::DIDNT_INHERIT_THE_FUNCTION);
}

template <>
inline void InvalidCall<void>()
{
}

class failure : public runtime_error
{
public:
    explicit failure(const std::string& _msg, const error_code& _code = make_error_code(error_code::UNKNOWN_ERROR)) noexcept
        : runtime_error(_msg, _code)
    {
    }

    explicit failure(const std::string& _msg, const std::string& _stack, const error_code& _code = make_error_code(error_code::UNKNOWN_ERROR)) noexcept
        : runtime_error(_msg, _stack, _code)
    {
    }

    explicit failure(const char* _msg, const error_code& _code = make_error_code(error_code::UNKNOWN_ERROR)) noexcept
        : runtime_error(_msg, _code)
    {
    }

    explicit failure(const error_code& _code) noexcept
        : runtime_error(_code)
    {
    }

    explicit failure(const error_code::int_type& _code) noexcept
        : runtime_error(_code)
    {
    }

    virtual ~failure() noexcept
    {
    }
};

_NAME_END

#endif
