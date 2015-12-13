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
#include "egl3_texture.h"

_NAME_BEGIN

#define MAX_COLOR_ATTACHMENTS 15

EGL3TextureSampler::EGL3TextureSampler() noexcept
	: _sampler(0)
{
}

EGL3TextureSampler::~EGL3TextureSampler() noexcept
{
	this->close();
}

bool 
EGL3TextureSampler::setup() except
{
	assert(!_sampler);

	glGenSamplers(1, &_sampler);
	
	auto wrap = this->getTexWrap();
	if (TextureWrap::REPEAT & wrap)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else if (TextureWrap::CLAMP_TO_EDGE & wrap)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	else if (TextureWrap::MODE_MIRROR & wrap)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	}

	auto filter = this->getTexFilter();
	if (filter == TextureFilter::GPU_NEAREST)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if (filter == TextureFilter::GPU_LINEAR)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else if (filter == TextureFilter::GPU_NEAREST_MIPMAP_LINEAR)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	}
	else if (filter == TextureFilter::GPU_NEAREST_MIPMAP_NEAREST)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	}
	else if (filter == TextureFilter::GPU_LINEAR_MIPMAP_NEAREST)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	}
	else if (filter == TextureFilter::GPU_LINEAR_MIPMAP_LINEAR)
	{
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	auto anis = this->getTexAnisotropy();
	if (anis == Anisotropy::ANISOTROPY_1)
		glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
	else if (anis == Anisotropy::ANISOTROPY_2)
		glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
	else if (anis == Anisotropy::ANISOTROPY_4)
		glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	else if (anis == Anisotropy::ANISOTROPY_8)
		glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
	else if (anis == Anisotropy::ANISOTROPY_16)
		glSamplerParameteri(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

	return true;
}

void 
EGL3TextureSampler::close() noexcept
{
	if (_sampler)
	{
		glDeleteSamplers(1, &_sampler);
		_sampler = 0;
	}
}

GLuint 
EGL3TextureSampler::getInstanceID() noexcept
{
	return _sampler;
}

EGL3Texture::EGL3Texture() noexcept
	: _texture(0)
{
}

EGL3Texture::~EGL3Texture() noexcept
{
	this->close();
}

bool
EGL3Texture::setup() except
{
	auto target = EGL3Types::asEGL3Target(this->getTexDim());
	auto internalFormat = EGL3Types::asEGL3Internalformat(this->getTexFormat());
	auto stream = this->getStream();

	glGenTextures(1, &_texture);
	glBindTexture(target, _texture);

	GLsizei w = (GLsizei)this->getWidth();
	GLsizei h = (GLsizei)this->getHeight();
	GLsizei depth = (GLsizei)this->getDepth();

	applyTextureWrap(target, this->getTexWrap());
	applyTextureFilter(target, this->getTexFilter());
	applyTextureAnis(target, this->getTexAnisotropy());

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

			glCompressedTexImage2D(GL_TEXTURE_2D, mip, internalFormat, w, h, 0, mipSize, (char*)stream + offset);

			w = std::max(w >> 1, 1);
			h = std::max(h >> 1, 1);

			offset += mipSize;
		}
	}
	else
	{
		auto format = EGL3Types::asEGL3Format(this->getTexFormat());
		auto type = EGL3Types::asEGL3Type(this->getTexFormat());

		if (stream)
		{
			auto level = 0;

			switch (target)
			{
			case GL_TEXTURE_2D:
				glTexImage2D(target, level, internalFormat, w, h, 0, format, type, stream);
			break;
			case GL_TEXTURE_2D_ARRAY:
				glTexImage3D(target, level, internalFormat, w, h, depth, 0, format, type, stream);
			break;
			case GL_TEXTURE_3D:
				glTexImage3D(target, level, internalFormat, w, h, depth, 0, format, type, stream);
			break;
			case GL_TEXTURE_CUBE_MAP:
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, internalFormat, w, h, 0, format, type, stream);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, internalFormat, w, h, 0, format, type, stream);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, internalFormat, w, h, 0, format, type, stream);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, internalFormat, w, h, 0, format, type, stream);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, internalFormat, w, h, 0, format, type, stream);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, internalFormat, w, h, 0, format, type, stream);
			}
			break;
			default:
				assert(false);
				break;
			}
		}
		else
		{
			auto level = target == GL_TEXTURE_CUBE_MAP ? 6 : 1;

			switch (target)
			{
			case GL_TEXTURE_2D:
			{
				if (this->isMultiSample())
					glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, level, internalFormat, w, h, GL_FALSE);
				else
					glTexStorage2D(target, level, internalFormat, w, h);
			}
			break;
			case GL_TEXTURE_2D_ARRAY:
			{
				glTexStorage3D(target, level, internalFormat, w, h, depth);
			}
			break;
			case GL_TEXTURE_3D:
			{
				glTexStorage3D(target, level, internalFormat, w, h, depth);
			}
			break;
			case GL_TEXTURE_CUBE_MAP:
			{
				if (this->isMultiSample())
					glTexStorage2DMultisample(GL_TEXTURE_CUBE_MAP, level, internalFormat, w, h, GL_FALSE);
				else
					glTexStorage2D(GL_TEXTURE_CUBE_MAP, level, internalFormat, w, h);
			}
			break;
			default:
				break;
			}
		}
	}

	if (this->isMipmap())
	{
		glGenerateMipmap(target);		
	}

	EGL3Check::checkError();

	return true;
}

void
EGL3Texture::close() noexcept
{
	if (_texture)
	{
		glDeleteTextures(1, &_texture);
		_texture = GL_NONE;
	}
}

GLuint
EGL3Texture::getInstanceID() noexcept
{
	return _texture;
}

void
EGL3Texture::applyTextureWrap(GLenum target, TextureWrap wrap) noexcept
{
	if (TextureWrap::REPEAT & wrap)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else if (TextureWrap::CLAMP_TO_EDGE & wrap)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	else if (TextureWrap::MODE_MIRROR & wrap)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	}
}

void
EGL3Texture::applyTextureFilter(GLenum target, TextureFilter filter) noexcept
{
	if (filter == TextureFilter::GPU_NEAREST)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if (filter == TextureFilter::GPU_LINEAR)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else if (filter == TextureFilter::GPU_NEAREST_MIPMAP_LINEAR)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	}
	else if (filter == TextureFilter::GPU_NEAREST_MIPMAP_NEAREST)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	}
	else if (filter == TextureFilter::GPU_LINEAR_MIPMAP_NEAREST)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	}
	else if (filter == TextureFilter::GPU_LINEAR_MIPMAP_LINEAR)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
}

void
EGL3Texture::applyTextureAnis(GLenum target, Anisotropy anis) noexcept
{
	if (anis == Anisotropy::ANISOTROPY_1)
		glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
	else if (anis == Anisotropy::ANISOTROPY_2)
		glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
	else if (anis == Anisotropy::ANISOTROPY_4)
		glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	else if (anis == Anisotropy::ANISOTROPY_8)
		glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
	else if (anis == Anisotropy::ANISOTROPY_16)
		glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
}

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
EGL3RenderTexture::setup(TexturePtr texture) except
{
	assert(!_fbo);
	assert(texture);

	EGL3Check::checkError();

	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	auto sharedDepthTarget = this->getSharedDepthTexture();
	auto sharedStencilTarget = this->getSharedStencilTexture();

	if (sharedDepthTarget == sharedStencilTarget)
	{
		if (sharedDepthTarget)
			this->bindRenderTexture(sharedDepthTarget->getResolveTexture(), GL_DEPTH_STENCIL_ATTACHMENT);
	}
	else
	{
		if (sharedDepthTarget)
			this->bindRenderTexture(sharedDepthTarget->getResolveTexture(), GL_DEPTH_ATTACHMENT);

		if (sharedStencilTarget)
			this->bindRenderTexture(sharedStencilTarget->getResolveTexture(), GL_STENCIL_ATTACHMENT);
	}

	auto resolveFormat = this->getTexFormat();

	if (resolveFormat == TextureFormat::DEPTH24_STENCIL8 || resolveFormat == TextureFormat::DEPTH32_STENCIL8)
		this->bindRenderTexture(texture, GL_DEPTH_STENCIL_ATTACHMENT);
	else if (resolveFormat == TextureFormat::DEPTH_COMPONENT16 || resolveFormat == TextureFormat::DEPTH_COMPONENT24 || resolveFormat == TextureFormat::DEPTH_COMPONENT32)
		this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT);
	else if (resolveFormat == TextureFormat::STENCIL8)
		this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT);
	else
		this->bindRenderTexture(texture, GL_COLOR_ATTACHMENT0);

	_resolveTexture = texture;

	GLenum draw = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &draw);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void
EGL3RenderTexture::setup(std::size_t w, std::size_t h, TextureDim dim, TextureFormat format) except
{
	_resolveTexture = std::make_shared<EGL3Texture>();
	_resolveTexture->setWidth(w);
	_resolveTexture->setHeight(h);
	_resolveTexture->setTexDim(dim);
	_resolveTexture->setTexFormat(format);
	_resolveTexture->setup();

	this->setup(_resolveTexture);
}

void
EGL3RenderTexture::setup(std::size_t w, std::size_t h, std::size_t d, TextureDim dim, TextureFormat format) except
{
	_resolveTexture = std::make_shared<EGL3Texture>();
	_resolveTexture->setWidth(w);
	_resolveTexture->setHeight(h);
	_resolveTexture->setDepth(d);
	_resolveTexture->setTexDim(dim);
	_resolveTexture->setTexFormat(format);
	_resolveTexture->setup();

	this->setup(_resolveTexture);
}

void
EGL3RenderTexture::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		glDeleteFramebuffers(1, &_fbo);
		_fbo = GL_NONE;
	}
}

void
EGL3RenderTexture::bindRenderTexture(TexturePtr texture, GLenum attachment) noexcept
{
	auto handle = std::dynamic_pointer_cast<EGL3Texture>(texture)->getInstanceID();

	switch (texture->getTexDim())
	{
		case TextureDim::DIM_2D:
		{
			if (texture->isMultiSample())
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, handle, 0);
			else
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0);
			break;
		}
		case TextureDim::DIM_2D_ARRAY:
		{
			glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, handle, 0, 0);
			break;
		}
		case TextureDim::DIM_CUBE:
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X, handle, 0);
			break;
		}
	}
}

void
EGL3RenderTexture::onSetRenderTextureAfter(RenderTexturePtr target) noexcept
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
EGL3RenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

void 
EGL3RenderTexture::setLayer(GLuint layer) noexcept
{
	_layer = layer;
}

GLuint 
EGL3RenderTexture::getLayer() const noexcept
{
	return _layer;
}

EGL3MultiRenderTexture::EGL3MultiRenderTexture() noexcept
	: _fbo(GL_NONE)
{
}

EGL3MultiRenderTexture::~EGL3MultiRenderTexture() noexcept
{
	this->close();
}

bool
EGL3MultiRenderTexture::setup() noexcept
{
	assert(GL_NONE == _fbo);

	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	auto sharedDepthTarget = this->getSharedDepthTexture();
	auto sharedStencilTarget = this->getSharedStencilTexture();

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

	for (auto& target : this->getRenderTextures())
	{
		this->bindRenderTexture(target, attachment);
		draw[count++] = attachment++;
	}

	glDrawBuffers(count, draw);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void
EGL3MultiRenderTexture::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		glDeleteFramebuffers(1, &_fbo);
		_fbo = GL_NONE;
	}
}

GLuint
EGL3MultiRenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

void
EGL3MultiRenderTexture::bindRenderTexture(RenderTexturePtr target, GLenum attachment) noexcept
{
	auto handle = std::dynamic_pointer_cast<EGL3Texture>(target->getResolveTexture())->getInstanceID();

	switch (target->getTexDim())
	{
		case TextureDim::DIM_2D:
		{
			if (target->isMultiSample())
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, handle, 0);
			else
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0);
			break;
		}
		case TextureDim::DIM_2D_ARRAY:
		{
			glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, handle, 0, 0);
			break;
		}
	}
}

_NAME_END