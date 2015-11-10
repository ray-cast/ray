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
#ifndef _H_SCRIPT_OBJECT_H_
#define _H_SCRIPT_OBJECT_H_

#include <ray/script_system.h>

_NAME_BEGIN

class ScriptObject final
{
public:
	ScriptObject() noexcept;
	~ScriptObject() noexcept;

	bool open(const std::string& name);
	void close() noexcept;

	bool setInterface(const std::string& controller) noexcept;

	bool construct() noexcept;

	std::string getName() const noexcept;

	std::size_t getFactoryByDecl(const std::string& decl) noexcept;
	std::size_t getInterfaceByDecl(const char* decl) noexcept;
	std::size_t getMethodByDecl(const char* decl) noexcept;

	int setArgByte(std::size_t arg, std::uint8_t value) noexcept;
	int setArgWord(std::size_t arg, std::uint16_t value) noexcept;
	int setArgDWord(std::size_t arg, std::uint32_t value) noexcept;
	int setArgQWord(std::size_t arg, std::uint64_t value) noexcept;
	int setArgFloat(std::size_t arg, float value) noexcept;
	int setArgDouble(std::size_t arg, double value) noexcept;
	int setArgAddress(std::size_t arg, void *addr) noexcept;

	bool exce(std::size_t function) noexcept;

private:

	asIScriptModule* _module;
	asIScriptObject* _object;
	asIObjectType* _interface;

	std::vector<asIScriptFunction*> _functions;
};

_NAME_END

#endif