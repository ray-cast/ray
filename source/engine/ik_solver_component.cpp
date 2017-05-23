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
#include <ray/ik_solver_component.h>

_NAME_BEGIN

__ImplementSubClass(IKSolverComponent, GameComponent, "IKSolver")

IKSolverComponent::IKSolverComponent() noexcept
	: _chainLength(1)
{
}

IKSolverComponent::~IKSolverComponent() noexcept
{
}

void 
IKSolverComponent::setTargetBone(GameObjectPtr bone) noexcept
{
	_target = bone;
}

GameObjectPtr 
IKSolverComponent::getTargetBone() const noexcept
{
	return _target;
}

void 
IKSolverComponent::setIterations(std::uint32_t iterations) noexcept
{
	_iterations = iterations;
}

std::uint32_t 
IKSolverComponent::getIterations() const noexcept
{
	return _iterations;
}

void 
IKSolverComponent::setChainLength(std::uint32_t length) noexcept
{
	_chainLength = length;
}

std::uint32_t 
IKSolverComponent::getChainLength() const noexcept
{
	return _chainLength;
}

void 
IKSolverComponent::addBone(const IKSolverBonePtr bone) noexcept
{
	_bones.push_back(bone);
}

void 
IKSolverComponent::setBones(const IKSolverBones& bones) noexcept
{
	_bones = bones;
}

void 
IKSolverComponent::setBones(const IKSolverBones&& bones) noexcept
{
	_bones = std::move(bones);
}

const IKSolverComponent::IKSolverBones&
IKSolverComponent::getBones() const noexcept
{
	return _bones;
}

GameComponentPtr
IKSolverComponent::clone() const noexcept
{
	auto iksolver = std::make_shared<IKSolverComponent>();
	return iksolver;
}

_NAME_END