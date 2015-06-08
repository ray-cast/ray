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
#ifndef _H_MATERIAL_MAKETER_H_
#define _H_MATERIAL_MAKETER_H_

#include <ray/material.h>
#include <ray/xmlreader.h>

_NAME_BEGIN

EXPORT TexturePtr createTexture(const std::string& name) noexcept;

class EXPORT MaterialMaker final
{
public:
    MaterialMaker() noexcept;
    MaterialMaker(const std::string& filename) except;
    ~MaterialMaker() noexcept;

    MaterialPtr load(const std::string& filename) except;
    MaterialPtr load(XMLReader& reader) except;

    operator MaterialPtr() noexcept;

private:

    ShaderPtr instanceShader(XMLReader& reader) noexcept;
    RenderStatePtr instanceState(XMLReader& reader) noexcept;
    MaterialPassPtr instancePass(XMLReader& reader) noexcept;
    MaterialTechPtr instanceTech(XMLReader& reader) noexcept;
    ShaderParamPtr instanceParameter(XMLReader& reader) noexcept;

private:
    MaterialMaker(const MaterialMaker&) noexcept = delete;
    MaterialMaker& operator=(const MaterialMaker&) noexcept = delete;

private:

    MaterialPtr _material;

    std::string _codes;

    std::map<std::string, ShaderPtr> _shaderList;
};

_NAME_END

#endif