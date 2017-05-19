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
#ifndef _H_IMAGE_UTIL_H_
#define _H_IMAGE_UTIL_H_

#include <ray/image.h>

_NAME_BEGIN

namespace image
{
	EXPORT void r32f_to_r8uint(const float* src, std::uint8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel);
	EXPORT void r32f_to_r8sint(const float* src, std::int8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel);

	EXPORT void r64f_to_r8uint(const double* src, std::uint8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel);
	EXPORT void r64f_to_r8sint(const double* src, std::int8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel);

	EXPORT void rgb32f_to_rgbt8(const float* src, std::uint8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel);
	EXPORT void rgb64f_to_rgbt8(const double* src, std::uint8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel);

	EXPORT void rgb32f_to_rgb8uint(const Image& src, Image& dst);
	EXPORT void rgb64f_to_rgb8uint(const Image& src, Image& dst);
	EXPORT void rgba32f_to_rgba8uint(const Image& src, Image& dst);
	EXPORT void rgba64f_to_rgba8uint(const Image& src, Image& dst);

	EXPORT void rgb32f_to_rgb8sint(const Image& src, Image& dst);
	EXPORT void rgb64f_to_rgb8sint(const Image& src, Image& dst);
	EXPORT void rgba32f_to_rgba8sint(const Image& src, Image& dst);
	EXPORT void rgba64f_to_rgba8sint(const Image& src, Image& dst);

	template<typename _Tx, typename size_t = std::uint32_t, typename channel_t = std::uint8_t>
	void flipHorizontal(_Tx* data, size_t w, size_t h, channel_t channel)
	{
		for (size_t j = 0; j < h / 2; j++)
			for (size_t i = 0; i < w * channel; i++)
				std::swap(data[i + j * (w * channel)], data[(h - j - 1) * (w * channel) + i]);
	}

	template<typename _Tx, typename _Ty, typename = std::enable_if_t<std::is_floating_point<_Tx>::value>>
	std::enable_if_t<std::is_floating_point<_Ty>::value, void> 
	RGBT_encode(_Tx r, _Tx g, _Tx b, _Ty encode[4], _Tx range = 1024) noexcept
	{
		static_assert(std::numeric_limits<_Ty>::max() <= std::numeric_limits<_Tx>::max());

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

	template<typename _Tx, typename _Ty, typename = std::enable_if_t<std::is_floating_point<_Tx>::value>>
	std::enable_if_t<std::is_signed<_Ty>::value, void>
	RGBT_encode(_Tx r, _Tx g, _Tx b, _Ty encode[4], _Tx range = 1024) noexcept
	{
		static_assert(std::numeric_limits<_Ty>::max() <= std::numeric_limits<_Tx>::max());

		_Tx max = 0;
		max = std::max(std::max(r, g), std::max(b, 1e-6f));
		max = std::min(max, range);

		_Tx a = (range + 1) / range *  max / (1 + max);
		a = std::ceil(a * 255.0f) / 255.0f;

		_Tx rcp = 1.0f / (a / (1.0f + 1.0f / range - a));

		_Tx minLimits = std::numeric_limits<_Ty>::min();
		_Tx maxLimits = std::numeric_limits<_Ty>::max();
		_Tx midLimits = (maxLimits + minLimits) >> 1;

		encode[0] = (_Ty)std::clamp<_Tx>(r * rcp * midLimits, minLimits, maxLimits);
		encode[1] = (_Ty)std::clamp<_Tx>(g * rcp * midLimits, minLimits, maxLimits);
		encode[2] = (_Ty)std::clamp<_Tx>(b * rcp * midLimits, minLimits, maxLimits);
		encode[3] = (_Ty)std::clamp<_Tx>(a * midLimits, minLimits, maxLimits);
	}

	template<typename _Tx, typename _Ty, typename = std::enable_if_t<std::is_floating_point<_Tx>::value>>
	inline std::enable_if_t<std::is_unsigned<_Ty>::value, void>
	RGBT_encode(_Tx r, _Tx g, _Tx b, _Ty encode[4], _Tx range = 1024) noexcept
	{
		static_assert(std::numeric_limits<_Ty>::max() <= std::numeric_limits<_Tx>::max());

		_Tx max = 0;
		max = std::max(std::max(r, g), std::max(b, _Tx(1e-6f)));
		max = std::min(max, range);

		_Tx a = (range + 1) / range *  max / (1 + max);
		a = std::ceil(a * 255.0f) / 255.0f;

		_Tx rcp = 1.0f / (a / (1.0f + 1.0f / range - a));

		_Tx minLimits = std::numeric_limits<_Ty>::min();
		_Tx maxLimits = std::numeric_limits<_Ty>::max();

		encode[0] = (_Ty)std::clamp<_Tx>(r * rcp * maxLimits, minLimits, maxLimits);
		encode[1] = (_Ty)std::clamp<_Tx>(g * rcp * maxLimits, minLimits, maxLimits);
		encode[2] = (_Ty)std::clamp<_Tx>(b * rcp * maxLimits, minLimits, maxLimits);
		encode[3] = (_Ty)std::clamp<_Tx>(a * maxLimits, minLimits, maxLimits);
	}

	template<typename _Tx, typename _Ty = std::uint8_t, typename = std::enable_if_t<std::is_floating_point<_Tx>::value>>
	inline std::enable_if_t<std::is_unsigned<_Ty>::value, void> 
	RGBE_encode(_Tx red, _Tx green, _Tx blue, _Ty rgbe[4])
	{
		_Tx v;
		int e;

		v = red;
		if (green > v) v = green;
		if (blue > v) v = blue;

		if (v < 1e-32)
		{
			rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
		}
		else
		{
			v = (_Tx)std::frexp(v, &e) * _Tx(256.0) / v;
			rgbe[0] = (_Ty)(red * v);
			rgbe[1] = (_Ty)(green * v);
			rgbe[2] = (_Ty)(blue * v);
			rgbe[3] = (_Ty)(e + 128);
		}
	}

	template<typename _Tx, typename _Ty = std::uint8_t, typename = std::enable_if_t<std::is_floating_point<_Tx>::value>>
	inline std::enable_if_t<std::is_unsigned<_Ty>::value, void> 
	RGBE_decode(_Ty rgbe[4], _Tx* red, _Tx* green, _Tx* blue)
	{
		_Tx f;

		if (rgbe[3])
		{
			f = (_Tx)std::ldexp(1.0, rgbe[3] - (int)(128 + 8));
			*red = rgbe[0] * f;
			*green = rgbe[1] * f;
			*blue = rgbe[2] * f;
		}
		else
		{
			*red = *green = *blue = _Tx(0.0f);
		}
	}

	EXPORT void smoothFilter(const float* src, float* dst, std::uint32_t w, std::uint32_t h, std::uint8_t c) noexcept;
	EXPORT void dilateFilter(const float* src, float* dst, std::uint32_t w, std::uint32_t h, std::uint8_t c) noexcept;
}

_NAME_END

#endif