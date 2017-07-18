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
#ifndef _H_RENDER_TYPES_H_
#define _H_RENDER_TYPES_H_

#include <ray/graphics_types.h>

_NAME_BEGIN

typedef std::shared_ptr<class Material> MaterialPtr;
typedef std::shared_ptr<class MaterialSemantic> MaterialSemanticPtr;
typedef std::shared_ptr<class MaterialPass> MaterialPassPtr;
typedef std::shared_ptr<class MaterialTech> MaterialTechPtr;
typedef std::shared_ptr<class MaterialParam> MaterialParamPtr;
typedef std::shared_ptr<class MaterialMacro> MaterialMacroPtr;
typedef std::shared_ptr<class MaterialSemantic> MaterialSemanticPtr;
typedef std::shared_ptr<class MaterialSemanticManager> MaterialSemanticManagerPtr;
typedef std::shared_ptr<class MaterialDesc> MaterialDescPtr;
typedef std::shared_ptr<class MaterialPassDesc> MaterialPassDescPtr;
typedef std::shared_ptr<class MaterialTechDesc> MaterialTechDescPtr;
typedef std::shared_ptr<class MaterialParamDesc> MaterialParamDescPtr;
typedef std::shared_ptr<class MaterialVariant> MaterialVariantPtr;
typedef std::shared_ptr<class MaterialLoader> MaterialLoaderPtr;
typedef std::shared_ptr<class MaterialManager> MaterialManagerPtr;
typedef std::shared_ptr<class Geometry> GeometryPtr;
typedef std::shared_ptr<class Camera> CameraPtr;
typedef std::shared_ptr<class Light> LightPtr;
typedef std::shared_ptr<class LightProbe> LightProbePtr;
typedef std::shared_ptr<class RenderSystem> RenderSystemPtr;
typedef std::shared_ptr<class RenderPipelineStage> RenderPipelineStagePtr;
typedef std::shared_ptr<class RenderScene> RenderScenePtr;
typedef std::shared_ptr<class RenderObject> RenderObjectPtr;
typedef std::shared_ptr<class RenderPostProcess> RenderPostProcessPtr;
typedef std::shared_ptr<class RenderDataManager> RenderDataManagerPtr;
typedef std::shared_ptr<class RenderPipeline> RenderPipelinePtr;
typedef std::shared_ptr<class RenderPipelineDevice> RenderPipelineDevicePtr;
typedef std::shared_ptr<class RenderPipelineController> RenderPipelineControllerPtr;
typedef std::shared_ptr<class RenderPipelineManager> RenderPipelineManagerPtr;
typedef std::shared_ptr<class RenderPipelineFramebuffer> RenderPipelineFramebufferPtr;

typedef std::weak_ptr<class Material> MaterialWeakPtr;
typedef std::weak_ptr<class MaterialPass> MaterialPassWeakPtr;
typedef std::weak_ptr<class MaterialTech> MaterialTechWeakPtr;
typedef std::weak_ptr<class MaterialParam> MaterialParamWeakPtr;
typedef std::weak_ptr<class MaterialVariant> MaterialVariantWeakPtr;
typedef std::weak_ptr<class MaterialLoader> MaterialLoaderWeakPtr;
typedef std::weak_ptr<class MaterialManager> MaterialManagerWeakPtr;
typedef std::weak_ptr<class Geometry> GeometryWeakPtr;
typedef std::weak_ptr<class RenderSystem> RenderSystemWeakPtr;
typedef std::weak_ptr<class RenderPipelineStage> RenderPipelineStageWeakPtr;
typedef std::weak_ptr<class RenderScene> RenderSceneWeakPtr;
typedef std::weak_ptr<class Camera> CameraWeakPtr;
typedef std::weak_ptr<class Light> LightWeakPtr;
typedef std::weak_ptr<class LightProbe> LightProbeWeakPtr;
typedef std::weak_ptr<class RenderObject> RenderObjectWeakPtr;
typedef std::weak_ptr<class RenderPostProcess> RenderPostProcessWeakPtr;
typedef std::weak_ptr<class RenderPipelineController> RenderPipelineControllerWeakPtr;
typedef std::weak_ptr<class RenderPipelineDevice> RenderPipelineWeakPtr;
typedef std::weak_ptr<class RenderPipelineManager> RenderPipelineManagerWeakPtr;
typedef std::weak_ptr<class RenderPipelineFramebuffer> RenderPipelineFramebufferWeakPtr;
typedef std::weak_ptr<class RenderDataManager> RenderDataManagerWeakPtr;

typedef std::vector<RenderPipelineStagePtr> RenderPipelineStages;
typedef std::vector<GeometryPtr> Geometryes;
typedef std::vector<RenderObjectPtr> RenderObjects;
typedef std::vector<RenderScene*> RenderScenes;
typedef std::vector<RenderPostProcessPtr> RenderPostProcessor;
typedef std::vector<CameraPtr> Cameras;
typedef std::vector<LightPtr> Lights;
typedef std::vector<LightProbePtr> LightProbes;

typedef std::vector<MaterialPtr> Materials;
typedef std::vector<MaterialPassPtr> MaterialPassList;
typedef std::vector<MaterialTechPtr> MaterialTechniques;
typedef std::vector<MaterialMacroPtr> MaterialMacros;
typedef std::vector<MaterialPassDescPtr> MaterialPassDescList;
typedef std::vector<MaterialTechDescPtr> MaterialTechniqueDescList;
typedef std::vector<MaterialVariantPtr> MaterialVariants;
typedef std::map<std::string, MaterialParamPtr> MaterialParams;

typedef std::vector<RenderObject*> RenderObjectRaws;

enum class CameraType : std::uint8_t
{
	CameraTypeOrtho,
	CameraTypePerspective,
	CameraTypeCube,
	CameraTypeBeginRange = CameraTypeOrtho,
	CameraTypeEndRange = CameraTypeCube,
	CameraTypeRangeSize = (CameraTypeEndRange - CameraTypeBeginRange + 1),
};

enum class CameraOrder : std::uint8_t
{
	CameraOrderCustom,
	CameraOrderShadow,
	CameraOrderLightProbe,
	CameraOrder3D,
	CameraOrder2D,
	CameraOrderBeginRange = CameraOrderCustom,
	CameraOrderEndRange = CameraOrder2D,
	CameraOrderRangeSize = (CameraOrderEndRange - CameraOrderBeginRange + 1),
};

enum CameraClearFlagBits
{
	CameraClearColorBit = 1 << 0,
	CameraClearDepthBit = 1 << 1,
	CameraClearStencilBit = 1 << 2,
	CameraClearDepthStencilBit = CameraClearDepthBit | CameraClearStencilBit,
	CameraClearMaxBit = 0x7FFFFFFF
};

typedef std::uint32_t CameraClearFlags;

enum CameraRenderFlagBits
{
	CameraRenderScreenBit = 0x00000001,
	CameraRenderTextureBit = 0x00000002,
	CameraRenderBeginBit = CameraRenderScreenBit,
	CameraRenderEndBit = CameraRenderTextureBit,
	CameraRenderMaxBit = 0x7FFFFFFF
};

typedef std::uint32_t CameraRenderFlags;

enum class LightType : std::uint8_t
{
	LightTypeSun,
	LightTypeDirectional,
	LightTypeAmbient,
	LightTypePoint,
	LightTypeSpot,
	LightTypeRectangle,
	LightTypeDisk,
	LightTypeTube,
	LightTypeEnvironment,
	LightTypeBeginRange = LightTypeSun,
	LightTypeEndRange = LightTypeEnvironment,
	LightTypeRangeSize = (LightTypeEndRange - LightTypeBeginRange + 1),
};

enum class ShadowMode : std::uint8_t
{
	ShadowModeNone,
	ShadowModeHard,
	ShadowModeSoft,
	ShadowModeBeginRange = ShadowModeNone,
	ShadowModeEndRange = ShadowModeSoft,
	ShadowModeRangeSize = (ShadowModeEndRange - ShadowModeBeginRange + 1),
};

enum class ShadowQuality : std::uint8_t
{
	ShadowQualityNone,
	ShadowQualityLow,
	ShadowQualityMedium,
	ShadowQualityHigh,
	ShadowQualityVeryHigh,
	ShadowQualityBeginRange = ShadowQualityNone,
	ShadowQualityEndRange = ShadowQualityVeryHigh,
	ShadowQualityRangeSize = (ShadowQualityEndRange - ShadowQualityBeginRange + 1),
};

enum LightShadowSize
{
	LightShadowSizeLow = 256,
	LightShadowSizeMedium = 512,
	LightShadowSizeHigh = 1024,
	LightShadowSizeVeryHigh = 2048,
	LightShadowSizeEnumCount = 4
};

enum class RenderPipelineType : std::uint8_t
{
	RenderPipelineTypeForward,
	RenderPipelineTypeForwardPlus,
	RenderPipelineTypeDeferredLighting,
	RenderPipelineTypeBeginRange = RenderPipelineTypeForward,
	RenderPipelineTypeEndRange = RenderPipelineTypeDeferredLighting,
	RenderPipelineTypeRangeSize = (RenderPipelineTypeEndRange - RenderPipelineTypeBeginRange + 1)
};

enum RenderQueue
{
	RenderQueueCustom,
	RenderQueueShadow,
	RenderQueueReflectiveShadow,
	RenderQueueOpaque,
	RenderQueueOpaqueBatch,
	RenderQueueOpaqueSpecific,
	RenderQueueOpaqueSpecificBatch,
	RenderQueueOpaqueShading,
	RenderQueueOpaqueLighting,
	RenderQueueTransparentBack,
	RenderQueueTransparentBatchBack,
	RenderQueueTransparentSpecificBack,
	RenderQueueTransparentSpecificBatchBack,
	RenderQueueTransparentShadingBack,
	RenderQueueTransparentFront,
	RenderQueueTransparentBatchFront,
	RenderQueueTransparentSpecificFront,
	RenderQueueTransparentSpecificBatchFront,
	RenderQueueTransparentShadingFront,
	RenderQueueFogs,
	RenderQueueLights,
	RenderQueueLightProbes,
	RenderQueuePostprocess,
	RenderQueueBeginRange = RenderQueueCustom,
	RenderQueueEndRange = RenderQueuePostprocess,
	RenderQueueRangeSize = (RenderQueueEndRange - RenderQueueBeginRange + 1),
	RenderQueueMaxEnum = 0x7FFFFFFF
};

enum GlobalSemanticType
{
	GlobalSemanticTypeNone,
	GlobalSemanticTypeModel,
	GlobalSemanticTypeModelInverse,
	GlobalSemanticTypeView,
	GlobalSemanticTypeViewInverse,
	GlobalSemanticTypeProject,
	GlobalSemanticTypeProjectInverse,
	GlobalSemanticTypeViewProject,
	GlobalSemanticTypeViewProjectInverse,
	GlobalSemanticTypeModelView,
	GlobalSemanticTypeModelViewProject,
	GlobalSemanticTypeModelViewInverse,
	GlobalSemanticTypeCameraAperture,
	GlobalSemanticTypeCameraNear,
	GlobalSemanticTypeCameraFar,
	GlobalSemanticTypeCameraPosition,
	GlobalSemanticTypeCameraDirection,
	GlobalSemanticTypeDepthMap,
	GlobalSemanticTypeDepthLinearMap,
	GlobalSemanticTypeLightingMap,
	GlobalSemanticTypeOpaqueShadingMap,
	GlobalSemanticTypeBeginRange = GlobalSemanticTypeNone,
	GlobalSemanticTypeEndRange = GlobalSemanticTypeOpaqueShadingMap,
	GlobalSemanticTypeRangeSize = (GlobalSemanticTypeEndRange - GlobalSemanticTypeBeginRange + 1),
	GlobalSemanticTypeMaxEnum = 0x7FFFFFFF
};

_NAME_END

#endif