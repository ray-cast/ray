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
#include <ray/imagpal.h>

_NAME_BEGIN

class EXPORT Image final
{
public:
	Image() noexcept;
	Image(std::uint32_t width, std::uint32_t height, std::uint16_t bpp, bool clear = false) noexcept;
	Image(std::uint32_t width, std::uint32_t height, std::uint16_t bpp, std::size_t dataSize, std::uint8_t* data, bool static_data = false, bool clear = false) noexcept;
	Image(StreamReader& stream, ImageType type = ImageType::ImageTypeUnknown) noexcept;
	~Image() noexcept;

	bool create(std::uint32_t width, std::uint32_t height, std::uint16_t bpp, bool clear = false) noexcept;
	bool create(std::uint32_t width, std::uint32_t height, std::uint16_t bpp, std::size_t dataSize, std::uint8_t* data, bool staticData = false, bool clear = false) noexcept;
	bool create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint16_t bpp, std::size_t dataSize, std::uint8_t* data, bool staticData = false, bool clear = false) noexcept;
	bool create(std::uint32_t width, std::uint32_t height, ImageType type, ImageFormat format, bool clear = false) noexcept;
	bool create(const Image& src) noexcept;

	void destroy() noexcept;
	void clear(std::uint8_t value = 0) noexcept { std::memset(_data, value, (std::size_t)this->size()); }

	std::uint32_t width()  const noexcept { return _width; }
	std::uint32_t height() const noexcept { return _height; }
	std::uint32_t depth()  const noexcept { return _depth; }
	std::uint32_t size()   const noexcept { return _size; }
	std::uint8_t* data()   const noexcept { return _data; }
	std::uint8_t* bits()   const noexcept { return _data; }
	std::uint16_t  bpp()    const noexcept { return _bpp; }
	bool      empty()  const noexcept { return _data == nullptr; }

	void setImageType(ImageType type) noexcept;
	ImageType getImageType() const noexcept;

	void setImageFormat(ImageFormat format) noexcept;
	ImageFormat getImageFormat() const noexcept;

	// for dds
	void setMipLevel(std::uint8_t level) noexcept;
	std::uint8_t getMipLevel() const noexcept;

	// for gif
	std::uint32_t getFrameDelay() const noexcept;
	void setFrameDelay(std::uint32_t delay) const noexcept;

	// palette
	void setPalette(PaletteData* pal) noexcept;
	void setPalette(std::size_t n, RGB* pal) noexcept;
	void setPalette(std::size_t n, RGBA* pal) noexcept;
	void setPalette(std::size_t n, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 0) noexcept;

	PaletteData* getPalette() noexcept;
	const PaletteData* getPalette() const noexcept;
	void getPalette(std::size_t n, RGB* rgb) noexcept;
	void getPalette(std::size_t n, RGBA* rgba) noexcept;
	void getPalette(std::size_t n, std::uint8_t* r = nullptr, std::uint8_t* g = nullptr, std::uint8_t* b = nullptr, std::uint8_t* a = nullptr) noexcept;

	// color
	void setColor(std::size_t n, const RGB& col)  noexcept { ((RGB&) this->rddata(n)) = col; }
	void setColor(std::size_t n, const RGBA& col) noexcept { ((RGBA&)this->rddata(n)) = col; }
	void setColor(std::size_t n, const BGR& col)  noexcept { ((BGR&) this->rddata(n)) = col; }
	void setColor(std::size_t n, const BGRA& col) noexcept { ((BGRA&)this->rddata(n)) = col; }
	void setColor(std::size_t n, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 0) noexcept { this->setColor(n, RGBA(r, g, b, a)); }

	void getColor(std::size_t n, RGB& col)  noexcept { col = (RGB&) this->rddata(n); }
	void getColor(std::size_t n, RGBA& col) noexcept { col = (RGBA&)this->rddata(n); }
	void getColor(std::size_t n, BGR& col)  noexcept { col = (BGR&) this->rddata(n); }
	void getColor(std::size_t n, BGRA& col) noexcept { col = (BGRA&)this->rddata(n); }

	std::uint8_t& rddata(std::size_t n) noexcept { return _data[(_bpp * n) >> 3]; }

public:
	bool load(const std::string& filename, ImageType type = ImageType::ImageTypeUnknown) noexcept;
	bool load(StreamReader& stream, ImageType type = ImageType::ImageTypeUnknown) noexcept;
	bool save(StreamWrite& stream, ImageType type = ImageType::ImageTypePNG) noexcept;

	bool emptyHandler() const noexcept;
	bool add(ImageHandlerPtr handler) noexcept;
	bool remove(ImageHandlerPtr handler) noexcept;
	bool find(StreamReader& stream, ImageHandlerPtr& handler) const noexcept;
	bool find(StreamReader& stream, ImageType type, ImageHandlerPtr& handler) const noexcept;
	bool find(ImageType type, ImageHandlerPtr& handler) const noexcept;

public:
	static void cmyk_to_rgb(std::uint8_t* rgb, const std::uint8_t* cmyk) noexcept;
	static void cmyk_to_rgba(std::uint8_t* rgba, const std::uint8_t* cmyk) noexcept;
	static void cmyk_to_rgb(RGB* texel, const std::uint8_t* cmyk) noexcept;
	static void cmyk_to_rgba(RGBA* texel, const std::uint8_t* cmyk) noexcept;

	static void hsv_to_rgb(RGB& rgb, float h, float s, float v) noexcept;
	static void hsv_to_rgba(RGBA& rgb, float h, float s, float v) noexcept;

	static void flipImageVertical(char* data, std::size_t width, std::size_t height, std::size_t component) noexcept;

private:
	void _init() noexcept;

private:
	Image(const Image&) noexcept = delete;
	Image& operator=(const Image&) noexcept = delete;

private:
	ImageType _imageType;
	ImageFormat _imageFormat;

	bool _isStatic;

	std::uint32_t _width;
	std::uint32_t _height;
	std::uint32_t _depth;

	std::uint16_t _bpp;

	std::size_t _size;

	std::uint8_t* _data;
	std::uint8_t _mipLevel;

	std::vector<ImageHandlerPtr> _handlers;
};

_NAME_END

#endif