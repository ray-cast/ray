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
#ifndef _H_MATH_UTIL_H_
#define _H_MATH_UTIL_H_

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
inline static T max(T t1, T t2)
{
    return t1 > t2 ? t1 : t2;
}

template<typename T>
inline static T min(T t1, T t2)
{
    return t1 < t2 ? t1 : t2;
}

template<typename T>
inline static T middle(T t1, T t2, T t3)
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
inline static T abs(T t)
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

EXPORT float fastSin(float theta);
EXPORT float fastCos(float theta);

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

        return 1.0 - (fRad * fRad / (2.0 * i + 2.0) / (2.0 * i + 3.0) * series(fRad, ++i));
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

    int e = int(std::round(x));
    T t = x - e;
    T m = (t * t + c[0] * t + c[1]) / (c[2] * t * c[1]);

    return std::ldexp<T>(m, e);
}

EXPORT void randomize();
EXPORT int random(int min, int max);
EXPORT float random(float min, float max);
EXPORT double random(double min, double max);

inline std::uint32_t part1by2(std::uint32_t n)
{
    n = (n ^ (n << 16)) & 0xff0000ff;
    n = (n ^ (n << 8)) & 0x0300f00f;
    n = (n ^ (n << 4)) & 0x030c30c3;
    n = (n ^ (n << 2)) & 0x09249249;
    return n;
}

inline std::uint32_t morton(std::uint32_t x, std::uint32_t y)
{
    return (part1by2(y) << 1) + part1by2(x);
}

inline std::uint32_t morton(std::uint32_t x, std::uint32_t y, std::uint32_t z)
{
    return (part1by2(z) << 2) + (part1by2(y) << 1) + part1by2(x);
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
    return key ^ (key >> 4);;
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

/*
template<typename T>
class Rect2
{
    T left;
    T right;
    T top;
    T bottom;

    Rect2()
    {
        left = top = right = bottom = 0.0;
    }

    Rect2(T l, T t, T r, T b)
    {
        left = l;
        right = r;
        top = t;
        bottom = b;
    }

    void setRect(T _left, T _top, T _right, T _bottom)
    {
        left    = _left;
        top     = _top;
        right   = _right;
        bottom  = _bottom;
    }

    void setSize(T lWidth,T lHeight)
    {
        right = lWidth;
        bottom = lHeight;
    }

    void setPos(const Vector2f& pt)
    {
        left += pt.x;
        top += pt.y;
        right += pt.x;
        bottom += pt.y;
    }

    bool contains(const Vector2f& pt) const
    {
        return (pt.x <right && pt.x > left && pt.y > top && pt.y <bottom);
    };
};

template<typename T = float>
class Ray2
{
    Vector2t<T> origin;
    Vector2t<T> normal;

    Ray2()
    {
    }

    Ray2(const Vector2t<T>& pt, const Vector2t<T>& normal)
    {
        origin = pt;
        setNormal(normal);
    }

    void setNormal(const Vector2t<T>& n)
    {
        normal = n;
        normal.normalize();
    }

    Vector2t<T> getPoint(float distance) const
    {
        return origin + normal * distance;
    }

    Vector2t<T> closestPoint(const Vector2t<T>& pt) const
    {
        Vector2t<T> vector = pt - origin;

        float t = normal.dot(vector);

        if (t <static_cast<T>(0.0)) return origin;

        return origin + t * normal;
    }

    float sqrDistance(const Vector2t<T>& pt) const
    {
        Vector2t<T> ac = pt - origin;

        float t = normal.dot(ac);

        if (t <= static_cast<T>(0.0)) return ac.dot();

        float denom = normal.dot();

        return ac.dot() - t * t / denom;
    }
};

template<typename T = float>
class Line2
{
public:
    Vector2t<T> pos0;
    Vector2t<T> pos1;

    Line2() { }
    Line2(const Vector2t<T>& pt0, const Vector2t<T> pt1)
        :pos0(pt0)
        ,pos1(pt1) { }

    void set(const Vector2t<T>& start, const Vector2t<T>& end)
    {
        pos0 = start;
        pos1 = end;
    }

    Vector2t<T> size()   const { return pos1 - pos0; }
    Vector2t<T> normal() const { return size().normalize(); }

    Vector2t<T> closestPoint(const Vector2t<T>& pt) const
    {
        Vector2t<T> ab = pos1 - pos0;
        Vector2t<T> ac = pt - pos0;

        float t = ac.dot(ab);

        if (t <= static_cast<T>(0.0)) { return pos0; }

        float denom = ab.dot();

        if (t >= denom) { return pos1; }

        t = t / ab.dot();

        return pos0 + t * ab;
    }

    float sqrDistance(const Vector2t<T>& pt) const
    {
        Vector2t<T> ab = pos1 - pos0;
        Vector2t<T> ac = pt - pos0;

        float t = ac.dot(ab);

        if (t <= static_cast<T>(0.0)) return ac.dot();

        float denom = ab.dot();

        if (t >= denom)
        {
            Vector2t<T> bc = pt - pos1;

            return bc.dot();
        }

        return ac.dot() - t * t / denom;
    }
};

template<typename T = float>
class Plane2
{
public:
    Vector2t<T> normal;
    float    distance;

    Plane2()
    {
    }

    Plane2(const Plane2& copy)
        :normal(copy.normal)
        ,distance(copy.distance)
    {
    }

    Plane2(const Vector2t<T>& pt0, const Vector2t<T>& pt1)
    {
        compute(pt0, pt1);
    }

    Plane2(const Vector2t<T>& n, float dist)
    {
        setNormal(n);
        distance = dist;
    }

    void setNormal(const Vector2t<T> n)
    {
        normal = n;
        normal.normalize();
    }

    void setDistance(const Vector2t<T>& pt)
    {
        distance = -normal.dot(pt);
    }

    void compute(const Vector2t<T>& pt0, const Vector2t<T>& pt1)
    {
        setNormal(pt1 - pt0);
        setDistance(pt0);
    }

    float getDistance(const Vector2t<T>& pt) const
    {
        return normal.dot(pt) + distance;
    }

    Vector2t<T> project(const Vector2t<T>& pt) const
    {
        return pt - getDistance(pt) * normal;
    }
};

template<typename T=float>
class Torus
{
public:
    T radius;

    Vector2t<T> center;
};

template<typename T>
class CyrusBeck
{
public:
    void setScissor(const Line2<T>* pt, size_t n)
    {
        for (size_t i = 0; i <n; i++)
            lines_.push_back(*pt++);
    }

    bool scissor(const Line2<T>& line)
    {
        Vector2t<T> ray = line.normal();

        size_t count = lines_.size();
        for (size_t i = 0; i <count; i++)
        {
            Vector2t<T> ac = lines_[i].pos0 - line.pos0;
        }
    }

private:
    std::vector<Line2<T>> lines_;
};

class Morton
{
public:
    static size_t part1By1(size_t n)
    {
        n = (n^(n <<8)) & 0x00ff00ff;
        n = (n^(n <<4)) & 0x0f0f0f0f;
        n = (n^(n <<2)) & 0x33333333;
        n = (n^(n <<1)) & 0x55555555;
        return n;
    }

    static size_t part1By2(size_t n)
    {
        n = (n^(n <<16)) & 0xff0000ff;
        n = (n^(n << 8)) & 0x0300f00f;
        n = (n^(n << 4)) & 0x030c30c3;
        n = (n^(n << 2)) & 0x09249249;
        return n;
    }

    static size_t morton(size_t x, size_t y)
    {
        return (part1By1(y) <<1) + part1By1(x);
    }

    static size_t morton(size_t x, size_t y, size_t z)
    {
        return (part1By2(z) <<2) + (part1By2(y) <<1) + part1By2(x);
    }
};

template<typename T=float>
class Polygon
{
    bool isConvex(const Vector3t<T> pt[], unsigned int n) const
    {
        T angleSum = 0.0f;

        for (unsigned int i = 0; i <n; i++)
        {
            Vector3t<T> e1;
            if (i==0)
            {
                e1 = pt[n - 1] - pt[i];
            }
            else
            {
                e1 = pt[i-1] - pt[i];
            }

            Vector3t<T> e2;
            if (i == n-1)
            {
                e2 = pt[0] - pt[i];
            }
            else
            {
                e2 = pt[i + 1] - pt[i];
            }

            e1.normalize();
            e2.normalize();

            T d = e1.dot(e2);

            T theta = ::safeAcos(d);

            angleSum += theta;
        }

        T convexAngleSum = (T)(n - 2) * M_PI;

        if (angleSum <convexAngleSum - (T)n * 0.0001f)
        {
            return false;
        }

        return true;
    }
};

template<typename T=float>
class Triangle2
{
    Vector2t<T> v[3];

    Triangle2();
    Triangle2(const Vector2t<T>& a, const Vector2t<T>& b, const Vector2t<T>& c);

    float area() const;

    Vector2t<T> barycentric(const Vector2t<T>& pt) const;

    Vector2t<T> incenter() const;

    Vector2t<T> circumcentrer() const;
};

*/

_NAME_END

#endif
