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
	Image() noexcept;
	Image(Image&& move) noexcept;
	~Image() noexcept;

	bool create(std::uint32_t width, std::uint32_t height, ImageFormat format, bool clear = true) noexcept;
	bool create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, ImageFormat format, bool clear = true) noexcept;
	bool create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, ImageFormat format, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase = 0, std::uint32_t layerBase = 0, bool clear = true) noexcept;
	bool create(const Image& src, ImageFormat format = ImageFormat::Undefined) noexcept;

	void destroy() noexcept;
	bool empty() const noexcept;

	ImageFormat format() const noexcept;

	std::uint32_t channel() const noexcept;

	std::uint32_t width() const noexcept;
	std::uint32_t height() const noexcept;
	std::uint32_t depth() const noexcept;

	std::uint32_t size() const noexcept;

	std::uint32_t mipBase() const noexcept;
	std::uint32_t mipLevel() const noexcept;

	std::uint32_t layerBase() const noexcept;
	std::uint32_t layerLevel() const noexcept;

	const char* data() const noexcept;

public:
	bool load(const std::string& filename, const char* type = "") noexcept;
	bool load(StreamReader& stream, const char* type = "") noexcept;

	bool save(const std::string& filename, const char* type = "tga") noexcept;
	bool save(StreamWrite& stream, const char* type = "tga") noexcept;

private:
	void _init() noexcept;

private:
	Image(const Image&) noexcept = delete;
	Image& operator=(const Image&) noexcept = delete;

private:
	ImageFormat _format;

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

ImageType type(ImageFormat format) noexcept;
ImageType type(const Image& image) noexcept;

ImageOrder order(ImageFormat format) noexcept;
ImageOrder order(const Image& image) noexcept;

template<typename _Tx, typename _Ty>
std::enable_if_t<std::is_floating_point<_Ty>::value, void> RGBTEncode(_Tx r, _Tx g, _Tx b, _Ty encode[4], _Tx range = 1024) noexcept
{
	_Tx max = 0;
	max = std::max(std::max(r, g), std::max(b, 1e-6f));
	max = std::min(max, range);

	_Tx alpha = (range + 1) / range *  max / (1 + max);
	alpha = std::ceil(alpha * 255.0f) / 255.0f;

	_Tx rcp = 1.0f / (alpha / (1.0f + 1.0f / range - alpha));

	encode[0] = r * rcp;
	encode[1] = g * rcp;
	encode[2] = b * rcp;
	encode[3] = alpha;
}

template<typename _Tx, typename _Ty>
std::enable_if_t<std::is_unsigned<_Ty>::value, void> RGBTEncode(_Tx r, _Tx g, _Tx b, _Ty encode[4], _Tx range = 1024) noexcept
{
	_Tx max = 0;
	max = std::max(std::max(r, g), std::max(b, 1e-6f));
	max = std::min(max, range);

	_Tx a = (range + 1) / range *  max / (1 + max);
	a = std::ceil(a * 255.0f) / 255.0f;

	_Tx rcp = 1.0f / (a / (1.0f + 1.0f / range - a));

	encode[0] = math::clamp<_Ty>(r * rcp * std::numeric_limits<_Ty>::max(), 0, std::numeric_limits<_Ty>::max());
	encode[1] = math::clamp<_Ty>(g * rcp * std::numeric_limits<_Ty>::max(), 0, std::numeric_limits<_Ty>::max());
	encode[2] = math::clamp<_Ty>(b * rcp * std::numeric_limits<_Ty>::max(), 0, std::numeric_limits<_Ty>::max());
	encode[3] = math::clamp<_Ty>(a * std::numeric_limits<_Ty>::max(), 0, std::numeric_limits<_Ty>::max());
}

}

_NAME_END

#endif