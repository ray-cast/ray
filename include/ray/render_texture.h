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

#include <ray/graphics_sampler.h>

_NAME_BEGIN

class EXPORT Texture
{
public:
	Texture() noexcept;
	virtual ~Texture() noexcept;

	virtual bool setup() except = 0;
	virtual void close() noexcept = 0;

	void setTexMipmap(bool enable) noexcept;
	void setTexFormat(TextureFormat format) noexcept;
	void setTexOp(SamplerOp op) noexcept;
	void setSamplerWrap(SamplerWrap wrap) noexcept;
	void setSamplerFilter(SamplerFilter filter) noexcept;
	void setTexDim(TextureDim mapping) noexcept;
	void setMipLevel(std::uint8_t level) noexcept;
	void setMipSize(std::uint32_t size) noexcept;
	void setSamplerAnis(SamplerAnis anis) noexcept;
	void setMultisample(bool enable) noexcept;
	void setWidth(int w) noexcept;
	void setHeight(int h) noexcept;
	void setDepth(int d) noexcept;
	void setStream(void* data) noexcept;
	void setSize(int w, int h, int depth = 0) noexcept;

	SamplerOp     getTexOp()   const noexcept;
	TextureFormat   getTexFormat()  const noexcept;
	TextureDim    getTexDim() const noexcept;
	SamplerWrap   getSamplerWrap() const noexcept;
	SamplerFilter getSamplerFilter() const noexcept;
	SamplerAnis    getSamplerAnis() const noexcept;

	int getWidth()   const noexcept;
	int getHeight()  const noexcept;
	int getDepth()   const noexcept;
	const int3& getSize()  const noexcept;

	std::int32_t getMipLevel() const noexcept;
	std::uint32_t getMipSize() const noexcept;

	void* getStream() const noexcept;

	bool isMipmap() const noexcept;
	bool isMultiSample() const noexcept;

	void copy(TexturePtr other) noexcept;
	void copy(const Texture& other) noexcept;

private:
	Texture(const Texture&) noexcept = delete;
	Texture& operator=(const Texture&) noexcept = delete;

private:

	bool _mipmap;
	bool _multisample;

	int3 _size;

	std::int32_t _mipLevel;
	std::uint32_t _mipSize;

	TextureFormat _format;
	TextureDim _dim;
	SamplerOp _texop;
	SamplerFilter _filter;
	SamplerWrap _wrap;
	SamplerAnis _anis;

	void* _data;
};

class EXPORT RenderTexture
{
public:
	RenderTexture() noexcept;
	virtual ~RenderTexture() noexcept;

	virtual bool setup(TexturePtr texture) except = 0;
	virtual void setup(std::size_t w, std::size_t h, TextureDim dim, TextureFormat format) except = 0;
	virtual void setup(std::size_t w, std::size_t h, std::size_t d, TextureDim dim, TextureFormat format) except = 0;

	virtual void close() noexcept = 0;

	TextureDim getTexDim() const noexcept;
	TextureFormat getTexFormat()  const noexcept;

	std::size_t getWidth()  const noexcept;
	std::size_t getHeight() const noexcept;
	std::size_t getDepth()  const noexcept;

	bool isMipmap() const noexcept;
	bool isMultiSample() const noexcept;

	TexturePtr getResolveTexture() const noexcept;

	void setSharedDepthTexture(RenderTexturePtr target) noexcept;
	void setSharedStencilTexture(RenderTexturePtr target) noexcept;

	RenderTexturePtr getSharedDepthTexture() const noexcept;
	RenderTexturePtr getSharedStencilTexture() const noexcept;

protected:

	virtual void onSetRenderTextureBefore(RenderTexturePtr target) noexcept;
	virtual void onSetRenderTextureAfter(RenderTexturePtr target) noexcept;

private:
	RenderTexture(RenderTexture&) noexcept = delete;
	RenderTexture& operator=(const RenderTexture&)noexcept = delete;

protected:

	TexturePtr _resolveTexture;

	RenderTexturePtr _sharedDepthTexture;
	RenderTexturePtr _sharedStencilTexture;
};

class EXPORT MultiRenderTexture
{
public:
	MultiRenderTexture() noexcept;
	virtual ~MultiRenderTexture() noexcept;

	virtual bool setup() noexcept = 0;
	virtual void close() noexcept = 0;

	virtual void attach(RenderTexturePtr texture) noexcept;
	virtual void detach(RenderTexturePtr texture) noexcept;

	virtual RenderTextures& getRenderTextures() noexcept;
	virtual const RenderTextures& getRenderTextures() const noexcept;

	virtual void setSharedDepthTexture(RenderTexturePtr target) noexcept;
	virtual void setSharedStencilTexture(RenderTexturePtr target) noexcept;

	virtual RenderTexturePtr getSharedDepthTexture() const noexcept;
	virtual RenderTexturePtr getSharedStencilTexture() const noexcept;

private:
	MultiRenderTexture(MultiRenderTexture&) noexcept = delete;
	MultiRenderTexture& operator=(const MultiRenderTexture&) noexcept = delete;

protected:

	RenderTextures _textures;

	RenderTexturePtr _sharedDepthTexture;
	RenderTexturePtr _sharedStencilTexture;
};

_NAME_END

#endif