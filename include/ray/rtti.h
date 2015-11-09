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

class EXPORT RTTI
{
public:
	typedef std::size_t HashCode;
	typedef std::shared_ptr<RTTI> pointer;

public:
    RTTI() noexcept {};
    virtual ~RTTI() noexcept {}

    virtual void* create() noexcept = 0;

	virtual const std::string& getBaseName() const noexcept = 0;
	virtual const std::string& getDerivedName() const noexcept = 0;

    virtual HashCode getBaseType() const noexcept = 0;
    virtual HashCode getDerivedType() const noexcept = 0;
};

template<typename Derived, typename Base = void>
class RTTIInterface : public RTTI
{
public:
	RTTIInterface(const std::string& derived, const std::string& base) noexcept
		: _derived(derived)
		, _base(base)
	{
	}

	virtual ~RTTIInterface() noexcept
	{
	}

	virtual void* create() noexcept { assert(nullptr); return nullptr; }

	virtual const std::string& getBaseName() const noexcept { return _base; }
	virtual const std::string& getDerivedName() const noexcept { return _derived; }

	virtual HashCode getBaseType() const noexcept { return typeid(Base).hash_code(); }
	virtual HashCode getDerivedType() const noexcept { return typeid(Derived).hash_code(); }

private:
	RTTIInterface(const RTTIInterface&) noexcept = delete;
	RTTIInterface& operator=(const RTTIInterface&) noexcept = delete;

private:

	std::string _derived;
	std::string _base;
};

template<typename Derived, typename Base = void>
class RTTIClass final : public RTTIInterface<Derived, Base>
{
public:
	RTTIClass(const std::string& derived, const std::string& base)
		: RTTIInterface<Derived, Base>(derived, base)
    {
    }

	virtual void* create() noexcept { return new Derived; }

private:
	RTTIClass(const RTTIClass&) noexcept = delete;
	RTTIClass& operator=(const RTTIClass&) noexcept = delete;
};

class EXPORT RTTIFactory
{
    __DeclareSingleton(RTTIFactory)
public:
    RTTIFactory() noexcept
    {
    }

    ~RTTIFactory() noexcept
    {
    }

	template<typename Derived, typename Base>
	RTTI::pointer createInterface(const std::string& derived, const std::string& base)
	{
		auto derivedName = this->getName(derived);
		auto baseName = this->getName(base);
		auto instance = std::make_shared<RTTIInterface<Derived, Base>>(derivedName, baseName);
		_rtti_lists[derived] = instance;
		return instance;
	}

    template<typename Derived, typename Base>
    RTTI::pointer createClass(const std::string& derived, const std::string& base)
    {
		auto derivedName = this->getName(derived);
		auto baseName = this->getName(base);
        auto instance = std::make_shared<RTTIClass<Derived, Base>>(derivedName, baseName);
        _rtti_lists[derived] = instance;
        return instance;
    }

    RTTI::pointer getRTTI(const std::string& name)
    {
        return _rtti_lists[name];
    }

    RTTI::pointer getRTTI(const char* name)
    {
        return _rtti_lists[name];
    }

	template<typename T>
	std::shared_ptr<T> createObject(const std::string& name)
    {
		std::string it = name;

		while (!it.empty())
		{
			auto rtti = this->getRTTI(it);
			if (!rtti)
				return nullptr;

			if (rtti->getDerivedType() == typeid(T).hash_code())
			{
				rtti = this->getRTTI(name);
				return std::shared_ptr<T>((T*)rtti->create());
			}

			it = rtti->getBaseName();
		}

        return nullptr;
    }

	template<typename T>
    std::shared_ptr<T> createObject(const char* name)
    {
		std::string string(name);
		return this->createObject<T>(string);
    }

private:
	std::string getName(std::string name) const
	{
		std::size_t pos = name.find("::", 0, 2);
		if (pos != std::string::npos)
			name.erase(0, pos + 2);
		return name;
	}

private:
    std::map<std::string, RTTI::pointer> _rtti_lists;
};

#define __DeclareInterface(Base)\
public:\
    static RTTI::HashCode getType() noexcept;\
    virtual const RTTI::pointer getRTTI() const noexcept = 0;\
private:\
    static _NAME RTTI::pointer _rtti;

#define __ImplementInterface(Base) \
    _NAME RTTI::pointer Base::_rtti = _NAME RTTIFactory::instance()->createInterface<Base, void>(#Base, "");\
    _NAME RTTI::HashCode Base::getType() noexcept\
    {\
        return typeid(Base).hash_code();\
    }\
    const _NAME RTTI::pointer Base::getRTTI() const noexcept\
    {\
        return _rtti;\
    }

#define __DeclareSubInterface(Derived, Base)\
public:\
    static RTTI::HashCode getType() noexcept;\
    virtual const RTTI::pointer getRTTI() const noexcept = 0;\
private:\
    static _NAME RTTI::pointer _rtti;

#define __ImplementSubInterface(Derived, Base) \
    _NAME RTTI::pointer Derived::_rtti = _NAME RTTIFactory::instance()->createInterface<Derived, Base>(#Derived, #Base);\
    _NAME RTTI::HashCode Derived::getType() noexcept\
    {\
        return typeid(Derived).hash_code();\
    }\
    const _NAME RTTI::pointer Derived::getRTTI() const noexcept\
    {\
        return _rtti;\
    }

#define __DeclareClass(Base) \
public:\
    static _NAME RTTI::HashCode getType() noexcept;\
    virtual const _NAME RTTI::pointer getRTTI() const noexcept;\
private:\
    static _NAME RTTI::pointer _rtti;

#define __ImplementClass(Base) \
    _NAME RTTI::pointer Base::_rtti = _NAME RTTIFactory::instance()->createClass<Base, void>(#Base, "");\
    _NAME RTTI::HashCode Derived::getType() noexcept\
    {\
        return typeid(Base).hash_code();\
    }\
    const _NAME RTTI::pointer Derived::getRTTI() const noexcept\
    {\
        return _rtti;\
    }

#define __DeclareSubClass(Derived, Base)\
public:\
    static _NAME RTTI::HashCode getType() noexcept;\
    virtual const _NAME RTTI::pointer getRTTI() const noexcept;\
private:\
    static _NAME RTTI::pointer _rtti;

#define __ImplementSubClass(Derived, Base) \
    _NAME RTTI::pointer Derived::_rtti = _NAME RTTIFactory::instance()->createClass<Derived, Base>(#Derived, #Base);\
    _NAME RTTI::HashCode Derived::getType() noexcept\
    {\
        return typeid(Derived).hash_code();\
    }\
    const _NAME RTTI::pointer Derived::getRTTI() const noexcept\
    {\
        return _rtti;\
    }

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

_NAME_END

#endif