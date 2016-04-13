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
	void setsRGBEnable(bool enable) noexcept;
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
	void setStencilRef(std::int32_t ref) noexcept;
	void setStencilFunc(GraphicsCompareFunc func) noexcept;
	void setStencilReadMask(std::uint32_t mask) noexcept;
	void setStencilWriteMask(std::uint32_t mask) noexcept;
	void setStencilFail(GraphicsStencilOp stencilOp) noexcept;
	void setStencilZFail(GraphicsStencilOp stencilOp) noexcept;
	void setStencilPass(GraphicsStencilOp stencilOp) noexcept;
	void setStencilTwoEnable(bool enable) noexcept;
	void setStencilTwoRef(std::int32_t ref) noexcept;
	void setStencilTwoFunc(GraphicsCompareFunc func) noexcept;
	void setStencilTwoReadMask(std::uint32_t mask) noexcept;
	void setStencilTwoWriteMask(std::uint32_t mask) noexcept;
	void setStencilTwoFail(GraphicsStencilOp stencilOp) noexcept;
	void setStencilTwoZFail(GraphicsStencilOp stencilOp) noexcept;
	void setStencilTwoPass(GraphicsStencilOp stencilOp) noexcept;

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
	bool getsRGBEnable() const noexcept;
	bool getMultisampleEnable() const noexcept;
	bool getRasterizerDiscardEnable() const noexcept;

	bool getDepthEnable() const noexcept;
	bool getDepthWriteEnable() const noexcept;
	bool getDepthBoundsEnable() const noexcept;
	float getDepthMin() const noexcept;
	float getDepthMax() const noexcept;
	GraphicsCompareFunc getDepthFunc() const noexcept;
	bool getDepthBiasEnable() const noexcept;
	float getDepthBias() const noexcept;
	float getDepthSlopeScaleBias() const noexcept;
	bool getDepthBiasClamp() const noexcept;
	bool getDepthClampEnable() const noexcept;

	bool getStencilEnable() const noexcept;
	std::int32_t getStencilRef() const noexcept;
	GraphicsCompareFunc getStencilFunc() const noexcept;
	std::uint32_t getStencilReadMask() const noexcept;
	std::uint32_t getStencilWriteMask() const noexcept;
	GraphicsStencilOp getStencilFail() const noexcept;
	GraphicsStencilOp getStencilZFail() const noexcept;
	GraphicsStencilOp getStencilPass() const noexcept;
	bool getStencilTwoEnable() const noexcept;
	std::int32_t getStencilTwoRef() const noexcept;
	GraphicsCompareFunc getStencilTwoFunc() const noexcept;
	std::uint32_t getStencilTwoReadMask() const noexcept;
	std::uint32_t getStencilTwoWriteMask() const noexcept;
	GraphicsStencilOp getStencilTwoFail() const noexcept;
	GraphicsStencilOp getStencilTwoZFail() const noexcept;
	GraphicsStencilOp getStencilTwoPass() const noexcept;

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
	bool _stencilTwoEnable;

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

	std::int32_t _stencilRef;
	std::int32_t _stencilTwoRef;

	std::uint32_t _stencilReadMask;
	std::uint32_t _stencilWriteMask;
	std::uint32_t _stencilTwoReadMask;
	std::uint32_t _stencilTwoWriteMask;

	GraphicsCompareFunc _stencilFunc;
	GraphicsCompareFunc _stencilTwoFunc;

	GraphicsStencilOp _stencilFail;
	GraphicsStencilOp _stencilZFail;
	GraphicsStencilOp _stencilPass;
	
	GraphicsStencilOp _stencilTwoFail;
	GraphicsStencilOp _stencilTwoZFail;
	GraphicsStencilOp _stencilTwoPass;
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