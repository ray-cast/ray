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
#ifndef _H_GRAPHICS_DATA_H_
#define _H_GRAPHICS_DATA_H_

#include <ray/graphics_resource.h>

_NAME_BEGIN

class EXPORT GraphicsDataDesc final
{
public:
	GraphicsDataDesc() noexcept;
	GraphicsDataDesc(GraphicsDataType type, GraphicsUsageFlags usage, const void* data, std::uint32_t size, std::uint32_t stride) noexcept;
	~GraphicsDataDesc() noexcept;

	void setType(GraphicsDataType type) noexcept;
	GraphicsDataType getType() const noexcept;

	void setUsage(GraphicsUsageFlags usage) noexcept;
	GraphicsUsageFlags getUsage() const noexcept;

	void setStride(std::uint32_t stride) noexcept;
	std::uint32_t getStride() const noexcept;

	void setStream(std::uint8_t* data) noexcept;
	std::uint8_t* getStream() const noexcept;

	void setStreamSize(std::uint32_t size) noexcept;
	std::uint32_t getStreamSize() const noexcept;

private:
	std::uint32_t _stride;
	std::uint8_t* _data;
	std::uint32_t _dataSize;
	GraphicsUsageFlags _usage;
	GraphicsDataType _type;
};

class EXPORT GraphicsData : public GraphicsResource
{
	__DeclareSubInterface(GraphicsData, GraphicsResource)
public:
	GraphicsData() noexcept;
	virtual ~GraphicsData() noexcept;

	virtual bool map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept = 0;
	virtual void unmap() noexcept = 0;

	virtual const GraphicsDataDesc& getGraphicsDataDesc() const noexcept = 0;

private:
	GraphicsData(const GraphicsData&) = delete;
	GraphicsData& operator=(const GraphicsData&) = delete;
};

_NAME_END

#endif