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

typedef std::shared_ptr<class Shader> ShaderPtr;
typedef std::shared_ptr<class ShaderVariant> ShaderVariantPtr;
typedef std::shared_ptr<class ShaderParameter> ShaderParameterPtr;
typedef std::shared_ptr<class ShaderAttribute> ShaderAttributePtr;
typedef std::shared_ptr<class ShaderUniform> ShaderUniformPtr;
typedef std::shared_ptr<class ShaderSubroutine> ShaderSubroutinePtr;
typedef std::shared_ptr<class ShaderObject> ShaderObjectPtr;

typedef std::vector<ShaderPtr> Shaders;
typedef std::vector<ShaderAttributePtr> ShaderAttributes;
typedef std::vector<ShaderUniformPtr> ShaderUniforms;
typedef std::vector<ShaderSubroutinePtr> ShaderSubroutines;
typedef std::vector<ShaderVariantPtr> ShaderVariants;
typedef std::vector<GraphicsRenderTexturePtr> GraphicsRenderTextures;
typedef std::vector<class VertexComponent> VertexComponents;
typedef std::shared_ptr<class RenderIndirect> RenderIndirectPtr;
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
	GPU_FREE,
	GPU_VSYNC,
	GPU_FPS30,
	GPU_FPS15,
};

enum GLapi
{
	GPU_OPENGL_API,
	GPU_OPENGL_ES_API
};

enum GLattr
{
	GPU_GL_RED_SIZE,
	GPU_GL_GREEN_SIZE,
	GPU_GL_BLUE_SIZE,
	GPU_GL_ALPHA_SIZE,
	GPU_GL_BUFFER_SIZE,
	GPU_GL_DOUBLEBUFFER,
	GPU_GL_DEPTH_SIZE,
	GPU_GL_STENCIL_SIZE,
	GPU_GL_ACCUM_RED_SIZE,
	GPU_GL_ACCUM_GREEN_SIZE,
	GPU_GL_ACCUM_BLUE_SIZE,
	GPU_GL_ACCUM_ALPHA_SIZE,
	GPU_GL_STEREO,
	GPU_GL_MULTISAMPLEBUFFERS,
	GPU_GL_MULTISAMPLESAMPLES,
	GPU_GL_ACCELERATED_VISUAL,
	GPU_GL_RETAINED_BACKING,
	GPU_GL_CONTEXT_MAJOR_VERSION,
	GPU_GL_CONTEXT_MINOR_VERSION,
	GPU_GL_CONTEXT_EGL,
	GPU_GL_CONTEXT_FLAGS,
	GPU_GL_CONTEXT_PROFILE_MASK,
	GPU_GL_SHARE_WITH_CURRENT_CONTEXT,
	GPU_GL_FRAMEBUFFER_SRGB_CAPABLE,
	GPU_GL_DEBUG_FLAG,
	GPU_GL_CORE_PROFILE,
	GPU_GL_COMPAT_PROFILE,
	GPU_GL_ANY_PROFILE,
	GPU_GL_REST_NOTIFICATION,
	GPU_GL_LOSE_CONTEXT_ONREST
};

enum CompareFunction
{
	GPU_NONE,
	GPU_LEQUAL,
	GPU_EQUAL,
	GPU_GREATER,
	GPU_LESS,
	GPU_GEQUAL,
	GPU_NOTEQUAL,
	GPU_ALWAYS,
	GPU_NEVER,
};

enum BlendFactor
{
	GPU_ZERO,
	GPU_ONE,
	GPU_DSTCOL,
	GPU_SRCCOLOR,
	GPU_SRCALPHA,
	GPU_DSTALPHA,
	GPU_ONEMINUSSRCCOL,
	GPU_ONEMINUSDSTCOL,
	GPU_ONEMINUSSRCALPHA,
	GPU_ONEMINUSDSTALPHA,
	GPU_CONSTANT_COLOR,
	GPU_CONSTANT_ALPHA,
	GPU_ONE_MINUS_CONSTANT_COLOR,
	GPU_ONE_MINUS_CONSTANT_ALPHA,
	GPU_SRC_ALPHA_SATURATE
};

enum BlendOperation
{
	GPU_ADD,
	GPU_SUBSTRACT,
	GPU_REVSUBTRACT,
};

enum ColorMask
{
	GPU_COLORMASK_RED = 1UL << 0,
	GPU_COLORMASK_GREEN = 1UL << 1,
	GPU_COLORMASK_BLUE = 1UL << 2,
	GPU_COLORMASK_ALPHA = 1UL << 3,
	GPU_COLORMASK_RGB = GPU_COLORMASK_RED | GPU_COLORMASK_GREEN | GPU_COLORMASK_BLUE,
	GPU_COLORMASK_RGBA = GPU_COLORMASK_RED | GPU_COLORMASK_GREEN | GPU_COLORMASK_BLUE | GPU_COLORMASK_ALPHA
};

enum CullMode
{
	GPU_CULL_NONE,
	GPU_CULL_FRONT,
	GPU_CULL_BACK,
	GPU_CULL_FRONT_BACK,
};

enum FillMode
{
	PointMode,
	WireframeMode,
	SolidMode,
};

enum StencilOperation
{
	STENCILOP_KEEP,
	STENCILOP_REPLACE,
	STENCILOP_INCR,
	STENCILOP_DECR,
	STENCILOP_ZERO,
	STENCILOP_INCR_WRAP,
	STENCILOP_DECR_WRAP
};

enum ClearFlags
{
	CLEAR_NONE,
	CLEAR_COLOR = 1UL << 0,
	CLEAR_DEPTH = 1UL << 1,
	CLEAR_STENCIL = 1UL << 2,
	CLEAR_COLOR_DEPTH = CLEAR_COLOR | CLEAR_DEPTH,
	CLEAR_COLOR_STENCIL = CLEAR_COLOR | CLEAR_STENCIL,
	CLEAR_DEPTH_STENCIL = CLEAR_DEPTH | CLEAR_STENCIL,
	CLEAR_ALL = CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL
};

enum class TextureFormat
{
	STENCIL8,
	DEPTH_COMPONENT16,
	DEPTH_COMPONENT24,
	DEPTH_COMPONENT32,
	DEPTH24_STENCIL8,
	DEPTH32_STENCIL8,
	R8G8B8,
	R8G8B8A8,
	R16G16B16,
	R16G16B16A16,
	R16G16B16A16_SNORM,
	R16G16B16F,
	R32G32B32F,
	R16G16B16A16F,
	R32G32B32A32F,
	R11G11B10F,
	SRGB,
	SRGBA,
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
	Float4
};

enum class IndexType
{
	None,
	Uint16,
	Uint32
};

enum class ShaderType
{
	ST_VERTEX,
	ST_FRAGMENT,
	ST_GEOMETRY,
	ST_COMPUTE,
	ST_TESS_EVALUATION,
	ST_TESS_CONTROL,
	ST_CG_VERTEX,
	ST_CG_FRAGMENT,
};

enum class ShaderVariantType
{
	None,
	Bool,
	Int,
	Int2,
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

class VertexAttrib
{
public:
	enum
	{
		Position,
		Normal,
		Texcoord,
		Diffuse,
		Specular,
		Weight,
		Tangent,
		Bitangent,
		Nums,
	};
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