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
#include <ray/ogl_state.h>

_NAME_BEGIN

OGLRenderState::OGLRenderState() noexcept
{
}

OGLRenderState::~OGLRenderState() noexcept
{
}

void 
OGLRenderState::apply(const RenderState& _stateCaptured) noexcept
{
	const auto& blendState = this->getBlendState();
	const auto& rasterState = this->getRasterState();
	const auto& depthState = this->getDepthState();
	const auto& stencilState = this->getStencilState();

	auto& _blendState = _stateCaptured.getBlendState();
	auto& _rasterState = _stateCaptured.getRasterState();
	auto& _depthState = _stateCaptured.getDepthState();
	auto& _stencilState = _stateCaptured.getStencilState();

	if (blendState.blendEnable)
	{
		if (!_blendState.blendEnable)
		{
#if _USE_RENDER_COMMAND
			BlendEnableCommand command;
			command.header = GL_BLEND_ENABLE_COMMAND;
			_renderCommands.write(&command, sizeof(command));
#else
			glEnable(GL_BLEND);
#endif
		}

		if (blendState.blendSeparateEnable)
		{
			if (_blendState.blendSrc != blendState.blendSrc ||
				_blendState.blendDest != blendState.blendDest ||
				_blendState.blendAlphaSrc != blendState.blendAlphaSrc ||
				_blendState.blendAlphaDest != blendState.blendAlphaDest)
			{
				GLenum sfactorRGB = OGLTypes::asBlendFactor(blendState.blendSrc);
				GLenum dfactorRGB = OGLTypes::asBlendFactor(blendState.blendDest);
				GLenum sfactorAlpha = OGLTypes::asBlendFactor(blendState.blendAlphaSrc);
				GLenum dfactorAlpha = OGLTypes::asBlendFactor(blendState.blendAlphaDest);

				glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
			}

			if (_blendState.blendOp != blendState.blendOp ||
				_blendState.blendAlphaOp != blendState.blendAlphaOp)
			{
				GLenum modeRGB = OGLTypes::asBlendOperation(blendState.blendOp);
				GLenum modeAlpha = OGLTypes::asBlendOperation(blendState.blendAlphaOp);

				glBlendEquationSeparate(modeRGB, modeAlpha);
			}
		}
		else
		{
			if (_blendState.blendSrc != blendState.blendSrc ||
				_blendState.blendDest != blendState.blendDest)
			{
				GLenum sfactorRGB = OGLTypes::asBlendFactor(blendState.blendSrc);
				GLenum dfactorRGB = OGLTypes::asBlendFactor(blendState.blendDest);

				glBlendFunc(sfactorRGB, dfactorRGB);
			}

			if (_blendState.blendOp != blendState.blendOp)
			{
				GLenum modeRGB = OGLTypes::asBlendOperation(blendState.blendOp);
				glBlendEquation(modeRGB);
			}
		}
	}
	else
	{
		if (_blendState.blendEnable)
		{
			glDisable(GL_BLEND);
		}
	}

	if (_rasterState.cullMode != rasterState.cullMode)
	{
		if (rasterState.cullMode != GPU_CULL_NONE)
		{
			GLenum mode = OGLTypes::asCullMode(rasterState.cullMode);
			glEnable(GL_CULL_FACE);
			glCullFace(mode);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}

#if !defined(EGLAPI)
	if (_rasterState.fillMode != rasterState.fillMode)
	{
		GLenum mode = OGLTypes::asFillMode(rasterState.fillMode);
		glPolygonMode(GL_FRONT_AND_BACK, mode);
	}
#endif

	if (_rasterState.scissorTestEnable != rasterState.scissorTestEnable)
	{
		if (rasterState.scissorTestEnable)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
	}

	if (_rasterState.srgbEnable != rasterState.srgbEnable)
	{
		if (rasterState.srgbEnable)
			glEnable(GL_FRAMEBUFFER_SRGB);
		else
			glDisable(GL_FRAMEBUFFER_SRGB);
	}

	if (depthState.depthEnable)
	{
		if (!_depthState.depthEnable)
		{
			glEnable(GL_DEPTH_TEST);
		}

		if (_depthState.depthFunc != depthState.depthFunc)
		{
			GLenum func = OGLTypes::asCompareFunction(depthState.depthFunc);
			glDepthFunc(func);
		}
	}
	else
	{
		if (_depthState.depthEnable)
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	if (_depthState.depthWriteMask != depthState.depthWriteMask)
	{
		GLboolean enable = depthState.depthWriteMask ? GL_TRUE : GL_FALSE;
		glDepthMask(enable);
	}

	if (depthState.depthBiasEnable)
	{
		if (!_depthState.depthBiasEnable)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
		}

		if (_depthState.depthBias != depthState.depthBias ||
			_depthState.depthSlopScaleBias != depthState.depthSlopScaleBias)
		{
			glPolygonOffset(depthState.depthSlopScaleBias, depthState.depthBias);
		}
	}
	else
	{
		if (_depthState.depthBiasEnable)
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}

	if (stencilState.stencilEnable)
	{
		if (!_stencilState.stencilEnable)
		{
			glEnable(GL_STENCIL_TEST);
		}

		if (stencilState.stencilTwoEnable)
		{
			if (_stencilState.stencilFunc != stencilState.stencilFunc ||
				_stencilState.stencilRef != stencilState.stencilRef ||
				_stencilState.stencilReadMask != stencilState.stencilReadMask)
			{
				GLenum frontfunc = OGLTypes::asCompareFunction(stencilState.stencilFunc);
				glStencilFuncSeparate(GL_FRONT, frontfunc, stencilState.stencilRef, stencilState.stencilReadMask);

				GLenum backfunc = OGLTypes::asCompareFunction(stencilState.stencilTwoFunc);
				glStencilFuncSeparate(GL_BACK, backfunc, stencilState.stencilRef, stencilState.stencilTwoReadMask);
			}

			if (_stencilState.stencilFail != _stencilState.stencilFail ||
				_stencilState.stencilZFail != _stencilState.stencilZFail ||
				_stencilState.stencilPass != _stencilState.stencilPass)
			{
				GLenum frontfail = OGLTypes::asStencilOperation(_stencilState.stencilFail);
				GLenum frontzfail = OGLTypes::asStencilOperation(_stencilState.stencilZFail);
				GLenum frontpass = OGLTypes::asStencilOperation(_stencilState.stencilPass);
				glStencilOpSeparate(GL_FRONT, frontfail, frontzfail, frontpass);

				GLenum backfail = OGLTypes::asStencilOperation(_stencilState.stencilTwoFail);
				GLenum backzfail = OGLTypes::asStencilOperation(_stencilState.stencilTwoZFail);
				GLenum backpass = OGLTypes::asStencilOperation(_stencilState.stencilTwoPass);
				glStencilOpSeparate(GL_BACK, backfail, backzfail, backpass);
			}

			if (_stencilState.stencilWriteMask != stencilState.stencilWriteMask ||
				_stencilState.stencilTwoWriteMask != stencilState.stencilTwoWriteMask)
			{
				glStencilMaskSeparate(GL_FRONT, stencilState.stencilWriteMask);
				glStencilMaskSeparate(GL_BACK, stencilState.stencilTwoWriteMask);
			}
		}
		else
		{
			if (_stencilState.stencilFunc != stencilState.stencilFunc ||
				_stencilState.stencilRef != stencilState.stencilRef ||
				_stencilState.stencilReadMask != stencilState.stencilReadMask)
			{
				GLenum func = OGLTypes::asCompareFunction(stencilState.stencilFunc);
				glStencilFunc(func, stencilState.stencilRef, stencilState.stencilReadMask);
			}

			if (_stencilState.stencilFail != stencilState.stencilFail ||
				_stencilState.stencilZFail != stencilState.stencilZFail ||
				_stencilState.stencilPass != stencilState.stencilPass)
			{
				GLenum fail = OGLTypes::asStencilOperation(stencilState.stencilFail);
				GLenum zfail = OGLTypes::asStencilOperation(stencilState.stencilZFail);
				GLenum pass = OGLTypes::asStencilOperation(stencilState.stencilPass);
				glStencilOp(fail, zfail, pass);
			}

			if (_stencilState.stencilWriteMask != stencilState.stencilWriteMask)
			{
				glStencilMask(stencilState.stencilWriteMask);
			}
		}
	}
	else
	{
		if (_stencilState.stencilEnable)
		{
			glDisable(GL_STENCIL_TEST);
		}
	}
}

_NAME_END