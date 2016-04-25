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
#include <ray/material_tech.h>

_NAME_BEGIN

__ImplementSubClass(MaterialTech, rtti::Interface, "MaterialTech")

MaterialTech::MaterialTech() noexcept
{
}

MaterialTech::~MaterialTech() noexcept
{
}

void
MaterialTech::addPass(MaterialPassPtr pass)
{
	assert(std::find(_passList.begin(), _passList.end(), pass) == _passList.end());
	_passList.push_back(pass);
}

void
MaterialTech::removePass(MaterialPassPtr pass)
{
	auto it = std::find(_passList.begin(), _passList.end(), pass);
	if (it != _passList.end())
	{
		_passList.erase(it);
	}
}

MaterialPassPtr
MaterialTech::getPass(const std::string& name) noexcept
{
	for (auto& it : _passList)
	{
		if (it->getName() == name)
			return it;
	}

	return nullptr;
}

const MaterialPassList&
MaterialTech::getPassList() const noexcept
{
	return _passList;
}

void
MaterialTech::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string& 
MaterialTech::getName() const noexcept
{
	return _name;
}

bool
MaterialTech::empty() const noexcept
{
	return _passList.empty();
}

std::size_t
MaterialTech::count() const noexcept
{
	return _passList.size();
}

MaterialTechPtr
MaterialTech::clone() const noexcept
{
	auto tech = std::make_shared<MaterialTech>();
	tech->setName(this->getName());

	for (auto& pass : _passList)
		tech->addPass(pass->clone());
	return tech;
}

_NAME_END