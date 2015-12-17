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
#ifndef _H_FONT_DISTANCE_FIELD_H_
#define _H_FONT_DISTANCE_FIELD_H_

#include <ray/font_bitmap.h>
#include <ray/kdtree.h>

struct FT_BitmapGlyphRec_;
struct FT_LibraryRec_;
struct FT_FaceRec_;

typedef FT_BitmapGlyphRec_* FT_BitmapGlyph;
typedef FT_LibraryRec_* FT_Library;
typedef FT_FaceRec_* FT_Face;

_NAME_BEGIN

struct DFFINFOHEADER
{
	std::uint8_t magic[3];
	std::uint32_t sizeOfGlyph;
	std::uint32_t sizeOfBitmap;
};

class EXPORT FontPointBitmap : public FontBitmap
{
public:
	FontPointBitmap() noexcept;
	virtual ~FontPointBitmap() noexcept;

	virtual void createFontMapping(StreamReader& stream, const std::wstring& charsets, std::size_t fontSize, std::size_t distanceSize, std::size_t numThreads);
	virtual void createFontMapping(const std::string& fontpath, const std::wstring& charsets, std::size_t fontSize, std::size_t distanceSize, std::size_t numThreads);

	virtual void clear() noexcept;

	virtual const FontGlyphs& getBitmapGlyphs() const noexcept;
	virtual const FontBitmaps& getBitmapData() const noexcept;
	virtual std::size_t getBitmapSize() const noexcept;

	virtual std::size_t getFontSize() const noexcept;

	virtual bool load(StreamReader& stream);
	virtual bool save(StreamWrite& stream);

	virtual bool load(const std::string& filepath);
	virtual bool save(const std::string& filepath);

protected:
	virtual void computeBitmaps(FT_Library library, FT_Face face, std::size_t internalSize, std::size_t startCode, std::size_t endCode);

protected:

	std::vector<FT_Face> _faces;
	std::vector<FT_Library> _librarys;

	std::size_t _fontSize;
	std::size_t _distanceSize;

	std::size_t _bitmapSize;
	FontBitmaps _bitmap;
	FontGlyphs  _bitmapGlyphs;
};

class EXPORT FontDistanceField : public FontPointBitmap
{
public:
	FontDistanceField() noexcept;
	virtual ~FontDistanceField() noexcept;

private:
	virtual void computeBitmaps(FT_Library library, FT_Face face, std::size_t internalSize, std::size_t startCode, std::size_t endCode);

	static void computeEdge(const FT_BitmapGlyph bitmapGlyph, std::size_t fontSize, std::size_t internalSize, KdimensionTree<short2>& nodes);
	static void computeDistance(const FT_BitmapGlyph bitmapGlyph, KdimensionTree<short2>& nodes, std::size_t fontSize, std::size_t internalSize, std::size_t distanceSize, std::vector<float>& distance);
	static void computeDistanceField(const std::vector<float>& distanceArray, std::vector<std::uint8_t>& bitmap, std::size_t bitmapSize, std::size_t distanceSize, std::size_t offsetX, std::size_t offsetY);
};

_NAME_END

#endif