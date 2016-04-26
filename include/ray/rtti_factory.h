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
#ifndef _H_RTTI_FACTORY_H_
#define _H_RTTI_FACTORY_H_

#include <ray/rtti.h>

_NAME_BEGIN

namespace rtti
{
	class EXPORT Factory final
	{
		__DeclareSingleton(Factory)
	public:
		Factory() noexcept;
		~Factory() noexcept;

		bool open() noexcept;

		bool add(Rtti* rtti) noexcept;

		Rtti* getRTTI(const char* name) noexcept;
		Rtti* getRTTI(const std::string& name) noexcept;

		const Rtti* getRTTI(const char* name) const noexcept;
		const Rtti* getRTTI(const std::string& name) const noexcept;

		InterfacePtr createObject(const char*, const Rtti& base) const except;
		InterfacePtr createObject(const std::string& name, const Rtti& base) const except;

		template<typename T>
		std::shared_ptr<T> make_shared(const std::string& name)
		{
			return std::dynamic_pointer_cast<T>(this->createObject(name, T::RTTI));
		}

		template<typename T>
		std::shared_ptr<T> make_shared(const char* name)
		{
			return std::dynamic_pointer_cast<T>(this->createObject(name, T::RTTI));
		}

		template<typename T>
		std::unique_ptr<T> make_unique(const std::string& name)
		{
			return std::dynamic_pointer_cast<T>(this->createObject(name, T::RTTI));
		}

		template<typename T>
		std::unique_ptr<T> make_unique(const char* name)
		{
			return std::dynamic_pointer_cast<T>(this->createObject(name, T::RTTI));
		}

	private:
		mutable std::vector<Rtti*> _rttis;
		mutable std::map<std::string, Rtti*> _rtti_lists;
	};

	template<typename T>
	std::shared_ptr<T> make_shared(const std::string& name)
	{
		return Factory::instance()->make_shared<T>(name);
	}

	template<typename T>
	std::shared_ptr<T> make_shared(const char* name)
	{
		return Factory::instance()->make_shared<T>(name);
	}

	template<typename T>
	std::unique_ptr<T> make_unique(const std::string& name)
	{
		return Factory::instance()->make_unique<T>(name);
	}

	template<typename T>
	std::unique_ptr<T> make_unique(const char* name)
	{
		return Factory::instance()->make_unique<T>(name);
	}
}

_NAME_END

#endif