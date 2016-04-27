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

__ImplementSubClass(EGL3Texture, GraphicsTexture, "EGL3Texture")

EGL3Texture::EGL3Texture() noexcept
	: _texture(GL_NONE)
	, _target(GL_INVALID_ENUM)
{
}

EGL3Texture::~EGL3Texture() noexcept
{
	this->close();
}

bool
EGL3Texture::setup(const GraphicsTextureDesc& textureDesc) noexcept
{
	assert(_texture == GL_NONE);

	GLenum target = EGL3Types::asTextureTarget(textureDesc.getTexDim(), textureDesc.isMultiSample());
	if (target == GL_INVALID_ENUM)
		return false;

	GLenum internalFormat = EGL3Types::asTextureInternalFormat(textureDesc.getTexFormat());
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
	GLsizei depth = (GLsizei)textureDesc.getDepth();

	GLsizei mipBase = textureDesc.getMipBase();
	GLsizei mipLevel = std::max((GLsizei)textureDesc.getMipLevel(), 1);

	if (!applySamplerWrap(target, textureDesc.getSamplerWrap()))
		return false;

	if (!applySamplerFilter(target, textureDesc.getSamplerFilter()))
		return false;

	if (!applySamplerAnis(target, textureDesc.getSamplerAnis()))
		return false;

	if (!applyMipmapLimit(target, mipBase, mipLevel))
		return false;

	if (target == GL_TEXTURE_2D)
		GL_CHECK(glTexStorage2D(target, mipLevel, internalFormat, w, h));
	else if (target == GL_TEXTURE_2D_MULTISAMPLE)
		GL_CHECK(glTexStorage2DMultisample(target, mipLevel, internalFormat, w, h, GL_FALSE));
	else if (target == GL_TEXTURE_2D_ARRAY)
		GL_CHECK(glTexStorage3D(target, mipLevel, internalFormat, w, h, depth));
	else if (target == GL_TEXTURE_3D)
		GL_CHECK(glTexStorage3D(target, mipLevel, internalFormat, w, h, depth));
	else if (target == GL_TEXTURE_CUBE_MAP)
		GL_CHECK(glTexStorage2D(target, mipLevel, internalFormat, w, h));

	auto stream = textureDesc.getStream();
	if (stream)
	{
		if (EGL3Types::isCompressedTexture(textureDesc.getTexFormat()))
		{
			GLsizei offset = 0;
			GLsizei blockSize = internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;

			GLint oldPackStore = 1;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 8);

			for (GLint mip = mipBase; mip < mipBase + mipLevel; mip++)
			{
				GLsizei mipSize = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;

				glCompressedTexSubImage2D(target, mip, 0, 0, w, h, internalFormat, mipSize, (char*)stream + offset);

				w = std::max(w >> 1, 1);
				h = std::max(h >> 1, 1);

				offset += mipSize;
			}

			glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
		}
		else
		{
			GLenum format = EGL3Types::asTextureFormat(textureDesc.getTexFormat());
			GLenum type = EGL3Types::asTextureType(textureDesc.getTexFormat());

			GLsizei offset = 0;
			GLsizei pixelSize = EGL3Types::getFormatNum(format);

			GLenum cubeFace[] =
			{
				GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
				GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
				GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
			};

			GLint oldPackStore = 1;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);
			glPixelStorei(GL_UNPACK_ALIGNMENT, pixelSize == 3 ? 1 : pixelSize);

			for (GLsizei mip = mipBase; mip < mipBase + mipLevel; mip++)
			{
				GLsizei mipSize = w * h * pixelSize;
				GLsizei layerBase = textureDesc.getLayerBase() + 1;
				GLsizei layerLevel = textureDesc.getLayerNums();

				for (GLsizei layer = layerBase; layer < layerBase + layerLevel; layer++)
				{
					if (target == GL_TEXTURE_2D || target == GL_TEXTURE_2D_MULTISAMPLE)
					{
						GL_CHECK(glTexSubImage2D(target, mip, 0, 0, w, h, format, type, (char*)stream + offset));
						offset += mipSize;
					}
					else
					{
						if (target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_CUBE_MAP_ARRAY_EXT)
						{
							for (std::size_t i = 0; i < 6; i++)
							{
								if (target == GL_TEXTURE_CUBE_MAP)
									GL_CHECK(glTexSubImage2D(cubeFace[i], mip, 0, 0, w, h, format, type, (char*)stream + offset));
								else
									GL_CHECK(glTexSubImage3D(cubeFace[i], mip, 0, 0, 0, w, h, layer, format, type, (char*)stream + offset));
								offset += mipSize;
							}
						}
						else
						{
							GL_CHECK(glTexSubImage3D(target, mip, 0, 0, 0, w, h, depth * layer, format, type, (char*)stream + offset));
							offset += mipSize * depth;
						}
					}

					w = std::max(w >> 1, 1);
					h = std::max(h >> 1, 1);
				}
			}

			glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
		}
	}

	GL_CHECK(glBindTexture(target, GL_NONE));

	_target = target;
	_textureDesc = textureDesc;

	return EGL3Check::checkError();
}

void
EGL3Texture::close() noexcept
{
	if (_texture != GL_NONE)
	{
		glDeleteTextures(1, &_texture);
		_texture = GL_NONE;
	}
}

GLenum
EGL3Texture::getTarget() const noexcept
{
	return _target;
}

GLuint
EGL3Texture::getInstanceID() const noexcept
{
	return _texture;
}

bool
EGL3Texture::applyMipmapLimit(GLenum target, std::uint32_t min, std::uint32_t count) noexcept
{
	GL_CHECK(glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, min));
	GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, min + count - 1));
	return true;
}

bool
EGL3Texture::applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept
{
	GLenum glwrap = EGL3Types::asSamplerWrap(wrap);
	if (glwrap != GL_INVALID_ENUM)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_S, glwrap));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_T, glwrap));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_R, glwrap));

		return true;
	}

	return false;
}

bool
EGL3Texture::applySamplerFilter(GLenum target, GraphicsSamplerFilter filter) noexcept
{
	GLenum min = EGL3Types::asSamplerMinFilter(filter);
	GLenum mag = EGL3Types::asSamplerMagFilter(filter);

	if (min != GL_INVALID_ENUM && mag != GL_INVALID_ENUM)
	{
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min));
		GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag));
		return true;
	}

	return false;
}

bool
EGL3Texture::applySamplerAnis(GLenum target, GraphicsSamplerAnis anis) noexcept
{
	if (EGL3Types::isSupportFeature(EGL3Features::EGL3_EXT_texture_filter_anisotropic))
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
EGL3Texture::getGraphicsTextureDesc() const noexcept
{
	return _textureDesc;
}

void
EGL3Texture::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3Texture::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END