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
#include <ray/iarchive.h>

_NAME_BEGIN

iarchive::isentry::isentry(iarchive* _istr)
	: _my_istr(_istr)
	, _ok(true)
{
	if (_my_istr->rdbuf() != 0)
		_my_istr->rdbuf()->lock();
}

iarchive::isentry::~isentry() noexcept
{
	if (_my_istr->rdbuf() != 0)
		_my_istr->rdbuf()->unlock();
};

iarchive::isentry::operator bool() const noexcept
{
	return _ok ? true : false;
}

iarchive::iarchive(archivebuf* buf) noexcept
{
	archive::_init(buf, ios_base::in);
}

iarchive::~iarchive() noexcept
{
}

bool
iarchive::is_open() const noexcept
{
	return this->rdbuf()->is_open();
}

iarchive&
iarchive::open(StreamReader& stream) noexcept
{
	const isentry ok(this);
	if (ok)
	{
		if (!this->rdbuf()->open(stream))
			this->setstate(ios_base::failbit);
		else
			this->clear(ios_base::goodbit);
	}

	return (*this);
}

iarchive&
iarchive::close() noexcept
{
	const isentry ok(this);
	if (ok)
	{
		if (!this->fail())
			this->rdbuf()->close();
	}

	return (*this);
}

std::string
iarchive::getCurrentNodeName() const noexcept
{
	return this->rdbuf()->getCurrentNodeName();
}

std::string
iarchive::getCurrentNodePath() const noexcept
{
	return this->rdbuf()->getCurrentNodePath();
}

void
iarchive::setToNode(const std::string& path) noexcept
{
	return this->rdbuf()->setToNode(path);
}

bool
iarchive::setToFirstChild() noexcept
{
	return this->rdbuf()->setToFirstChild();
}

bool
iarchive::setToFirstChild(const std::string& name) noexcept
{
	return this->rdbuf()->setToFirstChild(name);
}

bool
iarchive::setToNextChild() noexcept
{
	return this->rdbuf()->setToNextChild();
}

bool
iarchive::setToNextChild(const std::string& name) noexcept
{
	return this->rdbuf()->setToNextChild(name);
}

bool
iarchive::setToParent() noexcept
{
	return this->rdbuf()->setToParent();
}

bool
iarchive::hasChild() const noexcept
{
	return this->rdbuf()->hasChild();
}

bool
iarchive::hasAttr(const char* name) const noexcept
{
	return this->rdbuf()->hasAttr(name);
}

void
iarchive::clearAttrs() noexcept
{
	return this->rdbuf()->clearAttrs();
}

void
iarchive::addAttrs() noexcept
{
	this->rdbuf()->addAttrs();
}

void
iarchive::addAttrsInChildren() noexcept
{
	this->rdbuf()->addAttrsInChildren();
}

void
iarchive::addAttrsInChildren(const std::string& key) noexcept
{
	this->rdbuf()->addAttrsInChildren();
}

const std::vector<std::string>&
iarchive::getAttrList() const noexcept
{
	return this->rdbuf()->getAttrList();
}

std::string
iarchive::getText() const noexcept
{
	return this->rdbuf()->getText();
}

iarchive&
iarchive::getValue(const std::string& name, bool& result) noexcept
{
    const isentry ok(this);
    if (ok)
    {
        if (!this->bad() && this->rdbuf()->getValue(name, result))
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

iarchive&
iarchive::getValue(const std::string& name, int1& result) noexcept
{
    const isentry ok(this);
    if (ok)
    {
        if (!this->bad() && this->rdbuf()->getValue(name, result))
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

iarchive&
iarchive::getValue(const std::string& name, int2& result) noexcept
{
    const isentry ok(this);
    if (ok)
    {
        if (!this->bad() && this->rdbuf()->getValue(name, result))
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

iarchive&
iarchive::getValue(const std::string& name, int3& result) noexcept
{
    const isentry ok(this);
    if (ok)
    {
        if (!this->bad() && this->rdbuf()->getValue(name, result))
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

iarchive&
iarchive::getValue(const std::string& name, int4& result) noexcept
{
    const isentry ok(this);
    if (ok)
    {
        if (!this->bad() && this->rdbuf()->getValue(name, result))
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

iarchive&
iarchive::getValue(const std::string& name, float1& result) noexcept
{
    const isentry ok(this);
    if (ok)
    {
        if (!this->bad() && this->rdbuf()->getValue(name, result))
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

iarchive&
iarchive::getValue(const std::string& name, float2& result) noexcept
{
    const isentry ok(this);
    if (ok)
    {
        if (!this->bad() && this->rdbuf()->getValue(name, result))
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

iarchive&
iarchive::getValue(const std::string& name, float3& result) noexcept
{
    const isentry ok(this);
    if (ok)
    {
        if (!this->bad() && this->rdbuf()->getValue(name, result))
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

iarchive&
iarchive::getValue(const std::string& name, float4& result) noexcept
{
    const isentry ok(this);
    if (ok)
    {
        if (!this->bad() && this->rdbuf()->getValue(name, result))
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

iarchive&
iarchive::getValue(const std::string& name, std::string& result) noexcept
{
    const isentry ok(this);
    if (ok)
    {
        if (!this->bad() && this->rdbuf()->getValue(name, result))
            this->setstate(ios_base::failbit);
    }

    return (*this);
}

_NAME_END