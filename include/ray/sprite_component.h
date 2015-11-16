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
#ifndef _H_PARTICLE_EMITTER_H_
#define _H_PARTICLE_EMITTER_H_

#include <ray/game_component.h>

_NAME_BEGIN

struct Particle
{
	float age;

	Vector3 position;

	Vector3 color;
	Vector3 colorDelta;

	Vector3 size;
	Vector3 sizeDelta;

	Vector3 spin;
	Vector3 spinDelta;

	Vector3 velocity;
	Vector3 angularVelocity;
	Vector3 radialAccel;
	Vector3 tangentialAccel;
	Vector3 gravity;
};

enum ParticleMode
{
	Billboard,
	Stretch,
	SortedBillboard,
	HorizontaBillboard,
	VerticalBillboard
};

class EXPORT ParticleEmitterComponent : public GameComponent
{
	__DeclareSubClass(ParticleEmitterComponent, GameComponent)
public:
	ParticleEmitterComponent() noexcept;
	virtual ~ParticleEmitterComponent() noexcept;

	void clear();

	void start(bool emit) noexcept { _enable = true; };
	void stop(bool emit) noexcept { _enable = true; };

	void emitter();
	void emitter(Particle* particle) noexcept;

	void simulate(float delta) noexcept;

	void setEmissionMin(float _min) noexcept { _emission_min = _min; }
	void setEmissionMax(float _max) noexcept { _emission_max = _max; }

	void setAgeMin(float _min) noexcept { _age_min = _min; }
	void setAgeMax(float _max) noexcept { _age_max = _max; }

	void setEmissionVelocityScale(float _velocity) noexcept { _emission_velocity_scale = _velocity; }
	void setEmissionDirection(const Vector3& _direction) noexcept { _emission_direction = _direction; }

	void setSizeStart(const Vector3& _start) noexcept { _size_start = _start; }
	void setSizeEnd(const Vector3& _end) noexcept { _size_end = _end; }
	void setSizeVariance(const Vector3& _variance) noexcept { _size_variance = _variance; }

	void setSpinStart(const Vector3& _start) noexcept { _spin_start = _start; }
	void setSpinEnd(const Vector3& _end) noexcept { _spin_end = _end; }
	void setSpinVariance(const Vector3& _variance) noexcept { _spin_variance = _variance; }

	void setGravityMin(const Vector3& _min) noexcept { _gravity_min = _min; }
	void setGravityMax(const Vector3& _max) noexcept { _gravity_min = _max; }

	void setRadialMin(const Vector3& _min) noexcept { _radial_min = _min; }
	void setRadialMax(const Vector3& _max) noexcept { _radial_max = _max; }

	void setTangentialMin(const Vector3& _min) noexcept { _tangential_min = _min; }
	void setTangentialMax(const Vector3& _max) noexcept { _tangential_min = _max; }

	void setParticles(const std::vector<Particle*>& particl) noexcept { _particles = particl; }

	std::vector<Particle*>& getParticles() noexcept { return _particles; }
	const std::vector<Particle*>& getParticles() const noexcept { return _particles; }

	GameComponentPtr clone() const noexcept;

private:

	bool _enable;

	ParticleMode _mode;

	float _time;

	float _emission;
	float _emission_min;
	float _emission_max;
	float _emission_speed;
	float _emission_velocity_scale;
	Vector3 _emission_direction;

	float _particle_count;
	float _particle_size;

	float _age_min;
	float _age_max;

	Vector3 _position_start;
	Vector3 _position_end;
	Vector3 _position_variance;

	Vector3 _size_start;
	Vector3 _size_end;
	Vector3 _size_variance;

	Vector3 _spin_start;
	Vector3 _spin_end;
	Vector3 _spin_variance;

	Vector3 _color_start;
	Vector3 _color_end;
	Vector3 _color_variance;

	Vector3 _speed_min;
	Vector3 _speed_max;
	Vector3 _speed_variance;

	Vector3 _gravity_min;
	Vector3 _gravity_max;
	Vector3 _gravity_variance;

	Vector3 _radial_min;
	Vector3 _radial_max;
	Vector3 _radial_variance;

	Vector3 _tangential_min;
	Vector3 _tangential_max;
	Vector3 _tangential_variance;

	std::vector<Particle*> _particles;
};

_NAME_END

#endif