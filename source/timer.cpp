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
#include <ray/timer.h>
#include <ctime>
#include <cmath>
#include <limits>
#include <thread>

_NAME_BEGIN

Timer::Timer() noexcept
	: _appTime(0)
	, _frameTime(0)
	, _accumulateTime(0)
	, _accumulateFps(0)
	, _fps(0)
	, _averageFps(0)
	, _numFrames(0)
	, _startTime(0)
	, _currentFramePerSecond(0)
{
	this->reset();
}

Timer::~Timer() noexcept
{
}

bool
Timer::open() noexcept
{
	_startTime = (float)std::clock() / (float)CLOCKS_PER_SEC;
	return true;
}

float
Timer::startTime() const noexcept
{
	return _startTime;
}

float
Timer::elapsed() const noexcept
{
	return float(std::clock()) / CLOCKS_PER_SEC - _startTime;
}

float
Timer::elapsed_max() const noexcept
{
	return float(std::numeric_limits<std::clock_t>::max()) / float(CLOCKS_PER_SEC) - _startTime;
}

float
Timer::elapsed_min() const noexcept
{
	return float(1) / float(CLOCKS_PER_SEC);
}

float
Timer::fps() const noexcept
{
	return _fps;
}

float
Timer::averageFps() const noexcept
{
	return _averageFps;
}

float
Timer::appTime() const noexcept
{
	return _appTime;
}

float
Timer::frameTime() const noexcept
{
	return _frameTime;
}

float
Timer::delta() const noexcept
{
	return _frameTime;
}

float
Timer::vsync() const noexcept
{
	return this->delta() * (1 / 0.06f);
}

void
Timer::reset() noexcept
{
	_lastTime = (float)std::clock() / (float)CLOCKS_PER_SEC - _startTime;
}

void
Timer::sleep(float fps) const noexcept
{
	double first = CLOCKS_PER_SEC / fps;
	double second = this->delta() * CLOCKS_PER_SEC;
	if (first > second)
	{
		int sleep = std::round(first - second);
		if (sleep > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
		}
	}
}

void
Timer::update() noexcept
{
	_frameTime = this->elapsed() - _lastTime;

	_numFrames++;
	_accumulateTime += _frameTime;
	_appTime += _frameTime;

	if (_accumulateTime > 1)
	{
		_fps = _numFrames / _accumulateTime;

		_numFrames = 0;
		_accumulateTime--;
		_accumulateFps = 0;

		_framesPerSecondArray[_currentFramePerSecond] = _fps;

		for (size_t i = 0; i <= _currentFramePerSecond; i++)
			_accumulateFps += _framesPerSecondArray[i];

		if (_currentFramePerSecond < 9)
		{
			_currentFramePerSecond++;
			_averageFps = _accumulateFps / _currentFramePerSecond;
		}
		else
		{
			_currentFramePerSecond = 0;
		}
	}

	this->reset();
}

_NAME_END