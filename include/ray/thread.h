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
#ifndef _H_THREAD_H_
#define _H_THREAD_H_

#include <ray/platform.h>

#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>

_NAME_BEGIN

class EXPORT ThreadLambda final
{
public:
	ThreadLambda() noexcept;
	~ThreadLambda() noexcept;

	void start() noexcept;
	void stop() noexcept;

	void exce(std::function<void(void)> func) noexcept;

	void join() noexcept;

	void flush() noexcept;
	void finish() noexcept;

	std::exception_ptr exception() const noexcept;

private:

	void signalQuitCond() noexcept;
	void signalFlushCond() noexcept;
	void signalFlushFinishCond() noexcept;

	void waitFlushCond() noexcept;
	void waitFlushFinishedCond() noexcept;

	void dispose() noexcept;

private:

	std::mutex _mutex;
	std::condition_variable _flushRequest;
	std::condition_variable _finishRequest;

	std::atomic<bool> _isQuitRequest;
	std::atomic<bool> _isFlushRequest;

	std::exception_ptr _exception;

	std::unique_ptr<std::thread> _thread;

	std::vector<std::function<void(void)>> _taskUpdate;
	std::vector<std::function<void(void)>> _taskDispose;
};

_NAME_END

#endif