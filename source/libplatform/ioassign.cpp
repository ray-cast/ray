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
#include <ray/ioassign.h>

_NAME_BEGIN

IoAssign::IoAssign() noexcept
{
}

IoAssign::IoAssign(util::string&& _name, util::string&& _path) noexcept
	: name(std::move(_name))
	, path(std::move(_path))
{
}

IoAssign::IoAssign(const util::string& _name, const util::string& _path) noexcept
	: name(_name)
	, path(_path)
{
}

IoAssign::IoAssign(const util::string::pointer _name, const util::string::pointer _path) noexcept
	: name(_name)
	, path(_path)
{
}

IoAssign::IoAssign(std::pair<util::string, util::string>&& argv) noexcept
	: name(std::move(argv.first))
	, path(std::move(argv.second))
{
}

IoAssign::IoAssign(std::initializer_list<util::string>& argv) noexcept
	: name(*argv.begin())
	, path(*(argv.begin() + 1))
{
	assert(argv.size() == 2);
}

IoAssign::IoAssign(std::initializer_list<util::string>&& argv) noexcept
	: name(std::move(*argv.begin()))
	, path(std::move(*(argv.begin() + 1)))
{
	assert(argv.size() == 2);
}

IoAssign::~IoAssign() noexcept
{
}

_NAME_END