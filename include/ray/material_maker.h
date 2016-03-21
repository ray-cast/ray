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
#ifndef _H_MATERIAL_MAKETER_H_
#define _H_MATERIAL_MAKETER_H_

#include <ray/material_loader.h>
#include <ray/iarchive.h>
#include <ray/material.h>

_NAME_BEGIN

class EXPORT MaterialMaker final : public MaterialLoader
{
public:
	MaterialMaker() noexcept;
	~MaterialMaker() noexcept;

	MaterialPtr load(MaterialManager& manager, iarchive& reader) except;
	MaterialPtr load(MaterialManager& manager, const std::string& filename) noexcept;
	bool load(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except;

private:

	void instancePass(MaterialManager& manager, MaterialTechPtr& tech, iarchive& reader) except;
	void instanceTech(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except;
	void instanceSampler(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except;
	void instanceParameter(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except;
	void instanceMacro(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except;
	void instanceBuffer(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except;
	void instanceCodes(MaterialManager& manager, iarchive& reader) except;
	void instanceShader(MaterialManager& manager, GraphicsProgramDesc& program, iarchive& reader) except;
	void instanceInclude(MaterialManager& manager, MaterialDesc& material, iarchive& reader) except;

	static GraphicsVertexType stringToPrimitive(const std::string& primitive) noexcept;
	static GraphicsCullMode stringToCullMode(const std::string& cullmode) noexcept;
	static GraphicsPolygonMode stringToFillMode(const std::string& fillmode) noexcept;
	static GraphicsBlendOp stringToBlendOperation(const std::string& blendop) noexcept;
	static GraphicsBlendFactor stringToBlendFactor(const std::string& factor) noexcept;
	static GraphicsColorMask stringToColorMask(const std::string& mask) noexcept;
	static GraphicsCompareFunc stringToCompareFunc(const std::string& func) noexcept;
	static GraphicsStencilOp stringToStencilOp(const std::string& stencilop) noexcept;

private:
	MaterialMaker(const MaterialMaker&) noexcept = delete;
	MaterialMaker& operator=(const MaterialMaker&) noexcept = delete;

private:

	std::map<std::string, std::vector<char>> _shaderCodes;
};

_NAME_END

#endif