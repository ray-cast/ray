// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
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
#include <ray/graphics_state.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsState, GraphicsChild, "GraphicsState")

GraphicsStateDesc::GraphicsStateDesc() noexcept
	: _blendEnable(false)
	, _blendSeparateEnable(false)
	, _scissorTestEnable(false)
	, _srgbEnable(false)
	, _multisampleEnable(false)
	, _rasterizerDiscardEnable(false)
	, _depthEnable(true)
	, _depthBoundsEnable(false)
	, _depthWriteEnable(true)
	, _depthBiasEnable(false)
	, _depthClampEnable(false)
	, _depthBiasClamp(false)
	, _stencilEnable(false)
	, _stencilTwoEnable(false)
	, _blendOp(GraphicsBlendOp::GraphicsBlendOpAdd)
	, _blendAlphaOp(GraphicsBlendOp::GraphicsBlendOpAdd)
	, _blendSrc(GraphicsBlendFactor::GraphicsBlendFactorSrcAlpha)
	, _blendDest(GraphicsBlendFactor::GraphicsBlendFactorOneMinusSrcAlpha)
	, _blendAlphaSrc(GraphicsBlendFactor::GraphicsBlendFactorSrcAlpha)
	, _blendAlphaDest(GraphicsBlendFactor::GraphicsBlendFactorOneMinusSrcAlpha)
	, _colorWriteMask(GraphicsColorMask::GraphicsColorMaskRGBA)
	, _cullMode(GraphicsCullMode::GraphicsCullModeBack)
	, _polygonMode(GraphicsPolygonMode::GraphicsPolygonModeSolid)
	, _primitiveType(GraphicsVertexType::GraphicsVertexTypeTriangleList)
	, _frontFace(GraphicsFrontFace::GraphicsFrontFaceCW)
	, _depthMin(0.0)
	, _depthMax(1.0)
	, _depthSlopeScaleBias(0)
	, _depthBias(0)
	, _depthFunc(GraphicsCompareFunc::GraphicsCompareFuncLequal)
	, _stencilRef(0)
	, _stencilTwoRef(0)
	, _stencilReadMask(0xFFFFFFFF)
	, _stencilWriteMask(0xFFFFFFFF)
	, _stencilTwoReadMask(0xFFFFFFFF)
	, _stencilTwoWriteMask(0xFFFFFFFF)
	, _stencilFunc(GraphicsCompareFunc::GraphicsCompareFuncAlways)
	, _stencilTwoFunc(GraphicsCompareFunc::GraphicsCompareFuncAlways)
	, _stencilFail(GraphicsStencilOp::GraphicsStencilOpKeep)
	, _stencilZFail(GraphicsStencilOp::GraphicsStencilOpKeep)
	, _stencilPass(GraphicsStencilOp::GraphicsStencilOpKeep)	
	, _stencilTwoFail(GraphicsStencilOp::GraphicsStencilOpKeep)
	, _stencilTwoZFail(GraphicsStencilOp::GraphicsStencilOpKeep)
	, _stencilTwoPass(GraphicsStencilOp::GraphicsStencilOpKeep)
{
}

GraphicsStateDesc::~GraphicsStateDesc() noexcept
{
}

void
GraphicsStateDesc::setBlendEnable(bool enable) noexcept
{
	_blendEnable = enable;
}

void
GraphicsStateDesc::setBlendSeparateEnable(bool enable) noexcept
{
	_blendSeparateEnable = enable;
}

void
GraphicsStateDesc::setBlendOp(GraphicsBlendOp blendOp) noexcept
{
	_blendOp = blendOp;
}

void
GraphicsStateDesc::setBlendSrc(GraphicsBlendFactor factor) noexcept
{
	_blendSrc = factor;
}

void
GraphicsStateDesc::setBlendDest(GraphicsBlendFactor factor) noexcept
{
	_blendDest = factor;
}

void
GraphicsStateDesc::setBlendAlphaOp(GraphicsBlendOp blendOp) noexcept
{
	_blendAlphaOp = blendOp;
}

void
GraphicsStateDesc::setBlendAlphaSrc(GraphicsBlendFactor factor) noexcept
{
	_blendAlphaSrc = factor;
}

void
GraphicsStateDesc::setBlendAlphaDest(GraphicsBlendFactor factor) noexcept
{
	_blendAlphaDest = factor;
}

void
GraphicsStateDesc::setColorWriteMask(GraphicsColorMask mask) noexcept
{
	_colorWriteMask = mask;
}

void
GraphicsStateDesc::setCullMode(GraphicsCullMode mode) noexcept
{
	_cullMode = mode;
}

void
GraphicsStateDesc::setPolygonMode(GraphicsPolygonMode mode) noexcept
{
	_polygonMode = mode;
}

void
GraphicsStateDesc::setPrimitiveType(GraphicsVertexType type) noexcept
{
	_primitiveType = type;
}

void
GraphicsStateDesc::setFrontFace(GraphicsFrontFace face) noexcept
{
	_frontFace = face;
}

void
GraphicsStateDesc::setScissorTestEnable(bool enable) noexcept
{
	_scissorTestEnable = enable;
}

void
GraphicsStateDesc::setsRGBEnable(bool enable) noexcept
{
	_srgbEnable = enable;
}

void
GraphicsStateDesc::setMultisampleEnable(bool enable) noexcept
{
	_multisampleEnable = enable;
}

void
GraphicsStateDesc::setRasterizerDiscardEnable(bool enable) noexcept
{
	_rasterizerDiscardEnable = enable;
}

void
GraphicsStateDesc::setDepthEnable(bool enable) noexcept
{
	_depthEnable = enable;
}

void
GraphicsStateDesc::setDepthWriteEnable(bool enable) noexcept
{
	_depthWriteEnable = enable;
}

void
GraphicsStateDesc::setDepthBoundsEnable(bool enable) noexcept
{
	_depthBoundsEnable = enable;
}

void
GraphicsStateDesc::setDepthMin(float min) noexcept
{
	_depthMin = min;
}

void
GraphicsStateDesc::setDepthMax(float max) noexcept
{
	_depthMax = max;
}

void
GraphicsStateDesc::setDepthFunc(GraphicsCompareFunc func) noexcept
{
	_depthFunc = func;
}

void
GraphicsStateDesc::setDepthBiasEnable(bool enable) noexcept
{
	_depthBiasEnable = enable;
}

void
GraphicsStateDesc::setDepthBias(float bias) noexcept
{
	_depthBias = bias;
}

void
GraphicsStateDesc::setDepthSlopeScaleBias(float bias) noexcept
{
	_depthSlopeScaleBias = bias;
}

void
GraphicsStateDesc::setDepthBiasClamp(bool bias) noexcept
{
	_depthBiasClamp = bias;
}

void
GraphicsStateDesc::setDepthClampEnable(bool enable) noexcept
{
	_depthClampEnable = enable;
}

void
GraphicsStateDesc::setStencilEnable(bool enable) noexcept
{
	_stencilEnable = enable;
}

void
GraphicsStateDesc::setStencilRef(std::int32_t ref) noexcept
{
	_stencilRef = ref;
}

void
GraphicsStateDesc::setStencilFunc(GraphicsCompareFunc func) noexcept
{
	_stencilFunc = func;
}

void
GraphicsStateDesc::setStencilReadMask(std::uint32_t mask) noexcept
{
	_stencilReadMask = mask;
}

void
GraphicsStateDesc::setStencilWriteMask(std::uint32_t mask) noexcept
{
	_stencilWriteMask = mask;
}

void
GraphicsStateDesc::setStencilFail(GraphicsStencilOp stencilOp) noexcept
{
	_stencilFail = stencilOp;
}

void
GraphicsStateDesc::setStencilZFail(GraphicsStencilOp stencilOp) noexcept
{
	_stencilZFail = stencilOp;
}

void
GraphicsStateDesc::setStencilPass(GraphicsStencilOp stencilOp) noexcept
{
	_stencilPass = stencilOp;
}

void
GraphicsStateDesc::setStencilTwoEnable(bool enable) noexcept
{
	_stencilTwoEnable = enable;
}

void
GraphicsStateDesc::setStencilTwoRef(std::int32_t ref) noexcept
{
	_stencilTwoRef = ref;
}

void
GraphicsStateDesc::setStencilTwoFunc(GraphicsCompareFunc func) noexcept
{
	_stencilTwoFunc = func;
}

void
GraphicsStateDesc::setStencilTwoReadMask(std::uint32_t mask) noexcept
{
	_stencilTwoReadMask = mask;
}

void
GraphicsStateDesc::setStencilTwoWriteMask(std::uint32_t mask) noexcept
{
	_stencilTwoWriteMask = mask;
}

void
GraphicsStateDesc::setStencilTwoFail(GraphicsStencilOp stencilOp) noexcept
{
	_stencilTwoFail = stencilOp;
}

void
GraphicsStateDesc::setStencilTwoZFail(GraphicsStencilOp stencilOp) noexcept
{
	_stencilTwoZFail = stencilOp;
}

void
GraphicsStateDesc::setStencilTwoPass(GraphicsStencilOp stencilOp) noexcept
{
	_stencilTwoPass = stencilOp;
}

bool
GraphicsStateDesc::getBlendEnable() const noexcept
{
	return _blendEnable;
}

bool
GraphicsStateDesc::getBlendSeparateEnable() const noexcept
{
	return _blendSeparateEnable;
}

GraphicsBlendOp
GraphicsStateDesc::getBlendOp() const noexcept
{
	return _blendOp;
}

GraphicsBlendFactor
GraphicsStateDesc::getBlendSrc() const noexcept
{
	return _blendSrc;
}

GraphicsBlendFactor
GraphicsStateDesc::getBlendDest() const noexcept
{
	return _blendDest;
}

GraphicsBlendOp
GraphicsStateDesc::getBlendAlphaOp() const noexcept
{
	return _blendAlphaOp;
}

GraphicsBlendFactor
GraphicsStateDesc::getBlendAlphaSrc() const noexcept
{
	return _blendAlphaSrc;
}

GraphicsBlendFactor
GraphicsStateDesc::getBlendAlphaDest() const noexcept
{
	return _blendAlphaDest;
}

GraphicsColorMask
GraphicsStateDesc::getColorWriteMask() const noexcept
{
	return _colorWriteMask;
}

GraphicsCullMode
GraphicsStateDesc::getCullMode() const noexcept
{
	return _cullMode;
}

GraphicsPolygonMode
GraphicsStateDesc::getPolygonMode() const noexcept
{
	return _polygonMode;
}

GraphicsVertexType
GraphicsStateDesc::getPrimitiveType() const noexcept
{
	return _primitiveType;
}

GraphicsFrontFace
GraphicsStateDesc::getFrontFace() const noexcept
{
	return _frontFace;
}

bool
GraphicsStateDesc::getScissorTestEnable() const noexcept
{
	return _scissorTestEnable;
}

bool
GraphicsStateDesc::getsRGBEnable() const noexcept
{
	return _srgbEnable;
}

bool
GraphicsStateDesc::getMultisampleEnable() const noexcept
{
	return _multisampleEnable;
}

bool
GraphicsStateDesc::getRasterizerDiscardEnable() const noexcept
{
	return _rasterizerDiscardEnable;
}

bool
GraphicsStateDesc::getDepthEnable() const noexcept
{
	return _depthEnable;
}

bool
GraphicsStateDesc::getDepthWriteEnable() const noexcept
{
	return _depthWriteEnable;
}

bool
GraphicsStateDesc::getDepthBoundsEnable() const noexcept
{
	return _depthBoundsEnable;
}

float
GraphicsStateDesc::getDepthMin() const noexcept
{
	return _depthMin;
}

float
GraphicsStateDesc::getDepthMax() const noexcept
{
	return _depthMax;
}

GraphicsCompareFunc
GraphicsStateDesc::getDepthFunc() const noexcept
{
	return _depthFunc;
}

bool
GraphicsStateDesc::getDepthBiasEnable() const noexcept
{
	return _depthBiasEnable;
}

float
GraphicsStateDesc::getDepthBias() const noexcept
{
	return _depthBias;
}

float
GraphicsStateDesc::getDepthSlopeScaleBias() const noexcept
{
	return _depthSlopeScaleBias;
}

bool
GraphicsStateDesc::getDepthBiasClamp() const noexcept
{
	return _depthBiasClamp;
}

bool
GraphicsStateDesc::getDepthClampEnable() const noexcept
{
	return _depthClampEnable;
}

bool
GraphicsStateDesc::getStencilEnable() const noexcept
{
	return _stencilEnable;
}

std::int32_t
GraphicsStateDesc::getStencilRef() const noexcept
{
	return _stencilRef;
}

GraphicsCompareFunc
GraphicsStateDesc::getStencilFunc() const noexcept
{
	return _stencilFunc;
}

std::uint32_t
GraphicsStateDesc::getStencilReadMask() const noexcept
{
	return _stencilReadMask;
}

std::uint32_t
GraphicsStateDesc::getStencilWriteMask() const noexcept
{
	return _stencilWriteMask;
}

GraphicsStencilOp
GraphicsStateDesc::getStencilFail() const noexcept
{
	return _stencilFail;
}

GraphicsStencilOp
GraphicsStateDesc::getStencilZFail() const noexcept
{
	return _stencilZFail;
}

GraphicsStencilOp
GraphicsStateDesc::getStencilPass() const noexcept
{
	return _stencilPass;
}

bool
GraphicsStateDesc::getStencilTwoEnable() const noexcept
{
	return _stencilTwoEnable;
}

std::int32_t
GraphicsStateDesc::getStencilTwoRef() const noexcept
{
	return _stencilTwoRef;
}

GraphicsCompareFunc
GraphicsStateDesc::getStencilTwoFunc() const noexcept
{
	return _stencilTwoFunc;
}

std::uint32_t
GraphicsStateDesc::getStencilTwoReadMask() const noexcept
{
	return _stencilTwoReadMask;
}

std::uint32_t
GraphicsStateDesc::getStencilTwoWriteMask() const noexcept
{
	return _stencilTwoWriteMask;
}

GraphicsStencilOp
GraphicsStateDesc::getStencilTwoFail() const noexcept
{
	return _stencilTwoFail;
}

GraphicsStencilOp
GraphicsStateDesc::getStencilTwoZFail() const noexcept
{
	return _stencilTwoZFail;
}

GraphicsStencilOp
GraphicsStateDesc::getStencilTwoPass() const noexcept
{
	return _stencilTwoPass;
}

GraphicsState::GraphicsState() noexcept
{
}

GraphicsState::~GraphicsState() noexcept
{
}

_NAME_END