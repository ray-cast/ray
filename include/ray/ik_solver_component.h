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
#ifndef _H_IK_SOLVER_COMPONENT_H_
#define _H_IK_SOLVER_COMPONENT_H_

#include <ray/game_component.h>

_NAME_BEGIN

class EXPORT IKSolverBone
{
public:
	GameObjectPtr bone;
	std::uint8_t rotateLimited;

	float angleWeight;

	float3 minimumDegrees;
	float3 maximumDegrees;
};

class EXPORT IKSolverComponent final : public GameComponent
{
	__DeclareSubClass(IKSolverComponent, GameComponent)
public:
	typedef std::shared_ptr<IKSolverBone> IKSolverBonePtr;
	typedef std::vector<IKSolverBonePtr> IKSolverBones;

public:
	IKSolverComponent() noexcept;
	~IKSolverComponent() noexcept;

	void setTargetBone(GameObjectPtr bone) noexcept;
	GameObjectPtr getTargetBone() const noexcept;

	void setIterations(std::uint32_t iterations) noexcept;
	std::uint32_t getIterations() const noexcept;

	void setChainLength(std::uint32_t length) noexcept;
	std::uint32_t getChainLength() const noexcept;

	void addBone(IKSolverBonePtr bone) noexcept;
	void setBones(const IKSolverBones& bone) noexcept;
	void setBones(const IKSolverBones&& bone) noexcept;
	const IKSolverBones& getBones() const noexcept;

	GameComponentPtr clone() const noexcept;

private:
	IKSolverComponent(const IKSolverComponent&) = delete;
	IKSolverComponent& operator=(const IKSolverComponent&) = delete;

private:
	GameObjectPtr _target;

	std::uint32_t _iterations;
	std::uint32_t _chainLength;

	IKSolverBones _bones;
};

_NAME_END

#endif