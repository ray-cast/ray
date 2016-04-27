// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#ifndef _H_IO_INTERFACE_H_
#define _H_IO_INTERFACE_H_

#include <ray/iostream.h>
#include <ray/memory.h>

#include <queue>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <condition_variable>

_NAME_BEGIN

class EXPORT IoLoader
{
public:
	IoLoader() noexcept;
	virtual ~IoLoader() noexcept;

	void setName(const std::string& str) noexcept;
	const std::string& getName() const noexcept;

	bool loaded() const noexcept;
	bool load() except;

	void setCache(bool cache) noexcept;
	bool isCache() const noexcept;

	virtual std::shared_ptr<IoLoader> clone() const = 0;

protected:
	virtual bool doLoad() except = 0;
	virtual void doLoaded() except = 0;

	virtual void doCache() except = 0;
	virtual void doUnCache() except = 0;

private:
	IoLoader(const IoLoader& copy) = delete;
	IoLoader& operator=(const IoLoader&) = delete;

private:
	bool _loaded;
	bool _cached;

	std::string _name;
};

class EXPORT IoInterface final
{
	__DeclareSingleton(IoInterface)
public:
	IoInterface() noexcept;
	~IoInterface() noexcept;

	void open() noexcept;
	void close() noexcept;

	void pause() noexcept;
	void resume() noexcept;
	bool running() noexcept;

	void load(IoLoader& resource, bool async = false);

private:
	void dispose();

private:
	IoInterface(const IoInterface&) noexcept = delete;
	IoInterface& operator=(const IoInterface&) noexcept = delete;

private:

	bool _isQuit;
	bool _isPause;

	std::mutex _mutex;
	std::condition_variable _dispose;
	std::queue<std::shared_ptr<IoLoader>> _queue;

	std::unique_ptr<std::thread> _disposeThread;
};

_NAME_END

#endif