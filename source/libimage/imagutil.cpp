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
#include <ray/imagutil.h>
#include <ray/math.h>

_NAME_BEGIN

namespace image
{
	void r32f_to_r8uint(const float* src, std::uint8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel)
	{
		assert(src && dst);
		assert(w > 0 && h > 0 && channel > 0 && channel <= 4);

		for (std::size_t i = 0; i < w * h * channel; i++)
		{
			dst[i] = (std::uint8_t)math::clamp<std::uint16_t>((std::uint16_t)(src[i] * 255), 0, 255);
		}
	}

	void r32f_to_r8sint(const float* src, std::int8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel)
	{
		assert(src && dst);
		assert(w > 0 && h > 0 && channel > 0 && channel <= 4);

		for (std::size_t i = 0; i < w * h * channel; i++)
		{
			dst[i] = (std::uint8_t)math::clamp<std::uint16_t>((std::uint16_t)(src[i] * 127), -127, 127);
		}
	}

	void r64f_to_r8uint(const double* src, std::uint8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel)
	{
		assert(src && dst);
		assert(w > 0 && h > 0 && channel > 0 && channel <= 4);

		for (std::size_t i = 0; i < w * h * channel; i++)
		{
			dst[i] = (std::uint8_t)math::clamp<std::uint16_t>((std::uint16_t)(src[i] * 255), 0, 255);
		}
	}

	void r64f_to_r8sint(const double* src, std::int8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel)
	{
		assert(src && dst);
		assert(w > 0 && h > 0 && channel > 0 && channel <= 4);

		for (std::size_t i = 0; i < w * h * channel; i++)
		{
			dst[i] = (std::uint8_t)math::clamp<std::uint16_t>((std::uint16_t)(src[i] * 127), -127, 127);
		}
	}

	void rgb32f_to_rgbt8(const float* src, std::uint8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel)
	{
		assert(src && dst);
		assert(w > 0 && h > 0 && channel > 0 && channel <= 4);

		for (std::uint32_t i = 0; i < h; i++)
		{
			for (std::uint32_t j = 0; j < w; j++)
			{
				float r = src[i * w * channel + j * channel + 0];
				float g = src[i * w * channel + j * channel + 1];
				float b = src[i * w * channel + j * channel + 2];

				image::RGBTEncode(r, g, b, &dst[i * w * 4 + j * 4], 1024.0f);
			}
		}
	}

	void rgb64f_to_rgbt8(const double* src, std::uint8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel)
	{
		assert(src && dst);
		assert(w > 0 && h > 0 && channel > 0 && channel <= 4);

		for (std::uint32_t i = 0; i < h; i++)
		{
			for (std::uint32_t j = 0; j < w; j++)
			{
				double r = src[i * w * channel + j * channel + 0];
				double g = src[i * w * channel + j * channel + 1];
				double b = src[i * w * channel + j * channel + 2];

				image::RGBTEncode(r, g, b, &dst[i * w * 4 + j * 4], 1024.0);
			}
		}
	}

	void rgb32f_to_rgb8uint(const Image& srcImage, Image& dstImage)
	{
		assert(srcImage.format() == image::format_t::R32G32B32SFloat);
		assert(dstImage.format() == image::format_t::R8G8B8UInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (float3*)srcImage.data();
		auto dst = (Vector3t<std::uint8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		constexpr std::uint8_t minLimit = std::numeric_limits<std::uint8_t>::min();
		constexpr std::uint8_t maxLimit = std::numeric_limits<std::uint8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::uint8_t>(Vector3t<std::uint8_t>(src[i] * 255.0f), minLimit, maxLimit);
		}
	}

	void rgb64f_to_rgb8uint(const Image& srcImage, Image& dstImage)
	{
		assert(srcImage.format() == image::format_t::R64G64B64SFloat);
		assert(dstImage.format() == image::format_t::R8G8B8UInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (double3*)srcImage.data();
		auto dst = (Vector3t<std::uint8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		constexpr std::uint8_t minLimit = std::numeric_limits<std::uint8_t>::min();
		constexpr std::uint8_t maxLimit = std::numeric_limits<std::uint8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::uint8_t>(Vector3t<std::uint8_t>(src[i] * 255.0), minLimit, maxLimit);
		}
	}

	void rgba32f_to_rgba8uint(const Image& srcImage, Image& dstImage)
	{
		assert(srcImage.format() == image::format_t::R32G32B32A32SFloat);
		assert(dstImage.format() == image::format_t::R8G8B8A8UInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (float4*)srcImage.data();
		auto dst = (Vector4t<std::uint8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		constexpr std::uint8_t minLimit = std::numeric_limits<std::uint8_t>::min();
		constexpr std::uint8_t maxLimit = std::numeric_limits<std::uint8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::uint8_t>(Vector4t<std::uint8_t>(src[i] * 255.0f), minLimit, maxLimit);
		}
	}

	void rgba64f_to_rgba8uint(const Image& srcImage, Image& dstImage)
	{
		assert(srcImage.format() == image::format_t::R64G64B64A64SFloat);
		assert(dstImage.format() == image::format_t::R8G8B8A8UInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (double4*)srcImage.data();
		auto dst = (Vector4t<std::uint8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		constexpr std::int8_t minLimit = std::numeric_limits<std::int8_t>::min();
		constexpr std::int8_t maxLimit = std::numeric_limits<std::int8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::uint8_t>(Vector4t<std::uint8_t>(src[i] * 255.0), minLimit, maxLimit);
		}
	}

	void rgb32f_to_rgb8sint(const Image& srcImage, Image& dstImage)
	{
		assert(srcImage.format() == image::format_t::R64G64B64A64SFloat);
		assert(dstImage.format() == image::format_t::R8G8B8A8SInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (float3*)srcImage.data();
		auto dst = (Vector3t<std::int8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		std::int8_t minLimit = std::numeric_limits<std::int8_t>::min();
		std::int8_t maxLimit = std::numeric_limits<std::int8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::int8_t>(Vector3t<std::int8_t>(src[i] * 127.0f), minLimit, maxLimit);
		}
	}

	void rgb64f_to_rgb8sint(const Image& srcImage, Image& dstImage)
	{
		assert(srcImage.format() == image::format_t::R64G64B64A64SFloat);
		assert(dstImage.format() == image::format_t::R8G8B8A8SInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (double3*)srcImage.data();
		auto dst = (Vector3t<std::int8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		std::int8_t minLimit = std::numeric_limits<std::int8_t>::min();
		std::int8_t maxLimit = std::numeric_limits<std::int8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::int8_t>(Vector3t<std::int8_t>(src[i] * 127.0), minLimit, maxLimit);
		}
	}

	void rgba32f_to_rgba8sint(const Image& srcImage, Image& dstImage)
	{
		assert(srcImage.format() == image::format_t::R64G64B64A64SFloat);
		assert(dstImage.format() == image::format_t::R8G8B8A8SInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (float4*)srcImage.data();
		auto dst = (Vector4t<std::int8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		std::int8_t minLimit = std::numeric_limits<std::int8_t>::min();
		std::int8_t maxLimit = std::numeric_limits<std::int8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::int8_t>(Vector4t<std::int8_t>(src[i] * 127.0f), minLimit, maxLimit);
		}
	}

	void rgba64f_to_rgba8sint(const Image& srcImage, Image& dstImage)
	{
		assert(srcImage.format() == image::format_t::R64G64B64A64SFloat);
		assert(dstImage.format() == image::format_t::R8G8B8A8SInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (double4*)srcImage.data();
		auto dst = (Vector4t<std::int8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		std::int8_t minLimit = std::numeric_limits<std::int8_t>::min();
		std::int8_t maxLimit = std::numeric_limits<std::int8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::int8_t>(Vector4t<std::int8_t>(src[i] * 127.0), minLimit, maxLimit);
		}
	}

	void dilateFilter(const float* image, float* outImage, std::uint32_t w, std::uint32_t h, std::uint8_t c) noexcept
	{
		assert(image && outImage);
		assert(w > 0 && h > 0 && c > 0 && c <= 4);

		for (std::uint32_t y = 0; y < h; y++)
		{
			for (std::uint32_t x = 0; x < w; x++)
			{
				float color[4];
				bool valid = false;
				for (std::uint32_t i = 0; i < c; i++)
				{
					color[i] = image[(y * w + x) * c + i];
					valid |= color[i] > 0.0f;
				}
				if (!valid)
				{
					int n = 0;
					const int dx[] = { -1, 0, 1,  0 };
					const int dy[] = { 0, 1, 0, -1 };
					for (std::uint32_t d = 0; d < 4; d++)
					{
						std::int32_t cx = x + dx[d];
						std::int32_t cy = y + dy[d];
						if (cx >= 0 && cx < w && cy >= 0 && cy < h)
						{
							float dcolor[4];
							bool dvalid = false;
							for (std::uint32_t i = 0; i < c; i++)
							{
								dcolor[i] = image[(cy * w + cx) * c + i];
								dvalid |= dcolor[i] > 0.0f;
							}
							if (dvalid)
							{
								for (std::uint32_t i = 0; i < c; i++)
									color[i] += dcolor[i];
								n++;
							}
						}
					}
					if (n)
					{
						float in = 1.0f / n;
						for (std::uint32_t i = 0; i < c; i++)
							color[i] *= in;
					}
				}
				for (std::uint32_t i = 0; i < c; i++)
					outImage[(y * w + x) * c + i] = color[i];
			}
		}
	}

	void smoothFilter(const float* image, float* outImage, std::uint32_t w, std::uint32_t h, std::uint8_t c) noexcept
	{
		assert(image && outImage);
		assert(w > 0 && h > 0 && c > 0 && c <= 4);

		for (std::uint32_t y = 0; y < h; y++)
		{
			for (std::uint32_t x = 0; x < w; x++)
			{
				float color[4] = { 0 };
				int n = 0;

				for (std::int32_t dy = -1; dy <= 1; dy++)
				{
					std::int32_t cy = y + dy;
					for (std::int32_t dx = -1; dx <= 1; dx++)
					{
						std::int32_t cx = x + dx;
						if (cx >= 0 && cx < w && cy >= 0 && cy < h)
						{
							bool valid = false;
							for (std::uint32_t i = 0; i < c; i++)
								valid |= image[(cy * w + cx) * c + i] > 0.0f;
							if (valid)
							{
								for (std::uint32_t i = 0; i < c; i++)
									color[i] += image[(cy * w + cx) * c + i];
								n++;
							}
						}
					}
				}

				for (std::uint32_t i = 0; i < c; i++)
					outImage[(y * w + x) * c + i] = n ? color[i] / n : 0.0f;
			}
		}
	}
}

_NAME_END