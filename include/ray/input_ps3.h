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
#ifndef _H_INPUT_PS3_H_
#define _H_INPUT_PS3_H_

#include <ray/input_types.h>

_NAME_BEGIN

class InputPS3
{
public:
	enum Code
	{
		// PS3 controller.
		PS3_SELECT,
		PS3_L3,
		PS3_R3,
		PS3_START,
		PS3_UP,
		PS3_RIGHT,
		PS3_DOWN,
		PS3_LEFT,
		PS3_L2,
		PS3_R2,
		PS3_L1,
		PS3_R1,
		PS3_TRIANGLE,
		PS3_CIRCLE,
		PS3_CROSS,
		PS3_SQUARE,
		PS3_STICKLX,
		PS3_STICKLY,
		PS3_STICKRX,
		PS3_STICKRY,
		PS3_ROTX,
		PS3_ROTY,
		PS3_ROTZ,
		PS3_ROTX_KEYL,
		PS3_ROTX_KEYR,
		PS3_ROTZ_KEYD,
		PS3_ROTZ_KEYU,

		NumKeyCodes,
	};
};

_NAME_END

#endif