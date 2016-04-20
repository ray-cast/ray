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
#ifndef _H_MATERIAL_DESC_H_
#define _H_MATERIAL_DESC_H_

#include <ray/render_types.h>

_NAME_BEGIN

class EXPORT MaterialDesc final
{
public:
	MaterialDesc() noexcept;
	~MaterialDesc() noexcept;

	void addTech(MaterialTechPtr technique) noexcept;
	void removeTech(MaterialTechPtr technique) noexcept;
	MaterialTechPtr getTech(const std::string& name) noexcept;
	MaterialTechniques& getTechs() noexcept;

	void addParameter(MaterialParamPtr parameter) noexcept;
	void removeParameter(MaterialParamPtr parameter) noexcept;
	MaterialParamPtr getParameter(const std::string& name) const noexcept;
	MaterialParams& getParameters() noexcept;
	const MaterialParams& getParameters() const noexcept;

	void addMacro(MaterialParamPtr macro) noexcept;
	void removeMacro(MaterialParamPtr macro) noexcept;
	MaterialParamPtr getMacro(const std::string& name) const noexcept;
	MaterialParams& getMacros() noexcept;
	const MaterialParams& getMacros() const noexcept;

private:
	MaterialParams _parameters;
	MaterialParams _macros;
	MaterialTechniques _techniques;
};

_NAME_END

#endif