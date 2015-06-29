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
#ifndef _H_RTTI_H_
#define _H_RTTI_H_

#include <ray/platform.h>

_NAME_BEGIN

typedef std::uint32_t RTTIType;
typedef std::uint32_t ClassID;
typedef void*(*ClassFactoryFunc)(ClassID);

template <class T> inline
std::pair<std::string, T> make_name(const std::string& name, T&& value)
{
    return std::pair<const std::string&, T&>(name.c_str(), std::forward<T>(value));
}

template <class T> inline
std::pair<const char*, T&> make_name(const char* name, T& value)
{
    return std::pair<const char*, T&>(name, value);
}

#define rtti_name(T) make_name(#T, T)
#define rtti_alias(T, alias)  make_name(alias, T)

class IRTTI
{
public:
    IRTTI() noexcept {};
    virtual ~IRTTI() noexcept {}

    virtual void* create() noexcept = 0;

    virtual RTTIType getBaseType() const noexcept = 0;
    virtual RTTIType getDerivedType() const noexcept = 0;
};

template<typename Derived, typename Base = void>
class RTTI final : public IRTTI
{
public:
    RTTI(const std::string& derived, const std::string& base = "")
        : _derived(derived)
        , _base(base)
    {
    }

    virtual void* create() noexcept
    {
        return new Derived;
    }

    virtual RTTIType getBaseType() const noexcept
    {
        return typeid(Base).hash_code();
    }

    virtual RTTIType getDerivedType() const noexcept
    {
        return typeid(Derived).hash_code();
    }

private:

    std::string _derived;
    std::string _base;
};

class EXPORT RTTIFactory
{
    __DeclareSingleton(RTTIFactory)
public:
    RTTIFactory()
    {
    }

    ~RTTIFactory()
    {
        for (auto& it : _rtti_lists)
        {
            delete it.second;
        }
    }

    template<typename Derived, typename Base>
    IRTTI* createRTTI(const std::string& derived, const std::string& base = "")
    {
        auto instance = new RTTI<Derived, Base>(derived, base);
        _rtti_lists[derived] = instance;
        return instance;
    }

    IRTTI* getRTTI(const std::string& name)
    {
        return _rtti_lists[name];
    }

    IRTTI* getRTTI(const char* name)
    {
        return _rtti_lists[name];
    }

    void* createObject(const std::string& name)
    {
        auto rtti = this->getRTTI(name);
        if (rtti)
        {
            return rtti->create();
        }

        return nullptr;
    }

    void* createObject(const char* name)
    {
        auto rtti = this->getRTTI(name);
        if (rtti)
        {
            return rtti->create();
        }

        return nullptr;
    }

private:
    std::map<std::string, IRTTI*> _rtti_lists;
};

#define __DeclareInterface(Derived)\
public:\
    static RTTIType getType() noexcept;\
    virtual const IRTTI* getRTTI() const noexcept = 0;\

#define __ImplementInterface(Derived) \
    RTTIType Derived::getType() noexcept\
    {\
        return typeid(Derived).hash_code();\
    }

#define __DeclareSubInterface(Derived, Base)\
public:\
    static RTTIType getType() noexcept;\
    virtual const IRTTI* getRTTI() const noexcept = 0;\

#define __ImplementSubInterface(Derived, Base) \
    RTTIType Derived::getType() noexcept\
    {\
        return typeid(Derived).hash_code();\
    }\

#define __DeclareClass(Base) \
public:\
    static const IRTTI& getRTTI() noexcept;\
private:\
    static RTTI<Base> _rtti;

#define __ImplementClass(Base) \
    _NAME RTTI<Base> Base::_rtti = _NAME RTTI<Base>(#Base);\
    const IRTTI& Base::getRTTI() noexcept\
    {\
        return _rtti;\
    }

#define __DeclareSubClass(Derived, Base)\
public:\
    static _NAME RTTIType getType() noexcept;\
    virtual const _NAME IRTTI* getRTTI() const noexcept;\
private:\
    static _NAME IRTTI* _rtti;

#define __ImplementSubClass(Derived, Base) \
    _NAME IRTTI* Derived::_rtti = _NAME RTTIFactory::instance()->createRTTI<Derived, Base>(#Derived, #Base);\
    _NAME RTTIType Derived::getType() noexcept\
    {\
        return typeid(Derived).hash_code();\
    }\
    const _NAME IRTTI* Derived::getRTTI() const noexcept\
    {\
        return _rtti;\
    }

_NAME_END

#endif