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
#include "ogl_state.h"

_NAME_BEGIN

__ImplementSubClass(OGLGraphicsState, GraphicsState, "OGLGraphicsState")

OGLGraphicsState::OGLGraphicsState() noexcept
{
}

OGLGraphicsState::~OGLGraphicsState() noexcept
{
	this->close();
}

bool
OGLGraphicsState::setup(const GraphicsStateDesc& desc) noexcept
{
	_stateDesc = desc;
	return true;
}

void 
OGLGraphicsState::close() noexcept
{
}

void
OGLGraphicsState::apply(const GraphicsStateDesc& lastStateDesc) noexcept
{
	const auto& blendState = _stateDesc.getBlendState();
	const auto& rasterState = _stateDesc.getRasterState();
	const auto& depthState = _stateDesc.getDepthState();
	const auto& stencilState = _stateDesc.getStencilState();

	auto& _dstBlendState = lastStateDesc.getBlendState();
	auto& _dstRasterState = lastStateDesc.getRasterState();
	auto& _dstDepthState = lastStateDesc.getDepthState();
	auto& _dstStencilState = lastStateDesc.getStencilState();

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
				GLenum sfactorRGB = OGLTypes::asBlendFactor(blendState.blendSrc);
				GLenum dfactorRGB = OGLTypes::asBlendFactor(blendState.blendDest);
				GLenum sfactorAlpha = OGLTypes::asBlendFactor(blendState.blendAlphaSrc);
				GLenum dfactorAlpha = OGLTypes::asBlendFactor(blendState.blendAlphaDest);

				glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
			}

			if (_dstBlendState.blendOp != blendState.blendOp ||
				_dstBlendState.blendAlphaOp != blendState.blendAlphaOp)
			{
				GLenum modeRGB = OGLTypes::asBlendOperation(blendState.blendOp);
				GLenum modeAlpha = OGLTypes::asBlendOperation(blendState.blendAlphaOp);

				glBlendEquationSeparate(modeRGB, modeAlpha);
			}
		}
		else
		{
			if (_dstBlendState.blendSrc != blendState.blendSrc ||
				_dstBlendState.blendDest != blendState.blendDest)
			{
				GLenum sfactorRGB = OGLTypes::asBlendFactor(blendState.blendSrc);
				GLenum dfactorRGB = OGLTypes::asBlendFactor(blendState.blendDest);

				glBlendFunc(sfactorRGB, dfactorRGB);
			}

			if (_dstBlendState.blendOp != blendState.blendOp)
			{
				GLenum modeRGB = OGLTypes::asBlendOperation(blendState.blendOp);
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
		if (rasterState.cullMode != CullMode::None)
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

	if (_dstRasterState.fillMode != rasterState.fillMode)
	{
		GLenum mode = OGLTypes::asFillMode(rasterState.fillMode);
		glPolygonMode(GL_FRONT_AND_BACK, mode);
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
			GLenum func = OGLTypes::asCompareFunction(depthState.depthFunc);
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
				GLenum frontfunc = OGLTypes::asCompareFunction(stencilState.stencilFunc);
				glStencilFuncSeparate(GL_FRONT, frontfunc, stencilState.stencilRef, stencilState.stencilReadMask);

				GLenum backfunc = OGLTypes::asCompareFunction(stencilState.stencilTwoFunc);
				glStencilFuncSeparate(GL_BACK, backfunc, stencilState.stencilRef, stencilState.stencilTwoReadMask);
			}

			if (_dstStencilState.stencilFail != _dstStencilState.stencilFail ||
				_dstStencilState.stencilZFail != _dstStencilState.stencilZFail ||
				_dstStencilState.stencilPass != _dstStencilState.stencilPass)
			{
				GLenum frontfail = OGLTypes::asStencilOperation(_dstStencilState.stencilFail);
				GLenum frontzfail = OGLTypes::asStencilOperation(_dstStencilState.stencilZFail);
				GLenum frontpass = OGLTypes::asStencilOperation(_dstStencilState.stencilPass);
				glStencilOpSeparate(GL_FRONT, frontfail, frontzfail, frontpass);

				GLenum backfail = OGLTypes::asStencilOperation(_dstStencilState.stencilTwoFail);
				GLenum backzfail = OGLTypes::asStencilOperation(_dstStencilState.stencilTwoZFail);
				GLenum backpass = OGLTypes::asStencilOperation(_dstStencilState.stencilTwoPass);
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
				GLenum func = OGLTypes::asCompareFunction(stencilState.stencilFunc);
				glStencilFunc(func, stencilState.stencilRef, stencilState.stencilReadMask);
			}

			if (_dstStencilState.stencilFail != stencilState.stencilFail ||
				_dstStencilState.stencilZFail != stencilState.stencilZFail ||
				_dstStencilState.stencilPass != stencilState.stencilPass)
			{
				GLenum fail = OGLTypes::asStencilOperation(stencilState.stencilFail);
				GLenum zfail = OGLTypes::asStencilOperation(stencilState.stencilZFail);
				GLenum pass = OGLTypes::asStencilOperation(stencilState.stencilPass);
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

const GraphicsStateDesc& 
OGLGraphicsState::getGraphicsStateDesc() const noexcept
{
	return _stateDesc;
}

void
OGLGraphicsState::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLGraphicsState::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END