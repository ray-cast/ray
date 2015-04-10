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
#ifndef _H_BONE_H_
#define _H_BONE_H_

#include <ray/math.h>
#include <vector>

_NAME_BEGIN

struct AnimeTranslate
{
    double _time;

    Vector3 _translate;

    AnimeTranslate() {}

    AnimeTranslate(double time, const Vector3& value)
        :_time(time)
        , _translate(value)
    {}

    bool operator == (const AnimeTranslate& o) const
    {
        return o._translate == this->_translate;
    }

    bool operator != (const AnimeTranslate& o) const
    {
        return o._translate != this->_translate;
    }

    bool operator <(const AnimeTranslate& o) const
    {
        return _time < o._time;
    }

    bool operator >(const AnimeTranslate& o) const
    {
        return _time > o._time;
    }
};

struct AnimeRotation
{
    double _time;

    Quaternion _rotate;

    AnimeRotation() {}

    AnimeRotation(double time, const Quaternion& value)
        : _time(time)
        , _rotate(value)
    {}

    typedef Quaternion elem_type;

    bool operator == (const AnimeRotation& o) const
    {
        return o._rotate == this->_rotate;
    }

    bool operator != (const AnimeRotation& o) const
    {
        return o._rotate != this->_rotate;
    }

    bool operator <(const AnimeRotation& o) const
    {
        return _time < o._time;
    }

    bool operator >(const AnimeRotation& o) const
    {
        return _time > o._time;
    }
};

class AnimeScaling
{
public:
    Vector3 _scale;

    double _time;

    AnimeScaling() {}

    AnimeScaling(const AnimeScaling& copy)
        :_scale(copy._scale)
        , _time(copy._time)
    {
    }

    AnimeScaling(double time, const Vector3& v)
        :_time(time)
        , _scale(v)
    {
    }

    void loadIdentity()
    {
        _scale = Vector3::UnitX;
        _time = 0;
    }

    bool operator == (const AnimeScaling& o) const
    {
        return o._scale == this->_scale;
    }

    bool operator != (const AnimeScaling& o) const
    {
        return o._scale != this->_scale;
    }

    bool operator < (const AnimeScaling& o) const
    {
        return _time < o._time;
    }

    bool operator >(const AnimeScaling& o) const
    {
        return _time > o._time;
    }
};

class AnimeFrame
{
public:
    double _time;

    std::size_t _key_frame;

    AnimeFrame() { }

    AnimeFrame(double time, unsigned key)
        : _time(time)
        , _key_frame(key)
    {
    }

    void loadIdentity()
    {
        _key_frame = 0;
        _time = 0;
    }

    bool operator == (const AnimeFrame& o) const
    {
        return o._key_frame == this->_key_frame;
    }

    bool operator != (const AnimeFrame& o) const
    {
        return o._key_frame != this->_key_frame;
    }

    bool operator < (const AnimeFrame& o) const
    {
        return _time < o._time;
    }

    bool operator >(const AnimeFrame& o) const
    {
        return _time > o._time;
    }
};

struct MeshKey
{
    double _time;

    unsigned int _value;

    MeshKey() {}
    MeshKey(double time, const unsigned int value)
        : _time(time)
        , _value(value)
    {}

    typedef unsigned int elem_type;

    bool operator == (const MeshKey& o) const
    {
        return o._value == this->_value;
    }

    bool operator != (const MeshKey& o) const
    {
        return o._value != this->_value;
    }

    bool operator <(const MeshKey& o) const
    {
        return _time < o._time;
    }

    bool operator >(const MeshKey& o) const
    {
        return _time > o._time;
    }
};

class MeshAnim
{
public:
    std::string _name;

    std::size_t _num_keys;

    MeshKey* _keys;

    MeshAnim() : _num_keys(), _keys() {}

    ~MeshAnim()
    {
        delete[] _keys;
    }
};

class VertexWeight
{
public:
    VertexWeight() noexcept
    {
    }

    VertexWeight(std::size_t id, float weight) noexcept
        : _index(id)
        , _weight(weight)
    {
    }

private:

    float _weight;

    std::size_t _index; // vertex of index
};

class Bone
{
public:
    Bone() noexcept;
    explicit Bone(const std::string& name) noexcept;
    explicit Bone(const Bone& copy) noexcept;
    ~Bone() noexcept;

    void update() noexcept;

private:
    std::string _name;

    Bone* _parent;
    Bone* _children;

    AnimeTranslate _position;
    AnimeRotation  _rotation;
    AnimeScaling   _scaling;

    Matrix4x4 _offset_matrix;
    Matrix4x4 _bone_matrix;
    Matrix4x4 _node_matrix;
    Matrix4x4 _final_matrix;
    Matrix4x4 _anime_matrix;

    std::vector<VertexWeight*> _weights;
};

_NAME_END

#endif
