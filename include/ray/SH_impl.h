// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#ifndef _H_SH_IMPL_H_
#define _H_SH_IMPL_H_

#include <ray/vector4.h>

_NAME_BEGIN

template<typename T, std::uint8_t N>
class SH final
{
public:
	using value_t = T;
	using size_t = std::size_t;

	static constexpr float CosineA0 = M_PI;
	static constexpr float CosineA1 = M_TWO_PI / 3.0f;
	static constexpr float CosineA2 = M_PI / 4.0f;

	value_t coeff[N];

	SH() noexcept {}
	SH(value_t coeffs) noexcept { for (size_t i = 0; i < N; ++i) this->coeff[i] = coeffs; }

	template<typename S, typename = std::enable_if<std::is_pointer<S>::value>>
	explicit SH(S coeffs[N]) noexcept
	{
		for (size_t i = 0; i < N; n++)
		{
			assert(xyz[i] <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
			coeff[i] = coeffs[i];
		}
	}

	SH& operator+=(const SH& sh) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] += sh.coeff[i]; return *this; }
	SH& operator-=(const SH& sh) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] -= sh.coeff[i]; return *this; }
	SH& operator*=(const SH& sh) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] *= sh.coeff[i]; return *this; }
	SH& operator/=(const SH& sh) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] /= sh.coeff[i]; return *this; }

	SH& operator+=(const value_t& scale) noexcept 
	{
		for (std::uint8_t i = 0; i < N; ++i) 
			coeff[i] += scale;
		return *this; 
	}

	SH& operator-=(const value_t& scale) noexcept
	{
		for (std::uint8_t i = 0; i < N; ++i)
			coeff[i] -= scale;
		return *this;
	}

	SH& operator*=(const value_t& scale) noexcept
	{
		for (std::uint8_t i = 0; i < N; ++i)
			coeff[i] *= scale;
		return *this;
	}

	SH& operator/=(const value_t& scale) noexcept
	{
		for (std::uint8_t i = 0; i < N; ++i)
			coeff[i] /= scale;
		return *this;
	}

	value_t& operator[](size_t n) noexcept { assert(n < N); return coeff[n]; }

	const value_t& operator[](size_t n) const noexcept { assert(n < N); return coeff[n]; }

	void ConvolveWithCosineKernel() noexcept
	{
		coeff[0] *= CosineA0;

		for (size_t i = 1; i < N; ++i)
			if (i < 4)
				coeff[i] *= CosineA1;
			else if (i < 9)
				coeff[i] *= CosineA2;
	}
};

template<typename T, std::uint8_t N>
SH<T, N> operator+(const SH<T, N>& sh1, const SH<T, N>& scale) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] + sh2.coeff[i];
	return result;
}

template<typename T, std::uint8_t N>
SH<T, N> operator-(const SH<T, N>& sh1, const SH<T, N>& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] - sh2.coeff[i];
	return result;
}

template<typename T, std::uint8_t N>
SH<T, N> operator*(const SH<T, N>& sh1, const SH<T, N>& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] * sh2.coeff[i];
	return result;
}

template<typename T, std::uint8_t N>
SH<T, N> operator/(const SH<T, N>& sh1, const SH<T, N>& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] / sh2.coeff[i];
	return result;
}

template<typename T, std::uint8_t N>
SH<T, N> operator+(const SH<T, N>& sh1, const T& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] + sh2;
	return result;
}

template<typename T, std::uint8_t N>
SH<T, N> operator-(const SH<T, N>& sh1, const T& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] - sh2;
	return result;
}

template<typename T, std::uint8_t N>
SH<T, N> operator*(const SH<T, N>& sh1, const T& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] * sh2;
	return result;
}

template<typename T, std::uint8_t N>
SH<T, N> operator/(const SH<T, N>& sh1, const T& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] / sh2;
	return result;
}

template<typename T, std::uint8_t N>
SH<Vector3t<T>, N> operator+(const SH<Vector3t<T>, N>& sh1, const T& scale) noexcept
{
	SH<Vector3t<T>, N> result;
	for (SH<Vector3t<T>, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] + scale;
	return result;
}

template<typename T, std::uint8_t N>
SH<Vector3t<T>, N> operator-(const SH<Vector3t<T>, N>& sh1, const T& scale) noexcept
{
	SH<Vector3t<T>, N> result;
	for (SH<Vector3t<T>, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] - scale;
	return result;
}

template<typename T, std::uint8_t N>
SH<Vector3t<T>, N> operator*(const SH<Vector3t<T>, N>& sh1, const T& scale) noexcept
{
	SH<Vector3t<T>, N> result;
	for (SH<Vector3t<T>, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] * scale;
	return result;
}

template<typename T, std::uint8_t N>
SH<Vector3t<T>, N> operator/(const SH<Vector3t<T>, N>& sh1, const T& scale) noexcept
{
	SH<Vector3t<T>, N> result;
	for (SH<Vector3t<T>, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] / scale;
	return result;
}

template<typename T, std::uint8_t N>
SH<Vector3t<T>, N> operator*(const SH<T, N>& sh1, const Vector3t<T>& color) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] * color;
	return result;
}

template<typename T, std::uint8_t N>
SH<Vector3t<T>, N> operator/(const SH<T, N>& sh1, const Vector3t<T>& color) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] / color;
	return result;
}

template<typename T, std::uint8_t N>
SH<Vector3t<T>, N> operator*(const Vector3t<T>& color, const SH<T, N>& sh1) noexcept
{
	SH<Vector3t<T>, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = color * sh1.coeff[i];
	return result;
}

template<typename T, std::uint8_t N>
SH<Vector3t<T>, N> operator/(const Vector3t<T>& color, const SH<T, N>& sh1) noexcept
{
	SH<Vector3t<T>, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = color / sh1.coeff[i];
	return result;
}

template<typename T, std::uint8_t N>
SH<Vector3t<T>, N> operator*(const SH<Vector3t<T>, N>& sh1, const Vector3t<T>& color) noexcept
{
	SH<Vector3t<T>, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] * color;
	return result;
}

template<typename T, std::uint8_t N>
SH<Vector3t<T>, N> operator/(const SH<Vector3t<T>, N>& sh1, const Vector3t<T>& color) noexcept
{
	SH<Vector3t<T>, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] / color;
	return result;
}

template<typename T, std::uint8_t N>
SH<Vector3t<T>, N> operator*(const Vector3t<T>& color, const SH<Vector3t<T>, N>& sh1) noexcept
{
	SH<Vector3t<T>, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = color * sh1.coeff[i];
	return result;
}

template<typename T, std::uint8_t N>
SH<Vector3t<T>, N> operator/(const Vector3t<T>& color, const SH<Vector3t<T>, N>& sh1) noexcept
{
	SH<Vector3t<T>, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = color / sh1.coeff[i];
	return result;
}

namespace math
{
	template<typename T, std::uint8_t N>
	inline T dot(const SH<T, N>& a, const SH<T, N>& b) noexcept
	{
		T result = T(0);
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result += a.coeff[i] * b.coeff[i];
		return result;
	}

	template<typename T, std::uint8_t N>
	inline T length2(const SH<T, N>& sh) noexcept
	{
		return dot(sh, sh);
	}

	template<typename T, std::uint8_t N>
	inline T length(const SH<T, N>& sh) noexcept
	{
		return math::sqrt(dot(sh, sh));
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> min(const SH<T, N>& a, const SH<T, N>& b) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::min(a.coeff[i], b.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> max(const SH<T, N>& a, const SH<T, N>& b) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::max(a.coeff[i], b.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> normalize(const SH<T, N>& sh) noexcept
	{
		T magSq = length2(sh);
		if (magSq > 0)
		{
			T invSqrt = 1.0 / sqrt(magSq);
			return sh * invSqrt;
		}

		return sh;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> negate(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = -sh.coeff[i];
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> isfinite(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::isfinite(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> abs(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::abs(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> cos(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::cos(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> sin(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::sin(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> tan(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::tan(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> acos(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::acos(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> asin(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::asin(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> atan(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::atan(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> exp(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::exp(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> exp2(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::exp2(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> sqrt(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::sqrt(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> log(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::log(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> log2(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::log2(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> log10(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::log10(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> pow(const SH<T, N>& sh, T y) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::pow(sh.coeff[i], y);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> pow(const SH<T, N>& a, const SH<T, N>& b) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::pow(a.coeff[i], b.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> ceil(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::ceil(sh.coeff[i]);
		return result;
	}

	template<typename T, std::uint8_t N>
	inline SH<T, N> floor(const SH<T, N>& sh) noexcept
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::ceil(sh.coeff[i]);
		return result;
	}

	template<std::uint8_t N, typename T, typename = std::enable_if_t<N == 4>>
	inline SH<T, 4> ProjectOntoSH(const Vector3t<T>& dir) noexcept
	{
		SH<T, 4> sh;
		sh.coeff[0] =  0.282095f;
		sh.coeff[1] = -0.488603f * dir.y;
		sh.coeff[2] =  0.488603f * dir.z;
		sh.coeff[3] = -0.488603f * dir.x;

		return sh;
	}

	template<std::uint8_t N, typename T, typename = std::enable_if_t<N == 9>>
	inline SH<T, 9> ProjectOntoSH(const Vector3t<T>& dir) noexcept
	{
		SH<T, 9> sh;
		sh.coeff[0] =  0.282095f;
		sh.coeff[1] = -0.488603f * dir.y;
		sh.coeff[2] =  0.488603f * dir.z;
		sh.coeff[3] = -0.488603f * dir.x;
		sh.coeff[4] =  1.092548f * dir.x * dir.y;
		sh.coeff[5] = -1.092548f * dir.y * dir.z;
		sh.coeff[6] =  0.315392f * (3.0f * dir.z * dir.z - 1.0f);
		sh.coeff[7] = -1.092548f * dir.x * dir.z;
		sh.coeff[6] =  0.315392f * (3.0f * dir.z * dir.z - 1.0f);
		sh.coeff[7] =  1.092548f * dir.x * dir.z;
		sh.coeff[8] =  0.546274f * (dir.x * dir.x - dir.y * dir.y);

		return sh;
	}
	
	template<std::uint8_t N, typename T, typename = std::enable_if_t<N == 16>>
	inline SH<T, 16> ProjectOntoSH(const Vector3t<T>& dir) noexcept
	{
		SH<T, 16> sh;
		sh.coeff[0]  =  0.282095f;
		sh.coeff[1]  = -0.488603f * dir.y;
		sh.coeff[2]  =  0.488603f * dir.z;
		sh.coeff[3]  = -0.488603f * dir.x;
		sh.coeff[4]  =  1.092548f * dir.x * dir.y;
		sh.coeff[5]  = -1.092548f * dir.y * dir.z;
		sh.coeff[6]  =  0.315392f * (3.0f * dir.z * dir.z - 1.0f);
		sh.coeff[7]  = -1.092548f * dir.x * dir.z;
		sh.coeff[8]  =  0.546274f * (dir.x * dir.x - dir.y * dir.y);
		sh.coeff[9]  = -0.590044f * dir->y * (3.0f * dir->x * dir->x - dir->y * dir->y);
		sh.coeff[10] =  2.890610f * dir->x * dir->y * dir->z;
		sh.coeff[11] = -0.457046f * dir->y * (-1.0f + 5.0f * dir->z * dir->z);
		sh.coeff[12] =  0.373176f * dir->z * (5.0f * dir->z * dir->z - 3.0f);
		sh.coeff[13] =  0.457046f * dir->x * (1.0f - 5.0f * dir->z * dir->z);
		sh.coeff[14] =  1.445310f * dir->z * (dir->x * dir->x - dir->y * dir->y);
		sh.coeff[15] = -0.590044f * dir->x * (dir->x * dir->x - 3.0f * dir->y * dir->y);
	
		return sh;
	}

	template<std::uint8_t N, typename T, typename = std::enable_if_t<N == 25>>
	inline SH<T, 25> ProjectOntoSH(const Vector3t<T>& dir) noexcept
	{
		const T x = dir[0];
		const T y = dir[1];
		const T z = dir[2];

		const T x2 = x * x;
		const T y2 = y * y;
		const T z2 = z * z;

		const T z3 = z * z * z;

		const T x4 = x2 * x2;
		const T y4 = y2 * y2;
		const T z4 = z2 * z2;

		SH<T, 25> sh;
		sh.coeff[0] = 0.5f / std::sqrt(M_PI);
		sh.coeff[1] = -std::sqrt(3.0f / (M_PI * 4)) * y;
		sh.coeff[2] =  std::sqrt(3.0f / (M_PI * 4)) * z;
		sh.coeff[3] = -std::sqrt(3.0f / (M_PI * 4)) * x;

		sh.coeff[4] =  std::sqrt(15.0f / (M_PI * 4)) * y * x;
		sh.coeff[5] = -std::sqrt(15.0f / (M_PI * 4) )* y * z;
		sh.coeff[6] =  std::sqrt( 5.0f / (M_PI * 16))* (3.0f * z2 - 1.0f);
		sh.coeff[7] = -std::sqrt(15.0f / (M_PI * 4)) * x * z;
		sh.coeff[8] =  std::sqrt(15.0f / (M_PI * 16))* (x2 - y2);

		sh.coeff[9]  = -std::sqrt(70.0f  / (M_PI * 64)) * y * (3 * x2 - y2);
		sh.coeff[10] =  std::sqrt(105.0f / (M_PI * 4))*y*x*z;
		sh.coeff[11] = -std::sqrt(21.0f  / (M_PI * 16))*y*(-1.0f + 5.0f*z2);
		sh.coeff[12] =  std::sqrt(7.0f   / (M_PI * 16))*(5.0f*z3 - 3.0f*z);
		sh.coeff[13] = -std::sqrt(42.0f  / (M_PI * 64))*x*(-1.0f + 5.0f*z2);
		sh.coeff[14] =  std::sqrt(105.0f / (M_PI * 16))*(x2 - y2)*z;
		sh.coeff[15] = -std::sqrt(70.0f  / (M_PI * 64))*x*(x2 - 3.0f*y2);

		sh.coeff[16] =  3.0f * std::sqrt(35.0f / (M_PI * 16))*x*y*(x2 - y2);
		sh.coeff[17] = -3.0f * std::sqrt(70.0f / (M_PI * 64))*y*z*(3.0f*x2 - y2);
		sh.coeff[18] =  3.0f * std::sqrt(5.0f  / (M_PI * 16))*y*x*(-1.0f + 7.0f*z2);
		sh.coeff[19] = -3.0f * std::sqrt(10.0f / (M_PI * 64))*y*z*(-3.0f + 7.0f*z2);
		sh.coeff[20] =  (105.0f*z4 - 90.0f*z2 + 9.0f) / (16.0f * std::sqrt(M_PI));
		sh.coeff[21] = -3.0f * std::sqrt(10.0f / (M_PI * 64))*x*z*(-3.0f + 7.0f*z2);
		sh.coeff[22] =  3.0f * std::sqrt(5.0f  / (M_PI * 64))*(x2 - y2)*(-1.0f + 7.0f*z2);
		sh.coeff[23] = -3.0f * std::sqrt(70.0f / (M_PI * 64))*x*z*(x2 - 3.0f*y2);
		sh.coeff[24] =  3.0f * std::sqrt(35.0f / (4.0f*(M_PI * 64)))*(x4 - 6.0f*y2*x2 + y4);

		return sh;
	}

	template<std::uint8_t N, typename T, typename = std::enable_if_t<N == 36>>
	inline SH<T, 36> ProjectOntoSH(const Vector3t<T>& dir) noexcept
	{
		sh.coeff[0]  =  0.5f  / std::sqrt(M_PI);
		sh.coeff[1]  = -0.5f  / std::sqrt(M_PI / 3.0f) * dir->y;
		sh.coeff[2]  =  0.5f  / std::sqrt(M_PI / 3.0f) * dir->z;
		sh.coeff[3]  = -0.5f  / std::sqrt(M_PI / 3.0f) * dir->x;
		sh.coeff[4]  =  0.5f  / std::sqrt(M_PI / 15.0f) * dir->x * dir->y;
		sh.coeff[5]  = -0.5f  / std::sqrt(M_PI / 15.0f) * dir->y * dir->z;
		sh.coeff[6]  =  0.25f / std::sqrt(M_PI / 5.0f)  * (3.0f * dir->z * dir->z - 1.0f);
		sh.coeff[7]  = -0.5f  / std::sqrt(M_PI / 15.0f) * dir->x * dir->z;
		sh.coeff[8]  =  0.25f / std::sqrt(M_PI / 15.0f) * (dir->x * dir->x - dir->y * dir->y);
		sh.coeff[9]  = -std::sqrt(70.0f  / M_PI) / 8.0f * dir->y * (3.0f * dir->x * dir->x - dir->y * dir->y);
		sh.coeff[10] =  std::sqrt(105.0f / M_PI) / 2.0f * dir->x * dir->y * dir->z;
		sh.coeff[11] = -std::sqrt(42.0   / M_PI) / 8.0f * dir->y * (-1.0f + 5.0f * dir->z * dir->z);
		sh.coeff[12] =  std::sqrt(7.0f   / M_PI) / 4.0f * dir->z * dir->z * dir->z * 5.0f - 3.0f * dir->z;
		sh.coeff[13] =  std::sqrt(42.0   / M_PI) / 8.0f * dir->x * (1.0f - 5.0f * dir->z * dir->z);
		sh.coeff[14] =  std::sqrt(105.0f / M_PI) / 4.0f * dir->z * (dir->x * dir->x - dir->y * dir->y);
		sh.coeff[15] = -std::sqrt(70.0f  / M_PI) / 8.0f * dir->x * (dir->x * dir->x - 3.0f * dir->y * dir->y);
		sh.coeff[16] =  std::sqrt(35.0f  / M_PI) * dir->x * dir->y * (dir->x * dir->x - dir->y * dir->y) * 0.75;
		sh.coeff[17] =  3.0f * dir->z * sh.coeff[9];
		sh.coeff[18] =  std::sqrt(5.0f   / M_PI) * dir->x * dir->y * (7.0f * dir->z * dir->z - 1.0f) * 0.75;
		sh.coeff[19] =  std::sqrt(10.0f  / M_PI) * dir->y * dir->z * (3.0f - 7.0f * dir->z * dir->z) * 0.375f;
		sh.coeff[20] =  3.0f / (16.0f * std::sqrt(M_PI)) * (35.0f * dir->z * dir->z * dir->z * dir->z - 30.f * dir->z * dir->z + 3.0f);
		sh.coeff[21] =  std::sqrt(10.0f  / M_PI) * dir->x * dir->z * (3.0f - 7.0f * dir->z * dir->z) * 0.375f;
		sh.coeff[22] =  std::sqrt(5.0f   / M_PI) * (dir->x * dir->x - dir->y * dir->y) * (7.0f * dir->z * dir->z - 1.0f) * 0.375f;
		sh.coeff[23] =  3.0 * dir->z * sh.coeff[15];
		sh.coeff[24] =  std::sqrt(35.0f   / M_PI) * (dir->x * dir->x * dir->x * dir->x - 6.0f * dir->x * dir->x * dir->y * dir->y + dir->y * dir->y * dir->y * dir->y) * 3.0f / 16.0f;
		sh.coeff[25] = -std::sqrt(154.0f  / M_PI) * dir->y * (5.0f * dir->x * dir->x * dir->x * dir->x - 10.0f * dir->x * dir->x * dir->y * dir->y + dir->y * dir->y * dir->y * dir->y) * 3.0f / 32.0f;
		sh.coeff[26] =  std::sqrt(385.0f  / M_PI) * dir->x * dir->y * dir->z * (dir->x * dir->x - dir->y * dir->y) * 0.75f;
		sh.coeff[27] =  std::sqrt(770.0f  / M_PI) / 32.0f * dir->y * (3.0f * dir->x * dir->x - dir->y * dir->y) * (1.0f - 9.0f * dir->z * dir->z);
		sh.coeff[28] =  std::sqrt(1155.0f / M_PI) / 4.0f * dir->x * dir->y * dir->z * (3.0f * dir->z * dir->z - 1.0f);
		sh.coeff[29] =  std::sqrt(165.0f  / M_PI) / 16.0f * dir->y * (14.0f * dir->z * dir->z - 21.0f * dir->z * dir->z * dir->z * dir->z - 1.0f);
		sh.coeff[30] =  std::sqrt(11.0f   / M_PI) / 16.0f * dir->z * (63.0f * dir->z * dir->z * dir->z * dir->z - 70.0f * dir->z * dir->z + 15.0f);
		sh.coeff[31] =  std::sqrt(165.0f  / M_PI) / 16.0f * dir->x * (14.0f * dir->z * dir->z - 21.0f * dir->z * dir->z * dir->z * dir->z - 1.0f);
		sh.coeff[32] =  std::sqrt(1155.0f / M_PI) / 8.0f * dir->z * (dir->x * dir->x - dir->y * dir->y) * (3.0f * dir->z * dir->z - 1.0f);
		sh.coeff[33] =  std::sqrt(770.0f  / M_PI) / 32.0f * dir->x * (dir->x * dir->x - 3.0f * dir->y * dir->y) * (1.0f - 9.0f * dir->z * dir->z);
		sh.coeff[34] =  3.0f / 16.0f * std::sqrt(385.0f / M_PI) * dir->z * (dir->x * dir->x * dir->x * dir->x - 6.0 * dir->x * dir->x * dir->y * dir->y + dir->y * dir->y * dir->y * dir->y);
		sh.coeff[35] = -3.0f / 32.0f * std::sqrt(154.0f / M_PI) * dir->x * (dir->x * dir->x * dir->x * dir->x - 10.0f * dir->x * dir->x * dir->y * dir->y + 5.0f * dir->y * dir->y * dir->y * dir->y);
	}

	template<typename T>
	inline SH<T, 4> ProjectOntoH4(const Vector3t<T>& dir) noexcept
	{
		SH<T, 4> result;
		result[0] = (1.0f / std::sqrt(2.0f * M_PI));
		result[1] = std::sqrt(1.5f / M_PI) * dir.y;
		result[2] = std::sqrt(1.5f / M_PI) * (2 * dir.z - 1.0f);
		result[3] = std::sqrt(1.5f / M_PI) * dir.x;

		return result;
	}

	template<typename T>
	inline Vector3t<T> EvalSH4Cosine(const Vector3t<T>& dir, const SH<Vector3t<T>, 4>& sh) noexcept
	{
		SH<T, 4> dirSH = ProjectOntoSH4(dir);
		dirSH.coeff[0] *= CosineA0;
		dirSH.coeff[1] *= CosineA1;
		dirSH.coeff[2] *= CosineA1;
		dirSH.coeff[3] *= CosineA1;

		SH<T, 4> result;
		for (std::uint8_t i = 0; i < 4; ++i)
			result += dirSH.coeff[i] * sh.coeff[i];

		return result;
	}

	template<typename T>
	inline Vector3t<T> EvalSH9Cosine(const Vector3t<T>& dir, const SH<Vector3t<T>, 9>& sh) noexcept
	{
		SH<T, 9> dirSH = ProjectOntoSH9(dir);
		dirSH.coeff[0] *= CosineA0;
		dirSH.coeff[1] *= CosineA1;
		dirSH.coeff[2] *= CosineA1;
		dirSH.coeff[3] *= CosineA1;
		dirSH.coeff[4] *= CosineA2;
		dirSH.coeff[5] *= CosineA2;
		dirSH.coeff[6] *= CosineA2;
		dirSH.coeff[7] *= CosineA2;
		dirSH.coeff[8] *= CosineA2;

		Vector3t<T> result;
		for (std::uint8_t i = 0; i < 9; ++i)
			result += dirSH.coeff[i] * sh.coeff[i];

		return result;
	}

	template<typename T>
	inline T EvalH4(const SH<T, 4>& h, const Vector3t<T>& dir) noexcept
	{
		H4 b = ProjectOntoH4(dir);
		return math::dot(h, b);
	}

	template<typename T>
	inline SH<T, 4> ConvertToH4(const SH<T, 9>& sh) noexcept
	{
		const T rt2 = sqrt(2.0f);
		const T rt32 = sqrt(3.0f / 2.0f);
		const T rt52 = sqrt(5.0f / 2.0f);
		const T rt152 = sqrt(15.0f / 2.0f);
		const T convMatrix[4][9] =
		{
			{ 1.0f / rt2, 0, 0.5f * rt32, 0, 0, 0, 0, 0, 0 },
			{ 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0, 0, 0 },
			{ 0, 0, 1.0f / (2.0f * rt2), 0, 0, 0, 0.25f * rt152, 0, 0 },
			{ 0, 0, 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0 }
		};

		SH<T, 4> hBasis;

		for (std::uint8_t row = 0; row < 4; ++row)
		{
			hBasis.coeff[row] = 0.0f;
			for (std::uint8_t col = 0; col < 9; ++col)
				hBasis.coeff[row] += convMatrix[row][col] * sh.coeff[col];
		}

		return hBasis;
	}

	template<typename T>
	inline SH<T, 6> ConvertToH6(const SH<T, 9>& sh) noexcept
	{
		const T rt2 = sqrt(2.0f);
		const T rt32 = sqrt(3.0f / 2.0f);
		const T rt52 = sqrt(5.0f / 2.0f);
		const T rt152 = sqrt(15.0f / 2.0f);
		const T convMatrix[6][9] =
		{
			{ 1.0f / rt2, 0, 0.5f * rt32, 0, 0, 0, 0, 0, 0 },
			{ 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0, 0, 0 },
			{ 0, 0, 1.0f / (2.0f * rt2), 0, 0, 0, 0.25f * rt152, 0, 0 },
			{ 0, 0, 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0 },
			{ 0, 0, 0, 0, 1.0f / rt2, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 1.0f / rt2 }
		};

		SH<T, 6> hBasis;

		for (std::uint8_t row = 0; row < 6; ++row)
		{
			hBasis.coeff[row] = 0.0f;
			for (std::uint8_t col = 0; col < 9; ++col)
				hBasis.coeff[row] += convMatrix[row][col] * sh.coeff[col];
		}

		return hBasis;
	}

	template<typename T>
	inline SH<Vector3t<T>, 4> ConvertToH4(const SH<Vector3t<T>, 9>& sh) noexcept
	{
		const T rt2 = sqrt(2.0f);
		const T rt32 = sqrt(3.0f / 2.0f);
		const T rt52 = sqrt(5.0f / 2.0f);
		const T rt152 = sqrt(15.0f / 2.0f);
		const T convMatrix[4][9] =
		{
			{ 1.0f / rt2, 0, 0.5f * rt32, 0, 0, 0, 0, 0, 0 },
			{ 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0, 0, 0 },
			{ 0, 0, 1.0f / (2.0f * rt2), 0, 0, 0, 0.25f * rt152, 0, 0 },
			{ 0, 0, 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0 }
		};

		SH<Vector3t<T>, 4> hBasis;

		for (std::uint8_t row = 0; row < 4; ++row)
		{
			hBasis.coeff[row].set(0.0f);
			for (std::uint8_t col = 0; col < 9; ++col)
				hBasis.coeff[row] += convMatrix[row][col] * sh.coeff[col];
		}

		return hBasis;
	}

	template<typename T>
	inline SH<Vector3t<T>, 6> ConvertToH6(const SH<Vector3t<T>, 9>& sh) noexcept
	{
		const T rt2 = sqrt(2.0f);
		const T rt32 = sqrt(3.0f / 2.0f);
		const T rt52 = sqrt(5.0f / 2.0f);
		const T rt152 = sqrt(15.0f / 2.0f);
		const T convMatrix[6][9] =
		{
			{ 1.0f / rt2, 0, 0.5f * rt32, 0, 0, 0, 0, 0, 0 },
			{ 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0, 0, 0 },
			{ 0, 0, 1.0f / (2.0f * rt2), 0, 0, 0, 0.25f * rt152, 0, 0 },
			{ 0, 0, 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0 },
			{ 0, 0, 0, 0, 1.0f / rt2, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 1.0f / rt2 }
		};

		SH<Vector3t<T>, 6> hBasis;

		for (std::uint8_t row = 0; row < 6; ++row)
		{
			hBasis.coeff[row].set(0.0f);
			for (std::uint8_t col = 0; col < 9; ++col)
				hBasis.coeff[row] += convMatrix[row][col] * sh.coeff[col];
		}

		return hBasis;
	}

	template<typename T>
	inline Vector3t<T> CalcCubeNormal(T u, T v, std::uint8_t face) noexcept
	{
		switch (face)
		{
		case 0: //+x
			return normalize(Vector3t<T>( 1.0f, -v,-u));
		case 1: //-x
			return normalize(Vector3t<T>(-1.0f, -v, u));
		case 2: //+y
			return normalize(Vector3t<T>(u, 1.0f, v));
		case 3: //-y
			return normalize(Vector3t<T>(u,-1.0f,-v));
		case 4: //+z
			return normalize(Vector3t<T>( u,-v, 1.0f));
		case 5: //-z
			return normalize(Vector3t<T>(-u,-v,-1.0f));
		default:
			assert(false);
			return Vector3t<T>::Zero;
		}
	}

	template<typename T>
	inline Vector3t<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint8_t face) noexcept
	{
		const T u = ((x + 0.5f) / T(w)) * 2.0f - 1.0f;
		const T v = ((y + 0.5f) / T(h)) * 2.0f - 1.0f;
		return CalcCubeNormal(u, v, face);
	}

	template<std::size_t face, typename T, std::enable_if_t<face == 0, int> = 0>
	inline Vector3t<T> CalcCubeNormal(T u, T v) noexcept
	{
		return normalize(Vector3t<T>(1.0f, -v, -u));
	}

	template<std::size_t face, typename T, std::enable_if_t<face == 1, int> = 0>
	inline Vector3t<T> CalcCubeNormal(T u, T v) noexcept
	{
		return normalize(Vector3t<T>(-1.0f, -v, u));
	}

	template<std::size_t face, typename T, std::enable_if_t<face == 2, int> = 0>
	inline Vector3t<T> CalcCubeNormal(T u, T v) noexcept
	{
		return normalize(Vector3t<T>(u, 1.0f, v));
	}

	template<std::size_t face, typename T, std::enable_if_t<face == 3, int> = 0>
	inline Vector3t<T> CalcCubeNormal(T u, T v) noexcept
	{
		return normalize(Vector3t<T>(u, -1.0f, -v));
	}

	template<std::size_t face, typename T, std::enable_if_t<face == 4, int> = 0>
	inline Vector3t<T> CalcCubeNormal(T u, T v) noexcept
	{
		return normalize(Vector3t<T>(u, -v, 1.0f));
	}

	template<std::size_t face, typename T, std::enable_if_t<face == 5, int> = 0>
	inline Vector3t<T> CalcCubeNormal(T u, T v) noexcept
	{
		return normalize(Vector3t<T>(-u, -v, -1.0f));
	}

	template<std::size_t face, typename T, std::enable_if_t<face == 0, int> = 0>
	inline Vector3t<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
	{
		const T u = ((x + 0.5f) / T(w)) * 2.0f - 1.0f;
		const T v = ((y + 0.5f) / T(h)) * 2.0f - 1.0f;
		return normalize(Vector3t<T>(1.0f, -v, -u));
	}

	template<std::size_t face, typename T, std::enable_if_t<face == 1, int> = 0>
	inline Vector3t<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
	{
		const T u = ((x + 0.5f) / T(w)) * 2.0f - 1.0f;
		const T v = ((y + 0.5f) / T(h)) * 2.0f - 1.0f;
		return normalize(Vector3t<T>(-1.0f, -v, u));
	}

	template<std::size_t face, typename T, std::enable_if_t<face == 2, int> = 0>
	inline Vector3t<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
	{
		const T u = ((x + 0.5f) / T(w)) * 2.0f - 1.0f;
		const T v = ((y + 0.5f) / T(h)) * 2.0f - 1.0f;
		return normalize(Vector3t<T>(u, 1.0f, v));
	}

	template<std::size_t face, typename T, std::enable_if_t<face == 3, int> = 0>
	inline Vector3t<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
	{
		const T u = ((x + 0.5f) / T(w)) * 2.0f - 1.0f;
		const T v = ((y + 0.5f) / T(h)) * 2.0f - 1.0f;
		return normalize(Vector3t<T>(u, -1.0f, -v));
	}

	template<std::size_t face, typename T, std::enable_if_t<face == 4, int> = 0>
	inline Vector3t<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
	{
		const T u = ((x + 0.5f) / T(w)) * 2.0f - 1.0f;
		const T v = ((y + 0.5f) / T(h)) * 2.0f - 1.0f;
		return normalize(Vector3t<T>(u, -v, 1.0f));
	}

	template<std::size_t face, typename T, std::enable_if_t<face == 5, int> = 0>
	inline Vector3t<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
	{
		const T u = ((x + 0.5f) / T(w)) * 2.0f - 1.0f;
		const T v = ((y + 0.5f) / T(h)) * 2.0f - 1.0f;
		return normalize(Vector3t<T>(-u, -v, -1.0f));
	}

	template<std::uint8_t N, typename T, std::uint8_t face, std::enable_if_t<(N == 6 || N == 9 || N == 25) && face >= 0 && face < 6, int> = 0>
	SH<Vector3t<T>, N> CalcCubefaceToSH(std::uint32_t w, std::uint32_t h, const T* data, std::uint8_t channel, double& weights) noexcept
	{
		SH<Vector3t<T>, N> result(Vector3t<T>::Zero);

		for (std::uint32_t y = 0; y < h; ++y)
		{
			for (std::uint32_t x = 0; x < w; ++x)
			{
				const auto& sample = (Vector3t<T>&)data[(face * w * h + y * w + x) * channel];

				const T u = ((x + T(0.5f)) / w) * T(2.0f) - T(1.0f);
				const T v = ((y + T(0.5f)) / h) * T(2.0f) - T(1.0f);

				const T temp = 1.0f + u * u + v * v;
				const T weight = 4.0f / (sqrt(temp) * temp);

				const Vector3t<T> dir = CalcCubeNormal<face, T>(u, v);

				const SH<T, N> sh = ProjectOntoSH<N, T>(dir);

				for (std::uint8_t i = 0; i < N; ++i)
					result.coeff[i] += sample * sh.coeff[i] * weight;

				weights += weight;
			}
		}

		return result;
	}

	template<std::uint8_t N = 9, typename T = float, std::enable_if_t<N == 6 || N == 9 || N == 25, int> = 0>
	SH<Vector3t<T>, N> CalcCubemapToSH(std::uint32_t w, std::uint32_t h, const T* data, std::uint8_t channel = 3) noexcept
	{
		double weightSum = 0.0;

		SH<Vector3t<T>, N> result(Vector3t<T>::Zero);
		result += CalcCubefaceToSH<N, T, 0>(w, h, data, channel, weightSum);
		result += CalcCubefaceToSH<N, T, 1>(w, h, data, channel, weightSum);
		result += CalcCubefaceToSH<N, T, 2>(w, h, data, channel, weightSum);
		result += CalcCubefaceToSH<N, T, 3>(w, h, data, channel, weightSum);
		result += CalcCubefaceToSH<N, T, 4>(w, h, data, channel, weightSum);
		result += CalcCubefaceToSH<N, T, 5>(w, h, data, channel, weightSum);

		const float norm = T((4.0 * M_PI) / weightSum);
		return result * norm;
	}

	template<std::uint8_t N, typename T, std::uint8_t face, std::enable_if_t<(N == 9 || N == 16) && face >= 0 && face < 6, int> = 0>
	void CalcCubefaceToIrradiance(const SH<Vector3t<T>, N>& shColor, std::uint32_t w, std::uint32_t h, Vector3t<T>* dst) noexcept
	{
		Vector3t<T>* data = dst + w * h * face;

		for (std::uint32_t y = 0; y < h; ++y)
		{
			for (std::uint32_t x = 0; x < w; ++x)
			{
				const auto basis = ProjectOntoSH<N, T>(CalcCubeNormal<face, T>(x, y, w, h));
				Vector3t<T> color = shColor.coeff[0] * basis[0];

				std::uint8_t n = 1;
				for (; n < 4; ++n)
					color += shColor.coeff[n] * basis[n] * (2.0f / 3.0f);

				for (; n < 9; ++n)
					color += shColor.coeff[n] * basis[n] * (1.0f / 4.0f);

				*(data++) = color;
			}
		}
	}

	template<std::uint8_t N, typename T, std::size_t face, std::enable_if_t<(N == 25) && face >= 0 && face < 6, int> = 0>
	void CalcCubefaceToIrradiance(const SH<Vector3t<T>, N>& shColor, std::uint32_t w, std::uint32_t h, Vector3t<T>* dst) noexcept
	{
		Vector3t<T>* data = dst + (w * h) * face;

		for (std::uint32_t y = 0; y < h; ++y)
		{
			for (std::uint32_t x = 0; x < w; ++x)
			{
				const auto basis = ProjectOntoSH<N, T>(CalcCubeNormal<face, T>(x, y, w, h));

				Vector3t<T> color = shColor.coeff[0] * basis[0];

				std::uint8_t n = 1;
				for (; n < 4; ++n)
					color += shColor.coeff[n] * basis[n] * (2.0f / 3.0f);

				for (; n < 9; ++n)
					color += shColor.coeff[n] * basis[n] * (1.0f / 4.0f);

				for (n = 16; n < 25; ++n)
					color += shColor.coeff[n] * basis[n] * (-1.0f / 24.0f);

				*(data++) = color;
			}
		}
	}

	template<std::uint8_t N = 9, typename T = float, std::enable_if_t<N == 6 || N == 9 || N == 25, int> = 0>
	void CalcCubemapToIrradiance(const SH<Vector3t<T>, N>& shColor, std::uint32_t w, std::uint32_t h, Vector3t<T>* dst) noexcept
	{
		CalcCubefaceToIrradiance<N, T, 0>(shColor, w, h, dst);
		CalcCubefaceToIrradiance<N, T, 1>(shColor, w, h, dst);
		CalcCubefaceToIrradiance<N, T, 2>(shColor, w, h, dst);
		CalcCubefaceToIrradiance<N, T, 3>(shColor, w, h, dst);
		CalcCubefaceToIrradiance<N, T, 4>(shColor, w, h, dst);
		CalcCubefaceToIrradiance<N, T, 5>(shColor, w, h, dst);
	}
}

_NAME_END

#endif