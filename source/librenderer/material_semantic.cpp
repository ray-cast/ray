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
#include <ray/material_semantic.h>
#include <ray/material_param.h>

_NAME_BEGIN

MaterialSemantic::MaterialSemantic() noexcept
{
}

MaterialSemantic::~MaterialSemantic() noexcept
{
	this->close();
}

bool 
MaterialSemantic::setup() noexcept
{
	_parametes[GlobalSemanticType::GlobalSemanticTypeModel] = std::make_shared<MaterialParam>("matModel", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeModelInverse] = std::make_shared<MaterialParam>("matModelInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeView] = std::make_shared<MaterialParam>("matView", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeViewInverse] = std::make_shared<MaterialParam>("matViewInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeProject] = std::make_shared<MaterialParam>("matProject", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeProjectInverse] = std::make_shared<MaterialParam>("matProjectInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeViewProject] = std::make_shared<MaterialParam>("matViewProject", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeViewProjectInverse] = std::make_shared<MaterialParam>("matViewProjectInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeModelView] = std::make_shared<MaterialParam>("matModelView", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeModelViewProject] = std::make_shared<MaterialParam>("matModelViewProject", GraphicsUniformType::GraphicsUniformTypeFloat4x4);

	_parametes[GlobalSemanticType::GlobalSemanticTypeCameraAperture] = std::make_shared<MaterialParam>("CameraAperture", GraphicsUniformType::GraphicsUniformTypeFloat);
	_parametes[GlobalSemanticType::GlobalSemanticTypeCameraNear] = std::make_shared<MaterialParam>("CameraNear", GraphicsUniformType::GraphicsUniformTypeFloat);
	_parametes[GlobalSemanticType::GlobalSemanticTypeCameraFar] = std::make_shared<MaterialParam>("CameraFar", GraphicsUniformType::GraphicsUniformTypeFloat);
	_parametes[GlobalSemanticType::GlobalSemanticTypeCameraPosition] = std::make_shared<MaterialParam>("CameraPosition", GraphicsUniformType::GraphicsUniformTypeFloat3);
	_parametes[GlobalSemanticType::GlobalSemanticTypeCameraDirection] = std::make_shared<MaterialParam>("CameraDirection", GraphicsUniformType::GraphicsUniformTypeFloat3);

	_parametes[GlobalSemanticType::GlobalSemanticTypeDepthTexture] = std::make_shared<MaterialParam>("DepthTexture", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_parametes[GlobalSemanticType::GlobalSemanticTypeDepthLinearTexture] = std::make_shared<MaterialParam>("DepthLinearTexture", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_parametes[GlobalSemanticType::GlobalSemanticTypeDiffuseTexture] = std::make_shared<MaterialParam>("DiffuseTexture", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_parametes[GlobalSemanticType::GlobalSemanticTypeNormalTexture] = std::make_shared<MaterialParam>("NormalTexture", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_parametes[GlobalSemanticType::GlobalSemanticTypeLightingTexture] = std::make_shared<MaterialParam>("LightingTexture", GraphicsUniformType::GraphicsUniformTypeStorageImage);

	return true;
}

void 
MaterialSemantic::close() noexcept
{
	_parametes->reset();
}

const MaterialParamPtr&
MaterialSemantic::getSemantic(GlobalSemanticType type) const noexcept
{
	return _parametes[type];
}

_NAME_END