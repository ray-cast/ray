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
#ifndef _H_GRAPHICS_STATE_H_
#define _H_GRAPHICS_STATE_H_

#include <ray/graphics_child.h>

_NAME_BEGIN

class EXPORT GraphicsStateDesc final
{
public:
	GraphicsStateDesc() noexcept;
	~GraphicsStateDesc() noexcept;

	void setBlendEnable(bool enable) noexcept;
	void setBlendSeparateEnable(bool enable) noexcept;
	void setBlendOp(GraphicsBlendOp blendop) noexcept;
	void setBlendSrc(GraphicsBlendFactor factor) noexcept;
	void setBlendDest(GraphicsBlendFactor factor) noexcept;
	void setBlendAlphaOp(GraphicsBlendOp blendop) noexcept;
	void setBlendAlphaSrc(GraphicsBlendFactor factor) noexcept;
	void setBlendAlphaDest(GraphicsBlendFactor factor) noexcept;

	void setColorWriteMask(GraphicsColorMask mask) noexcept;
	void setCullMode(GraphicsCullMode mode) noexcept;
	void setPolygonMode(GraphicsPolygonMode mode) noexcept;
	void setPrimitiveType(GraphicsVertexType type) noexcept;
	void setFrontFace(GraphicsFrontFace face) noexcept;
	void setScissorTestEnable(bool enable) noexcept;
	void setLinear2sRGBEnable(bool enable) noexcept;
	void setMultisampleEnable(bool enable) noexcept;
	void setRasterizerDiscardEnable(bool enable) noexcept;

	void setDepthEnable(bool enable) noexcept;
	void setDepthWriteEnable(bool enable) noexcept;
	void setDepthBoundsEnable(bool enable) noexcept;
	void setDepthMin(float mix) noexcept;
	void setDepthMax(float max) noexcept;
	void setDepthFunc(GraphicsCompareFunc func) noexcept;
	void setDepthBiasEnable(bool enable) noexcept;
	void setDepthBias(float bias) noexcept;
	void setDepthSlopeScaleBias(float bias) noexcept;
	void setDepthBiasClamp(bool bias) noexcept;
	void setDepthClampEnable(bool enable) noexcept;

	void setStencilEnable(bool enable) noexcept;
	void setStencilFrontRef(std::uint32_t ref) noexcept;
	void setStencilFrontFunc(GraphicsCompareFunc func) noexcept;
	void setStencilFrontReadMask(std::uint32_t mask) noexcept;
	void setStencilFrontWriteMask(std::uint32_t mask) noexcept;
	void setStencilFrontFail(GraphicsStencilOp stencilOp) noexcept;
	void setStencilFrontZFail(GraphicsStencilOp stencilOp) noexcept;
	void setStencilFrontPass(GraphicsStencilOp stencilOp) noexcept;
	void setStencilBackRef(std::uint32_t ref) noexcept;
	void setStencilBackFunc(GraphicsCompareFunc func) noexcept;
	void setStencilBackReadMask(std::uint32_t mask) noexcept;
	void setStencilBackWriteMask(std::uint32_t mask) noexcept;
	void setStencilBackFail(GraphicsStencilOp stencilOp) noexcept;
	void setStencilBackZFail(GraphicsStencilOp stencilOp) noexcept;
	void setStencilBackPass(GraphicsStencilOp stencilOp) noexcept;

	bool getBlendEnable() const noexcept;
	bool getBlendSeparateEnable() const noexcept;
	GraphicsBlendOp getBlendOp() const noexcept;
	GraphicsBlendFactor getBlendSrc() const noexcept;
	GraphicsBlendFactor getBlendDest() const noexcept;
	GraphicsBlendOp getBlendAlphaOp() const noexcept;
	GraphicsBlendFactor getBlendAlphaSrc() const noexcept;
	GraphicsBlendFactor getBlendAlphaDest() const noexcept;

	GraphicsColorMask getColorWriteMask() const noexcept;
	GraphicsCullMode getCullMode() const noexcept;
	GraphicsPolygonMode getPolygonMode() const noexcept;
	GraphicsVertexType getPrimitiveType() const noexcept;
	GraphicsFrontFace getFrontFace() const noexcept;
	bool getScissorTestEnable() const noexcept;
	bool getLinear2sRGBEnable() const noexcept;
	bool getMultisampleEnable() const noexcept;
	bool getRasterizerDiscardEnable() const noexcept;

	bool getDepthEnable() const noexcept;
	bool getDepthWriteEnable() const noexcept;
	bool getDepthBoundsEnable() const noexcept;
	bool getDepthBiasEnable() const noexcept;
	bool getDepthBiasClamp() const noexcept;
	bool getDepthClampEnable() const noexcept;
	float getDepthMin() const noexcept;
	float getDepthMax() const noexcept;
	float getDepthBias() const noexcept;
	float getDepthSlopeScaleBias() const noexcept;
	GraphicsCompareFunc getDepthFunc() const noexcept;

	bool getStencilEnable() const noexcept;
	GraphicsCompareFunc getStencilFrontFunc() const noexcept;
	std::uint32_t getStencilFrontRef() const noexcept;
	std::uint32_t getStencilFrontReadMask() const noexcept;
	std::uint32_t getStencilFrontWriteMask() const noexcept;
	GraphicsStencilOp getStencilFrontFail() const noexcept;
	GraphicsStencilOp getStencilFrontZFail() const noexcept;
	GraphicsStencilOp getStencilFrontPass() const noexcept;
	GraphicsCompareFunc getStencilBackFunc() const noexcept;
	std::uint32_t getStencilBackRef() const noexcept;
	std::uint32_t getStencilBackReadMask() const noexcept;
	std::uint32_t getStencilBackWriteMask() const noexcept;
	GraphicsStencilOp getStencilBackFail() const noexcept;
	GraphicsStencilOp getStencilBackZFail() const noexcept;
	GraphicsStencilOp getStencilBackPass() const noexcept;

private:
	bool _blendEnable;
	bool _blendSeparateEnable;
	bool _scissorTestEnable;
	bool _srgbEnable;
	bool _multisampleEnable;
	bool _rasterizerDiscardEnable;
	bool _depthEnable;
	bool _depthWriteEnable;
	bool _depthBoundsEnable;
	bool _depthClampEnable;
	bool _depthBiasEnable;
	bool _depthBiasClamp;
	bool _stencilEnable;

	GraphicsBlendOp _blendOp;
	GraphicsBlendOp _blendAlphaOp;

	GraphicsBlendFactor _blendSrc;
	GraphicsBlendFactor _blendDest;
	GraphicsBlendFactor _blendAlphaSrc;
	GraphicsBlendFactor _blendAlphaDest;

	GraphicsColorMask _colorWriteMask;

	GraphicsCullMode    _cullMode;
	GraphicsPolygonMode _polygonMode;
	GraphicsVertexType  _primitiveType;
	GraphicsFrontFace   _frontFace;

	float _depthMin;
	float _depthMax;
	float _depthBias;
	float _depthSlopeScaleBias;
	GraphicsCompareFunc _depthFunc;

	std::uint32_t _stencilFrontRef;
	std::uint32_t _stencilFrontReadMask;
	std::uint32_t _stencilFrontWriteMask;
	GraphicsStencilOp _stencilFrontFail;
	GraphicsStencilOp _stencilFrontZFail;
	GraphicsStencilOp _stencilFrontPass;
	GraphicsCompareFunc _stencilFrontFunc;

	std::uint32_t _stencilBackRef;
	std::uint32_t _stencilBackReadMask;
	std::uint32_t _stencilBackWriteMask;
	GraphicsStencilOp _stencilBackFail;
	GraphicsStencilOp _stencilBackZFail;
	GraphicsStencilOp _stencilBackPass;
	GraphicsCompareFunc _stencilBackFunc;
};

class EXPORT GraphicsState : public GraphicsChild
{
	__DeclareSubInterface(GraphicsState, GraphicsChild)
public:
	GraphicsState() noexcept;
	virtual ~GraphicsState() noexcept;

	virtual const GraphicsStateDesc& getGraphicsStateDesc() const noexcept = 0;

private:
	GraphicsState(const GraphicsState&) noexcept = delete;
	GraphicsState& operator=(const GraphicsState&) noexcept = delete;
};

_NAME_END

#endif