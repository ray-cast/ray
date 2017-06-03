// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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

class EXPORT GraphicsAttachmentLayout final
{
public:
	GraphicsAttachmentLayout() noexcept;
	GraphicsAttachmentLayout(std::uint32_t slot, GraphicsImageLayout type, GraphicsFormat format) noexcept;
	~GraphicsAttachmentLayout() noexcept;

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

class EXPORT GraphicsAttachmentBinding final
{
public:
	GraphicsAttachmentBinding() noexcept;
	GraphicsAttachmentBinding(GraphicsTexturePtr texture, std::uint32_t mipLevel, std::uint32_t layer) noexcept;
	~GraphicsAttachmentBinding() noexcept;

	void setBindingLevel(std::uint32_t mipLevel) noexcept;
	std::uint32_t getBindingLevel() const noexcept;

	void setBindingLayer(std::uint32_t layer) noexcept;
	std::uint32_t getBindingLayer() const noexcept;

	void setBindingTexture(GraphicsTexturePtr texture) noexcept;
	GraphicsTexturePtr getBindingTexture() const noexcept;

private:
	std::uint32_t _mipLevel;
	std::uint32_t _layer;
	GraphicsTexturePtr _texture;
};

class EXPORT GraphicsFramebufferLayoutDesc final
{
public:
	GraphicsFramebufferLayoutDesc() noexcept;
	~GraphicsFramebufferLayoutDesc() noexcept;

	void addComponent(const GraphicsAttachmentLayout& component) noexcept;
	void setComponents(const GraphicsAttachmentLayouts& components) noexcept;
	const GraphicsAttachmentLayouts& getComponents() const noexcept;

private:
	GraphicsAttachmentLayouts _components;
};

class EXPORT GraphicsFramebufferDesc final
{
public:
	GraphicsFramebufferDesc() noexcept;
	virtual ~GraphicsFramebufferDesc() noexcept;

	void setWidth(std::uint32_t w) noexcept;
	void setHeight(std::uint32_t h) noexcept;
	void setLayer(std::uint32_t layer) noexcept;

	std::uint32_t getWidth() const noexcept;
	std::uint32_t getHeight() const noexcept;
	std::uint32_t getLayer() const noexcept;

	void addColorAttachment(const GraphicsAttachmentBinding& texture) noexcept;
	void setColorAttachments(const GraphicsAttachmentBindings& bindings) noexcept;
	const GraphicsAttachmentBinding& getColorAttachment(std::uint32_t n = 0) const noexcept;
	const GraphicsAttachmentBindings& getColorAttachments() const noexcept;

	void setDepthStencilAttachment(const GraphicsAttachmentBinding& target) noexcept;
	const GraphicsAttachmentBinding& getDepthStencilAttachment() const noexcept;

	void setGraphicsFramebufferLayout(GraphicsFramebufferLayoutPtr layout) noexcept;
	GraphicsFramebufferLayoutPtr getGraphicsFramebufferLayout() const noexcept;

private:
	std::uint32_t _width;
	std::uint32_t _height;
	std::uint32_t _layer;

	GraphicsAttachmentBindings _colorAttachments;
	GraphicsAttachmentBinding _depthDepthAttachment;
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