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
#ifndef _H_MATERIAL_MANAGER_H_
#define _H_MATERIAL_MANAGER_H_

#include <ray/material.h>

_NAME_BEGIN

class EXPORT MaterialManager final
{
public:
	MaterialManager() noexcept;
	~MaterialManager() noexcept;

	void setup() noexcept;
	void close() noexcept;

	void setMatrixParam(MaterialSemantic index, const Matrix4x4& m) noexcept;
	const Matrix4x4& getMatrixParam(MaterialSemantic index) noexcept;
	std::string getMatrixParmName(MaterialSemantic index) const noexcept;
	MaterialSemantic getMatrixParamSemantic(const std::string& name) const noexcept;

	void setFloatParam(MaterialSemantic index, float v) noexcept;
	float getFloatParam(MaterialSemantic index) noexcept;
	std::string getFloatParmName(MaterialSemantic index) const noexcept;
	MaterialSemantic getFloatParamSemantic(const std::string& name) const noexcept;

	void setFloat3Param(MaterialSemantic index, const float3& v) noexcept;
	const Vector3& getFloat3Param(MaterialSemantic index) noexcept;
	std::string getFloat3ParmName(MaterialSemantic index) const noexcept;
	MaterialSemantic getFloat3ParamSemantic(const std::string& name) const noexcept;

	void setFloat4Param(MaterialSemantic index, const float4& v) noexcept;
	const Vector4& getFloat4Param(MaterialSemantic index) noexcept;
	std::string getFloat4ParmName(MaterialSemantic index) const noexcept;
	MaterialSemantic getFloat4ParamSemantic(const std::string& name) const noexcept;

	void setTexParam(MaterialSemantic index, TexturePtr v) noexcept;
	TexturePtr getTexParam(MaterialSemantic index) noexcept;
	std::string getTexParmName(MaterialSemantic index) const noexcept;
	MaterialSemantic getTexParamSemantic(const std::string& name) const noexcept;

	MaterialParamSemanticPtr getParamPointer(MaterialSemantic semantic) const noexcept;
	MaterialParamSemanticPtr getParamPointer(const std::string& name) const noexcept;

	MaterialPtr createMaterial(const std::string& name) except;

	void setMaterialPass(MaterialPassPtr& pass) noexcept;
	MaterialPassPtr& getMaterialPass() noexcept;
	const MaterialPassPtr& getMaterialPass() const noexcept;

private:

	MaterialPassPtr _material;
	MaterialParamSemantics _semantics;
};

_NAME_END

#endif