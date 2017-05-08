// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#ifndef _H_ARCHIVE_H_
#define _H_ARCHIVE_H_

#include <ray/archive_buf.h>

_NAME_BEGIN

class EXPORT archive : public ios_base
{
public:
	archive() noexcept;
	virtual ~archive() noexcept;

	void setOpenMode(ios_base::openmode mode) noexcept;
	ios_base::openmode getOpenMode() const noexcept;

	archivebuf* rdbuf() const noexcept;
	void set_rdbuf(archivebuf* buf) noexcept;

	std::string getCurrentNodeName() const noexcept;
	std::string getCurrentNodePath() const noexcept;

	bool setToNode(const std::string& path) noexcept;
	bool setToFirstChild() noexcept;
	bool setToFirstChild(const std::string& name) noexcept;
	bool setToNextChild() noexcept;
	bool setToNextChild(const std::string& name) noexcept;
	bool setToParent() noexcept;
	bool setToRoot() noexcept;

	bool hasChild() const noexcept;

protected:
	void _init(archivebuf* _buf, ios_base::openmode mode) noexcept;

private:
	archive& operator=(const archive&) = delete;
	archive(const archive&) = delete;

private:
	archivebuf* _strbuf;
	ios_base::openmode _mode;
};

template <class T> inline
std::pair<const char*, T&> make_archive(T&& value, const char* name)
{
	return std::pair<const char*, T&>(name, std::forward<T>(value));
}

template <class T> inline
std::pair<std::string, T> make_archive(T&& value, const std::string& name)
{
	return std::pair<const std::string&, T&>(name.c_str(), std::forward<T>(value));
}

_NAME_END

#endif