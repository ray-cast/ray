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
#include "egl3_state.h"

_NAME_BEGIN

__ImplementSubClass(EGL3GraphicsState, GraphicsState, "EGL3GraphicsState")

EGL3GraphicsState::EGL3GraphicsState() noexcept
{
}

EGL3GraphicsState::~EGL3GraphicsState() noexcept
{
}

void
EGL3GraphicsState::apply(const GraphicsState& _stateCaptured) noexcept
{
	const auto& blendState = this->getBlendState();
	const auto& rasterState = this->getRasterState();
	const auto& depthState = this->getDepthState();
	const auto& stencilState = this->getStencilState();

	auto& _dstBlendState = _stateCaptured.getBlendState();
	auto& _dstRasterState = _stateCaptured.getRasterState();
	auto& _dstDepthState = _stateCaptured.getDepthState();
	auto& _dstStencilState = _stateCaptured.getStencilState();

	if (blendState.blendEnable)
	{
		if (!_dstBlendState.blendEnable)
			glEnable(GL_BLEND);

		if (blendState.blendSeparateEnable)
		{
			if (_dstBlendState.blendSrc != blendState.blendSrc ||
				_dstBlendState.blendDest != blendState.blendDest ||
				_dstBlendState.blendAlphaSrc != blendState.blendAlphaSrc ||
				_dstBlendState.blendAlphaDest != blendState.blendAlphaDest)
			{
				GLenum sfactorRGB = EGL3Types::asBlendFactor(blendState.blendSrc);
				GLenum dfactorRGB = EGL3Types::asBlendFactor(blendState.blendDest);
				GLenum sfactorAlpha = EGL3Types::asBlendFactor(blendState.blendAlphaSrc);
				GLenum dfactorAlpha = EGL3Types::asBlendFactor(blendState.blendAlphaDest);

				glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
			}

			if (_dstBlendState.blendOp != blendState.blendOp ||
				_dstBlendState.blendAlphaOp != blendState.blendAlphaOp)
			{
				GLenum modeRGB = EGL3Types::asBlendOperation(blendState.blendOp);
				GLenum modeAlpha = EGL3Types::asBlendOperation(blendState.blendAlphaOp);

				glBlendEquationSeparate(modeRGB, modeAlpha);
			}
		}
		else
		{
			if (_dstBlendState.blendSrc != blendState.blendSrc ||
				_dstBlendState.blendDest != blendState.blendDest)
			{
				GLenum sfactorRGB = EGL3Types::asBlendFactor(blendState.blendSrc);
				GLenum dfactorRGB = EGL3Types::asBlendFactor(blendState.blendDest);

				glBlendFunc(sfactorRGB, dfactorRGB);
			}

			if (_dstBlendState.blendOp != blendState.blendOp)
			{
				GLenum modeRGB = EGL3Types::asBlendOperation(blendState.blendOp);
				glBlendEquation(modeRGB);
			}
		}
	}
	else
	{
		if (_dstBlendState.blendEnable)
		{
			glDisable(GL_BLEND);
		}
	}

	if (_dstRasterState.cullMode != rasterState.cullMode)
	{
		if (rasterState.cullMode != GPU_CULL_NONE)
		{
			GLenum mode = EGL3Types::asCullMode(rasterState.cullMode);
			glEnable(GL_CULL_FACE);
			glCullFace(mode);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}

	if (_dstRasterState.scissorTestEnable != rasterState.scissorTestEnable)
	{
		if (rasterState.scissorTestEnable)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
	}

	if (_dstRasterState.srgbEnable != rasterState.srgbEnable)
	{
		if (rasterState.srgbEnable)
			glEnable(GL_FRAMEBUFFER_SRGB);
		else
			glDisable(GL_FRAMEBUFFER_SRGB);
	}

	if (depthState.depthEnable)
	{
		if (!_dstDepthState.depthEnable)
		{
			glEnable(GL_DEPTH_TEST);
		}

		if (_dstDepthState.depthFunc != depthState.depthFunc)
		{
			GLenum func = EGL3Types::asCompareFunction(depthState.depthFunc);
			glDepthFunc(func);
		}
	}
	else
	{
		if (_dstDepthState.depthEnable)
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	if (_dstDepthState.depthWriteMask != depthState.depthWriteMask)
	{
		GLboolean enable = depthState.depthWriteMask ? GL_TRUE : GL_FALSE;
		glDepthMask(enable);
	}

	if (depthState.depthBiasEnable)
	{
		if (!_dstDepthState.depthBiasEnable)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
		}

		if (_dstDepthState.depthBias != depthState.depthBias ||
			_dstDepthState.depthSlopScaleBias != depthState.depthSlopScaleBias)
		{
			glPolygonOffset(depthState.depthSlopScaleBias, depthState.depthBias);
		}
	}
	else
	{
		if (_dstDepthState.depthBiasEnable)
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}

	if (stencilState.stencilEnable)
	{
		if (!_dstStencilState.stencilEnable)
		{
			glEnable(GL_STENCIL_TEST);
		}

		if (stencilState.stencilTwoEnable)
		{
			if (_dstStencilState.stencilFunc != stencilState.stencilFunc ||
				_dstStencilState.stencilRef != stencilState.stencilRef ||
				_dstStencilState.stencilReadMask != stencilState.stencilReadMask)
			{
				GLenum frontfunc = EGL3Types::asCompareFunction(stencilState.stencilFunc);
				glStencilFuncSeparate(GL_FRONT, frontfunc, stencilState.stencilRef, stencilState.stencilReadMask);

				GLenum backfunc = EGL3Types::asCompareFunction(stencilState.stencilTwoFunc);
				glStencilFuncSeparate(GL_BACK, backfunc, stencilState.stencilRef, stencilState.stencilTwoReadMask);
			}

			if (_dstStencilState.stencilFail != _dstStencilState.stencilFail ||
				_dstStencilState.stencilZFail != _dstStencilState.stencilZFail ||
				_dstStencilState.stencilPass != _dstStencilState.stencilPass)
			{
				GLenum frontfail = EGL3Types::asStencilOperation(_dstStencilState.stencilFail);
				GLenum frontzfail = EGL3Types::asStencilOperation(_dstStencilState.stencilZFail);
				GLenum frontpass = EGL3Types::asStencilOperation(_dstStencilState.stencilPass);
				glStencilOpSeparate(GL_FRONT, frontfail, frontzfail, frontpass);

				GLenum backfail = EGL3Types::asStencilOperation(_dstStencilState.stencilTwoFail);
				GLenum backzfail = EGL3Types::asStencilOperation(_dstStencilState.stencilTwoZFail);
				GLenum backpass = EGL3Types::asStencilOperation(_dstStencilState.stencilTwoPass);
				glStencilOpSeparate(GL_BACK, backfail, backzfail, backpass);
			}

			if (_dstStencilState.stencilWriteMask != stencilState.stencilWriteMask ||
				_dstStencilState.stencilTwoWriteMask != stencilState.stencilTwoWriteMask)
			{
				glStencilMaskSeparate(GL_FRONT, stencilState.stencilWriteMask);
				glStencilMaskSeparate(GL_BACK, stencilState.stencilTwoWriteMask);
			}
		}
		else
		{
			if (_dstStencilState.stencilFunc != stencilState.stencilFunc ||
				_dstStencilState.stencilRef != stencilState.stencilRef ||
				_dstStencilState.stencilReadMask != stencilState.stencilReadMask)
			{
				GLenum func = EGL3Types::asCompareFunction(stencilState.stencilFunc);
				glStencilFunc(func, stencilState.stencilRef, stencilState.stencilReadMask);
			}

			if (_dstStencilState.stencilFail != stencilState.stencilFail ||
				_dstStencilState.stencilZFail != stencilState.stencilZFail ||
				_dstStencilState.stencilPass != stencilState.stencilPass)
			{
				GLenum fail = EGL3Types::asStencilOperation(stencilState.stencilFail);
				GLenum zfail = EGL3Types::asStencilOperation(stencilState.stencilZFail);
				GLenum pass = EGL3Types::asStencilOperation(stencilState.stencilPass);
				glStencilOp(fail, zfail, pass);
			}

			if (_dstStencilState.stencilWriteMask != stencilState.stencilWriteMask)
			{
				glStencilMask(stencilState.stencilWriteMask);
			}
		}
	}
	else
	{
		if (_dstStencilState.stencilEnable)
		{
			glDisable(GL_STENCIL_TEST);
		}
	}
}

_NAME_END