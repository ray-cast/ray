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
#include <ray/material_manager.h>
#include <ray/render_system.h>

_NAME_BEGIN

MaterialPass::MaterialPass(RenderPass pass) noexcept
	:_pass(pass)
{
}

MaterialPass::~MaterialPass() noexcept
{
	this->close();
}

void 
MaterialPass::setup(Material& material) except
{
	assert(_shaderObject);

	if (!_renderState)
	{
		_renderState = RenderSystem::instance()->createRenderState();
	}

	if (_shaderObject->setup())
	{
		auto& uniforms = _shaderObject->getActiveUniforms();
		for (auto& uniform : uniforms)
		{
			auto param = material.getParameter(uniform->getName());
			if (param)
			{
				param->addShaderUniform(uniform);
				_parameters.push_back(param);
			}
		}
	}
}

void
MaterialPass::close() noexcept
{
	if (_shaderObject)
	{
		auto uniforms = _shaderObject->getActiveUniforms();
		for (auto& it : uniforms)
		{
			auto param = this->getParameter(it->getName());
			if (param)
			{
				param->removeShaderUniform(it);
			}
		}

		_shaderObject->close();
		_shaderObject = nullptr;
	}

	if (_renderState)
	{
		_renderState->close();
		_renderState = nullptr;
	}

	_parameters.clear();
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

RenderPass
MaterialPass::getRenderPass() const noexcept
{
	return _pass;
}

const MaterialParams& 
MaterialPass::getParameters() const noexcept
{
	return _parameters;
}

MaterialParamPtr
MaterialPass::getParameter(const std::string& name) const noexcept
{
	assert(!name.empty());

	for (auto& it : _parameters)
	{
		if (it->getName() == name)
		{
			return it;
		}
	}

	return nullptr;
}

void
MaterialPass::setShaderObject(ShaderObjectPtr shader) noexcept
{
	_shaderObject = shader;
}

void
MaterialPass::setRenderState(RenderStatePtr state) noexcept
{
	_renderState = state;
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

_NAME_END