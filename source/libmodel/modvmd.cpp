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
#include "modvmd.h"

#include <ray/anim.h>
#include <iconv.h>

_NAME_BEGIN

static bool VMDMotionSorter(const VMDMotion &m1, const VMDMotion &m2)
{
	return m1.frame < m2.frame;
}

static bool VMDMorphSorter(const VMDMorph &m1, const VMDMorph &m2)
{
	return m1.frame < m2.frame;
}

static bool VMDCameraFrameSorter(const VMDCamera &m1, const VMDCamera &m2)
{
	return m1.frame < m2.frame;
}

static bool VMDLightFrameSorter(const VMDLight &m1, const VMDLight &m2)
{
	return m1.frame < m2.frame;
}

static bool VMDSelfShadowFrameSorter(const VMDSelfShadow& m1, const VMDSelfShadow& m2)
{
	return m1.frame < m2.frame;
}

VMDHandler::VMDHandler() noexcept
{
}

VMDHandler::~VMDHandler() noexcept
{
}

bool
VMDHandler::doCanRead(StreamReader& stream) const noexcept
{
	assert(sizeof(VMDMotion) == 111);
	assert(sizeof(VMDMorph) == 23);
	assert(sizeof(VMDCamera) == 61);
	assert(sizeof(VMDLight) == 28);
	assert(sizeof(VMDSelfShadow) == 9);

	VMD_Header hdr;

	if (stream.read((char*)&hdr, sizeof(hdr)))
	{
		if (std::strncmp(hdr.magic, "Vocaloid Motion Data 0002", 30) == 0)
		{
			return true;
		}
	}

	return false;
}

bool
VMDHandler::doLoad(Model& model, StreamReader& stream) noexcept
{
	VMD vmd;

	if (!stream.read((char*)&vmd.Header, sizeof(vmd.Header))) return false;

	if (!stream.read((char*)&vmd.NumMotion, sizeof(vmd.NumMotion))) return false;

	if (vmd.NumMotion > 0)
	{
		vmd.MotionLists.resize(vmd.NumMotion);

		if (!stream.read((char*)vmd.MotionLists.data(), sizeof(VMDMotion) * vmd.NumMotion)) return false;

		std::sort(vmd.MotionLists.begin(), vmd.MotionLists.end(), VMDMotionSorter);
	}

	if (!stream.read((char*)&vmd.NumMorph, sizeof(vmd.NumMorph))) return false;
	if (vmd.NumMorph > 0)
	{
		vmd.MorphLists.resize(vmd.NumMorph);

		if (!stream.read((char*)vmd.MorphLists.data(), sizeof(VMDMorph) * vmd.NumMorph)) return false;

		std::sort(vmd.MorphLists.begin(), vmd.MorphLists.end(), VMDMorphSorter);
	}

	if (!stream.read((char*)&vmd.NumCamera, sizeof(vmd.NumCamera))) return false;
	if (vmd.NumCamera > 0)
	{
		vmd.CameraLists.resize(vmd.NumCamera);

		if (!stream.read((char*)vmd.CameraLists.data(), sizeof(VMDCamera) * vmd.NumCamera)) return false;

		std::sort(vmd.CameraLists.begin(), vmd.CameraLists.end(), VMDCameraFrameSorter);
	}

	if (!stream.read((char*)&vmd.NumLight, sizeof(vmd.NumLight))) return false;
	if (vmd.NumLight > 0)
	{
		vmd.LightLists.resize(vmd.NumLight);

		if (!stream.read((char*)vmd.LightLists.data(), sizeof(VMDLight) * vmd.NumLight)) return false;

		std::sort(vmd.LightLists.begin(), vmd.LightLists.end(), VMDLightFrameSorter);
	}

	if (!stream.read((char*)&vmd.NumSelfShadow, sizeof(vmd.NumSelfShadow))) return false;
	if (vmd.NumSelfShadow > 0)
	{
		vmd.SelfShadowLists.resize(vmd.NumSelfShadow);

		if (!stream.read((char*)vmd.SelfShadowLists.data(), sizeof(VMDSelfShadow) * vmd.NumSelfShadow)) return false;

		std::sort(vmd.SelfShadowLists.begin(), vmd.SelfShadowLists.end(), VMDSelfShadowFrameSorter);
	}

	auto animtion = std::make_shared<AnimationProperty>();

	for (auto& it : vmd.MotionLists)
	{
		char inbuf[MAX_PATH + 1] = { 0 };
		char outbuf[MAX_PATH + 1] = { 0 };
		char *in = inbuf;
		char *out = outbuf;
		std::size_t in_size = (size_t)MAX_PATH;
		std::size_t out_size = (size_t)MAX_PATH;

		memcpy(in, it.name, sizeof(it.name));

		iconv_t ic = iconv_open("GBK", "SJIS");
		iconv(ic, &in, &in_size, &out, &out_size);
		iconv_close(ic);

		BoneAnimation anim;
		anim.setName(outbuf);
		anim.setPosition(it.location);
		anim.setRotation(it.rotate);
		anim.setFrameNo(it.frame);

		Interpolation interp;

		// http://harigane.at.webry.info/201103/article_1.html
		for (int k = 0; k < 4; k++)
		{
			interp.interpX[k] = it.interpolation[k * 4 + 0];
			interp.interpY[k] = it.interpolation[k * 4 + 1];
			interp.interpZ[k] = it.interpolation[k * 4 + 2];
			interp.interpW[k] = it.interpolation[k * 4 + 3];
		}

		anim.setInterpolation(interp);

		animtion->addBoneAnimation(anim);
	}

	model.addAnimtion(animtion);

	return true;
}

bool
VMDHandler::doSave(Model& model, StreamWrite& stream) noexcept
{
	VMD vmd;
	std::memset(&vmd.Header, 0, sizeof(vmd.Header));
	std::memcpy(vmd.Header.magic, "Vocaloid Motion Data 0002", 15);

	stream.write((char*)&vmd.Header, sizeof(vmd.Header));

	return true;
}

_NAME_END