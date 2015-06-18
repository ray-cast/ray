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
    , scissorTestEnable(false)
    , multisampleEnable(false)
    , srgbEnable(false)
    , pointSizeEnable(false)
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

RenderClearState::RenderClearState() noexcept
    : clearFlags(ClearFlags::CLEAR_COLOR)
    , clearColor(0, 0, 0, 0)
    , clearDepth(1)
    , clearStencil(0)
{
}

RenderState::RenderState() noexcept
{
}

RenderState::~RenderState() noexcept
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

RenderClearState&
RenderState::getClearState() noexcept
{
    return _clearState;
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

const RenderClearState&
RenderState::getClearState() const noexcept
{
    return _clearState;
}

CullMode
RenderState::stringToCullMode(const std::string& cullmode) noexcept
{
    if (cullmode == "back")
        return CullMode::GPU_CULL_BACK;
    else if (cullmode == "front")
        return CullMode::GPU_CULL_FRONT;
    else if (cullmode == "frontback")
        return CullMode::GPU_CULL_FRONT_BACK;
    else if (cullmode == "none")
        return CullMode::GPU_CULL_NONE;
    else
    {
        assert(false);
        return CullMode::GPU_CULL_NONE;
    }
}

FillMode
RenderState::stringToFillMode(const std::string& fillmode) noexcept
{
    if (fillmode == "point")
        return FillMode::GPU_POINT_MODE;
    else if (fillmode == "line")
        return FillMode::GPU_WIREFRAME_MODE;
    else if (fillmode == "solid")
        return FillMode::GPU_SOLID_MODE;
    else
    {
        assert(false);
        return FillMode::GPU_SOLID_MODE;
    }
}

BlendOperation
RenderState::stringToBlendOperation(const std::string& blendop) noexcept
{
    if (blendop == "sub")
        return GPU_SUBSTRACT;
    else if (blendop == "revsub")
        return GPU_REVSUBTRACT;
    else if (blendop == "add")
        return GPU_ADD;
    else
    {
        assert(false);
        return GPU_ADD;
    }
}

BlendFactor
RenderState::stringToBlendFactor(const std::string& factor) noexcept
{
    if (factor == "zero")
        return GPU_ZERO;
    else if (factor == "one")
        return GPU_ONE;
    else if (factor == "dstcol")
        return GPU_DSTCOL;
    else if (factor == "srccol")
        return GPU_SRCCOLOR;
    else if (factor == "srcalpha")
        return GPU_SRCALPHA;
    else if (factor == "dstalpha")
        return GPU_DSTALPHA;
    else if (factor == "invscrcol")
        return GPU_ONEMINUSSRCCOL;
    else if (factor == "invdstcol")
        return GPU_ONEMINUSDSTCOL;
    else if (factor == "invsrcalpha")
        return GPU_ONEMINUSSRCALPHA;
    else if (factor == "invdstalpha")
        return GPU_ONEMINUSDSTALPHA;
    else
    {
        assert(false);
        return GPU_ZERO;
    }
}

ColorMask
RenderState::stringToColorMask(const std::string& mask) noexcept
{
    if (mask == "red")
        return ColorMask::GPU_COLORMASK_RED;
    else if (mask == "green")
        return ColorMask::GPU_COLORMASK_GREEN;
    else if (mask == "blue")
        return ColorMask::GPU_COLORMASK_BLUE;
    else if (mask == "alpha")
        return ColorMask::GPU_COLORMASK_ALPHA;
    else if (mask == "rgb")
        return ColorMask::GPU_COLORMASK_RGB;
    else if (mask == "rgba")
        return ColorMask::GPU_COLORMASK_RGBA;
    else
    {
        assert(false);
        return ColorMask::GPU_COLORMASK_RGBA;
    }
}

CompareFunction
RenderState::stringToCompareFunc(const std::string& func) noexcept
{
    if (func == "lequal")
        return GPU_LEQUAL;
    else if (func == "equal")
        return GPU_EQUAL;
    else if (func == "greater")
        return GPU_GREATER;
    else if (func == "less")
        return GPU_LESS;
    else if (func == "gequal")
        return GPU_GEQUAL;
    else if (func == "notequal")
        return GPU_NOTEQUAL;
    else if (func == "always")
        return GPU_ALWAYS;
    else if (func == "never")
        return GPU_NEVER;
    else if (func == "none")
        return GPU_NONE;
    else
    {
        assert(false);
        return GPU_NONE;
    }
}

StencilOperation
RenderState::stringToStencilOp(const std::string& stencilop) noexcept
{
    if (stencilop == "keep")
        return STENCILOP_KEEP;
    else if (stencilop == "replace")
        return STENCILOP_REPLACE;
    else if (stencilop == "inc")
        return STENCILOP_INCR;
    else if (stencilop == "dec")
        return STENCILOP_DECR;
    else if (stencilop == "incwrap")
        return STENCILOP_INCR_WRAP;
    else if (stencilop == "decwrap")
        return STENCILOP_DECR_WRAP;
    else
    {
        assert(false);
        return STENCILOP_KEEP;
    }
}

ClearFlags
RenderState::stringToClearFlags(const std::string& flags) noexcept
{
    if (flags == "color")
        return ClearFlags::CLEAR_COLOR;
    else if (flags == "depth")
        return ClearFlags::CLEAR_DEPTH;
    else if (flags == "stencil")
        return ClearFlags::CLEAR_STENCIL;
    else if (flags == "color_depth")
        return ClearFlags::CLEAR_COLOR_DEPTH;
    else if (flags == "color_stencil")
        return ClearFlags::CLEAR_COLOR_STENCIL;
    else if (flags == "all")
        return ClearFlags::CLEAR_ALL;
    else if (flags == "none")
        return ClearFlags::CLEAR_NONE;
    else
    {
        assert(false);
        return ClearFlags::CLEAR_NONE;
    }
}

_NAME_END