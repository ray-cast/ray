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
#include "ogl_framebuffer.h"
#include "ogl_texture.h"

_NAME_BEGIN

__ImplementSubClass(OGLRenderTexture, GraphicsRenderTexture, "OGLRenderTexture")

OGLRenderTexture::OGLRenderTexture() noexcept
	: _fbo(GL_NONE)
{
}

OGLRenderTexture::~OGLRenderTexture() noexcept
{
	this->close();
}

bool
OGLRenderTexture::setup(const GraphicsRenderTextureDesc& framebufferDesc) noexcept
{
	assert(GL_NONE == _fbo);

	glCreateFramebuffers(1, &_fbo);
	if (_fbo == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateFramebuffers() fail");
		return false;
	}

	auto sharedDepthTarget = framebufferDesc.getSharedDepthTexture();
	auto sharedStencilTarget = framebufferDesc.getSharedStencilTexture();

	if (sharedDepthTarget == sharedStencilTarget)
	{
		if (sharedDepthTarget)
			this->bindRenderTexture(sharedDepthTarget, GL_DEPTH_STENCIL_ATTACHMENT);
	}
	else
	{
		if (sharedDepthTarget)
		{
			this->bindRenderTexture(sharedDepthTarget, GL_DEPTH_ATTACHMENT);
		}

		if (sharedStencilTarget)
		{
			this->bindRenderTexture(sharedStencilTarget, GL_STENCIL_ATTACHMENT);
		}
	}

	GLenum draw[MAX_COLOR_ATTACHMENTS] = { 0 };
	GLenum attachment = GL_COLOR_ATTACHMENT0;
	GLsizei count = 0;

	for (auto& texture : framebufferDesc.getTextures())
	{
		this->bindRenderTexture(texture, attachment);
		draw[count++] = attachment++;
	}

	glNamedFramebufferDrawBuffers(_fbo, count, draw);

	_framebufferDesc = framebufferDesc;
	return true;
}

void
OGLRenderTexture::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		glDeleteFramebuffers(1, &_fbo);
		_fbo = GL_NONE;
	}
}

void
OGLRenderTexture::setLayer(GraphicsTexturePtr renderTexture, GLuint layer) noexcept
{
	auto texture = renderTexture->downcast<OGLTexture>();
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
OGLRenderTexture::getLayer() const noexcept
{
	return 0;
}

void
OGLRenderTexture::discard() noexcept
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
OGLRenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

void
OGLRenderTexture::bindRenderTexture(GraphicsTexturePtr texture, GLenum attachment) noexcept
{
	assert(texture);

	auto gltexture = texture->downcast<OGLTexture>();
	auto handle = gltexture->getInstanceID();

	glNamedFramebufferTexture(_fbo, attachment, handle, 0);
}

const GraphicsRenderTextureDesc&
OGLRenderTexture::getGraphicsRenderTextureDesc() const noexcept
{
	return _framebufferDesc;
}

void
OGLRenderTexture::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLRenderTexture::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END