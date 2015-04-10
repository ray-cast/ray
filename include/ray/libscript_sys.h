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

#ifndef _H_LIBSCRIPT_SYS_H_
#define _H_LIBSCRIPT_SYS_H_

#include <string>
#include <vector>
#include <ray/platform/platform.h>
#include <ray/platform/memory.h>
#include <ray/platform/singleton.h>
#include <ray/script/libscript_stack.h>
#include <ray/script/libscript_value.h>
#include <ray/script/libscript_table.h>
#include <ray/script/libscript_function.h>
#include <ray/script/libscript_thread.h>
#include <ray/script/libscript_helper.h>

/// @addtogroup script
/// @{

_NAME_BEGIN

class EXPORT Script final : public Stack
{
public:
    Script(MemAllocFunc alloc = nullptr, void* ud = nullptr);
    Script(MemAlloc* alloc);
    ~Script();

    void exec(const std::string& fileName);
    void execString(const std::string& str);

    Value newFunction(const std::string& script);
    Value newFunction(CFunction function);
    Value newTable();
    Value newThread();
    Value newThread(Function function);
    Value getGlobal(const std::string& name);
    Table getGlobalTable();
    Value getNil();

private:
    Script(const Script& copy) = delete;
    Script& operator=(const Script& copy) = delete;
};

/// @}

_NAME_END

#endif