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
#include <ray/memory.h>

_NAME_BEGIN

typedef std::uint32_t RTTIType;

#define __DeclareClass(type) \
public:\
    static std::shared_ptr<type> create();

#define __ImplementClass(type) \
    std::shared_ptr<type> type::create() \
    {\
    return std::make_shared<type>();\
    }\

#define __DeclareInterface(Derived)\
public:\
    virtual RTTIType getBaseType() const noexcept; \
    virtual RTTIType getDerivedType() const noexcept; \
    static RTTIType getType() noexcept;\

#define __ImplementInterface(Derived) \
    RTTIType Derived::getBaseType() const noexcept \
    {\
        return typeid(Derived).hash_code();\
    }\
    RTTIType Derived::getDerivedType() const noexcept \
    {\
        return typeid(Derived).hash_code();\
    }\
    RTTIType Derived::getType() noexcept\
    {\
        return typeid(Derived).hash_code();\
    }

#define __DeclareSubInterface(Derived, Base)\
public:\
    virtual RTTIType getBaseType() const noexcept; \
    virtual RTTIType getDerivedType() const noexcept; \
    static RTTIType getType() noexcept;\

#define __ImplementSubInterface(Derived, Base) \
    RTTIType Derived::getBaseType() const noexcept \
    {\
        return typeid(Base).hash_code();\
    }\
    RTTIType Derived::getDerivedType() const noexcept \
    {\
        return typeid(Derived).hash_code();\
    }\
    RTTIType Derived::getType() noexcept\
    {\
        return typeid(Derived).hash_code();\
    }

#define __DeclareSubClass(Derived, Base)\
public:\
    static std::shared_ptr<Derived> create(); \
    virtual RTTIType getBaseType() const noexcept; \
    virtual RTTIType getDerivedType() const noexcept; \
    static RTTIType getType() noexcept;\

#define __ImplementSubClass(Derived, Base) \
    std::shared_ptr<Derived> Derived::create() \
    {\
        return std::make_shared<Derived>();\
    }\
    RTTIType Derived::getDerivedType() const noexcept \
    {\
        return typeid(Derived).hash_code();\
    }\
    RTTIType Derived::getBaseType() const noexcept \
    {\
        return typeid(Base).hash_code();\
    }\
    RTTIType Derived::getType() noexcept\
    {\
        return typeid(Derived).hash_code();\
    }

_NAME_END