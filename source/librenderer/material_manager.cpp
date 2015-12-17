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
	this->setup();
}

MaterialManager::~MaterialManager() noexcept
{
	this->close();
}

void
MaterialManager::setup() noexcept
{
	_semantics.resize(MaterialSemantic::NumSemantic);

	_semantics[matModel] = std::make_shared<MaterialSemanticParam>("matModel", ShaderVariantType::Float4x4);
	_semantics[matModelInverse] = std::make_shared<MaterialSemanticParam>("matModelInverse", ShaderVariantType::Float4x4);
	_semantics[matModelInverseTranspose] = std::make_shared<MaterialSemanticParam>("matModelInverseTranspose", ShaderVariantType::Float4x4);
	_semantics[matProject] = std::make_shared<MaterialSemanticParam>("matProject", ShaderVariantType::Float4x4);
	_semantics[matProjectInverse] = std::make_shared<MaterialSemanticParam>("matProjectInverse", ShaderVariantType::Float4x4);
	_semantics[matView] = std::make_shared<MaterialSemanticParam>("matView", ShaderVariantType::Float4x4);
	_semantics[matViewInverse] = std::make_shared<MaterialSemanticParam>("matViewInverse", ShaderVariantType::Float4x4);
	_semantics[matViewInverseTranspose] = std::make_shared<MaterialSemanticParam>("matViewInverseTranspose", ShaderVariantType::Float4x4);
	_semantics[matViewProject] = std::make_shared<MaterialSemanticParam>("matViewProject", ShaderVariantType::Float4x4);
	_semantics[matViewProjectInverse] = std::make_shared<MaterialSemanticParam>("matViewProjectInverse", ShaderVariantType::Float4x4);

	_semantics[CameraAperture] = std::make_shared<MaterialSemanticParam>("CameraAperture", ShaderVariantType::Float);
	_semantics[CameraFar] = std::make_shared<MaterialSemanticParam>("CameraFar", ShaderVariantType::Float);
	_semantics[CameraNear] = std::make_shared<MaterialSemanticParam>("CameraNear", ShaderVariantType::Float);
	_semantics[CameraView] = std::make_shared<MaterialSemanticParam>("CameraView", ShaderVariantType::Float3);
	_semantics[CameraPosition] = std::make_shared<MaterialSemanticParam>("CameraPosition", ShaderVariantType::Float3);
	_semantics[CameraDirection] = std::make_shared<MaterialSemanticParam>("CameraDirection", ShaderVariantType::Float3);

	_semantics[DepthMap] = std::make_shared<MaterialSemanticParam>("DepthMap", ShaderVariantType::Texture);
	_semantics[ColorMap] = std::make_shared<MaterialSemanticParam>("ColorMap", ShaderVariantType::Texture);
	_semantics[NormalMap] = std::make_shared<MaterialSemanticParam>("NormalMap", ShaderVariantType::Texture);

	_semantics[DeferredDepthMap] = std::make_shared<MaterialSemanticParam>("DeferredDepthMap", ShaderVariantType::Texture);
	_semantics[DeferredDepthLinearMap] = std::make_shared<MaterialSemanticParam>("DeferredDepthLinearMap", ShaderVariantType::Texture);
	_semantics[DeferredGraphicMap] = std::make_shared<MaterialSemanticParam>("DeferredGraphicMap", ShaderVariantType::Texture);
	_semantics[DeferredNormalMap] = std::make_shared<MaterialSemanticParam>("DeferredNormalMap", ShaderVariantType::Texture);
	_semantics[DeferredLightMap] = std::make_shared<MaterialSemanticParam>("DeferredLightMap", ShaderVariantType::Texture);
	_semantics[DeferredShadowMap] = std::make_shared<MaterialSemanticParam>("DeferredShadowMap", ShaderVariantType::Texture);
}

void
MaterialManager::close() noexcept
{
	_semantics.clear();
}

void
MaterialManager::setMatrixParam(MaterialSemantic index, const Matrix4x4& m) noexcept
{
	_semantics[index]->assign(m);
}

const Matrix4x4&
MaterialManager::getMatrixParam(MaterialSemantic index) noexcept
{
	return _semantics[index]->getFloat4x4();
}

std::string
MaterialManager::getMatrixParmName(MaterialSemantic index) const noexcept
{
	return _semantics[index]->getName();
}

MaterialSemantic
MaterialManager::getMatrixParamSemantic(const std::string& name) const noexcept
{
	if (getMatrixParmName(MaterialSemantic::matModel) == name)
		return MaterialSemantic::matModel;
	if (getMatrixParmName(MaterialSemantic::matModelInverse) == name)
		return  MaterialSemantic::matModelInverse;
	if (getMatrixParmName(MaterialSemantic::matModelInverseTranspose) == name)
		return MaterialSemantic::matModelInverseTranspose;
	if (getMatrixParmName(MaterialSemantic::matView) == name)
		return MaterialSemantic::matView;
	if (getMatrixParmName(MaterialSemantic::matViewInverse) == name)
		return MaterialSemantic::matViewInverse;
	if (getMatrixParmName(MaterialSemantic::matViewInverseTranspose) == name)
		return MaterialSemantic::matViewInverseTranspose;
	if (getMatrixParmName(MaterialSemantic::matProject) == name)
		return MaterialSemantic::matProject;
	if (getMatrixParmName(MaterialSemantic::matProjectInverse) == name)
		return MaterialSemantic::matProjectInverse;
	if (getMatrixParmName(MaterialSemantic::matViewProject) == name)
		return MaterialSemantic::matViewProject;
	if (getMatrixParmName(MaterialSemantic::matViewProjectInverse) == name)
		return MaterialSemantic::matViewProjectInverse;

	return NotSemantic;
}

void
MaterialManager::setFloatParam(MaterialSemantic index, float v) noexcept
{
	_semantics[index]->assign(v);
}

float
MaterialManager::getFloatParam(MaterialSemantic index) noexcept
{
	return _semantics[index]->getFloat();
}

std::string
MaterialManager::getFloatParmName(MaterialSemantic index) const noexcept
{
	return _semantics[index]->getName();
}

MaterialSemantic
MaterialManager::getFloatParamSemantic(const std::string& name) const noexcept
{
	if (getFloatParmName(MaterialSemantic::CameraAperture) == name)
		return MaterialSemantic::CameraAperture;
	if (getFloatParmName(MaterialSemantic::CameraFar) == name)
		return MaterialSemantic::CameraFar;
	if (getFloatParmName(MaterialSemantic::CameraNear) == name)
		return MaterialSemantic::CameraNear;

	return MaterialSemantic::NotSemantic;
}

void
MaterialManager::setFloat3Param(MaterialSemantic index, const float3& v) noexcept
{
	_semantics[index]->assign(v);
}

const Vector3&
MaterialManager::getFloat3Param(MaterialSemantic index) noexcept
{
	return _semantics[index]->getFloat3();
}

std::string
MaterialManager::getFloat3ParmName(MaterialSemantic index) const noexcept
{
	return _semantics[index]->getName();
}

MaterialSemantic
MaterialManager::getFloat3ParamSemantic(const std::string& name) const noexcept
{
	if (getFloat3ParmName(MaterialSemantic::CameraView) == name)
		return MaterialSemantic::CameraView;
	if (getFloat3ParmName(MaterialSemantic::CameraPosition) == name)
		return MaterialSemantic::CameraPosition;
	if (getFloat3ParmName(MaterialSemantic::CameraDirection) == name)
		return MaterialSemantic::CameraDirection;
	return MaterialSemantic::NotSemantic;
}

void
MaterialManager::setFloat4Param(MaterialSemantic index, const float4& v) noexcept
{
	_semantics[index]->assign(v);
}

const Vector4&
MaterialManager::getFloat4Param(MaterialSemantic index) noexcept
{
	return _semantics[index]->getFloat4();
}

std::string
MaterialManager::getFloat4ParmName(MaterialSemantic index) const noexcept
{
	return _semantics[index]->getName();
}

MaterialSemantic
MaterialManager::getFloat4ParamSemantic(const std::string& name) const noexcept
{
	return MaterialSemantic::NotSemantic;
}

void
MaterialManager::setTexParam(MaterialSemantic index, TexturePtr texture) noexcept
{
	_semantics[index]->assign(texture);
}

TexturePtr
MaterialManager::getTexParam(MaterialSemantic index) noexcept
{
	return _semantics[index]->getTexture();
}

std::string
MaterialManager::getTexParmName(MaterialSemantic index) const noexcept
{
	return _semantics[index]->getName();
}

MaterialSemantic
MaterialManager::getTexParamSemantic(const std::string& name) const noexcept
{
	if (getTexParmName(MaterialSemantic::DepthMap) == name)
		return MaterialSemantic::DepthMap;
	if (getTexParmName(MaterialSemantic::ColorMap) == name)
		return MaterialSemantic::ColorMap;
	if (getTexParmName(MaterialSemantic::DeferredDepthMap) == name)
		return MaterialSemantic::DeferredDepthMap;
	if (getTexParmName(MaterialSemantic::DeferredGraphicMap) == name)
		return MaterialSemantic::DeferredGraphicMap;
	if (getTexParmName(MaterialSemantic::DeferredNormalMap) == name)
		return MaterialSemantic::DeferredNormalMap;
	if (getTexParmName(MaterialSemantic::DeferredLightMap) == name)
		return MaterialSemantic::DeferredLightMap;
	return (MaterialSemantic)NotSemantic;
}

MaterialSemanticParamPtr
MaterialManager::getParamPointer(MaterialSemantic semantic) const noexcept
{
	assert(semantic != MaterialSemantic::NotSemantic);
	assert(semantic < MaterialSemantic::NumSemantic);
	return _semantics[semantic];
}

MaterialSemanticParamPtr
MaterialManager::getParamPointer(const std::string& name) const noexcept
{
	for (auto& it : _semantics)
	{
		if (it->getName() == name)
		{
			return it;
		}
	}

	return nullptr;
}

MaterialPtr
MaterialManager::createMaterial(const std::string& name) except
{
	MaterialMaker maker;
	return maker.load(name);
}

void
MaterialManager::setMaterialPass(MaterialPassPtr& pass) noexcept
{
	auto& semantics = pass->getParameters();
	for (auto& it : semantics)
	{
		auto semantic = it->getSemantic();
		if (semantic == MaterialSemantic::NotSemantic)
			continue;
		else if (semantic == MaterialSemantic::NotSemantic)
			continue;
		else if (semantic == MaterialSemantic::matModel)
			it->assign(this->getMatrixParam(matModel));
		else if (semantic == MaterialSemantic::matModelInverse)
			it->assign(this->getMatrixParam(matModelInverse));
		else if (semantic == MaterialSemantic::matModelInverseTranspose)
			it->assign(this->getMatrixParam(matModelInverseTranspose));
		else if (semantic == MaterialSemantic::matView)
			it->assign(this->getMatrixParam(MaterialSemantic::matView));
		else if (semantic == MaterialSemantic::matViewInverse)
			it->assign(this->getMatrixParam(MaterialSemantic::matViewInverse));
		else if (semantic == MaterialSemantic::matViewInverseTranspose)
			it->assign(this->getMatrixParam(MaterialSemantic::matViewInverseTranspose));
		else if (semantic == MaterialSemantic::matProject)
			it->assign(this->getMatrixParam(MaterialSemantic::matProject));
		else if (semantic == MaterialSemantic::matProjectInverse)
			it->assign(this->getMatrixParam(MaterialSemantic::matProjectInverse));
		else if (semantic == MaterialSemantic::matViewProject)
			it->assign(this->getMatrixParam(MaterialSemantic::matViewProject));
		else if (semantic == MaterialSemantic::matViewProjectInverse)
			it->assign(this->getMatrixParam(MaterialSemantic::matViewProjectInverse));
		else if (semantic == MaterialSemantic::DeferredDepthLinearMap)
			it->assign(this->getTexParam(MaterialSemantic::DeferredDepthLinearMap));
		else if (semantic == MaterialSemantic::DeferredDepthMap)
			it->assign(this->getTexParam(MaterialSemantic::DeferredDepthMap));
		else if (semantic == MaterialSemantic::DeferredGraphicMap)
			it->assign(this->getTexParam(MaterialSemantic::DeferredGraphicMap));
		else if (semantic == MaterialSemantic::DeferredLightMap)
			it->assign(this->getTexParam(MaterialSemantic::DeferredLightMap));
		else if (semantic == MaterialSemantic::DeferredNormalMap)
			it->assign(this->getTexParam(MaterialSemantic::DeferredNormalMap));
		else if (semantic == MaterialSemantic::DeferredShadowMap)
			it->assign(this->getTexParam(MaterialSemantic::DeferredShadowMap));
		else if (semantic == MaterialSemantic::DepthMap)
			it->assign(this->getTexParam(MaterialSemantic::DepthMap));
		else if (semantic == MaterialSemantic::NormalMap)
			it->assign(this->getTexParam(MaterialSemantic::NormalMap));
		else if (semantic == MaterialSemantic::ColorMap)
			it->assign(this->getTexParam(MaterialSemantic::ColorMap));
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