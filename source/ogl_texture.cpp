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
#include <ray/ogl_texture.h>

_NAME_BEGIN

#define MAX_COLOR_ATTACHMENTS 15

OGLTextureSample::OGLTextureSample() noexcept
	:_sample(0)
{
}

OGLTextureSample::~OGLTextureSample() noexcept
{
	this->close();
}

bool 
OGLTextureSample::setup() except
{
	assert(!_sample);

	glGenSamplers(1, &_sample);
	
	auto wrap = this->getTexWrap();
	if (TextureWrap::REPEAT & wrap)
	{
		glSamplerParameteri(_sample, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(_sample, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glSamplerParameteri(_sample, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else if (TextureWrap::CLAMP_TO_EDGE & wrap)
	{
		glSamplerParameteri(_sample, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_sample, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_sample, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	else if (TextureWrap::MODE_MIRROR & wrap)
	{
		glSamplerParameteri(_sample, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glSamplerParameteri(_sample, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glSamplerParameteri(_sample, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	}

	auto filter = this->getTexFilter();
	if (filter == TextureFilter::GPU_NEAREST)
	{
		glSamplerParameteri(_sample, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(_sample, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if (filter == TextureFilter::GPU_LINEAR)
	{
		glSamplerParameteri(_sample, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_sample, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else if (filter == TextureFilter::GPU_NEAREST_MIPMAP_LINEAR)
	{
		glSamplerParameteri(_sample, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glSamplerParameteri(_sample, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	}
	else if (filter == TextureFilter::GPU_NEAREST_MIPMAP_NEAREST)
	{
		glSamplerParameteri(_sample, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glSamplerParameteri(_sample, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	}
	else if (filter == TextureFilter::GPU_LINEAR_MIPMAP_NEAREST)
	{
		glSamplerParameteri(_sample, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glSamplerParameteri(_sample, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	}
	else if (filter == TextureFilter::GPU_LINEAR_MIPMAP_LINEAR)
	{
		glSamplerParameteri(_sample, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glSamplerParameteri(_sample, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	auto anis = this->getTexAnisotropy();
	if (anis == Anisotropy::ANISOTROPY_1)
		glSamplerParameteri(_sample, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
	else if (anis == Anisotropy::ANISOTROPY_2)
		glSamplerParameteri(_sample, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
	else if (anis == Anisotropy::ANISOTROPY_4)
		glSamplerParameteri(_sample, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	else if (anis == Anisotropy::ANISOTROPY_8)
		glSamplerParameteri(_sample, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
	else if (anis == Anisotropy::ANISOTROPY_16)
		glSamplerParameteri(_sample, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

	return true;
}

void 
OGLTextureSample::close() noexcept
{
	if (_sample)
	{
		glDeleteSamplers(1, &_sample);
		_sample = 0;
	}
}

GLuint 
OGLTextureSample::getInstanceID() noexcept
{
	return _sample;
}

OGLTexture::OGLTexture() noexcept
	: _texture(0)
	, _textureAddr(0)
	, _sampleAddr(0)
{
}

OGLTexture::~OGLTexture() noexcept
{
	this->close();
}

bool
OGLTexture::setup() except
{
	auto target = OGLTypes::asOGLTarget(this->getTexDim());
	auto internalFormat = OGLTypes::asOGLInternalformat(this->getTexFormat());
	auto stream = this->getStream();

	glGenTextures(1, &_texture);
	glBindTexture(target, _texture);

	GLsizei w = (GLsizei)this->getWidth();
	GLsizei h = (GLsizei)this->getHeight();
	GLsizei depth = (GLsizei)this->getDepth();

	applyTextureWrap(target, this->getTexWrap());
	applyTextureFilter(target, this->getTexFilter());
	applyTextureAnis(target, this->getTexAnisotropy());

#if !defined(EGLAPI)
	if (internalFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
		internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
		internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
		internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT ||
		internalFormat == GL_COMPRESSED_RG_RGTC2)
#else
	if (internalFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
		internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
		internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
		internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
#endif
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
		auto level = 0;
		auto format = OGLTypes::asOGLFormat(this->getTexFormat());
		auto type = OGLTypes::asOGLType(this->getTexFormat());

		switch (target)
		{
		case GL_TEXTURE_2D:
		{
#if !defined(EGLAPI)
			if (this->isMultiSample())
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, level, internalFormat, w, h, GL_FALSE);
			else
#endif
			{
				glTexImage2D(target, level, internalFormat, w, h, 0, format, type, stream);
			}
		}
		break;
		case GL_TEXTURE_2D_ARRAY:
		{
#if !defined(EGLAPI)
			if (this->isMultiSample())
				glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, level, internalFormat, w, h, depth, GL_FALSE);
			else
#endif
			{
				glTexImage3D(target, level, internalFormat, w, h, depth, 0, format, type, 0);
			}
		}
		break;
		case GL_TEXTURE_3D:
		{
			glTexImage3D(target, level, internalFormat, w, h, depth, 0, format, type, 0);
		}
		break;
		case GL_TEXTURE_CUBE_MAP:
		{
#if !defined(EGLAPI)
			if (this->isMultiSample())
			{
				glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, internalFormat, w, h, GL_FALSE);
				glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, internalFormat, w, h, GL_FALSE);
				glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, internalFormat, w, h, GL_FALSE);
				glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, internalFormat, w, h, GL_FALSE);
				glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, internalFormat, w, h, GL_FALSE);
				glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, internalFormat, w, h, GL_FALSE);
			}
			else
#endif
			{
#if !defined(EGLAPI)
				if (OGLFeatures::ARB_direct_state_access && !stream)
					glTextureStorage2D(_texture, level + 1, internalFormat, w, h);
				else
#endif
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, internalFormat, w, h, 0, format, type, stream);
					glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, internalFormat, w, h, 0, format, type, stream);
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, internalFormat, w, h, 0, format, type, stream);
					glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, internalFormat, w, h, 0, format, type, stream);
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, internalFormat, w, h, 0, format, type, stream);
					glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, internalFormat, w, h, 0, format, type, stream);
				}
			}
		}
		break;
		default:
			break;
		}
	}

	if (this->isMipmap())
	{
#if !defined(EGLAPI)
		if (OGLFeatures::ARB_direct_state_access)
			glGenerateTextureMipmap(_texture);
		else
#endif
			glGenerateMipmap(target);		
	}

#if !defined(EGLAPI)
	if (OGLFeatures::ARB_bindless_texture)
	{
		_textureAddr = glGetTextureHandleARB(_texture);
		glMakeTextureHandleResidentARB(_textureAddr);
	}

#endif

	return true;
}

void
OGLTexture::close() noexcept
{
	if (_texture)
	{
		glDeleteTextures(1, &_texture);
		_texture = GL_NONE;
		_textureAddr = GL_NONE;
	}
}

GLuint
OGLTexture::getInstanceID() noexcept
{
	return _texture;
}

GLuint64
OGLTexture::getInstanceAddr() noexcept
{
	return _textureAddr;
}

void
OGLTexture::applyTextureWrap(GLenum target, TextureWrap wrap) noexcept
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
OGLTexture::applyTextureFilter(GLenum target, TextureFilter filter) noexcept
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
OGLTexture::applyTextureAnis(GLenum target, Anisotropy anis) noexcept
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

OGLRenderTexture::OGLRenderTexture() noexcept
	: _fbo(GL_NONE)
	, _layer(GL_NONE)
{
}

OGLRenderTexture::~OGLRenderTexture() noexcept
{
	this->close();
}

bool
OGLRenderTexture::setup() noexcept
{
	assert(!_fbo);

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

	if (resolveFormat == PixelFormat::DEPTH24_STENCIL8 || resolveFormat == PixelFormat::DEPTH32_STENCIL8)
		this->bindRenderTexture(this->getResolveTexture(), GL_DEPTH_STENCIL_ATTACHMENT);
	else if (resolveFormat == PixelFormat::DEPTH_COMPONENT16 || resolveFormat == PixelFormat::DEPTH_COMPONENT24 || resolveFormat == PixelFormat::DEPTH_COMPONENT32)
		this->bindRenderTexture(this->getResolveTexture(), GL_DEPTH_ATTACHMENT);
	else if (resolveFormat == PixelFormat::STENCIL8)
		this->bindRenderTexture(this->getResolveTexture(), GL_STENCIL_ATTACHMENT);
	else
		this->bindRenderTexture(this->getResolveTexture(), GL_COLOR_ATTACHMENT0);

#if !defined(EGLAPI)
	if (OGLFeatures::ARB_direct_state_access)
		glNamedFramebufferDrawBuffer(_fbo, GL_COLOR_ATTACHMENT0);
	else
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
#else
	GLenum draw = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &draw);
#endif

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
OGLRenderTexture::bindRenderTexture(TexturePtr texture, GLenum attachment) noexcept
{
	auto handle = std::dynamic_pointer_cast<OGLTexture>(texture)->getInstanceID();

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
#if !defined(EGLAPI)
	case TextureDim::DIM_3D:
	{
		glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, handle, 0, 0);
		break;
	}
#endif
	}
}

void
OGLRenderTexture::onSetRenderTextureAfter(RenderTexturePtr target) noexcept
{
	if (_fbo)
	{
		auto resolveFormat = target->getTexFormat();
		if (resolveFormat == PixelFormat::DEPTH24_STENCIL8 || resolveFormat == PixelFormat::DEPTH32_STENCIL8)
			this->bindRenderTexture(this->getResolveTexture(), GL_STENCIL_ATTACHMENT);
		else if (resolveFormat == PixelFormat::DEPTH_COMPONENT16 || resolveFormat == PixelFormat::DEPTH_COMPONENT24 || resolveFormat == PixelFormat::DEPTH_COMPONENT32)
			this->bindRenderTexture(this->getResolveTexture(), GL_DEPTH_ATTACHMENT);
		else if (resolveFormat == PixelFormat::STENCIL8)
			this->bindRenderTexture(this->getResolveTexture(), GL_STENCIL_ATTACHMENT);
		else
			this->bindRenderTexture(this->getResolveTexture(), GL_COLOR_ATTACHMENT0);
	}
}

GLuint
OGLRenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

void 
OGLRenderTexture::setLayer(GLuint layer) noexcept
{
	_layer = layer;
}

GLuint 
OGLRenderTexture::getLayer() const noexcept
{
	return _layer;
}

OGLMultiRenderTexture::OGLMultiRenderTexture() noexcept
	: _fbo(GL_NONE)
{
}

OGLMultiRenderTexture::~OGLMultiRenderTexture() noexcept
{
	this->close();
}

bool
OGLMultiRenderTexture::setup() noexcept
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

#if !defined(EGLAPI)
	if (OGLFeatures::ARB_direct_state_access)
		glNamedFramebufferDrawBuffers(_fbo, count, draw);
	else
		glDrawBuffers(count, draw);
#else
	glDrawBuffers(count, draw);
#endif

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

GLuint
OGLMultiRenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

void
OGLMultiRenderTexture::bindRenderTexture(RenderTexturePtr target, GLenum attachment) noexcept
{
	auto handle = std::dynamic_pointer_cast<OGLTexture>(target->getResolveTexture())->getInstanceID();

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
	case TextureDim::DIM_3D:
	{
#if !defined(EGLAPI)
		glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, handle, 0, 0);
		break;
#else
		assert(false);
#endif
	}
	}
}

_NAME_END