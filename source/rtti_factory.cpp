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
#include <ray/rtti_factory.h>

_NAME_BEGIN

using namespace rtti;

__ImplementSingleton(Factory)

Factory::Factory() noexcept
{
}

Factory::~Factory() noexcept
{
}

bool 
Factory::open() noexcept
{
	for (auto& it : _rttis)
	{
		if (it)
			_rtti_lists[it->getName()] = it;
	}
	return true;
}

bool 
Factory::add(Rtti* rtti) noexcept
{
	_rttis.push_back(rtti);
	return true;
}

Rtti* 
Factory::getRTTI(const std::string& name) noexcept
{
	return _rtti_lists[name];
}

Rtti* 
Factory::getRTTI(const char* name) noexcept
{
	return _rtti_lists[name];
}

const Rtti*
Factory::getRTTI(const std::string& name) const noexcept
{
	return _rtti_lists[name];
}

const Rtti*
Factory::getRTTI(const char* name) const noexcept
{
	return _rtti_lists[name];
}

Interface*
Factory::createObject(const char* name, const Rtti& base) const except
{
	assert(name);

	auto rtti = this->getRTTI(name);
	if (!rtti)
		return nullptr;

	if (rtti->isDerivedFrom(base))
		return rtti->create();

	return nullptr;
}

Interface*
Factory::createObject(const std::string& name, const Rtti& base) const except
{
	assert(!name.empty());

	auto rtti = this->getRTTI(name);
	if (!rtti)
		return nullptr;

	if (rtti->isDerivedFrom(base))
		return rtti->create();

	return nullptr;
}

_NAME_END