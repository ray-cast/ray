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
#include <ray/iointerface.h>

_NAME_BEGIN

__ImplementSingleton(IoInterface);

IoLoader::IoLoader() noexcept
	: _cached(true)
	, _loaded(false)
{
}

IoLoader::~IoLoader() noexcept
{
}

void
IoLoader::setCache(bool _cache) noexcept
{
	if (_cached != _cache)
	{
		if (!_name.empty())
		{
			if (_cache)
				this->doCache();
			else
				this->doUnCache();
		}

		_cached = _cache;
	}
}

bool
IoLoader::getCache() const noexcept
{
	return _cached;
}

bool
IoLoader::load()
{
	if (!_loaded)
	{
		if (this->doLoad())
			_loaded = true;
	}

	if (_loaded)
		this->doLoaded();

	return _loaded;
}

bool
IoLoader::loaded() const noexcept
{
	return _loaded;
}

void
IoLoader::setName(const std::string& str) noexcept
{
	_name = str;
}

const std::string&
IoLoader::getName() const noexcept
{
	return _name;
}

IoInterface::IoInterface() noexcept
	: _isPause(false)
	, _isQuit(false)
{
}

IoInterface::~IoInterface() noexcept
{
	this->close();
}

void
IoInterface::pause() noexcept
{
	_isPause = true;
}

void
IoInterface::resume() noexcept
{
	_isPause = false;
}

bool
IoInterface::running() noexcept
{
	return _isPause;
}

void
IoInterface::load(IoLoader& resource, bool async)
{
	if (async)
	{
		_queue.push(resource.clone());
		_dispose.notify_one();
	}
	else
	{
		resource.load();
	}
}

void
IoInterface::open() noexcept
{
	if (!_disposeThread)
	{
		_disposeThread = std::make_unique<std::thread>(std::bind(&IoInterface::dispose, this));
	}
}

void
IoInterface::close() noexcept
{
	_isQuit = true;
	if (_disposeThread)
	{
		_dispose.notify_one();
		_disposeThread->join();
		_disposeThread.reset();
	}
}

void
IoInterface::dispose()
{
	try
	{
		while (!_isQuit)
		{
			if (!_isPause)
			{
				std::unique_lock<std::mutex> _lock(_mutex);
				if (_queue.empty())
				{
					_dispose.wait(_lock);
				}

				if (!_queue.empty())
				{
					auto res = _queue.front();
					_queue.pop();

					if (res)
					{
						res->load();
					}
				}
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::microseconds(60));
			}
		}
	}
	catch (...)
	{
		_isQuit = true;
	}
}

_NAME_END