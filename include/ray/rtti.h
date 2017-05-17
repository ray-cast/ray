// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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

#include <ray/rtti_macros.h>

_NAME_BEGIN

namespace rtti
{
	typedef std::shared_ptr<class Interface> InterfacePtr;

	class EXPORT Rtti
	{
	public:
		typedef Interface*(*RttiConstruct)();
	public:
		Rtti(const std::string& name, RttiConstruct creator, const Rtti* parent) noexcept;
		~Rtti() noexcept;

		InterfacePtr create() const except;

		const Rtti* getParent() const noexcept;

		const std::string& type_name() const noexcept;

		bool isDerivedFrom(const Rtti* other) const;
		bool isDerivedFrom(const Rtti& other) const;
		bool isDerivedFrom(const std::string& name) const;

	private:
		std::string _name;
		const Rtti* _parent;
		RttiConstruct _construct;
	};

	class EXPORT Interface : public std::enable_shared_from_this<Interface>
	{
		__DeclareClass(Interface)
	public:
		Interface() noexcept;
		virtual ~Interface() noexcept;

		bool isInstanceOf(const Rtti* rtti) const noexcept;
		bool isInstanceOf(const Rtti& rtti) const noexcept;
		bool isInstanceOf(const std::string& className) const noexcept;

		template<typename T>
		bool isInstanceOf() const noexcept
		{
			return this->isInstanceOf(T::getRtti());
		}

		bool isA(const Rtti* rtti) const noexcept;
		bool isA(const Rtti& rtti) const noexcept;
		bool isA(const std::string& rttiName) const noexcept;

		template<typename T>
		bool isA() const noexcept
		{
			return this->isA(T::getRtti());
		}

		template<typename T>
		T* upcast() noexcept
		{
			assert(this->isA<T>());
			return dynamic_cast<T*>(this);
		}

		template<typename T>
		T* downcast() noexcept
		{
			assert(this->isA<T>());
			return dynamic_cast<T*>(this);
		}

		template<typename T>
		T* cast() noexcept
		{
			return reinterpret_cast<T*>(this);
		}

		template<typename T>
		const T* upcast() const noexcept
		{
			assert(this->isA<T>());
			return dynamic_cast<const T*>(this);
		}

		template<typename T>
		const T* downcast() const noexcept
		{
			assert(this->isA<T>());
			return dynamic_cast<const T*>(this);
		}

		template<typename T>
		const T* cast() const noexcept
		{
			return reinterpret_cast<const T*>(this);
		}

		template<typename T>
		std::shared_ptr<T> upcast_pointer() noexcept
		{
			assert(this->isA<T>());
			return std::dynamic_pointer_cast<T>(this->shared_from_this());
		}

		template<typename T>
		std::shared_ptr<T> downcast_pointer() noexcept
		{
			assert(this->isA<T>());
			return std::dynamic_pointer_cast<T>(this->shared_from_this());
		}

		template<typename T>
		std::shared_ptr<T> cast_pointer() noexcept
		{
			assert(this->isA<T>());
			return std::dynamic_pointer_cast<T>(this->shared_from_this());
		}

		template<typename T>
		std::shared_ptr<const T> upcast_pointer() const noexcept
		{
			assert(this->isA<T>());
			return std::dynamic_pointer_cast<const T>(this->shared_from_this());
		}

		template<typename T>
		std::shared_ptr<const T> downcast_pointer() const noexcept
		{
			assert(this->isA<T>());
			return std::dynamic_pointer_cast<const T>(this->shared_from_this());
		}

		template<typename T>
		std::shared_ptr<const T> cast_pointer() const noexcept
		{
			assert(this->isA<T>());
			return std::dynamic_pointer_cast<const T>(this->shared_from_this());
		}
	};
}

_NAME_END

#endif