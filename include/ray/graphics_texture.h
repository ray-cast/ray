// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#ifndef _H_RENDER_TEXTURE_H_
#define _H_RENDER_TEXTURE_H_

#include <ray/graphics_resource.h>

_NAME_BEGIN

class EXPORT GraphicsTextureDesc final
{
public:
	GraphicsTextureDesc() noexcept;
	~GraphicsTextureDesc() noexcept;
	
	void setWidth(std::uint32_t w) noexcept;
	void setHeight(std::uint32_t h) noexcept;
	void setDepth(std::uint32_t d) noexcept;
	void setSize(std::uint32_t w, std::uint32_t h, std::uint32_t depth = 0) noexcept;
	std::uint32_t getWidth() const noexcept;
	std::uint32_t getHeight() const noexcept;
	std::uint32_t getDepth()  const noexcept;
	const uint3& getSize() const noexcept;

	void setTexFormat(GraphicsFormat format) noexcept;
	void setTexDim(GraphicsTextureDim mapping) noexcept;
	void setTexUsage(std::uint32_t flags) noexcept;
	GraphicsFormat getTexFormat()  const noexcept;
	GraphicsTextureDim getTexDim() const noexcept;
	std::uint32_t getTexUsage() const noexcept;

	void setSamplerWrap(GraphicsSamplerWrap wrap) noexcept;
	void setSamplerFilter(GraphicsSamplerFilter filter) noexcept;
	void setSamplerAnis(GraphicsSamplerAnis anis) noexcept;
	GraphicsSamplerWrap getSamplerWrap() const noexcept;
	GraphicsSamplerFilter getSamplerFilter() const noexcept;
	GraphicsSamplerAnis getSamplerAnis() const noexcept;

	void setMultisample(bool enable) noexcept;
	bool isMultiSample() const noexcept;

	void setLayerNums(std::uint32_t layer) noexcept;
	void setLayerBase(std::uint32_t minLayer) noexcept;
	std::uint32_t getLayerBase() const noexcept;
	std::uint32_t getLayerNums() const noexcept;

	void setMipLevel(std::uint32_t level) noexcept;
	void setMipBase(std::uint32_t minLevel) noexcept;
	std::uint32_t getMipLevel() const noexcept;
	std::uint32_t getMipBase() const noexcept;

	void setStream(void* data) noexcept;
	void setStreamSize(std::uint32_t size) noexcept;
	void* getStream() const noexcept;
	std::uint32_t getStreamSize() const noexcept;

private:
	std::string _name;

	bool _multisample;

	uint3 _size;

	std::uint32_t _layerBase;
	std::uint32_t _layer;

	std::uint32_t _mipLevel;
	std::uint32_t _mipBase;

	std::uint32_t _textureUsage;

	GraphicsFormat _format;
	GraphicsTextureDim _dim;
	GraphicsSamplerOp _texop;
	GraphicsSamplerFilter _filter;
	GraphicsSamplerWrap _wrap;
	GraphicsSamplerAnis _anis;

	void* _data;
	std::uint32_t _dataSize;
};

class EXPORT GraphicsTexture : public GraphicsResource
{
	__DeclareSubInterface(RenderTexture, GraphicsResource)
public:
	GraphicsTexture() noexcept;
	virtual ~GraphicsTexture() noexcept;

	virtual const GraphicsTextureDesc& getGraphicsTextureDesc() const noexcept = 0;

private:
	GraphicsTexture(const GraphicsTexture&) noexcept = delete;
	GraphicsTexture& operator=(const GraphicsTexture&) noexcept = delete;
};

_NAME_END

#endif