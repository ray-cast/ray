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
#include <ray/except.h>

#if defined(_BUILD_PLATFORM_WINDOWS)
#	include <ray/win_wk.h>
#endif

_NAME_BEGIN

exception::exception() noexcept
{
}

exception::~exception() noexcept
{
}

failure::failure(const error_code& code) noexcept
	: _code(code)
{
	this->printStack();
}

failure::failure(const error_code::int_type& code) noexcept
	: _code(code)
{
	this->printStack();
}

failure::failure(const char* msg, const error_code& code) noexcept
	: _message(msg)
	, _code(code)
{
	this->printStack();
}

failure::failure(const char* msg, const char* stack, const error_code& code) noexcept
	: _message(msg)
	, _stack(stack)
	, _code(code)
{
}

failure::failure(const util::string& msg, const error_code& code) noexcept
	: _message(msg)
	, _code(code)
{
	this->printStack();
}

failure::failure(const util::string& msg, const util::string& stack, const error_code& code) noexcept
	: _message(msg)
	, _stack(stack)
	, _code(code)
{
}

failure::failure(failure&& move) noexcept
	: _message(std::move(move._message))
	, _stack(std::move(move._stack))
	, _code(std::move(move._code))
	, _info(std::move(move._info))
{
}

failure::~failure() noexcept
{
}

void
failure::printStack() noexcept
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

const char*
failure::message() const noexcept
{
	return _message.c_str();
}

const char*
failure::stack() const noexcept
{
	return _stack.c_str();
}

const char*
failure::what() const noexcept
{
	return _info.c_str();
}

const error_code&
failure::code() const noexcept
{
	return (_code);
}

_NAME_END