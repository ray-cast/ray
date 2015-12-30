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

__ImplementSubClass(EGL3RenderTexture, GraphicsRenderTexture, "EGL3RenderTexture")
__ImplementSubClass(EGL3MultiRenderTexture, GraphicsMultiRenderTexture, "EGL3MultiRenderTexture")

EGL3RenderTexture::EGL3RenderTexture() noexcept
	: _fbo(GL_NONE)
	, _layer(GL_NONE)
{
}

EGL3RenderTexture::~EGL3RenderTexture() noexcept
{
	this->close();
}

bool
EGL3RenderTexture::setup(const GraphicsRenderTextureDesc& framebufferDesc) except
{
	assert(!_fbo);

	if (!framebufferDesc.getGraphicsTexture())
		return false;

	GL_CHECK(glGenFramebuffers(1, &_fbo));
	if (_fbo == GL_NONE)
	{
		GL_PLATFORM_LOG("glGenFramebuffers fail");
		return false;
	}

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));

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

	TextureFormat format = textureDesc.getTexFormat();

	if (format == TextureFormat::DEPTH24_STENCIL8 || format == TextureFormat::DEPTH32_STENCIL8)
		this->bindRenderTexture(texture, GL_DEPTH_STENCIL_ATTACHMENT);
	else if (format == TextureFormat::DEPTH_COMPONENT16 || format == TextureFormat::DEPTH_COMPONENT24 || format == TextureFormat::DEPTH_COMPONENT32)
		this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT);
	else if (format == TextureFormat::STENCIL8)
		this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT);
	else
		this->bindRenderTexture(texture, GL_COLOR_ATTACHMENT0);

	_framebufferDesc = framebufferDesc;

	GLenum draw = GL_COLOR_ATTACHMENT0;
	GL_CHECK(glDrawBuffers(1, &draw));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	return true;
}

void
EGL3RenderTexture::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		GL_CHECK(glDeleteFramebuffers(1, &_fbo));
		_fbo = GL_NONE;
	}
}

void
EGL3RenderTexture::setActive(bool active) noexcept
{
	if (_isActive != active)
	{
		if (active)
			glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		_isActive = active;
	}
}

bool
EGL3RenderTexture::getActive() noexcept
{
	return _isActive;
}

void
EGL3RenderTexture::setLayer(GLuint layer) noexcept
{
	assert(this->getActive());

	if (_layer != layer)
	{
		auto& textureDesc = _framebufferDesc.getGraphicsTexture()->getGraphicsTextureDesc();
		if (textureDesc.getTexDim() != TextureDim::DIM_2D_ARRAY ||
			textureDesc.getTexDim() != TextureDim::DIM_CUBE)
		{
			return;
		}

		auto texture = this->getResolveTexture()->downcast<EGL3Texture>();
		auto textureID = texture->getInstanceID();

		GLenum attachment = GL_COLOR_ATTACHMENT0;

		if (textureDesc.getTexDim() == TextureDim::DIM_2D_ARRAY)
			GL_CHECK(glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, textureID, 0, layer));
		else if (textureDesc.getTexDim() == TextureDim::DIM_CUBE)
			GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, textureID, 0));

		_layer = layer;
	}
}

GLuint
EGL3RenderTexture::getLayer() const noexcept
{
	return _layer;
}

void
EGL3RenderTexture::discard() noexcept
{
	GLenum attachment = GL_COLOR_ATTACHMENT0;
	GL_CHECK(glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachment));
}

void
EGL3RenderTexture::bindRenderTexture(GraphicsTexturePtr texture, GLenum attachment) noexcept
{
	assert(texture);

	auto gltexture = texture->downcast<EGL3Texture>();
	auto handle = gltexture->getInstanceID();
	auto target = gltexture->getTarget();

	if (target == GL_TEXTURE_2D)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0));
	else if (target == GL_TEXTURE_2D_MULTISAMPLE)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, handle, 0));
	else if (target == GL_TEXTURE_2D_ARRAY)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_ARRAY, handle, 0));
	else if (target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE_ARRAY, handle, 0));
	else if (target == GL_TEXTURE_CUBE_MAP)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0));
	else
		assert(false);
}

GLuint
EGL3RenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

GraphicsTexturePtr
EGL3RenderTexture::getResolveTexture() const noexcept
{
	return _framebufferDesc.getGraphicsTexture();
}

const GraphicsRenderTextureDesc&
EGL3RenderTexture::getGraphicsRenderTextureDesc() const noexcept
{
	return _framebufferDesc;
}

void
EGL3RenderTexture::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3RenderTexture::getDevice() noexcept
{
	return _device.lock();
}

EGL3MultiRenderTexture::EGL3MultiRenderTexture() noexcept
	: _fbo(GL_NONE)
	, _isActive(false)
{
}

EGL3MultiRenderTexture::~EGL3MultiRenderTexture() noexcept
{
	this->close();
}

bool
EGL3MultiRenderTexture::setup(const GraphicsMultiRenderTextureDesc& multiFramebufferDesc) noexcept
{
	assert(GL_NONE == _fbo);

	GL_CHECK(glGenFramebuffers(1, &_fbo));
	if (_fbo == GL_NONE)
	{
		GL_PLATFORM_LOG("glGenFramebuffers fail");
		return false;
	}

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));

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

	GL_CHECK(glDrawBuffers(count, draw));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	return true;
}

void
EGL3MultiRenderTexture::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		GL_CHECK(glDeleteFramebuffers(1, &_fbo));
		_fbo = GL_NONE;
	}
}

void
EGL3MultiRenderTexture::setActive(bool active) noexcept
{
	if (_isActive != active)
	{
		if (active)
			GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));
		_isActive = active;
	}
}

bool
EGL3MultiRenderTexture::getActive() noexcept
{
	return _isActive;
}

void
EGL3MultiRenderTexture::setLayer(GraphicsRenderTexturePtr renderTexture, GLuint layer) noexcept
{
	auto texture = renderTexture->getResolveTexture()->downcast<EGL3Texture>();
	auto textureID = texture->getInstanceID();
	auto& textureDesc = renderTexture->getResolveTexture()->getGraphicsTextureDesc();
	if (textureDesc.getTexDim() != TextureDim::DIM_2D_ARRAY ||
		textureDesc.getTexDim() != TextureDim::DIM_CUBE)
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

	if (textureDesc.getTexDim() == TextureDim::DIM_2D_ARRAY)
		GL_CHECK(glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, textureID, 0, layer));
	else if (textureDesc.getTexDim() == TextureDim::DIM_CUBE)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, textureID, 0));
}

GLuint
EGL3MultiRenderTexture::getLayer() const noexcept
{
	return 0;
}

void
EGL3MultiRenderTexture::discard() noexcept
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

	GL_CHECK(glInvalidateFramebuffer(GL_FRAMEBUFFER, size, attachments));
}

GLuint
EGL3MultiRenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

const GraphicsMultiRenderTextureDesc&
EGL3MultiRenderTexture::getGraphicsMultiRenderTextureDesc() const noexcept
{
	return _multiFramebufferDesc;
}

void
EGL3MultiRenderTexture::bindRenderTexture(GraphicsTexturePtr texture, GLenum attachment) noexcept
{
	assert(texture);

	auto gltexture = texture->downcast<EGL3Texture>();
	auto handle = gltexture->getInstanceID();
	auto target = gltexture->getTarget();

	if (target == GL_TEXTURE_2D)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0));
	else if (target == GL_TEXTURE_2D_MULTISAMPLE)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, handle, 0));
	else if (target == GL_TEXTURE_2D_ARRAY)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_ARRAY, handle, 0));
	else if (target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE_ARRAY, handle, 0));
	else if (target == GL_TEXTURE_CUBE_MAP)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0));
	else
		assert(false);
}

void
EGL3MultiRenderTexture::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3MultiRenderTexture::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END