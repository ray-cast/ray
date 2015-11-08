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
#ifndef _H_WIN_WKBS_H_
#define _H_WIN_WKBS_H_

#include <ray/string.h>

_NAME_BEGIN

enum STACKWALKOPTIONS
{
    SWO_NONE = 1 << 0,
    SWO_SYMBOL = 1 << 1,
    SWO_LINE = 1 << 2,
    SWO_MODULE_INFO = 1 << 3,
    SWO_FILE_VERSION = 1 << 4,
    SWO_ENTRY = 1 << 5,
    SWO_SYM_BUILD_PATH = 1 << 6,
    SWO_SYM_USE_SYM_SERVER = 1 << 7
};

class EXPORT WalkerBase
{
public:
    enum _Options : std::uint8_t {};
    static const _Options none = (_Options)SWO_NONE;
    static const _Options symbol = (_Options)SWO_SYMBOL;
    static const _Options line = (_Options)SWO_LINE;
    static const _Options module_info = (_Options)SWO_MODULE_INFO;
    static const _Options file_version = (_Options)SWO_FILE_VERSION;
    static const _Options sym_build_path = (_Options)SWO_SYM_BUILD_PATH;
    static const _Options sym_use_sym_server = (_Options)SWO_SYM_USE_SYM_SERVER;
    static const _Options options_all = (_Options)(symbol | line | module_info | file_version | sym_build_path | sym_use_sym_server);
    static const _Options normal = (_Options)(options_all ^ module_info ^ symbol);

    static const int STACKWALK_MAX_NAMELEN = 2048;
    static const int MAX_BUFFER_LENGTH = 2048;

    struct CallStackVariable
    {
        DWORD size;
        DWORD typeID;
        ULONG64 modBase;
        DWORD64 address;
        string typeName;
        string varName;
        string valueName;
    };

    struct VariableList
    {
        std::vector<CallStackVariable> list;
    };

    struct CallStackEntry
    {
        DWORD64 offset;  // if 0, we have no valid entry
        DWORD64 offsetFromSmybol;
        DWORD offsetFromLine;
        DWORD lineNumber;
        DWORD64 baseOfImage;
        string name;
        string lineFileName;
        string moduleName;
        string imageName;
        string loadedImageName;
        VariableList variable;
    };

    enum _CallstackEntryType { _EntryMask = 256 };
    static const _CallstackEntryType first = (_CallstackEntryType)0x0;
    static const _CallstackEntryType next = (_CallstackEntryType)0x1;
    static const _CallstackEntryType last = (_CallstackEntryType)0x2;

	typedef std::basic_ostringstream<char_type> ostringstream;

    _INT_BITMASK(_Options, Options);
    _INT_BITMASK(_CallstackEntryType, EntryType);

protected:

    static string GetCurrentDirectory() noexcept;
    static string GetModuleFileName(HMODULE hModule = 0) noexcept;
    static string GetModuleDirectory(HMODULE hModule = 0) noexcept;
    static string GetEnvironmentVariable(const string& name) noexcept;
};

_NAME_END

#endif