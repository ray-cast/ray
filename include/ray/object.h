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
#ifndef _H_OBJECT_H_
#define _H_OBJECT_H_

#include <ray/reference.h>

_NAME_BEGIN

template<typename T>
class Object : public Reference<T>
{
public:
    typedef std::shared_ptr<T> pointer;

public:
    virtual void setVisible(bool visible) noexcept
    {
        _visible = visible;
    }

    virtual bool getVisible() const noexcept
    {
        return _visible;
    }

    virtual void setName(const std::string& name) noexcept
    {
        _name = name;
    }

    virtual const std::string& getName() const noexcept
    {
        return _name;
    }

    virtual pointer clone() const noexcept = 0;

protected:
    Object() noexcept
        : _visible(true)
    {
    }

    Object(const std::string& name) noexcept
        : _name(name)
        , _visible(true)
    {
    }

    virtual ~Object() noexcept
    {
    }

private:
    Object(const Object& copy) noexcept = delete;
    Object& operator=(const Object& copy) noexcept = delete;

private:

    bool _visible;

    std::string _name;
};

_NAME_END

#endif