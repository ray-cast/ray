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
#include <ray/version.h>
#include <ray/revision.h>

_NAME_BEGIN

static const char* LEGAL_INFORMATION =

"Open Source Library (Ray).\n"
"A free C++ library to 3D engine into applications\n\n"

"(c) 2013-2016, ray team\n"
"License under the terms and conditions of the 3-clause BSD license\n"
;

const char* GetVersionString()
{
	return LEGAL_INFORMATION;
}

unsigned int GetVersionMinor()
{
	return 0;
}

unsigned int GetVersionMajor()
{
	return 3;
}

unsigned int GetVersionRevision()
{
	return GITRevision;
}

unsigned int GetVersionCompileFlags()
{
	unsigned int flags = 0;

#ifdef _BUILD_BOOST_WORKAROUND
	flags |= CFLAGS_NOBOOST;
#endif
#ifdef _BUILD_SINGLETHREADED
	flags |= CFLAGS_SINGLETHREADED;
#endif
#ifdef _BUILD_DEBUG
	flags |= CFLAGS_DEBUG;
#endif
#ifdef _BUILD_DLL_EXPORT
	flags |= CFLAGS_SHARED;
#endif
#ifdef _STLPORT_VERSION
	flags |= CFLAGS_STLPORT;
#endif

	return flags;
}

_NAME_END