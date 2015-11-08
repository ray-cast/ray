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
#ifdef _BUILD_PLATFORM_WINDOWS
#include <ray/win_wk.h>
#include <ray/win_int.h>
#include <ray/format.h>
#include <ray/memory.h>

#include <new>
#include <iomanip>

_NAME_BEGIN

#undef new

StackWalker::CaptureContext::CaptureContext(HANDLE thread) noexcept
	: _thread(::GetCurrentThread())
{
	if (thread == 0)
	{
		thread = ::GetCurrentThread();
	}

	_context = { 0 };
	_context.ContextFlags = CONTEXT_ALL;

	if (thread == _thread)
	{
		::RtlCaptureContext(&_context);
	}
	else
	{
		::SuspendThread(_thread);
		::GetThreadContext(_thread, &_context);
	}

	_thread = thread;
}

StackWalker::CaptureContext::~CaptureContext() noexcept
{
	if (_thread != ::GetCurrentThread())
	{
		::ResumeThread(_thread);
	}
}

HANDLE
StackWalker::CaptureContext::thread() const noexcept
{
	return _thread;
}

CONTEXT&
StackWalker::CaptureContext::context() noexcept
{
	return _context;
}

StackWalker::StackWalker(DWORD processid, HANDLE process, Options options) noexcept
	: _internal(nullptr)
	, _options(options)
{
	_internal = new(std::nothrow) StackWalkerInternal(this, process);
	if (_internal)
	{
		_internal->loadModules();
	}
}

StackWalker::~StackWalker() noexcept
{
	delete _internal;
}

const string&
StackWalker::printStack(HANDLE thread) noexcept
{
	try
	{
		if (_internal->isModuleLoaded())
		{
			CaptureContext capture(thread);

			this->clear();

			this->printStack(capture.thread(), capture.context());
		}

		return this->str();
	}
	catch (...)
	{
		return this->str();
	}
}

void
StackWalker::printStack(const HANDLE thread, CONTEXT& context) noexcept
{
	assert(0 != thread);

	DWORD imageType;
	STACKFRAME64 sf;

	BYTE symbolBuf[sizeof(SYMBOL_INFO) + STACKWALK_MAX_NAMELEN];
	PSYMBOLINFO symbol = (PSYMBOLINFO)symbolBuf;
	symbol->MaxNameLen = STACKWALK_MAX_NAMELEN;

	if (_internal->firstStackFrame(&imageType, thread, &sf, &context))
	{
		do
		{
			CallStackEntry entry;

			entry.offset = sf.AddrPC.Offset;

			if (!_internal->getSymFromAddr(sf.AddrPC.Offset, &entry.offsetFromSmybol, symbol))
				return;

			IMAGEHLPLINE line;
			if (!_internal->getLineFromAddr(sf.AddrPC.Offset, &entry.offsetFromLine, line))
				return;

			IMAGEHLPMODULE module;
			if (!_internal->getModuleInfo(sf.AddrPC.Offset, module))
				return;

			VariableList _variable;
			if (!_internal->getLocalVariable(&context, _variable))
				return;

			entry.name = symbol->Name;
			entry.lineNumber = line.LineNumber;
			entry.lineFileName = line.FileName;
			entry.moduleName = module.ModuleName;
			entry.imageName = module.ImageName;
			entry.baseOfImage = module.BaseOfImage;
			entry.loadedImageName = module.LoadedImageName;
			entry.variable = _variable;

			this->printEntry(entry);
		} while (_internal->nextStackFrame(imageType, thread, &sf, &context));
	}
}

void
StackWalker::printModule(const string& img, const string& mod, DWORD64 baseAddr, DWORD size, DWORD result, const string& symType, const string& pdbName, ULONGLONG fileVersion) noexcept
{
	if (_options & _Mybase::module_info)
	{
		if (_options & _Mybase::file_version)
		{
			DWORD v4 = (DWORD)(fileVersion)& 0xFFFF;
			DWORD v3 = (DWORD)(fileVersion >> 16) & 0xFFFF;
			DWORD v2 = (DWORD)(fileVersion >> 32) & 0xFFFF;
			DWORD v1 = (DWORD)(fileVersion >> 48) & 0xFFFF;
			this->print(tformat(__TEXT("%s:%s (%p), size: %d (result: %d), SymType: '%s', PDB: '%s', fileVersion: %d.%d.%d.%d")) % img % mod % baseAddr % size % result % symType % pdbName % v1 % v2 % v3 % v4, _module);
		}
		else
		{
			this->print(tformat(__TEXT("%s:%s (%p), size: %d (result: %d), SymType: '%s', PDB: '%s'")) % img % mod % baseAddr % size % result % symType % pdbName, _module);
		}
	}
}

void
StackWalker::printSymbol(const string& searchPath, DWORD symOptions, const string& username) noexcept
{
	if (_options & _Mybase::symbol)
	{
		this->print(tformat(__TEXT("SymInit: Symbol-SearchPath: '%s', symOptions: %d, UserName: '%s'")) % searchPath % symOptions % username, _module);

		OSVERSIONINFOEX ver = { 0 };
		ver.dwOSVersionInfoSize = sizeof(ver);

		DWORDLONG conditionMask = 0;

		VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
		VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
		VER_SET_CONDITION(conditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

		if (::VerifyVersionInfo(&ver, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, conditionMask))
		{
			this->print(tformat(__TEXT("OS-Version: %d.%d.%d (%s) 0x%x-0x%x")) % ver.dwMajorVersion % ver.dwMinorVersion % ver.dwBuildNumber % ver.szCSDVersion % ver.wSuiteMask % ver.wProductType, _module);
		}
	}
}

void
StackWalker::printEntry(CallStackEntry& entry) noexcept
{
	if (_options & _Mybase::line)
	{
		if (entry.name.empty())
			entry.name = __TEXT("(function-name not available)");

		if (entry.lineFileName.empty())
			entry.lineFileName = __TEXT("(filename not available)");

		if (entry.moduleName.empty())
			entry.moduleName = __TEXT("(module-name not available)");

		/*for (auto& it : entry.variable.list)
		{
			this->print(format("%s %s = %s") % it.typeName % it.varName % it.valueName, _entry);
		}*/

		if (entry.lineNumber == 0)
		{
			this->print(tformat(__TEXT("%P %s: %s!%s")) % entry.offset % entry.lineFileName % entry.moduleName % entry.name, _entry);
		}
		else
		{
			if (entry.moduleName.empty())
			{
				this->print(tformat(__TEXT("%s (%d): %s")) % entry.lineFileName % entry.lineNumber % entry.name, _entry);
			}
			else
			{
				this->print(tformat(__TEXT("%P %s (%d): %s!%s")) % entry.offset % entry.lineFileName % entry.lineNumber % entry.moduleName % entry.name, _entry);
			}
		}
	}
}

void
StackWalker::error(const char* funcName, DWORD gle, DWORD64 addr) noexcept
{
	this->print(tformat(__TEXT("%s, GetLastError: %d (Address: %p)")) % funcName % gle % addr, _error);
}

void
StackWalker::print(const string& text, ostringstream& ostream) noexcept
{
	ostream << text << std::endl;
}

void
StackWalker::clear() noexcept
{
	ostringstream null;

	_entry.copyfmt(null);
}

const string&
StackWalker::str() noexcept
{
	if (_error.str().empty())
	{
		return (_message = _module.str() + _entry.str());
	}
	else
	{
		return (_message = _error.str());
	}
}

_NAME_END

#endif