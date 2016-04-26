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
#include "ogl_core_framebuffer.h"
#include "ogl_core_texture.h"
#include "ogl_framebuffer.h"

_NAME_BEGIN

__ImplementSubClass(OGLCoreFramebuffer, GraphicsFramebuffer, "OGLCoreFramebuffer")

OGLCoreFramebuffer::OGLCoreFramebuffer() noexcept
	: _fbo(GL_NONE)
{
}

OGLCoreFramebuffer::~OGLCoreFramebuffer() noexcept
{
	this->close();
}

bool
OGLCoreFramebuffer::setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept
{
	assert(GL_NONE == _fbo);
	assert(framebufferDesc.getGraphicsFramebufferLayout());
	assert(framebufferDesc.getGraphicsFramebufferLayout()->isInstanceOf<OGLFramebufferLayout>());
	assert(framebufferDesc.getWidth() > 0 && framebufferDesc.getHeight() > 0);

	glCreateFramebuffers(1, &_fbo);
	if (_fbo == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateFramebuffers() fail");
		return false;
	}

	auto sharedDepthStnecilTarget = framebufferDesc.getSharedDepthStencilTexture();
	if (sharedDepthStnecilTarget)
	{
		auto format = sharedDepthStnecilTarget->getGraphicsTextureDesc().getTexFormat();
		if (OGLTypes::isDepthStencilFormat(format))
		{
			if (!this->bindRenderTexture(sharedDepthStnecilTarget, GL_DEPTH_STENCIL_ATTACHMENT))
				return false;
		}
		else if (OGLTypes::isDepthFormat(format))
		{
			if (!this->bindRenderTexture(sharedDepthStnecilTarget, GL_DEPTH_ATTACHMENT))
				return false;
		}
		else if (OGLTypes::isStencilFormat(format))
		{
			if (!this->bindRenderTexture(sharedDepthStnecilTarget, GL_STENCIL_ATTACHMENT))
				return false;
		}
		else
		{
			GL_PLATFORM_LOG("Invalid texture format");
			return false;
		}
	}

	GLenum draw[GL_COLOR_ATTACHMENT15 - GL_COLOR_ATTACHMENT0];
	GLenum attachment = 0;

	auto& textures = framebufferDesc.getTextures();
	if (framebufferDesc.getTextures().size() > (sizeof(draw) / sizeof(draw[0])))
	{
		GL_PLATFORM_LOG("The color attachment in framebuffer is out of range");
		return false;
	}

	for (auto& texture : textures)
	{
		if (!this->bindRenderTexture(texture, GL_COLOR_ATTACHMENT0 + attachment))
			return false;

		draw[attachment] = GL_COLOR_ATTACHMENT0 + attachment;

		attachment++;
	}

	glNamedFramebufferDrawBuffers(_fbo, attachment, draw);

	_framebufferDesc = framebufferDesc;
	return OGLCheck::checkError();
}

void
OGLCoreFramebuffer::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		glDeleteFramebuffers(1, &_fbo);
		_fbo = GL_NONE;
	}
}

void
OGLCoreFramebuffer::setLayer(GraphicsTexturePtr renderTexture, GLuint layer) noexcept
{
	auto texture = renderTexture->downcast<OGLCoreTexture>();
	auto textureID = texture->getInstanceID();
	auto& textureDesc = renderTexture->getGraphicsTextureDesc();
	if (textureDesc.getTexDim() != GraphicsTextureDim::GraphicsTextureDim2DArray ||
		textureDesc.getTexDim() != GraphicsTextureDim::GraphicsTextureDimCube ||
		textureDesc.getTexDim() != GraphicsTextureDim::GraphicsTextureDimCubeArray)
	{
		return;
	}

	GLenum attachment = GL_COLOR_ATTACHMENT0;

	for (auto& it : _framebufferDesc.getTextures())
	{
		if (it == renderTexture)
			break;
		attachment++;
	}

	glNamedFramebufferTextureLayer(_fbo, attachment, textureID, 0, layer);
}

GLuint
OGLCoreFramebuffer::getLayer() const noexcept
{
	return 0;
}

void
OGLCoreFramebuffer::discard() noexcept
{
	GLenum attachments[24];
	GLenum attachment = GL_COLOR_ATTACHMENT0;

	auto& targets = _framebufferDesc.getTextures();
	auto size = targets.size();

	for (std::size_t i = 0; i < size; i++)
	{
		attachments[i] = attachment;
		attachment++;
	}

	glInvalidateNamedFramebufferData(_fbo, size, attachments);
}

GLuint
OGLCoreFramebuffer::getInstanceID() noexcept
{
	return _fbo;
}

bool
OGLCoreFramebuffer::bindRenderTexture(GraphicsTexturePtr texture, GLenum attachment) noexcept
{
	assert(texture);
	assert(texture->isInstanceOf<OGLCoreTexture>());

	auto gltexture = texture->downcast<OGLCoreTexture>();
	auto handle = gltexture->getInstanceID();
	auto target = gltexture->getTarget();

	if (target != GL_TEXTURE_2D && target != GL_TEXTURE_2D_MULTISAMPLE && target != GL_TEXTURE_2D_ARRAY && target != GL_TEXTURE_CUBE_MAP)
	{
		GL_PLATFORM_LOG("Invalid texture target");
		return false;
	}

	glNamedFramebufferTexture(_fbo, attachment, handle, 0);

	return OGLCheck::checkError();
}

const GraphicsFramebufferDesc&
OGLCoreFramebuffer::getGraphicsFramebufferDesc() const noexcept
{
	return _framebufferDesc;
}

void
OGLCoreFramebuffer::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLCoreFramebuffer::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END