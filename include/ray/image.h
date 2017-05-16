// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
#ifndef _H_IMAGE_H_
#define _H_IMAGE_H_

#include <ray/imagtypes.h>
#include <ray/imaghandler.h>

_NAME_BEGIN

namespace image
{

class EXPORT Image final
{
public:
	using format_t = image::format_t;

public:
	Image() noexcept;
	Image(Image&& move) noexcept;
	~Image() noexcept;

	bool create(std::uint32_t width, std::uint32_t height, format_t format, bool clear = true) noexcept;
	bool create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, format_t format, bool clear = true) noexcept;
	bool create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, format_t format, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase = 0, std::uint32_t layerBase = 0, bool clear = true) noexcept;
	bool create(const Image& src, format_t format = format_t::Undefined) noexcept;

	void clear() noexcept;
	bool empty() const noexcept;

	format_t format() const noexcept;

	std::uint32_t width() const noexcept;
	std::uint32_t height() const noexcept;
	std::uint32_t depth() const noexcept;

	std::size_t size() const noexcept;

	std::uint32_t mipBase() const noexcept;
	std::uint32_t mipLevel() const noexcept;

	std::uint32_t layerBase() const noexcept;
	std::uint32_t layerLevel() const noexcept;

	const char* data() const noexcept;

public:
	bool load(const std::string& filename, const char* type = nullptr) noexcept;
	bool load(StreamReader& stream, const char* type = nullptr) noexcept;

	bool save(const std::string& filename, const char* type = "tga") noexcept;
	bool save(StreamWrite& stream, const char* type = "tga") noexcept;

private:
	void _init() noexcept;

private:
	Image(const Image&) noexcept = delete;
	Image& operator=(const Image&) noexcept = delete;

private:
	format_t _format;

	std::uint32_t _width;
	std::uint32_t _height;
	std::uint32_t _depth;

	std::uint32_t _mipBase;
	std::uint32_t _mipLevel;

	std::uint32_t _layerBase;
	std::uint32_t _layerLevel;

	std::size_t _size;

	std::unique_ptr<std::uint8_t[]> _data;
};

value_t value_type(image::format_t format) noexcept;
value_t value_type(const Image& image) noexcept;

swizzle_t swizzle_type(image::format_t format) noexcept;
swizzle_t swizzle_type(const Image& image) noexcept;

std::uint32_t channel(image::format_t format) noexcept;
std::uint32_t channel(const Image& image) noexcept;

std::uint32_t type_size(image::format_t format) noexcept;
std::uint32_t type_size(const Image& image) noexcept;

std::size_t data_size(std::uint32_t width, std::uint32_t height, std::uint32_t depth, image::format_t format, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase = 0, std::uint32_t layerBase = 0) noexcept;
std::size_t data_size(const Image& image) noexcept;

}

_NAME_END

#endif