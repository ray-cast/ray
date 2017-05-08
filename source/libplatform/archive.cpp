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
#include <ray/archive.h>

_NAME_BEGIN

archive::archive() noexcept
{
}

archive::~archive() noexcept
{
}

archivebuf*
archive::rdbuf() const noexcept
{
	return _strbuf;
}

void
archive::set_rdbuf(archivebuf* buf) noexcept
{
	_strbuf = buf;
}

void
archive::_init(archivebuf* _buf, ios_base::openmode mode) noexcept
{
	this->set_rdbuf(_buf);
	_mode = mode;
	ios_base::_init();
}

void
archive::setOpenMode(ios_base::openmode mode) noexcept
{
	_mode = mode;
}

ios_base::openmode
archive::getOpenMode() const noexcept
{
	return _mode;
}

std::string
archive::getCurrentNodeName() const noexcept
{
	return this->rdbuf()->getCurrentNodeName();
}

std::string
archive::getCurrentNodePath() const noexcept
{
	return this->rdbuf()->getCurrentNodePath();
}

bool
archive::setToNode(const std::string& path) noexcept
{
	return this->rdbuf()->setToNode(path);
}

bool
archive::setToFirstChild() noexcept
{
	return this->rdbuf()->setToFirstChild();
}

bool
archive::setToFirstChild(const std::string& name) noexcept
{
	return this->rdbuf()->setToFirstChild(name);
}

bool
archive::setToNextChild() noexcept
{
	return this->rdbuf()->setToNextChild();
}

bool
archive::setToNextChild(const std::string& name) noexcept
{
	return this->rdbuf()->setToNextChild(name);
}

bool
archive::setToParent() noexcept
{
	return this->rdbuf()->setToParent();
}

bool
archive::setToRoot() noexcept
{
	return this->rdbuf()->setToRoot();
}

bool
archive::hasChild() const noexcept
{
	return this->rdbuf()->hasChild();
}

_NAME_END