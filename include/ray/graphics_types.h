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
#ifndef _H_GRAPHICS_TYPES_H_
#define _H_GRAPHICS_TYPES_H_

#include <ray/memory.h>
#include <ray/math.h>
#include <ray/except.h>
#include <ray/variant.h>
#include <ray/model.h>

_NAME_BEGIN

#ifndef MAX_VERTEX_UNIT
#	define MAX_VERTEX_UNIT 16
#endif

#ifndef MAX_TEXTURE_UNIT
#	define MAX_TEXTURE_UNIT 32
#endif

#ifndef MAX_SAMPLER_UNIT
#	define MAX_SAMPLER_UNIT 32
#endif

#ifndef MAX_COLOR_ATTACHMENTS
#	define MAX_COLOR_ATTACHMENTS 15
#endif

typedef std::shared_ptr<class GraphicsData> GraphicsDataPtr;
typedef std::shared_ptr<class GraphicsResource> GraphicsResourcePtr;
typedef std::shared_ptr<class GraphicsLayout> GraphicsLayoutPtr;
typedef std::shared_ptr<class GraphicsDevice> GraphicsDevicePtr;
typedef std::shared_ptr<class GraphicsContext> GraphicsContextPtr;
typedef std::shared_ptr<class GraphicsState> GraphicsStatePtr;
typedef std::shared_ptr<class GraphicsTexture> GraphicsTexturePtr;
typedef std::shared_ptr<class GraphicsSampler> GraphicsSamplerPtr;
typedef std::shared_ptr<class GraphicsRenderTexture> GraphicsRenderTexturePtr;
typedef std::shared_ptr<class GraphicsMultiRenderTexture> GraphicsMultiRenderTexturePtr;
typedef std::shared_ptr<class GraphicsShader> GraphicsShaderPtr;
typedef std::shared_ptr<class GraphicsProgram> GraphicsProgramPtr;

typedef std::weak_ptr<GraphicsDevice>  GraphicsDeviceWeakPtr;

typedef std::shared_ptr<class ShaderDesc> ShaderDescPtr;
typedef std::shared_ptr<class ShaderVariant> ShaderVariantPtr;
typedef std::shared_ptr<class ShaderParameter> ShaderParameterPtr;
typedef std::shared_ptr<class ShaderAttribute> ShaderAttributePtr;
typedef std::shared_ptr<class ShaderUniform> ShaderUniformPtr;
typedef std::shared_ptr<class ShaderSubroutine> ShaderSubroutinePtr;
typedef std::shared_ptr<class ShaderObjectDesc> ShaderObjectDescPtr;

typedef std::shared_ptr<class RenderIndirect> RenderIndirectPtr;

typedef std::vector<ShaderDesc> ShadersDesc;
typedef std::vector<ShaderAttributePtr> ShaderAttributes;
typedef std::vector<ShaderUniformPtr> ShaderUniforms;
typedef std::vector<ShaderSubroutinePtr> ShaderSubroutines;
typedef std::vector<ShaderVariantPtr> ShaderVariants;
typedef std::vector<GraphicsRenderTexturePtr> GraphicsRenderTextures;
typedef std::vector<class VertexComponent> VertexComponents;
typedef std::vector<RenderIndirectPtr> RenderIndirects;

typedef void* WindHandle;

enum
{
	GraphicsDevice_D3D,
	GraphicsDevice_OPENGL,
	GraphicsDevice_OPENGL_ES
};

enum SwapInterval
{
	Free,
	Vsync,
	Fps30,
	Fps15,
};

enum GLapi
{
	OPENGL_API,
	OPENGL_ES_API
};

enum GLattr
{
	GL_RED_SIZE,
	GL_GREEN_SIZE,
	GL_BLUE_SIZE,
	GL_ALPHA_SIZE,
	GL_BUFFER_SIZE,
	GL_DOUBLEBUFFER,
	GL_DEPTH_SIZE,
	GL_STENCIL_SIZE,
	GL_ACCUM_RED_SIZE,
	GL_ACCUM_GREEN_SIZE,
	GL_ACCUM_BLUE_SIZE,
	GL_ACCUM_ALPHA_SIZE,
	GL_STEREO,
	GL_MULTISAMPLEBUFFERS,
	GL_MULTISAMPLESAMPLES,
	GL_ACCELERATED_VISUAL,
	GL_RETAINED_BACKING,
	GL_CONTEXT_MAJOR_VERSION,
	GL_CONTEXT_MINOR_VERSION,
	GL_CONTEXT_EGL,
	GL_CONTEXT_FLAGS,
	GL_CONTEXT_PROFILE_MASK,
	GL_SHARE_WITH_CURRENT_CONTEXT,
	GL_FRAMEBUFFER_SRGB_CAPABLE,
	GL_DEBUG_FLAG,
	GL_CORE_PROFILE,
	GL_COMPAT_PROFILE,
	GL_ANY_PROFILE,
	GL_REST_NOTIFICATION,
	GL_LOSE_CONTEXT_ONREST
};

enum CompareFunction
{
	None,
	Lequal,
	Equal,
	Greater,
	Less,
	Gequal,
	NotEqual,
	Always,
	Never,
};

enum BlendFactor
{
	Zero,
	One,
	DstCol,
	SrcColor,
	SrcAlpha,
	DstAlpha,
	OneMinusSrcCol,
	OneMinusDstCol,
	OneMinusSrcAlpha,
	OneMinusDstAlpha,
	ConstantColor,
	ConstantAlpha,
	OneMinusConstantColor,
	OneMinusConstantAlpha,
	SrcAlphaSaturate
};

enum BlendOperation
{
	Add,
	Subtract,
	RevSubtract,
};

enum ColorMask
{
	COLORMASK_RED = 1UL << 0,
	COLORMASK_GREEN = 1UL << 1,
	COLORMASK_BLUE = 1UL << 2,
	COLORMASK_ALPHA = 1UL << 3,
	COLORMASK_RGB = COLORMASK_RED | COLORMASK_GREEN | COLORMASK_BLUE,
	COLORMASK_RGBA = COLORMASK_RED | COLORMASK_GREEN | COLORMASK_BLUE | COLORMASK_ALPHA
};

enum class CullMode
{
	None,
	Front,
	Back,
	FrontBack,
};

enum class FillMode
{
	PointMode,
	WireframeMode,
	SolidMode,
};

enum class StencilOperation
{
	Keep,
	Replace,
	Incr,
	Decr,
	Zero,
	IncrWrap,
	DecrWrap
};

enum ClearFlags
{
	Color = 1UL << 0,
	Depth = 1UL << 1,
	Stencil = 1UL << 2,
	ColorDepth = Color | Depth,
	ColorStencil = Color | Stencil,
	DepthStencil = Depth | Stencil,
	All = Color | Depth | Stencil
};

enum class TextureFormat
{
	STENCIL8,
	DEPTH_COMPONENT16,
	DEPTH_COMPONENT24,
	DEPTH_COMPONENT32,
	DEPTH24_STENCIL8,
	DEPTH32_STENCIL8,
	R4G4B4A4,
	R5G6B5,
	R5G5B5A1,
	R10G10B10A2,
	R8G8B8,
	R8G8B8A8,
	R8G8B8_SNORM,
	R8G8B8A8_SNORM,
	R16G16B16,
	R16G16B16A16,
	R16G16B16_SNORM,
	R16G16B16A16_SNORM,
	R16G16B16F,
	R32G32B32F,
	R16G16B16A16F,
	R32G32B32A32F,
	R11G11B10F,
	SR8G8B8,
	SR8G8B8A8,
	R8,
	R16F,
	R32F,
	RG16F,
	RG32F,
	RGB_DXT1,
	RGBA_DXT1,
	RGBA_DXT3,
	RGBA_DXT5,
	RG_ATI2,
};

enum class TextureDim
{
	DIM_2D,
	DIM_3D,
	DIM_2D_ARRAY,
	DIM_3D_ARRAY,
	DIM_CUBE,
	DIM_CUBE_ARRAY,
};

enum class SamplerAnis
{
	Anis0,
	Anis1,
	Anis2,
	Anis4,
	Anis8,
	Anis16,
};

enum class SamplerOp
{
	Multiply,    //* T = T1 * T2
	Add,         //* T = T1 + T2
	Subtract,    //* T = T1 - T2
	Divide,      //* T = T1 / T2
	SmoothAdd,   //* T = (T1 + T2) - (T1 * T2)
	SignedAdd,   //* T = T1 + (T2-0.5)
};

enum class SamplerWrap
{
	Repeat,
	Mirror,
	ClampToEdge,
};

enum class SamplerFilter
{
	Nearest,
	Linear,
	NearestMipmapLinear,
	NearestMipmapNearest,
	LinearMipmapNearest,
	LinearMipmapLinear,
};

enum class GraphicsStream
{
	VBO,
	IBO,
	UBO,
	DIBO,
	TBO,
	SSBO
};

enum class VertexType
{
	Point,
	Line,
	Triangle,
	Fan,
	PointOrLine,
	TriangleOrLine,
	FanOrLine,
};

enum class VertexFormat
{
	Char,
	Char2,
	Char3,
	Char4,
	Short,
	Short2,
	Short3,
	Short4,
	Int,
	Int2,
	Int3,
	Int4,
	Uchar,
	Uchar2,
	Uchar3,
	Uchar4,
	Ushort,
	Ushort2,
	Ushort3,
	Ushort4,
	Uint,
	Uint2,
	Uint3,
	Uint4,
	Float,
	Float2,
	Float3,
	Float4,
	Float3x3,
	Float4x4,
};

enum class IndexType
{
	None,
	Uint16,
	Uint32
};

enum class ShaderType
{
	Vertex,
	Fragment,
	Geometry,
	Compute,
	TessEvaluation,
	TessControl,
};

enum class ShaderVariantType
{
	None,
	Bool,
	Int,
	Int2,
	Int3,
	Int4,
	Float,
	Float2,
	Float3,
	Float4,
	Float3x3,
	Float4x4,
	FloatArray,
	Float2Array,
	Float3Array,
	Float4Array,
	Texture,
	Buffer,
};

class EXPORT RenderIndirect final
{
public:
	std::int32_t startVertice;
	std::int32_t startIndice;
	std::int32_t startInstances;
	std::int32_t numVertices;
	std::int32_t numIndices;
	std::int32_t numInstances;

	RenderIndirect() noexcept;
};

_NAME_END

#endif