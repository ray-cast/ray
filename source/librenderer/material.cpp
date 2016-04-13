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
#include <ray/material.h>
#include <ray/material_tech.h>
#include <ray/material_param.h>
#include <ray/graphics_shader.h>
#include <ray/graphics_input_layout.h>

_NAME_BEGIN

__ImplementSubClass(Material, rtti::Interface, "Material")


Material::Material() noexcept
{
}

Material::~Material() noexcept
{
	this->close();
}

bool
Material::setup(const MaterialDesc& materialDesc) noexcept
{
	_materialDesc = materialDesc;

	for (auto& technique : _materialDesc.getTechs())
	{
		auto& passList = technique->getPassList();
		for (auto& pass : passList)
			pass->setup(*this);
	}

	return true;
}

void
Material::close() noexcept
{
	for (auto& technique : _materialDesc.getTechs())
	{
		auto& passList = technique->getPassList();
		for (auto& pass : passList)
			pass->close();
	}
}

const std::string&
Material::getName() const noexcept
{
	return _materialDesc.getName();
}

MaterialTechPtr
Material::getTech(const std::string& name) noexcept
{
	return _materialDesc.getTech(name);
}

MaterialTechniques&
Material::getTechs() noexcept
{
	return _materialDesc.getTechs();
}

MaterialParamPtr
Material::getParameter(const std::string& name) const noexcept
{
	return _materialDesc.getParameter(name);
}

MaterialParams&
Material::getParameters() noexcept
{
	return _materialDesc.getParameters();
}

const MaterialParams&
Material::getParameters() const noexcept
{
	return _materialDesc.getParameters();
}

MaterialVariantPtr
Material::getMacro(const std::string& name) const noexcept
{
	return _materialDesc.getMacro(name);
}

MaterialVariants&
Material::getMacros() noexcept
{
	return _materialDesc.getMacros();
}

const MaterialVariants&
Material::getMacros() const noexcept
{
	return _materialDesc.getMacros();
}

_NAME_END