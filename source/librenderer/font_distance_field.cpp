// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include <ray/font_distance_field.h>
#include <ray/mstream.h>
#include <ray/fstream.h>
#include <ray/ioserver.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftbitmap.h>
#include <freetype/ftstroke.h>

#include <thread>

_NAME_BEGIN

int getGrayBitmap(const FT_BitmapGlyph bitmapGlyph, int x, int y)
{
	if (x < 0 || y < 0) return 0;
	if (x >= (bitmapGlyph->bitmap.pitch << 3)) return 0;
	if (y >= bitmapGlyph->bitmap.rows) return 0;
	std::size_t offset = y * bitmapGlyph->bitmap.pitch + (x >> 3);
	std::uint8_t array = bitmapGlyph->bitmap.buffer[offset];
	std::uint8_t index = 7 - (x % 8);
	return (array >> index) & 0x1;
}

FontPointBitmap::FontPointBitmap() noexcept
	: _fontSize(32)
{
}

FontPointBitmap::~FontPointBitmap() noexcept
{
	this->clear();
}

void
FontPointBitmap::createFontMapping(StreamReader& stream, const std::wstring& charsets, std::size_t fontSize, std::size_t distanceSize, std::size_t numThreads)
{
	std::vector<std::unique_ptr<std::thread>> _threads;

	std::size_t streamSize = stream.size();
	if (streamSize == 0)
		return;

	std::vector<FT_Byte> filebase(streamSize);
	stream.read((char*)filebase.data(), filebase.size());

	_faces.resize(numThreads);
	_librarys.resize(numThreads);

	for (std::size_t i = 0; i < numThreads; i++)
	{
		FT_Library& library = (FT_Library&)_librarys[i];
		FT_Face& face = (FT_Face&)_faces[i];

		if (FT_Init_FreeType(&library)) { this->clear(); return; }
		if (FT_New_Memory_Face(library, filebase.data(), filebase.size(), 0, &face)) { this->clear(); return; }
		if (FT_Select_Charmap(face, FT_ENCODING_UNICODE)) { this->clear(); return; }
		if (FT_Set_Pixel_Sizes(face, fontSize, fontSize)) { this->clear(); return; }
	}

	_bitmapGlyphs.clear();

	for (auto& ch : charsets)
	{
		auto index = FT_Get_Char_Index((FT_Face)_faces[0], ch);
		if (index)
		{
			FontGlyph glyph;
			glyph.glyph = ch;
			_bitmapGlyphs.push_back(glyph);
		}
	}

	if (!_bitmapGlyphs.empty())
	{
		std::size_t power = 0;
		std::size_t bitmapSize = 0;
		std::size_t size;
		do
		{
			power++;
			bitmapSize = std::pow(2, power);
			size = bitmapSize / distanceSize;
		} while ((size * size) < charsets.size());

		_bitmap.resize(bitmapSize * bitmapSize);
		_bitmapSize = bitmapSize;

		_fontSize = fontSize;

		_distanceSize = distanceSize;

		std::size_t num = _bitmapGlyphs.size() / numThreads;
		std::size_t startCode = 0;
		std::size_t endCode = startCode + num + _bitmapGlyphs.size() % numThreads;
		std::size_t internalSize = ((FT_Face)_faces[0])->size->metrics.max_advance / 64;

		if (numThreads > 1)
		{
			for (std::size_t i = 0; i < numThreads; i++)
			{
				_threads.push_back(std::make_unique<std::thread>(
					std::bind(&FontPointBitmap::computeBitmaps, this,
						_librarys[i],
						_faces[i],
						internalSize,
						startCode,
						endCode
						)));

				startCode = endCode;
				endCode += num;

				if (num == 0)
					break;
			}

			for (auto& it : _threads)
			{
				if (it)
					it->join();
			}
		}
		else
		{
			this->computeBitmaps(_librarys[0], _faces[0], internalSize, 0, _bitmapGlyphs.size());
		}
	}

	if (!_faces.empty())
	{
		for (auto& it : _faces)
			FT_Done_Face((FT_Face)it);
		_faces.clear();
	}

	if (!_librarys.empty())
	{
		for (auto& it : _librarys)
			FT_Done_FreeType((FT_Library)it);
		_librarys.clear();
	}
}

void
FontPointBitmap::createFontMapping(const std::string& fontpath, const std::wstring& charsets, std::size_t fontSize, std::size_t distanceSize, std::size_t numThreads)
{
	StreamReaderPtr stream;
	if (IoServer::instance()->openFile(stream, fontpath, ios_base::in))
		this->createFontMapping(*stream, charsets, fontSize, distanceSize, numThreads);
}

void
FontPointBitmap::computeBitmaps(FT_Library library, FT_Face face, std::size_t internalSize, std::size_t startCode, std::size_t endCode)
{
	assert(library && face);
	assert(startCode <= _bitmapGlyphs.size());
	assert(endCode <= _bitmapGlyphs.size());

	for (std::size_t i = startCode; i < endCode; i++)
	{
		FT_Glyph glyph;
		FT_UInt index = FT_Get_Char_Index(face, _bitmapGlyphs[i].glyph);

		FT_Error error;
		error = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT); assert(!error);
		error = FT_Get_Glyph(face->glyph, &glyph); assert(!error);
		error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_MONO, 0, false); assert(!error);

		FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;

		if (bitmapGlyph->bitmap.buffer)
		{
			std::size_t height = bitmapGlyph->bitmap.rows;
			std::size_t width = bitmapGlyph->bitmap.pitch << 3;

			std::size_t offsetX = bitmapGlyph->left;
			std::size_t offsetY = _fontSize - bitmapGlyph->top;

			_bitmapGlyphs[i].advanceX = glyph->advance.x;
			_bitmapGlyphs[i].advanceY = glyph->advance.y;
			_bitmapGlyphs[i].left = bitmapGlyph->left;
			_bitmapGlyphs[i].top = bitmapGlyph->top;
			_bitmapGlyphs[i].width = bitmapGlyph->bitmap.pitch << 3;
			_bitmapGlyphs[i].height = bitmapGlyph->bitmap.rows;
			_bitmapGlyphs[i].offsetX = offsetX;
			_bitmapGlyphs[i].offsetY = offsetY;

			for (std::size_t y = 0; y < height; y++)
			{
				for (std::size_t x = 0; x < width; x++)
				{
					auto center = getGrayBitmap(bitmapGlyph, x, y);
					_bitmap[_bitmapSize * (y + offsetY * _distanceSize) + offsetX * _distanceSize + x] = center;
				}
			}
		}

		FT_Done_Glyph(glyph);
	}
}

void
FontPointBitmap::clear() noexcept
{
	_bitmap.clear();

	if (!_faces.empty())
	{
		for (auto& it : _faces)
			FT_Done_Face((FT_Face)it);
		_faces.clear();
	}

	if (!_librarys.empty())
	{
		for (auto& it : _librarys)
			FT_Done_FreeType((FT_Library)it);
		_librarys.clear();
	}
}

const FontGlyphs&
FontPointBitmap::getBitmapGlyphs() const noexcept
{
	return _bitmapGlyphs;
}

const FontBitmaps&
FontPointBitmap::getBitmapData() const noexcept
{
	return _bitmap;
}

std::size_t
FontPointBitmap::getBitmapSize() const noexcept
{
	return _bitmapSize;
}

std::size_t
FontPointBitmap::getFontSize() const noexcept
{
	return _fontSize;
}

bool
FontPointBitmap::load(StreamReader& stream)
{
	DFFINFOHEADER header;
	if (stream.read((char*)&header, sizeof(header)))
	{
		if (header.magic[0] != 'D' ||
			header.magic[1] != 'F' ||
			header.magic[2] != 'F')
		{
			return false;
		}

		_bitmapSize = header.sizeOfBitmap;
		_bitmap.resize(_bitmapSize * _bitmapSize);
		_bitmapGlyphs.resize(header.sizeOfGlyph);

		if (!stream.read((char*)_bitmapGlyphs.data(), sizeof(FontGlyph) * _bitmapGlyphs.size()))
			return false;

		if (!stream.read((char*)_bitmap.data(), _bitmapSize * _bitmapSize))
			return false;

		return true;
	}

	return false;
}

bool
FontPointBitmap::load(const std::string& filepath)
{
	StreamReaderPtr stream;
	if (IoServer::instance()->openFile(stream, filepath))
		return this->load(*stream);
	return false;
}

bool
FontPointBitmap::save(StreamWrite& stream)
{
	if (stream)
	{
		if (!_bitmapGlyphs.empty())
		{
			DFFINFOHEADER header;
			header.magic[0] = 'D';
			header.magic[1] = 'F';
			header.magic[2] = 'F';
			header.sizeOfGlyph = _bitmapGlyphs.size();
			header.sizeOfBitmap = _bitmapSize;

			stream.write((char*)&header, sizeof(header));
			stream.write((char*)_bitmapGlyphs.data(), sizeof(FontGlyph) * _bitmapGlyphs.size());
			stream.write((char*)_bitmap.data(), _bitmap.size());
			return true;
		}
	}

	return false;
}

bool
FontPointBitmap::save(const std::string& filepath)
{
	StreamWritePtr stream;
	if (IoServer::instance()->openFile(stream, filepath))
		return this->save(*stream);
	return false;
}

FontDistanceField::FontDistanceField() noexcept
{
}

FontDistanceField::~FontDistanceField() noexcept
{
}

void
FontDistanceField::computeBitmaps(FT_Library library, FT_Face face, std::size_t internalSize, std::size_t startCode, std::size_t endCode)
{
	assert(library && face);
	assert(startCode <= _bitmapGlyphs.size());
	assert(endCode <= _bitmapGlyphs.size());

	KdimensionTree<short2> nodes;

	std::vector<float> distance(_distanceSize * _distanceSize);

	for (std::size_t i = startCode; i < endCode; i++)
	{
		FT_Glyph glyph;
		FT_UInt index = FT_Get_Char_Index(face, _bitmapGlyphs[i].glyph);

		FT_Error error;
		error = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT); assert(!error);
		error = FT_Get_Glyph(face->glyph, &glyph); assert(!error);
		error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_MONO, 0, false); assert(!error);

		FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;

		if (bitmapGlyph->bitmap.buffer)
		{
			std::size_t offsetY = (i / (_bitmapSize / _distanceSize));
			std::size_t offsetX = (i - offsetY * _bitmapSize / _distanceSize);

			_bitmapGlyphs[i].advanceX = glyph->advance.x;
			_bitmapGlyphs[i].advanceY = glyph->advance.y;
			_bitmapGlyphs[i].left = bitmapGlyph->left / (float)internalSize;
			_bitmapGlyphs[i].top = (_fontSize - bitmapGlyph->top) / (float)internalSize;
			_bitmapGlyphs[i].width = (bitmapGlyph->bitmap.pitch << 3) / (float)internalSize;
			_bitmapGlyphs[i].height = bitmapGlyph->bitmap.rows / (float)internalSize;
			_bitmapGlyphs[i].offsetX = offsetX;
			_bitmapGlyphs[i].offsetY = offsetY;

			nodes.clear();
			std::memset(distance.data(), 0, distance.size());

			computeEdge(bitmapGlyph, _fontSize, internalSize, nodes);
			computeDistance(bitmapGlyph, nodes, _fontSize, internalSize, _distanceSize, distance);
			computeDistanceField(distance, _bitmap, _fontSize, _bitmapSize, _distanceSize, offsetX, offsetY);
		}

		FT_Done_Glyph(glyph);
	}
}

void
FontDistanceField::computeEdge(const FT_BitmapGlyph bitmapGlyph, std::size_t fontSize, std::size_t internalSize, KdimensionTree<short2>& nodes)
{
	std::size_t height = bitmapGlyph->bitmap.rows;
	std::size_t width = bitmapGlyph->bitmap.pitch << 3;

	std::size_t offsetX = bitmapGlyph->left;
	std::size_t offsetY = fontSize - bitmapGlyph->top;

	for (std::size_t y = 0; y < height; y++)
	{
		for (std::size_t x = 0; x < width; x++)
		{
			auto center = getGrayBitmap(bitmapGlyph, x, y);
			auto left = getGrayBitmap(bitmapGlyph, x + 1, y);
			auto right = getGrayBitmap(bitmapGlyph, x - 1, y);
			auto up = getGrayBitmap(bitmapGlyph, x, y + 1);
			auto down = getGrayBitmap(bitmapGlyph, x, y - 1);

			if ((center & (center ^ (center & up & down & left & right))))
			{
				nodes.insert(short2(x + offsetX, y + offsetY));
			}
		}
	}
}

void
FontDistanceField::computeDistance(const FT_BitmapGlyph bitmapGlyph, KdimensionTree<short2>& nodes, std::size_t fontSize, std::size_t internalSize, std::size_t distanceSize, std::vector<float>& distance)
{
	std::size_t _sampleSize = internalSize / distanceSize;
	std::size_t _sampleHalf = _sampleSize >> 1;

	std::size_t offsetX = bitmapGlyph->left;
	std::size_t offsetY = (fontSize - bitmapGlyph->top);

	for (std::size_t y = 0; y < distanceSize; y++)
	{
		for (std::size_t x = 0; x < distanceSize; x++)
		{
			std::size_t ix = _sampleHalf + x * _sampleSize;
			std::size_t iy = _sampleHalf + y * _sampleSize;

			KdimensionNearest<short2> result;
			nodes.search(result, short2(ix, iy), fontSize);

			std::size_t index = y * distanceSize + x;

			std::size_t dx = ix - offsetX;
			std::size_t dy = iy - offsetY;

			if (getGrayBitmap(bitmapGlyph, dx, dy))
				distance[index] = sqrt(result.getDistanceSqrt()) + EPSILON;
			else
				distance[index] = -sqrt(result.getDistanceSqrt());
		}
	}
}

void
FontDistanceField::computeDistanceField(const std::vector<float>& distanceArray, std::vector<std::uint8_t>& bitmap, std::size_t fontSize, std::size_t bitmapSize, std::size_t distanceSize, std::size_t offsetX, std::size_t offsetY)
{
	float total = 0;
	for (auto& distance : distanceArray)
	{
		if (distance > 0)
			total += distance;
		else
			total += -distance;
	}

	float invSqr = 1.0 / sqrt(total);

	for (std::size_t y = 0; y < distanceSize; y++)
	{
		for (std::size_t x = 0; x < distanceSize; x++)
		{
			std::size_t index = distanceSize * y + x;

			float value = distanceArray[index];

			if (value >= 0)
				value = 255;
			else if (value == 0)
				value = 0;
			else
				value = math::lerp(255.0f, 0.0f, math::clamp(-value * invSqr, 0.0f, 1.0f));

			bitmap[bitmapSize * (y + offsetY * distanceSize) + offsetX * distanceSize + x] = value;
		}
	}
}

_NAME_END