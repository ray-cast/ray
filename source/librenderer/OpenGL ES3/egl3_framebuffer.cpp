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
#include "egl3_framebuffer.h"
#include "egl3_texture.h"

_NAME_BEGIN

__ImplementSubClass(EGL3Framebuffer, GraphicsFramebuffer, "EGL3Framebuffer")
__ImplementSubClass(EGL3FramebufferLayout, GraphicsFramebufferLayout, "EGL3FramebufferLayout")

EGL3FramebufferLayout::EGL3FramebufferLayout() noexcept
{
}

EGL3FramebufferLayout::~EGL3FramebufferLayout() noexcept
{
	this->close();
}

bool
EGL3FramebufferLayout::setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept
{
	_framebufferLayoutDesc = framebufferDesc;
	return true;
}

void
EGL3FramebufferLayout::close() noexcept
{
}

const GraphicsFramebufferLayoutDesc&
EGL3FramebufferLayout::getGraphicsFramebufferLayoutDesc() const noexcept
{
	return _framebufferLayoutDesc;
}

void
EGL3FramebufferLayout::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3FramebufferLayout::getDevice() noexcept
{
	return _device.lock();
}

EGL3Framebuffer::EGL3Framebuffer() noexcept
	: _fbo(GL_NONE)
{
}

EGL3Framebuffer::~EGL3Framebuffer() noexcept
{
	this->close();
}

bool
EGL3Framebuffer::setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept
{
	assert(GL_NONE == _fbo);
	assert(framebufferDesc.getGraphicsFramebufferLayout());
	assert(framebufferDesc.getGraphicsFramebufferLayout()->isInstanceOf<EGL3FramebufferLayout>());
	assert(framebufferDesc.getWidth() > 0 && framebufferDesc.getHeight() > 0);

	GL_CHECK(glGenFramebuffers(1, &_fbo));
	if (_fbo == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateFramebuffers() fail.");
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	GLenum drawCount = 0;
	GLenum drawBuffers[GL_COLOR_ATTACHMENT15 - GL_COLOR_ATTACHMENT0];

	const auto& textureComponents = framebufferDesc.getGraphicsFramebufferLayout()->getGraphicsFramebufferLayoutDesc().getComponents();
	const auto& colorAttachments = framebufferDesc.getColorAttachments();
	if (colorAttachments.size() > (sizeof(drawBuffers) / sizeof(drawBuffers[0])))
	{
		GL_PLATFORM_LOG("The color attachment in framebuffer is out of range.");
		return false;
	}

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
			GLint mipLevel = colorAttachments[drawCount].getBindingLevel();
			GLint layer = colorAttachments[drawCount].getBindingLayer();

			if (!this->bindRenderTexture(colorAttachments[drawCount].getBindingTexture(), slot, mipLevel, layer))
				return false;

			drawBuffers[drawCount++] = slot;
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

			if (EGL3Types::isDepthStencilFormat(format))
			{
				if (!this->bindRenderTexture(texture, GL_DEPTH_STENCIL_ATTACHMENT, level, layer))
					return false;
			}
			else if (EGL3Types::isDepthFormat(format))
			{
				if (!this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT, level, layer))
					return false;
			}
			else if (EGL3Types::isStencilFormat(format))
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

	GL_CHECK(glDrawBuffers(drawCount, drawBuffers));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE));

	_framebufferDesc = framebufferDesc;
	return EGL3Check::checkError();
}

void
EGL3Framebuffer::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		glDeleteFramebuffers(1, &_fbo);
		_fbo = GL_NONE;
	}
}

GLuint
EGL3Framebuffer::getInstanceID() noexcept
{
	return _fbo;
}

bool
EGL3Framebuffer::bindRenderTexture(GraphicsTexturePtr renderTexture, GLenum attachment, GLint level, GLint layer) noexcept
{
	assert(renderTexture);

	auto texture = renderTexture->downcast<EGL3Texture>();
	auto textureID = texture->getInstanceID();
	auto textureTarget = texture->getTarget();
	auto& textureDesc = renderTexture->getGraphicsTextureDesc();

	if (layer > 0)
	{
		if (textureDesc.getTexDim() != GraphicsTextureDim::GraphicsTextureDim2DArray ||
			textureDesc.getTexDim() != GraphicsTextureDim::GraphicsTextureDimCube ||
			textureDesc.getTexDim() != GraphicsTextureDim::GraphicsTextureDimCubeArray)
		{
			GL_PLATFORM_LOG("Invalid texture target");
			return false;
		}
		
		GL_CHECK(glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, textureID, level, layer));
	}
	else
	{
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureTarget, textureID, level));
	}

	return EGL3Check::checkError();
}

const GraphicsFramebufferDesc&
EGL3Framebuffer::getGraphicsFramebufferDesc() const noexcept
{
	return _framebufferDesc;
}

void
EGL3Framebuffer::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3Framebuffer::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END