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
#include <ray/graphics_descriptor.h>
#include <ray/graphics_variant.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsUniformSet, rtti::Interface, "GraphicsUniformSet")
__ImplementSubInterface(GraphicsDescriptorPool, GraphicsChild, "GraphicsDescriptorPool")
__ImplementSubInterface(GraphicsDescriptorSet, GraphicsChild, "GraphicsDescriptorSet")
__ImplementSubInterface(GraphicsDescriptorSetLayout, GraphicsChild, "GraphicsDescriptorSetLayout")

GraphicsUniformSet::GraphicsUniformSet() noexcept
{
}

GraphicsUniformSet::~GraphicsUniformSet() noexcept
{
}

GraphicsDescriptorSetLayoutDesc::GraphicsDescriptorSetLayoutDesc() noexcept
{
}

GraphicsDescriptorSetLayoutDesc::~GraphicsDescriptorSetLayoutDesc() noexcept
{
}

void
GraphicsDescriptorSetLayoutDesc::setUniformComponents(const GraphicsUniforms& component) noexcept
{
	_uniforms = component;
}

const GraphicsUniforms&
GraphicsDescriptorSetLayoutDesc::getUniformComponents() const noexcept
{
	return _uniforms;
}

void
GraphicsDescriptorSetLayoutDesc::addUniformComponent(GraphicsUniformPtr component) noexcept
{
	assert(component);
	auto it = std::find(_uniforms.begin(), _uniforms.end(), component);
	if (it == _uniforms.end())
		_uniforms.erase(it);
}

void
GraphicsDescriptorSetLayoutDesc::removeUniformComponent(GraphicsUniformPtr component) noexcept
{
	assert(component);
	auto it = std::find(_uniforms.begin(), _uniforms.end(), component);
	if (it != _uniforms.end())
		_uniforms.erase(it);
}

void
GraphicsDescriptorSetLayoutDesc::setUniformBlockComponents(const GraphicsUniformBlocks& component) noexcept
{
	_uniformBlocks = component;
}

const GraphicsUniformBlocks&
GraphicsDescriptorSetLayoutDesc::getUniformBlockComponents() const noexcept
{
	return _uniformBlocks;
}

void
GraphicsDescriptorSetLayoutDesc::addUniformBlockComponent(GraphicsUniformBlockPtr component) noexcept
{
	assert(component);
	auto it = std::find(_uniformBlocks.begin(), _uniformBlocks.end(), component);
	if (it == _uniformBlocks.end())
		_uniformBlocks.erase(it);
}

void
GraphicsDescriptorSetLayoutDesc::removeUniformBlockComponent(GraphicsUniformBlockPtr component) noexcept
{
	assert(component);
	auto it = std::find(_uniformBlocks.begin(), _uniformBlocks.end(), component);
	if (it != _uniformBlocks.end())
		_uniformBlocks.erase(it);
}


GraphicsDescriptorPoolComponent::GraphicsDescriptorPoolComponent() noexcept
	: _type(GraphicsUniformType::GraphicsUniformTypeNone)
	, _descriptorNums(0)
{
}

GraphicsDescriptorPoolComponent::GraphicsDescriptorPoolComponent(GraphicsUniformType type, std::uint32_t nums) noexcept
	: _type(type)
	, _descriptorNums(nums)
{
}

GraphicsDescriptorPoolComponent::~GraphicsDescriptorPoolComponent() noexcept
{
}

void
GraphicsDescriptorPoolComponent::setDescriptorType(GraphicsUniformType type) noexcept
{
	_type = type;
}

GraphicsUniformType
GraphicsDescriptorPoolComponent::getDescriptorType() const noexcept
{
	return _type;
}

void
GraphicsDescriptorPoolComponent::setDescriptorNums(std::uint32_t nums) noexcept
{
	_descriptorNums = nums;
}

std::uint32_t
GraphicsDescriptorPoolComponent::getDescriptorNums() const noexcept
{
	return _descriptorNums;
}

GraphicsDescriptorPoolDesc::GraphicsDescriptorPoolDesc() noexcept
	: _maxSets(1)
{
}

GraphicsDescriptorPoolDesc::GraphicsDescriptorPoolDesc(std::uint32_t maxSets) noexcept
	: _maxSets(maxSets)
{
}

GraphicsDescriptorPoolDesc::~GraphicsDescriptorPoolDesc() noexcept
{
}

void
GraphicsDescriptorPoolDesc::setMaxSets(std::uint32_t maxsets) noexcept
{
	_maxSets = maxsets;
}

std::uint32_t
GraphicsDescriptorPoolDesc::getMaxSets() const noexcept
{
	return _maxSets;
}

const GraphicsDescriptorPoolComponents&
GraphicsDescriptorPoolDesc::getGraphicsDescriptorPoolComponents() const noexcept
{
	return _poolSizes;
}

void
GraphicsDescriptorPoolDesc::addGraphicsDescriptorPoolComponent(const GraphicsDescriptorPoolComponent& component) noexcept
{
	auto it = _poolSizes.begin();
	auto end = _poolSizes.begin();
	for (; it != end; ++it)
	{
		if (it->getDescriptorType() == component.getDescriptorType())
		{
			_poolSizes.erase(it);
			return;
		}
	}

	_poolSizes.push_back(component);
}

GraphicsDescriptorSetDesc::GraphicsDescriptorSetDesc() noexcept
{
}

GraphicsDescriptorSetDesc::~GraphicsDescriptorSetDesc() noexcept
{
}

void
GraphicsDescriptorSetDesc::setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr layout) noexcept
{
	_layout = layout;
}

GraphicsDescriptorSetLayoutPtr
GraphicsDescriptorSetDesc::getGraphicsDescriptorSetLayout() const noexcept
{
	return _layout;
}

void
GraphicsDescriptorSetDesc::setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr pool) noexcept
{
	_pool = pool;
}

GraphicsDescriptorPoolPtr
GraphicsDescriptorSetDesc::getGraphicsDescriptorPool() const noexcept
{
	return _pool;
}

GraphicsDescriptorSetLayout::GraphicsDescriptorSetLayout() noexcept
{
}

GraphicsDescriptorSetLayout::~GraphicsDescriptorSetLayout() noexcept
{
}

GraphicsDescriptorSet::GraphicsDescriptorSet() noexcept
{
}

GraphicsDescriptorSet::~GraphicsDescriptorSet() noexcept
{
}

GraphicsDescriptorPool::GraphicsDescriptorPool() noexcept
{
}

GraphicsDescriptorPool::~GraphicsDescriptorPool() noexcept
{
}

_NAME_END