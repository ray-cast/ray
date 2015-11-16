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
#ifndef _H_GAME_MESSAGE_H_
#define _H_GAME_MESSAGE_H_

#include <ray/game_types.h>

_NAME_BEGIN

class EXPORT GameEvent
{
public:
	GameEvent(const std::type_info& info) noexcept;
	GameEvent(const GameEvent& other) noexcept;
	virtual ~GameEvent() noexcept;

	virtual std::size_t code() const noexcept;

	GameEvent& operator=(const GameEvent& other) noexcept;

private:

	std::size_t _code;
};

inline bool operator==(const GameEvent& m1, const GameEvent& m2)
{
	return m1.code() == m2.code();
}

class EXPORT GameMessage final
{
public:
	GameMessage() noexcept;
	GameMessage(GameEvent* event) noexcept;
	GameMessage(const GameMessage& other) noexcept;
	~GameMessage() noexcept;

	GameEvent* event() const noexcept;

	GameMessage& operator=(const GameMessage& other) noexcept;

public:

	GameEvent* _event;
};

class EXPORT GameListener : public rtti::Interface
{
	__DeclareInterface(GameListener)
public:
	GameListener() noexcept;
	GameListener(const std::string& name) noexcept;
	virtual ~GameListener() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	virtual void load(iarchive& reader) except;
	virtual void save(oarchive& write) except;

	virtual void sendMessage(const GameMessage& message) except;

protected:

	virtual void onMessage(const GameMessage& message) except;

private:
	GameListener(const GameListener&) noexcept = delete;
	GameListener& operator=(const GameListener&) noexcept = delete;

private:

	std::string _name;
};

class EXPORT GameDispatcher
{
public:
	GameDispatcher() noexcept;
	virtual ~GameDispatcher() noexcept;

	virtual void enableMessagePosting(bool enable) noexcept;
	virtual bool enableMessagePosting() const noexcept;

	virtual void addGameListener(GameListenerPtr listener) noexcept;
	virtual GameListeners getGameListeners() const noexcept;
	virtual GameListenerPtr getGameListener(const std::string& name) const noexcept;

	virtual void sendMessage(const GameMessage& event) except;
	virtual void postMessage(const GameMessage& event) except;

	virtual void peekMessages(GameMessage& event) noexcept;
	virtual bool pollMessages(GameMessage& event) noexcept;
	virtual bool waitMessages(GameMessage& event) noexcept;
	virtual bool waitMessages(GameMessage& event, int timeout) noexcept;
	virtual void flushMessage() noexcept;

private:
	GameDispatcher(const GameDispatcher&) noexcept = delete;
	GameDispatcher& operator=(const GameDispatcher&) noexcept = delete;

private:

	typedef std::queue<GameMessage>  GameMessages;

	bool _enableMessagePosting;

	GameMessages _events;

	std::mutex _mutex;
	std::condition_variable _dispose;

	std::vector<GameListenerPtr> _gameListener;
};

_NAME_END

#endif