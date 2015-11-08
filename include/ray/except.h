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

_NAME_BEGIN

class EXPORT exception
{
public:
	exception(const error_code::int_type code) noexcept;
	exception(const error_code& code) noexcept;
	exception(const string& msg, const error_code& code) noexcept;
	exception(const string& msg, const string& stack, const error_code& code) noexcept;

	virtual ~exception() noexcept;

	const string& message() const noexcept;
	const string& stack() const noexcept;
	const string& what() const noexcept;

	const error_code& code() const noexcept;

private:

	void printStack() noexcept;

private:

	string _info;
    string _stack;
	string _message;

    error_code _code;
};

class EXPORT failure : public exception
{
public:
	failure(const string& _msg, const error_code& _code = make_error_code(error_code::UNKNOWN_ERROR)) noexcept;
	failure(const string& _msg, const string& _stack, const error_code& _code = make_error_code(error_code::UNKNOWN_ERROR)) noexcept;
	failure(const error_code& _code) noexcept;
	failure(const error_code::int_type& _code) noexcept;

	virtual ~failure() noexcept;
};

template <typename T>
inline T InvalidCall()
{
	throw failure(error_code::DIDNT_INHERIT_THE_FUNCTION);
}

template <>
inline void InvalidCall<void>()
{
}

_NAME_END

#endif
