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
	: _texture(GL_NONE)
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

	GLenum target = EGL2Types::asTextureTarget(textureDesc.getTexDim(), textureDesc.isMultiSample());
	if (target == GL_INVALID_ENUM)
		return false;

	GLenum internalFormat = EGL2Types::asTextureInternalFormat(textureDesc.getTexFormat());
	if (internalFormat == GL_INVALID_ENUM)
		return false;

	GL_CHECK(glGenTextures(1, &_texture));
	if (_texture == GL_NONE)
	{
		GL_PLATFORM_LOG("glGenTextures() fail");
		return false;
	}

	GL_CHECK(glBindTexture(target, _texture));

	GLsizei w = (GLsizei)textureDesc.getWidth();
	GLsizei h = (GLsizei)textureDesc.getHeight();

	GLsizei mipBase = textureDesc.getMipBase();
	GLsizei mipLevel = textureDesc.getMipLevel();

	if (!applySamplerWrap(target, textureDesc.getSamplerWrap()))
		return false;

	if (!applySamplerFilter(target, textureDesc.getSamplerFilter()))
		return false;

	if (!applySamplerAnis(target, textureDesc.getSamplerAnis()))
		return false;

	const char* stream = (const char*)textureDesc.getStream();

	if (EGL2Types::isCompressedTexture(textureDesc.getTexFormat()))
	{
		std::size_t offset = 0;
		std::size_t blockSize = internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;

		for (GLint mip = mipBase; mip < mipBase + mipLevel; mip++)
		{
			auto mipSize = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;

			GL_CHECK(glCompressedTexImage2D(GL_TEXTURE_2D, mip, internalFormat, w, h, 0, mipSize, (char*)stream + offset));

			w = std::max(w >> 1, 1);
			h = std::max(h >> 1, 1);

			offset += stream ? mipSize : 0;
		}
	}
	else
	{
		GLenum format = EGL2Types::asTextureFormat(textureDesc.getTexFormat());
		GLenum type = EGL2Types::asTextureType(textureDesc.getTexFormat());

		switch (target)
		{
		case GL_TEXTURE_2D:
		case GL_TEXTURE_2D_MULTISAMPLE:
			GL_CHECK(glTexImage2D(target, mipLevel, internalFormat, w, h, 0, format, type, stream));
			break;
		case GL_TEXTURE_CUBE_MAP:
			GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipLevel, internalFormat, w, h, 0, format, type, stream));
			GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipLevel, internalFormat, w, h, 0, format, type, stream));
			GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipLevel, internalFormat, w, h, 0, format, type, stream));
			GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipLevel, internalFormat, w, h, 0, format, type, stream));
			GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipLevel, internalFormat, w, h, 0, format, type, stream));
			GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipLevel, internalFormat, w, h, 0, format, type, stream));
			break;
#ifndef __AMD__
		case GL_TEXTURE_CUBE_MAP_ARRAY_EXT:
		{
			GLsizei depth = (GLsizei)textureDesc.getDepth();
			GL_CHECK(glTexImage3DOES(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipLevel, internalFormat, w, h, depth, 0, format, type, stream));
			GL_CHECK(glTexImage3DOES(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipLevel, internalFormat, w, h, depth, 0, format, type, stream));
			GL_CHECK(glTexImage3DOES(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipLevel, internalFormat, w, h, depth, 0, format, type, stream));
			GL_CHECK(glTexImage3DOES(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipLevel, internalFormat, w, h, depth, 0, format, type, stream));
			GL_CHECK(glTexImage3DOES(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipLevel, internalFormat, w, h, depth, 0, format, type, stream));
			GL_CHECK(glTexImage3DOES(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipLevel, internalFormat, w, h, depth, 0, format, type, stream));
		}
		break;
#endif
		default:
			assert(false);
			return false;
			break;
		}
	}

	GL_CHECK(glBindTexture(target, GL_NONE));

	_target = target;
	_textureDesc = textureDesc;

	return EGL2Check::checkError();
}

void
EGL2Texture::close() noexcept
{
	if (_texture != GL_NONE)
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
EGL2Texture::getInstanceID() const noexcept
{
	return _texture;
}

bool
EGL2Texture::applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept
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
EGL2Texture::applySamplerFilter(GLenum target, GraphicsSamplerFilter filter) noexcept
{
	GLenum min = EGL2Types::asSamplerMinFilter(filter);
	GLenum mag = EGL2Types::asSamplerMagFilter(filter);
	if (min != GL_INVALID_ENUM && mag != GL_INVALID_ENUM)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag));
		return true;
	}

	return false;
}

bool
EGL2Texture::applySamplerAnis(GLenum target, GraphicsSamplerAnis anis) noexcept
{
	if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_filter_anisotropic))
	{
		if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis1)
			GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1));
		else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis2)
			GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2));
		else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis4)
			GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4));
		else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis8)
			GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8));
		else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis16)
			GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16));
		else
		{
			GL_PLATFORM_LOG("Can't support anisotropy format");
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