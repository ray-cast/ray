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
#ifndef _H_GRAPHICS_TYPES_H_
#define _H_GRAPHICS_TYPES_H_

#include <ray/memory.h>
#include <ray/math.h>
#include <ray/except.h>
#include <ray/variant.h>
#include <ray/model.h>

_NAME_BEGIN

typedef std::shared_ptr<class GraphicsDevice> GraphicsDevicePtr;
typedef std::shared_ptr<class GraphicsSwapchain> GraphicsSwapchainPtr;
typedef std::shared_ptr<class GraphicsResource> GraphicsResourcePtr;
typedef std::shared_ptr<class GraphicsContext> GraphicsContextPtr;
typedef std::shared_ptr<class GraphicsData> GraphicsDataPtr;
typedef std::shared_ptr<class GraphicsInputLayout> GraphicsInputLayoutPtr;
typedef std::shared_ptr<class GraphicsState> GraphicsStatePtr;
typedef std::shared_ptr<class GraphicsTexture> GraphicsTexturePtr;
typedef std::shared_ptr<class GraphicsSampler> GraphicsSamplerPtr;
typedef std::shared_ptr<class GraphicsFramebuffer> GraphicsFramebufferPtr;
typedef std::shared_ptr<class GraphicsFramebufferLayout> GraphicsFramebufferLayoutPtr;
typedef std::shared_ptr<class GraphicsParam> GraphicsParamPtr;
typedef std::shared_ptr<class GraphicsAttribute> GraphicsAttributePtr;
typedef std::shared_ptr<class GraphicsUniform> GraphicsUniformPtr;
typedef std::shared_ptr<class GraphicsUniformBlock> GraphicsUniformBlockPtr;
typedef std::shared_ptr<class GraphicsShader> GraphicsShaderPtr;
typedef std::shared_ptr<class GraphicsProgram> GraphicsProgramPtr;
typedef std::shared_ptr<class GraphicsPipeline> GraphicsPipelinePtr;
typedef std::shared_ptr<class GraphicsDescriptorPool> GraphicsDescriptorPoolPtr;
typedef std::shared_ptr<class GraphicsDescriptorSet> GraphicsDescriptorSetPtr;
typedef std::shared_ptr<class GraphicsDescriptorSetLayout> GraphicsDescriptorSetLayoutPtr;
typedef std::shared_ptr<class GraphicsUniformSet> GraphicsUniformSetPtr;
typedef std::shared_ptr<class GraphicsVariant> GraphicsVariantPtr;
typedef std::shared_ptr<class GraphicsSystem> GraphicsSystemPtr;
typedef std::shared_ptr<class GraphicsCommandPool> GraphicsCommandPoolPtr;
typedef std::shared_ptr<class GraphicsCommandQueue> GraphicsCommandQueuePtr;
typedef std::shared_ptr<class GraphicsCommandList> GraphicsCommandListPtr;
typedef std::shared_ptr<class GraphicsSemaphore> GraphicsSemaphorePtr;
typedef std::shared_ptr<class GraphicsIndirect> GraphicsIndirectPtr;

typedef std::weak_ptr<class GraphicsDevice> GraphicsDeviceWeakPtr;
typedef std::weak_ptr<class GraphicsSwapchain> GraphicsSwapchainWeakPtr;
typedef std::weak_ptr<class GraphicsResource> GraphicsResourceWeakPtr;
typedef std::weak_ptr<class GraphicsContext> GraphicsContextWeakPtr;
typedef std::weak_ptr<class GraphicsData> GraphicsDataWeakPtr;
typedef std::weak_ptr<class GraphicsInputLayout> GraphicsInputLayoutWeakPtr;
typedef std::weak_ptr<class GraphicsState> GraphicsStateWeakPtr;
typedef std::weak_ptr<class GraphicsTexture> GraphicsTextureWeakPtr;
typedef std::weak_ptr<class GraphicsSampler> GraphicsSamplerWeakPtr;
typedef std::weak_ptr<class GraphicsFramebuffer> GraphicsFramebufferWeakPtr;
typedef std::weak_ptr<class GraphicsShader> GraphicsShaderWeakPtr;
typedef std::weak_ptr<class GraphicsProgram> GraphicsProgramWeakPtr;
typedef std::weak_ptr<class GraphicsPipeline> GraphicsPipelineWeakPtr;
typedef std::weak_ptr<class GraphicsDescriptorPool> GraphicsDescriptorPoolWeakPtr;
typedef std::weak_ptr<class GraphicsDescriptorSet> GraphicsDescriptorSetWeakPtr;
typedef std::weak_ptr<class GraphicsDescriptorSetLayout> GraphicsDescriptorSetLayoutWeakPtr;
typedef std::weak_ptr<class GraphicsUniformSet> GraphicsUniformWeakPtr;
typedef std::weak_ptr<class GraphicsVariant> GraphicsVariantWeakPtr;
typedef std::weak_ptr<class GraphicsSystem> GraphicsSystemWeakPtr;
typedef std::weak_ptr<class GraphicsCommandPool> GraphicsCommandPoolWeakPtr;
typedef std::weak_ptr<class GraphicsCommandQueue> GraphicsCommandQueueWeakPtr;
typedef std::weak_ptr<class GraphicsCommandList> GraphicsCommandPoolListWeakPtr;

typedef std::vector<GraphicsShaderPtr> GraphicsShaders;
typedef std::vector<GraphicsVariantPtr> GraphicsVariants;
typedef std::vector<GraphicsFramebufferPtr> GraphicsFramebuffers;
typedef std::vector<GraphicsIndirectPtr> GraphicsIndirects;
typedef std::vector<GraphicsUniformSetPtr> GraphicsUniformSets;
typedef std::vector<GraphicsTexturePtr> GraphicsTextures;
typedef std::vector<class GraphicsVertexLayout> GraphicsVertexLayouts;
typedef std::vector<class GraphicsDescriptorPoolComponent> GraphicsDescriptorPoolComponents;
typedef std::vector<class GraphicsAttachmentDesc> GraphicsAttachmentDescs;
typedef std::vector<GraphicsAttributePtr> GraphicsAttributes;
typedef std::vector<GraphicsUniformPtr> GraphicsUniforms;
typedef std::vector<GraphicsUniformBlockPtr> GraphicsUniformBlocks;

class GraphicsDeviceDesc;
class GraphicsSwapchainDesc;
class GraphicsContextDesc;
class GraphicsInputLayoutDesc;
class GraphicsDataDesc;
class GraphicsTextureDesc;
class GraphicsSamplerDesc;
class GraphicsFramebufferDesc;
class GraphicsStateDesc;
class GraphicsShaderDesc;
class GraphicsProgramDesc;
class GraphicsPipelineDesc;
class GraphicsDescriptorSetDesc;
class GraphicsDescriptorSetLayoutDesc;
class GraphicsDescriptorPoolDesc;
class GraphicsSemaphoreDesc;

typedef void* WindHandle;

enum GraphicsDeviceType
{
	GraphicsDeviceTypeD3D9,
	GraphicsDeviceTypeD3D11,
	GraphicsDeviceTypeD3D12,
	GraphicsDeviceTypeOpenGL,
	GraphicsDeviceTypeOpenGLCore,
	GraphicsDeviceTypeOpenGLES2,
	GraphicsDeviceTypeOpenGLES3,
	GraphicsDeviceTypeOpenGLES31,
	GraphicsDeviceTypeVulkan,
	GraphicsDeviceTypeBeginRange = GraphicsDeviceTypeD3D9,
	GraphicsDeviceTypeEndRange = GraphicsDeviceTypeVulkan,
	GraphicsDeviceTypeRangeSize = (GraphicsDeviceTypeEndRange - GraphicsDeviceTypeBeginRange + 1),
	GraphicsDeviceTypeMaxEnum = 0x7FFFFFFF
};

enum GraphicsSwapInterval
{
	GraphicsSwapIntervalFree,
	GraphicsSwapIntervalVsync,
	GraphicsSwapIntervalFps30,
	GraphicsSwapIntervalFps15,
};

enum GraphicsCompareFunc
{
	GraphicsCompareFuncNone,
	GraphicsCompareFuncLequal,
	GraphicsCompareFuncEqual,
	GraphicsCompareFuncGreater,
	GraphicsCompareFuncLess,
	GraphicsCompareFuncGequal,
	GraphicsCompareFuncNotEqual,
	GraphicsCompareFuncAlways,
	GraphicsCompareFuncNever,
	GraphicsCompareFuncBeginRange = GraphicsCompareFuncNone,
	GraphicsCompareFuncEndRange = GraphicsCompareFuncNever,
	GraphicsCompareFuncRangeSize = (GraphicsCompareFuncEndRange - GraphicsCompareFuncBeginRange + 1),
	GraphicsCompareFuncMaxEnum = 0x7FFFFFFF
};

enum GraphicsBlendFactor
{
	GraphicsBlendFactorZero,
	GraphicsBlendFactorOne,
	GraphicsBlendFactorDstCol,
	GraphicsBlendFactorSrcColor,
	GraphicsBlendFactorSrcAlpha,
	GraphicsBlendFactorDstAlpha,
	GraphicsBlendFactorOneMinusSrcCol,
	GraphicsBlendFactorOneMinusDstCol,
	GraphicsBlendFactorOneMinusSrcAlpha,
	GraphicsBlendFactorOneMinusDstAlpha,
	GraphicsBlendFactorConstantColor,
	GraphicsBlendFactorConstantAlpha,
	GraphicsBlendFactorOneMinusConstantColor,
	GraphicsBlendFactorOneMinusConstantAlpha,
	GraphicsBlendFactorSrcAlphaSaturate,
	GraphicsBlendFactorBeginRange = GraphicsBlendFactorZero,
	GraphicsBlendFactorEndRange = GraphicsBlendFactorSrcAlphaSaturate,
	GraphicsBlendFactorRangeSize = (GraphicsBlendFactorEndRange - GraphicsBlendFactorBeginRange + 1),
	GraphicsBlendFactorMaxEnum = 0x7FFFFFFF
};

enum GraphicsBlendOp
{
	GraphicsBlendOpAdd,
	GraphicsBlendOpSubtract,
	GraphicsBlendOpRevSubtract,
	GraphicsBlendOpBeginRange = GraphicsBlendOpAdd,
	GraphicsBlendOpEndRange = GraphicsBlendOpRevSubtract,
	GraphicsBlendOpRangeSize = (GraphicsBlendOpEndRange - GraphicsBlendOpBeginRange + 1),
	GraphicsBlendOpMaxEnum = 0x7FFFFFFF
};

enum GraphicsColorMask
{
	GraphicsColorMaskR = 1UL << 0,
	GraphicsColorMaskG = 1UL << 1,
	GraphicsColorMaskB = 1UL << 2,
	GraphicsColorMaskA = 1UL << 3,
	GraphicsColorMaskRGB = GraphicsColorMaskR | GraphicsColorMaskG | GraphicsColorMaskB,
	GraphicsColorMaskRGBA = GraphicsColorMaskR | GraphicsColorMaskG | GraphicsColorMaskB | GraphicsColorMaskA
};

enum GraphicsCullMode
{
	GraphicsCullModeNone,
	GraphicsCullModeFront,
	GraphicsCullModeBack,
	GraphicsCullModeFrontBack,
	GraphicsCullModeBeginRange = GraphicsCullModeNone,
	GraphicsCullModeEndRange = GraphicsCullModeFrontBack,
	GraphicsCullModeRangeSize = (GraphicsCullModeEndRange - GraphicsCullModeBeginRange + 1),
	GraphicsCullModeMaxEnum = 0x7FFFFFFF
};

enum GraphicsFrontFace
{
	GraphicsFrontFaceCW = 0,
	GraphicsFrontFaceCCW = 1,
	GraphicsFrontFaceBeginRange = GraphicsFrontFaceCW,
	GraphicsFrontFaceEndRange = GraphicsFrontFaceCCW,
	GraphicsFrontFaceRangeSize = (GraphicsFrontFaceEndRange - GraphicsFrontFaceBeginRange + 1),
	GraphicsFrontFaceMaxEnum = 0x7FFFFFFF
};

enum GraphicsPolygonMode
{
	GraphicsPolygonModePoint,
	GraphicsPolygonModeWireframe,
	GraphicsPolygonModeSolid,
	GraphicsPolygonModeBeginRange = GraphicsPolygonModePoint,
	GraphicsPolygonModeEndRange = GraphicsPolygonModeSolid,
	GraphicsPolygonModeRangeSize = (GraphicsPolygonModeEndRange - GraphicsPolygonModeBeginRange + 1),
	GraphicsPolygonModeMaxEnum = 0x7FFFFFFF
};

enum GraphicsStencilOp
{
	GraphicsStencilOpKeep,
	GraphicsStencilOpReplace,
	GraphicsStencilOpIncr,
	GraphicsStencilOpDecr,
	GraphicsStencilOpZero,
	GraphicsStencilOpIncrWrap,
	GraphicsStencilOpDecrWrap,
	GraphicsStencilOpBeginRange = GraphicsStencilOpKeep,
	GraphicsStencilOpEndRange = GraphicsStencilOpDecrWrap,
	GraphicsStencilOpRangeSize = (GraphicsStencilOpEndRange - GraphicsStencilOpBeginRange + 1),
	GraphicsStencilOpMaxEnum = 0x7FFFFFFF
};

enum GraphicsClearFlags
{
	GraphicsClearFlagsColor = 1UL << 0,
	GraphicsClearFlagsDepth = 1UL << 1,
	GraphicsClearFlagsStencil = 1UL << 2,
	GraphicsClearFlagsColorDepth = GraphicsClearFlagsColor | GraphicsClearFlagsDepth,
	GraphicsClearFlagsColorStencil = GraphicsClearFlagsColor | GraphicsClearFlagsStencil,
	GraphicsClearFlagsDepthStencil = GraphicsClearFlagsDepth | GraphicsClearFlagsStencil,
	GraphicsClearFlagsAll = GraphicsClearFlagsColor | GraphicsClearFlagsDepth | GraphicsClearFlagsStencil
};

enum GraphicsFormatType
{
	GraphicsFormatTypeNone,
	GraphicsFormatTypeRInt,
	GraphicsFormatTypeRUInt,
	GraphicsFormatTypeRSRGB,
	GraphicsFormatTypeRSNorm,
	GraphicsFormatTypeRUNorm,
	GraphicsFormatTypeRSScaled,
	GraphicsFormatTypeRUScaled,
	GraphicsFormatTypeRFloat,
	GraphicsFormatTypeRGInt,
	GraphicsFormatTypeRGUInt,
	GraphicsFormatTypeRGSRGB,
	GraphicsFormatTypeRGSNorm,
	GraphicsFormatTypeRGUNorm,
	GraphicsFormatTypeRGSScaled,
	GraphicsFormatTypeRGUScaled,
	GraphicsFormatTypeRGFloat,
	GraphicsFormatTypeRGBInt,
	GraphicsFormatTypeRGBUInt,
	GraphicsFormatTypeRGBSRGB,
	GraphicsFormatTypeRGBSNorm,
	GraphicsFormatTypeRGBUNorm,
	GraphicsFormatTypeRGBSScaled,
	GraphicsFormatTypeRGBUScaled,
	GraphicsFormatTypeRGBFloat,
	GraphicsFormatTypeRGBAInt,
	GraphicsFormatTypeRGBAUInt,
	GraphicsFormatTypeRGBASRGB,
	GraphicsFormatTypeRGBASNorm,
	GraphicsFormatTypeRGBAUNorm,
	GraphicsFormatTypeRGBASScaled,
	GraphicsFormatTypeRGBAUScaled,
	GraphicsFormatTypeRGBAFloat,
	GraphicsFormatTypeBGRInt,
	GraphicsFormatTypeBGRUInt,
	GraphicsFormatTypeBGRSRGB,
	GraphicsFormatTypeBGRSNorm,
	GraphicsFormatTypeBGRUNorm,
	GraphicsFormatTypeBGRSScaled,
	GraphicsFormatTypeBGRUScaled,
	GraphicsFormatTypeBGRFloat,
	GraphicsFormatTypeBGRAInt,
	GraphicsFormatTypeBGRAUInt,
	GraphicsFormatTypeBGRASRGB,
	GraphicsFormatTypeBGRASNorm,
	GraphicsFormatTypeBGRAUNorm,
	GraphicsFormatTypeBGRASScaled,
	GraphicsFormatTypeBGRAUScaled,
	GraphicsFormatTypeBGRAFloat,
	GraphicsFormatTypeABGRInt,
	GraphicsFormatTypeABGRUInt,
	GraphicsFormatTypeABGRSRGB,
	GraphicsFormatTypeABGRSNorm,
	GraphicsFormatTypeABGRUNorm,
	GraphicsFormatTypeABGRSScaled,
	GraphicsFormatTypeABGRUScaled,
	GraphicsFormatTypeABGRFloat,
	GraphicsFormatTypeDepth,
	GraphicsFormatTypeDepthStencil,
	GraphicsFormatTypeBeginRange = GraphicsFormatTypeNone,
	GraphicsFormatTypeEndRange = GraphicsFormatTypeDepthStencil,
	GraphicsFormatTypeRangeSize = (GraphicsFormatTypeEndRange - GraphicsFormatTypeBeginRange + 1),
	GraphicsFormatTypeMaxEnum = 0x7FFFFFFF
};

enum GraphicsFormat
{
	GraphicsFormatUndefined = 0,
	GraphicsFormatR4G4UNormPack8 = 1,
	GraphicsFormatR4G4B4A4UNormPack16 = 2,
	GraphicsFormatB4G4R4A4UNormPack16 = 3,
	GraphicsFormatR5G6B5UNormPack16 = 4,
	GraphicsFormatB5G6R5UNormPack16 = 5,
	GraphicsFormatR5G5B5A1UNormPack16 = 6,
	GraphicsFormatB5G5R5A1UNormPack16 = 7,
	GraphicsFormatA1R5G5B5UNormPack16 = 8,
	GraphicsFormatR8UNorm = 9,
	GraphicsFormatR8SNorm = 10,
	GraphicsFormatR8UScaled = 11,
	GraphicsFormatR8SScaled = 12,
	GraphicsFormatR8UInt = 13,
	GraphicsFormatR8SInt = 14,
	GraphicsFormatR8SRGB = 15,
	GraphicsFormatR8G8UNorm = 16,
	GraphicsFormatR8G8SNorm = 17,
	GraphicsFormatR8G8UScaled = 18,
	GraphicsFormatR8G8SScaled = 19,
	GraphicsFormatR8G8UInt = 20,
	GraphicsFormatR8G8SInt = 21,
	GraphicsFormatR8G8SRGB = 22,
	GraphicsFormatR8G8B8UNorm = 23,
	GraphicsFormatR8G8B8SNorm = 24,
	GraphicsFormatR8G8B8UScaled = 25,
	GraphicsFormatR8G8B8SScaled = 26,
	GraphicsFormatR8G8B8UInt = 27,
	GraphicsFormatR8G8B8SInt = 28,
	GraphicsFormatR8G8B8SRGB = 29,
	GraphicsFormatB8G8R8UNorm = 30,
	GraphicsFormatB8G8R8SNorm = 31,
	GraphicsFormatB8G8R8UScaled = 32,
	GraphicsFormatB8G8R8SScaled = 33,
	GraphicsFormatB8G8R8UInt = 34,
	GraphicsFormatB8G8R8SInt = 35,
	GraphicsFormatB8G8R8SRGB = 36,
	GraphicsFormatR8G8B8A8UNorm = 37,
	GraphicsFormatR8G8B8A8SNorm = 38,
	GraphicsFormatR8G8B8A8UScaled = 39,
	GraphicsFormatR8G8B8A8SScaled = 40,
	GraphicsFormatR8G8B8A8UInt = 41,
	GraphicsFormatR8G8B8A8SInt = 42,
	GraphicsFormatR8G8B8A8SRGB = 43,
	GraphicsFormatB8G8R8A8UNorm = 44,
	GraphicsFormatB8G8R8A8SNorm = 45,
	GraphicsFormatB8G8R8A8UScaled = 46,
	GraphicsFormatB8G8R8A8SScaled = 47,
	GraphicsFormatB8G8R8A8UInt = 48,
	GraphicsFormatB8G8R8A8SInt = 49,
	GraphicsFormatB8G8R8A8SRGB = 50,
	GraphicsFormatA8B8G8R8UNormPack32 = 51,
	GraphicsFormatA8B8G8R8SNormPack32 = 52,
	GraphicsFormatA8B8G8R8UScaledPack32 = 53,
	GraphicsFormatA8B8G8R8SScaledPack32 = 54,
	GraphicsFormatA8B8G8R8UIntPack32 = 55,
	GraphicsFormatA8B8G8R8SIntPack32 = 56,
	GraphicsFormatA8B8G8R8SRGBPack32 = 57,
	GraphicsFormatA2R10G10B10UNormPack32 = 58,
	GraphicsFormatA2R10G10B10SNormPack32 = 59,
	GraphicsFormatA2R10G10B10UScaledPack32 = 60,
	GraphicsFormatA2R10G10B10SScaledPack32 = 61,
	GraphicsFormatA2R10G10B10UIntPack32 = 62,
	GraphicsFormatA2R10G10B10SIntPack32 = 63,
	GraphicsFormatA2B10G10R10UNormPack32 = 64,
	GraphicsFormatA2B10G10R10SNormPack32 = 65,
	GraphicsFormatA2B10G10R10UScaledPack32 = 66,
	GraphicsFormatA2B10G10R10SScaledPack32 = 67,
	GraphicsFormatA2B10G10R10UIntPack32 = 68,
	GraphicsFormatA2B10G10R10SIntPack32 = 69,
	GraphicsFormatR16UNorm = 70,
	GraphicsFormatR16SNorm = 71,
	GraphicsFormatR16UScaled = 72,
	GraphicsFormatR16SScaled = 73,
	GraphicsFormatR16UInt = 74,
	GraphicsFormatR16SInt = 75,
	GraphicsFormatR16SFloat = 76,
	GraphicsFormatR16G16UNorm = 77,
	GraphicsFormatR16G16SNorm = 78,
	GraphicsFormatR16G16UScaled = 79,
	GraphicsFormatR16G16SScaled = 80,
	GraphicsFormatR16G16UInt = 81,
	GraphicsFormatR16G16SInt = 82,
	GraphicsFormatR16G16SFloat = 83,
	GraphicsFormatR16G16B16UNorm = 84,
	GraphicsFormatR16G16B16SNorm = 85,
	GraphicsFormatR16G16B16UScaled = 86,
	GraphicsFormatR16G16B16SScaled = 87,
	GraphicsFormatR16G16B16UInt = 88,
	GraphicsFormatR16G16B16SInt = 89,
	GraphicsFormatR16G16B16SFloat = 90,
	GraphicsFormatR16G16B16A16UNorm = 91,
	GraphicsFormatR16G16B16A16SNorm = 92,
	GraphicsFormatR16G16B16A16UScaled = 93,
	GraphicsFormatR16G16B16A16SScaled = 94,
	GraphicsFormatR16G16B16A16UInt = 95,
	GraphicsFormatR16G16B16A16SInt = 96,
	GraphicsFormatR16G16B16A16SFloat = 97,
	GraphicsFormatR32UInt = 98,
	GraphicsFormatR32SInt = 99,
	GraphicsFormatR32SFloat = 100,
	GraphicsFormatR32G32UInt = 101,
	GraphicsFormatR32G32SInt = 102,
	GraphicsFormatR32G32SFloat = 103,
	GraphicsFormatR32G32B32UInt = 104,
	GraphicsFormatR32G32B32SInt = 105,
	GraphicsFormatR32G32B32SFloat = 106,
	GraphicsFormatR32G32B32A32UInt = 107,
	GraphicsFormatR32G32B32A32SInt = 108,
	GraphicsFormatR32G32B32A32SFloat = 109,
	GraphicsFormatR64UInt = 110,
	GraphicsFormatR64SInt = 111,
	GraphicsFormatR64SFloat = 112,
	GraphicsFormatR64G64UInt = 113,
	GraphicsFormatR64G64SInt = 114,
	GraphicsFormatR64G64SFloat = 115,
	GraphicsFormatR64G64B64UInt = 116,
	GraphicsFormatR64G64B64SInt = 117,
	GraphicsFormatR64G64B64SFloat = 118,
	GraphicsFormatR64G64B64A64UInt = 119,
	GraphicsFormatR64G64B64A64SInt = 120,
	GraphicsFormatR64G64B64A64SFloat = 121,
	GraphicsFormatB10G11R11UFloatPack32 = 122,
	GraphicsFormatE5B9G9R9UFloatPack32 = 123,
	GraphicsFormatD16UNorm = 124,
	GraphicsFormatX8_D24UNormPack32 = 125,
	GraphicsFormatD32_SFLOAT = 126,
	GraphicsFormatS8UInt = 127,
	GraphicsFormatD16UNorm_S8UInt = 128,
	GraphicsFormatD24UNorm_S8UInt = 129,
	GraphicsFormatD32_SFLOAT_S8UInt = 130,
	GraphicsFormatBC1RGBUNormBlock = 131,
	GraphicsFormatBC1RGBSRGBBlock = 132,
	GraphicsFormatBC1RGBAUNormBlock = 133,
	GraphicsFormatBC1RGBASRGBBlock = 134,
	GraphicsFormatBC2UNormBlock = 135,
	GraphicsFormatBC2SRGBBlock = 136,
	GraphicsFormatBC3UNormBlock = 137,
	GraphicsFormatBC3SRGBBlock = 138,
	GraphicsFormatBC4UNormBlock = 139,
	GraphicsFormatBC4SNormBlock = 140,
	GraphicsFormatBC5UNormBlock = 141,
	GraphicsFormatBC5SNormBlock = 142,
	GraphicsFormatBC6HUFloatBlock = 143,
	GraphicsFormatBC6HSFloatBlock = 144,
	GraphicsFormatBC7UNormBlock = 145,
	GraphicsFormatBC7SRGBBlock = 146,
	GraphicsFormatETC2R8G8B8UNormBlock = 147,
	GraphicsFormatETC2R8G8B8SRGBBlock = 148,
	GraphicsFormatETC2R8G8B8A1UNormBlock = 149,
	GraphicsFormatETC2R8G8B8A1SRGBBlock = 150,
	GraphicsFormatETC2R8G8B8A8UNormBlock = 151,
	GraphicsFormatETC2R8G8B8A8SRGBBlock = 152,
	GraphicsFormatEACR11UNormBlock = 153,
	GraphicsFormatEACR11SNormBlock = 154,
	GraphicsFormatEACR11G11UNormBlock = 155,
	GraphicsFormatEACR11G11SNormBlock = 156,
	GraphicsFormatASTC4x4UNormBlock = 157,
	GraphicsFormatASTC4x4SRGBBlock = 158,
	GraphicsFormatASTC5x4UNormBlock = 159,
	GraphicsFormatASTC5x4SRGBBlock = 160,
	GraphicsFormatASTC5x5UNormBlock = 161,
	GraphicsFormatASTC5x5SRGBBlock = 162,
	GraphicsFormatASTC6x5UNormBlock = 163,
	GraphicsFormatASTC6x5SRGBBlock = 164,
	GraphicsFormatASTC6x6UNormBlock = 165,
	GraphicsFormatASTC6x6SRGBBlock = 166,
	GraphicsFormatASTC8x5UNormBlock = 167,
	GraphicsFormatASTC8x5SRGBBlock = 168,
	GraphicsFormatASTC8x6UNormBlock = 169,
	GraphicsFormatASTC8x6SRGBBlock = 170,
	GraphicsFormatASTC8x8UNormBlock = 171,
	GraphicsFormatASTC8x8SRGBBlock = 172,
	GraphicsFormatASTC10x5UNormBlock = 173,
	GraphicsFormatASTC10x5SRGBBlock = 174,
	GraphicsFormatASTC10x6UNormBlock = 175,
	GraphicsFormatASTC10x6SRGBBlock = 176,
	GraphicsFormatASTC10x8UNormBlock = 177,
	GraphicsFormatASTC10x8SRGBBlock = 178,
	GraphicsFormatASTC10x10UNormBlock = 179,
	GraphicsFormatASTC10x10SRGBBlock = 180,
	GraphicsFormatASTC12x10UNormBlock = 181,
	GraphicsFormatASTC12x10SRGBBlock = 182,
	GraphicsFormatASTC12x12UNormBlock = 183,
	GraphicsFormatASTC12x12SRGBBlock = 184,
	GraphicsFormatBeginRange = GraphicsFormatUndefined,
	GraphicsFormatEndRange = GraphicsFormatASTC12x12SRGBBlock,
	GraphicsFormatRangeSize = (GraphicsFormatEndRange - GraphicsFormatBeginRange + 1),
	GraphicsFormatMaxEnum = 0x7FFFFFFF
};

enum GraphicsTextureDim
{
	GraphicsTextureDim2D,
	GraphicsTextureDim3D,
	GraphicsTextureDimCube,
	GraphicsTextureDim2DArray,
	GraphicsTextureDimCubeArray,
	GraphicsTextureDimBeginRange = GraphicsTextureDim2D,
	GraphicsTextureDimEndRange = GraphicsTextureDimCubeArray,
	GraphicsTextureDimRangeSize = (GraphicsTextureDimEndRange - GraphicsTextureDimBeginRange + 1),
	GraphicsTextureDimMaxEnum = 0x7FFFFFFF
};

enum GraphicsSamplerAnis
{
	GraphicsSamplerAnis1,
	GraphicsSamplerAnis2,
	GraphicsSamplerAnis4,
	GraphicsSamplerAnis8,
	GraphicsSamplerAnis16,
	GraphicsSamplerAnis32,
	GraphicsSamplerAnis64,
	GraphicsSamplerAnisBeginRange = GraphicsSamplerAnis1,
	GraphicsSamplerAnisEndRange = GraphicsSamplerAnis16,
	GraphicsSamplerAnisRangeSize = (GraphicsSamplerAnisEndRange - GraphicsSamplerAnisBeginRange + 1),
	GraphicsSamplerAnisMaxEnum = 0x7FFFFFFF
};

enum GraphicsSamplerOp
{
	GraphicsSamplerOpMultiply,    //* T = T1 * T2
	GraphicsSamplerOpAdd,         //* T = T1 + T2
	GraphicsSamplerOpSubtract,    //* T = T1 - T2
	GraphicsSamplerOpDivide,      //* T = T1 / T2
	GraphicsSamplerOpSmoothAdd,   //* T = (T1 + T2) - (T1 * T2)
	GraphicsSamplerOpSignedAdd,   //* T = T1 + (T2-0.5)
	GraphicsSamplerOpBeginRange = GraphicsSamplerOpMultiply,
	GraphicsSamplerOpEndRange = GraphicsSamplerOpSignedAdd,
	GraphicsSamplerOpRangeSize = (GraphicsSamplerOpEndRange - GraphicsSamplerOpBeginRange + 1),
	GraphicsSamplerOpMaxEnum = 0x7FFFFFFF
};

enum GraphicsSamplerWrap
{
	GraphicsSamplerWrapNone,
	GraphicsSamplerWrapRepeat,
	GraphicsSamplerWrapMirror,
	GraphicsSamplerWrapClampToEdge,
	GraphicsSamplerWrapBeginRange = GraphicsSamplerWrapNone,
	GraphicsSamplerWrapEndRange = GraphicsSamplerWrapClampToEdge,
	GraphicsSamplerWrapRangeSize = (GraphicsSamplerWrapEndRange - GraphicsSamplerWrapBeginRange + 1),
	GraphicsSamplerWrapMaxEnum = 0x7FFFFFFF
};

enum GraphicsSamplerFilter
{
	GraphicsSamplerFilterNearest,
	GraphicsSamplerFilterLinear,
	GraphicsSamplerFilterNearestMipmapLinear,
	GraphicsSamplerFilterNearestMipmapNearest,
	GraphicsSamplerFilterLinearMipmapNearest,
	GraphicsSamplerFilterLinearMipmapLinear,
	GraphicsSamplerFilterBeginRange = GraphicsSamplerFilterNearest,
	GraphicsSamplerFilterEndRange = GraphicsSamplerFilterLinearMipmapLinear,
	GraphicsSamplerFilterRangeSize = (GraphicsSamplerFilterEndRange - GraphicsSamplerFilterBeginRange + 1),
	GraphicsSamplerFilterMaxEnum = 0x7FFFFFFF
};

enum GraphicsSampleFlagBits
{
	GraphicsSampleFlagBits1Bit = 0x00000001,
	GraphicsSampleFlagBits2Bit = 0x00000002,
	GraphicsSampleFlagBits4Bit = 0x00000004,
	GraphicsSampleFlagBits8Bit = 0x00000008,
	GraphicsSampleFlagBits16Bit = 0x00000010,
	GraphicsSampleFlagBits32Bit = 0x00000020,
	GraphicsSampleFlagBits64Bit = 0x00000040,
};

enum GraphicsDataType
{
	GraphicsDataTypeNone,
	GraphicsDataTypeTransferSrc,
	GraphicsDataTypeTransferDst,
	GraphicsDataTypeUniformTexelBuffer,
	GraphicsDataTypeUniformBuffer,
	GraphicsDataTypeStorageTexelBuffer,
	GraphicsDataTypeStorageBuffer,
	GraphicsDataTypeStorageVertexBuffer,
	GraphicsDataTypeStorageIndexBuffer,
	GraphicsDataTypeBeginRange = GraphicsDataTypeNone,
	GraphicsDataTypeEndRange = GraphicsDataTypeStorageIndexBuffer,
	GraphicsDataTypeRangeSize = (GraphicsDataTypeEndRange - GraphicsDataTypeBeginRange + 1),
	GraphicsDataTypeMaxEnum = 0x7FFFFFFF
};

enum GraphicsVertexType
{
	GraphicsVertexTypePointList = 0,
	GraphicsVertexTypeLineList = 1,
	GraphicsVertexTypeLineStrip = 2,
	GraphicsVertexTypeTriangleList = 3,
	GraphicsVertexTypeTriangleStrip = 4,
	GraphicsVertexTypeTriangleFan = 5,
	GraphicsVertexTypeLineListWithAdjacency = 6,
	GraphicsVertexTypeLineStripWithAdjacency = 7,
	GraphicsVertexTypeTriangleListWithAdjacency = 8,
	GraphicsVertexTypeTriangleStripWithAdjacency = 9,
	GraphicsVertexTypePatchList = 10,
	GraphicsVertexTypeBeginRange = GraphicsVertexTypePointList,
	GraphicsVertexTypeEndRange = GraphicsVertexTypePatchList,
	GraphicsVertexTypeRangeSize = (GraphicsVertexTypeEndRange - GraphicsVertexTypeBeginRange + 1),
	GraphicsVertexTypeMaxEnum = 0x7FFFFFFF
};

enum GraphicsVertexDivisor
{
	GraphicsVertexDivisorVertex = 0,
	GraphicsVertexDivisorInstance = 1,
	GraphicsVertexDivisorBeginRange = GraphicsVertexDivisorVertex,
	GraphicsVertexDivisorEndRange = GraphicsVertexDivisorInstance,
	GraphicsVertexDivisorRangeSize = (GraphicsVertexDivisorEndRange - GraphicsVertexDivisorBeginRange + 1),
	GraphicsVertexDivisorMaxEnum = 0x7FFFFFFF
};

enum GraphicsIndexType
{
	GraphicsIndexTypeNone,
	GraphicsIndexTypeUInt16,
	GraphicsIndexTypeUInt32,
	GraphicsIndexTypeBeginRange = GraphicsIndexTypeNone,
	GraphicsIndexTypeEndRange = GraphicsIndexTypeUInt32,
	GraphicsIndexTypeRangeSize = (GraphicsIndexTypeEndRange - GraphicsIndexTypeBeginRange + 1),
	GraphicsIndexTypeMaxEnum = 0x7FFFFFFF
};

enum GraphicsShaderStage
{
	GraphicsShaderStageNone,
	GraphicsShaderStageVertex,
	GraphicsShaderStageFragment,
	GraphicsShaderStageGeometry,
	GraphicsShaderStageCompute,
	GraphicsShaderStageTessEvaluation,
	GraphicsShaderStageTessControl,
	GraphicsShaderStageBeginRange = GraphicsShaderStageNone,
	GraphicsShaderStageEndRange = GraphicsShaderStageTessControl,
	GraphicsShaderStageRangeSize = (GraphicsShaderStageEndRange - GraphicsShaderStageBeginRange + 1),
	GraphicsShaderStageMaxEnum = 0x7FFFFFFF
};

enum GraphicsUniformType
{
	GraphicsUniformTypeNone,
    GraphicsUniformTypeBool,
	GraphicsUniformTypeBool2,
	GraphicsUniformTypeBool3,
	GraphicsUniformTypeBool4,
    GraphicsUniformTypeInt,
    GraphicsUniformTypeInt2,
    GraphicsUniformTypeInt3,
    GraphicsUniformTypeInt4,
    GraphicsUniformTypeUInt,
    GraphicsUniformTypeUInt2,
    GraphicsUniformTypeUInt3,
    GraphicsUniformTypeUInt4,
    GraphicsUniformTypeFloat,
    GraphicsUniformTypeFloat2,
    GraphicsUniformTypeFloat3,
    GraphicsUniformTypeFloat4,
	GraphicsUniformTypeFloat2x2,
    GraphicsUniformTypeFloat3x3,
    GraphicsUniformTypeFloat4x4,
    GraphicsUniformTypeBoolArray,
	GraphicsUniformTypeBool2Array,
	GraphicsUniformTypeBool3Array,
	GraphicsUniformTypeBool4Array,
    GraphicsUniformTypeIntArray,
    GraphicsUniformTypeInt2Array,
    GraphicsUniformTypeInt3Array,
    GraphicsUniformTypeInt4Array,
    GraphicsUniformTypeUIntArray,
    GraphicsUniformTypeUInt2Array,
    GraphicsUniformTypeUInt3Array,
    GraphicsUniformTypeUInt4Array,
    GraphicsUniformTypeHalfArray,
    GraphicsUniformTypeHalf2Array,
    GraphicsUniformTypeHalf3Array,
    GraphicsUniformTypeHalf4Array,
    GraphicsUniformTypeFloatArray,
    GraphicsUniformTypeFloat2Array,
    GraphicsUniformTypeFloat3Array,
    GraphicsUniformTypeFloat4Array,
	GraphicsUniformTypeFloat2x2Array,
    GraphicsUniformTypeFloat3x3Array,
    GraphicsUniformTypeFloat4x4Array,
	GraphicsUniformTypeSampler,
	GraphicsUniformTypeSamplerImage,
	GraphicsUniformTypeCombinedImageSampler,
	GraphicsUniformTypeStorageImage,
	GraphicsUniformTypeStorageTexelBuffer,
	GraphicsUniformTypeStorageBuffer,
	GraphicsUniformTypeStorageBufferDynamic,
	GraphicsUniformTypeUniformTexelBuffer,
	GraphicsUniformTypeUniformBuffer,
	GraphicsUniformTypeUniformBufferDynamic,
	GraphicsUniformTypeInputAttachment,
	GraphicsUniformTypeBeginRange = GraphicsUniformTypeNone,
	GraphicsUniformTypeEndRange = GraphicsUniformTypeInputAttachment,
	GraphicsUniformTypeRangeSize = (GraphicsUniformTypeInputAttachment - GraphicsUniformTypeNone + 1),
	GraphicsUniformTypeMaxEnum = 0x7FFFFFFF
};

enum GraphicsViewLayout
{
	GraphicsViewLayoutUndefined,
	GraphicsViewLayoutGeneral,
	GraphicsViewLayoutColorAttachmentOptimal,
	GraphicsViewLayoutDepthStencilAttachmentOptimal,
	GraphicsViewLayoutDepthStencilReadOnlyOptimal,
	GraphicsViewLayoutShaderReadOnlyOptimal,
	GraphicsViewLayoutTransferSrcOptimal,
	GraphicsViewLayoutTransferDstOptimal,
	GraphicsViewLayoutPreinitialized,
	GraphicsViewLayoutPresentSrcKhr,
	GraphicsViewLayoutBeginRange = GraphicsViewLayoutUndefined,
	GraphicsViewLayoutEndRange = GraphicsViewLayoutPreinitialized,
	GraphicsViewLayoutRangeSize = (GraphicsViewLayoutPreinitialized - GraphicsViewLayoutUndefined + 1),
	GraphicsViewLayoutMaxEnum = 0x7FFFFFFF
};

enum GraphicsViewUsageFlagBits
{
	GraphicsViewUsageFlagBitsTransferSrcBit = 0x00000001,
	GraphicsViewUsageFlagBitsTransferDstBit = 0x00000002,
	GraphicsViewUsageFlagBitsSampledBit = 0x00000004,
	GraphicsViewUsageFlagBitsStorageBit = 0x00000008,
	GraphicsViewUsageFlagBitsColorAttachmentBit = 0x00000010,
	GraphicsViewUsageFlagBitsDepthStencilAttachmentBit = 0x00000020,
	GraphicsViewUsageFlagBitsTransientAttachmentBit = 0x00000040,
	GraphicsViewUsageFlagBitsInputAttachmentBit = 0x00000080,
};

enum GraphicsUsageFlags
{
	GraphicsUsageFlagsReadBit = 0x00000001,
	GraphicsUsageFlagsWriteBit = 0x00000002,
	GraphicsUsageFlagsPersistentBit = 0x00000004,
	GraphicsUsageFlagsCoherentBit = 0x00000008,
	GraphicsUsageFlagsFlushExplicitBit = 0x00000010,
	GraphicsUsageFlagsDynamicStorageBit = 0x00000020,
	GraphicsUsageFlagsClientStorageBit = 0x00000040,
	GraphicsUsageFlagsImmutableStorage = 0x00000080
};

enum GraphicsAccessFlagsBits
{
	GraphicsAccessFlagsMapReadBit = 0x00000001,
	GraphicsAccessFlagsMapWriteBit = 0x00000002,
	GraphicsAccessFlagsUnsynchronizedBit = 0x00000004
};

enum GraphicsCommandType
{
	GraphicsCommandTypeGraphics = 0,
	GraphicsCommandTypeCompute = 1,
	GraphicsCommandTypeBundle = 2,
	GraphicsCommandTypeCopy = 3
};

enum GraphicsCommandQueueFlags
{
	GraphicsCommandQueueFlagsNone = 0,
	GraphicsCommandQueueFlagsDisableGpuTimeOut = 0x1
};

enum GraphicsCommandQueuePriority
{
	GraphicsCommandQueuePriorityNormal = 0,
	GraphicsCommandQueuePriorityHigh = 100
};

enum GraphicsCommandPoolFlags
{
	GraphicsCommandPoolTransientBit = 0x00000001,
	GraphicsCommandPoolResetCommandBuffer = 0x00000002
};

_NAME_END

#endif