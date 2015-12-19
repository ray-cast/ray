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
#include "egl2_texture.h"

_NAME_BEGIN

#define MAX_COLOR_ATTACHMENTS 15

EGL2Texture::EGL2Texture() noexcept
	: _texture(0)
{
}

EGL2Texture::~EGL2Texture() noexcept
{
	this->close();
}

bool
EGL2Texture::setup() except
{
	auto target = EGL2Types::asEGL2Target(this->getTexDim());
	auto internalFormat = EGL2Types::asEGL2Internalformat(this->getTexFormat());
	auto stream = this->getStream();

	GL_CHECK(glGenTextures(1, &_texture));
	GL_CHECK(glBindTexture(target, _texture));

	GLsizei w = (GLsizei)this->getWidth();
	GLsizei h = (GLsizei)this->getHeight();
	GLsizei depth = (GLsizei)this->getDepth();

	applyTextureWrap(target, this->getSamplerWrap());
	applyTextureFilter(target, this->getSamplerFilter());
	applyTextureAnis(target, this->getSamplerAnis());

	if (internalFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
		internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
		internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
		internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
	{
		GLint level = (GLint)this->getMipLevel();
		GLsizei size = this->getMipSize();
		std::size_t offset = 0;
		std::size_t blockSize = internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;

		for (GLint mip = 0; mip < level; mip++)
		{
			auto mipSize = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;

			GL_CHECK(glCompressedTexImage2D(GL_TEXTURE_2D, mip, internalFormat, w, h, 0, mipSize, (char*)stream + offset));

			w = std::max(w >> 1, 1);
			h = std::max(h >> 1, 1);

			offset += mipSize;
		}
	}
	else
	{
		auto format = EGL2Types::asEGL2Format(this->getTexFormat());
		auto type = EGL2Types::asEGL2Type(this->getTexFormat());

		auto level = 0;

		switch (target)
		{
		case GL_TEXTURE_2D:
			GL_CHECK(glTexImage2D(target, level, internalFormat, w, h, 0, format, type, stream));
		break;
		case GL_TEXTURE_CUBE_MAP:
		{
			GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, internalFormat, w, h, 0, format, type, stream));
			GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, internalFormat, w, h, 0, format, type, stream));
			GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, internalFormat, w, h, 0, format, type, stream));
			GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, internalFormat, w, h, 0, format, type, stream));
			GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, internalFormat, w, h, 0, format, type, stream));
			GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, internalFormat, w, h, 0, format, type, stream));
		}
		break;
		default:
			assert(false);
			break;
		}
	}

	if (this->isMipmap())
		GL_CHECK(glGenerateMipmap(target));

	return true;
}

void
EGL2Texture::close() noexcept
{
	if (_texture)
	{
		glDeleteTextures(1, &_texture);
		_texture = GL_NONE;
	}
}

GLuint
EGL2Texture::getInstanceID() noexcept
{
	return _texture;
}

void
EGL2Texture::applyTextureWrap(GLenum target, SamplerWrap wrap) noexcept
{
	if (SamplerWrap::Repeat == wrap)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT));
	}
	else if (SamplerWrap::ClampToEdge == wrap)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	}
	else if (SamplerWrap::Mirror == wrap)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
	}
}

void
EGL2Texture::applyTextureFilter(GLenum target, SamplerFilter filter) noexcept
{
	if (filter == SamplerFilter::Nearest)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	}
	else if (filter == SamplerFilter::Linear)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	}
	else if (filter == SamplerFilter::NearestMipmapLinear)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
	}
	else if (filter == SamplerFilter::NearestMipmapNearest)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
	}
	else if (filter == SamplerFilter::LinearMipmapNearest)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
	}
	else if (filter == SamplerFilter::LinearMipmapLinear)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	}
}

void
EGL2Texture::applyTextureAnis(GLenum target, SamplerAnis anis) noexcept
{
	if (anis == SamplerAnis::Anis1)
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1));
	else if (anis == SamplerAnis::Anis2)
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2));
	else if (anis == SamplerAnis::Anis4)
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4));
	else if (anis == SamplerAnis::Anis8)
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8));
	else if (anis == SamplerAnis::Anis16)
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16));
}

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
EGL2RenderTexture::setup(TexturePtr texture) except
{
	assert(!_fbo);
	assert(texture);

	GL_CHECK(glGenFramebuffers(1, &_fbo));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));

	auto sharedDepthTarget = this->getSharedDepthTexture();
	auto sharedStencilTarget = this->getSharedStencilTexture();
	auto resolveFormat = this->getTexFormat();

	if (sharedDepthTarget)
		this->bindRenderTexture(sharedDepthTarget->getResolveTexture(), GL_DEPTH_ATTACHMENT);

	if (sharedStencilTarget)
		this->bindRenderTexture(sharedStencilTarget->getResolveTexture(), GL_STENCIL_ATTACHMENT);

	if (resolveFormat == TextureFormat::DEPTH_COMPONENT16 || resolveFormat == TextureFormat::DEPTH_COMPONENT24 || resolveFormat == TextureFormat::DEPTH_COMPONENT32)
		this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT);
	else if (resolveFormat == TextureFormat::STENCIL8)
		this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT);
	else
		this->bindRenderTexture(texture, GL_COLOR_ATTACHMENT0);

	_resolveTexture = texture;

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	return true;
}

void
EGL2RenderTexture::setup(std::size_t w, std::size_t h, TextureDim dim, TextureFormat format) except
{
	_resolveTexture = std::make_shared<EGL2Texture>();
	_resolveTexture->setWidth(w);
	_resolveTexture->setHeight(h);
	_resolveTexture->setTexDim(dim);
	_resolveTexture->setTexFormat(format);
	_resolveTexture->setup();

	this->setup(_resolveTexture);
}

void
EGL2RenderTexture::setup(std::size_t w, std::size_t h, std::size_t d, TextureDim dim, TextureFormat format) except
{
	_resolveTexture = std::make_shared<EGL2Texture>();
	_resolveTexture->setWidth(w);
	_resolveTexture->setHeight(h);
	_resolveTexture->setDepth(d);
	_resolveTexture->setTexDim(dim);
	_resolveTexture->setTexFormat(format);
	_resolveTexture->setup();

	this->setup(_resolveTexture);
}

void
EGL2RenderTexture::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		glDeleteFramebuffers(1, &_fbo);
		_fbo = GL_NONE;
	}
}

void
EGL2RenderTexture::bindRenderTexture(TexturePtr texture, GLenum attachment) noexcept
{
	auto handle = std::dynamic_pointer_cast<EGL2Texture>(texture)->getInstanceID();

	switch (texture->getTexDim())
	{
		case TextureDim::DIM_2D:
		{
			if (texture->isMultiSample())
				GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, handle, 0));
			else
				GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0));
			break;
		}
		case TextureDim::DIM_CUBE:
		{
			GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X, handle, 0));
			break;
		}
	}
}

void
EGL2RenderTexture::onSetRenderTextureAfter(RenderTexturePtr target) noexcept
{
	if (_fbo)
	{
		auto resolveFormat = target->getTexFormat();
		if (resolveFormat == TextureFormat::DEPTH24_STENCIL8 || resolveFormat == TextureFormat::DEPTH32_STENCIL8)
			this->bindRenderTexture(this->getResolveTexture(), GL_STENCIL_ATTACHMENT);
		else if (resolveFormat == TextureFormat::DEPTH_COMPONENT16 || resolveFormat == TextureFormat::DEPTH_COMPONENT24 || resolveFormat == TextureFormat::DEPTH_COMPONENT32)
			this->bindRenderTexture(this->getResolveTexture(), GL_DEPTH_ATTACHMENT);
		else if (resolveFormat == TextureFormat::STENCIL8)
			this->bindRenderTexture(this->getResolveTexture(), GL_STENCIL_ATTACHMENT);
		else
			this->bindRenderTexture(this->getResolveTexture(), GL_COLOR_ATTACHMENT0);
	}
}

GLuint
EGL2RenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

void
EGL2RenderTexture::setLayer(GLuint layer) noexcept
{
	_layer = layer;
}

GLuint
EGL2RenderTexture::getLayer() const noexcept
{
	return _layer;
}

EGL2MultiRenderTexture::EGL2MultiRenderTexture() noexcept
	: _fbo(GL_NONE)
{
}

EGL2MultiRenderTexture::~EGL2MultiRenderTexture() noexcept
{
	this->close();
}

bool
EGL2MultiRenderTexture::setup() noexcept
{
	assert(GL_NONE == _fbo);

	GL_CHECK(glGenFramebuffers(1, &_fbo));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));

	auto sharedDepthTarget = this->getSharedDepthTexture();
	auto sharedStencilTarget = this->getSharedStencilTexture();

	if (sharedDepthTarget)
		this->bindRenderTexture(sharedDepthTarget, GL_DEPTH_ATTACHMENT);

	if (sharedStencilTarget)
		this->bindRenderTexture(sharedStencilTarget, GL_STENCIL_ATTACHMENT);

	GLenum draw[MAX_COLOR_ATTACHMENTS] = { 0 };
	GLenum attachment = GL_COLOR_ATTACHMENT0;
	GLsizei count = 0;

	for (auto& target : this->getRenderTextures())
	{
		this->bindRenderTexture(target, attachment);
		draw[count++] = attachment++;
	}

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	return true;
}

void
EGL2MultiRenderTexture::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		glDeleteFramebuffers(1, &_fbo);
		_fbo = GL_NONE;
	}
}

GLuint
EGL2MultiRenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

void
EGL2MultiRenderTexture::bindRenderTexture(RenderTexturePtr target, GLenum attachment) noexcept
{
	auto handle = std::dynamic_pointer_cast<EGL2Texture>(target->getResolveTexture())->getInstanceID();

	switch (target->getTexDim())
	{
		case TextureDim::DIM_2D:
		{
			if (target->isMultiSample())
				GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, handle, 0));
			else
				GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0));
			break;
		}
	}
}

_NAME_END