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
#ifndef _H_ZINTRIN_H_
#define _H_ZINTRIN_H_

#include <ray/platform.h>

#ifdef __SSE__
#include <ray/trait.h>

#if defined(__GNUC__)
#if defined(__INTEL__) && !defined(__LLVM__)
#include <intrin.h>
#include <cpuid.h>
#endif
#elif defined(_VISUAL_STUDIO_)
#include <intrin.h>
#include <malloc.h>
#endif

_NAME_BEGIN

namespace trait
{
	template<>
	struct _typeaddition<float>
	{
		typedef float            value_type;
		typedef float*           pointer;
		typedef float&           reference;
		typedef const float*     const_pointer;
		typedef const float&     const_reference;
#ifdef __SSE__
		typedef __m64            xmm64;
		typedef __m128           xmm128;
#endif
#if   __AVX__
		typedef __m256           xmm256;
#endif
	};

	template <>
	struct _typeaddition<double>
	{
		typedef double           value_type;
		typedef double*          pointer;
		typedef double&          reference;
		typedef const double*     const_pointer;
		typedef const double&    const_reference;
#ifdef __SSE__
		typedef __m64             xmm64;
		typedef __m128d           xmm128;
#endif
#if   __AVX__
		typedef __m256d           xmm256;
#endif
	};
}

inline __m128 _mm_dot_p(__m128 v1, __m128 v2)
{
#if  defined(__SSE4__)
	return _mm_dp_ps(v1, v2, 0xFF);
#elif defined(__SSE3__)
	__m128 v3;

	v3 = _mm_mul_ps(v1, v2);
	v3 = _mm_hadd_ps(v3, v3);

	return _mm_hadd_ps(v3, v3);
#else
	__m128 s, r;
	s = _mm_mul_ps(v1, v2);
	r = _mm_add_ss(s, _mm_movehl_ps(s, s));
	r = _mm_add_ss(r, _mm_shuffle_ps(r, r, 1));
	return r;
#endif
}

inline __m128d _mm_dot_p(__m128d v1, __m128d v2)
{
#if  defined(__SSE4__)
	return _mm_dp_pd(v1, v2, 0xFF);
#elif defined(__SSE3__)
	__m128d v3;

	v3 = _mm_mul_pd(v1, v2);
	v3 = _mm_hadd_pd(v3, v3);

	return _mm_hadd_pd(v3, v3);
#else
	__m128d s, r;
	s = _mm_mul_pd(v1, v2);
	//r = _mm_add_sd(s , _mm_movehl_ps(s , s));
	r = _mm_add_sd(r, _mm_shuffle_pd(r, r, 1));
	return r;
#endif
}

inline __m128 _mm_cross_p(__m128 a, __m128 b)
{
	__m128 ea, eb;

	ea = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1));
	eb = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2));

	__m128 xa = _mm_mul_ps(ea, eb);

	a = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2));
	b = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1));

	__m128 xb = _mm_mul_ps(a, b);

	return _mm_sub_ps(xa, xb);
}

inline __m128d _mm_cross_p(__m128d a, __m128d b)
{
	__m128d a1, b1, a2, b2, v1, v2;

	a1 = _mm_shuffle_pd(a, a, _MM_SHUFFLE(3, 0, 2, 1));
	b1 = _mm_shuffle_pd(b, b, _MM_SHUFFLE(3, 1, 0, 2));

	v1 = _mm_mul_pd(a1, b1);

	a2 = _mm_shuffle_pd(a, a, _MM_SHUFFLE(3, 1, 0, 2));
	b2 = _mm_shuffle_pd(b, b, _MM_SHUFFLE(3, 0, 2, 1));

	v2 = _mm_mul_pd(a2, b2);

	return _mm_sub_pd(v1, v2);
}

inline __m128 _mm_rsqrt(const __m128 a)
{
	static const __m128 _05 = _mm_set1_ps(0.5f);
	static const __m128 _3 = _mm_set1_ps(3.f);
	__m128 rsqrt = _mm_rsqrt_ss(a);

	rsqrt =
		_mm_mul_ss(
			_mm_mul_ss(_05, rsqrt),
			_mm_sub_ss(_3, _mm_mul_ss(a, _mm_mul_ss(rsqrt, rsqrt))));
	return rsqrt;
}

inline __m128d _mm_rsqrt(const __m128d a)
{
	static const __m128d _05 = _mm_set1_pd(0.5f);
	static const __m128d _3 = _mm_set1_pd(3.f);
	static const __m128d _1 = _mm_set1_pd(1.0);

	__m128d rsqrt = _mm_div_sd(_1, _mm_sqrt_sd(a, a));

	rsqrt =
		_mm_mul_sd(
			_mm_mul_sd(_05, rsqrt),
			_mm_sub_sd(_3, _mm_mul_sd(a, _mm_mul_sd(rsqrt, rsqrt))));
	return rsqrt;
}

inline __m128 _mm_normalize(__m128 a)
{
	__m128 v1 = _mm_dot_p(a, a);

	__m128 v2;

	v2 = _mm_rsqrt(v1);
	v2 = _mm_shuffle_ps(v2, v2, 0);

	return _mm_div_ps(a, v2);
}

inline __m128d _mm_normalize(__m128d a)
{
	__m128d v1 = _mm_dot_p(a, a);

	__m128d v2;

	v2 = _mm_rsqrt(v1);
	v2 = _mm_shuffle_pd(v2, v2, 0);

	return _mm_div_pd(a, v2);
}

//inline __m128 operator+(__m128 a, __m128 b)
//    { return ::_mm_add_ps(a, b); }
//
//inline __m128d operator+(__m128d a, __m128d b)
//    { return ::_mm_add_pd(a, b); }
//
//inline __m128 operator-(__m128 a, __m128 b)
//    { return ::_mm_sub_ps(a, b); }
//
//inline __m128d operator-(__m128d a, __m128d b)
//    { return ::_mm_sub_pd(a, b); }
//
//inline __m128 operator*(__m128 a, __m128 b)
//    { return ::_mm_mul_ps(a, b); }
//
//inline __m128d operator*(__m128d a, __m128d b)
//    { return ::_mm_mul_pd(a, b); }
//
//inline __m128 operator/(__m128 a, __m128 b)
//    { return ::_mm_div_ps(a, b); }
//
//inline __m128d operator/(__m128d a, __m128d b)
//    { return ::_mm_div_pd(a, b); }
//
//inline __m128 operator*(__m128 a, float scale)
//    { return ::_mm_mul_ps(a, _mm_set1_ps(scale)); }
//
//inline __m128d operator*(__m128d a, double scale)
//    { return ::_mm_mul_pd(a, _mm_set1_pd(scale)); }
//
//inline __m128 operator/(__m128 a, float scale)
//    { return ::_mm_div_ps(a, _mm_set1_ps(scale)); }
//
//inline __m128d operator/(__m128d a, double scale)
//    { return ::_mm_div_pd(a, _mm_set1_pd(scale)); }
//
//inline __m128 operator==(__m128 a, __m128 b)
//    { return ::_mm_cmpeq_ps(a, b); }
//
//inline __m128d operator==(__m128d a, __m128d b)
//    { return ::_mm_cmpeq_pd(a, b); }
//
//inline __m128 operator!=(__m128 a, __m128 b)
//    { return ::_mm_cmpneq_ps(a, b); }
//
//inline __m128d operator!=(__m128d a, __m128d b)
//    { return ::_mm_cmpneq_pd(a, b); }
//
//inline bool operator&&(bool a, __m128 b)
//{
//    float t1;
//    ::_mm_store_ps(&t1, b);
//    return a && t1;
//}
//
//inline bool operator&&(bool a, __m128d b)
//{
//    double t1;
//    ::_mm_store_pd(&t1, b);
//    return a && t1;
//}
//
//inline bool operator&&(__m128 a, __m128 b)
//{
//    float t1, t2;
//    ::_mm_store_ps(&t1, a);
//    ::_mm_store_ps(&t2, b);
//    return t1 && t2;
//}
//
//inline bool operator&&(__m128d a, __m128d b)
//{
//    double t1, t2;
//    ::_mm_store_pd(&t1, a);
//    ::_mm_store_pd(&t2, b);
//    return t1 && t2;
//}
//
//inline __m128 operator>(__m128 a, __m128 b)
//    { return ::_mm_cmpgt_ps(a, b); }
//
//inline __m128d operator>(__m128d a, __m128d b)
//    { return ::_mm_cmpgt_pd(a, b); }
//
//inline __m128 operator>=(__m128 a, __m128 b)
//    { return ::_mm_cmpge_ps(a, b); }
//
//inline __m128d operator>=(__m128d a, __m128d b)
//    { return ::_mm_cmpge_pd(a, b); }
//
//inline __m128 operator<(__m128 a, __m128 b)
//    { return ::_mm_cmplt_ps(a, b); }
//
//inline __m128d operator<(__m128d a, __m128d b)
//    { return ::_mm_cmplt_pd(a, b); }
//
//inline __m128 operator<=(__m128 a, __m128 b)
//    { return ::_mm_cmple_ps(a, b); }
//
//inline __m128d operator<=(__m128d a, __m128d b)
//    { return ::_mm_cmple_pd(a, b); }
//
//inline __m128 operator&(__m128 a, __m128 b)
//    { return ::_mm_and_ps(a, b); }
//
//inline __m128d operator&(__m128d a, __m128d b)
//    { return ::_mm_and_pd(a, b); }
//
//inline __m128 operator|(__m128 a, __m128 b)
//    { return ::_mm_or_ps(a, b); }
//
//inline __m128d operator|(__m128d a, __m128d b)
//    { return ::_mm_or_pd(a, b); }

_NAME_END

#endif

#endif