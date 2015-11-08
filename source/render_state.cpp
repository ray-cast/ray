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
#include <ray/render_state.h>

_NAME_BEGIN

RenderBlendState::RenderBlendState() noexcept
	: blendEnable(false)
	, blendSeparateEnable(false)
	, blendOp(GPU_ADD)
	, blendSrc(GPU_SRCALPHA)
	, blendDest(GPU_ONEMINUSSRCALPHA)
	, blendAlphaOp(GPU_ADD)
	, blendAlphaSrc(GPU_SRCALPHA)
	, blendAlphaDest(GPU_ONEMINUSSRCALPHA)
	, colorWriteMask(GPU_COLORMASK_RGBA)
{
}

RenderRasterState::RenderRasterState() noexcept
	: cullMode(GPU_CULL_BACK)
	, fillMode(GPU_SOLID_MODE)
	, primitiveType(VertexType::GPU_TRIANGLE)
	, scissorTestEnable(false)
	, srgbEnable(false)
	, multisampleEnable(false)
{
}

RenderDepthState::RenderDepthState() noexcept
	: depthEnable(true)
	, depthWriteMask(true)
	, depthFunc(GPU_LEQUAL)
	, depthBiasEnable(false)
	, depthSlopScaleBias(0)
	, depthBias(0)
{
}

RenderStencilState::RenderStencilState() noexcept
	: stencilEnable(false)
	, stencilRef(0)
	, stencilFunc(GPU_ALWAYS)
	, stencilReadMask(0xFFFFFFFF)
	, stencilWriteMask(0xFFFFFFFF)
	, stencilFail(STENCILOP_KEEP)
	, stencilZFail(STENCILOP_KEEP)
	, stencilPass(STENCILOP_KEEP)
	, stencilTwoEnable(false)
	, stencilTwoFunc(GPU_ALWAYS)
	, stencilTwoReadMask(0xFFFFFFFF)
	, stencilTwoWriteMask(0xFFFFFFFF)
	, stencilTwoFail(STENCILOP_KEEP)
	, stencilTwoZFail(STENCILOP_KEEP)
	, stencilTwoPass(STENCILOP_KEEP)
{
}

RenderState::RenderState() noexcept
{
}

RenderState::~RenderState() noexcept
{
}

void
RenderState::setup() noexcept
{
}

void
RenderState::close() noexcept
{
}

void
RenderState::setBlendState(const RenderBlendState& state) noexcept
{
	_blendState = state;
}

void
RenderState::setRasterState(const RenderRasterState& state) noexcept
{
	_rasterState = state;
}

void
RenderState::setDepthState(const RenderDepthState& state) noexcept
{
	_depthState = state;
}

void
RenderState::setStencilState(const RenderStencilState& state) noexcept
{
	_stencilState = state;
}

RenderBlendState&
RenderState::getBlendState() noexcept
{
	return _blendState;
}

RenderRasterState&
RenderState::getRasterState() noexcept
{
	return _rasterState;
}

RenderDepthState&
RenderState::getDepthState() noexcept
{
	return _depthState;
}

RenderStencilState&
RenderState::getStencilState() noexcept
{
	return _stencilState;
}

const RenderBlendState&
RenderState::getBlendState() const noexcept
{
	return _blendState;
}

const RenderRasterState&
RenderState::getRasterState() const noexcept
{
	return _rasterState;
}

const RenderDepthState&
RenderState::getDepthState() const noexcept
{
	return _depthState;
}

const RenderStencilState&
RenderState::getStencilState() const noexcept
{
	return _stencilState;
}

_NAME_END