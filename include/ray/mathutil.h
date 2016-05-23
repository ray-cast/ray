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
#ifndef _H_MATH_CMN_H_
#define _H_MATH_CMN_H_

#include <ray/platform.h>

#include <math.h>
#include <float.h>
#include <algorithm>

_NAME_BEGIN

#ifndef M_PI
#   define M_PI   3.14159265358979323846f
#   define M_PI_2 1.57079632679489661923f
#   define M_PI_4 0.785398163397448309616f
#   define M_PI_8 0.392699081698724154808f
#   define M_1_PI 0.318309886183790671538f
#   define M_2_PI 0.636619772367581343076f
#endif
#ifndef M_PI_3
#   define M_PI_3 1.047197551196597746f
#endif
#ifndef M_TWO_PI
#   define M_TWO_PI 3.14159265358979323846f * 2.0
#endif

#ifndef DEG_TO_RAD
#   define DEG_TO_RAD(x) (x * M_PI / 180.0f)
#   define RAD_TO_DEG(x) (x * 180.0f / M_PI)
#endif

#ifndef FIXP16
#   define FIXP16
#   define FIXP16_SHIFT     16
#   define FIXP16_MAG       65535
#   define FIXP16_DP_MASK   0x0000FFFF
#   define FIXP16_WP_MASK   0xFFFF0000
#   define FIXP16_ROUND_UP  0x00008000
#endif

#ifndef EPSILON
#   define EPSILON 0.001f
#   define EPSILON_E2 (1E-2)
#   define EPSILON_E3 (1E-3)
#   define EPSILON_E4 (1E-4)
#   define EPSILON_E5 (1E-5)
#   define EPSILON_E6 (1E-6)
#endif

#ifndef SAFE_DELETE
#   define SAFE_DELETE(p)            { if(p) { delete (p); (p)=0; } }
#   define SAFE_DELETE_ARRAY(p)    { if(p) { delete[] (p);    (p)=0; } }
#endif

#ifndef BIG_NUMBER
#   define BIG_NUMBER 1e37f
#endif

#ifndef COMPONENT
#   define COMPONENT
#   define COMPONENT_X 1
#   define COMPONENT_Y 2
#   define COMPONENT_Z 3
#   define COMPONENT_W 4
#endif

namespace math
{
template<typename T>
inline T wrapPI(const T& theta) noexcept
{
    theta += M_PI;
    theta -= std::floor(theta * M_PI_2);
    theta -= M_PI;
    return theta;
}

template<typename T>
inline T max(const T& t1, const T& t2) noexcept
{
    return t1 > t2 ? t1 : t2;
}

template<typename T>
inline T min(const T& t1, const T& t2) noexcept
{
    return t1 < t2 ? t1 : t2;
}

template<typename T>
inline T middle(const T& t1, const T& t2, const T& t3) noexcept
{
    if (t1 < t2)
    {
        if (t2 < t3)
            return t2;
        else
            return t3;
    }
    else
    {
        if (t1 < t3)
            return t1;
        else
            return t3;
    }
}

template<typename T>
inline std::uint8_t bitScanReverse(T number) noexcept
{
	std::uint8_t Result = 0;
	T n = 1;

	for (; n < 32; n++)
	{
		T val = static_cast<T>(1) << n;

		Result++;

		if (number & val)
		{
			break;
		}
	}

	return Result;
}

template<typename T>
inline T ceilToPowerOfTwo(T number) noexcept
{
	T input = number;
	T result = bitScanReverse(number);
	T test = ~(static_cast<T>(1) << result);

	return test & input;
}

inline int _hash_int_0(int key) noexcept
{
	return  ~key + (key << 15);
}

inline int _hash_int_1(int key) noexcept
{
	return key ^ (key >> 12);
}

inline int _hash_int_2(int key) noexcept
{
	return key + (key << 2);
}

inline int _hash_int_3(int key) noexcept
{
	return key ^ (key >> 4);
}

inline int _hash_int_4(int key) noexcept
{
	return key * 2057;
}

inline int _hash_int_5(int key) noexcept
{
	return key ^ (key >> 16);
}

inline int _hash_int(int key) noexcept
{
	return _hash_int_5(_hash_int_4(_hash_int_3(_hash_int_2(_hash_int_1(_hash_int_0(key))))));
}

inline int hash_int(int x, int y, int z) noexcept
{
	return _hash_int(x) ^ _hash_int(y) ^ _hash_int(z);
}

inline float hash_float(float x, float y, float z) noexcept
{
	return x + y * (1.0f / 255.0f) + z * (1.0f / 65025.0f);
}

inline double hash_double(double x, double y, double z) noexcept
{
	return x + y * (1.0f / 255.0f) + z * (1.0f / 65025.0f);
}

inline std::size_t _hash_string(unsigned hash, const char* str) noexcept
{
	return (!*str ? hash : _hash_string(((hash << 5) + hash) + *str, str + 1));
}

inline std::size_t hash_string(const char* str) noexcept
{
	return (!str ? 0 : _hash_string(5381, str));
}

template<typename T>
inline T clamp(const T& t, const T& min, const T& max) noexcept
{
    return std::max(min, std::min(max, t));
}

template<typename T>
inline T saturate(const T& v) noexcept
{
	return clamp(v, 0.0f, 1.0f);
}

template<typename T1, typename T2>
inline T1 lerp(const T1& t1, const T1& t2, const T2& t3) noexcept
{
    return t1 + (t2 - t1) * t3;
}

template<typename T>
inline T smothlerp(const T& x, const T& x1, const T& x2, const T& q00, const T& q01) noexcept
{
    return ((x2 - x) / (x2 - x1)) * q00 + ((x - x1) / (x2 - x1)) * q01;
}

template<typename T>
inline T biLerp(const T& x, const T& y, const T& q11, const T& q12, const T& q21, const T& q22, const T& x1, const T& x2, const T& y1, const T& y2) noexcept
{
    float r1 = smothlerp(x, x1, x2, q11, q21);
    float r2 = smothlerp(x, x1, x2, q12, q22);

    return smothlerp(y, y1, y2, r1, r2);
}

template<typename T>
inline T triLerp(const T& x, const T& y, const T& z, const T& q000, const T& q001, const T& q010, const T& q011, const T& q100, const T& q101, const T& q110, const T& q111, const T& x1, const T& x2, const T& y1, const T& y2, const T& z1, const T& z2) noexcept
{
    float x00 = smothlerp(x, x1, x2, q000, q100);
    float x10 = smothlerp(x, x1, x2, q010, q110);
    float x01 = smothlerp(x, x1, x2, q001, q101);
    float x11 = smothlerp(x, x1, x2, q011, q111);
    float r0 = smothlerp(y, y1, y2, x00, x01);
    float r1 = smothlerp(y, y1, y2, x10, x11);

    return smothlerp(z, z1, z2, r0, r1);
}

template<typename T>
struct _mathutil;

template<>
struct _mathutil<float>
{
    static inline bool equal(float a, float b) noexcept
    {
        return a + (float)EPSILON_E4 > b && a - (float)EPSILON_E4 < b;
    }

	static inline void sinCos(float* returnSin, float* returnCos, float theta) noexcept
	{
		*returnSin = std::sinf(theta);
		*returnCos = std::cosf(theta);
	}

	static inline float rad2deg(float angle) noexcept
	{
		return angle / M_PI * 180.0f;
	}

	static inline float deg2rad(float angle) noexcept
	{
		return angle * M_PI / 180.0f;
	}

	static inline float abs(float t) noexcept
	{
		return t >= 0.0f ? t : -t;
	}

	static inline float exp(float x) noexcept
	{
		return std::expf(x);
	}

	static inline float exp2(float x) noexcept
	{
		return std::exp2f(x);
	}

	static inline float fast_exp2(float x) noexcept
	{
		static const float c[3] = { 5.79526f, 12.82461f, -2.88611f };

		int e = int(round(x));
		float t = x - e;
		float m = (t * t + c[0] * t + c[1]) / (c[2] * t * c[1]);

		return std::ldexpf(m, e);
	}

	static inline float cos(float theta) noexcept
	{
		return std::cosf(theta);
	}

	static inline float sin(float theta) noexcept
	{
		return std::sinf(theta);
	}

	static inline float tan(float theta) noexcept
	{
		return std::tanf(theta);
	}

	static inline float atan2(float y, float x) noexcept
	{
		return std::atan2f(y, x);
	}

	static inline float log(float theta) noexcept
	{
		return std::logf(theta);
	}

	static inline float pow(float x, float y) noexcept
	{
		return std::powf(x, y);
	}

	static inline float sqrt(float theta) noexcept
	{
		return std::sqrtf(theta);
	}

	static inline float rsqrt(float x) noexcept
	{
		float xhalf = 0.5f*x;
		int i = *(int*)&x;
		i = 0x5f3759df - (i >> 1);
		x = *(float*)&i;
		x = x*(1.5f - xhalf*x*x);
		x = x*(1.5f - xhalf*x*x);
		return x;
	}

	static inline float acos(float theta) noexcept
	{
		return std::acosf(theta);
	}

	static inline float asin(float theta) noexcept
	{
		return std::asinf(theta);
	}

	static inline float atan(float theta) noexcept
	{
		return std::atanf(theta);
	}

	static inline float cosh(float theta) noexcept
	{
		return std::coshf(theta);
	}

	static inline float sinh(float theta) noexcept
	{
		return std::sinhf(theta);
	}

	static inline float tanh(float theta) noexcept
	{
		return std::tanhf(theta);
	}

	static inline float log10(float theta) noexcept
	{
		return std::log10f(theta);
	}

	static inline float ceil(float theta) noexcept
	{
		return std::ceilf(theta);
	}

	static inline float fabs(float theta) noexcept
	{
		return std::fabsf(theta);
	}

	static inline float floor(float theta) noexcept
	{
		return std::floorf(theta);
	}

	static inline float modf(float x, float *y) noexcept
	{
		float d;
		float f = std::modff(x, &d);
		*y = d;
		return f;
	}

	static inline float fraction(float v) noexcept
	{
		float intPart;
		return modff(v, &intPart);
	}

	static inline float safeAcos(float x) noexcept
	{
		if (x <= -1.0f) { return M_PI; }
		if (x >= 1.0f) { return 0.0f; }
		return std::acosf(x);
	}

	static inline float snorm2unorm(float x) noexcept
	{
		return x * 0.5f + 0.5f;
	}

	static inline float unorm2snorm(float x) noexcept
	{
		return x * 2.0f - 1.0f;
	}
};

template<>
struct _mathutil<double>
{
    static inline bool equal(double a, double b) noexcept
    {
        return a + (double)EPSILON_E4 > b && a - (double)EPSILON_E4 < b;
    }

	static inline void sinCos(double* returnSin, double* returnCos, double theta) noexcept
	{
		*returnSin = std::sinf(theta);
		*returnCos = std::cosf(theta);
	}

	static inline double rad2deg(double angle) noexcept
	{
		return angle / M_PI * 180.0;
	}

	static inline double deg2rad(double angle) noexcept
	{
		return angle * M_PI / 180.0;
	}

	static inline double abs(double t) noexcept
	{
		return t >= 0.0f ? t : -t;
	}

	static inline double exp(double x) noexcept
	{
		return std::exp(x);
	}

	static inline double exp2(double x) noexcept
	{
		return std::exp2(x);
	}

	static inline double fast_exp2(double x) noexcept
	{
		static const double c[3] = { 5.79526, 12.82461, -2.88611 };

		int e = int(round(x));
		double t = x - e;
		double m = (t * t + c[0] * t + c[1]) / (c[2] * t * c[1]);

		return std::ldexp(m, e);
	}

	static inline double cos(double theta) noexcept
	{
		return std::cos(theta);
	}

	static inline double sin(double theta) noexcept
	{
		return std::sin(theta);
	}

	static inline double tan(double theta) noexcept
	{
		return std::tan(theta);
	}

	static inline double atan2(double y, double x) noexcept
	{
		return std::atan2(y, x);
	}

	static inline double log(double theta) noexcept
	{
		return std::log(theta);
	}

	static inline double pow(double x, double y) noexcept
	{
		return std::pow(x, y);
	}

	static inline double sqrt(double theta) noexcept
	{
		return std::sqrt(theta);
	}

	static inline double rsqrt(double y) noexcept
	{
		long long i;
		double x2;
		const double threehalfs = 1.5F;
		x2 = y * 0.5F;
		i = *(long long*)&y;
		i = 0x5fe6ec85e7de30dall - (i >> 1);
		y = *(double*)&i;
		y = y * (threehalfs - (x2*y*y));
		y = y * (threehalfs - (x2*y*y));
		return y;
	}

	static inline double acos(double theta) noexcept
	{
		return std::acos(theta);
	}

	static inline double asin(double theta) noexcept
	{
		return std::asin(theta);
	}

	static inline double atan(double theta) noexcept
	{
		return std::atan(theta);
	}

	static inline double cosh(double theta) noexcept
	{
		return std::cosh(theta);
	}

	static inline double sinh(double theta) noexcept
	{
		return std::sinh(theta);
	}

	static inline double tanh(double theta) noexcept
	{
		return std::tanh(theta);
	}

	static inline double log10(double theta) noexcept
	{
		return std::log10(theta);
	}

	static inline double ceil(double theta) noexcept
	{
		return std::ceil(theta);
	}

	static inline double fabs(double theta) noexcept
	{
		return std::fabs(theta);
	}

	static inline double floor(double theta) noexcept
	{
		return std::floor(theta);
	}

	static inline double modf(double x, double *y) noexcept
	{
		double d;
		double f = std::modf(x, &d);
		*y = d;
		return f;
	}

	static inline double fraction(double v) noexcept
	{
		double intPart;
		return modf(v, &intPart);
	}

	static inline double safeAcos(double x) noexcept
	{
		if (x <= -1.0) { return M_PI; }
		if (x >= 1.0) { return 0.0; }
		return std::acos(x);
	}

	static inline double snorm2unorm(double x) noexcept
	{
		return x * 0.5 + 0.5;
	}

	static inline double unorm2snorm(double x) noexcept
	{
		return x * 2.0 - 1.0;
	}
};

template<typename T>
inline bool equal(T a, T b) noexcept
{
    return _mathutil<T>::equal(a, b);
}

template<typename T>
inline void sinCos(T* returnSin, T* returnCos, T theta) noexcept
{
	_mathutil<T>::sinCos(returnSin, returnCos, theta);
}

template<typename T>
inline T rad2deg(T angle) noexcept
{
	return _mathutil<T>::rad2deg(angle);
}

template<typename T>
inline T deg2rad(T angle) noexcept
{
	return _mathutil<T>::deg2rad(angle);
}

template<typename T>
inline T abs(T t) noexcept
{
	return _mathutil<T>::abs(t);
}

template<typename T>
inline T exp(T x) noexcept
{
	return _mathutil<T>::exp(x);
}

template<typename T>
inline T exp2(T x) noexcept
{
	return _mathutil<T>::exp2(x);
}

template<typename T>
inline T fast_exp2(T x) noexcept
{
	return _mathutil<T>::fast_exp2(x);
}

template<typename T>
inline T cos(T theta) noexcept
{
	return _mathutil<T>::cos(theta);
}

template<typename T>
inline T sin(T theta) noexcept
{
	return _mathutil<T>::sin(theta);
}

template<typename T>
inline T tan(T theta) noexcept
{
	return _mathutil<T>::tan(theta);
}

template<typename T>
inline T atan2(T y, T x) noexcept
{
	return _mathutil<T>::atan2(y, x);
}

template<typename T>
inline T log(T theta) noexcept
{
	return _mathutil<T>::log(theta);
}

template<typename T>
inline T pow(T x, T y) noexcept
{
	return _mathutil<T>::pow(x, y);
}

template<typename T>
inline T sqrt(T x) noexcept
{
	return _mathutil<T>::sqrt(x);
}

template<typename T>
inline T rsqrt(T x) noexcept
{
	return _mathutil<T>::rsqrt(x);
}

template<typename T>
inline T acos(T theta) noexcept
{
	return _mathutil<T>::acos(theta);
}

template<typename T>
inline T asin(T theta) noexcept
{
	return _mathutil<T>::asin(theta);
}

template<typename T>
inline T atan(T theta) noexcept
{
	return _mathutil<T>::atan(theta);
}

template<typename T>
inline T cosh(T theta) noexcept
{
	return _mathutil<T>::cosh(theta);
}

template<typename T>
inline T sinh(T theta) noexcept
{
	return _mathutil<T>::sinh(theta);
}

template<typename T>
inline T tanh(T theta) noexcept
{
	return _mathutil<T>::tanh(theta);
}

template<typename T>
inline T log10(T theta) noexcept
{
	return _mathutil<T>::log10(theta);
}

template<typename T>
inline T ceil(T theta) noexcept
{
	return _mathutil<T>::ceil(theta);
}

template<typename T>
inline T fabs(T theta) noexcept
{
	return _mathutil<T>::fabs(theta);
}

template<typename T>
inline T floor(T theta) noexcept
{
	return _mathutil<T>::floor(theta);
}

template<typename T>
inline T modf(T x, T* y) noexcept
{
	return _mathutil<T>::modf(x, y);
}

template<typename T>
inline T fraction(T v) noexcept
{
	return _mathutil<T>::fraction(v);
}

template<typename T>
inline T safeAcos(T theta) noexcept
{
	return _mathutil<T>::safeAcos(theta);
}

template<typename T>
static inline T snorm2unorm(T x) noexcept
{
	return _mathutil<T>::snorm2unorm(x);
}

template<typename T>
static inline T unorm2snorm(T x) noexcept
{
	return _mathutil<T>::unorm2snorm(x);
}

inline float fpFromIEEE(std::uint32_t raw) noexcept
{
    return *reinterpret_cast<float*>(&raw);
}

inline std::uint32_t fpToIEEE(float fp) noexcept
{
    return *reinterpret_cast<std::uint32_t*>(&fp);
}

inline double fpFromIEEE(std::uint64_t raw) noexcept
{
    return *reinterpret_cast<double*>(&raw);
}

inline std::uint64_t fpToIEEE(double fp) noexcept
{
    return *reinterpret_cast<std::uint64_t*>(&fp);
}

template<typename T>
inline bool closeTo(T a, T b, T epsilon) noexcept
{
    if (a == b) return true;

    T diff = abs(a - b);
    if ((a == T(0.0) || b == T(0.0)) && (diff < epsilon))
        return true;

    return diff / (abs(a) + abs(b)) < epsilon;
}

template<typename T>
inline T GaussianDistribution(T x, T y, T r) noexcept
{
	T g = 1.0f / sqrt(2.0f * M_PI * r * r);
	g *= exp(-(x * x + y * y) / (2 * r * r));
	return g;
}

template<typename T>
inline void GaussianKernel(T weights[], T offsets[], std::size_t radius, T deviation, T size) noexcept
{
	for (std::size_t i = 0; i < radius; ++i)
		weights[i] = GaussianDistribution(-1.0f * i, 0.0f, radius / deviation);

	for (std::size_t i = 0; i < radius; ++i)
		weights[i] /= weights[0];

	for (std::size_t i = 0; i < radius; ++i)
		offsets[i] = i / size;
}

EXPORT void randomize();
EXPORT void randomize(unsigned int);
EXPORT int random(int min, int max);
EXPORT float random(float min, float max);
EXPORT double random(double min, double max);

EXPORT std::uint32_t morton2(std::uint32_t x, std::uint32_t y);
EXPORT std::uint32_t morton3(std::uint32_t x, std::uint32_t y, std::uint32_t z);

}

_NAME_END

#endif
