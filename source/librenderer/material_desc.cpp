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
#include <ray/graphics_input_layout.h>
#include <ray/graphics_shader.h>

_NAME_BEGIN

MaterialPassDesc::MaterialPassDesc() noexcept
{
}

MaterialPassDesc::~MaterialPassDesc() noexcept
{
}

void
MaterialPassDesc::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
MaterialPassDesc::getName() const noexcept
{
	return _name;
}

void
MaterialPassDesc::setGraphicsState(GraphicsStatePtr& state) noexcept
{
	_state = state;
}

void
MaterialPassDesc::setGraphicsProgram(GraphicsProgramPtr& program) noexcept
{
	_program = program;
}

void
MaterialPassDesc::setGraphicsInputLayout(GraphicsInputLayoutPtr& inputLayout) noexcept
{
	_inputLayout = inputLayout;
}

void
MaterialPassDesc::setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr& descriptorPool) noexcept
{
	_descriptorPool = descriptorPool;
}

void
MaterialPassDesc::setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr& descriptorSetLayout) noexcept
{
	_descriptorSetLayout = descriptorSetLayout;
}

GraphicsStatePtr
MaterialPassDesc::getGraphicsState() const noexcept
{
	return _state;
}

GraphicsProgramPtr
MaterialPassDesc::getGraphicsProgram() const noexcept
{
	return _program;
}

GraphicsInputLayoutPtr
MaterialPassDesc::getGraphicsInputLayout() const noexcept
{
	return _inputLayout;
}

GraphicsDescriptorPoolPtr
MaterialPassDesc::getGraphicsDescriptorPool() const noexcept
{
	return _descriptorPool;
}

GraphicsDescriptorSetLayoutPtr
MaterialPassDesc::getGraphicsDescriptorSetLayout() const noexcept
{
	return _descriptorSetLayout;
}

MaterialTechDesc::MaterialTechDesc() noexcept
{
}

MaterialTechDesc::~MaterialTechDesc() noexcept
{
}

void 
MaterialTechDesc::addPass(MaterialPassDescPtr pass) noexcept
{
	_passList.push_back(pass);
}

void 
MaterialTechDesc::removePass(MaterialPassDescPtr pass) noexcept
{
	assert(pass);
	auto it = std::find(_passList.begin(), _passList.end(), pass);
	if (it != _passList.end())
		_passList.erase(it);
}

MaterialDesc::MaterialDesc() noexcept
{
}

MaterialDesc::~MaterialDesc() noexcept
{
}

void
MaterialDesc::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
MaterialDesc::getName() const noexcept
{
	return _name;
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
MaterialDesc::addMacro(MaterialVariantPtr macro) noexcept
{
	assert(macro);
	assert(std::find(_macros.begin(), _macros.end(), macro) == _macros.end());
	_macros.push_back(macro);
}

void
MaterialDesc::removeMacro(MaterialVariantPtr macro) noexcept
{
	assert(macro);
	auto it = std::find(_macros.begin(), _macros.end(), macro);
	if (it != _macros.end())
	{
		_macros.erase(it);
	}
}

MaterialVariantPtr
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

MaterialVariants&
MaterialDesc::getMacros() noexcept
{
	return _macros;
}

const MaterialVariants&
MaterialDesc::getMacros() const noexcept
{
	return _macros;
}

void
MaterialDesc::addInputLayout(GraphicsInputLayoutPtr inputLayout) noexcept
{
	_inputLayouts.push_back(inputLayout);
}

void
MaterialDesc::removeInputLayout(GraphicsInputLayoutPtr inputLayout) noexcept
{
	assert(inputLayout);
	auto it = std::find(_inputLayouts.begin(), _inputLayouts.end(), inputLayout);
	if (it != _inputLayouts.end())
	{
		_inputLayouts.erase(it);
	}
}

GraphicsInputLayoutPtr
MaterialDesc::getInputLayout(const std::string& name) const noexcept
{
	assert(!name.empty());
	for (auto& it : _inputLayouts)
	{
		if (it->getGraphicsInputLayoutDesc().getName() == name)
			return it;
	}

	return nullptr;
}

GraphicsInputLayouts&
MaterialDesc::getInputLayouts() noexcept
{
	return _inputLayouts;
}

const GraphicsInputLayouts&
MaterialDesc::getInputLayouts() const noexcept
{
	return _inputLayouts;
}

void
MaterialDesc::addShader(GraphicsShaderPtr inputLayout) noexcept
{
	_shaders.push_back(inputLayout);
}

void
MaterialDesc::removeShader(GraphicsShaderPtr inputLayout) noexcept
{
	assert(inputLayout);
	auto it = std::find(_shaders.begin(), _shaders.end(), inputLayout);
	if (it != _shaders.end())
	{
		_shaders.erase(it);
	}
}

GraphicsShaderPtr
MaterialDesc::getShader(const std::string& name) const noexcept
{
	assert(!name.empty());
	for (auto& it : _shaders)
	{
		if (it->getGraphicsShaderDesc().getName() == name)
			return it;
	}

	return nullptr;
}

GraphicsShaders&
MaterialDesc::getShaders() noexcept
{
	return _shaders;
}

const GraphicsShaders&
MaterialDesc::getShaders() const noexcept
{
	return _shaders;
}

_NAME_END