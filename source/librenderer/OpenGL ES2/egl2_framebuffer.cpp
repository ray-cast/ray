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
#include "egl2_framebuffer.h"
#include "egl2_texture.h"

_NAME_BEGIN

__ImplementSubClass(EGL2Framebuffer, GraphicsFramebuffer, "EGL2Framebuffer")
__ImplementSubClass(EGL2FramebufferLayout, GraphicsFramebufferLayout, "EGL2FramebufferLayout")

EGL2FramebufferLayout::EGL2FramebufferLayout() noexcept
{
}

EGL2FramebufferLayout::~EGL2FramebufferLayout() noexcept
{
	this->close();
}

bool
EGL2FramebufferLayout::setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept
{
	_framebufferLayoutDesc = framebufferDesc;
	return true;
}

void
EGL2FramebufferLayout::close() noexcept
{
}

const GraphicsFramebufferLayoutDesc&
EGL2FramebufferLayout::getGraphicsFramebufferLayoutDesc() const noexcept
{
	return _framebufferLayoutDesc;
}

void
EGL2FramebufferLayout::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2FramebufferLayout::getDevice() noexcept
{
	return _device.lock();
}

EGL2Framebuffer::EGL2Framebuffer() noexcept
	: _fbo(GL_NONE)
{
}

EGL2Framebuffer::~EGL2Framebuffer() noexcept
{
	this->close();
}

bool
EGL2Framebuffer::setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept
{
	assert(GL_NONE == _fbo);
	assert(framebufferDesc.getGraphicsFramebufferLayout());
	assert(framebufferDesc.getWidth() > 0 && framebufferDesc.getHeight() > 0);

	std::uint32_t numAttachment = framebufferDesc.getColorAttachments().size();
	if (numAttachment > 1)
	{
		GL_PLATFORM_LOG("Can't support multi framebuffer");
		return false;
	}

	GL_CHECK(glGenFramebuffers(1, &_fbo));
	if (_fbo == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateFramebuffers() fail");
		return false;
	}

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));

	const auto& textureComponents = framebufferDesc.getGraphicsFramebufferLayout()->getGraphicsFramebufferLayoutDesc().getComponents();
	const auto& colorAttachments = framebufferDesc.getColorAttachments();

	for (std::size_t i = 0; i < textureComponents.size(); i++)
	{
		auto type = textureComponents[i].getAttachType();
		switch (type)
		{
		case GraphicsImageLayout::GraphicsImageLayoutGeneral:
			break;
		case GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal:
		{
			GLint slot = GL_COLOR_ATTACHMENT0 + textureComponents[i].getAttachSlot();
			GLint mipLevel = colorAttachments[0].getBindingLevel();
			GLint layer = colorAttachments[0].getBindingLayer();

			if (!this->bindRenderTexture(colorAttachments[0].getBindingTexture(), slot, mipLevel, layer))
				return false;
		}
		break;
		case GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal:
		case GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal:
		{
			const auto& depthStencilAttachment = framebufferDesc.getDepthStencilAttachment();
			if (!depthStencilAttachment.getBindingTexture())
			{
				GL_PLATFORM_LOG("Need depth or stencil texture.");
				return false;
			}

			auto texture = depthStencilAttachment.getBindingTexture();
			auto format = texture->getGraphicsTextureDesc().getTexFormat();
			auto level = depthStencilAttachment.getBindingLevel();
			auto layer = depthStencilAttachment.getBindingLayer();

			if (EGL2Types::isDepthFormat(format))
			{
				if (!this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT, level, layer))
					return false;
			}
			else if (EGL2Types::isStencilFormat(format))
			{
				if (!this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT, level, layer))
					return false;
			}
			else
			{
				GL_PLATFORM_LOG("Invalid texture format");
				return false;
			}
		}
		case GraphicsImageLayout::GraphicsImageLayoutShaderReadOnlyOptimal:
			break;
		case GraphicsImageLayout::GraphicsImageLayoutTransferSrcOptimal:
			break;
		case GraphicsImageLayout::GraphicsImageLayoutTransferDstOptimal:
			break;
		case GraphicsImageLayout::GraphicsImageLayoutPreinitialized:
			break;
		case GraphicsImageLayout::GraphicsImageLayoutPresentSrcKhr:
			break;
		default:
			break;
		}
	}

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE));

	_framebufferDesc = framebufferDesc;

	return EGL2Check::checkError();
}

void
EGL2Framebuffer::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		glDeleteFramebuffers(1, &_fbo);
		_fbo = GL_NONE;
	}
}

GLuint
EGL2Framebuffer::getInstanceID() noexcept
{
	return _fbo;
}

bool
EGL2Framebuffer::bindRenderTexture(GraphicsTexturePtr texture, GLenum attachment, GLint level, GLint layer) noexcept
{
	assert(texture);

	auto gltexture = texture->downcast<EGL2Texture>();
	auto handle = gltexture->getInstanceID();
	auto target = gltexture->getTarget();

	if (target != GL_TEXTURE_2D && target != GL_TEXTURE_2D_MULTISAMPLE  && target != GL_TEXTURE_CUBE_MAP)
	{
		GL_PLATFORM_LOG("Invalid texture target");
		return false;
	}

	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, handle, 0));

	return true;
}

const GraphicsFramebufferDesc&
EGL2Framebuffer::getGraphicsFramebufferDesc() const noexcept
{
	return _framebufferDesc;
}

void
EGL2Framebuffer::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2Framebuffer::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END