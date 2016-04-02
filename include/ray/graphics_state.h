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

struct EXPORT RenderBlendState
{
	bool           blendEnable;
	bool           blendSeparateEnable;

	GraphicsBlendOp     blendOp;
	GraphicsBlendFactor blendSrc;
	GraphicsBlendFactor blendDest;

	GraphicsBlendOp     blendAlphaOp;
	GraphicsBlendFactor blendAlphaSrc;
	GraphicsBlendFactor blendAlphaDest;

	GraphicsColorMask      colorWriteMask;

	RenderBlendState() noexcept;
};

struct EXPORT RenderRasterState
{
	GraphicsCullMode    cullMode;
	GraphicsPolygonMode polygonMode;
	GraphicsVertexType  primitiveType;
	GraphicsFrontFace   frontFace;
	bool                scissorTestEnable;
	bool                srgbEnable;
	bool                multisampleEnable;
	bool                rasterizerDiscardEnable;

	RenderRasterState() noexcept;
};

struct EXPORT RenderDepthState
{
	bool                depthEnable;
	bool                depthWriteEnable;
	bool                depthBoundsEnable;
	float               depthMin;
	float               depthMax;
	GraphicsCompareFunc depthFunc;

	bool                depthBiasEnable;
	float               depthBias;
	float               depthSlopeScaleBias;

	bool				depthClampEnable;
	bool				depthBiasClamp;

	RenderDepthState() noexcept;
};

struct EXPORT RenderStencilState
{
	bool                stencilEnable;
	int                 stencilRef;
	GraphicsCompareFunc stencilFunc;
	unsigned int        stencilReadMask;
	unsigned int        stencilWriteMask;
	GraphicsStencilOp   stencilFail;
	GraphicsStencilOp   stencilZFail;
	GraphicsStencilOp   stencilPass;

	bool                stencilTwoEnable;
	int                 stencilTwoRef;
	GraphicsCompareFunc stencilTwoFunc;
	unsigned int        stencilTwoReadMask;
	unsigned int        stencilTwoWriteMask;
	GraphicsStencilOp   stencilTwoFail;
	GraphicsStencilOp   stencilTwoZFail;
	GraphicsStencilOp   stencilTwoPass;

	RenderStencilState() noexcept;
};

class EXPORT GraphicsStateDesc final
{
public:
	GraphicsStateDesc() noexcept;
	~GraphicsStateDesc() noexcept;

	void setBlendState(const RenderBlendState& state) noexcept;
	void setRasterState(const RenderRasterState& state) noexcept;
	void setDepthState(const RenderDepthState& state) noexcept;
	void setStencilState(const RenderStencilState& state) noexcept;

	RenderBlendState& getBlendState() noexcept;
	RenderRasterState& getRasterState() noexcept;
	RenderDepthState& getDepthState() noexcept;
	RenderStencilState& getStencilState() noexcept;

	const RenderBlendState& getBlendState() const noexcept;
	const RenderRasterState& getRasterState() const noexcept;
	const RenderDepthState& getDepthState() const noexcept;
	const RenderStencilState& getStencilState() const noexcept;

private:
	RenderBlendState _blendState;
	RenderRasterState _rasterState;
	RenderDepthState _depthState;
	RenderStencilState _stencilState;
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