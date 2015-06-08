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
#ifndef _H_INSTANCE_H_
#define _H_INSTANCE_H_

#include <ray/reference.h>

_NAME_BEGIN

template<typename T>
class Instance : public Reference<T>
{
public:
    typedef std::shared_ptr<T> pointer;
    typedef const pointer const_pointer;

public:
    typedef std::size_t InstanceID;

    Instance() noexcept
    {
        if (_empty_lists.empty())
        {
            _instance_lists.push_back((T*)this);
            _instance = _instance_lists.size();
        }
        else
        {
            InstanceID instance = _empty_lists.back();
            _empty_lists.pop_back();
            _instance_lists[instance - 1] = (T*)this;
            _instance = instance;
        }
    }

    virtual ~Instance() noexcept
    {
        _instance_lists[_instance - 1] = nullptr;
        _empty_lists.push_back(_instance);
    }

    InstanceID getInstanceID() const noexcept
    {
        return _instance;
    }

    static std::vector<T*>& instances() noexcept
    {
        return _instance_lists;
    }

private:

    InstanceID _instance;

    static std::vector<T*> _instance_lists;
    static std::vector<InstanceID> _empty_lists;
};

template<typename T> std::vector<T*> Instance<T>::_instance_lists;
template<typename T> std::vector<typename Instance<T>::InstanceID> Instance<T>::_empty_lists;

_NAME_END

#endif
