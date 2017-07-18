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
#ifndef _H_LIGHT_PROBE_H_
#define _H_LIGHT_PROBE_H_

#include <ray/SH.h>
#include <ray/render_object.h>

_NAME_BEGIN

class EXPORT LightProbe final : public RenderObject, public RenderListener
{
public:
	LightProbe() noexcept;
	~LightProbe() noexcept;

	void setSH9(const SH9& sh) noexcept;
	const SH9& getSH9() const noexcept;

	const CameraPtr& getCamera() const noexcept;

private:
	bool setupProbeCamera() noexcept;

	void _updateTransform() noexcept;
	void _updateBoundingBox() noexcept;

public:
	void onGenProbeBefore(const Camera& camera) noexcept;
	void onGenProbeAfter(const Camera& camera) noexcept;

private:
	virtual void onMoveAfter() noexcept;

	virtual void onSceneChangeBefore() noexcept;
	virtual void onSceneChangeAfter() noexcept;

	virtual bool onVisiableTest(const Camera& camera, const Frustum& fru) noexcept;

	virtual void onAddRenderData(RenderDataManager& manager) noexcept;

	virtual void onRenderObjectPre(const Camera& camera) noexcept;
	virtual void onRenderObjectPost(const Camera& camera) noexcept;

private:
	SH9 _sh;

	CameraPtr _camera;
};

_NAME_END

#endif