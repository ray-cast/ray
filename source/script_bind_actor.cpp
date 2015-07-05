// +----------------------------------------------------------------------
// | Project : Xiemoperor.
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
#include <ray/script_bind_actor.h>
#include <ray/game_object.h>
#include <ray/mesh_component.h>
#include <ray/physics_body_component.h>
#include <ray/physics_character_component.h>
#include <ray/script_smart_ptr_wrapper.h>

_NAME_BEGIN

typedef std::shared_ptr<class PhysicsShape> PhysicsShapePtr;
typedef std::shared_ptr<class PhysicsBodyComponent> PhysicsBodyComponentPtr;
typedef std::shared_ptr<class PhysicsCharacterComponent> PhysicsCharacterComponentPtr;
typedef std::shared_ptr<class MeshComponent> MeshComponentPtr;

template <typename T>
void construct(void * address)
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
	return t ? true : false;
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

	r = engine->RegisterObjectType("CombineInstance", sizeof(CombineInstance), asOBJ_VALUE | asOBJ_APP_CLASS_CDK); assert(r >= 0);

	r = engine->RegisterObjectType("MeshFilter", sizeof(MeshPropertyPtr), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("MeshFilter", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<MeshPropertyPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("MeshFilter", asBEHAVE_CONSTRUCT, "void f(const Mesh& in)", asFUNCTION(copy_construct<MeshPropertyPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("MeshFilter", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<MeshPropertyPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);

	r = engine->RegisterObjectType("CombineMesh", sizeof(CombineInstance::Instance), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineMesh", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<CombineInstance::Instance>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineMesh", asBEHAVE_CONSTRUCT, "void f(const CombineMesh& in)", asFUNCTION(copy_construct<CombineInstance::Instance>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineMesh", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<CombineInstance::Instance>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("CombineMesh", "void set_mesh(Mesh)", asMETHOD(CombineInstance::Instance, setMesh), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("CombineMesh", "void set_transform(const float4x4& in)", asMETHOD(CombineInstance::Instance, setTransform), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("CombineMesh", "Mesh get_mesh()", asMETHOD(CombineInstance::Instance, getMesh), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("CombineMesh", "const float4x4& get_transform()", asMETHOD(CombineInstance::Instance, getTransform), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectBehaviour("CombineInstance", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<CombineInstance>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineInstance", asBEHAVE_CONSTRUCT, "void f(int i)", asFUNCTION((construct_1<CombineInstance, int>)), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineInstance", asBEHAVE_CONSTRUCT, "void f(const CombineInstance& in)", asFUNCTION(copy_construct<CombineInstance>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("CombineInstance", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<CombineInstance>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("CombineInstance", "CombineMesh& opIndex(uint) const", asMETHOD(CombineInstance, at), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectType("MeshComponent", sizeof(MeshComponentPtr), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("MeshComponent", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<MeshComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("MeshComponent", asBEHAVE_CONSTRUCT, "void f(const MeshComponent& in)", asFUNCTION(copy_construct<MeshComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("MeshComponent", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<MeshComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshComponent", "MeshComponent& opAssign(const MeshComponent& in)", asFUNCTION(assign<MeshComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshComponent", "bool opImplConv() const", asFUNCTION(ImplConv<MeshComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("MeshComponent", "void set_name(const string& in)", CALLER(MeshComponent, setName), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshComponent", "const string& get_name() const", CALLER(MeshComponent, getName), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshComponent", "void set_mesh(Mesh)", CALLER(MeshComponent, setMesh), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("MeshComponent", "Mesh get_mesh() const", CALLER(MeshComponent, getMesh), asCALL_CDECL_OBJFIRST); assert(r >= 0);

	r = engine->RegisterObjectType("PhysicsBodyComponent", sizeof(PhysicsBodyComponentPtr), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("PhysicsBodyComponent", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<PhysicsBodyComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("PhysicsBodyComponent", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<PhysicsBodyComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "PhysicsBodyComponent& opAssign(const PhysicsBodyComponent& in)", asFUNCTION(assign<PhysicsBodyComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "bool opImplConv() const", asFUNCTION(ImplConv<PhysicsBodyComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void setMass(float value)", CALLER(PhysicsBodyComponent, setMass), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void setLinearVelocity(const float3& in)", CALLER(PhysicsBodyComponent, setLinearVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void setAngularVelocity(const float3& in)", CALLER(PhysicsBodyComponent, setAngularVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void setLinearDamping(float value) ", CALLER(PhysicsBodyComponent, setLinearDamping), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void setAngularDamping(float value)", CALLER(PhysicsBodyComponent, setAngularDamping), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void setGravity(const float3& in)", CALLER(PhysicsBodyComponent, setGravity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void setConstantForce(const float3& in)", CALLER(PhysicsBodyComponent, setConstantForce), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void setConstantTorque(const float3& in)", CALLER(PhysicsBodyComponent, setConstantTorque), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void setConstantVelocity(const float3& in)", CALLER(PhysicsBodyComponent, setConstantVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void setConstanAngularVelocity(const float3& in)", CALLER(PhysicsBodyComponent, setConstanAngularVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void sleep(bool sleep)", CALLER(PhysicsBodyComponent, sleep), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "bool isSleep() const", CALLER(PhysicsBodyComponent, isSleep), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "float getMass() const", CALLER(PhysicsBodyComponent, getMass), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "float getLinearDamping() const", CALLER(PhysicsBodyComponent, getLinearDamping), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "float getAngularDamping() const", CALLER(PhysicsBodyComponent, getAngularDamping), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "const float3& getGravity() const", CALLER(PhysicsBodyComponent, getGravity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "const float3& getLinearVelocity() const", CALLER(PhysicsBodyComponent, getLinearVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "const float3& getAngularVelocity() const", CALLER(PhysicsBodyComponent, getAngularVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "const float3& getConstantForce() const", CALLER(PhysicsBodyComponent, getConstantForce), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "const float3& getConstantTorque() const", CALLER(PhysicsBodyComponent, getConstantTorque), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "const float3& getConstantVelocity() const", CALLER(PhysicsBodyComponent, getConstantVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "const float3& getConstantAngularVelocity() const", CALLER(PhysicsBodyComponent, getConstantAngularVelocity), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void addForce(const float3& in)", CALLER(PhysicsBodyComponent, addForce), asCALL_CDECL_OBJFIRST); assert(r > 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void addRelativeForce(const float3& in, const float3& in)", CALLER(PhysicsBodyComponent, addRelativeForce), asCALL_CDECL_OBJFIRST); assert(r > 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void addTorque(const float3& in)", CALLER(PhysicsBodyComponent, addTorque), asCALL_CDECL_OBJFIRST); assert(r > 0);
	r = engine->RegisterObjectMethod("PhysicsBodyComponent", "void addImpulse(const float3& in, const float3& in)", CALLER(PhysicsBodyComponent, addImpulse), asCALL_CDECL_OBJFIRST); assert(r > 0);

	r = engine->RegisterObjectType("PhysicsCharacterComponent", sizeof(PhysicsCharacterComponentPtr), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("PhysicsCharacterComponent", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<PhysicsCharacterComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("PhysicsCharacterComponent", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<PhysicsCharacterComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsCharacterComponent", "PhysicsCharacterComponent& opAssign(const PhysicsCharacterComponent& in)", asFUNCTION(assign<PhysicsCharacterComponentPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("PhysicsCharacterComponent", "bool opImplConv() const", asFUNCTION(ImplConv<PhysicsCharacterComponentPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("PhysicsCharacterComponent", "void setWalkDirection(const float3& in)", CALLER(PhysicsCharacterComponent, setWalkDirection), asCALL_CDECL_OBJFIRST); assert(r > 0);
	r = engine->RegisterObjectMethod("PhysicsCharacterComponent", "void jump(float)", CALLER(PhysicsCharacterComponent, jump), asCALL_CDECL_OBJFIRST); assert(r > 0);
	r = engine->RegisterObjectMethod("PhysicsCharacterComponent", "bool canJumping() const", CALLER(PhysicsCharacterComponent, canJumping), asCALL_CDECL_OBJFIRST); assert(r > 0);

	r = engine->RegisterObjectType("GameObject", sizeof(GameObjectPtr), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GameObject", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<GameObjectPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GameObject", asBEHAVE_CONSTRUCT, "void f(const GameObject& in)", asFUNCTION(copy_construct<GameObjectPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GameObject", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<GameObjectPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "GameObject& opAssign(const GameObject& in)", asFUNCTION(assign<GameObjectPtr>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "bool opImplConv() const", asFUNCTION(ImplConv<GameObjectPtr>), asCALL_CDECL_OBJFIRST);
	r = engine->RegisterObjectMethod("GameObject", "void set_name(const string& in)", CALLER(GameObject, setName), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "const string& get_name() const", CALLER(GameObject, getName), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "void set_active(bool)", CALLER(GameObject, setActive), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "bool get_active() const", CALLER(GameObject, getActive), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "void set_translate(const float3& in)", CALLER(GameObject, setTranslate), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "void set_lookat(const float3& in)", CALLER(GameObject, setLookAt), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "void set_up(const float3& in)", CALLER(GameObject, setUpVector), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "const float3& get_translate() const", CALLER(GameObject, getTranslate), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "const float3& get_lookat() const", CALLER(GameObject, getLookAt), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "const float3& get_up() const", CALLER(GameObject, getUpVector), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "MeshComponent get_mesh()", CALLER(GameObject, getComponent<MeshComponent>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "PhysicsBodyComponent get_rigidbody()", CALLER(GameObject, getComponent<PhysicsBodyComponent>), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("GameObject", "PhysicsCharacterComponent get_character()", CALLER(GameObject, getComponent<PhysicsCharacterComponent>), asCALL_CDECL_OBJFIRST); assert(r >= 0);

	r = engine->RegisterObjectMethod("GameObject", "GameObject clone() const", CALLER(GameObject, clone), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterGlobalFunction("GameObject find(const string& in)", asFUNCTION(GameObject::find<GameObject>), asCALL_CDECL); assert(r >= 0);
}

_NAME_END