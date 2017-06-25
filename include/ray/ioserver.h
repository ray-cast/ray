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
#ifndef _H_IO_SERVER_H_
#define _H_IO_SERVER_H_

#include <ray/package.h>
#include <ray/ioassign.h>

_NAME_BEGIN

class EXPORT IoServer : public ios_base
{
	__DeclareSingleton(IoServer)
public:
	IoServer() noexcept;
	~IoServer() noexcept;

	IoServer& addIoListener(IoListenerPtr& listener) noexcept;
	IoServer& removeIoListener(IoListenerPtr& listener) noexcept;

	IoServer& mountArchives() noexcept;
	IoServer& unmountArchives() noexcept;

	IoServer& addAssign(const IoAssign& assign) noexcept;
	IoServer& removeAssign(const util::string& name) noexcept;
	IoServer& getAssign(const util::string& name, util::string& path) noexcept;

	IoServer& getResolveAssign(const util::string& url, util::string& resolvePath) noexcept;
	IoServer& getResolveAssign(util::string::const_pointer url, util::string& resolvePath) noexcept;

	IoServer& openFileURL(StreamReaderPtr& stream, const util::string& path, open_mode mode = ios_base::in) noexcept;
	IoServer& openFileURL(StreamReaderPtr& stream, util::string::const_pointer path, open_mode mode = ios_base::in) noexcept;

	IoServer& openFileFromDisk(StreamReaderPtr& stream, const std::string& resolvePath, open_mode mode = ios_base::in) noexcept;
	IoServer& openFileFromDisk(StreamReaderPtr& stream, std::string::const_pointer resolvePath, open_mode mode = ios_base::in) noexcept;
	IoServer& openFileFromDisk(StreamReaderPtr& stream, const std::wstring& resolvePath, open_mode mode = ios_base::in) noexcept;
	IoServer& openFileFromDisk(StreamReaderPtr& stream, std::wstring::const_pointer resolvePath, open_mode mode = ios_base::in) noexcept;

	IoServer& openFileFromDiskUTF8(StreamReaderPtr& stream, const std::string& resolvePath, open_mode mode = ios_base::in) noexcept;
	IoServer& openFileFromDiskUTF8(StreamReaderPtr& stream, std::string::const_pointer resolvePath, open_mode mode = ios_base::in) noexcept;

	IoServer& openFileFromDiskURL(StreamReaderPtr& stream, const util::string& url, open_mode mode = ios_base::in) noexcept;
	IoServer& openFileFromDiskURL(StreamReaderPtr& stream, util::string::const_pointer url, open_mode mode = ios_base::in) noexcept;

	IoServer& openFileFromFileSystem(StreamReaderPtr& stream, const util::string& path, open_mode mode = ios_base::in) noexcept;
	IoServer& openFileFromFileSystem(StreamReaderPtr& stream, util::string::const_pointer path, open_mode mode = ios_base::in) noexcept;

	IoServer& saveFileToDisk(StreamWritePtr& stream, const std::string& resolvePath, open_mode mode = ios_base::out) noexcept;
	IoServer& saveFileToDisk(StreamWritePtr& stream, std::string::const_pointer resolvePath, open_mode mode = ios_base::out) noexcept;
	IoServer& saveFileToDisk(StreamWritePtr& stream, const std::wstring& resolvePath, open_mode mode = ios_base::out) noexcept;
	IoServer& saveFileToDisk(StreamWritePtr& stream, std::wstring::const_pointer resolvePath, open_mode mode = ios_base::out) noexcept;

	IoServer& saveFileToDiskUTF8(StreamWritePtr& stream, const std::string& resolvePath, open_mode mode = ios_base::out) noexcept;
	IoServer& saveFileToDiskUTF8(StreamWritePtr& stream, std::string::const_pointer resolvePath, open_mode mode = ios_base::out) noexcept;

	IoServer& saveFileToDiskURL(StreamWritePtr& stream, const util::string& url, open_mode mode = ios_base::out) noexcept;
	IoServer& saveFileToDiskURL(StreamWritePtr& stream, util::string::const_pointer url, open_mode mode = ios_base::out) noexcept;

	IoServer& openFile(StreamPtr& stream, const util::string& path, open_mode mode = ios_base::in) noexcept;
	IoServer& openFileFromFileSystem(StreamPtr& stream, const util::string& path, open_mode mode = ios_base::in) noexcept;
	IoServer& openFileFromDisk(StreamPtr& stream, const util::string& path, open_mode mode = ios_base::in) noexcept;

	IoServer& deleteFile(const util::string& path) noexcept;

	IoServer& existsFile(const std::string& path) noexcept;
	IoServer& existsFileFromFileSystem(const std::string& path) noexcept;

	IoServer& existsFileFromDisk(const std::string& path, open_mode mode = 0) noexcept;
	IoServer& existsFileFromDisk(const std::wstring& path, open_mode mode = 0) noexcept;
	IoServer& existsFileFromDisk(std::string::const_pointer path, open_mode mode = 0) noexcept;
	IoServer& existsFileFromDisk(std::wstring::const_pointer path, open_mode mode = 0) noexcept;

	IoServer& existsFileFromDiskURL(const util::string& url, open_mode mode = 0) noexcept;
	IoServer& existsFileFromDiskURL(util::string::const_pointer url, open_mode mode = 0) noexcept;

	IoServer& copyFile(const util::string& path, const util::string& to) noexcept;

	IoServer& createDirectory(const util::string& path) noexcept;
	IoServer& deleteDirectory(const util::string& path) noexcept;
	IoServer& existsDirectory(const util::string& path) noexcept;

private:

	bool _enablePackage;

	std::vector<IoListenerPtr> _ioListener;
	std::map<util::string, util::string> _assignTable;
};

_NAME_END

#endif