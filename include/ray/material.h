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
#ifndef _H_METERIAL_H_
#define _H_METERIAL_H_

#include <ray/material_pass.h>
#include <ray/material_tech.h>
#include <ray/material_param.h>
#include <ray/material_variant.h>

_NAME_BEGIN

class EXPORT Material final : public rtti::Interface
{
	__DeclareSubClass(Material, rtti::Interface)
public:
	Material() noexcept;
	~Material() noexcept;

	bool setup() noexcept;
	void close() noexcept;

	void addTech(MaterialTechPtr& technique) noexcept;
	void addTech(MaterialTechPtr&& technique) noexcept;
	void removeTech(MaterialTechPtr& technique) noexcept;
	void removeTech(MaterialTechPtr&& technique) noexcept;
	MaterialTechPtr getTech(const std::string& name) noexcept;
	const MaterialTechniques& getTechs() const noexcept;

	void addParameter(MaterialParamPtr& parameter) noexcept;
	void addParameter(MaterialParamPtr&& parameter) noexcept;
	void removeParameter(MaterialParamPtr& parameter) noexcept;
	void removeParameter(MaterialParamPtr&& parameter) noexcept;
	MaterialParamPtr getParameter(const std::string& name) const noexcept;
	MaterialParams& getParameters() noexcept;
	const MaterialParams& getParameters() const noexcept;

	void addMacro(MaterialParamPtr& macro) noexcept;
	void addMacro(MaterialParamPtr&& macro) noexcept;
	void removeMacro(MaterialParamPtr& macro) noexcept;
	void removeMacro(MaterialParamPtr&& macro) noexcept;
	MaterialParamPtr getMacro(const std::string& name) const noexcept;
	MaterialParams& getMacros() noexcept;
	const MaterialParams& getMacros() const noexcept;

	MaterialPtr clone() const noexcept;

private:
	Material(const Material&) noexcept = delete;
	Material& operator=(const Material&) noexcept = delete;

private:
	MaterialParams _parameters;
	MaterialParams _macros;
	MaterialTechniques _techniques;
};

_NAME_END

#endif