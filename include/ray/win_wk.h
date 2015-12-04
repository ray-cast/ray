// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#ifndef _H_WIN_WK_H_
#define _H_WIN_WK_H_

#include <ray/win_wkbs.h>
#include <sstream>

_NAME_BEGIN

class StackWalkerInternal;
class EXPORT StackWalker : public WalkerBase
{
	friend StackWalkerInternal;
	typedef WalkerBase _Mybase;

	class CaptureContext
	{
	public:
		explicit CaptureContext(HANDLE thread) noexcept;
		~CaptureContext() noexcept;

		HANDLE thread() const noexcept;
		CONTEXT& context() noexcept;

	private:

		HANDLE _thread;

		CONTEXT _context;
	};

public:
	StackWalker(DWORD processid = 0, HANDLE process = 0, Options options = _Mybase::normal) noexcept;
	~StackWalker() noexcept;

	const util::string& printStack(HANDLE thread = 0) noexcept;

private:

	void printStack(const HANDLE hThread, CONTEXT& context) noexcept;
	void printSymbol(const util::string& searchPath, DWORD symOptions, const util::string& username) noexcept;
	void printModule(const util::string& img, const util::string& mod, DWORD64 baseAddr, DWORD size, DWORD result, const util::string& symType, const util::string& pdbName, ULONGLONG fileVersion) noexcept;
	void printEntry(CallStackEntry& entry) noexcept;
	void print(const util::string& text, ostringstream& ostream) noexcept;
	void error(const char* funcName, DWORD gle = NO_ERROR, DWORD64 addr = 0x0) noexcept;

	void clear() noexcept;
	const util::string& str() noexcept;

private:
	StackWalkerInternal* _internal;

	Options _options;

	util::string _message;

	ostringstream _module;
	ostringstream _entry;
	ostringstream _error;
};

_NAME_END

#endif