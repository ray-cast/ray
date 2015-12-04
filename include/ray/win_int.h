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
#ifndef _H_WIN_INTERNAL_H_
#define _H_WIN_INTERNAL_H_

#include <ray/win_wkbs.h>

#ifndef _NO_CVCONST_H
#   define _NO_CVCONST_H
#   include <DbgHelp.h>
#else
#   include <DbgHelp.h>
#endif

_NAME_BEGIN

#if __UNICODE__
typedef IMAGEHLP_LINEW64 IMAGEHLPLINE;
typedef IMAGEHLP_MODULEW64 IMAGEHLPMODULE;
typedef PSYMBOL_INFOW PSYMBOLINFO;
#else
typedef IMAGEHLP_LINE64 IMAGEHLPLINE;
typedef IMAGEHLP_MODULE64 IMAGEHLPMODULE;
typedef PSYMBOL_INFO PSYMBOLINFO;
#endif

class StackWalker;
class EXPORT StackWalkerInternal : public WalkerBase
{
private:
    enum SymBaseType
    {
        SymNone,
        SymVoid,
        SymChar,
        SymWChar,
        SymInt,
        SymUInt,
        SymFloat,
        SymLong,
        SymBool,
        SymLongLong,
        SymULongLong,
        SymULong,
        SymUChar,
        SymDouble,
        SymShort,
        SymUShort,
        SymEnd,
    };

    enum SymTagEnum
    {
        SymTagNull,
        SymTagExe,
        SymTagCompiland,
        SymTagCompilandDetails,
        SymTagCompilandEnv,
        SymTagFunction,
        SymTagBlock,
        SymTagData,
        SymTagAnnotation,
        SymTagLabel,
        SymTagPublicSymbol,
        SymTagUDT,
        SymTagEnum,
        SymTagFunctionType,
        SymTagPointerType,
        SymTagArrayType,
        SymTagBaseType,
        SymTagTypedef,
        SymTagBaseClass,
        SymTagFriend,
        SymTagFunctionArgType,
        SymTagFuncDebugStart,
        SymTagFuncDebugEnd,
        SymTagUsingNamespace,
        SymTagVTableShape,
        SymTagVTable,
        SymTagCustom,
        SymTagThunk,
        SymTagCustomType,
        SymTagManagedType,
        SymTagDimension,
        SymTagCallSite,
        SymTagMax
    };

    struct VariableCall
    {
        StackWalkerInternal* self;
        PCONTEXT context;
        VariableList* variable;
    };

public:
    typedef WalkerBase _Mybase;

    StackWalkerInternal(StackWalker* walker, HANDLE process) noexcept;
    ~StackWalkerInternal() noexcept;

    bool loadModules() noexcept;
    bool loadModules(HANDLE process, DWORD dwProcessId) noexcept;
    bool loadModule(HANDLE process, const util::string& img, const util::string& mod, DWORD64 baseAddr, DWORD size) noexcept;

    bool firstStackFrame(PDWORD imageType, HANDLE thread, LPSTACKFRAME64 StackFrame, PCONTEXT context) noexcept;
    bool nextStackFrame(DWORD imageType, HANDLE thread, LPSTACKFRAME64 StackFrame, PCONTEXT context) noexcept;

    bool isModuleLoaded() const noexcept;

    bool getModuleInfo(DWORD64 baseAddr, IMAGEHLPMODULE& info) noexcept;
    bool getSymFromAddr(DWORD64 address, PDWORD64 displacement, PSYMBOLINFO symbol) noexcept;
    bool getLineFromAddr(DWORD64 qwAddr, PDWORD pdwDisplacement, IMAGEHLPLINE& Line64) noexcept;
    bool getLocalVariable(const PCONTEXT context, VariableList& variable) noexcept;

	util::string getTypeName(PSYMBOLINFO syminfo) noexcept;
	util::string getTypeName(DWORD typeID, DWORD64 modBase) noexcept;
	util::string getTypeValue(PSYMBOLINFO syminfo, const PCONTEXT context) noexcept;
	util::string getTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, DWORD size) noexcept;

private:

    bool loadSymbol(const util::string& filepath) noexcept;

    bool loadModulesFromTH32(HANDLE process, DWORD pid) noexcept;
    bool loadModulesFromPSAPI(HANDLE process) noexcept;

    SymBaseType getBaseType(DWORD typeID, DWORD64 modBase) noexcept;

	util::string getSymTypeName(SYM_TYPE type) noexcept;
	util::string getBaseTypeName(DWORD typeID, DWORD64 modBase) noexcept;
	util::string getPointerTypeName(DWORD typeID, DWORD64 modBase) noexcept;
	util::string getArrayTypeName(DWORD typeID, DWORD64 modBase) noexcept;
	util::string getUDTTypeName(DWORD typeID, DWORD64 modBase) noexcept;
	util::string getEnumTypeName(DWORD typeID, DWORD64 modBase) noexcept;
	util::string getFuncTypeName(DWORD typeID, DWORD64 modBase) noexcept;

    BOOL isSimpleType(DWORD typeID, DWORD64 modBase) noexcept;
    BOOL getDataMemberInfo(DWORD memberID, DWORD64 modBase, DWORD64 address, const BYTE* pData, ostringstream& stream) noexcept;
    BOOL VariantEqual(VARIANT var, DWORD baseType, const BYTE* pData) noexcept;

    DWORD64 getTypeAddress(PSYMBOLINFO syminfo, const PCONTEXT context) noexcept;

	util::string getTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, const BYTE* pData) noexcept;
	util::string getBaseTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, const BYTE* pData) noexcept;
	util::string getPointerTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, const BYTE* pData) noexcept;
	util::string getArrayTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, const BYTE* pData) noexcept;
	util::string getUDTTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, const BYTE* pData) noexcept;
	util::string getEnumTypeValue(DWORD typeID, DWORD64 modBase, DWORD64 address, const BYTE* pData) noexcept;

    static BOOL CALL EnumSymCallBack(PSYMBOLINFO syminfo, ULONG symsize, PVOID data) noexcept;

private:
    StackWalker* _walker;

    HANDLE _process;
    DWORD  _process_id;

    bool _module_loaded;
};

_NAME_END

#endif