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

#ifndef _H_LIBSCRIPT_FWD_H_
#define _H_LIBSCRIPT_FWD_H_

#include <string>
#include <tuple>

#include <ray/platform/platform.h>
#include <ray/platform/except.h>
#include <ray/platform/memory.h>

struct lua_State;

_NAME_BEGIN

class Script;

class ValueHandler;

typedef lua_State* RawInterface;

typedef int(*CFunction)(RawInterface raw);

typedef int(*ChunkWriterFunc) (RawInterface raw, const void* p, size_t sz, void* ud);

typedef const char* (*ChunkReaderFunc) (RawInterface raw, void* data, size_t* size);

typedef void* (*MemAllocFunc) (void *ud, void *ptr, size_t osize, size_t nsize);

struct MemAlloc
{
    virtual void* alloc(std::size_t s) = 0;
    virtual void free(void* ptr, std::size_t s) = 0;
};

struct Class
{
    void* c;
    bool ref;
    bool readonly;
    std::string metaname;
};

typedef struct {} VOID_T;

template<typename _Class>
struct ClassInfo {
    _Class* obj;
    bool ref;
    bool readonly;
};

template<typename _T>
struct Wrapper {
    _T value;
};

#define METATABLEPREFIX "_metaclass_"

#define SCRIPT_EXCEPTION(s) throw failure(s)

_NAME_END

#endif