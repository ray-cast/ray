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

	GL_CHECK(glGenFramebuffers(1, &_fbo));
	if (_fbo == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateFramebuffers() fail");
		return false;
	}

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));

	auto sharedDepthTarget = framebufferDesc.getSharedDepthTexture();
	auto sharedStencilTarget = framebufferDesc.getSharedStencilTexture();

	if (sharedDepthTarget == sharedStencilTarget)
	{
		GL_PLATFORM_LOG("Can't support DepthStencil attachment");
		return false;
	}
	else
	{
		if (sharedDepthTarget)
		{
			if (!this->bindRenderTexture(sharedDepthTarget, GL_DEPTH_ATTACHMENT))
				return false;
		}

		if (sharedStencilTarget)
		{
			if (!this->bindRenderTexture(sharedStencilTarget, GL_STENCIL_ATTACHMENT))
				return false;
		}
	}

	if (framebufferDesc.getTextures().size() > 0)
	{
		GL_PLATFORM_LOG("Can't support multi framebuffer");
		return false;
	}

	if (!this->bindRenderTexture(framebufferDesc.getTextures().front(), GL_COLOR_ATTACHMENT0))
		return false;

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
EGL2Framebuffer::bindRenderTexture(GraphicsTexturePtr texture, GLenum attachment) noexcept
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

	return EGL2Check::checkError();
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