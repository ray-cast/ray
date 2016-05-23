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
#ifndef _H_GRAPHICS_FRAMEBUFFER_H_
#define _H_GRAPHICS_FRAMEBUFFER_H_

#include <ray/graphics_child.h>

_NAME_BEGIN

class EXPORT GraphicsAttachment final
{
public:
	GraphicsAttachment() noexcept;
	GraphicsAttachment(std::uint32_t slot, GraphicsImageLayout type, GraphicsFormat format) noexcept;
	~GraphicsAttachment() noexcept;

	void setAttachSlot(std::uint32_t slot) noexcept;
	std::uint32_t getAttachSlot() const noexcept;

	void setAttachType(GraphicsImageLayout layout) noexcept;
	GraphicsImageLayout getAttachType() const noexcept;

	void setAttachFormat(GraphicsFormat format) noexcept;
	GraphicsFormat getAttachFormat() const noexcept;

private:
	std::uint32_t _slot;
	GraphicsFormat _format;
	GraphicsImageLayout _type;
};

class EXPORT GraphicsFramebufferLayoutDesc final
{
public:
	GraphicsFramebufferLayoutDesc() noexcept;
	~GraphicsFramebufferLayoutDesc() noexcept;

	void addComponent(const GraphicsAttachment& component) noexcept;

	void setComponents(const GraphicsAttachments& components) noexcept;
	const GraphicsAttachments& getComponents() const noexcept;

private:
	GraphicsAttachments _components;
};

class EXPORT GraphicsFramebufferDesc final
{
public:
	GraphicsFramebufferDesc() noexcept;
	virtual ~GraphicsFramebufferDesc() noexcept;

	void setWidth(std::uint32_t w) noexcept;
	void setHeight(std::uint32_t h) noexcept;

	std::uint32_t getWidth() const noexcept;
	std::uint32_t getHeight() const noexcept;

	void attach(GraphicsTexturePtr texture) noexcept;
	void detach(GraphicsTexturePtr texture) noexcept;

	GraphicsTextures& getTextures() noexcept;
	const GraphicsTextures& getTextures() const noexcept;

	void setSharedDepthStencilTexture(GraphicsTexturePtr target) noexcept;
	GraphicsTexturePtr getSharedDepthStencilTexture() const noexcept;

	void setGraphicsFramebufferLayout(GraphicsFramebufferLayoutPtr layout) noexcept;
	GraphicsFramebufferLayoutPtr getGraphicsFramebufferLayout() const noexcept;

private:
	std::uint32_t _width;
	std::uint32_t _height;

	GraphicsTextures _textures;
	GraphicsTexturePtr _sharedDepthStencilTexture;
	GraphicsFramebufferLayoutPtr _framebufferLayout;
};

class EXPORT GraphicsFramebufferLayout : public GraphicsChild
{
	__DeclareSubInterface(GraphicsFramebufferLayout, GraphicsChild)
public:
	GraphicsFramebufferLayout() noexcept;
	virtual ~GraphicsFramebufferLayout() noexcept;

	virtual const GraphicsFramebufferLayoutDesc& getGraphicsFramebufferLayoutDesc() const noexcept = 0;

private:
	GraphicsFramebufferLayout(GraphicsFramebufferLayout&) noexcept = delete;
	GraphicsFramebufferLayout& operator=(const GraphicsFramebufferLayout&) noexcept = delete;
};

class EXPORT GraphicsFramebuffer : public GraphicsChild
{
	__DeclareSubInterface(GraphicsFramebuffer, GraphicsChild)
public:
	GraphicsFramebuffer() noexcept;
	virtual ~GraphicsFramebuffer() noexcept;

	virtual const GraphicsFramebufferDesc& getGraphicsFramebufferDesc() const noexcept = 0;

private:
	GraphicsFramebuffer(GraphicsFramebuffer&) noexcept = delete;
	GraphicsFramebuffer& operator=(const GraphicsFramebuffer&) noexcept = delete;
};

_NAME_END

#endif