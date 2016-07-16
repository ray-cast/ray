// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#ifndef _H_OGL_SWAPCHAIN_H_
#define _H_OGL_SWAPCHAIN_H_

#if defined(_BUILD_OPENGL)
#	if defined(_BUILD_PLATFORM_WINDOWS)
#		include "wgl_swapchain.h"
#		define ToplevelSwapchain WGLSwapchain
#	elif defined(_BUILD_PLATFORM_LINUX)
#		include "x11_swapchain.h"
#		define ToplevelSwapchain XGLSwapchain
#	elif defined(_BUILD_PLATFORM_APPLE)
#		include "nsgl_swapchain.h"
#		define ToplevelSwapchain NSGLSwapchain
#	endif
#endif

_NAME_BEGIN

class OGLSwapchain final : public ToplevelSwapchain
{
	__DeclareSubClass(OGLSwapchain, ToplevelSwapchain)
public:
	OGLSwapchain() noexcept;
	~OGLSwapchain() noexcept;

private:
	OGLSwapchain(const OGLSwapchain&) noexcept = delete;
	OGLSwapchain& operator=(const OGLSwapchain&) noexcept = delete;
};

_NAME_END

#endif