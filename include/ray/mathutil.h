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
#   define EPSILON_E4 (float)(1E-4)
#   define EPSILON_E5 (float)(1E-5)
#   define EPSILON_E6 (float)(1E-6)
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

inline bool equals(float a, float b)
{
    return fabs(b - a) < EPSILON ? true : false;
}

template<typename T>
inline T wrapPI(T theta)
{
    theta += M_PI;
    theta -= floor(theta * M_PI_2);
    theta -= M_PI;
    return theta;
}

template<typename T>
inline T max(T t1, T t2)
{
    return t1 > t2 ? t1 : t2;
}

template<typename T>
inline T min(T t1, T t2)
{
    return t1 < t2 ? t1 : t2;
}

template<typename T>
inline T middle(T t1, T t2, T t3)
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
inline T abs(T t)
{
    return t >= 0 ? t : -t;
}

inline float invSqrt(float x)
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float*)&i;
    x = x*(1.5f - xhalf*x*x);
    x = x*(1.5f - xhalf*x*x);
    return x;
}

inline double invSqrt(double y)
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

template<double& R>
class Sine
{
    enum { max_recursive = 10 };

    inline static double sin(double fRad)
    {
        return R * series(fRad, 0);
    }

    inline static double series(double fRad, int i)
    {
        if (i > max_recursive) { return 1.0; }

        return 1.0 - (fRad * fRad / (2.0 * i + 2.0) / (2.0 * i + 3.0) * series(fRad, i + 1));
    }
};

template<typename _Tx, typename _Ty>
inline void sinCos(_Tx* returnSin, _Tx* returnCos, _Ty theta)
{
    *returnSin = std::sin(theta);
    *returnCos = std::cos(theta);
}

template<typename T>
inline T safeAcos(T x)
{
    if (x <= -1.0) { return M_PI; }
    if (x >= 1.0) { return 0; }
    return std::acos(x);
}

template<typename T>
inline T radians(T angle)
{
    return angle / M_PI * static_cast<T>(180.0);
}

template<typename T>
inline T degrees(T angle)
{
    return angle * M_PI / static_cast<T>(180.0);
}

template<typename T>
inline T clamp(T t, T min, T max)
{
    return std::max(min, std::min(max, t));
}

template<typename T1, typename T2>
inline T1 lerp(T1 t1, T1 t2, T2 t3)
{
    return t1 + (t2 - t1) * t3;
}

template<typename T>
inline T smothlerp(T x, T x1, T x2, T q00, T q01)
{
    return ((x2 - x) / (x2 - x1)) * q00 + ((x - x1) / (x2 - x1)) * q01;
}

template<typename T>
inline T biLerp(T x, T y, T q11, T q12, T q21, T q22, T x1, T x2, T y1, T y2)
{
    float r1 = smothlerp(x, x1, x2, q11, q21);
    float r2 = smothlerp(x, x1, x2, q12, q22);

    return smothlerp(y, y1, y2, r1, r2);
}

template<typename T>
inline T triLerp(T x, T y, T z, T q000, T q001, T q010, T q011, T q100, T q101, T q110, T q111, T x1, T x2, T y1, T y2, T z1, T z2)
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
inline std::uint8_t bitScanReverse(T number)
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
inline T ceilToPowerOfTwo(T number)
{
    T input = number;
    T result = bitScanReverse(number);
    T test = ~(static_cast<T>(1) << result);

    return test & input;
}

template<typename T>
inline T fast_exp2(T x)
{
    static const T c[3] = { 5.79526, 12.82461, -2.88611 };

    int e = int(round(x));
    T t = x - e;
    T m = (t * t + c[0] * t + c[1]) / (c[2] * t * c[1]);

    return std::ldexp<T>(m, e);
}

inline constexpr int _hash_int_0(int key)
{
    return  ~key + (key << 15);
}

inline constexpr int _hash_int_1(int key)
{
    return key ^ (key >> 12);
}

inline constexpr int _hash_int_2(int key)
{
    return key + (key << 2);
}

inline constexpr int _hash_int_3(int key)
{
    return key ^ (key >> 4);
}

inline constexpr int _hash_int_4(int key)
{
    return key * 2057;
}

inline constexpr int _hash_int_5(int key)
{
    return key ^ (key >> 16);
}

inline constexpr int _hash_int(int key)
{
    return _hash_int_5(_hash_int_4(_hash_int_3(_hash_int_2(_hash_int_1(_hash_int_0(key))))));
}

inline constexpr int hash_int(int x, int y, int z)
{
    return _hash_int(x) ^ _hash_int(y) ^ _hash_int(z);
}

inline constexpr float hash_float(float x, float y, float z)
{
    return x + y * (1.0f / 255.0f) + z * (1.0f / 65025.0f);
}

inline constexpr double hash_double(double x, double y, double z)
{
    return x + y * (1.0f / 255.0f) + z * (1.0f / 65025.0f);
}

inline constexpr std::size_t _hash_string(unsigned hash, const char* str)
{
    return (!*str ? hash : _hash_string(((hash << 5) + hash) + *str, str + 1));
}

inline constexpr std::size_t hash_string(const char* str)
{
    return (!str ? 0 : _hash_string(5381, str));
}

inline float cosf(float arg)
{
    return std::cos(arg);
}

inline float sinf(float arg)
{
    return std::sin(arg);
}

inline float tanf(float arg)
{
    return std::tan(arg);
}

inline float atan2f(float y, float x)
{
    return std::atan2(y, x);
}

inline float logf(float arg)
{
    return std::log(arg);
}

inline float powf(float x, float y)
{
    return std::pow(x, y);
}

inline float sqrtf(float arg)
{
    return std::sqrt(arg);
}

inline float acosf(float arg)
{
    return std::acos(arg);
}

inline float asinf(float arg)
{
    return std::asin(arg);
}

inline float atanf(float arg)
{
    return std::atan(arg);
}

inline float coshf(float arg)
{
    return std::cosh(arg);
}

inline float sinhf(float arg)
{
    return std::sinh(arg);
}

inline float tanhf(float arg)
{
    return std::tanh(arg);
}

inline float log10f(float arg)
{
    return std::log10(arg);
}

inline float ceilf(float arg)
{
    return std::ceil(arg);
}

inline float fabsf(float arg)
{
    return std::fabs(arg);
}

inline float floorf(float arg)
{
    return std::floor(arg);
}

inline float modff(float x, float *y)
{
    double d;
    float f = (float)modf((double)x, &d);
    *y = (float)d;
    return f;
}

inline float fractionf(float v)
{
    float intPart;
    return modff(v, &intPart);
}

inline double fraction(double v)
{
    double intPart;
    return modf(v, &intPart);
}

inline float fpFromIEEE(std::uint32_t raw)
{
    return *reinterpret_cast<float*>(&raw);
}

inline std::uint32_t fpToIEEE(float fp)
{
    return *reinterpret_cast<std::uint32_t*>(&fp);
}

inline double fpFromIEEE(std::uint64_t raw)
{
    return *reinterpret_cast<double*>(&raw);
}

inline std::uint64_t fpToIEEE(double fp)
{
    return *reinterpret_cast<std::uint64_t*>(&fp);
}

inline bool closeTo(float a, float b, float epsilon)
{
    if (a == b) return true;

    float diff = fabs(a - b);
    if ((a == 0 || b == 0) && (diff < epsilon))
        return true;

    return diff / (fabs(a) + fabs(b)) < epsilon;
}

inline bool closeTo(double a, double b, double epsilon)
{
    if (a == b) return true;

    double diff = fabs(a - b);
    if ((a == 0 || b == 0) && (diff < epsilon))
        return true;

    return diff / (fabs(a) + fabs(b)) < epsilon;
}

inline float safeAcos(float x)
{
    if (x <= -1.0) { return M_PI; }
    if (x >= 1.0) { return 0.0; }
    return acos(x);
}

template<typename T>
inline T GaussianDistribution(T x, T y, T r)
{
	T g = 1.0f / sqrt(2.0f * M_PI * r * r);
	g *= exp(-(x * x + y * y) / (2 * r * r));
	return g;
}

template<typename T>
inline void GaussianKernel(std::vector<T>& weights, std::vector<T> offsets, std::size_t radius, T deviation, T size)
{
	weights.resize(radius);
	offsets.resize(radius);

	for (int i = 0; i < radius; ++i)
		weights[i] = GaussianDistribution(-i, 0.0f, radius / deviation);

	for (std::size_t i = 0; i < weights.size(); ++i)
		weights[i] /= weights[0];

	for (std::size_t i = 0; i < offsets.size(); ++i)
		offsets[i] = i / size;
}

EXPORT void randomize();
EXPORT int random(int min, int max);
EXPORT float random(float min, float max);
EXPORT double random(double min, double max);

EXPORT float fastSin(float theta);
EXPORT float fastCos(float theta);

EXPORT std::uint32_t morton2(std::uint32_t x, std::uint32_t y);
EXPORT std::uint32_t morton3(std::uint32_t x, std::uint32_t y, std::uint32_t z);

}

_NAME_END

#endif
