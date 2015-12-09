// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#ifndef _H_MODVMD_H_
#define _H_MODVMD_H_

#include <ray/model.h>

_NAME_BEGIN

#pragma pack(push)
#pragma pack(1)

typedef Vector2 VMD_Vector2;
typedef Vector3 VMD_Vector3;
typedef Vector4 VMD_Vector4;
typedef Quaternion VMD_Quaternion;

typedef Vector3 VMD_Color3;
typedef Vector4 VMD_Color4;

typedef char          VMD_char;
typedef std::int8_t   VMD_int8_t;
typedef std::uint8_t  VMD_uint8_t;
typedef std::uint16_t VMD_uint16_t;
typedef std::uint32_t VMD_uint32_t;

typedef float VMD_Float;

struct VMD_Header
{
	VMD_char magic[30];
	VMD_char name[20];
};

struct VMDMotion
{
	VMD_char name[15];
	VMD_uint32_t frame;
	VMD_Vector3 location;
	VMD_Quaternion rotate;
	VMD_uint8_t interpolation[64];
};

struct VMDMorph
{
	VMD_char name[15];
	VMD_uint32_t frame;
	VMD_Float weight;
};

struct VMDCamera
{
	VMD_uint32_t frame;
	VMD_Float length;
	VMD_Vector3 location;
	VMD_Vector3 rotation;
	VMD_uint8_t interpolation[24];
	VMD_uint32_t viewingAngle;
	VMD_uint8_t perspective;
};

struct VMDLight
{
	VMD_uint32_t frame;
	VMD_Vector3 rgb;
	VMD_Vector3 location;
};

struct VMDSelfShadow
{
	VMD_uint32_t frame;
	VMD_uint8_t mode; // 00-02
	VMD_Float distance; // 0.1 - (dist * 0.000001)
};

#pragma pack(pop)

struct VMD
{
	VMD_Header Header;

	VMD_uint32_t NumMotion;
	std::vector<VMDMotion> MotionLists;

	VMD_uint32_t NumMorph;
	std::vector<VMDMorph> MorphLists;

	VMD_uint32_t NumCamera;
	std::vector<VMDCamera> CameraLists;

	VMD_uint32_t NumLight;
	std::vector<VMDLight> LightLists;

	VMD_uint32_t NumSelfShadow;
	std::vector<VMDSelfShadow> SelfShadowLists;
};

class VMDHandler final : public ModelHandler
{
public:
	VMDHandler() noexcept;
	~VMDHandler();

	bool doCanRead(StreamReader& stream) const noexcept;

	bool doLoad(Model& image, StreamReader& stream) noexcept;
	bool doSave(Model& image, StreamWrite& stream) noexcept;
};

_NAME_END

#endif