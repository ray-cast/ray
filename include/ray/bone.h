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

#include <ray/modtypes.h>

_NAME_BEGIN

struct EXPORT AnimeTranslate
{
	double _time;

	Vector3 _translate;

	AnimeTranslate()
		: _time(0)
		, _translate(0, 0, 0)
	{
	}

	AnimeTranslate(double time, const Vector3& value)
		: _time(time)
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

struct EXPORT AnimeRotation
{
	double _time;

	Quaternion _rotate;

	AnimeRotation()
		: _time(0)
		, _rotate(0, 0, 0, 1)
	{
	}

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

class EXPORT AnimeScaling
{
public:
	Vector3 _scale;

	double _time;

	AnimeScaling()
		: _time(0)
		, _scale(1, 1, 1)
	{
	}

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

class EXPORT AnimeFrame
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

class EXPORT VertexWeight
{
public:
	float weight1;
	float weight2;
	float weight3;
	float weight4;

	std::int16_t bone1;
	std::int16_t bone2;
	std::int16_t bone3;
	std::int16_t bone4;
};

class EXPORT IKChild
{
public:
	std::uint16_t BoneIndex;
	std::uint8_t RotateLimited;

	Vector3 MinimumRadian;
	Vector3 MaximumRadian;
};

class EXPORT IKAttr
{
public:
	std::uint16_t IKBoneIndex;
	std::uint16_t IKTargetBoneIndex;
	std::uint32_t IKLoopCount;
	std::uint32_t IKLinkCount;

	float IKLimitedRadian;

	std::vector<IKChild> IKList;
};

class EXPORT Bone
{
public:
	Bone() noexcept;
	Bone(const std::string& name) noexcept;
	~Bone() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setParent(std::int16_t parent) noexcept;
	std::int16_t getParent() const noexcept;

	void setChild(std::int16_t child) noexcept;
	std::int16_t getChild() const noexcept;

	void setPosition(const Vector3& position) noexcept;
	const Vector3& getPosition() const noexcept;

	void setRotation(const Quaternion& position) noexcept;
	const Quaternion& getRotation() const noexcept;

	void setScaling(const Vector3& scale) noexcept;
	const Vector3& getScaling() const noexcept;

	void setWorldTransform(const Matrix4x4& transform) noexcept;
	const Matrix4x4& getWorldTransform() const noexcept;

	void setLocalTransform(const Matrix4x4& transform) noexcept;
	const Matrix4x4& getLocalTransform() const noexcept;

	void setTransform(const Matrix4x4& transform) noexcept;
	const Matrix4x4& getTransform() const noexcept;

private:
	std::string _name;

	std::int16_t _parent;
	std::int16_t _child;

	AnimeTranslate _position;
	AnimeRotation  _rotation;
	AnimeScaling   _scaling;

	mutable bool _needUpdate;

	mutable Matrix4x4 _transform;
	mutable Matrix4x4 _worldTransform;
	mutable Matrix4x4 _localTransform;
};

_NAME_END

#endif
