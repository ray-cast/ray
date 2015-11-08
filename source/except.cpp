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
#include <ray/except.h>

#if defined(_BUILD_PLATFORM_WINDOWS)
#	include <ray/win_wk.h>
#endif

_NAME_BEGIN

exception::exception(const error_code::int_type code) noexcept
	: _message()
	, _code(code)
{
	this->printStack();
}

exception::exception(const error_code& code) noexcept
	: _message()
	, _code(code)
{
	this->printStack();
}

exception::exception(const string& msg, const error_code& code) noexcept
	: _message(msg)
	, _code(code)
{
	this->printStack();
}

exception::exception(const string& msg, const string& stack, const error_code& code) noexcept
	: _message(msg)
	, _stack(stack)
	, _code(code)
{
	this->printStack();
}

exception::~exception() noexcept
{
}

void 
exception::printStack() noexcept
{
#ifdef _BUILD_PLATFORM_WINDOWS
	StackWalker stack;
	_stack = stack.printStack();
#endif

	_info.append(__TEXT("[MESSAGE]: \n"));
	_info.append(_message);
	_info.append(__TEXT("\n"));
	_info.append(__TEXT("[STACK INFO]: \n"));
	_info.append(_stack);
	_info.append(__TEXT("\n"));
}

const string& 
exception::message() const noexcept
{
	return _message;
}

const string& 
exception::stack() const noexcept
{
	return _stack;
}

const error_code& 
exception::code() const noexcept
{
	return (this->_code);
}

const string&
exception::what() const noexcept
{
	return _info;
}

failure::failure(const string& _msg, const error_code& _code) noexcept
	: exception(_msg, _code)
{
}

failure::failure(const string& _msg, const string& _stack, const error_code& _code) noexcept
	: exception(_msg, _stack, _code)
{
}

failure::failure(const error_code& _code) noexcept
	: exception(_code)
{
}

failure::failure(const error_code::int_type& _code) noexcept
	: exception(_code)
{
}

failure::~failure() noexcept
{
}

_NAME_END