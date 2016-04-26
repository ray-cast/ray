#ifndef _H_INTERVAL_H_
#define _H_INTERVAL_H_

#include <ray/platform.h>

_NAME_BEGIN

template<typename T, typename S>
class Interval
{
public:
	Interval()
		:_timer(0)
		, _shouldStep(false)
	{
	}

	Interval(const T& Start, const T& End, S Duration)
		:_start(Start)
		, _end(End)
		, _lerp(lerp(Start, End, Duration))
		, _current(Start)
		, _duration(Duration)
		, _timer(0.0f)
		, _shouldStep(true)
	{
	}

	T lerp(T t1, T t2, S t3)
	{
		if (t3 == S(0))
		{
			return t2 - t1;
		}
		return T((t2 - t1) / t3);
	}

	void setStart(const T& Strat)
	{
		_start = Strat;
		_lerp = lerp(_start, _end, _duration);
	}

	void setEnd(const T& End)
	{
		_end = End;
		_lerp = lerp(_start, _end, _duration);
	}

	void setDuration(S Duration)
	{
		_duration = Duration;
		_lerp = lerp(_start, _end, _duration);
	}

	void setInterval(const T& Start, const T& End, S Duration)
	{
		_start = Start;
		_end = End;
		_duration = Duration;
		_lerp = lerp(_start, _end, _duration);
	}

	void setCurrent(const T& Current)
	{
		_current = Current;
	}

	float getTimer() const
	{
		return Time_r;
	}

	float getDuration() const
	{
		return _duration;
	}

	const T& getStart() const
	{
		return _start;
	}

	const T& getEnd() const
	{
		return _end;
	}

	const T& getCurrent() const
	{
		return _current;
	}

	bool isShouldStep() const
	{
		return _shouldStep;
	}

	void reset()
	{
		_timer = 0.0f;
		_current = _start;
		_shouldStep = true;
	}

	const T& step(S dt)
	{
		if (!_shouldStep)
		{
			return _current;
		}

		_timer += dt;

		if (_timer >= _duration)
		{
			_current = _end;

			_shouldStep = false;

			return _current;
		}

		return _current += _lerp * dt;
	}

private:
	T _start;
	T _end;
	T _current;
	T _lerp;

	S _duration;
	S _timer;

	bool _shouldStep;
};

_NAME_END

#endif