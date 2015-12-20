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
#include <ray/material_manager.h>
#include <ray/material_maker.h>

_NAME_BEGIN

MaterialManager::MaterialManager() noexcept
{
}

MaterialManager::~MaterialManager() noexcept
{
	this->close();
}

void
MaterialManager::open(GraphicsDevicePtr device) noexcept
{
	_semantics.clear();
	_graphicsDevice = device;
}

void
MaterialManager::close() noexcept
{
	_semantics.clear();
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

MaterialSemanticPtr
MaterialManager::createSemantic(const std::string& name, ShaderVariantType type) noexcept
{
	if (!name.empty() && type != ShaderVariantType::None)
	{
		auto it = std::find_if(_semantics.begin(), _semantics.end(), [&](MaterialSemanticPtr& it) { return it->getName() == name;});
		if (it == _semantics.end())
		{
			_semantics.push_back(std::make_shared<MaterialSemantic>(name, type));
			return _semantics.back();
		}
	}

	return nullptr;
}

void
MaterialManager::addSemantic(MaterialSemanticPtr semantc) noexcept
{
	if (semantc && !semantc->getName().empty() && semantc->getType() != ShaderVariantType::None)
	{
		auto it = std::find_if(_semantics.begin(), _semantics.end(), [&](MaterialSemanticPtr& it) { return it->getName() == semantc->getName();});
		if (it == _semantics.end())
			_semantics.push_back(semantc);
	}
}

void
MaterialManager::removeSemantic(MaterialSemanticPtr semantc) noexcept
{
	if (semantc && !semantc->getName().empty() && semantc->getType() != ShaderVariantType::None)
	{
		auto it = std::find_if(_semantics.begin(), _semantics.end(), [&](MaterialSemanticPtr& it) { return it->getName() == semantc->getName();});
		if (it == _semantics.end())
			_semantics.push_back(semantc);
	}
}

MaterialSemanticPtr
MaterialManager::getSemantic(const std::string& name) noexcept
{
	if (!name.empty())
	{
		auto it = std::find_if(_semantics.begin(), _semantics.end(), [&](MaterialSemanticPtr it) { return it->getName() == name;});
		if (it != _semantics.end())
			return *it;
	}
	return nullptr;
}

MaterialPtr
MaterialManager::createMaterial(const std::string& name) except
{
	auto& material = _materials[name];
	if (!material)
	{
		MaterialMaker maker;
		_materials[name] = maker.load(*this, name);
	}

	return material;
}

MaterialPtr
MaterialManager::getMaterial(const std::string& name) noexcept
{
	return _materials[name];
}

void
MaterialManager::setMaterialPass(MaterialPassPtr& pass) noexcept
{
	auto& semantics = pass->getParameters();
	for (auto& it : semantics)
	{
		auto semantic = it->getSemantic();
		if (semantic)
		{
			auto type = semantic->getType();
			switch (type)
			{
			case ray::ShaderVariantType::Bool:
				it->assign(semantic->getBool());
				break;
			case ray::ShaderVariantType::Int:
				it->assign(semantic->getInt());
				break;
			case ray::ShaderVariantType::Int2:
				it->assign(semantic->getInt2());
				break;
			case ray::ShaderVariantType::Float:
				it->assign(semantic->getFloat());
				break;
			case ray::ShaderVariantType::Float2:
				it->assign(semantic->getFloat2());
				break;
			case ray::ShaderVariantType::Float3:
				it->assign(semantic->getFloat3());
				break;
			case ray::ShaderVariantType::Float4:
				it->assign(semantic->getFloat4());
				break;
			case ray::ShaderVariantType::Float3x3:
				it->assign(semantic->getFloat3x3());
				break;
			case ray::ShaderVariantType::Float4x4:
				it->assign(semantic->getFloat4x4());
				break;
			case ray::ShaderVariantType::FloatArray:
				it->assign(semantic->getFloatArray());
				break;
			case ray::ShaderVariantType::Float2Array:
				it->assign(semantic->getFloat2Array());
				break;
			case ray::ShaderVariantType::Float3Array:
				it->assign(semantic->getFloat3Array());
				break;
			case ray::ShaderVariantType::Float4Array:
				it->assign(semantic->getFloat4Array());
				break;
			case ray::ShaderVariantType::Texture:
				it->assign(semantic->getTexture());
				break;
			default:
				break;
			}
		}
	}

	_material = pass;
}

MaterialPassPtr&
MaterialManager::getMaterialPass() noexcept
{
	return _material;
}

const MaterialPassPtr&
MaterialManager::getMaterialPass() const noexcept
{
	return _material;
}

_NAME_END