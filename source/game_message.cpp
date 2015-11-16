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
#include <ray/game_message.h>

_NAME_BEGIN

__ImplementInterface(GameListener, "GameListener")

GameEvent::GameEvent(const std::type_info& info) noexcept
	: _code(info.hash_code())
{
}

GameEvent::GameEvent(const GameEvent& other) noexcept
	: _code(other._code)
{
}

std::size_t 
GameEvent::code() const noexcept
{
	return _code;
}

GameEvent::~GameEvent() noexcept
{
}

GameEvent&
GameEvent::operator=(const GameEvent& other) noexcept
{
	_code = other._code;
	return *this;
}

GameMessage::GameMessage() noexcept
	: _event(nullptr)
{
}

GameMessage::GameMessage(GameEvent* event) noexcept
	: _event(event)
{
}

GameMessage::GameMessage(const GameMessage& other) noexcept
	: _event(other._event)
{
}

GameMessage::~GameMessage() noexcept
{
}

GameMessage&
GameMessage::operator=(const GameMessage & other) noexcept
{
	_event = other._event;
	return *this;
}

GameEvent*
GameMessage::event() const noexcept
{
	return _event;
}

GameListener::GameListener() noexcept
{
}

GameListener::GameListener(const std::string& name) noexcept
	: _name(name)
{
}

GameListener::~GameListener() noexcept
{
}

void
GameListener::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
GameListener::getName() const noexcept
{
	return _name;
}

void
GameListener::load(iarchive& reader) except
{
	reader >> make_alias(_name, "name");
}

void
GameListener::save(oarchive& write) except
{
	write << make_alias(_name, "name");
}

void
GameListener::sendMessage(const GameMessage& message) except
{
	this->onMessage(message);
}

void
GameListener::onMessage(const GameMessage&) except
{
}

GameDispatcher::GameDispatcher() noexcept
	: _enableMessagePosting(true)
{
}

GameDispatcher::~GameDispatcher() noexcept
{
}

void
GameDispatcher::enableMessagePosting(bool enable) noexcept
{
	_enableMessagePosting = enable;
}

bool
GameDispatcher::enableMessagePosting() const noexcept
{
	return _enableMessagePosting;
}

void
GameDispatcher::addGameListener(GameListenerPtr listener) noexcept
{
	if (listener)
	{
		auto it = std::find(_gameListener.begin(), _gameListener.end(), listener);
		if (it == _gameListener.end())
		{
			_gameListener.push_back(listener);
		}
	}
}

GameListeners
GameDispatcher::getGameListeners() const noexcept
{
	return _gameListener;
}

GameListenerPtr
GameDispatcher::getGameListener(const std::string& name) const noexcept
{
	for (auto& it : _gameListener)
	{
		if (it->getName() == name)
			return it;
	}

	return nullptr;
}

void
GameDispatcher::sendMessage(const GameMessage& event) except
{
	for (auto& it : _gameListener)
	{
		it->sendMessage(event);
	}
}

void
GameDispatcher::postMessage(const GameMessage& event) except
{
	if (_enableMessagePosting)
	{
		_mutex.lock();
		_events.push(event);
		_mutex.unlock();

		_dispose.notify_one();
	}
}

void
GameDispatcher::peekMessages(GameMessage& event) noexcept
{
}

bool
GameDispatcher::pollMessages(GameMessage& event) noexcept
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (lock)
	{
		if (!_events.empty())
		{
			event = _events.front();
			_events.pop();
			return true;
		}
	}

	return false;
}

bool
GameDispatcher::waitMessages(GameMessage& event) noexcept
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_events.empty())
	{
		_dispose.wait(lock);
	}

	return this->pollMessages(event);
}

bool
GameDispatcher::waitMessages(GameMessage& event, int timeout) noexcept
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_events.empty())
	{
		_dispose.wait_for(lock, std::chrono::milliseconds(timeout));
	}

	return this->pollMessages(event);
}

void
GameDispatcher::flushMessage() noexcept
{
	_mutex.lock();
	_events = std::queue<GameMessage>();
	_mutex.unlock();
}

_NAME_END