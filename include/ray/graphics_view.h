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
#ifndef _H_GRAPHICS_VIEW_H_
#define _H_GRAPHICS_VIEW_H_

#include <ray/graphics_child.h>

_NAME_BEGIN

class EXPORT GraphicsRenderTextureDesc final
{
public:
	GraphicsRenderTextureDesc() noexcept;
	virtual ~GraphicsRenderTextureDesc() noexcept;

	void setWidth(std::uint32_t w) noexcept;
	void setHeight(std::uint32_t h) noexcept;

	std::uint32_t getWidth() const noexcept;
	std::uint32_t getHeight() const noexcept;

	void attach(GraphicsTexturePtr texture) noexcept;
	void detach(GraphicsTexturePtr texture) noexcept;

	GraphicsTextures& getTextures() noexcept;
	const GraphicsTextures& getTextures() const noexcept;

	void setSharedDepthTexture(GraphicsTexturePtr target) noexcept;
	void setSharedStencilTexture(GraphicsTexturePtr target) noexcept;

	GraphicsTexturePtr getSharedDepthTexture() const noexcept;
	GraphicsTexturePtr getSharedStencilTexture() const noexcept;
	
private:
	std::uint32_t _width;
	std::uint32_t _height;

	GraphicsTextures _textures;
	GraphicsTexturePtr _sharedDepthTexture;
	GraphicsTexturePtr _sharedStencilTexture;
};

class EXPORT GraphicsRenderTexture : public GraphicsChild
{
	__DeclareSubInterface(GraphicsRenderTexture, GraphicsResource)
public:
	GraphicsRenderTexture() noexcept;
	virtual ~GraphicsRenderTexture() noexcept;

	virtual const GraphicsRenderTextureDesc& getGraphicsRenderTextureDesc() const noexcept = 0;

private:
	GraphicsRenderTexture(GraphicsRenderTexture&) noexcept = delete;
	GraphicsRenderTexture& operator=(const GraphicsRenderTexture&) noexcept = delete;
};

_NAME_END

#endif