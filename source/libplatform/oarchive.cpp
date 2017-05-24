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
#include <ray/oarchive.h>

_NAME_BEGIN

oarchive::osentry::osentry(oarchive* _istr)
	: _ok(true)
	, _my_istr(_istr)
{
	if (_my_istr->rdbuf() != 0)
		_my_istr->rdbuf()->lock();
}

oarchive::osentry::~osentry() noexcept
{
	if (_my_istr->rdbuf() != 0)
		_my_istr->rdbuf()->unlock();
}

oarchive::osentry::operator bool() const noexcept
{
	return _ok ? true : false;
}

oarchive::oarchive(archivebuf* buf) noexcept
{
	archive::_init(buf, ios_base::out);
}

oarchive::~oarchive() noexcept
{
}

void
oarchive::emplace(type_t type) noexcept
{
	this->rdbuf()->emplace(type);
}
void
oarchive::clear() noexcept
{
	this->rdbuf()->clear();
}

archivebuf&
oarchive::at(const string_t& key) except
{
	return this->rdbuf()->at(key);
}

archivebuf&
oarchive::at(const string_t::value_type* key) except
{
	return this->rdbuf()->at(key);
}

archivebuf&
oarchive::at(const std::size_t n) except
{
	return this->rdbuf()->at(n);
}

const archivebuf&
oarchive::at(const string_t& key) const except
{
	return this->rdbuf()->at(key);
}

const archivebuf&
oarchive::at(string_t::const_pointer key) const except
{
	return this->rdbuf()->at(key);
}

const archivebuf&
oarchive::at(const std::size_t n) const except
{
	return this->rdbuf()->at(n);
}

oarchive&
oarchive::operator=(boolean_t value) except
{
	this->rdbuf()->operator=(value);
	return *this;
}

oarchive&
oarchive::operator=(number_integer_t value) except
{
	this->rdbuf()->operator=(value);
	return *this;
}

oarchive&
oarchive::operator=(number_unsigned_t value) except
{
	this->rdbuf()->operator=(value);
	return *this;
}

oarchive&
oarchive::operator=(number_float_t value) except
{
	this->rdbuf()->operator=(value);
	return *this;
}

oarchive&
oarchive::operator=(string_t&& value) except
{
	this->rdbuf()->operator=(value);
	return *this;
}

oarchive&
oarchive::operator=(const string_t& value) except
{
	this->rdbuf()->operator=(value);
	return *this;
}

oarchive&
oarchive::operator[](std::size_t n) except
{
	this->rdbuf()->operator[](n);
	return *this;
}

oarchive&
oarchive::operator[](const string_t& key) except
{
	this->rdbuf()->operator[](key);
	return *this;
}

oarchive&
oarchive::operator[](string_t::const_pointer key) except
{
	this->rdbuf()->operator[](key);
	return *this;
}

const oarchive&
oarchive::operator[](std::size_t n) const except
{
	this->rdbuf()->operator[](n);
	return *this;
}

const oarchive&
oarchive::operator[](const string_t& key) const except
{
	this->rdbuf()->operator[](key);
	return *this;
}

const oarchive&
oarchive::operator[](string_t::const_pointer key) const except
{
	this->rdbuf()->operator[](key);
	return *this;
}

_NAME_END