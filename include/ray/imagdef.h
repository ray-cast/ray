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
#ifndef _H_IMAGDEF_H_
#define _H_IMAGDEF_H_

#include <ray/iostream.h>
#include <ray/dccmn.h>

_NAME_BEGIN

#define _BUILD_BMP_HANDLER   1
#define _BUILD_DDS_HANDLER   1
#define _BUILD_JPEG_HANDLER  1
#define _BUILD_PNG_HANDLER   1
#define _BUILD_TGA_HANDLER   1

enum
{
	UNKNOWN_HANDLER,
	BMP_HANDLER,
	ICO_HANDLER,
	JPEG_HANDLER,
	PNG_HANDLER,
	GIF_HANDLER,
	IFF_HANDLER,
	PCX_HANDLER,
	TGA_HANDLER,
	DDS1_HANDLER,
	DDS2_HANDLER,
	DDS3_HANDLER,
	DDS4_HANDLER,
	DDS5_HANDLER,
	BC4U_HANDLER,
	BC4S_HANDLER,
	BC5S_HANDLER,
	ATI2_HANDLER,
	TIFF_HANDLER,
	XPM_HANDLER,
	WBMP_HANDLER,
	WMF_HANDLER,
};

_NAME_END

#endif