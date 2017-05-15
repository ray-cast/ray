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

template<typename T, std::size_t N>
class SH final
{
public:
	using value_t = T;
	using size_t = std::size_t;

	static const value_t CosineA0 = M_PI;
	static const value_t CosineA1 = M_TWO_PI / 3.0f;
	static const value_t CosineA2 = M_PI / 4.0f;

	value_t coeff[N];

	SH() noexcept {}
	explicit SH(value_t* coeffs) noexcept { for (size_t i = 0; i < N; ++i) this->coeff[i] = coeffs[N]; }

	SH& operator+=(const SH& sh) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] += sh.coeff[i]; return *this; }
	SH& operator-=(const SH& sh) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] -= sh.coeff[i]; return *this; }
	SH& operator*=(const SH& sh) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] *= sh.coeff[i]; return *this; }
	SH& operator/=(const SH& sh) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] /= sh.coeff[i]; return *this; }

	SH& operator+=(const value_t& scale) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] += scale; return *this; }
	SH& operator-=(const value_t& scale) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] -= scale; return *this; }
	SH& operator/=(const value_t& scale) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] /= scale; return *this; }
	SH& operator*=(const value_t& scale) noexcept { for (size_t i = 0; i < N; ++i) coeff[i] *= scale; return *this; }

	value_t& operator[](size_t n) noexcept { assert(n < N); return coeff[n]; }

	constexpr value_t& operator[](size_t n) const noexcept { assert(n < N); return coeff[n]; }

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

template<typename T, std::size_t N>
SH<T, N> operator+(const SH<T, N>& sh1, const SH<T, N>& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] + sh2.coeff[i];
	return result;
}

template<typename T, std::size_t N>
SH<T, N> operator-(const SH<T, N>& sh1, const SH<T, N>& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] - sh2.coeff[i];
	return result;
}

template<typename T, std::size_t N>
SH<T, N> operator*(const SH<T, N>& sh1, const SH<T, N>& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] * sh2.coeff[i];
	return result;
}

template<typename T, std::size_t N>
SH<T, N> operator/(const SH<T, N>& sh1, const SH<T, N>& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] / sh2.coeff[i];
	return result;
}

template<typename T, std::size_t N>
SH<T, N> operator+(const SH<T, N>& sh1, const T& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] + sh2;
	return result;
}

template<typename T, std::size_t N>
SH<T, N> operator-(const SH<T, N>& sh1, const T& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] - sh2;
	return result;
}

template<typename T, std::size_t N>
SH<T, N> operator*(const SH<T, N>& sh1, const T& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] * sh2;
	return result;
}

template<typename T, std::size_t N>
SH<T, N> operator/(const SH<T, N>& sh1, const T& sh2) noexcept
{
	SH<T, N> result;
	for (SH<T, N>::size_t i = 0; i < N; ++i)
		result.coeff[i] = sh1.coeff[i] / sh2;
	return result;
}

namespace math
{
	template<typename T, std::size_t N>
	inline T dot(const SH<T, N>& a, const SH<T, N>& b)
	{
		T result = T(0);
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result += a.coeff[i] * b.coeff[i];
		return result;
	}

	template<typename T, std::size_t N>
	inline T length2(const SH<T, N>& sh)
	{
		return dot(sh, sh);
	}

	template<typename T, std::size_t N>
	inline T length(const SH<T, N>& sh)
	{
		return math::sqrt(dot(sh, sh));
	}

	template<typename T, std::size_t N>
	inline SH<T, N> min(const SH<T, N>& a, const SH<T, N>& b)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::min(a.coeff[i], b.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> max(const SH<T, N>& a, const SH<T, N>& b)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::max(a.coeff[i], b.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> normalize(const SH<T, N>& sh)
	{
		T magSq = length2(sh);
		if (magSq > 0)
		{
			T invSqrt = 1.0 / sqrt(magSq);
			return sh * invSqrt;
		}

		return sh;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> negate(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = -sh.coeff[i];
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> isfinite(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::isfinite(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> abs(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::abs(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> cos(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::cos(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> sin(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::sin(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> tan(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::tan(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> acos(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::acos(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> asin(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::asin(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> atan(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::atan(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> exp(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::exp(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> exp2(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::exp2(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> sqrt(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::sqrt(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> log(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::log(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> log2(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::log2(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> log10(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::log10(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> pow(const SH<T, N>& sh, T y)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::pow(sh.coeff[i], y);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> pow(const SH<T, N>& a, const SH<T, N>& b)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::pow(a.coeff[i], b.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> ceil(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::ceil(sh.coeff[i]);
		return result;
	}

	template<typename T, std::size_t N>
	inline SH<T, N> floor(const SH<T, N>& sh)
	{
		SH<T, N> result;
		for (SH<T, N>::size_t i = 0; i < N; ++i)
			result.coeff[i] = std::ceil(sh.coeff[i]);
		return result;
	}

	template<typename T>
	inline SH<T, 4> ProjectOntoSH4(const Vector3t<T>& dir)
	{
		SH<T, 4> sh;
		sh.coeff[0] = 0.282095f;
		sh.coeff[1] = 0.488603f * dir.y;
		sh.coeff[2] = 0.488603f * dir.z;
		sh.coeff[3] = 0.488603f * dir.x

		return sh;
	}

	template<typename T>
	inline SH<T, 9> ProjectOntoSH9(const Vector3t<T>& dir)
	{
		SH<T, 9> sh;
		sh.coeff[0] = 0.282095f;
		sh.coeff[1] = 0.488603f * dir.y;
		sh.coeff[2] = 0.488603f * dir.z;
		sh.coeff[3] = 0.488603f * dir.x;
		sh.coeff[4] = 1.092548f * dir.x * dir.y;
		sh.coeff[5] = 1.092548f * dir.y * dir.z;
		sh.coeff[6] = 0.315392f * (3.0f * dir.z * dir.z - 1.0f);
		sh.coeff[7] = 1.092548f * dir.x * dir.z;
		sh.coeff[8] = 0.546274f * (dir.x * dir.x - dir.y * dir.y);

		return sh;
	}

	template<typename T>
	inline SH<T, 4> ProjectOntoH4(const Vector3t<T>& dir)
	{
		SH<T, 4> result;
		result[0] = (1.0f / std::sqrt(2.0f * 3.14159f));
		result[1] = std::sqrt(1.5f / 3.14159f) * dir.y;
		result[2] = std::sqrt(1.5f / 3.14159f) * (2 * dir.z - 1.0f);
		result[3] = std::sqrt(1.5f / 3.14159f) * dir.x;

		return result;
	}

	template<typename T>
	inline SH<Vector3t<T>, 4> ProjectOntoSH4Color(const Vector3t<T>& dir, const Vector3t<T>& color)
	{
		SH<T, 4> sh = ProjectOntoSH4(dir);
		SH<Vector3t<T>, 4> shColor;
		for (SH<T, 4>::size_t i = 0; i < 4; ++i)
			shColor.coeff[i] = color * sh.coeff[i];

		return shColor;
	}

	template<typename T>
	inline SH<Vector3t<T>, 9> ProjectOntoSH9Color(const Vector3t<T>& dir, const Vector3t<T>& color)
	{
		SH<T, 9> sh = ProjectOntoSH9(dir);
		SH<Vector3t<T>, 9> result;
		for (SH<T, 4>::size_t i = 0; i < 9; ++i)
			result.coeff[i] = color * sh.coeff[i];
		return result;
	}

	template<typename T>
	inline SH<Vector3t<T>, 4> ProjectOntoH4Color(const Vector3t<T>& dir, const Vector3t<T>& color)
	{
		SH<T, 4> projected = ProjectOntoH4(dir);
		SH<Vector3t<T>, 4> hColor;
		for (SH<T, 4>::size_t i = 0; i < 4; ++i)
			hColor.coeff[i] = color * projected.coeff[i];
		return hColor;
	}

	template<typename T>
	inline Vector3t<T> EvalSH4Cosine(const Vector3t<T>& dir, const SH<Vector3t<T>, 4>& sh)
	{
		SH<T, 4> dirSH = ProjectOntoSH4(dir);
		dirSH.coeff[0] *= CosineA0;
		dirSH.coeff[1] *= CosineA1;
		dirSH.coeff[2] *= CosineA1;
		dirSH.coeff[3] *= CosineA1;

		SH<T, 4> result;
		for (SH<T, 4>::size_t i = 0; i < 4; ++i)
			result += dirSH.coeff[i] * sh.coeff[i];

		return result;
	}

	template<typename T>
	inline Vector3t<T> EvalSH9Cosine(const Vector3t<T>& dir, const SH<Vector3t<T>, 9>& sh)
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
		for (SH<T, 4>::size_t i = 0; i < 9; ++i)
			result += dirSH.coeff[i] * sh.coeff[i];

		return result;
	}

	template<typename T>
	inline T EvalH4(const SH<T, 4>& h, const Vector3t<T>& dir)
	{
		H4 b = ProjectOntoH4(dir);
		return math::dot(h, b);
	}

	template<typename T>
	inline SH<T, 4> ConvertToH4(const SH<T, 9>& sh)
	{
		const float rt2 = sqrt(2.0f);
		const float rt32 = sqrt(3.0f / 2.0f);
		const float rt52 = sqrt(5.0f / 2.0f);
		const float rt152 = sqrt(15.0f / 2.0f);
		const float convMatrix[4][9] =
		{
			{ 1.0f / rt2, 0, 0.5f * rt32, 0, 0, 0, 0, 0, 0 },
			{ 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0, 0, 0 },
			{ 0, 0, 1.0f / (2.0f * rt2), 0, 0, 0, 0.25f * rt152, 0, 0 },
			{ 0, 0, 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0 }
		};

		SH<T, 4> hBasis;

		for (SH<T, 4>::size_t row = 0; row < 4; ++row)
		{
			hBasis.coeff[row] = 0.0f;

			for (SH<T, 4>::size_t col = 0; col < 9; ++col)
				hBasis.coeff[row] += convMatrix[row][col] * sh.coeff[col];
		}

		return hBasis;
	}

	template<typename T, typename C = Vector3t<T>>
	inline SH<C, 4> ConvertToH4(const SH<C, 9>& sh)
	{
		const float rt2 = sqrt(2.0f);
		const float rt32 = sqrt(3.0f / 2.0f);
		const float rt52 = sqrt(5.0f / 2.0f);
		const float rt152 = sqrt(15.0f / 2.0f);
		const float convMatrix[4][9] =
		{
			{ 1.0f / rt2, 0, 0.5f * rt32, 0, 0, 0, 0, 0, 0 },
			{ 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0, 0, 0 },
			{ 0, 0, 1.0f / (2.0f * rt2), 0, 0, 0, 0.25f * rt152, 0, 0 },
			{ 0, 0, 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0 }
		};

		SH<C, 4> hBasis;

		for (SH<T, 4>::size_t row = 0; row < 4; ++row)
		{
			hBasis.coeff[row].set(0.0f);

			for (SH<T, 4>::size_t col = 0; col < 9; ++col)
				hBasis.coeff[row] += convMatrix[row][col] * sh.coeff[col];
		}

		return hBasis;
	}

	template<typename T>
	inline SH<T, 6> ConvertToH6(const SH<T, 9>& sh)
	{
		const float rt2 = sqrt(2.0f);
		const float rt32 = sqrt(3.0f / 2.0f);
		const float rt52 = sqrt(5.0f / 2.0f);
		const float rt152 = sqrt(15.0f / 2.0f);
		const float convMatrix[6][9] =
		{
			{ 1.0f / rt2, 0, 0.5f * rt32, 0, 0, 0, 0, 0, 0 },
			{ 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0, 0, 0 },
			{ 0, 0, 1.0f / (2.0f * rt2), 0, 0, 0, 0.25f * rt152, 0, 0 },
			{ 0, 0, 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0 },
			{ 0, 0, 0, 0, 1.0f / rt2, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 1.0f / rt2 }
		};

		SH<T, 6> hBasis;

		for (SH<T, 4>::size_t row = 0; row < 6; ++row)
		{
			hBasis.coeff[row] = 0.0f;

			for (SH<T, 4>::size_t col = 0; col < 9; ++col)
				hBasis.coeff[row] += convMatrix[row][col] * sh.coeff[col];
		}

		return hBasis;
	}

	template<typename T, typename C = Vector3t<T>>
	inline SH<T, 6> ConvertToH6(const SH<C, 9>& sh)
	{
		const float rt2 = sqrt(2.0f);
		const float rt32 = sqrt(3.0f / 2.0f);
		const float rt52 = sqrt(5.0f / 2.0f);
		const float rt152 = sqrt(15.0f / 2.0f);
		const float convMatrix[6][9] =
		{
			{ 1.0f / rt2, 0, 0.5f * rt32, 0, 0, 0, 0, 0, 0 },
			{ 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0, 0, 0 },
			{ 0, 0, 1.0f / (2.0f * rt2), 0, 0, 0, 0.25f * rt152, 0, 0 },
			{ 0, 0, 0, 1.0f / rt2, 0, 0, 0, (3.0f / 8.0f) * rt52, 0 },
			{ 0, 0, 0, 0, 1.0f / rt2, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 1.0f / rt2 }
		};

		SH<C, 6> hBasis;

		for (SH<T, 4>::size_t row = 0; row < 6; ++row)
		{
			hBasis.coeff[row].set(0.0f);

			for (SH<T, 4>::size_t col = 0; col < 9; ++col)
				hBasis.coeff[row] += convMatrix[row][col] * sh.coeff[col];
		}

		return hBasis;
	}

	template<typename T>
	inline Vector3t<T> CalcCubeNormal(std::uint32_t x, std::uint32_t y, std::uint32_t s, std::uint32_t w, std::uint32_t h)
	{
		float u = ((x + 0.5f) / float(w)) * 2.0f - 1.0f;
		float v = ((y + 0.5f) / float(h)) * 2.0f - 1.0f;

		switch (s) 
		{
		case 0: //+z
			return normalize(Vector3t<T>(u, v, 1.0f));
		case 1: //-z
			return normalize(Vector3t<T>(-u, v, -1.0f));
		case 2: //+x
			return normalize(Vector3t<T>(1.0f, v, -u));
		case 3: //-x
			return normalize(Vector3t<T>(-1.0f, v, u));
		case 4: //+y
			return normalize(Vector3t<T>(u, 1.0f, -v));
		case 5: //-y
			return normalize(Vector3t<T>(u, -1.0f, v));
		default:
			assert(false);
			return Vector3t<T>::Zero;
		}
	}
	
	template<typename T>
	inline SH<Vector3t<T>, 9> ProjectCubemapToSH9(std::uint32_t w, std::uint32_t h, Vector3t<T>* data)
	{
		SH<Vector3t<T>, 9> result;

		float weightSum = 0.0f;

		for (SH<T, 4>::size_t face = 0; face < 6; ++face)
		{
			for (SH<T, 4>::size_t y = 0; y < h; ++y)
			{
				for (SH<T, 4>::size_t x = 0; x < w; ++x)
				{
					const Vector3t<T>& sample = data[face * (width * height) + width * y + x];

					T u = (x + T(0.5f)) / width;
					T v = (y + T(0.5f)) / height;

					u = u * T(2.0f) - T(1.0f);
					v = v * T(2.0f) - T(1.0f);

					T temp = 1.0f + u * u + v * v;
					T weight = 4.0f / (std::sqrt(temp) * temp);

					Vector3t<T> dir = CalcCubeNormal(x, y, face, width, height);
					result += ProjectOntoSH9Color(dir, sample) * weight;
					weightSum += weight;
				}
			}
		}

		result *= (T(4.0f) * M_PI) / weightSum;
		return result;
	}
}

_NAME_END

#endif