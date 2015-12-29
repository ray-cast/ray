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

__ImplementSubClass(EGL2RenderTexture, GraphicsRenderTexture, "EGL2RenderTexture")
__ImplementSubClass(EGL2MultiRenderTexture, GraphicsMultiRenderTexture, "EGL2MultiRenderTexture")

EGL2RenderTexture::EGL2RenderTexture() noexcept
	: _fbo(GL_NONE)
	, _layer(GL_NONE)
{
}

EGL2RenderTexture::~EGL2RenderTexture() noexcept
{
	this->close();
}

bool
EGL2RenderTexture::setup(const GraphicsRenderTextureDesc& framebufferDesc) except
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

	if (sharedDepthTarget == sharedStencilTarget)
	{
		GL_PLATFORM_LOG("Can't support GL_DEPTH_STENCIL_ATTACHMENT");
		return false;
	}
	else
	{
		if (sharedDepthTarget)
			this->bindRenderTexture(sharedDepthTarget->getResolveTexture(), GL_DEPTH_ATTACHMENT);

		if (sharedStencilTarget)
			this->bindRenderTexture(sharedStencilTarget->getResolveTexture(), GL_STENCIL_ATTACHMENT);
	}

	auto texture = framebufferDesc.getGraphicsTexture();
	auto textureDesc = texture->getGraphicsTextureDesc();

	TextureFormat format = textureDesc.getTexFormat();

	if (format == TextureFormat::DEPTH24_STENCIL8 || format == TextureFormat::DEPTH32_STENCIL8)
	{
		GL_PLATFORM_LOG("Can't support GL_DEPTH_STENCIL_ATTACHMENT");
		return false;
	}
	else if (format == TextureFormat::DEPTH_COMPONENT16 || format == TextureFormat::DEPTH_COMPONENT24 || format == TextureFormat::DEPTH_COMPONENT32)
		this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT);
	else if (format == TextureFormat::STENCIL8)
		this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT);
	else
		this->bindRenderTexture(texture, GL_COLOR_ATTACHMENT0);

	_framebufferDesc = framebufferDesc;

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	return true;
}

void
EGL2RenderTexture::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		GL_CHECK(glDeleteFramebuffers(1, &_fbo));
		_fbo = GL_NONE;
	}
}

void
EGL2RenderTexture::setActive(bool active) noexcept
{
	if (_isActive != active)
	{
		if (active)
			glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		_isActive = active;
	}
}

bool
EGL2RenderTexture::getActive() noexcept
{
	return _isActive;
}

void
EGL2RenderTexture::setLayer(GLuint layer) noexcept
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

		auto texture = this->getResolveTexture()->downcast<EGL2Texture>();
		auto textureID = texture->getInstanceID();

		GLenum attachment = GL_COLOR_ATTACHMENT0;

		if (textureDesc.getTexDim() == TextureDim::DIM_CUBE)
			GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, textureID, 0));

		_layer = layer;
	}
}

GLuint
EGL2RenderTexture::getLayer() const noexcept
{
	return _layer;
}

void
EGL2RenderTexture::discard() noexcept
{
}

void
EGL2RenderTexture::bindRenderTexture(GraphicsTexturePtr texture, GLenum attachment) noexcept
{
	assert(texture);

	auto gltexture = texture->downcast<EGL2Texture>();
	auto handle = gltexture->getInstanceID();
	auto target = gltexture->getTarget();

	if (target == GL_TEXTURE_2D)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0));
	else if (target == GL_TEXTURE_2D_MULTISAMPLE)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, handle, 0));
	else if (target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE_ARRAY, handle, 0));
	else if (target == GL_TEXTURE_CUBE_MAP)
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0));
	else
		assert(false);
}

GLuint
EGL2RenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

GraphicsTexturePtr
EGL2RenderTexture::getResolveTexture() const noexcept
{
	return _framebufferDesc.getGraphicsTexture();
}

const GraphicsRenderTextureDesc&
EGL2RenderTexture::getGraphicsRenderTextureDesc() const noexcept
{
	return _framebufferDesc;
}

void
EGL2RenderTexture::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2RenderTexture::getDevice() noexcept
{
	return _device.lock();
}

EGL2MultiRenderTexture::EGL2MultiRenderTexture() noexcept
{
}

EGL2MultiRenderTexture::~EGL2MultiRenderTexture() noexcept
{
}

const GraphicsMultiRenderTextureDesc&
EGL2MultiRenderTexture::getGraphicsMultiRenderTextureDesc() const noexcept
{
	return _multiFramebufferDesc;
}

void
EGL2MultiRenderTexture::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2MultiRenderTexture::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END