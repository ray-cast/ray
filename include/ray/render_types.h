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
#ifndef _H_RENDER_TYPES_H_
#define _H_RENDER_TYPES_H_

#include <ray/graphics_types.h>

#include <ray/material.h>

_NAME_BEGIN

typedef std::shared_ptr<class RenderObject> RenderObjectPtr;
typedef std::shared_ptr<class RenderPostProcess> RenderPostProcessPtr;
typedef std::shared_ptr<class RenderPipelineController> RenderPipelineControllerPtr;
typedef std::shared_ptr<class RenderPipeline> RenderPipelinePtr;
typedef std::shared_ptr<class RenderPipelineManager> RenderPipelineManagerPtr;
typedef std::shared_ptr<class RenderDataManager> RenderDataManagerPtr;
typedef std::shared_ptr<class RenderMesh> RenderMeshPtr;
typedef std::shared_ptr<class RenderSystem> RenderSystemPtr;
typedef std::shared_ptr<class RenderBuffer> RenderBufferPtr;
typedef std::shared_ptr<class RenderScene> RenderScenePtr;

typedef std::shared_ptr<class Camera> CameraPtr;
typedef std::shared_ptr<class Light> LightPtr;

typedef std::weak_ptr<class Camera> CameraWeakPtr;
typedef std::weak_ptr<class Light> LightWeakPtr;
typedef std::weak_ptr<class RenderScene> RenderSceneWeakPtr;

typedef std::vector<RenderBufferPtr> RenderBuffers;
typedef std::vector<RenderMeshPtr> RenderMeshes;
typedef std::vector<RenderObjectPtr> RenderObjects;
typedef std::vector<RenderScenePtr> RenderScenes;
typedef std::vector<RenderPostProcessPtr> RenderPostProcessor;

typedef std::vector<CameraPtr> Cameras;
typedef std::vector<LightPtr> Lights;

enum CameraType
{
	CameraTypeOrtho,
	CameraTypePerspective,
	CameraTypeBeginRange = CameraTypeOrtho,
	CameraTypeEndRange = CameraTypePerspective,
	CameraTypeRangeSize = (CameraTypeEndRange - CameraTypeBeginRange + 1),
	CameraTypeMaxEnum = 0x7FFFFFFF
};

enum CameraOrder
{
	CameraOrderShadow,
	CameraOrderColor,
	CameraOrderNormal,
	CameraOrderLight,
	CameraOrderShading,
	CameraOrderCustom,
	CameraOrderCubeMap,
	CameraOrderMain,
	CameraOrderBeginRange = CameraOrderShadow,
	CameraOrderEndRange = CameraOrderMain,
	CameraOrderRangeSize = (CameraOrderEndRange - CameraOrderBeginRange + 1),
	CameraOrderMaxEnum = 0x7FFFFFFF
};

enum CameraRender
{
	CameraRenderScreen,
	CameraRenderTexture,
	CameraRenderCubeMap,
	CameraRenderBeginRange = CameraRenderScreen,
	CameraRenderEndRange = CameraRenderCubeMap,
	CameraRenderRangeSize = (CameraRenderEndRange - CameraRenderBeginRange + 1),
	CameraRenderMaxEnum = 0x7FFFFFFF
};

enum LightType
{
	LightTypeSun,
	LightTypeDirectional,
	LightTypeAmbient,
	LightTypePoint,
	LightTypeSpot,
	LightTypeArea,
	LightTypeHemiSphere,
	LightTypeBeginRange = LightTypeSun,
	LightTypeEndRange = LightTypeHemiSphere,
	LightTypeRangeSize = (LightTypeEndRange - LightTypeBeginRange + 1),
	LightTypeMaxEnum = 0x7FFFFFFF
};

_NAME_END

#endif