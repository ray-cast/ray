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
	
	void setTexMipmap(bool enable) noexcept;
	void setTexFormat(GraphicsFormat format) noexcept;
	void setTexOp(GraphicsSamplerOp op) noexcept;
	void setSamplerWrap(GraphicsSamplerWrap wrap) noexcept;
	void setSamplerFilter(GraphicsSamplerFilter filter) noexcept;
	void setTexDim(GraphicsTextureDim mapping) noexcept;
	void setMipLevel(std::uint8_t level) noexcept;
	void setMipSize(std::uint32_t size) noexcept;
	void setSamplerAnis(GraphicsSamplerAnis anis) noexcept;
	void setMultisample(bool enable) noexcept;
	void setWidth(int w) noexcept;
	void setHeight(int h) noexcept;
	void setDepth(int d) noexcept;
	void setStream(void* data) noexcept;
	void setSize(int w, int h, int depth = 0) noexcept;

	GraphicsFormat getTexFormat()  const noexcept;
	GraphicsTextureDim getTexDim() const noexcept;
	GraphicsSamplerOp getTexOp()   const noexcept;
	GraphicsSamplerWrap getSamplerWrap() const noexcept;
	GraphicsSamplerFilter getSamplerFilter() const noexcept;
	GraphicsSamplerAnis getSamplerAnis() const noexcept;

	int getWidth()   const noexcept;
	int getHeight()  const noexcept;
	int getDepth()   const noexcept;
	const int3& getSize()  const noexcept;

	std::int32_t getMipLevel() const noexcept;
	std::uint32_t getMipSize() const noexcept;

	void* getStream() const noexcept;

	bool isMipmap() const noexcept;
	bool isMultiSample() const noexcept;

private:

	bool _mipmap;
	bool _multisample;

	int3 _size;

	std::int32_t _mipLevel;
	std::uint32_t _mipSize;

	GraphicsFormat _format;
	GraphicsTextureDim _dim;
	GraphicsSamplerOp _texop;
	GraphicsSamplerFilter _filter;
	GraphicsSamplerWrap _wrap;
	GraphicsSamplerAnis _anis;

	void* _data;
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