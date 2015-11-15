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
#include <ray/script_bind_actor.h>
#include <ray/script_bind_array.h>
#include <ray/script_array.h>

_NAME_BEGIN

template <typename T>
void construct(void* address)
{
	new (address)T;
}

template <typename _Tx, typename _Ty>
void construct_1(void * address, _Ty arg)
{
	new (address)_Tx(arg);
}

template <typename T>
void destroy(T * object)
{
	object->~T();
}

template <typename T>
void copy_construct(void * address, T * other)
{
	new (address)T(*other);
}

template <typename T>
void assign(T * lhs, T* rhs)
{
	*lhs = *rhs;
}

template <typename T>
bool ImplConv(T* t)
{
	assert(t);
	return *t ? true : false;
}

template <typename T>
bool ImplCast(T* t)
{
	assert(t);
	return *t ? true : false;
}

GameObjectPtr instantiate(const std::string& name)
{
	auto object = GameObject::find<GameObject>(name);
	if (object)
		return object->clone();
	return nullptr;
}

void setVertexArray(MeshPropertyPtr& mesh, ScriptArray& array)
{
	auto& vertices = mesh->getVertexArray();
	vertices.resize(array.count());
	std::memcpy(vertices.data(), array.data(), array.count() * sizeof(float3));
}

void setNormalArray(MeshPropertyPtr& mesh, const ScriptArray& array)
{
	auto& normals = mesh->getNormalArray();
	normals.resize(array.count());
	std::memcpy(normals.data(), array.data(), array.count() * sizeof(float3));
}

void setTexcoordArray(MeshPropertyPtr& mesh, const ScriptArray& array)
{
	auto& texcoords = mesh->getVertexArray();
	texcoords.resize(array.count());
	std::memcpy(texcoords.data(), array.data(), array.count() * sizeof(float2));
}

void setColorArray(MeshPropertyPtr& mesh, const ScriptArray& array)
{
	auto& colors = mesh->getVertexArray();
	colors.resize(array.count());
	std::memcpy(colors.data(), array.data(), array.count() * sizeof(float4));
}

void setFaceArray(MeshPropertyPtr& mesh, const ScriptArray& array)
{
	auto& faces = mesh->getFaceArray();
	faces.resize(array.count());
	std::memcpy(faces.data(), array.data(), faces.size() * sizeof(uint));
}

ScriptBindActor::ScriptBindActor() noexcept
{
}

ScriptBindActor::~ScriptBindActor() noexcept
{
}

void
ScriptBindActor::setup(asIScriptEngine* engine) noexcept
{
	assert(engine);

	int r = 0;

	r = engine->RegisterInterface("IController"); assert(r >= 0);

	r = engine->RegisterObjectType("Mesh", sizeof(MeshPropertyPtr), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Mesh", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<MeshPropertyPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Mesh", asBEHAVE_CONSTRUCT, "void f(const Mesh& in)", asFUNCTION(copy_construct<MeshPropertyPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Mesh", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<MeshPropertyPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void setName(const string& in)", CALLER(MeshProperty, setName), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "const string& getName() const", CALLER(MeshProperty, getName), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void setParent(Mesh& in)", CALLER(MeshProperty, setParent), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "const Mesh getParent() const", CALLER(MeshProperty, getParent), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void addChild(Mesh& in)", CALLER(MeshProperty, addChild), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void removeChild(Mesh& in) const", CALLER(MeshProperty, removeChild), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void cleanupChildren() const", CALLER(MeshProperty, cleanupChildren), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "Mesh findChild(const string& in, bool recurse = true)", CALLER(MeshProperty, findChild), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "uint getChildCount() const", CALLER(MeshProperty, getChildCount), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void mergeVertices()", CALLER(MeshProperty, mergeMeshes), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "uint getNumVertices() const", CALLER(MeshProperty, getNumVertices), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "uint getNumIndices() const", CALLER(MeshProperty, getNumIndices), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void computeBoundingBox()", CALLER(MeshProperty, computeBoundingBox), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void computeFaceNormals()", CALLER(MeshProperty, computeFaceNormals), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void computeVertexNormals()", CALLER_PR(MeshProperty, computeVertexNormals, (), void), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void computeMorphNormals()", CALLER(MeshProperty, computeMorphNormals), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void computeTangents()", CALLER(MeshProperty, computeTangents), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void setVertexArray(const vector<float3>& in)", asFUNCTION(setVertexArray), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void setNormalArray(const vector<float3>& in)", asFUNCTION(setNormalArray), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void setColorArray(const vector<float4>& in)", asFUNCTION(setColorArray), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void setTangentArray(const vector<float3>& in)", asFUNCTION(setVertexArray), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void setTexcoordArray(const vector<float2>& in)", asFUNCTION(setTexcoordArray), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("Mesh", "void setFaceArray(const vector<uint>& in)", asFUNCTION(setFaceArray), asCALL_CDECL_OBJFIRST); assert(r >= 0);

	r = engine->RegisterObjectType("CombineMesh", sizeof(CombineInstance::Instance), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineMesh", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<CombineInstance::Instance>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineMesh", asBEHAVE_CONSTRUCT, "void f(const CombineMesh& in)", asFUNCTION(copy_construct<CombineInstance::Instance>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineMesh", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<CombineInstance::Instance>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("CombineMesh", "void setMesh(Mesh& in)", asMETHOD(CombineInstance::Instance, setMesh), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("CombineMesh", "void setTransform(const float4x4& in)", asMETHOD(CombineInstance::Instance, setTransform), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("CombineMesh", "void setTransform(const float3& in, const float3& in = float3(0,0,0), const float3& in = float3(1,1,1))", asMETHOD(CombineInstance::Instance, makeTransform), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("CombineMesh", "Mesh getMesh()", asMETHOD(CombineInstance::Instance, getMesh), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("CombineMesh", "const float4x4& getTransform()", asMETHOD(CombineInstance::Instance, getTransform), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectType("CombineInstance", sizeof(CombineInstance), asOBJ_VALUE | asOBJ_APP_CLASS_CDK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineInstance", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<CombineInstance>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineInstance", asBEHAVE_CONSTRUCT, "void f(int i)", asFUNCTION((construct_1<CombineInstance, int>)), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineInstance", asBEHAVE_CONSTRUCT, "void f(const CombineInstance& in)", asFUNCTION(copy_construct<CombineInstance>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineInstance", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<CombineInstance>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("CombineInstance", "CombineMesh& opIndex(uint)", asMETHOD(CombineInstance, at), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("CombineInstance", "const CombineMesh& opIndex(uint) const", asMETHOD(CombineInstance, at), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectType("MeshFilter", sizeof(MeshComponentPtr), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("MeshFilter", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<MeshComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("MeshFilter", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<MeshComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshFilter", "MeshFilter& opAssign(const MeshFilter& in)", asFUNCTION(assign<MeshComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshFilter", "bool opImplConv() const", asFUNCTION(ImplConv<MeshComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("MeshFilter", "bool opImplCast() const", asFUNCTION(ImplCast<MeshComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("MeshFilter", "bool opConv() const", asFUNCTION(ImplConv<MeshComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("MeshFilter", "bool opCast() const", asFUNCTION(ImplCast<MeshComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("MeshFilter", "void setName(const string& in)", CALLER(MeshComponent, setName), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshFilter", "const string& getName() const", CALLER(MeshComponent, getName), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshFilter", "void setMesh(Mesh)", CALLER(MeshComponent, setMesh), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshFilter", "Mesh getMesh() const", CALLER(MeshComponent, getMesh), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshFilter", "void setCombieInstnace(const CombineInstance& in)", CALLER(MeshComponent, setCombieInstnace), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshFilter", "const BoundingBox& getBoundingBox() const", CALLER(MeshComponent, getBoundingBox), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshFilter", "const BoundingBox& getBoundingBoxDownwards() const", CALLER(MeshComponent, getBoundingBoxDownwards), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	

	r = engine->RegisterObjectType("PhysicsRigidbody", sizeof(PhysicsBodyComponentPtr), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("PhysicsRigidbody", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<PhysicsBodyComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("PhysicsRigidbody", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<PhysicsBodyComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "PhysicsRigidbody& opAssign(const PhysicsRigidbody& in)", asFUNCTION(assign<PhysicsBodyComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "bool opConv() const", asFUNCTION(ImplConv<PhysicsBodyComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "bool opCast() const", asFUNCTION(ImplCast<PhysicsBodyComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "bool opImplConv() const", asFUNCTION(ImplConv<PhysicsBodyComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "bool opImplCast() const", asFUNCTION(ImplCast<PhysicsBodyComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void setMass(float value)", CALLER(PhysicsBodyComponent, setMass), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void setLinearVelocity(const float3& in)", CALLER(PhysicsBodyComponent, setLinearVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void setAngularVelocity(const float3& in)", CALLER(PhysicsBodyComponent, setAngularVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void setLinearDamping(float value) ", CALLER(PhysicsBodyComponent, setLinearDamping), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void setAngularDamping(float value)", CALLER(PhysicsBodyComponent, setAngularDamping), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void setGravity(const float3& in)", CALLER(PhysicsBodyComponent, setGravity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void setConstantForce(const float3& in)", CALLER(PhysicsBodyComponent, setConstantForce), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void setConstantTorque(const float3& in)", CALLER(PhysicsBodyComponent, setConstantTorque), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void setConstantVelocity(const float3& in)", CALLER(PhysicsBodyComponent, setConstantVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void setConstanAngularVelocity(const float3& in)", CALLER(PhysicsBodyComponent, setConstanAngularVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void sleep(bool sleep)", CALLER(PhysicsBodyComponent, sleep), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "bool isSleep() const", CALLER(PhysicsBodyComponent, isSleep), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "float getMass() const", CALLER(PhysicsBodyComponent, getMass), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "float getLinearDamping() const", CALLER(PhysicsBodyComponent, getLinearDamping), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "float getAngularDamping() const", CALLER(PhysicsBodyComponent, getAngularDamping), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "const float3& getGravity() const", CALLER(PhysicsBodyComponent, getGravity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "const float3& getLinearVelocity() const", CALLER(PhysicsBodyComponent, getLinearVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "const float3& getAngularVelocity() const", CALLER(PhysicsBodyComponent, getAngularVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "const float3& getConstantForce() const", CALLER(PhysicsBodyComponent, getConstantForce), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "const float3& getConstantTorque() const", CALLER(PhysicsBodyComponent, getConstantTorque), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "const float3& getConstantVelocity() const", CALLER(PhysicsBodyComponent, getConstantVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "const float3& getConstantAngularVelocity() const", CALLER(PhysicsBodyComponent, getConstantAngularVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void addForce(const float3& in)", CALLER(PhysicsBodyComponent, addForce), asCALL_CDECL_OBJFIRST); assert(r > 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void addRelativeForce(const float3& in, const float3& in)", CALLER(PhysicsBodyComponent, addRelativeForce), asCALL_CDECL_OBJFIRST); assert(r > 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void addTorque(const float3& in)", CALLER(PhysicsBodyComponent, addTorque), asCALL_CDECL_OBJFIRST); assert(r > 0);
	r = engine->RegisterObjectMethod("PhysicsRigidbody", "void addImpulse(const float3& in, const float3& in)", CALLER(PhysicsBodyComponent, addImpulse), asCALL_CDECL_OBJFIRST); assert(r > 0);

	r = engine->RegisterObjectType("PhysicsCharacter", sizeof(PhysicsCharacterComponentPtr), asOBJ_VALUE | asOBJ_ASHANDLE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("PhysicsCharacter", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<PhysicsCharacterComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("PhysicsCharacter", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<PhysicsCharacterComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsCharacter", "PhysicsCharacter& opAssign(const PhysicsCharacter& in)", asFUNCTION(assign<PhysicsCharacterComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsCharacter", "bool opImplConv() const", asFUNCTION(ImplConv<PhysicsCharacterComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("PhysicsCharacter", "bool opImplCast() const", asFUNCTION(ImplCast<PhysicsCharacterComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("PhysicsCharacter", "bool opConv() const", asFUNCTION(ImplConv<PhysicsCharacterComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("PhysicsCharacter", "bool opCast() const", asFUNCTION(ImplCast<PhysicsCharacterComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("PhysicsCharacter", "void setWalkDirection(const float3& in)", CALLER(PhysicsCharacterComponent, setWalkDirection), asCALL_CDECL_OBJFIRST); assert(r > 0);
	r = engine->RegisterObjectMethod("PhysicsCharacter", "void jump(float)", CALLER(PhysicsCharacterComponent, jump), asCALL_CDECL_OBJFIRST); assert(r > 0);
	r = engine->RegisterObjectMethod("PhysicsCharacter", "bool canJumping() const", CALLER(PhysicsCharacterComponent, canJumping), asCALL_CDECL_OBJFIRST); assert(r > 0);

	r = engine->RegisterObjectType("GameObject", sizeof(GameObjectPtr), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GameObject", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<GameObjectPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GameObject", asBEHAVE_CONSTRUCT, "void f(const GameObject& in)", asFUNCTION(copy_construct<GameObjectPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GameObject", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<GameObjectPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "GameObject& opAssign(const GameObject& in)", asFUNCTION(assign<GameObjectPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "bool opImplConv() const", asFUNCTION(ImplConv<GameObjectPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("GameObject", "void setName(const string& in)", CALLER(GameObject, setName), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "const string& getName() const", CALLER(GameObject, getName), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "void setActive(bool)", CALLER(GameObject, setActive), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "bool getActive() const", CALLER(GameObject, getActive), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "void setTranslate(const float3& in)", CALLER(GameObject, setTranslate), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "void setLookat(const float3& in)", CALLER(GameObject, setLookAt), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "void setUp(const float3& in)", CALLER(GameObject, setUpVector), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "const float3& getTranslate() const", CALLER(GameObject, getTranslate), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "const float3& getLookat() const", CALLER(GameObject, getLookAt), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "const float3& getUp() const", CALLER(GameObject, getUpVector), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "GameObject clone() const", CALLER(GameObject, clone), asCALL_CDECL_OBJFIRST); assert(r >= 0);

	r = engine->RegisterObjectMethod("GameObject", "MeshFilter getMeshFilter()", CALLER(GameObject, getComponent<MeshComponent>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "PhysicsRigidbody getPhysicsRigidbody()", CALLER(GameObject, getComponent<PhysicsBodyComponent>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "PhysicsCharacter getPhysicsCharacter()", CALLER(GameObject, getComponent<PhysicsCharacterComponent>), asCALL_CDECL_OBJFIRST); assert(r >= 0);

	r = engine->RegisterObjectType("local", 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0);
	r = engine->RegisterGlobalProperty("local self", this); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "void setName(const string& in)", asMETHOD(ScriptBindActor, setName), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "const string& getName() const", asMETHOD(ScriptBindActor, getName), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "void setActive(bool)", asMETHOD(ScriptBindActor, setActive), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "bool getActive() const", asMETHOD(ScriptBindActor, getActive), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "void setTranslate(const float3& in)", asMETHOD(ScriptBindActor, setTranslate), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "void setLookat(const float3& in)", asMETHOD(ScriptBindActor, setLookat), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "void setUp(const float3& in)", asMETHOD(ScriptBindActor, setUp), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "const float3& getTranslate() const", asMETHOD(ScriptBindActor, getTranslate), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "const float3& getLookat() const", asMETHOD(ScriptBindActor, getLookat), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "const float3& getUp() const", asMETHOD(ScriptBindActor, getUp), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "MeshFilter getMeshFilter()", asMETHOD(ScriptBindActor, getMeshFilter), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "PhysicsRigidbody getPhysicsRigidbody()", asMETHOD(ScriptBindActor, getPhysicsRigidbody), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("local", "PhysicsCharacter getPhysicsCharacter()", asMETHOD(ScriptBindActor, getPhysicsCharacter), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterGlobalFunction("GameObject find(const string& in)", asFUNCTION(GameObject::find<GameObject>), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterGlobalFunction("GameObject instantiate(const string& in)", asFUNCTION(instantiate), asCALL_CDECL); assert(r >= 0);
}

void
ScriptBindActor::setGameObject(GameObjectPtr object) noexcept
{
	_object = object;
}

void
ScriptBindActor::setName(const std::string& name) noexcept
{
	_object.lock()->setName(name);
}

const std::string&
ScriptBindActor::getName() const noexcept
{
	return _object.lock()->getName();
}

void
ScriptBindActor::setActive(bool active) noexcept
{
	_object.lock()->setActive(active);
}

bool
ScriptBindActor::getActive() const noexcept
{
	return _object.lock()->getActive();
}

void 
ScriptBindActor::setTranslate(const Vector3& translate) noexcept
{
	_object.lock()->setTranslate(translate);
}

const Vector3& 
ScriptBindActor::getTranslate() const noexcept
{
	return _object.lock()->getTranslate();
}

void
ScriptBindActor::setLookat(const Vector3& lookat) noexcept
{
	_object.lock()->setLookAt(lookat);
}

const Vector3& 
ScriptBindActor::getLookat() const noexcept
{
	return _object.lock()->getLookAt();
}

void 
ScriptBindActor::setUp(const Vector3& up) noexcept
{
	_object.lock()->setUpVector(up);
}

const Vector3& 
ScriptBindActor::getUp() const noexcept
{
	return _object.lock()->getUpVector();
}

MeshComponentPtr 
ScriptBindActor::getMeshFilter() noexcept
{
	return _object.lock()->getComponent<MeshComponent>();
}

PhysicsBodyComponentPtr 
ScriptBindActor::getPhysicsRigidbody() noexcept
{
	return _object.lock()->getComponent<PhysicsBodyComponent>();
}

PhysicsCharacterComponentPtr 
ScriptBindActor::getPhysicsCharacter() noexcept
{
	return _object.lock()->getComponent<PhysicsCharacterComponent>();
}

_NAME_END
#endif