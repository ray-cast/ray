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
#if defined(_BUILD_SCRIPT)
#include <ray/script_bind_math.h>

_NAME_BEGIN

template <typename T>
void construct(T* address)
{
	new(address) T();
}

template <typename _T, typename _Tx, typename _Ty>
void construct_2(_T * address, _Tx x, _Ty y)
{
	new(address) _T(x, y);
}

template <typename _T, typename _Tx, typename _Ty, typename _Tz>
void construct_3(_T * address, _Tx x, _Ty y, _Tz z)
{
	new(address) _T(x, y, z);
}

template <typename _T, typename _Tx, typename _Ty, typename _Tz, typename _Tw>
void construct_4(_T * address, _Tx x, _Ty y, _Tz z, _Tw w)
{
	new(address) _T(x, y, z, w);
}

template <typename T>
void copy_construct(T* address, const T& other)
{
	new(address) T(other);
}

template <typename T>
void destroy(T * object)
{
	object->~T();
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

	// float2 class
	r = engine->RegisterObjectType("float2", sizeof(float2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_PRIMITIVE); assert(r >= 0);
	r = engine->RegisterObjectProperty("float2", "float x", asOFFSET(float2, x)); assert(r >= 0);
	r = engine->RegisterObjectProperty("float2", "float y", asOFFSET(float2, y)); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float2", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<float2>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float2", asBEHAVE_CONSTRUCT, "void f(float x, float y)", asFUNCTION((construct_2<float2, float, float>)), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float2", asBEHAVE_CONSTRUCT, "void f(const float2& in)", asFUNCTION(copy_construct<float2>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float2& opAssign(const float2& in)", asMETHODPR(float2, operator =, (const float2&), float2&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float2& opAddAssign(const float2& in)", asMETHODPR(float2, operator +=, (const float2&), float2&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float2& opSubAssign(const float2& in)", asMETHODPR(float2, operator -=, (const float2&), float2&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float2& opMulAssign(const float2& in)", asMETHODPR(float2, operator *=, (const float2&), float2&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float2& opDivAssign(const float2& in)", asMETHODPR(float2, operator /=, (const float2&), float2&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "bool opEquals(const float2 &in) const", asFUNCTIONPR(operator==, (const float2&, const float2&), bool), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float2 opMul(float)", asFUNCTIONPR(operator*, (const float2&, float), float2), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float2& opMulAssign(const float value)", asMETHODPR(float2, operator *=, (const float), float2&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float2& opDivAssign(const float value)", asMETHODPR(float2, operator /=, (const float), float2&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float2 opAdd(const float2& in) const", asFUNCTIONPR(operator+, (const float2&, const float2&), float2), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float2 opSub(const float2& in) const", asFUNCTIONPR(operator-, (const float2&, const float2&), float2), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float dot(const float2& in) const", asMETHOD(float2, dot), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float normalize()", asMETHOD(float2, normalize), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float length() const", asMETHOD(float2, length), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float length2() const", asMETHOD(float2, length2), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float distance() const", asMETHOD(float2, distance), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float2", "float sqrDistance() const", asMETHOD(float2, sqrDistance), asCALL_THISCALL); assert(r >= 0);

	// float3 class
	r = engine->RegisterObjectType("float3", sizeof(float3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_PRIMITIVE); assert(r >= 0);
	r = engine->RegisterObjectProperty("float3", "float x", asOFFSET(float3, x)); assert(r >= 0);
	r = engine->RegisterObjectProperty("float3", "float y", asOFFSET(float3, y)); assert(r >= 0);
	r = engine->RegisterObjectProperty("float3", "float z", asOFFSET(float3, z)); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<float3>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION((construct_3<float3, float, float, float>)), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3", asBEHAVE_CONSTRUCT, "void f(const float3& in)", asFUNCTION(copy_construct<float3>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opAssign(const float3& in)", asMETHODPR(float3, operator =, (const float3&), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opAddAssign(const float3& in)", asMETHODPR(float3, operator +=, (const float3&), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opSubAssign(const float3& in)", asMETHODPR(float3, operator -=, (const float3&), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opMulAssign(const float3& in)", asMETHODPR(float3, operator *=, (const float3&), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opDivAssign(const float3& in)", asMETHODPR(float3, operator /=, (const float3&), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opMulAssign(const float value)", asMETHODPR(float3, operator *=, (const float), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3& opDivAssign(const float value)", asMETHODPR(float3, operator /=, (const float), float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3 opMul(float)", asFUNCTIONPR(operator*, (const float3&, float), float3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3 opAdd(const float3& in) const", asFUNCTIONPR(operator+, (const float3&, const float3&), float3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3 opSub(const float3& in) const", asFUNCTIONPR(operator-, (const float3&, const float3&), float3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "bool opEquals(const float3 &in) const", asFUNCTIONPR(operator==, (const float3&, const float3&), bool), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float dot(const float3& in) const", asMETHOD(float3, dot), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float3 cross(const float3& in) const", asMETHOD(float3, cross), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float normalize()", asMETHOD(float3, normalize), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float length() const", asMETHOD(float3, length), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float length2() const", asMETHOD(float3, length2), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float distance() const", asMETHOD(float3, distance), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3", "float sqrDistance() const", asMETHOD(float3, sqrDistance), asCALL_THISCALL); assert(r >= 0);

	// float4 class
	r = engine->RegisterObjectType("float4", sizeof(float4), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_PRIMITIVE); assert(r >= 0);
	r = engine->RegisterObjectProperty("float4", "float x", asOFFSET(float4, x)); assert(r >= 0);
	r = engine->RegisterObjectProperty("float4", "float y", asOFFSET(float4, y)); assert(r >= 0);
	r = engine->RegisterObjectProperty("float4", "float z", asOFFSET(float4, z)); assert(r >= 0);
	r = engine->RegisterObjectProperty("float4", "float w", asOFFSET(float4, z)); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<float4>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION((construct_4<float4, float, float, float, float>)), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4", asBEHAVE_CONSTRUCT, "void f(const float3& in)", asFUNCTION(copy_construct<float4>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float4& opAssign(const float4& in)", asMETHODPR(float4, operator =, (const float4&), float4&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float4& opAddAssign(const float4& in)", asMETHODPR(float4, operator +=, (const float4&), float4&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float4& opSubAssign(const float4& in)", asMETHODPR(float4, operator -=, (const float4&), float4&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float4& opMulAssign(const float4& in)", asMETHODPR(float4, operator *=, (const float4&), float4&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float4& opDivAssign(const float4& in)", asMETHODPR(float4, operator /=, (const float4&), float4&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float dot() const", asMETHODPR(float4, dot, (void) const, float), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float dot(const float4& in) const", asMETHODPR(float4, dot, (const float4&) const, float), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float normalize()", asMETHOD(float4, normalize), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float length() const", asMETHOD(float4, length), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "float length2() const", asMETHOD(float4, length2), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "void cross(const float4& in) const", asMETHODPR(float4, cross, (const float4&), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4", "void cross(const float4& in, const float4& in) const", asMETHODPR(float4, cross, (const float4&, const float4&), void), asCALL_THISCALL); assert(r >= 0);
	
	// float3x3
	r = engine->RegisterObjectType("float3x3", sizeof(float3x3), asOBJ_VALUE | asGetTypeTraits<float3x3>()); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3x3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<float3x3>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3x3", asBEHAVE_CONSTRUCT, "void f(const float3x3& in)", asFUNCTION(copy_construct<float3x3>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float3x3", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<float3x3>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3x3", "float3 opMul(const float3& in) const", asFUNCTIONPR(operator*, (const float3&, const float3&), float3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float3x3", "float3x3& makeRotate(float, const float3& in)", asMETHODPR(float3x3, makeRotate, (float, const float3&), float3x3&), asCALL_THISCALL); assert(r >= 0);

	// float 4x4
	r = engine->RegisterObjectType("float4x4", sizeof(float4x4), asOBJ_VALUE | asGetTypeTraits<float4x4>()); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4x4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<float4x4>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4x4", asBEHAVE_CONSTRUCT, "void f(const float4x4& in)", asFUNCTION(copy_construct<float4x4>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("float4x4", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<float4x4>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("float4x4", "float4x4& makeTransform(const float3& in, const float3& in = float3(0.0, 0.0, 0.0), const float3& in = float3(1.0, 1.0, 1.0))", asMETHOD(float4x4, makeTransform), asCALL_THISCALL);

	// bound
	r = engine->RegisterObjectType("BoundingBox", sizeof(Bound), asOBJ_VALUE | asGetTypeTraits<Bound>()); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("BoundingBox", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<Bound>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("BoundingBox", asBEHAVE_CONSTRUCT, "void f(const BoundingBox& in)", asFUNCTION(copy_construct<Bound>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("BoundingBox", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<Bound>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("BoundingBox", "float3 get_size() const", asMETHOD(Bound, size), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("BoundingBox", "float3 get_extents() const", asMETHOD(Bound, extents), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("BoundingBox", "const float3& get_center() const", asMETHODPR(Bound, center, () const, const float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("BoundingBox", "void set_center(const float3& in)", asMETHODPR(Bound, center, (const float3&), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("BoundingBox", "const float3& get_min() const", asMETHODPR(Bound, min, () const, const float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("BoundingBox", "const float3& get_max() const", asMETHODPR(Bound, max, () const, const float3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("BoundingBox", "bool empty() const", asMETHOD(Bound, empty), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("BoundingBox", "void reset() const", asMETHOD(Bound, reset), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("BoundingBox", "float radius() const", asMETHOD(Bound, radius), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("BoundingBox", "void encapsulate(const float3& in) const", asMETHODPR(Bound, encapsulate, (const float3&), void), asCALL_THISCALL); assert(r >= 0);
}

_NAME_END
#endif