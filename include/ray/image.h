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

#include <ray/imagbase.h>
#include <ray/imaghandler.h>
#include <ray/imagpal.h>

_NAME_BEGIN

class EXPORT Image final : public ImageBase
{
public:
    Image() noexcept;
    Image(size_type width, size_type height, bpp_type bpp, bool clear = false) noexcept;
    Image(size_type width, size_type height, bpp_type bpp, std::size_t dataSize, image_buf data, bool static_data = false, bool clear = false) noexcept;
    Image(StreamReader& stream, ImageType type = ImageType::ImageTypeUnknown) noexcept;
    ~Image() noexcept;

    bool create(size_type width, size_type height, bpp_type bpp, bool clear = false) noexcept;
	bool create(size_type width, size_type height, bpp_type bpp, std::size_t dataSize, image_buf data, bool staticData = false, bool clear = false) noexcept;
    bool create(const Image& src) noexcept;

    void destroy() noexcept;
    void clear(pass_val value = 0) noexcept { std::memset(_data, value, (std::size_t)this->size()); }

    size_type width()  const noexcept { return _width; }
    size_type height() const noexcept { return _height; }
    size_type size()   const noexcept { return _size; }
    image_buf data()   const noexcept { return _data; }
    image_buf bits()   const noexcept { return _data; }
    bpp_type  bpp()    const noexcept { return _bpp; }
    bool      empty()  const noexcept { return _data == nullptr; }

	void setImageType(ImageType type) noexcept;
	ImageType getImageType() const noexcept;

	// for dds
	void setMipLevel(std::uint8_t level) noexcept;
	std::uint8_t getMipLevel() const noexcept;

    // for gif
    delay_type getFrameDelay() const noexcept;
    void setFrameDelay(delay_type delay) const noexcept;

    // palette
    void setPalette(PaletteData* pal) noexcept;
    void setPalette(std::size_t n, RGB* pal) noexcept;
    void setPalette(std::size_t n, RGBA* pal) noexcept;
    void setPalette(std::size_t n, pass_val r, pass_val g, pass_val b, pass_val a = 0) noexcept;

    PaletteData* getPalette() noexcept;
    const PaletteData* getPalette() const noexcept;
    void getPalette(std::size_t n, RGB* rgb) noexcept;
    void getPalette(std::size_t n, RGBA* rgba) noexcept;
    void getPalette(std::size_t n, pass_val* r = nullptr, pass_val* g = nullptr, pass_val* b = nullptr, pass_val* a = nullptr) noexcept;

    // color
    void setColor(std::size_t n, const RGB& col)  noexcept { ((RGB&) this->rddata(n)) = col; }
    void setColor(std::size_t n, const RGBA& col) noexcept { ((RGBA&)this->rddata(n)) = col; }
    void setColor(std::size_t n, const BGR& col)  noexcept { ((BGR&) this->rddata(n)) = col; }
    void setColor(std::size_t n, const BGRA& col) noexcept { ((BGRA&)this->rddata(n)) = col; }
    void setColor(std::size_t n, pass_val r, pass_val g, pass_val b, pass_val a = 0) noexcept { this->setColor(n, RGBA(r, g, b, a)); }

    void getColor(std::size_t n, RGB& col)  noexcept { col = (RGB&) this->rddata(n); }
    void getColor(std::size_t n, RGBA& col) noexcept { col = (RGBA&)this->rddata(n); }
    void getColor(std::size_t n, BGR& col)  noexcept { col = (BGR&) this->rddata(n); }
    void getColor(std::size_t n, BGRA& col) noexcept { col = (BGRA&)this->rddata(n); }

    pass_val& rddata(std::size_t n) noexcept { return _data[(_bpp * n) >> 3]; }

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
    static void cmyk_to_rgb(image_buf rgb, const image_buf cmyk) noexcept;
    static void cmyk_to_rgba(image_buf rgba, const image_buf cmyk) noexcept;
    static void cmyk_to_rgb(RGB* texel, const image_buf cmyk) noexcept;
    static void cmyk_to_rgba(RGBA* texel, const image_buf cmyk) noexcept;

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

	bool _isStatic;
	bool _hasMask;

	size_type _width;
	size_type _height;
	std::size_t _size;

	image_buf _data;

	bpp_type _bpp;

	std::uint8_t _mipLevel;

    std::vector<ImageHandlerPtr> _handlers;
};

_NAME_END

#endif