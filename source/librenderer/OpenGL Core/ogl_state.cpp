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
				GLenum sfactorRGB = OGLTypes::asBlendFactor(_stateDesc.getBlendSrc());
				GLenum dfactorRGB = OGLTypes::asBlendFactor(_stateDesc.getBlendDest());
				GLenum sfactorAlpha = OGLTypes::asBlendFactor(_stateDesc.getBlendAlphaSrc());
				GLenum dfactorAlpha = OGLTypes::asBlendFactor(_stateDesc.getBlendAlphaDest());

				glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
			}

			if (lastStateDesc.getBlendOp() != _stateDesc.getBlendOp() ||
				lastStateDesc.getBlendAlphaOp() != _stateDesc.getBlendAlphaOp())
			{
				GLenum modeRGB = OGLTypes::asBlendOperation(_stateDesc.getBlendOp());
				GLenum modeAlpha = OGLTypes::asBlendOperation(_stateDesc.getBlendAlphaOp());

				glBlendEquationSeparate(modeRGB, modeAlpha);
			}
		}
		else
		{
			if (lastStateDesc.getBlendSrc() != _stateDesc.getBlendSrc() ||
				lastStateDesc.getBlendDest() != _stateDesc.getBlendDest())
			{
				GLenum sfactorRGB = OGLTypes::asBlendFactor(_stateDesc.getBlendSrc());
				GLenum dfactorRGB = OGLTypes::asBlendFactor(_stateDesc.getBlendDest());

				glBlendFunc(sfactorRGB, dfactorRGB);
			}

			if (lastStateDesc.getBlendOp() != _stateDesc.getBlendOp())
			{
				GLenum modeRGB = OGLTypes::asBlendOperation(_stateDesc.getBlendOp());
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
			GLenum mode = OGLTypes::asCullMode(_stateDesc.getCullMode());
			glEnable(GL_CULL_FACE);
			glCullFace(mode);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}

	if (lastStateDesc.getPolygonMode() != _stateDesc.getPolygonMode())
	{
		GLenum mode = OGLTypes::asFillMode(_stateDesc.getPolygonMode());
		glPolygonMode(GL_FRONT_AND_BACK, mode);
	}

	if (lastStateDesc.getScissorTestEnable() != _stateDesc.getScissorTestEnable())
	{
		if (_stateDesc.getScissorTestEnable())
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
	}

	if (lastStateDesc.getsRGBEnable() != _stateDesc.getsRGBEnable())
	{
		if (_stateDesc.getsRGBEnable())
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
			GLenum func = OGLTypes::asCompareFunction(_stateDesc.getDepthFunc());
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

		if (_stateDesc.getStencilTwoEnable())
		{
			if (lastStateDesc.getStencilFunc() != _stateDesc.getStencilFunc() ||
				lastStateDesc.getStencilRef() != _stateDesc.getStencilRef() ||
				lastStateDesc.getStencilReadMask() != _stateDesc.getStencilReadMask())
			{
				GLenum frontfunc = OGLTypes::asCompareFunction(_stateDesc.getStencilFunc());
				glStencilFuncSeparate(GL_FRONT, frontfunc, _stateDesc.getStencilRef(), _stateDesc.getStencilReadMask());
			}

			if (lastStateDesc.getStencilTwoFunc() != _stateDesc.getStencilTwoFunc() ||
				lastStateDesc.getStencilTwoRef() != _stateDesc.getStencilTwoRef() ||
				lastStateDesc.getStencilTwoReadMask() != _stateDesc.getStencilTwoReadMask())
			{
				GLenum backfunc = OGLTypes::asCompareFunction(_stateDesc.getStencilTwoFunc());
				glStencilFuncSeparate(GL_BACK, backfunc, _stateDesc.getStencilTwoRef(), _stateDesc.getStencilTwoReadMask());
			}

			if (lastStateDesc.getStencilFail() != lastStateDesc.getStencilFail() ||
				lastStateDesc.getStencilZFail() != lastStateDesc.getStencilZFail() ||
				lastStateDesc.getStencilPass() != lastStateDesc.getStencilPass())
			{
				GLenum frontfail = OGLTypes::asStencilOperation(lastStateDesc.getStencilFail());
				GLenum frontzfail = OGLTypes::asStencilOperation(lastStateDesc.getStencilZFail());
				GLenum frontpass = OGLTypes::asStencilOperation(lastStateDesc.getStencilPass());
				glStencilOpSeparate(GL_FRONT, frontfail, frontzfail, frontpass);
			}

			if (lastStateDesc.getStencilTwoFail() != lastStateDesc.getStencilTwoFail() ||
				lastStateDesc.getStencilTwoZFail() != lastStateDesc.getStencilTwoZFail() ||
				lastStateDesc.getStencilTwoPass() != lastStateDesc.getStencilTwoPass())
			{
				GLenum backfail = OGLTypes::asStencilOperation(lastStateDesc.getStencilTwoFail());
				GLenum backzfail = OGLTypes::asStencilOperation(lastStateDesc.getStencilTwoZFail());
				GLenum backpass = OGLTypes::asStencilOperation(lastStateDesc.getStencilTwoPass());
				glStencilOpSeparate(GL_BACK, backfail, backzfail, backpass);
			}

			if (lastStateDesc.getStencilWriteMask() != _stateDesc.getStencilWriteMask() ||
				lastStateDesc.getStencilTwoWriteMask() != _stateDesc.getStencilTwoWriteMask())
			{
				glStencilMaskSeparate(GL_FRONT, _stateDesc.getStencilWriteMask());
				glStencilMaskSeparate(GL_BACK, _stateDesc.getStencilTwoWriteMask());
			}
		}
		else
		{
			if (lastStateDesc.getStencilFunc() != _stateDesc.getStencilFunc() ||
				lastStateDesc.getStencilRef() != _stateDesc.getStencilRef() ||
				lastStateDesc.getStencilReadMask() != _stateDesc.getStencilReadMask())
			{
				GLenum func = OGLTypes::asCompareFunction(_stateDesc.getStencilFunc());
				glStencilFunc(func, _stateDesc.getStencilRef(), _stateDesc.getStencilReadMask());
			}

			if (lastStateDesc.getStencilFail() != _stateDesc.getStencilFail() ||
				lastStateDesc.getStencilZFail() != _stateDesc.getStencilZFail() ||
				lastStateDesc.getStencilPass() != _stateDesc.getStencilPass())
			{
				GLenum fail = OGLTypes::asStencilOperation(_stateDesc.getStencilFail());
				GLenum zfail = OGLTypes::asStencilOperation(_stateDesc.getStencilZFail());
				GLenum pass = OGLTypes::asStencilOperation(_stateDesc.getStencilPass());
				glStencilOp(fail, zfail, pass);
			}

			if (lastStateDesc.getStencilWriteMask() != _stateDesc.getStencilWriteMask())
			{
				glStencilMask(_stateDesc.getStencilWriteMask());
			}
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