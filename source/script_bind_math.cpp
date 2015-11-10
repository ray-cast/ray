// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/script_bind_math.h>
#include <ray/math.h>
#include <angelscript.h>

_NAME_BEGIN

template <typename T>
void construct(T * address)
{
	(*address) = T();
}

template <typename _T, typename _Tx, typename _Ty, typename _Tz>
void construct_3(_T * address, _Tx x, _Ty y, _Tz z)
{
	(*address) = _T(x, y, z);
}

template <typename _T, typename _Tx, typename _Ty, typename _Tz, typename _Tw>
void construct_4(_T * address, _Tx x, _Ty y, _Tz z, _Tw w)
{
	(*address) = _T(x, y, z, w);
}

template <typename T>
void destroy(T * object)
{
	object->~T();
}

template <typename T>
void copy_construct(T* address, T * other)
{
	(*address) = T(*other);
}

template <typename T>
void assign(T * lhs, T* rhs)
{
	*lhs = *rhs;
}

void ConstructFloat3(float3* thisPointer)
{
	*thisPointer = float3();
}

void ConstructFloat3Float3(const float x, const float y, const float z, float3* thisPointer)
{
	*thisPointer = float3(x, y, z);
}

void CopyConstructFloat3(const float3& other, float3 *thisPointer)
{
	*thisPointer = float3(other);
}

void DestructFloat3(float3* thisPointer)
{
	thisPointer->~Vector3();
}

float3 AddFloat3Float3(const float3& v1, const float3& v2)
{
	return v1 + v2;
}

float3 SubFloat3Float3(const float3& v1, const float3& v2)
{
	return v1 - v2;
}

float Float3GetX(float3& v)
{
	return v.x;
}

float Float3GetY(float3& v)
{
	return v.y;
}

float Float3GetZ(float3& v)
{
	return v.z;
}

void Float3SetX(float3& v, float value)
{
	v.x = value;
}

void Float3SetY(float3& v, float value)
{
	v.y = value;
}

void Float3SetZ(float3& v, float value)
{
	v.z = value;
}

float3 Float3MulFloat(const float3& v, float value)
{
	return float3(v) * value;
}

bool Float3Equals(const float3& v1, const float3& v2)
{
	return v1 == v2;
}

float Float4GetX(float4& v)
{
	return v.x;
}

float Float4GetY(float4& v)
{
	return v.y;
}

float Float4GetZ(float4& v)
{
	return v.z;
}

float Float4GetW(float4& v)
{
	return v.w;
}

void Float4SetX(float4& v, float value)
{
	v.x = value;
}

void Float4SetY(float4& v, float value)
{
	v.y = value;
}

void Float4SetZ(float4& v, float value)
{
	v.z = value;
}

void Float4SetW(float4& v, float value)
{
	v.w = value;
}

void makeRotate(float3x3& m, float angle, const float3& axis)
{
	m.makeRotate(angle, axis);
}

float3 MulFloat3x3Float3(const float3x3& m, const float3& v)
{
	return m * v;
}

template<typename T>
T script_abs(T x)
{
	return std::abs(x);
}

void
ScriptBindMath::setup(asIScriptEngine* engine) noexcept
{
	int r;

	r = engine->RegisterGlobalFunction("float fpFromIEEE(uint)", asFUNCTIONPR(fpFromIEEE, (std::uint32_t), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("uint fpToIEEE(float)", asFUNCTIONPR(fpToIEEE, (float), std::uint32_t), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("double fpFromIEEE(uint64)", asFUNCTIONPR(fpFromIEEE, (std::uint64_t), double), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("uint64 fpToIEEE(double)", asFUNCTIONPR(fpToIEEE, (double), std::uint64_t), asCALL_CDECL); assert(r >= 0);

	// Close to comparison with epsilon
	r = engine->RegisterGlobalFunction("bool closeTo(float, float, float = 0.00001f)", asFUNCTIONPR(closeTo, (float, float, float), bool), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("bool closeTo(double, double, double = 0.0000000001)", asFUNCTIONPR(closeTo, (double, double, double), bool), asCALL_CDECL); assert(r >= 0);

	// Trigonometric functions
	r = engine->RegisterGlobalFunction("float cos(float)", asFUNCTIONPR(cosf, (float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float sin(float)", asFUNCTIONPR(sinf, (float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float tan(float)", asFUNCTIONPR(tanf, (float), float), asCALL_CDECL); assert(r >= 0);

	r = engine->RegisterGlobalFunction("float acos(float)", asFUNCTIONPR(acosf, (float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float asin(float)", asFUNCTIONPR(asinf, (float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float atan(float)", asFUNCTIONPR(atanf, (float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float atan2(float,float)", asFUNCTIONPR(atan2f, (float, float), float), asCALL_CDECL); assert(r >= 0);

	// Hyberbolic functions
	r = engine->RegisterGlobalFunction("float cosh(float)", asFUNCTIONPR(coshf, (float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float sinh(float)", asFUNCTIONPR(sinhf, (float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float tanh(float)", asFUNCTIONPR(tanhf, (float), float), asCALL_CDECL); assert(r >= 0);

	// Exponential and logarithmic functions
	r = engine->RegisterGlobalFunction("float log(float)", asFUNCTIONPR(logf, (float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float log10(float)", asFUNCTIONPR(log10f, (float), float), asCALL_CDECL); assert(r >= 0);

	// Power functions
	r = engine->RegisterGlobalFunction("float pow(float, float)", asFUNCTIONPR(powf, (float, float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float sqrt(float)", asFUNCTIONPR(sqrtf, (float), float), asCALL_CDECL); assert(r >= 0);

	// Nearest integer, absolute value, and remainder functions
	r = engine->RegisterGlobalFunction("float ceil(float)", asFUNCTIONPR(ceilf, (float), float), asCALL_CDECL); assert(r >= 0);

	r = engine->RegisterGlobalFunction("int min(int, int)", asFUNCTIONPR(min<int>, (int, int), int), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float min(float, float)", asFUNCTIONPR(min<float>, (float, float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("double min(double, double)", asFUNCTIONPR(min<double>, (double, double), double), asCALL_CDECL); assert(r >= 0);

	r = engine->RegisterGlobalFunction("int max(int, int)", asFUNCTIONPR(max<int>, (int, int), int), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float max(float, float)", asFUNCTIONPR(max<float>, (float, float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("double max(double, double)", asFUNCTIONPR(max<double>, (double, double), double), asCALL_CDECL); assert(r >= 0);

	r = engine->RegisterGlobalFunction("int abs(int)", asFUNCTIONPR(abs<int>, (int), int), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float abs(float)", asFUNCTIONPR(abs<int>, (int), int), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("double abs(double)", asFUNCTIONPR(abs<double>, (double), double), asCALL_CDECL); assert(r >= 0);

	r = engine->RegisterGlobalFunction("float floor(float)", asFUNCTIONPR(floorf, (float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float fraction(float)", asFUNCTIONPR(fractionf, (float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float clamp(float t, float min, float max)", asFUNCTIONPR(clamp, (float, float, float), float), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("float lerp(float t1, float t2, float t3)", asFUNCTIONPR(lerp, (float, float, float), float), asCALL_CDECL); assert(r >= 0);

	// random
	r = engine->RegisterGlobalFunction("void simplex_seed(int)", asFUNCTION(simplex_seed), asCALL_CDECL);
	r = engine->RegisterGlobalFunction("float simplex2(float x, float y, int octaves, float persistence, float lacunarity)", asFUNCTION(simplex2), asCALL_CDECL);
	r = engine->RegisterGlobalFunction("float simplex3(float x, float y, float z, int octaves, float persistence, float lacunarity)", asFUNCTION(simplex3), asCALL_CDECL);

	// float3 class
	r = engine->RegisterObjectType("float3", sizeof(float3), asOBJ_VALUE | asGetTypeTraits<float3>()); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<float3>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3", asBEHAVE_CONSTRUCT, "void f(float x, float y, float z)", asFUNCTION((construct_3<float3, float, float, float>)), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3", asBEHAVE_CONSTRUCT, "void f(const float3& in)", asFUNCTION(copy_construct<float3>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<float3>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opAssign(const float3& in)", asMETHODPR(float3, operator =, (const float3&), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opAddAssign(const float3& in)", asMETHODPR(float3, operator +=, (const float3&), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opSubAssign(const float3& in)", asMETHODPR(float3, operator -=, (const float3&), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opMulAssign(const float3& in)", asMETHODPR(float3, operator *=, (const float3&), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opDivAssign(const float3& in)", asMETHODPR(float3, operator /=, (const float3&), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "bool opEquals(const float3 &in) const", asFUNCTIONPR(Float3Equals, (const float3&, const float3&), bool), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3 opMul(float)", asFUNCTION(Float3MulFloat), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opMulAssign(const float value)", asMETHODPR(float3, operator *=, (const float), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opDivAssign(const float value)", asMETHODPR(float3, operator /=, (const float), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3 opAdd(const float3& in) const", asFUNCTION(AddFloat3Float3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3 opSub(const float3& in) const", asFUNCTION(SubFloat3Float3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float get_x() const", asFUNCTION(Float3GetX), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float get_y() const", asFUNCTION(Float3GetY), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float get_z() const", asFUNCTION(Float3GetZ), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "void set_x(float value)", asFUNCTION(Float3SetX), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "void set_y(float value)", asFUNCTION(Float3SetY), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "void set_z(float value)", asFUNCTION(Float3SetZ), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float dot(const float3& in) const", asMETHOD(float3, dot), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3 cross(const float3& in) const", asMETHOD(float3, cross), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float normalize()", asMETHOD(float3, normalize), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float length() const", asMETHOD(float3, length), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float length2() const", asMETHOD(float3, length2), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float distance() const", asMETHOD(float3, distance), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float sqrDistance() const", asMETHOD(float3, sqrDistance), asCALL_THISCALL); assert(r >= 0);

	// float4 class
	r = engine->RegisterObjectType("float4", sizeof(float4), asOBJ_VALUE | asGetTypeTraits<float4>()); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<float4>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4", asBEHAVE_CONSTRUCT, "void f(float x, float y, float z, float w)", asFUNCTION((construct_4<float4, float, float, float, float>)), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4", asBEHAVE_CONSTRUCT, "void f(const float3& in)", asFUNCTION(copy_construct<float4>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<float4>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float get_x() const", asFUNCTION(Float4GetX), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float get_y() const", asFUNCTION(Float4GetY), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float get_z() const", asFUNCTION(Float4GetZ), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float get_w() const", asFUNCTION(Float4GetW), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "void set_x(float value)", asFUNCTION(Float4SetX), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "void set_y(float value)", asFUNCTION(Float4SetY), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "void set_z(float value)", asFUNCTION(Float4SetZ), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "void set_w(float value)", asFUNCTION(Float4SetW), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	
	// float3x3
	r = engine->RegisterObjectType("float3x3", sizeof(float3x3), asOBJ_VALUE | asGetTypeTraits<float3x3>()); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3x3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<float3x3>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3x3", asBEHAVE_CONSTRUCT, "void f(const float3x3& in)", asFUNCTION(copy_construct<float3x3>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3x3", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<float3x3>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3x3", "float3 opMul(const float3& in) const", asFUNCTION(MulFloat3x3Float3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3x3", "void makeRotate(float angle, const float3& in)", asFUNCTION(makeRotate), asCALL_CDECL_OBJFIRST); assert(r >= 0);

	// float 4x4

	r = engine->RegisterObjectType("float4x4", sizeof(float4x4), asOBJ_VALUE | asGetTypeTraits<float4x4>()); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4x4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<float4x4>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4x4", asBEHAVE_CONSTRUCT, "void f(const float4x4& in)", asFUNCTION(copy_construct<float4x4>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4x4", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<float4x4>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4x4", "float4x4& makeTransform(const float3& in translate, const float3& in rotate = float3(0.0, 0.0, 0.0), const float3& in scale = float3(1.0, 1.0, 1.0))", asMETHOD(float4x4, makeTransform), asCALL_THISCALL);
}

_NAME_END