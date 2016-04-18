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
#include <ray/rtti.h>

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
typedef std::shared_ptr<class GraphicsPipelineLayout> GraphicsPipelineLayoutPtr;
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
typedef std::weak_ptr<class GraphicsFramebufferLayout> GraphicsFramebufferLayoutWeakPtr;
typedef std::weak_ptr<class GraphicsShader> GraphicsShaderWeakPtr;
typedef std::weak_ptr<class GraphicsProgram> GraphicsProgramWeakPtr;
typedef std::weak_ptr<class GraphicsPipeline> GraphicsPipelineWeakPtr;
typedef std::weak_ptr<class GraphicsPipelineLayout> GraphicsPipelineLayoutWeakPtr;
typedef std::weak_ptr<class GraphicsDescriptorPool> GraphicsDescriptorPoolWeakPtr;
typedef std::weak_ptr<class GraphicsDescriptorSet> GraphicsDescriptorSetWeakPtr;
typedef std::weak_ptr<class GraphicsDescriptorSetLayout> GraphicsDescriptorSetLayoutWeakPtr;
typedef std::weak_ptr<class GraphicsUniformSet> GraphicsUniformWeakPtr;
typedef std::weak_ptr<class GraphicsVariant> GraphicsVariantWeakPtr;
typedef std::weak_ptr<class GraphicsSystem> GraphicsSystemWeakPtr;
typedef std::weak_ptr<class GraphicsCommandPool> GraphicsCommandPoolWeakPtr;
typedef std::weak_ptr<class GraphicsCommandQueue> GraphicsCommandQueueWeakPtr;
typedef std::weak_ptr<class GraphicsCommandList> GraphicsCommandPoolListWeakPtr;
typedef std::weak_ptr<class GraphicsSemaphoreDesc> GraphicsSemaphoreDescWeakPtr;

typedef std::vector<GraphicsShaderPtr> GraphicsShaders;
typedef std::vector<GraphicsVariantPtr> GraphicsVariants;
typedef std::vector<GraphicsFramebufferPtr> GraphicsFramebuffers;
typedef std::vector<GraphicsIndirectPtr> GraphicsIndirects;
typedef std::vector<GraphicsUniformSetPtr> GraphicsUniformSets;
typedef std::vector<GraphicsTexturePtr> GraphicsTextures;
typedef std::vector<GraphicsAttributePtr> GraphicsAttributes;
typedef std::vector<GraphicsUniformPtr> GraphicsUniforms;
typedef std::vector<GraphicsUniformBlockPtr> GraphicsUniformBlocks;
typedef std::vector<GraphicsDescriptorSetLayoutPtr> GraphicsDescriptorSetLayouts;
typedef std::vector<GraphicsInputLayoutPtr> GraphicsInputLayouts;
typedef std::vector<class GraphicsVertexLayout> GraphicsVertexLayouts;
typedef std::vector<class GraphicsDescriptorPoolComponent> GraphicsDescriptorPoolComponents;
typedef std::vector<class GraphicsAttachmentDesc> GraphicsAttachmentDescs;

class GraphicsDeviceDesc;
class GraphicsSwapchainDesc;
class GraphicsContextDesc;
class GraphicsInputLayoutDesc;
class GraphicsDataDesc;
class GraphicsTextureDesc;
class GraphicsSamplerDesc;
class GraphicsFramebufferDesc;
class GraphicsFramebufferLayoutDesc;
class GraphicsStateDesc;
class GraphicsShaderDesc;
class GraphicsProgramDesc;
class GraphicsPipelineDesc;
class GraphicsPipelineLayoutDesc;
class GraphicsDescriptorSetDesc;
class GraphicsDescriptorSetLayoutDesc;
class GraphicsDescriptorPoolDesc;
class GraphicsCommandQueueDesc;
class GraphicsCommandPoolDesc;
class GraphicsCommandListDesc;
class GraphicsSemaphoreDesc;

typedef void* WindHandle;

enum GraphicsDeviceType
{
	GraphicsDeviceTypeD3D9 = 0,
	GraphicsDeviceTypeD3D11 = 1,
	GraphicsDeviceTypeD3D12 = 2,
	GraphicsDeviceTypeOpenGL = 3,
	GraphicsDeviceTypeOpenGLCore = 4,
	GraphicsDeviceTypeOpenGLES2 = 5,
	GraphicsDeviceTypeOpenGLES3 = 6,
	GraphicsDeviceTypeOpenGLES31 = 7,
	GraphicsDeviceTypeVulkan = 8,
	GraphicsDeviceTypeBeginRange = GraphicsDeviceTypeD3D9,
	GraphicsDeviceTypeEndRange = GraphicsDeviceTypeVulkan,
	GraphicsDeviceTypeRangeSize = (GraphicsDeviceTypeEndRange - GraphicsDeviceTypeBeginRange + 1),
	GraphicsDeviceTypeMaxEnum = 0x7FFFFFFF
};

enum GraphicsSwapInterval
{
	GraphicsSwapIntervalFree = 0,
	GraphicsSwapIntervalVsync = 1,
	GraphicsSwapIntervalFps30 = 2,
	GraphicsSwapIntervalFps15 = 3,
	GraphicsSwapIntervalBeginRange = GraphicsSwapIntervalFree,
	GraphicsSwapIntervalEndRange = GraphicsSwapIntervalFps15,
	GraphicsSwapIntervalRangeSize = (GraphicsSwapIntervalEndRange - GraphicsSwapIntervalBeginRange + 1),
	GraphicsSwapIntervalMaxEnum = 0x7FFFFFFF
};

enum GraphicsCompareFunc
{
	GraphicsCompareFuncNone = 0,
	GraphicsCompareFuncLequal = 1,
	GraphicsCompareFuncEqual = 2,
	GraphicsCompareFuncGreater = 3,
	GraphicsCompareFuncLess = 4,
	GraphicsCompareFuncGequal = 5,
	GraphicsCompareFuncNotEqual = 6,
	GraphicsCompareFuncAlways = 7,
	GraphicsCompareFuncNever = 8,
	GraphicsCompareFuncBeginRange = GraphicsCompareFuncNone,
	GraphicsCompareFuncEndRange = GraphicsCompareFuncNever,
	GraphicsCompareFuncRangeSize = (GraphicsCompareFuncEndRange - GraphicsCompareFuncBeginRange + 1),
	GraphicsCompareFuncMaxEnum = 0x7FFFFFFF
};

enum GraphicsBlendFactor
{
	GraphicsBlendFactorZero = 0,
	GraphicsBlendFactorOne = 1,
	GraphicsBlendFactorDstCol = 2,
	GraphicsBlendFactorSrcColor = 3,
	GraphicsBlendFactorSrcAlpha = 4,
	GraphicsBlendFactorDstAlpha = 5,
	GraphicsBlendFactorOneMinusSrcCol = 6,
	GraphicsBlendFactorOneMinusDstCol = 7,
	GraphicsBlendFactorOneMinusSrcAlpha = 8,
	GraphicsBlendFactorOneMinusDstAlpha = 9,
	GraphicsBlendFactorConstantColor = 10,
	GraphicsBlendFactorConstantAlpha = 11,
	GraphicsBlendFactorOneMinusConstantColor = 12,
	GraphicsBlendFactorOneMinusConstantAlpha = 13,
	GraphicsBlendFactorSrcAlphaSaturate = 14,
	GraphicsBlendFactorBeginRange = GraphicsBlendFactorZero,
	GraphicsBlendFactorEndRange = GraphicsBlendFactorSrcAlphaSaturate,
	GraphicsBlendFactorRangeSize = (GraphicsBlendFactorEndRange - GraphicsBlendFactorBeginRange + 1),
	GraphicsBlendFactorMaxEnum = 0x7FFFFFFF
};

enum GraphicsBlendOp
{
	GraphicsBlendOpAdd = 0,
	GraphicsBlendOpSubtract = 1,
	GraphicsBlendOpRevSubtract = 2,
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
	GraphicsCullModeNone = 0,
	GraphicsCullModeFront = 1,
	GraphicsCullModeBack = 2,
	GraphicsCullModeFrontBack = 3,
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
	GraphicsPolygonModePoint = 0,
	GraphicsPolygonModeWireframe = 1,
	GraphicsPolygonModeSolid = 2,
	GraphicsPolygonModeBeginRange = GraphicsPolygonModePoint,
	GraphicsPolygonModeEndRange = GraphicsPolygonModeSolid,
	GraphicsPolygonModeRangeSize = (GraphicsPolygonModeEndRange - GraphicsPolygonModeBeginRange + 1),
	GraphicsPolygonModeMaxEnum = 0x7FFFFFFF
};

enum GraphicsStencilOp
{
	GraphicsStencilOpKeep = 0,
	GraphicsStencilOpReplace = 1,
	GraphicsStencilOpIncr = 2,
	GraphicsStencilOpDecr = 3,
	GraphicsStencilOpZero = 4,
	GraphicsStencilOpIncrWrap = 5,
	GraphicsStencilOpDecrWrap = 6,
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
	GraphicsFormatTypeNone = 0,
	GraphicsFormatTypeRInt = 1,
	GraphicsFormatTypeRUInt = 2,
	GraphicsFormatTypeRSRGB = 3,
	GraphicsFormatTypeRSNorm = 4,
	GraphicsFormatTypeRUNorm = 5,
	GraphicsFormatTypeRSScaled = 6,
	GraphicsFormatTypeRUScaled = 7,
	GraphicsFormatTypeRFloat = 8,
	GraphicsFormatTypeRGInt = 9,
	GraphicsFormatTypeRGUInt = 10,
	GraphicsFormatTypeRGSRGB = 11,
	GraphicsFormatTypeRGSNorm = 12,
	GraphicsFormatTypeRGUNorm = 13,
	GraphicsFormatTypeRGSScaled = 14,
	GraphicsFormatTypeRGUScaled = 15,
	GraphicsFormatTypeRGFloat = 16,
	GraphicsFormatTypeRGBInt = 17,
	GraphicsFormatTypeRGBUInt = 18,
	GraphicsFormatTypeRGBSRGB = 19,
	GraphicsFormatTypeRGBSNorm = 20,
	GraphicsFormatTypeRGBUNorm = 21,
	GraphicsFormatTypeRGBSScaled = 22,
	GraphicsFormatTypeRGBUScaled =23,
	GraphicsFormatTypeRGBFloat = 24,
	GraphicsFormatTypeRGBAInt = 25,
	GraphicsFormatTypeRGBAUInt = 26,
	GraphicsFormatTypeRGBASRGB = 27,
	GraphicsFormatTypeRGBASNorm = 28,
	GraphicsFormatTypeRGBAUNorm = 29,
	GraphicsFormatTypeRGBASScaled = 30,
	GraphicsFormatTypeRGBAUScaled = 31,
	GraphicsFormatTypeRGBAFloat = 32,
	GraphicsFormatTypeBGRInt = 33,
	GraphicsFormatTypeBGRUInt = 34,
	GraphicsFormatTypeBGRSRGB = 35,
	GraphicsFormatTypeBGRSNorm = 36,
	GraphicsFormatTypeBGRUNorm = 37,
	GraphicsFormatTypeBGRSScaled = 38,
	GraphicsFormatTypeBGRUScaled = 39,
	GraphicsFormatTypeBGRFloat = 40,
	GraphicsFormatTypeBGRAInt = 41,
	GraphicsFormatTypeBGRAUInt = 42,
	GraphicsFormatTypeBGRASRGB = 43,
	GraphicsFormatTypeBGRASNorm = 44,
	GraphicsFormatTypeBGRAUNorm = 45,
	GraphicsFormatTypeBGRASScaled = 46,
	GraphicsFormatTypeBGRAUScaled = 47,
	GraphicsFormatTypeBGRAFloat = 48,
	GraphicsFormatTypeABGRInt = 49,
	GraphicsFormatTypeABGRUInt = 50,
	GraphicsFormatTypeABGRSRGB = 51,
	GraphicsFormatTypeABGRSNorm = 52,
	GraphicsFormatTypeABGRUNorm = 53,
	GraphicsFormatTypeABGRSScaled = 54,
	GraphicsFormatTypeABGRUScaled = 55,
	GraphicsFormatTypeABGRFloat = 56,
	GraphicsFormatTypeDepth = 57,
	GraphicsFormatTypeDepthStencil = 58,
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
	GraphicsTextureDim2D = 0,
	GraphicsTextureDim3D = 1,
	GraphicsTextureDimCube = 2,
	GraphicsTextureDim2DArray = 3,
	GraphicsTextureDimCubeArray = 4,
	GraphicsTextureDimBeginRange = GraphicsTextureDim2D,
	GraphicsTextureDimEndRange = GraphicsTextureDimCubeArray,
	GraphicsTextureDimRangeSize = (GraphicsTextureDimEndRange - GraphicsTextureDimBeginRange + 1),
	GraphicsTextureDimMaxEnum = 0x7FFFFFFF
};

enum GraphicsSamplerAnis
{
	GraphicsSamplerAnis1 = 0,
	GraphicsSamplerAnis2 = 1,
	GraphicsSamplerAnis4 = 2,
	GraphicsSamplerAnis8 = 3,
	GraphicsSamplerAnis16 = 4,
	GraphicsSamplerAnis32 = 5,
	GraphicsSamplerAnis64 = 6,
	GraphicsSamplerAnisBeginRange = GraphicsSamplerAnis1,
	GraphicsSamplerAnisEndRange = GraphicsSamplerAnis16,
	GraphicsSamplerAnisRangeSize = (GraphicsSamplerAnisEndRange - GraphicsSamplerAnisBeginRange + 1),
	GraphicsSamplerAnisMaxEnum = 0x7FFFFFFF
};

enum GraphicsSamplerOp
{
	GraphicsSamplerOpMultiply = 0,    //* T = T1 * T2
	GraphicsSamplerOpAdd = 1,         //* T = T1 + T2
	GraphicsSamplerOpSubtract = 2,    //* T = T1 - T2
	GraphicsSamplerOpDivide = 3,      //* T = T1 / T2
	GraphicsSamplerOpSmoothAdd = 4,   //* T = (T1 + T2) - (T1 * T2)
	GraphicsSamplerOpSignedAdd = 5,   //* T = T1 + (T2-0.5)
	GraphicsSamplerOpBeginRange = GraphicsSamplerOpMultiply,
	GraphicsSamplerOpEndRange = GraphicsSamplerOpSignedAdd,
	GraphicsSamplerOpRangeSize = (GraphicsSamplerOpEndRange - GraphicsSamplerOpBeginRange + 1),
	GraphicsSamplerOpMaxEnum = 0x7FFFFFFF
};

enum GraphicsSamplerWrap
{
	GraphicsSamplerWrapNone = 0,
	GraphicsSamplerWrapRepeat = 1,
	GraphicsSamplerWrapMirror = 2,
	GraphicsSamplerWrapClampToEdge = 3,
	GraphicsSamplerWrapBeginRange = GraphicsSamplerWrapNone,
	GraphicsSamplerWrapEndRange = GraphicsSamplerWrapClampToEdge,
	GraphicsSamplerWrapRangeSize = (GraphicsSamplerWrapEndRange - GraphicsSamplerWrapBeginRange + 1),
	GraphicsSamplerWrapMaxEnum = 0x7FFFFFFF
};

enum GraphicsSamplerFilter
{
	GraphicsSamplerFilterNearest = 0,
	GraphicsSamplerFilterLinear = 1,
	GraphicsSamplerFilterNearestMipmapLinear = 2,
	GraphicsSamplerFilterNearestMipmapNearest = 3,
	GraphicsSamplerFilterLinearMipmapNearest = 4,
	GraphicsSamplerFilterLinearMipmapLinear = 5,
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
	GraphicsDataTypeNone = 0,
	GraphicsDataTypeTransferSrc = 1,
	GraphicsDataTypeTransferDst = 2,
	GraphicsDataTypeUniformTexelBuffer = 3,
	GraphicsDataTypeUniformBuffer = 4,
	GraphicsDataTypeStorageTexelBuffer = 5,
	GraphicsDataTypeStorageBuffer = 6,
	GraphicsDataTypeStorageVertexBuffer = 7,
	GraphicsDataTypeStorageIndexBuffer = 8,
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
	GraphicsIndexTypeNone = 0,
	GraphicsIndexTypeUInt16 = 1,
	GraphicsIndexTypeUInt32 = 2,
	GraphicsIndexTypeBeginRange = GraphicsIndexTypeNone,
	GraphicsIndexTypeEndRange = GraphicsIndexTypeUInt32,
	GraphicsIndexTypeRangeSize = (GraphicsIndexTypeEndRange - GraphicsIndexTypeBeginRange + 1),
	GraphicsIndexTypeMaxEnum = 0x7FFFFFFF
};

enum GraphicsShaderStage
{
	GraphicsShaderStageNone = 0,
	GraphicsShaderStageVertex = 1,
	GraphicsShaderStageFragment = 2,
	GraphicsShaderStageGeometry = 3,
	GraphicsShaderStageCompute = 4,
	GraphicsShaderStageTessEvaluation = 5,
	GraphicsShaderStageTessControl = 6,
	GraphicsShaderStageBeginRange = GraphicsShaderStageNone,
	GraphicsShaderStageEndRange = GraphicsShaderStageTessControl,
	GraphicsShaderStageRangeSize = (GraphicsShaderStageEndRange - GraphicsShaderStageBeginRange + 1),
	GraphicsShaderStageMaxEnum = 0x7FFFFFFF
};

enum GraphicsShaderLang
{
	GraphicsShaderLangNone = 0,
	GraphicsShaderLangHLSL = 1,
	GraphicsShaderLangHLSLbytecodes = 2,
	GraphicsShaderLangGLSL = 3,
	GraphicsShaderLangBeginRange = GraphicsShaderLangHLSL,
	GraphicsShaderLangEndRange = GraphicsShaderLangGLSL,
	GraphicsShaderLangRangeSize = (GraphicsShaderLangEndRange - GraphicsShaderLangBeginRange + 1),
	GraphicsShaderLangMaxEnum = 0x7FFFFFFF
};

enum GraphicsUniformType
{
	GraphicsUniformTypeNone = 0,
    GraphicsUniformTypeBool = 1,
	GraphicsUniformTypeBool2 = 2,
	GraphicsUniformTypeBool3 = 3,
	GraphicsUniformTypeBool4 = 4,
    GraphicsUniformTypeInt = 5,
    GraphicsUniformTypeInt2 = 6,
    GraphicsUniformTypeInt3 = 7,
    GraphicsUniformTypeInt4 = 8,
    GraphicsUniformTypeUInt = 9,
    GraphicsUniformTypeUInt2 = 10,
    GraphicsUniformTypeUInt3 = 11,
    GraphicsUniformTypeUInt4 = 12,
    GraphicsUniformTypeFloat = 13,
    GraphicsUniformTypeFloat2 = 14,
    GraphicsUniformTypeFloat3 = 15,
    GraphicsUniformTypeFloat4 = 16,
	GraphicsUniformTypeFloat2x2 = 17,
    GraphicsUniformTypeFloat3x3 = 18,
    GraphicsUniformTypeFloat4x4 = 19,
	GraphicsUniformTypeBoolArray = 20,
	GraphicsUniformTypeBool2Array = 21,
	GraphicsUniformTypeBool3Array = 22,
	GraphicsUniformTypeBool4Array = 23,
    GraphicsUniformTypeIntArray = 24,
    GraphicsUniformTypeInt2Array = 25,
    GraphicsUniformTypeInt3Array = 26,
    GraphicsUniformTypeInt4Array = 27,
    GraphicsUniformTypeUIntArray = 28,
    GraphicsUniformTypeUInt2Array = 29,
    GraphicsUniformTypeUInt3Array = 30,
    GraphicsUniformTypeUInt4Array = 31,
    GraphicsUniformTypeFloatArray = 32,
    GraphicsUniformTypeFloat2Array = 33,
    GraphicsUniformTypeFloat3Array = 34,
    GraphicsUniformTypeFloat4Array = 35,
	GraphicsUniformTypeFloat2x2Array = 36,
    GraphicsUniformTypeFloat3x3Array = 37,
    GraphicsUniformTypeFloat4x4Array = 38,
	GraphicsUniformTypeSampler = 39,
	GraphicsUniformTypeSamplerImage = 40,
	GraphicsUniformTypeCombinedImageSampler = 41,
	GraphicsUniformTypeStorageImage = 42,
	GraphicsUniformTypeStorageTexelBuffer = 43,
	GraphicsUniformTypeStorageBuffer = 44,
	GraphicsUniformTypeStorageBufferDynamic = 45,
	GraphicsUniformTypeUniformTexelBuffer = 46,
	GraphicsUniformTypeUniformBuffer = 47,
	GraphicsUniformTypeUniformBufferDynamic = 48,
	GraphicsUniformTypeInputAttachment = 49,
	GraphicsUniformTypeBeginRange = GraphicsUniformTypeNone,
	GraphicsUniformTypeEndRange = GraphicsUniformTypeInputAttachment,
	GraphicsUniformTypeRangeSize = (GraphicsUniformTypeInputAttachment - GraphicsUniformTypeNone + 1),
	GraphicsUniformTypeMaxEnum = 0x7FFFFFFF
};

enum GraphicsViewLayout
{
	GraphicsViewLayoutUndefined = 0,
	GraphicsViewLayoutGeneral = 1,
	GraphicsViewLayoutColorAttachmentOptimal = 2,
	GraphicsViewLayoutDepthStencilAttachmentOptimal = 3,
	GraphicsViewLayoutDepthStencilReadOnlyOptimal = 4,
	GraphicsViewLayoutShaderReadOnlyOptimal = 5,
	GraphicsViewLayoutTransferSrcOptimal = 6,
	GraphicsViewLayoutTransferDstOptimal = 7,
	GraphicsViewLayoutPreinitialized = 8,
	GraphicsViewLayoutPresentSrcKhr = 9,
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
	GraphicsCommandTypeCopy = 3,
	GraphicsCommandTypeBeginRange = GraphicsCommandTypeGraphics,
	GraphicsCommandTypeEndRange = GraphicsCommandTypeCopy,
	GraphicsCommandTypeRangeSize = (GraphicsCommandTypeEndRange - GraphicsCommandTypeBeginRange + 1),
	GraphicsCommandTypeMaxEnum = 0x7FFFFFFF
};

enum GraphicsCommandQueueFlags
{
	GraphicsCommandQueueFlagsNone = 0,
	GraphicsCommandQueueFlagsDisableGpuTimeOut = 0x1
};

enum GraphicsCommandQueuePriority
{
	GraphicsCommandQueuePriorityNormal = 0,
	GraphicsCommandQueuePriorityHigh = 1,
	GraphicsCommandQueuePriorityBeginRange = GraphicsCommandQueuePriorityNormal,
	GraphicsCommandQueuePriorityEndRange = GraphicsCommandQueuePriorityHigh,
	GraphicsCommandQueuePriorityRangeSize = (GraphicsCommandQueuePriorityEndRange - GraphicsCommandQueuePriorityBeginRange + 1),
	GraphicsCommandQueuePriorityMaxEnum = 0x7FFFFFFF
};

enum GraphicsCommandPoolFlags
{
	GraphicsCommandPoolTransientBit = 0x00000001,
	GraphicsCommandPoolResetCommandBuffer = 0x00000002
};

_NAME_END

#endif