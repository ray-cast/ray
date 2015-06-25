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
#include <ray/win_int.h>
#include <ray/win_wk.h>
#include <ray/memory.h>

#include <TLHelp32.h>
#include <Psapi.h>
#include <iomanip>

_NAME_BEGIN

StackWalkerInternal::StackWalkerInternal(StackWalker* walker, HANDLE process) noexcept
    : _walker(walker)
    , _process(process)
    , _module_loaded(false)
{
    assert(nullptr != walker);

    _process = ((0 != process) ? process : ::GetCurrentProcess());
    _process_id = ::GetProcessId(process);
}

StackWalkerInternal::~StackWalkerInternal() noexcept
{
    if (0 != _process)
    {
        ::SymCleanup(_process);
    }
}

bool
StackWalkerInternal::loadSymbol(const std::string& filepath) noexcept
{
    assert(!filepath.empty());

    try
    {
        if (::SymInitialize(_process, (PCSTR)filepath.c_str(), false))
        {
            DWORD options = ::SymGetOptions();
            options |= SYMOPT_LOAD_LINES;
            options |= SYMOPT_FAIL_CRITICAL_ERRORS;
            options = ::SymSetOptions(options);

            TCHAR buf[STACKWALK_MAX_NAMELEN] = { 0 };

            if (::SymGetSearchPath(_process, buf, STACKWALK_MAX_NAMELEN))
            {
                TCHAR username[STACKWALK_MAX_NAMELEN] = { 0 };
                DWORD size = STACKWALK_MAX_NAMELEN;
                if (::GetUserName(username, &size))
                {
                    _walker->printSymbol(buf, options, username);
                    return true;
                }
                else
                {
                    _walker->error("GetUserName");
                    return false;
                }
            }
            else
            {
                _walker->error("SymGetSearchPath");
                return false;
            }
        }
        else
        {
            _walker->error("SymInitialize");
            return false;
        }
    }
    catch (...)
    {
        return false;
    }
}

bool
StackWalkerInternal::loadModules() noexcept
{
    try
    {
        if (_module_loaded) return true;

        std::string path;

        path += ".;";
        path += GetCurrentDirectory() + ";";
        path += GetModuleDirectory() + ";";
        path += GetEnvironmentVariable("_NT_SYMBOL_PATH") + ";";
        path += GetEnvironmentVariable("SYSTEMROOT") + ";";
        path += GetEnvironmentVariable("SYSTEMROOT") + "\\system32" + ";";

        std::string drive = GetEnvironmentVariable("SYSTEMDRIVE");
        if (drive.empty())
        {
            path += "SRV*c:\\websymbols*http://msdl.microsoft.com/download/symbols;";
        }
        else
        {
            path += "SRV*" + drive + "\\websymbols";
            path += "*http://msdl.microsoft.com/download/symbols;";
        }

        bool result = loadSymbol(path);
        if (!result)
        {
            ::SetLastError(ERROR_DLL_INIT_FAILED);
            _walker->error("while initializing StackWalkerInternal");
            return false;
        }

        _module_loaded = loadModules(_process, _process_id);

        return _module_loaded;
    }
    catch (...)
    {
        return false;
    }
}

bool
StackWalkerInternal::loadModulesFromTH32(HANDLE hprocess, DWORD pid) noexcept
{
    HANDLE hsnap = (HANDLE)-1;

    try
    {
        hsnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
        if (hsnap == (HANDLE)-1)
            return false;

        MODULEENTRY32 me = { 0 };
        me.dwSize = sizeof(me);

        std::size_t cnt = 0;
        if (::Module32First(hsnap, &me))
        {
            do
            {
                this->loadModule(hprocess, me.szExePath, me.szModule, (DWORD64)me.modBaseAddr, me.modBaseSize);

                cnt++;
            } while (::Module32Next(hsnap, &me));
        }

        ::CloseHandle(hsnap);

        return cnt ? true : false;
    }
    catch (...)
    {
        if (hsnap != (HANDLE)-1)
            ::CloseHandle(hsnap);

        return false;
    }
}

bool
StackWalkerInternal::loadModulesFromPSAPI(HANDLE hprocess) noexcept
{
    try
    {
        DWORD TTBUFLEN;

        DWORD needed = 200;

        std::unique_ptr<HMODULE[]> modules = std::make_unique<HMODULE[]>(needed);
        do
        {
            TTBUFLEN = needed;

            if (!::EnumProcessModules(hprocess, modules.get(), TTBUFLEN, &needed))
            {
                if (needed < TTBUFLEN)
                    return false;
            }
        } while (needed > TTBUFLEN);

        MODULEINFO mi;
        std::size_t cnt = 0;
        std::unique_ptr<TCHAR[]> tt = std::make_unique<TCHAR[]>(TTBUFLEN);
        std::unique_ptr<TCHAR[]> tt2 = std::make_unique<TCHAR[]>(TTBUFLEN);

        for (std::size_t i = 0; i < (needed / sizeof(modules[0])); i++, cnt++)
        {
            // base address, size
            ::GetModuleInformation(hprocess, modules[i], &mi, sizeof(mi));
            // image file name
            do
            {
                TTBUFLEN = needed;

                tt[0] = 0;

                needed = ::GetModuleFileNameEx(hprocess, modules[i], tt.get(), TTBUFLEN);
            } while (needed > TTBUFLEN);

            // module name
            do
            {
                TTBUFLEN = needed;

                tt2[0] = 0;

                needed = ::GetModuleBaseName(hprocess, modules[i], tt2.get(), TTBUFLEN);
            } while (needed > TTBUFLEN);

            if (this->loadModule(hprocess, tt.get(), tt2.get(), (DWORD64)mi.lpBaseOfDll, mi.SizeOfImage))
            {
                _walker->error("loadModule");
            }
        }

        return (cnt != 0) ? true : false;
    }
    catch (...)
    {
        return false;
    }
}

bool
StackWalkerInternal::loadModule(HANDLE process, const std::string& img, const std::string& mod, DWORD64 baseAddr, DWORD size) noexcept
{
    assert(0 != process);
    assert(0 != img.size());
    assert(0 != mod.size());

    try
    {
        std::string image = img;
        std::string modulename = mod;
        DWORD result = 0;

        if (!::SymLoadModuleEx(process, 0, image.data(), modulename.data(), baseAddr, size, 0, 0))
        {
            result = ::GetLastError();
        }

        ULONGLONG fileVersion = 0;

        // try to retrive the file-version:
        DWORD handle;
        DWORD size = ::GetFileVersionInfoSize(img.data(), &handle);
        if (size)
        {
            std::unique_ptr<CHAR[]> data = std::make_unique<CHAR[]>(size);

            if (::GetFileVersionInfo(img.data(), handle, size, data.get()))
            {
                UINT len;
                TCHAR bloack[] = "\\";

                VS_FIXEDFILEINFO* info = nullptr;
                if (::VerQueryValue(data.get(), bloack, (LPVOID*)&info, &len))
                {
                    fileVersion = ((ULONGLONG)info->dwFileVersionLS) + ((ULONGLONG)info->dwFileVersionMS << 32);
                }
            }
        }

        IMAGEHLP_MODULE64 module;
        std::string type = "-unknown-";
        if (this->getModuleInfo(baseAddr, module))
        {
            type = this->getSymTypeName(module.SymType);
        }

        _walker->printModule(img, mod, baseAddr, size, result, type, module.LoadedImageName, fileVersion);

        return (result == 0) ? true : false;
    }
    catch (...)
    {
        return false;
    }
}

bool
StackWalkerInternal::loadModules(HANDLE process, DWORD dwProcessId) noexcept
{
    // first try toolhelp32
    if (this->loadModulesFromTH32(process, dwProcessId))
        return true;

    // then try psapi
    return this->loadModulesFromPSAPI(process);
}

bool
StackWalkerInternal::isModuleLoaded() const noexcept
{
    return _module_loaded;
}

bool
StackWalkerInternal::firstStackFrame(PDWORD imageType, HANDLE thread, LPSTACKFRAME64 sf, PCONTEXT context) noexcept
{
    assert(imageType);
    assert(nullptr != thread);
    assert(nullptr != sf);
    assert(nullptr != context);

#ifdef _M_IX86
    // normally, call ImageNtHeader() and use machine info from PE header
    *imageType = IMAGE_FILE_MACHINE_I386;
    sf->AddrPC.Offset = context->Eip;
    sf->AddrPC.Mode = AddrModeFlat;
    sf->AddrFrame.Offset = context->Ebp;
    sf->AddrFrame.Mode = AddrModeFlat;
    sf->AddrStack.Offset = context->Esp;
    sf->AddrStack.Mode = AddrModeFlat;
#elif _M_X64
    *imageType = IMAGE_FILE_MACHINE_AMD64;
    sf->AddrPC.Offset = context->Rip;
    sf->AddrPC.Mode = AddrModeFlat;
    sf->AddrFrame.Offset = context->Rsp;
    sf->AddrFrame.Mode = AddrModeFlat;
    sf->AddrStack.Offset = context->Rsp;
    sf->AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
    *imageType = IMAGE_FILE_MACHINE_IA64;
    sf->AddrPC.Offset = context->StIIP;
    sf->AddrPC.Mode = AddrModeFlat;
    sf->AddrFrame.Offset = context->IntSp;
    sf->AddrFrame.Mode = AddrModeFlat;
    sf->AddrBStore.Offset = context->RsBSP;
    sf->AddrBStore.Mode = AddrModeFlat;
    sf->AddrStack.Offset = context->IntSp;
    sf->AddrStack.Mode = AddrModeFlat;
#else
    return false;
#endif

    return this->nextStackFrame(*imageType, thread, sf, context);
}

bool
StackWalkerInternal::nextStackFrame(DWORD imageType, HANDLE thread, LPSTACKFRAME64 StackFrame, PCONTEXT context) noexcept
{
    if (!::StackWalk64(imageType, _process, thread, StackFrame, context, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0))
    {
        if (::GetLastError() == NO_ERROR)
            return false;
        else
        {
            _walker->error("StackWalk64", ::GetLastError());
            return false;
        }
    }

    if (StackFrame->AddrPC.Offset == 0 ||
        StackFrame->AddrPC.Offset == StackFrame->AddrReturn.Offset)
    {
        _walker->error("StackWalk64-Endless-Callstack!", 0);
        return false;
    }

    return true;
}

bool
StackWalkerInternal::getModuleInfo(DWORD64 baseAddr, IMAGEHLP_MODULE64& module) noexcept
{
    assert(0 != baseAddr);

    module.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

    if (!::SymGetModuleInfo64(_process, baseAddr, &module))
    {
        _walker->error("SymFromAddr", ::GetLastError());
        return false;
    }

    return true;
}

bool
StackWalkerInternal::getSymFromAddr(DWORD64 address, PDWORD64 displacement, PSYMBOL_INFO symbol) noexcept
{
    assert(0 != address);
    assert(0 != symbol);

    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    if (::SymFromAddr(_process, address, displacement, symbol))
    {
        TCHAR temp[STACKWALK_MAX_NAMELEN];

        DWORD length = 0;
        
        length = ::UnDecorateSymbolName(symbol->Name, temp, STACKWALK_MAX_NAMELEN, UNDNAME_NAME_ONLY);
        if (length > 0)
        {
            std::strncpy(symbol->Name, temp, length);
        }

        length = ::UnDecorateSymbolName(symbol->Name, temp, STACKWALK_MAX_NAMELEN, UNDNAME_COMPLETE);
        if (length > 0)
        {
            std::strncpy(symbol->Name, temp, length);
        }

        return true;
    }
    else
    {
        _walker->error("SymFromAddr", ::GetLastError());
        return false;
    }
}

bool
StackWalkerInternal::getLineFromAddr(DWORD64 qwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINE64 Line64) noexcept
{
    assert(0 != qwAddr);
    assert(0 != Line64);

    Line64->SizeOfStruct = sizeof(line);

    if (!::SymGetLineFromAddr64(_process, qwAddr, pdwDisplacement, Line64))
    {
        if (::GetLastError() == ERROR_INVALID_ADDRESS ||
            ::GetLastError() == ERROR_NOACCESS)
        {
            Line64->LineNumber = 0;
            return true;
        }

        _walker->error("SymGetLineFromAddr64", ::GetLastError());
        return false;
    }

    return true;
}

BOOL CALL
StackWalkerInternal::EnumSymCallBack(PSYMBOL_INFO syminfo, ULONG symsize, PVOID data) noexcept
{
    if (syminfo->Tag == SymTagData)
    {
        VariableCall* userdata = (VariableCall*)data;

        CallStackVariable _variable;

        _variable.size = syminfo->Size;
        _variable.typeID = syminfo->TypeIndex;
        _variable.modBase = syminfo->ModBase;
        _variable.varName = syminfo->Name;
        _variable.typeName = userdata->self->getTypeName(syminfo);
        _variable.address = userdata->self->getTypeAddress(syminfo, userdata->context);
        _variable.valueName = userdata->self->getTypeValue(syminfo, userdata->context);

        userdata->variable->list.push_back(_variable);
    }

    return TRUE;
}

bool
StackWalkerInternal::getLocalVariable(const PCONTEXT context, VariableList& variable) noexcept
{
    IMAGEHLP_STACK_FRAME isf;

#ifdef __WIN64__
    isf.InstructionOffset = context->Rip;
#else
    isf.InstructionOffset = context->Eip;
#endif

    if (::SymSetContext(_process, &isf, 0))
    {
        VariableCall data;
        data.self = this;
        data.context = context;
        data.variable = &variable;

        ::SymEnumSymbols(_process, 0, 0, EnumSymCallBack, &data);

        return true;
    }

    return false;
}

std::string
StackWalkerInternal::getTypeName(PSYMBOL_INFO syminfo) noexcept
{
    assert(syminfo->Tag == SymTagData);

    return this->getTypeName(syminfo->TypeIndex, syminfo->ModBase);
}

std::string
StackWalkerInternal::getTypeName(DWORD typeID, DWORD64 modBase) noexcept
{
    DWORD typeTag;

    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_SYMTAG, &typeTag);

    switch (typeTag)
    {
    case SymTagBaseType:
        return this->getBaseTypeName(typeID, modBase);
    case SymTagPointerType:
        return this->getPointerTypeName(typeID, modBase);
    case SymTagArrayType:
        return this->getArrayTypeName(typeID, modBase);
    case SymTagUDT:
        return this->getUDTTypeName(typeID, modBase);
    case SymTagEnum:
        return this->getEnumTypeName(typeID, modBase);
    case SymTagFunctionType:
        return this->getFuncTypeName(typeID, modBase);
    default:
        return "??";
    }
}

std::string
StackWalkerInternal::getSymTypeName(SYM_TYPE type) noexcept
{
    switch (type)
    {
    case SYM_TYPE::SymNone:
        return "-nosymbols-";
        break;
    case SYM_TYPE::SymCoff:
        return "COFF";
        break;
    case SYM_TYPE::SymCv:
        return "CV";
        break;
    case SYM_TYPE::SymPdb:
        return "PDB";
        break;
    case SYM_TYPE::SymExport:
        return "-exported-";
        break;
    case SYM_TYPE::SymDeferred:
        return "-deferred-";
        break;
    case SYM_TYPE::SymSym:
        return "SYM";
        break;
    case SYM_TYPE::SymDia:
        return "DIA";
        break;
    case SYM_TYPE::SymVirtual:
        return "Virtual";
        break;
    default:
        return "-unknown-";
    }
}

StackWalkerInternal::SymBaseType
StackWalkerInternal::getBaseType(DWORD typeID, DWORD64 modBase) noexcept
{
    DWORD baseType;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_BASETYPE, &baseType);

    ULONG64 length;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_LENGTH, &length);

    switch (baseType)
    {
    case 1:
        return SymVoid;
    case 2:
        return SymChar;
    case 3:
        return SymWChar;
    case 6:
        if (length == 2)
            return SymShort;
        if (length == 4)
            return SymInt;
        if (length == 8)
            return SymLongLong;
    case 7:
        if (length == 1)
            return SymUChar;
        if (length == 2)
            return SymUShort;
        if (length == 4)
            return SymUInt;
        if (length == 8)
            return SymULongLong;
    case 8:
        if (length == 4)
            return SymFloat;
        if (length == 8)
            return SymDouble;
    case 10:
        return SymBool;
    case 13:
        return SymLong;
    case 14:
        return SymULong;
    default:
        return SymNone;
    }
}

std::string
StackWalkerInternal::getBaseTypeName(DWORD typeID, DWORD64 modBase) noexcept
{
    switch (this->getBaseType(typeID, modBase))
    {
    case SymVoid:
        return "void";
    case SymBool:
        return "bool";
    case SymChar:
        return "char";
    case SymWChar:
        return "wchar_t";
    case SymShort:
        return "short";
    case SymInt:
        return "int";
    case SymLong:
        return "long";
    case SymLongLong:
        return "long long";
    case SymUChar:
        return "unsigned char";
    case SymUShort:
        return "unsigned short";
    case SymUInt:
        return "unsigned int";
    case SymULong:
        return "unsigned long";
    case SymULongLong:
        return "unsigned long long";
    case SymFloat:
        return "float";
    case SymDouble:
        return "double";
    case SymEnd:
        return "";
    default:
        return "<no-type>";
    }
}

std::string
StackWalkerInternal::getPointerTypeName(DWORD typeID, DWORD64 modBase) noexcept
{
    BOOL isReference = false;

#if !defined (__GNUWIN__)
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_IS_REFERENCE, &isReference);
#endif

    DWORD innerTypeID;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_TYPEID, &innerTypeID);

    return this->getTypeName(innerTypeID, modBase) + (isReference == TRUE ? "&" : "*");
}

std::string
StackWalkerInternal::getArrayTypeName(DWORD typeID, DWORD64 modBase) noexcept
{
    DWORD innerTypeID;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_TYPEID, &innerTypeID);

    DWORD elemCount;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_COUNT, &elemCount);

    std::ostringstream strBuilder;

    strBuilder << this->getTypeName(innerTypeID, modBase) << "[" << elemCount << "]";

    return strBuilder.str();
}

std::string
StackWalkerInternal::getUDTTypeName(DWORD typeID, DWORD64 modBase) noexcept
{
    WCHAR* buf = nullptr;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_SYMNAME, &buf);
    if (buf)
    {
        char str[MAX_BUFFER_LENGTH];

        ::wcstombs(str, buf, MAX_BUFFER_LENGTH);

        return std::string(str);
    }

    return "??";
}

std::string
StackWalkerInternal::getEnumTypeName(DWORD typeID, DWORD64 modBase) noexcept
{
    WCHAR* buf = nullptr;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_SYMNAME, &buf);
    if (buf)
    {
        char str[MAX_BUFFER_LENGTH];

        ::wcstombs(str, buf, MAX_BUFFER_LENGTH);

        return std::string(str);
    }

    return "??";
}

std::string
StackWalkerInternal::getFuncTypeName(DWORD typeID, DWORD64 modBase) noexcept
{
    DWORD paramCount = 0;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_CHILDRENCOUNT, &paramCount);

    DWORD returnTypeID = 0;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_TYPEID, &returnTypeID);

    std::ostringstream name;
    name << this->getTypeName(returnTypeID, modBase);

    BYTE buf[sizeof(TI_FINDCHILDREN_PARAMS) + sizeof(ULONG)* MAX_BUFFER_LENGTH];
    TI_FINDCHILDREN_PARAMS* findParams = (TI_FINDCHILDREN_PARAMS*)buf;
    findParams->Count = paramCount;
    findParams->Start = 0;

    ::SymGetTypeInfo(_process, modBase, typeID, TI_FINDCHILDREN, findParams);

    name << '(';

    for (std::size_t i = 0; i != paramCount; i++)
    {
        DWORD paramTypeID;
        ::SymGetTypeInfo(_process, modBase, findParams->ChildId[i], TI_GET_TYPEID, &paramTypeID);

        if (i != 0)
        {
            name << (", ");
        }

        name << getTypeName(paramTypeID, modBase);
    }

    name << ')';

    return name.str();
}

BOOL
StackWalkerInternal::isSimpleType(DWORD typeID, DWORD64 modBase) noexcept
{
    DWORD symTag;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_SYMTAG, &symTag);

    switch (symTag)
    {
    case SymTagBaseType:
    case SymTagPointerType:
    case SymTagEnum:
        return TRUE;
    default:
        return FALSE;
    }
}

BOOL
StackWalkerInternal::getDataMemberInfo(DWORD memberID, DWORD64 modBase, DWORD64 address, const BYTE* pData, std::ostringstream& stream) noexcept
{
    DWORD memberTag;
    SymGetTypeInfo(_process, modBase, memberID, TI_GET_SYMTAG, &memberTag);

    if (memberTag != SymTagData && memberTag != SymTagBaseClass)
    {
        return FALSE;
    }

    DWORD memberTypeID;
    SymGetTypeInfo(_process, modBase, memberID, TI_GET_TYPEID, &memberTypeID);

    stream << this->getTypeName(memberTypeID, modBase);

    if (memberTag == SymTagData)
    {
        WCHAR* name;
        SymGetTypeInfo(_process, modBase, memberID, TI_GET_SYMNAME, &name);

        char str[MAX_BUFFER_LENGTH];

        ::wcstombs(str, name, MAX_BUFFER_LENGTH);

        stream << "  " << std::string(str);
    }
    else
    {
        stream << "  <base-class>";
    }

    ULONG64 length;
    SymGetTypeInfo(_process, modBase, memberTypeID, TI_GET_LENGTH, &length);

    stream << "  " << length;

    DWORD offset;
    SymGetTypeInfo(_process, modBase, memberID, TI_GET_OFFSET, &offset);

    DWORD64 childAddress = address + offset;

    stream << " 0x" << std::hex << std::uppercase << childAddress;

    if (this->isSimpleType(memberTypeID, modBase) == TRUE)
    {
        stream << "  " << this->getTypeValue(memberTypeID, modBase, childAddress, pData + offset);
    }

    return TRUE;
}

DWORD64
StackWalkerInternal::getTypeAddress(PSYMBOL_INFO syminfo, const PCONTEXT context) noexcept
{
    DWORD64 address = 0;

    if ((syminfo->Flags & SYMFLAG_REGREL) == 0)
    {
        address = syminfo->Address;
    }
    else
    {
#ifdef __WIN64__
        address = context->Rsp + syminfo->Address;
#else
        DWORD64 displacement;

        SYMBOL_INFO symbol;
        this->getSymFromAddr(context->Eip, &displacement, &symbol);
        if (displacement == 0)
        {
            address = context->Esp - 4 + syminfo->Address;
        }
        else
        {
            address = context->Ebp + syminfo->Address;
        }
#endif
    }

    return address;
}

std::string
StackWalkerInternal::getTypeValue(PSYMBOL_INFO syminfo, PCONTEXT context) noexcept
{
    return this->getTypeValue(syminfo->TypeIndex, syminfo->ModBase, this->getTypeAddress(syminfo, context), syminfo->Size);
}

std::string
StackWalkerInternal::getTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, DWORD size) noexcept
{
    std::unique_ptr<BYTE[]> data = std::make_unique<BYTE[]>(size);

    SIZE_T bytesRead = 0;
    ::ReadProcessMemory(_process, (LPVOID)address, data.get(), size, &bytesRead);

    return this->getTypeValue(typeID, modBase, address, data.get());
}

std::string
StackWalkerInternal::getTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, const BYTE* pData) noexcept
{
    DWORD typeTag = SymTagNull;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_SYMTAG, &typeTag);

    switch (typeTag)
    {
    case SymTagBaseType:
        return this->getBaseTypeValue(typeID, modBase, address, pData);
    case SymTagPointerType:
        return this->getPointerTypeValue(typeID, modBase, address, pData);
    case SymTagEnum:
        return this->getEnumTypeValue(typeID, modBase, address, pData);
    case SymTagArrayType:
        return this->getArrayTypeValue(typeID, modBase, address, pData);
    case SymTagUDT:
        return this->getUDTTypeValue(typeID, modBase, address, pData);
    case SymTagTypedef:
        DWORD actTypeID;
        ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_TYPEID, &actTypeID);

        return this->getTypeValue(actTypeID, modBase, address, pData);
    default:
        return "??";
    }
}

std::string
StackWalkerInternal::getBaseTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, const BYTE* pData) noexcept
{
    std::ostringstream value;

    switch (this->getBaseType(typeID, modBase))
    {
    case SymVoid:
        value << "void";
        break;
    case SymBool:
        value << (*pData == 0 ? "false" : "true");
        break;
    case SymChar:
        value << (int)(*((char*)pData));
        break;
    case SymUChar:
        value << (int)(*((unsigned char*)pData));
        break;
    case SymWChar:
        value << *((wchar_t*)pData);
        break;
    case SymShort:
        value << *((short*)pData);
        break;
    case SymUShort:
        value << *((unsigned short*)pData);
        break;
    case SymInt:
        value << *((int*)pData);
        break;
    case SymUInt:
        value << *((unsigned int*)pData);
        break;
    case SymLong:
        value << *((long*)pData);
        break;
    case SymULong:
        value << *((unsigned long*)pData);
        break;
    case SymLongLong:
        value << *((long long*)pData);
        break;
    case SymULongLong:
        value << *((unsigned long long*)pData);
        break;
    case SymFloat:
        value << *((float*)pData);
        break;
    case SymDouble:
        value << *((double*)pData);
        break;
    case SymEnd:
    default:
        break;
    }

    return value.str();
}

std::string
StackWalkerInternal::getPointerTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, const BYTE* pData) noexcept
{
    ULONG64 length;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_LENGTH, &length);

    std::ostringstream value;

    std::string str = this->getPointerTypeName(typeID, modBase);
    if (str == "char*" || str == "unsigned char*")
    {
        char* array = (char*)(*(std::intptr_t*)pData);

        char* text = array;

        std::size_t length = 0;

        while (*text >= 0x20 && *text <= 0x7e)
        {
            length++;
            text++;
        }

        if (length > 0)
        {
            value << R"(")" << std::string(array, length) << R"(")";
        }
    }
    else if (str == "char**" || str == "unsigned char**")
    {
        char** array = (char**)(*(std::intptr_t*)pData);

        char* text = array[0];

        std::size_t length = 0;

        while (*text >= 0x20 && *text <= 0x7e)
        {
            length++;
            text++;
        }

        if (length > 0)
        {
            value << R"(")" << std::string(array[0], length) << R"(")";
        }
    }
    else
    {
        switch (length)
        {
        case 0:
            value << TEXT("??");
            break;
        case 4:
            value << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << *((DWORD*)pData);
            break;
        case 8:
            value << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << *((DWORD64*)pData);
            break;
        }
    }

    return value.str();
}

std::string
StackWalkerInternal::getArrayTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, const BYTE* pData) noexcept
{
    DWORD elemCount;
    SymGetTypeInfo(_process, modBase, typeID, TI_GET_COUNT, &elemCount);

    elemCount = elemCount > 1024 ? 1024 : elemCount;

    DWORD innerTypeID;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_TYPEID, &innerTypeID);

    ULONG64 elemLen;
    ::SymGetTypeInfo(_process, modBase, innerTypeID, TI_GET_LENGTH, &elemLen);

    std::ostringstream value;

    for (DWORD index = 0; index != elemCount; ++index)
    {
        DWORD elemOffset = DWORD(index * elemLen);

        value << TEXT("  [") << index << TEXT("]  ") << this->getTypeValue(innerTypeID, modBase, address + elemOffset, pData + index * elemLen);

        if (index != elemCount - 1)
        {
            value << std::endl;
        }
    }

    return value.str();
}

std::string
StackWalkerInternal::getUDTTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, const BYTE* pData) noexcept
{
    /*
    std::ostringstream value;

    DWORD memberCount;
    ::SymGetTypeInfo(_process, modBase, typeID, TI_GET_CHILDRENCOUNT, &memberCount);

    TI_FINDCHILDREN_PARAMS* pFindParams = (TI_FINDCHILDREN_PARAMS*)malloc(sizeof(TI_FINDCHILDREN_PARAMS)+memberCount * sizeof(ULONG));
    pFindParams->Start = 0;
    pFindParams->Count = memberCount;

    ::SymGetTypeInfo(_process, modBase, typeID, TI_FINDCHILDREN, pFindParams);

    for (int index = 0; index != memberCount; ++index)
    {
        if (getDataMemberInfo(pFindParams->ChildId[index], modBase, address, pData, value))
        {
            value << std::endl;
        }
    }

    return value.str();*/
    std::ostringstream value;

    value << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << address;

    return value.str();
}

std::string
StackWalkerInternal::getEnumTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, const BYTE* pData) noexcept
{
    std::string valueName;

    DWORD baseType;
    SymGetTypeInfo(_process, modBase, typeID, TI_GET_BASETYPE, &baseType);

    DWORD childrenCount;
    SymGetTypeInfo(_process, modBase, typeID, TI_GET_CHILDRENCOUNT, &childrenCount);

    TI_FINDCHILDREN_PARAMS* pFindParams = (TI_FINDCHILDREN_PARAMS*)malloc(sizeof(TI_FINDCHILDREN_PARAMS) + childrenCount * sizeof(ULONG));
    pFindParams->Start = 0;
    pFindParams->Count = childrenCount;

    SymGetTypeInfo(_process, modBase, typeID, TI_FINDCHILDREN, pFindParams);

    for (DWORD index = 0; index != childrenCount; ++index)
    {
        VARIANT enumValue;
        SymGetTypeInfo(_process, modBase, pFindParams->ChildId[index], TI_GET_VALUE, &enumValue);

        if (VariantEqual(enumValue, baseType, pData) == TRUE) {
            //获取枚举值的名称
            WCHAR* pBuffer;
            SymGetTypeInfo(_process, modBase, pFindParams->ChildId[index], TI_GET_SYMNAME, &pBuffer);

            char str[MAX_BUFFER_LENGTH];

            ::wcstombs(str, pBuffer, MAX_BUFFER_LENGTH);

            valueName = str;

            break;
        }
    }

    free(pFindParams);

    //如果没有找到对应的枚举值，则显示它的基本类型值
    if (valueName.length() == 0)
    {
        valueName = this->getBaseTypeValue(typeID, modBase, address, pData);
    }

    return valueName;
}

BOOL
StackWalkerInternal::VariantEqual(VARIANT var, DWORD baseType, const BYTE* pData) noexcept
{
    switch (baseType)
    {
    case SymChar:
        return var.cVal == *((char*)pData);

    case SymUChar:
        return var.bVal == *((unsigned char*)pData);

    case SymShort:
        return var.iVal == *((short*)pData);

    case SymWChar:
    case SymUShort:
        return var.uiVal == *((unsigned short*)pData);

    case SymUInt:
        return var.uintVal == *((unsigned int*)pData);

    case SymLong:
        return var.lVal == *((long*)pData);

    case SymULong:
        return var.ulVal == *((unsigned long*)pData);

    case SymLongLong:
        return var.llVal == *((long long*)pData);

    case SymULongLong:
        return var.ullVal == *((unsigned long long*)pData);

    case SymInt:
    default:
        return var.intVal == *((int*)pData);
    }
}

_NAME_END