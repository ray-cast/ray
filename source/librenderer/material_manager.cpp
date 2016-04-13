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
#include <ray/material_manager.h>
#include <ray/material_maker.h>
#include <ray/material_param.h>
#include <ray/material_pass.h>

_NAME_BEGIN

MaterialManager::MaterialManager() noexcept
{
}

MaterialManager::~MaterialManager() noexcept
{
	this->close();
}

bool
MaterialManager::setup(GraphicsDevicePtr device) noexcept
{
	_graphicsDevice = device;
	_materialLoader = std::make_shared<MaterialMaker>();
	return true;
}

void
MaterialManager::close() noexcept
{
	_semantics.clear();
}

void
MaterialManager::setMaterialLoader(MaterialLoaderPtr loader) noexcept
{
	assert(loader);
	_materialLoader = loader;
}

MaterialLoaderPtr
MaterialManager::getMaterialLoader() const noexcept
{
	return _materialLoader;
}

void
MaterialManager::setGraphicsDevice(GraphicsDevicePtr device) noexcept
{
	_graphicsDevice = device;
}

GraphicsDevicePtr
MaterialManager::getGraphicsDevice() noexcept
{
	return _graphicsDevice;
}

MaterialVariantPtr
MaterialManager::createSemantic(const std::string& name, GraphicsUniformType type) noexcept
{
	assert(!name.empty());
	assert(type != GraphicsUniformType::GraphicsUniformTypeNone);

	auto it = std::find_if(_semantics.begin(), _semantics.end(), [&](MaterialVariantPtr& it) { return it->getName() == name;});
	if (it == _semantics.end())
	{
		_semantics.push_back(std::make_shared<MaterialVariant>(name, type));
		return _semantics.back();
	}

	return nullptr;
}

void
MaterialManager::destroySemantic(MaterialVariantPtr semantc) noexcept
{
	if (semantc && !semantc->getName().empty() && semantc->getType() != GraphicsUniformType::GraphicsUniformTypeNone)
	{
		auto it = std::find_if(_semantics.begin(), _semantics.end(), [&](MaterialVariantPtr& it) { return it->getName() == semantc->getName();});
		if (it == _semantics.end())
			_semantics.push_back(semantc);
	}
}

MaterialVariantPtr
MaterialManager::getSemantic(const std::string& name) noexcept
{
	if (!name.empty())
	{
		auto it = std::find_if(_semantics.begin(), _semantics.end(), [&](MaterialVariantPtr it) { return it->getName() == name;});
		if (it != _semantics.end())
			return *it;
	}
	return nullptr;
}

MaterialPtr
MaterialManager::createMaterial(const std::string& name) noexcept
{
	auto& material = _materials[name];
	if (!material)
		_materials[name] = _materialLoader->load(*this, name);
	return material;
}

MaterialPtr
MaterialManager::getMaterial(const std::string& name) noexcept
{
	return _materials[name];
}

void
MaterialManager::destroyMaterial(MaterialPtr material) noexcept
{
	_materials[material->getName()] = nullptr;
}

_NAME_END