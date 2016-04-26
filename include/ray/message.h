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
#ifndef _H_MESSAGE_H_
#define _H_MESSAGE_H_

#include <ray/message_types.h>

_NAME_BEGIN

class EXPORT Message : public rtti::Interface
{
	__DeclareSubClass(Message, rtti::Interface)
public:
	Message() noexcept;
	virtual ~Message() noexcept;

	void setInstanceID(std::uint32_t instance) noexcept;
	std::uint32_t getInstanceID() const noexcept;

	virtual std::streamsize write(const char* str, std::streamsize cnt) noexcept;
	virtual std::streamsize read(char* str, std::streamsize cnt) noexcept;

private:

	std::uint32_t _instanceID;
};

class EXPORT MessageBatch : public Message
{
	__DeclareSubClass(MessageBatch, Message)
public:
	MessageBatch() noexcept;
	virtual ~MessageBatch() noexcept;

	void addMessage(MessagePtr message) noexcept;
	const Messages& getMessage() const noexcept;
private:

	Messages _message;
};

class EXPORT MessageListener : public rtti::Interface
{
	__DeclareSubClass(MessageListener, rtti::Interface)
public:
	MessageListener() noexcept;
	virtual ~MessageListener() noexcept;

	virtual void onMessage(const MessagePtr& message) except;

private:
	MessageListener(const MessageListener&) noexcept = delete;
	MessageListener& operator=(const MessageListener&) noexcept = delete;
};

class EXPORT MessageDispatcher : public rtti::Interface
{
	__DeclareSubClass(MessageDispatcher, rtti::Interface)
public:
	MessageDispatcher() noexcept;
	virtual ~MessageDispatcher() noexcept;

	virtual void enableMessagePosting(bool enable) noexcept;
	virtual bool enableMessagePosting() const noexcept;

	virtual void addMessageListener(MessageListenerPtr listener) noexcept;
	virtual MessageListeners getMessageListeners() const noexcept;

	virtual void sendMessage(const MessagePtr& event) except;
	virtual void postMessage(const MessagePtr& event) except;

	virtual void peekMessages(MessagePtr& event) noexcept;
	virtual bool pollMessages(MessagePtr& event) noexcept;
	virtual bool waitMessages(MessagePtr& event) noexcept;
	virtual bool waitMessages(MessagePtr& event, int timeout) noexcept;
	virtual void flushMessage() noexcept;

private:
	MessageDispatcher(const MessageDispatcher&) noexcept = delete;
	MessageDispatcher& operator=(const MessageDispatcher&) noexcept = delete;

private:

	typedef std::queue<MessagePtr>  Messages;

	bool _enableMessagePosting;

	Messages _events;

	std::mutex _mutex;
	std::condition_variable _dispose;

	std::vector<MessageListenerPtr> _MessageListener;
};

template<class _Ty, class... _Types>
inline typename std::enable_if<!std::is_array<_Ty>::value, std::shared_ptr<_Ty> >::type make_message(_Types&&... _Args)
{
	return std::make_shared<_Ty>(std::forward<_Types>(_Args)...);
}

_NAME_END

#endif