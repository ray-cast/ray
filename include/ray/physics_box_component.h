// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#ifndef _H_PHYSICS_BOX_COMPONENT_H_
#define _H_PHYSICS_BOX_COMPONENT_H_

#include <ray/physics_shape_component.h>

_NAME_BEGIN

class PhysicsShapeBox;
class EXPORT PhysicsBoxComponent final : public PhysicsShapeComponent
{
	__DeclareSubClass(PhysicsBoxComponent, PhysicsShapeComponent)
public:
	PhysicsBoxComponent() noexcept;
	PhysicsBoxComponent(const float3& size) noexcept;
	~PhysicsBoxComponent() noexcept;

	void setSize(const float3& size) noexcept;
	const float3& getSize() const noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	virtual GameComponentPtr clone() const noexcept;

private:
	virtual void onActivate() noexcept;
	virtual void onDeactivate() noexcept;

	virtual PhysicsShapePtr getCollisionShape() noexcept;

private:
	PhysicsBoxComponent(const PhysicsBoxComponent&) noexcept = delete;
	PhysicsBoxComponent& operator=(const PhysicsBoxComponent&)noexcept = delete;

private:

	std::shared_ptr<PhysicsShapeBox> _shape;
};

_NAME_END

#endif