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
#include "ogl_texture.h"

_NAME_BEGIN

__ImplementSubClass(OGLTexture, GraphicsTexture, "OGLTexture")

OGLTexture::OGLTexture() noexcept
	: _target(GL_INVALID_ENUM)
	, _pbo(GL_NONE)
	, _pboSize(0)
	, _upbo(GL_NONE)
	, _upboSize(0)
	, _texture(GL_NONE)
{
}

OGLTexture::~OGLTexture() noexcept
{
	this->close();
}

bool
OGLTexture::setup(const GraphicsTextureDesc& textureDesc) noexcept
{
	assert(_texture == GL_NONE);

	GLenum target = OGLTypes::asTextureTarget(textureDesc.getTexDim(), textureDesc.isMultiSample());
	if (target == GL_INVALID_ENUM)
		return false;

	GLenum internalFormat = OGLTypes::asTextureInternalFormat(textureDesc.getTexFormat());
	if (internalFormat == GL_INVALID_ENUM)
		return false;

	glGenTextures(1, &_texture);
	if (_texture == GL_NONE)
	{
		GL_PLATFORM_LOG("glGenTextures() fail");
		return false;
	}

	glBindTexture(target, _texture);

	GLsizei w = (GLsizei)textureDesc.getWidth();
	GLsizei h = (GLsizei)textureDesc.getHeight();
	GLsizei depth = (GLsizei)textureDesc.getDepth();

	GLsizei mipBase = textureDesc.getMipBase();
	GLsizei mipLevel = textureDesc.getMipLevel();

	if (!applySamplerWrap(target, textureDesc.getSamplerWrap()))
		return false;

	if (!applySamplerFilter(target, textureDesc.getSamplerFilter()))
		return false;

	if (!applySamplerAnis(target, textureDesc.getSamplerAnis()))
		return false;

	if (!applyMipmapLimit(target, mipBase, mipLevel))
		return false;

	const char* stream = (const char*)textureDesc.getStream();
	if (OGLTypes::isCompressedTexture(textureDesc.getTexFormat()))
	{
		std::size_t offset = 0;
		std::size_t blockSize = internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;

		GLint oldPackStore = 1;
		glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 8);

		for (GLint mip = mipBase; mip < mipBase + mipLevel; mip++)
		{
			auto mipSize = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;

			glCompressedTexImage2D(GL_TEXTURE_2D, mip, internalFormat, w, h, 0, mipSize, (char*)stream + offset);

			w = std::max(w >> 1, 1);
			h = std::max(h >> 1, 1);

			offset += stream ? mipSize : 0;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
	}
	else
	{
		GLenum format = OGLTypes::asTextureFormat(textureDesc.getTexFormat());
		if (format == GL_INVALID_ENUM)
			return false;

		GLenum type = OGLTypes::asTextureType(textureDesc.getTexFormat());
		if (type == GL_INVALID_ENUM)
			return false;

		GLsizei offset = 0;
		GLsizei pixelSize = stream ? OGLTypes::getFormatNum(format, type) : 1;

		GLint oldPackStore = 1;
		glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);

		if (pixelSize == 1)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		else if (pixelSize == 2)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		else if (pixelSize == 4 || pixelSize == 12)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		else if (pixelSize == 8 || pixelSize == 16)
			glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
		else
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (GLsizei mip = mipBase; mip < mipBase + mipLevel; mip++)
		{
			GLsizei mipSize = w * h * pixelSize;
			GLsizei layerBase = textureDesc.getLayerBase();
			GLsizei layerLevel = textureDesc.getLayerNums();

			for (GLsizei layer = layerBase; layer < layerBase + layerLevel; layer++)
			{
				if (target == GL_TEXTURE_2D ||
					target == GL_TEXTURE_2D_MULTISAMPLE)
				{
					glTexImage2D(target, mip, internalFormat, w, h, 0, format, type, stream ? stream + offset : nullptr);
				}
				else
				{
					if (target == GL_TEXTURE_CUBE_MAP_ARRAY ||
						target == GL_TEXTURE_CUBE_MAP)
					{
						GLenum cubeFace[] =
						{
							GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
							GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
							GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
						};

						for (GLsizei face = 0; face < 6; face++)
						{
							if (target == GL_TEXTURE_CUBE_MAP)
								glTexImage2D(cubeFace[face], mip, internalFormat, w, h, 0, format, type, stream ? (char*)stream + offset : nullptr);
							else
								glTexImage3D(cubeFace[face], mip, internalFormat, w, h, layer, 0, format, type, stream ? (char*)stream + offset : nullptr);

							offset += mipSize;
						}
					}
					else
					{
						glTexImage3D(target, mip, internalFormat, w, h, depth * layer, 0, format, type, stream ? (char*)stream + offset : nullptr);
						offset += mipSize;
					}
				}
			}

			w = std::max(w >> 1, 1);
			h = std::max(h >> 1, 1);
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
	}

	glBindTexture(target, GL_NONE);

	_target = target;
	_textureDesc = textureDesc;

	return OGLCheck::checkError();
}

void
OGLTexture::close() noexcept
{
	if (_texture != GL_NONE)
	{
		glDeleteTextures(1, &_texture);
		_texture = GL_NONE;
	}

	if (_pbo != GL_NONE)
	{
		glDeleteBuffers(1, &_pbo);
		_pbo = GL_NONE;
	}

	if (_upbo != GL_NONE)
	{
		glDeleteBuffers(1, &_upbo);
		_upbo = GL_NONE;
	}
}

bool
OGLTexture::map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, void** data) noexcept
{
	assert(data);

	GLenum format = OGLTypes::asTextureFormat(_textureDesc.getTexFormat());
	if (format == GL_INVALID_ENUM)
		return false;

	GLenum type = OGLTypes::asTextureType(_textureDesc.getTexFormat());
	if (type == GL_INVALID_ENUM)
		return false;

	if (type == GL_HALF_FLOAT)
		type = GL_FLOAT;

	GLsizei num = OGLTypes::getFormatNum(format, type);
	if (num == 0)
		return false;

	if (_pbo == GL_NONE)
		glGenBuffers(1, &_pbo);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo);

	GLsizei mapSize = w * h * num;
	if (_pboSize < mapSize)
	{
		glBufferData(GL_PIXEL_PACK_BUFFER, mapSize, nullptr, GL_STREAM_READ);
		_pboSize = mapSize;
	}

	glBindTexture(_target, _texture);
	glGetTexImage(_target, 0, format, type, 0);

	*data = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, mapSize, GL_MAP_READ_BIT);
	data += (y * _textureDesc.getWidth() * num) * x;

	return *data ? true : false;
}

void
OGLTexture::unmap() noexcept
{
	glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo);
	glUnmapBufferARB(GL_PIXEL_PACK_BUFFER);
}

GLenum
OGLTexture::getTarget() const noexcept
{
	return _target;
}

GLuint
OGLTexture::getInstanceID() const noexcept
{
	return _texture;
}

bool
OGLTexture::applyMipmapLimit(GLenum target, std::uint32_t min, std::uint32_t count) noexcept
{
	glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, min);
	glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, min + count - 1);
	return true;
}

bool
OGLTexture::applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept
{
	GLenum glwrap = OGLTypes::asSamplerWrap(wrap);
	if (glwrap != GL_INVALID_ENUM)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, glwrap);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, glwrap);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, glwrap);

		return true;
	}

	return false;
}

bool
OGLTexture::applySamplerFilter(GLenum target, GraphicsSamplerFilter filter) noexcept
{
	GLenum min = OGLTypes::asSamplerMinFilter(filter);
	GLenum mag = OGLTypes::asSamplerMagFilter(filter);
	if (min != GL_INVALID_ENUM && mag != GL_INVALID_ENUM)
	{
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag);
		return true;
	}

	return false;
}

bool
OGLTexture::applySamplerAnis(GLenum target, GraphicsSamplerAnis anis) noexcept
{
	if (GLEW_EXT_texture_filter_anisotropic)
	{
		if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis1)
			glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
		else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis2)
			glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
		else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis4)
			glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
		else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis8)
			glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
		else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis16)
			glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
		else
		{
			GL_PLATFORM_LOG("Can't support anisotropy format");
			return false;
		}
	}
	return true;
}

const GraphicsTextureDesc&
OGLTexture::getGraphicsTextureDesc() const noexcept
{
	return _textureDesc;
}

void
OGLTexture::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLTexture::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END