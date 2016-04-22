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
	: _ok(true)
	, _my_istr(_istr)
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

bool
iarchive::getValue(const std::string& name, bool& result) noexcept
{
    if (!this->rdbuf()->getValue(name, result))
		return false;
    return true;
}

bool
iarchive::getValue(const std::string& name, int1& result) noexcept
{
    if (!this->rdbuf()->getValue(name, result))
		return false;
    return true;
}

bool
iarchive::getValue(const std::string& name, int2& result) noexcept
{
    if (!this->rdbuf()->getValue(name, result))
		return false;
    return true;
}

bool
iarchive::getValue(const std::string& name, int3& result) noexcept
{
    if (!this->rdbuf()->getValue(name, result))
		return false;
    return true;
}

bool
iarchive::getValue(const std::string& name, int4& result) noexcept
{
    if (!this->rdbuf()->getValue(name, result))
		return false;
    return true;
}

bool
iarchive::getValue(const std::string& name, uint1& result) noexcept
{
	if (!this->rdbuf()->getValue(name, result))
		return false;
	return true;
}

bool
iarchive::getValue(const std::string& name, uint2& result) noexcept
{
	if (!this->rdbuf()->getValue(name, result))
		return false;
	return true;
}

bool
iarchive::getValue(const std::string& name, uint3& result) noexcept
{
	if (!this->rdbuf()->getValue(name, result))
		return false;
	return true;
}

bool
iarchive::getValue(const std::string& name, uint4& result) noexcept
{
	if (!this->rdbuf()->getValue(name, result))
		return false;
	return true;
}

bool
iarchive::getValue(const std::string& name, float1& result) noexcept
{
    if (!this->rdbuf()->getValue(name, result))
		return false;
    return true;
}

bool
iarchive::getValue(const std::string& name, float2& result) noexcept
{
    if (!this->rdbuf()->getValue(name, result))
		return false;
    return true;
}

bool
iarchive::getValue(const std::string& name, float3& result) noexcept
{
    if (!this->rdbuf()->getValue(name, result))
		return false;
    return true;
}

bool
iarchive::getValue(const std::string& name, float4& result) noexcept
{
    if (!this->rdbuf()->getValue(name, result))
		return false;
    return true;
}

bool
iarchive::getValue(const std::string& name, std::string& result) noexcept
{
    if (!this->rdbuf()->getValue(name, result))
		return false;
    return true;
}

_NAME_END