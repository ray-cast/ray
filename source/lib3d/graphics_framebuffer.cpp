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
#include <ray/graphics_framebuffer.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsFramebuffer, GraphicsChild, "GraphicsFramebuffer")
__ImplementSubInterface(GraphicsFramebufferLayout, GraphicsChild, "GraphicsFramebufferLayout")

GraphicsAttachmentLayout::GraphicsAttachmentLayout() noexcept
	: _type(GraphicsImageLayout::GraphicsImageLayoutUndefined)
	, _slot(0)
	, _format(GraphicsFormat::GraphicsFormatUndefined)
{
}

GraphicsAttachmentLayout::GraphicsAttachmentLayout(std::uint32_t slot, GraphicsImageLayout type, GraphicsFormat format) noexcept
	: _type(type)
	, _slot(slot)
	, _format(format)
{
}

GraphicsAttachmentLayout::~GraphicsAttachmentLayout() noexcept
{
}

void
GraphicsAttachmentLayout::setAttachSlot(std::uint32_t slot) noexcept
{
	_slot = slot;
}

std::uint32_t
GraphicsAttachmentLayout::getAttachSlot() const noexcept
{
	return _slot;
}

void
GraphicsAttachmentLayout::setAttachType(GraphicsImageLayout type) noexcept
{
	_type = type;
}

GraphicsImageLayout
GraphicsAttachmentLayout::getAttachType() const noexcept
{
	return _type;
}

void
GraphicsAttachmentLayout::setAttachFormat(GraphicsFormat format) noexcept
{
	_format = format;
}

GraphicsFormat
GraphicsAttachmentLayout::getAttachFormat() const noexcept
{
	return _format;
}

GraphicsAttachmentBinding::GraphicsAttachmentBinding() noexcept
	: _mipLevel(0)
	, _layer(0)
{
}

GraphicsAttachmentBinding::GraphicsAttachmentBinding(GraphicsTexturePtr texture, std::uint32_t mipLevel, std::uint32_t layer) noexcept
	: _mipLevel(mipLevel)
	, _layer(layer)
	, _texture(texture)
{
}

GraphicsAttachmentBinding::~GraphicsAttachmentBinding() noexcept
{
}

void
GraphicsAttachmentBinding::setBindingLevel(std::uint32_t mipLevel) noexcept
{
	_mipLevel = mipLevel;
}

std::uint32_t 
GraphicsAttachmentBinding::getBindingLevel() const noexcept
{
	return _mipLevel;
}

void
GraphicsAttachmentBinding::setBindingLayer(std::uint32_t layer) noexcept
{
	_layer = layer;
}

std::uint32_t 
GraphicsAttachmentBinding::getBindingLayer() const noexcept
{
	return _layer;
}

void
GraphicsAttachmentBinding::setBindingTexture(GraphicsTexturePtr texture) noexcept
{
	_texture = texture;
}

GraphicsTexturePtr 
GraphicsAttachmentBinding::getBindingTexture() const noexcept
{
	return _texture;
}

GraphicsFramebufferLayoutDesc::GraphicsFramebufferLayoutDesc() noexcept
{
}

GraphicsFramebufferLayoutDesc::~GraphicsFramebufferLayoutDesc() noexcept
{
}

void
GraphicsFramebufferLayoutDesc::addComponent(const GraphicsAttachmentLayout& component) noexcept
{
	assert(component.getAttachFormat() != GraphicsFormat::GraphicsFormatUndefined);
	assert(std::find_if(_components.begin(), _components.end(), [&](const GraphicsAttachmentLayout& it) { return  it.getAttachSlot() == component.getAttachSlot(); }) == _components.end());
	_components.push_back(component);
}

void 
GraphicsFramebufferLayoutDesc::setComponents(const GraphicsAttachmentLayouts& components) noexcept
{
	_components = components;
}

const GraphicsAttachmentLayouts&
GraphicsFramebufferLayoutDesc::getComponents() const noexcept
{
	return _components;
}

GraphicsFramebufferDesc::GraphicsFramebufferDesc() noexcept
	: _width(0)
	, _height(0)
	, _layer(1)
{
}

GraphicsFramebufferDesc::~GraphicsFramebufferDesc() noexcept
{
}

void
GraphicsFramebufferDesc::setWidth(std::uint32_t w) noexcept
{
	_width = w;
}

void
GraphicsFramebufferDesc::setHeight(std::uint32_t h) noexcept
{
	_height = h;
}

void 
GraphicsFramebufferDesc::setLayer(std::uint32_t layer) noexcept
{
	_layer = layer;
}

std::uint32_t
GraphicsFramebufferDesc::getWidth() const noexcept
{
	return _width;
}

std::uint32_t
GraphicsFramebufferDesc::getHeight() const noexcept
{
	return _height;
}

std::uint32_t
GraphicsFramebufferDesc::getLayer() const noexcept
{
	return _layer;
}

void 
GraphicsFramebufferDesc::addColorAttachment(const GraphicsAttachmentBinding& texture) noexcept
{
	assert(texture.getBindingTexture());
	_colorAttachments.push_back(texture);
}

void 
GraphicsFramebufferDesc::setColorAttachments(const GraphicsAttachmentBindings& bindings) noexcept
{
	_colorAttachments = bindings;
}

const GraphicsAttachmentBinding& 
GraphicsFramebufferDesc::getColorAttachment(std::uint32_t n) const noexcept
{
	assert(n < _colorAttachments.size());
	return _colorAttachments[n];
}

const GraphicsAttachmentBindings&
GraphicsFramebufferDesc::getColorAttachments() const noexcept
{
	return _colorAttachments;
}

void
GraphicsFramebufferDesc::setDepthStencilAttachment(const GraphicsAttachmentBinding& attachment) noexcept
{
	_depthDepthAttachment = attachment;
}

const GraphicsAttachmentBinding&
GraphicsFramebufferDesc::getDepthStencilAttachment() const noexcept
{
	return _depthDepthAttachment;
}

void
GraphicsFramebufferDesc::setGraphicsFramebufferLayout(GraphicsFramebufferLayoutPtr layout) noexcept
{
	_framebufferLayout = layout;
}

GraphicsFramebufferLayoutPtr
GraphicsFramebufferDesc::getGraphicsFramebufferLayout() const noexcept
{
	return _framebufferLayout;
}

GraphicsFramebufferLayout::GraphicsFramebufferLayout() noexcept
{
}

GraphicsFramebufferLayout::~GraphicsFramebufferLayout() noexcept
{
}

GraphicsFramebuffer::GraphicsFramebuffer() noexcept
{
}

GraphicsFramebuffer::~GraphicsFramebuffer() noexcept
{
}

_NAME_END