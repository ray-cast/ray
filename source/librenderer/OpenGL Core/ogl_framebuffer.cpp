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
__ImplementSubClass(OGLMultiRenderTexture, GraphicsMultiRenderTexture, "OGLMultiRenderTexture")

OGLRenderTexture::OGLRenderTexture() noexcept
	: _fbo(GL_NONE)
	, _layer(GL_NONE)
	, _isActive(false)
{
}

OGLRenderTexture::~OGLRenderTexture() noexcept
{
	this->close();
}

bool
OGLRenderTexture::setup(const GraphicsRenderTextureDesc& framebufferDesc) except
{
	assert(!_fbo);

	if (!framebufferDesc.getGraphicsTexture())
		return false;

	glCreateFramebuffers(1, &_fbo);
	if (_fbo == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateFramebuffers() fail");
		return false;
	}

	auto sharedDepthTarget = framebufferDesc.getSharedDepthTexture();
	auto sharedStencilTarget = framebufferDesc.getSharedStencilTexture();

	if (sharedDepthTarget || sharedStencilTarget)
	{
		if (sharedDepthTarget == sharedStencilTarget)
		{
			this->bindRenderTexture(sharedDepthTarget->getResolveTexture(), GL_DEPTH_STENCIL_ATTACHMENT);
		}
		else
		{
			if (sharedDepthTarget)
				this->bindRenderTexture(sharedDepthTarget->getResolveTexture(), GL_DEPTH_ATTACHMENT);

			if (sharedStencilTarget)
				this->bindRenderTexture(sharedStencilTarget->getResolveTexture(), GL_STENCIL_ATTACHMENT);
		}
	}

	auto texture = framebufferDesc.getGraphicsTexture();
	auto textureDesc = texture->getGraphicsTextureDesc();

	GraphicsFormat format = textureDesc.getTexFormat();

	if (format == GraphicsFormat::GraphicsFormatD24UNorm_S8UInt || format == GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt)
		this->bindRenderTexture(texture, GL_DEPTH_STENCIL_ATTACHMENT);
	else if (format == GraphicsFormat::GraphicsFormatD16UNorm || format == GraphicsFormat::GraphicsFormatX8_D24UNormPack32 || format == GraphicsFormat::GraphicsFormatD32_SFLOAT)
		this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT);
	else if (format == GraphicsFormat::GraphicsFormatS8UInt)
		this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT);
	else
		this->bindRenderTexture(texture, GL_COLOR_ATTACHMENT0);

	_framebufferDesc = framebufferDesc;

	glNamedFramebufferDrawBuffer(_fbo, GL_COLOR_ATTACHMENT0);
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
OGLRenderTexture::setActive(bool active) noexcept
{
	if (_isActive != active)
	{
		if (active)
			glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		_isActive = active;
	}
}

bool
OGLRenderTexture::getActive() noexcept
{
	return _isActive;
}

void
OGLRenderTexture::setLayer(GLuint layer) noexcept
{
	assert(this->getActive());

	if (_layer != layer)
	{
		auto& textureDesc = _framebufferDesc.getGraphicsTexture()->getGraphicsTextureDesc();
		if (textureDesc.getTexDim() != GraphicsTextureDim::GraphicsTextureDim2DArray ||
			textureDesc.getTexDim() != GraphicsTextureDim::GraphicsTextureDimCube ||
			textureDesc.getTexDim() != GraphicsTextureDim::GraphicsTextureDimCubeArray)
		{
			return;
		}

		auto texture = this->getResolveTexture()->downcast<OGLTexture>();
		auto textureID = texture->getInstanceID();

		GLenum attachment = GL_COLOR_ATTACHMENT0;
		glNamedFramebufferTextureLayer(_fbo, attachment, textureID, 0, layer);

		_layer = layer;
	}
}

GLuint
OGLRenderTexture::getLayer() const noexcept
{
	return _layer;
}

void
OGLRenderTexture::discard() noexcept
{
	GLenum attachment[3];
	GLsizei index = 0;

	auto sharedDepthTarget = _framebufferDesc.getSharedDepthTexture();
	auto sharedStencilTarget = _framebufferDesc.getSharedStencilTexture();

	if (sharedDepthTarget == sharedStencilTarget)
	{
		if (sharedDepthTarget)
			attachment[index++] = GL_DEPTH_STENCIL_ATTACHMENT;
	}
	else
	{
		if (sharedDepthTarget)
			attachment[index++] = GL_DEPTH_ATTACHMENT;

		if (sharedStencilTarget)
			attachment[index++] = GL_STENCIL_ATTACHMENT;
	}

	auto texture = _framebufferDesc.getGraphicsTexture();
	auto textureDesc = texture->getGraphicsTextureDesc();

	GraphicsFormat format = textureDesc.getTexFormat();

	if (format == GraphicsFormat::GraphicsFormatD24UNorm_S8UInt || format == GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt)
		attachment[index++] = GL_DEPTH_STENCIL_ATTACHMENT;
	else if (format == GraphicsFormat::GraphicsFormatD16UNorm || format == GraphicsFormat::GraphicsFormatX8_D24UNormPack32 || format == GraphicsFormat::GraphicsFormatD32_SFLOAT)
		attachment[index++] = GL_DEPTH_ATTACHMENT;
	else if (format == GraphicsFormat::GraphicsFormatS8UInt)
		attachment[index++] = GL_STENCIL_ATTACHMENT;
	else
		attachment[index++] = GL_COLOR_ATTACHMENT0;

	glInvalidateNamedFramebufferData(_fbo, index, attachment);
}

void
OGLRenderTexture::bindRenderTexture(GraphicsTexturePtr texture, GLenum attachment) noexcept
{
	assert(texture);

	auto gltexture = texture->downcast<OGLTexture>();
	auto handle = gltexture->getInstanceID();

	glNamedFramebufferTexture(_fbo, attachment, handle, 0);
}

GLuint
OGLRenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

GraphicsTexturePtr
OGLRenderTexture::getResolveTexture() const noexcept
{
	return _framebufferDesc.getGraphicsTexture();
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

OGLMultiRenderTexture::OGLMultiRenderTexture() noexcept
	: _fbo(GL_NONE)
	, _isActive(false)
{
}

OGLMultiRenderTexture::~OGLMultiRenderTexture() noexcept
{
	this->close();
}

bool
OGLMultiRenderTexture::setup(const GraphicsMultiRenderTextureDesc& multiFramebufferDesc) noexcept
{
	assert(GL_NONE == _fbo);

	glCreateFramebuffers(1, &_fbo);
	if (_fbo == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateFramebuffers() fail");
		return false;
	}

	auto sharedDepthTarget = multiFramebufferDesc.getSharedDepthTexture();
	auto sharedStencilTarget = multiFramebufferDesc.getSharedStencilTexture();

	if (sharedDepthTarget == sharedStencilTarget)
	{
		if (sharedDepthTarget)
			this->bindRenderTexture(sharedDepthTarget->getResolveTexture(), GL_DEPTH_STENCIL_ATTACHMENT);
	}
	else
	{
		if (sharedDepthTarget)
		{
			this->bindRenderTexture(sharedDepthTarget->getResolveTexture(), GL_DEPTH_ATTACHMENT);
		}

		if (sharedStencilTarget)
		{
			this->bindRenderTexture(sharedStencilTarget->getResolveTexture(), GL_STENCIL_ATTACHMENT);
		}
	}

	GLenum draw[MAX_COLOR_ATTACHMENTS] = { 0 };
	GLenum attachment = GL_COLOR_ATTACHMENT0;
	GLsizei count = 0;

	for (auto& target : multiFramebufferDesc.getRenderTextures())
	{
		this->bindRenderTexture(target->getResolveTexture(), attachment);
		draw[count++] = attachment++;
	}

	glNamedFramebufferDrawBuffers(_fbo, count, draw);

	_multiFramebufferDesc = multiFramebufferDesc;
	return true;
}

void
OGLMultiRenderTexture::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		glDeleteFramebuffers(1, &_fbo);
		_fbo = GL_NONE;
	}
}

void
OGLMultiRenderTexture::setActive(bool active) noexcept
{
	if (_isActive != active)
	{
		if (active)
			glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		_isActive = active;
	}
}

bool
OGLMultiRenderTexture::getActive() noexcept
{
	return _isActive;
}

void
OGLMultiRenderTexture::setLayer(GraphicsRenderTexturePtr renderTexture, GLuint layer) noexcept
{
	auto texture = renderTexture->getResolveTexture()->downcast<OGLTexture>();
	auto textureID = texture->getInstanceID();
	auto& textureDesc = renderTexture->getResolveTexture()->getGraphicsTextureDesc();
	if (textureDesc.getTexDim() != GraphicsTextureDim::GraphicsTextureDim2DArray ||
		textureDesc.getTexDim() != GraphicsTextureDim::GraphicsTextureDimCube ||
		textureDesc.getTexDim() != GraphicsTextureDim::GraphicsTextureDimCubeArray)
	{
		return;
	}

	GLenum attachment = GL_COLOR_ATTACHMENT0;

	for (auto& it : _multiFramebufferDesc.getRenderTextures())
	{
		if (it == renderTexture)
			break;
		attachment++;
	}

	glNamedFramebufferTextureLayer(_fbo, attachment, textureID, 0, layer);
}

GLuint
OGLMultiRenderTexture::getLayer() const noexcept
{
	return 0;
}

void
OGLMultiRenderTexture::discard() noexcept
{
	GLenum attachments[24];
	GLenum attachment = GL_COLOR_ATTACHMENT0;

	auto& targets = _multiFramebufferDesc.getRenderTextures();
	auto size = targets.size();

	for (std::size_t i = 0; i < size; i++)
	{
		attachments[i] = attachment;
		attachment++;
	}

	glInvalidateNamedFramebufferData(_fbo, size, attachments);
}

GLuint
OGLMultiRenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

const GraphicsMultiRenderTextureDesc&
OGLMultiRenderTexture::getGraphicsMultiRenderTextureDesc() const noexcept
{
	return _multiFramebufferDesc;
}

void
OGLMultiRenderTexture::bindRenderTexture(GraphicsTexturePtr texture, GLenum attachment) noexcept
{
	assert(texture);

	auto gltexture = texture->downcast<OGLTexture>();
	auto handle = gltexture->getInstanceID();

	glNamedFramebufferTexture(_fbo, attachment, handle, 0);
}

void
OGLMultiRenderTexture::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLMultiRenderTexture::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END