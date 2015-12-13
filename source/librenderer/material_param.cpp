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
#include <ray/material_param.h>

_NAME_BEGIN

MaterialParam::MaterialParam() noexcept
	: _semantic(MaterialSemantic::NotSemantic)
{
}

MaterialParam::MaterialParam(const std::string& name, float value, MaterialSemantic semantic) noexcept
	: ShaderVariant(name, value)
	, _semantic(semantic)
{
}

MaterialParam::MaterialParam(const std::string& name, const float3& value, MaterialSemantic semantic) noexcept
	: ShaderVariant(name, value)
	, _semantic(semantic)
{
}

MaterialParam::MaterialParam(const std::string& name, const Vector4& value, MaterialSemantic semantic) noexcept
	: ShaderVariant(name, value)
	, _semantic(semantic)
{
}

MaterialParam::MaterialParam(const std::string& name, const Matrix4x4& value, MaterialSemantic semantic) noexcept
	: ShaderVariant(name, value)
	, _semantic(semantic)
{
}

MaterialParam::MaterialParam(const std::string& name, ShaderVariantType type, MaterialSemantic semantic) noexcept
	: ShaderVariant(name, type)
	, _semantic(semantic)
{
}

MaterialParam::~MaterialParam() noexcept
{
}

void
MaterialParam::setSemantic(MaterialSemantic semantic) noexcept
{
	if (_semantic != semantic)
	{
		if (semantic == MaterialSemantic::matModel ||
			semantic == MaterialSemantic::matModelInverse ||
			semantic == MaterialSemantic::matModelInverseTranspose ||
			semantic == MaterialSemantic::matProject ||
			semantic == MaterialSemantic::matProjectInverse ||
			semantic == MaterialSemantic::matView ||
			semantic == MaterialSemantic::matViewInverse ||
			semantic == MaterialSemantic::matViewInverseTranspose ||
			semantic == MaterialSemantic::matViewProject ||
			semantic == MaterialSemantic::matViewProjectInverse)
		{
			this->setType(ShaderVariantType::SPT_FLOAT4X4);
		}
		else if (semantic == MaterialSemantic::CameraAperture ||
			semantic == MaterialSemantic::CameraNear ||
			semantic == MaterialSemantic::CameraFar)
		{
			this->setType(ShaderVariantType::SPT_FLOAT);
		}
		else if (semantic == MaterialSemantic::CameraView ||
			semantic == MaterialSemantic::CameraPosition ||
			semantic == MaterialSemantic::CameraDirection)
		{
			this->setType(ShaderVariantType::SPT_FLOAT3);
		}
		else if (semantic == MaterialSemantic::DeferredDepthLinearMap ||
			semantic == MaterialSemantic::DeferredDepthMap ||
			semantic == MaterialSemantic::DeferredGraphicMap ||
			semantic == MaterialSemantic::DeferredNormalMap ||
			semantic == MaterialSemantic::DeferredLightMap ||
			semantic == MaterialSemantic::DeferredShadowMap ||
			semantic == MaterialSemantic::DepthMap ||
			semantic == MaterialSemantic::ColorMap ||
			semantic == MaterialSemantic::NormalMap)
		{
			this->setType(ShaderVariantType::SPT_TEXTURE);
		}
		else
		{
			assert(false);
		}

		_semantic = semantic;
	}
}

MaterialSemantic
MaterialParam::getSemantic() const noexcept
{
	return _semantic;
}

void
MaterialParam::addShaderUniform(ShaderUniformPtr uniform) noexcept
{
	assert(std::find(_uniforms.begin(), _uniforms.end(), uniform) == _uniforms.end());
	_uniforms.push_back(uniform);
}

void
MaterialParam::removeShaderUniform(ShaderUniformPtr uniform) noexcept
{
	auto it = std::find(_uniforms.begin(), _uniforms.end(), uniform);
	if (it != _uniforms.end())
	{
		_uniforms.erase(it);
	}
}

void
MaterialParam::onChangeBefore() noexcept
{
}

void
MaterialParam::onChangeAfter() noexcept
{
	for (auto& it : _uniforms)
		it->needUpdate(true);
}

_NAME_END