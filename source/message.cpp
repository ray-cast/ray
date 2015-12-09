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
#include <ray/message.h>

_NAME_BEGIN

__ImplementSubClass(Message, rtti::Interface, "Message")
__ImplementSubClass(MessageBatch, Message, "MessageBatch")
__ImplementSubClass(MessageListener, rtti::Interface, "MessageListener")
__ImplementSubClass(MessageDispatcher, rtti::Interface, "MessageDispatcher")

Message::Message() noexcept
	: _instanceID(0)
{
}

Message::~Message() noexcept
{
}

void 
Message::setInstanceID(std::uint32_t instance) noexcept
{
	_instanceID = instance;
}

std::uint32_t 
Message::getInstanceID() const noexcept
{
	return _instanceID;
}

void 
Message::write(StreamWrite& stream) noexcept
{
}

void 
Message::read(StreamReader& stream) noexcept
{
}

MessageBatch::MessageBatch() noexcept
{
}

MessageBatch::~MessageBatch() noexcept
{
}

void 
MessageBatch::addMessage(MessagePtr message) noexcept
{
	_message.push_back(message);
}

const Messages& 
MessageBatch::getMessage() const noexcept
{
	return _message;
}

MessageListener::MessageListener() noexcept
{
}

MessageListener::~MessageListener() noexcept
{
}

void
MessageListener::onMessage(const MessagePtr& message) except
{
}

MessageDispatcher::MessageDispatcher() noexcept
	: _enableMessagePosting(true)
{
}

MessageDispatcher::~MessageDispatcher() noexcept
{
}

void
MessageDispatcher::enableMessagePosting(bool enable) noexcept
{
	_enableMessagePosting = enable;
}

bool
MessageDispatcher::enableMessagePosting() const noexcept
{
	return _enableMessagePosting;
}

void
MessageDispatcher::addMessageListener(MessageListenerPtr listener) noexcept
{
	if (listener)
	{
		auto it = std::find(_MessageListener.begin(), _MessageListener.end(), listener);
		if (it == _MessageListener.end())
		{
			_MessageListener.push_back(listener);
		}
	}
}

MessageListeners
MessageDispatcher::getMessageListeners() const noexcept
{
	return _MessageListener;
}

void
MessageDispatcher::sendMessage(const MessagePtr& event) except
{
	for (auto& it : _MessageListener)
	{
		it->onMessage(event);
	}
}

void
MessageDispatcher::postMessage(const MessagePtr& event) except
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
MessageDispatcher::peekMessages(MessagePtr& event) noexcept
{
}

bool
MessageDispatcher::pollMessages(MessagePtr& event) noexcept
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
MessageDispatcher::waitMessages(MessagePtr& event) noexcept
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_events.empty())
	{
		_dispose.wait(lock);
	}

	return this->pollMessages(event);
}

bool
MessageDispatcher::waitMessages(MessagePtr& event, int timeout) noexcept
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_events.empty())
	{
		_dispose.wait_for(lock, std::chrono::milliseconds(timeout));
	}

	return this->pollMessages(event);
}

void
MessageDispatcher::flushMessage() noexcept
{
	_mutex.lock();
	_events = std::queue<MessagePtr>();
	_mutex.unlock();
}

_NAME_END