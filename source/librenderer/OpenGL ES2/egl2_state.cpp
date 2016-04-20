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
#include "egl2_state.h"

_NAME_BEGIN

__ImplementSubClass(EGL2GraphicsState, GraphicsState, "EGL2GraphicsState")

EGL2GraphicsState::EGL2GraphicsState() noexcept
{
}

EGL2GraphicsState::~EGL2GraphicsState() noexcept
{
}

bool
EGL2GraphicsState::setup(const GraphicsStateDesc& desc) noexcept
{
	_stateDesc = desc;
	return true;
}

void
EGL2GraphicsState::close() noexcept
{
}

void
EGL2GraphicsState::apply(const GraphicsStateDesc& lastStateDesc) noexcept
{
	if (_stateDesc.getBlendEnable())
	{
		if (!lastStateDesc.getBlendEnable())
			glEnable(GL_BLEND);

		if (_stateDesc.getBlendSeparateEnable())
		{
			if (lastStateDesc.getBlendSrc() != _stateDesc.getBlendSrc() ||
				lastStateDesc.getBlendDest() != _stateDesc.getBlendDest() ||
				lastStateDesc.getBlendAlphaSrc() != _stateDesc.getBlendAlphaSrc() ||
				lastStateDesc.getBlendAlphaDest() != _stateDesc.getBlendAlphaDest())
			{
				GLenum sfactorRGB = EGL2Types::asBlendFactor(_stateDesc.getBlendSrc());
				GLenum dfactorRGB = EGL2Types::asBlendFactor(_stateDesc.getBlendDest());
				GLenum sfactorAlpha = EGL2Types::asBlendFactor(_stateDesc.getBlendAlphaSrc());
				GLenum dfactorAlpha = EGL2Types::asBlendFactor(_stateDesc.getBlendAlphaDest());

				glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
			}

			if (lastStateDesc.getBlendOp() != _stateDesc.getBlendOp() ||
				lastStateDesc.getBlendAlphaOp() != _stateDesc.getBlendAlphaOp())
			{
				GLenum modeRGB = EGL2Types::asBlendOperation(_stateDesc.getBlendOp());
				GLenum modeAlpha = EGL2Types::asBlendOperation(_stateDesc.getBlendAlphaOp());

				glBlendEquationSeparate(modeRGB, modeAlpha);
			}
		}
		else
		{
			if (lastStateDesc.getBlendSrc() != _stateDesc.getBlendSrc() ||
				lastStateDesc.getBlendDest() != _stateDesc.getBlendDest())
			{
				GLenum sfactorRGB = EGL2Types::asBlendFactor(_stateDesc.getBlendSrc());
				GLenum dfactorRGB = EGL2Types::asBlendFactor(_stateDesc.getBlendDest());

				glBlendFunc(sfactorRGB, dfactorRGB);
			}

			if (lastStateDesc.getBlendOp() != _stateDesc.getBlendOp())
			{
				GLenum modeRGB = EGL2Types::asBlendOperation(_stateDesc.getBlendOp());
				glBlendEquation(modeRGB);
			}
		}
	}
	else
	{
		if (lastStateDesc.getBlendEnable())
		{
			glDisable(GL_BLEND);
		}
	}

	if (lastStateDesc.getCullMode() != _stateDesc.getCullMode())
	{
		if (_stateDesc.getCullMode() != GraphicsCullMode::GraphicsCullModeNone)
		{
			GLenum mode = EGL2Types::asCullMode(_stateDesc.getCullMode());
			glEnable(GL_CULL_FACE);
			glCullFace(mode);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}

	if (lastStateDesc.getScissorTestEnable() != _stateDesc.getScissorTestEnable())
	{
		if (_stateDesc.getScissorTestEnable())
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
	}

	if (lastStateDesc.getLinear2sRGBEnable() != _stateDesc.getLinear2sRGBEnable())
	{
		if (_stateDesc.getLinear2sRGBEnable())
			glEnable(GL_FRAMEBUFFER_SRGB);
		else
			glDisable(GL_FRAMEBUFFER_SRGB);
	}

	if (_stateDesc.getDepthEnable())
	{
		if (!lastStateDesc.getDepthEnable())
		{
			glEnable(GL_DEPTH_TEST);
		}

		if (lastStateDesc.getDepthFunc() != _stateDesc.getDepthFunc())
		{
			GLenum func = EGL2Types::asCompareFunction(_stateDesc.getDepthFunc());
			glDepthFunc(func);
		}
	}
	else
	{
		if (lastStateDesc.getDepthEnable())
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	if (lastStateDesc.getDepthWriteEnable() != _stateDesc.getDepthWriteEnable())
	{
		GLboolean enable = _stateDesc.getDepthWriteEnable() ? GL_TRUE : GL_FALSE;
		glDepthMask(enable);
	}

	if (_stateDesc.getDepthBiasEnable())
	{
		if (!lastStateDesc.getDepthBiasEnable())
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
		}

		if (lastStateDesc.getDepthBias() != _stateDesc.getDepthBias() ||
			lastStateDesc.getDepthSlopeScaleBias() != _stateDesc.getDepthSlopeScaleBias())
		{
			glPolygonOffset(_stateDesc.getDepthSlopeScaleBias(), _stateDesc.getDepthBias());
		}
	}
	else
	{
		if (lastStateDesc.getDepthBiasEnable())
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}

	if (_stateDesc.getStencilEnable())
	{
		if (!lastStateDesc.getStencilEnable())
		{
			glEnable(GL_STENCIL_TEST);
		}

		if (lastStateDesc.getStencilFrontFunc() != _stateDesc.getStencilFrontFunc() ||
			lastStateDesc.getStencilFrontRef() != _stateDesc.getStencilFrontRef() ||
			lastStateDesc.getStencilFrontReadMask() != _stateDesc.getStencilFrontReadMask())
		{
			GLenum frontfunc = EGL2Types::asCompareFunction(_stateDesc.getStencilFrontFunc());
			glStencilFuncSeparate(GL_FRONT, frontfunc, _stateDesc.getStencilFrontRef(), _stateDesc.getStencilFrontReadMask());
		}

		if (lastStateDesc.getStencilBackFunc() != _stateDesc.getStencilBackFunc() ||
			lastStateDesc.getStencilBackRef() != _stateDesc.getStencilBackRef() ||
			lastStateDesc.getStencilBackReadMask() != _stateDesc.getStencilBackReadMask())
		{
			GLenum backfunc = EGL2Types::asCompareFunction(_stateDesc.getStencilBackFunc());
			glStencilFuncSeparate(GL_BACK, backfunc, _stateDesc.getStencilBackRef(), _stateDesc.getStencilBackReadMask());
		}

		if (lastStateDesc.getStencilFrontFail() != lastStateDesc.getStencilFrontFail() ||
			lastStateDesc.getStencilFrontZFail() != lastStateDesc.getStencilFrontZFail() ||
			lastStateDesc.getStencilFrontPass() != lastStateDesc.getStencilFrontPass())
		{
			GLenum frontfail = EGL2Types::asStencilOperation(lastStateDesc.getStencilFrontFail());
			GLenum frontzfail = EGL2Types::asStencilOperation(lastStateDesc.getStencilFrontZFail());
			GLenum frontpass = EGL2Types::asStencilOperation(lastStateDesc.getStencilFrontPass());
			glStencilOpSeparate(GL_FRONT, frontfail, frontzfail, frontpass);
		}

		if (lastStateDesc.getStencilBackFail() != lastStateDesc.getStencilBackFail() ||
			lastStateDesc.getStencilBackZFail() != lastStateDesc.getStencilBackZFail() ||
			lastStateDesc.getStencilBackPass() != lastStateDesc.getStencilBackPass())
		{
			GLenum backfail = EGL2Types::asStencilOperation(lastStateDesc.getStencilBackFail());
			GLenum backzfail = EGL2Types::asStencilOperation(lastStateDesc.getStencilBackZFail());
			GLenum backpass = EGL2Types::asStencilOperation(lastStateDesc.getStencilBackPass());
			glStencilOpSeparate(GL_BACK, backfail, backzfail, backpass);
		}

		if (lastStateDesc.getStencilFrontWriteMask() != _stateDesc.getStencilFrontWriteMask())
		{
			glStencilMaskSeparate(GL_FRONT, _stateDesc.getStencilFrontWriteMask());
		}

		if (lastStateDesc.getStencilBackWriteMask() != _stateDesc.getStencilBackWriteMask())
		{
			glStencilMaskSeparate(GL_BACK, _stateDesc.getStencilBackWriteMask());
		}
	}
	else
	{
		if (lastStateDesc.getStencilEnable())
		{
			glDisable(GL_STENCIL_TEST);
		}
	}
}

const GraphicsStateDesc&
EGL2GraphicsState::getGraphicsStateDesc() const noexcept
{
	return _stateDesc;
}

void
EGL2GraphicsState::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2GraphicsState::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END