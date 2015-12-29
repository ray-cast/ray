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

__ImplementSubClass(EGL2Texture, GraphicsTexture, "EGL2Texture")

EGL2Texture::EGL2Texture() noexcept
	: _texture(0)
	, _target(GL_INVALID_ENUM)
{
}

EGL2Texture::~EGL2Texture() noexcept
{
	this->close();
}

bool
EGL2Texture::setup(const GraphicsTextureDesc& textureDesc) noexcept
{
	assert(_texture == GL_NONE);

	GL_CHECK(glGenTextures(1, &_texture));
	if (_texture == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateTextures fail");
		return false;
	}

	GLenum target = EGL2Types::asTarget(textureDesc.getTexDim(), textureDesc.isMultiSample());
	if (target == GL_INVALID_ENUM)
		return false;

	GL_CHECK(glBindTexture(target, _texture));

	GLsizei w = (GLsizei)textureDesc.getWidth();
	GLsizei h = (GLsizei)textureDesc.getHeight();
	GLsizei depth = (GLsizei)textureDesc.getDepth();

	if (!applySamplerWrap(target, textureDesc.getSamplerWrap()))
		return false;

	if (!applySamplerFilter(target, textureDesc.getSamplerFilter()))
		return false;

	if (!applySamplerAnis(target, textureDesc.getSamplerAnis()))
		return false;
	
	auto internalFormat = EGL2Types::asInternalformat(textureDesc.getTexFormat());
	if (internalFormat == GL_INVALID_ENUM)
		return false;

	auto stream = textureDesc.getStream();
	if (internalFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
		internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
		internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
		internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
	{
		GLint level = (GLint)textureDesc.getMipLevel();
		GLsizei size = textureDesc.getMipSize();
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
		auto format = EGL2Types::asFormat(textureDesc.getTexFormat());
		auto type = EGL2Types::asType(textureDesc.getTexFormat());

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

	if (textureDesc.isMipmap())
		GL_CHECK(glGenerateMipmap(target));

	_target = target;
	_textureDesc = textureDesc;

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

GLenum
EGL2Texture::getTarget() const noexcept
{
	return _target;
}

GLuint
EGL2Texture::getInstanceID() noexcept
{
	return _texture;
}

bool
EGL2Texture::applySamplerWrap(GLenum target, SamplerWrap wrap) noexcept
{
	GLenum glwrap = EGL2Types::asSamplerWrap(wrap);
	if (glwrap != GL_INVALID_ENUM)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_S, glwrap));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_T, glwrap));

		return true;
	}

	return false;
}

bool
EGL2Texture::applySamplerFilter(GLenum target, SamplerFilter filter) noexcept
{
	GLenum glfilter = EGL2Types::asSamplerFilter(filter);
	if (glfilter != GL_INVALID_ENUM)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, glfilter));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, glfilter));

		return true;
	}

	return false;
}

bool
EGL2Texture::applySamplerAnis(GLenum target, SamplerAnis anis) noexcept
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
	else
	{
		if (anis != SamplerAnis::Anis0)
		{
			GL_PLATFORM_LOG("Invalid SamplerAnis");
			return false;
		}
	}

	return true;
}

const GraphicsTextureDesc&
EGL2Texture::getGraphicsTextureDesc() const noexcept
{
	return _textureDesc;
}

void
EGL2Texture::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2Texture::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END