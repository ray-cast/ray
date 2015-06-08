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
#include <ray/material_pass.h>
#include <ray/material.h>

_NAME_BEGIN

MaterialPass::MaterialPass(RenderPass pass) noexcept
    :_pass(pass)
{
    _renderState = std::make_shared<RenderState>();
}

MaterialPass::~MaterialPass() noexcept
{
}

void
MaterialPass::setup(const Material& material) noexcept
{
    assert(_shaderObject);

    if (_shaderObject->setup())
    {
        _paramBindings = std::make_shared<MaterialParmBinds>();
        _paramBindings->_buildUniformBinds(*material.getMaterialSemantic(), material.getParameters(), _shaderObject->getActiveUniforms());
    }
}

void
MaterialPass::setName(const std::string& name) noexcept
{
    _name = name;
}

const std::string&
MaterialPass::getName() const noexcept
{
    return _name;
}

void
MaterialPass::setShaderObject(ShaderObjectPtr shader) noexcept
{
    _shaderObject = shader;
}

RenderPass
MaterialPass::getRenderPass() noexcept
{
    return _pass;
}

ShaderObjectPtr
MaterialPass::getShaderObject() noexcept
{
    return _shaderObject;
}

RenderStatePtr
MaterialPass::getRenderState() noexcept
{
    return _renderState;
}

ShaderConstantBufferPtr
MaterialPass::getShaderConstantBuffer() noexcept
{
    return _paramBindings->getConstantBuffer();
}

_NAME_END