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
#include <ray/reference.h>

_NAME_BEGIN

__ImplementSubClass(Reference, rtti::Interface, "Reference")

Reference::Reference() noexcept
	: _pointer(nullptr)
	, _gc(false)
	, _count(1)
{
}

Reference::Reference(const Reference& copy) noexcept
	: _pointer(copy._pointer)
{
	_gc.store(copy._gc);
	_count.store(copy._count);
}

Reference::~Reference() noexcept
{
	assert(0 == this->_count);
}

void
Reference::addRef() noexcept
{
	++_count;
}

void 
Reference::release() noexcept
{
	assert(_count > 0);
	_gc = false;
	_count.fetch_sub(1);
	if (_count == 0)
		delete this;
}

int 
Reference::refCount() const noexcept
{
	return _count;
}

void
Reference::destroy() noexcept
{
	_gc = true;
}

bool
Reference::isDestroy() const noexcept
{
	return _gc;
}

void 
Reference::setUserPointer(std::intptr_t* pointer) noexcept
{
	_pointer = pointer;
}

std::intptr_t* 
Reference::getUserPointer() const noexcept
{
	return _pointer;
}

_NAME_END