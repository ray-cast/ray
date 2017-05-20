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
#include <ray/graphics_command.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsCommandQueue, GraphicsChild, "GraphicsCommandQueue")
__ImplementSubInterface(GraphicsCommandList, GraphicsChild, "GraphicsCommandList")
__ImplementSubInterface(GraphicsCommandPool, GraphicsChild, "GraphicsCommandPool")

GraphicsCommandQueueDesc::GraphicsCommandQueueDesc() noexcept
	: _mask(0)
	, _type(GraphicsCommandType::GraphicsCommandTypeGraphics)
	, _flags(GraphicsCommandQueueFlagBits::GraphicsCommandQueueFlagNoneBit)
	, _priority(GraphicsCommandQueuePriority::GraphicsCommandQueuePriorityNormal)
{
}

GraphicsCommandQueueDesc::~GraphicsCommandQueueDesc() noexcept
{
}

void
GraphicsCommandQueueDesc::setCommandQueueMask(std::uint32_t mask) noexcept
{
	_mask = mask;
}

std::uint32_t
GraphicsCommandQueueDesc::getCommandQueueMask() const noexcept
{
	return _mask;
}

void
GraphicsCommandQueueDesc::setCommandQueueType(GraphicsCommandType type) noexcept
{
	_type = type;
}

GraphicsCommandType
GraphicsCommandQueueDesc::getCommandQueueType() const noexcept
{
	return _type;
}

void
GraphicsCommandQueueDesc::setCommandQueueFlags(GraphicsCommandQueueFlags flags) noexcept
{
	_flags = flags;
}

GraphicsCommandQueueFlags
GraphicsCommandQueueDesc::getCommandQueueFlags() const noexcept
{
	return _flags;
}

void
GraphicsCommandQueueDesc::setCommandQueuePriority(GraphicsCommandQueuePriority priority) noexcept
{
	_priority = priority;
}

GraphicsCommandQueuePriority
GraphicsCommandQueueDesc::getCommandQueuePriority() const noexcept
{
	return _priority;
}

GraphicsCommandPoolDesc::GraphicsCommandPoolDesc() noexcept
	: _flags(GraphicsCommandPoolFlagBits::GraphicsCommandPoolFlagResetCommandBufferBit)
	, _type(GraphicsCommandType::GraphicsCommandTypeGraphics)
{
}

GraphicsCommandPoolDesc::GraphicsCommandPoolDesc(GraphicsCommandType type, std::uint32_t flags) noexcept
	: _flags(flags)
	, _type(type)
{
}

GraphicsCommandPoolDesc::~GraphicsCommandPoolDesc() noexcept
{
}

void
GraphicsCommandPoolDesc::setCommandListType(GraphicsCommandType type) noexcept
{
	_type = type;
}

GraphicsCommandType
GraphicsCommandPoolDesc::getCommandListType() const noexcept
{
	return _type;
}

void
GraphicsCommandPoolDesc::setCommandFlags(std::uint32_t flags) noexcept
{
	_flags = flags;
}

std::uint32_t
GraphicsCommandPoolDesc::getCommandFlags() const noexcept
{
	return _flags;
}

GraphicsCommandListDesc::GraphicsCommandListDesc() noexcept
{
}

GraphicsCommandListDesc::~GraphicsCommandListDesc() noexcept
{
}

void
GraphicsCommandListDesc::setGraphicsCommandPool(GraphicsCommandPoolPtr pool) noexcept
{
	_pool = pool;
}

GraphicsCommandPoolPtr
GraphicsCommandListDesc::getGraphicsCommandPool() const noexcept
{
	return _pool;
}

GraphicsCommandPool::GraphicsCommandPool() noexcept
{
}

GraphicsCommandPool::~GraphicsCommandPool() noexcept
{
}

GraphicsCommandList::GraphicsCommandList() noexcept
{
}

GraphicsCommandList::~GraphicsCommandList() noexcept
{
}

GraphicsCommandQueue::GraphicsCommandQueue() noexcept
{
}

GraphicsCommandQueue::~GraphicsCommandQueue() noexcept
{
}

_NAME_END