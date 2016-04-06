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
#include "vk_graphics_data.h"
#include "vk_device.h"
#include "vk_system.h"

_NAME_BEGIN

__ImplementSubClass(VulkanGraphicsData, GraphicsData, "VulkanGraphicsData")

VulkanGraphicsData::VulkanGraphicsData() noexcept
	: _vkBuffer(VK_NULL_HANDLE)
{
}

VulkanGraphicsData::~VulkanGraphicsData() noexcept
{
	this->close();
}

bool 
VulkanGraphicsData::setup(const GraphicsDataDesc& dataDesc) noexcept
{
	assert(_vkBuffer == VK_NULL_HANDLE);

	auto type = dataDesc.getType();
	if (type == GraphicsDataType::GraphicsDataTypeNone)
	{
		VK_PLATFORM_LOG("Unknown data type.");
		return false;
	}

	auto streamSize = dataDesc.getStreamSize();
	if (streamSize == 0)
	{
		VK_PLATFORM_LOG("Empty stream size");
		return false;
	}

	VkBufferCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.pNext = nullptr;
	info.size = streamSize;
	info.usage = VulkanTypes::asBufferUsageFlagBits(type);
	info.flags = 0;
	info.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
	info.queueFamilyIndexCount = 0;
	info.pQueueFamilyIndices = nullptr;

	if (vkCreateBuffer(this->getDevice()->downcast<VulkanDevice>()->getDevice(), &info, nullptr, &_vkBuffer) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkCreateBuffer() fail.");
		return false;
	}

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkBuffer, &memReq);

	if (!_memory.setup(nullptr, memReq.size, memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
		return false;

	auto stream = dataDesc.getStream();	
	if (stream != nullptr)
	{
		void* data = _memory.map(0, streamSize, GraphicsAccessFlagsBits::GraphicsAccessFlagsMapReadBit);
		if (data)
		{
			std::memcpy(data, stream, streamSize);
			_memory.unmap();
		}
	}

	if (vkBindBufferMemory(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkBuffer, _memory.getDeviceMemory(), 0) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkBindBufferMemory() fail.");
		return false;
	}

	return true;
}

void 
VulkanGraphicsData::close() noexcept
{
	if (_vkBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkBuffer, nullptr);
		_vkBuffer = VK_NULL_HANDLE;
	}
}

VkBuffer
VulkanGraphicsData::getBuffer() const noexcept
{
	return _vkBuffer;
}

void
VulkanGraphicsData::setDevice(GraphicsDevicePtr device) noexcept
{
	_memory.setDevice(device);
}

GraphicsDevicePtr
VulkanGraphicsData::getDevice() noexcept
{
	return _memory.getDevice();
}

const GraphicsDataDesc&
VulkanGraphicsData::getGraphicsDataDesc() const noexcept
{
	return _dataDesc;
}

_NAME_END