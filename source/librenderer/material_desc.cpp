// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include <ray/material_desc.h>
#include <ray/material_tech.h>
#include <ray/material_param.h>
#include <ray/material_variant.h>

_NAME_BEGIN

MaterialDesc::MaterialDesc() noexcept
{
}

MaterialDesc::~MaterialDesc() noexcept
{
}

void
MaterialDesc::addTech(MaterialTechPtr technique) noexcept
{
	assert(technique);
	assert(std::find(_techniques.begin(), _techniques.end(), technique) == _techniques.end());
	_techniques.push_back(technique);
}

void
MaterialDesc::removeTech(MaterialTechPtr technique) noexcept
{
	assert(technique);
	auto it = std::find(_techniques.begin(), _techniques.end(), technique);
	if (it != _techniques.end())
	{
		_techniques.erase(it);
	}
}

MaterialTechPtr
MaterialDesc::getTech(const std::string& name) noexcept
{
	for (auto& it : _techniques)
	{
		if (it->getName() == name)
			return it;
	}

	return nullptr;
}

MaterialTechniques&
MaterialDesc::getTechs() noexcept
{
	return _techniques;
}

void
MaterialDesc::addParameter(MaterialParamPtr parameter) noexcept
{
	assert(parameter);
	assert(std::find(_parameters.begin(), _parameters.end(), parameter) == _parameters.end());
	_parameters.push_back(parameter);
}

void
MaterialDesc::removeParameter(MaterialParamPtr parameter) noexcept
{
	assert(parameter);
	auto it = std::find(_parameters.begin(), _parameters.end(), parameter);
	if (it != _parameters.end())
	{
		_parameters.erase(it);
	}
}

MaterialParamPtr
MaterialDesc::getParameter(const std::string& name) const noexcept
{
	assert(!name.empty());
	for (auto& it : _parameters)
	{
		if (it->getName() == name)
			return it;
	}

	return nullptr;
}

MaterialParams&
MaterialDesc::getParameters() noexcept
{
	return _parameters;
}

const MaterialParams&
MaterialDesc::getParameters() const noexcept
{
	return _parameters;
}

void
MaterialDesc::addMacro(MaterialParamPtr macro) noexcept
{
	assert(macro);
	assert(std::find(_macros.begin(), _macros.end(), macro) == _macros.end());
	_macros.push_back(macro);
}

void
MaterialDesc::removeMacro(MaterialParamPtr macro) noexcept
{
	assert(macro);
	auto it = std::find(_macros.begin(), _macros.end(), macro);
	if (it != _macros.end())
	{
		_macros.erase(it);
	}
}

MaterialParamPtr
MaterialDesc::getMacro(const std::string& name) const noexcept
{
	assert(!name.empty());
	for (auto& it : _macros)
	{
		if (it->getName() == name)
			return it;
	}

	return nullptr;
}

MaterialParams&
MaterialDesc::getMacros() noexcept
{
	return _macros;
}

const MaterialParams&
MaterialDesc::getMacros() const noexcept
{
	return _macros;
}

_NAME_END