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
#ifndef _H_MODUTIL_H_
#define _H_MODUTIL_H_

#include <ray/modtypes.h>

_NAME_BEGIN

class CombineInstance
{
public:
    struct Instance
    {
        Matrix4x4 transform;
        MeshPropertyPtr mesh;

        void setMesh(MeshPropertyPtr other)
        {
            mesh = other;
        }

        MeshPropertyPtr getMesh() const
        {
            return mesh;
        }

        void setTransform(const float4x4& m)
        {
            transform = m;
        }

		void makeTransform(const float3& translate, const EulerAngles& euler, const float3& scale)
		{
			transform.makeTransform(translate, Quaternion().makeRotate(euler), scale);
		}

        const Matrix4x4& getTransform() const
        {
            return transform;
        }
    };

public:
    CombineInstance() noexcept
    {
    }

    CombineInstance(std::size_t size)
    {
        _instances.resize(size);
        for (auto& it : _instances)
            it.transform.loadIdentity();
    }

    ~CombineInstance() noexcept
    {
    }

    void resize(std::size_t size)
    {
        _instances.resize(size);
    }

    bool empty() const noexcept
    {
        return _instances.empty();
    }

    Instance& at(std::size_t i)
    {
        return _instances[i];
    }

    Instance& operator[](std::size_t i)
    {
        return _instances[i];
    }

    const std::vector<Instance>& getInstances() const noexcept
    {
        return _instances;
    }

    void push_back(Instance& instance)
    {
        _instances.push_back(instance);
    }

private:
    std::vector<Instance> _instances;
};

_NAME_END

#endif