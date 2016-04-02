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
#ifndef _H_GRAPHICS_COMMAND_H_
#define _H_GRAPHICS_COMMAND_H_

#include <ray/graphics_child.h>

_NAME_BEGIN

class ClearValueRect
{
public:
	std::int32_t x;
	std::int32_t y;
	std::int32_t w;
	std::int32_t h;
	std::uint32_t first;
	std::uint32_t count;
};

class ClearValue
{
public:
	GraphicsClearFlags flags;
	ClearValueRect* rect;
	float4 color;
	float depth;
	std::int32_t stencil;
};

class GraphicsCommandQueueDesc
{
public:
	GraphicsCommandListType type;
	GraphicsCommandQueueFlags flags;
	GraphicsCommandQueuePriority priority;
	std::uint32_t mask;
};

class GraphicsCommandPoolDesc
{
public:
	GraphicsCommandPoolDesc() noexcept;
	GraphicsCommandPoolDesc(GraphicsCommandListType type, std::uint32_t flags) noexcept;
	~GraphicsCommandPoolDesc() noexcept;

	void setCommandListType(GraphicsCommandListType type) noexcept;
	GraphicsCommandListType getCommandListType() const noexcept;

	void setCommandFlags(std::uint32_t flags) noexcept;
	std::uint32_t getCommandFlags() const noexcept;

private:
	std::uint32_t _flags;
	GraphicsCommandListType _type;
};

class GraphicsCommandListDesc
{
public:
	GraphicsCommandListDesc() noexcept;
	~GraphicsCommandListDesc() noexcept;

	void setGraphicsCommandPool(GraphicsCommandPoolPtr pool) noexcept;
	GraphicsCommandPoolPtr getGraphicsCommandPool() const noexcept;

private:
	GraphicsCommandPoolPtr _pool;
};

class GraphicsCommandQueue : public GraphicsChild
{
	__DeclareSubClass(GraphicsCommandQueue, GraphicsChild)
public:
	GraphicsCommandQueue() noexcept;
	virtual ~GraphicsCommandQueue() noexcept;

	virtual void beginEvent() noexcept = 0;
	virtual void endEvent() noexcept = 0;

	virtual void wait() noexcept = 0;
	
	virtual void executeCommandLists(GraphicsCommandListPtr commandLists[], std::uint32_t count) noexcept = 0;

	virtual void present(GraphicsSwapchainPtr canvas[], std::uint32_t count) noexcept = 0;

	virtual const GraphicsCommandQueueDesc& getGraphicsCommandQueueDesc() const noexcept = 0;
};

class GraphicsCommandPool : public GraphicsChild
{
	__DeclareSubClass(GraphicsCommandPool, GraphicsChild)
public:
	GraphicsCommandPool() noexcept;
	virtual ~GraphicsCommandPool() noexcept;

	virtual const GraphicsCommandPoolDesc& getGraphicsCommandPoolDesc() const noexcept = 0;
};

class GraphicsCommandList : public GraphicsChild
{
	__DeclareSubClass(GraphicsCommandList, GraphicsChild)
public:
	GraphicsCommandList() noexcept;
	virtual ~GraphicsCommandList() noexcept;

	virtual void renderBegin() noexcept = 0;
	virtual void renderEnd() noexcept = 0;

	virtual void setViewport(const Viewport viewport[], std::uint32_t first, std::uint32_t count) noexcept = 0;
	virtual void setScissor(const Scissor Scissor[], std::uint32_t first, std::uint32_t count) noexcept = 0;

	virtual void clearTexture(GraphicsTexturePtr texture, const ClearValue& value) noexcept = 0;

	virtual void setRenderTexture(GraphicsFramebufferPtr framebuffer) noexcept = 0;
	virtual void clearRenderTexture(ClearValue value[], std::uint32_t first, std::uint32_t count) noexcept = 0;

	virtual void setPipeline(GraphicsPipelinePtr pipeline) noexcept = 0;
	virtual void setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept = 0;

	virtual void setVertexBuffers(GraphicsDataPtr data[], std::uint32_t first, std::uint32_t count) noexcept = 0;
	virtual void setIndexBuffer(GraphicsDataPtr data) noexcept = 0;

	virtual void drawRenderBuffer(const GraphicsIndirect& renderable) noexcept = 0;
	virtual void drawRenderBuffer(const GraphicsIndirect renderable[], std::size_t count) noexcept = 0;

	virtual const GraphicsCommandListDesc& getGraphicsCommandListDesc() const noexcept = 0;
};

_NAME_END

#endif