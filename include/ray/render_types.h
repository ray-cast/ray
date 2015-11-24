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
#ifndef _H_RENDER_TYPES_H_
#define _H_RENDER_TYPES_H_

#include <ray/memory.h>
#include <ray/math.h>
#include <ray/except.h>
#include <ray/variant.h>
#include <ray/model.h>

_NAME_BEGIN

typedef std::shared_ptr<class VertexComponent> VertexComponentPtr;
typedef std::shared_ptr<class VertexBufferData> VertexBufferDataPtr;
typedef std::shared_ptr<class IndexBufferData> IndexBufferDataPtr;

typedef std::shared_ptr<class Texture> TexturePtr;
typedef std::shared_ptr<class TextureSample> TextureSamplePtr;
typedef std::shared_ptr<class MultiRenderTexture> MultiRenderTexturePtr;

typedef std::shared_ptr<class Shader> ShaderPtr;
typedef std::shared_ptr<class ShaderVariant> ShaderVariantPtr;
typedef std::shared_ptr<class ShaderParameter> ShaderParameterPtr;
typedef std::shared_ptr<class ShaderAttribute> ShaderAttributePtr;
typedef std::shared_ptr<class ShaderUniform> ShaderUniformPtr;
typedef std::shared_ptr<class ShaderSubroutine> ShaderSubroutinePtr;
typedef std::shared_ptr<class ShaderObject> ShaderObjectPtr;

typedef std::shared_ptr<class RenderObject> RenderObjectPtr;
typedef std::shared_ptr<class RenderPostProcess> RenderPostProcessPtr;
typedef std::shared_ptr<class RenderMesh> RenderMeshPtr;
typedef std::shared_ptr<class RenderDevice> RenderDevicePtr;
typedef std::shared_ptr<class RenderWindow> RenderWindowPtr;
typedef std::shared_ptr<class RenderPipeline> RenderPipelinePtr;
typedef std::shared_ptr<class RenderSystem> RenderSystemPtr;
typedef std::shared_ptr<class RenderState> RenderStatePtr;
typedef std::shared_ptr<class RenderBuffer> RenderBufferPtr;
typedef std::shared_ptr<class RenderTexture> RenderTexturePtr;
typedef std::shared_ptr<class RenderIndirect> RenderIndirectPtr;
typedef std::shared_ptr<class RenderScene> RenderScenePtr;

typedef std::shared_ptr<class Material> MaterialPtr;
typedef std::shared_ptr<class MaterialPass> MaterialPassPtr;

typedef std::shared_ptr<class Camera> CameraPtr;
typedef std::shared_ptr<class Light> LightPtr;

typedef std::weak_ptr<class Camera> CameraWeakPtr;
typedef std::weak_ptr<class Light> LightWeakPtr;
typedef std::weak_ptr<class RenderScene> RenderSceneWeakPtr;

typedef std::vector<VertexComponent> VertexComponents;
typedef std::vector<ShaderPtr> Shaders;
typedef std::vector<ShaderAttributePtr> ShaderAttributes;
typedef std::vector<ShaderUniformPtr> ShaderUniforms;
typedef std::vector<ShaderSubroutinePtr> ShaderSubroutines;
typedef std::vector<ShaderVariantPtr> ShaderVariants;

typedef std::vector<RenderTexturePtr> RenderTextures;
typedef std::vector<RenderBufferPtr> RenderBuffers;
typedef std::vector<RenderMeshPtr> RenderMeshes;
typedef std::vector<RenderObjectPtr> RenderObjects;
typedef std::vector<RenderScenePtr> RenderScenes;
typedef std::vector<RenderIndirectPtr> RenderIndirects;

typedef std::vector<CameraPtr> Cameras;
typedef std::vector<LightPtr> Lights;

typedef void* WindHandle;

enum
{
	RENDERDEVICE_D3D,
	RENDERDEVICE_OPENGL,
	RENDERDEVICE_OPENGL_ES
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
	GPU_POINT_MODE,
	GPU_WIREFRAME_MODE,
	GPU_SOLID_MODE,
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

enum Anisotropy
{
	ANISOTROPY_0,
	ANISOTROPY_1,
	ANISOTROPY_2,
	ANISOTROPY_4,
	ANISOTROPY_8,
	ANISOTROPY_16,
};

enum TextureOp
{
	OP_MULTIPLY,    //* T = T1 * T2
	OP_ADD,         //* T = T1 + T2
	OP_SUBTRACT,    //* T = T1 - T2
	OP_DIVIDE,      //* T = T1 / T2
	OP_SMOOTHADD,   //* T = (T1 + T2) - (T1 * T2)
	OP_SIGNEDADD,   //* T = T1 + (T2-0.5)
};

enum PixelFormat
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

enum TextureWrap
{
	REPEAT,
	CLAMP_TO_EDGE,
	MODE_MIRROR,
};

enum TextureFilter
{
	GPU_NEAREST,
	GPU_LINEAR,
	GPU_NEAREST_MIPMAP_LINEAR,
	GPU_NEAREST_MIPMAP_NEAREST,
	GPU_LINEAR_MIPMAP_NEAREST,
	GPU_LINEAR_MIPMAP_LINEAR,
};

enum TextureDim
{
	DIM_2D,
	DIM_3D,
	DIM_2D_ARRAY,
	DIM_3D_ARRAY,
	DIM_CUBE,
	DIM_CUBE_ARRAY,
};

enum VertexType
{
	GPU_POINT,
	GPU_LINE,
	GPU_TRIANGLE,
	GPU_FAN,
	GPU_POINT_OR_LINE,
	GPU_TRIANGLE_OR_LINE,
	GPU_FAN_OR_LINE,
};

enum VertexUsage
{
	GPU_DYNAMIC_STORAGE_BIT = 1 << 0,
	GPU_MAP_READ_BIT		= 1 << 1,
	GPU_MAP_WRITE_BIT		= 1 << 2,
	GPU_MAP_PERSISTENT_BIT	= 1 << 3,
	GPU_MAP_COHERENT_BIT	= 1 << 4,
	GPU_CLIENT_STORAGE_BIT	= 1 << 5,
	GPU_IMMUTABLE_STORAGE   = 1 << 6
};

enum VertexAttrib
{
	GPU_ATTRIB_POSITION,
	GPU_ATTRIB_NORMAL,
	GPU_ATTRIB_TEXCOORD,
	GPU_ATTRIB_DIFFUSE,
	GPU_ATTRIB_SPECULAR,
	GPU_ATTRIB_WEIGHT,
	GPU_ATTRIB_TANGENT,
	GPU_ATTRIB_BITANGENT,
	GPU_ATTRIB_NUMS,
};

enum VertexFormat
{
	GPU_VERTEX_BYTE,
	GPU_VERTEX_BYTE2,
	GPU_VERTEX_BYTE3,
	GPU_VERTEX_BYTE4,
	GPU_VERTEX_SHORT,
	GPU_VERTEX_SHORT2,
	GPU_VERTEX_SHORT3,
	GPU_VERTEX_SHORT4,
	GPU_VERTEX_INT,
	GPU_VERTEX_INT2,
	GPU_VERTEX_INT3,
	GPU_VERTEX_INT4,
	GPU_VERTEX_UNSIGNED_BYTE,
	GPU_VERTEX_UNSIGNED_BYTE2,
	GPU_VERTEX_UNSIGNED_BYTE3,
	GPU_VERTEX_UNSIGNED_BYTE4,
	GPU_VERTEX_UNSIGNED_SHORT,
	GPU_VERTEX_UNSIGNED_SHORT2,
	GPU_VERTEX_UNSIGNED_SHORT3,
	GPU_VERTEX_UNSIGNED_SHORT4,
	GPU_VERTEX_UNSIGNED_INT,
	GPU_VERTEX_UNSIGNED_INT2,
	GPU_VERTEX_UNSIGNED_INT3,
	GPU_VERTEX_UNSIGNED_INT4,
	GPU_VERTEX_FLOAT,
	GPU_VERTEX_FLOAT2,
	GPU_VERTEX_FLOAT3,
	GPU_VERTEX_FLOAT4,
};

enum IndexType
{
	GPU_UINT16,
	GPU_UINT32
};

enum ShaderType
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

enum ShaderVariantType
{
	SPT_NONE,
	SPT_BOOL,
	SPT_INT,
	SPT_INT2,
	SPT_FLOAT,
	SPT_FLOAT2,
	SPT_FLOAT3,
	SPT_FLOAT4,
	SPT_FLOAT3X3,
	SPT_FLOAT4X4,
	SPT_FLOAT_ARRAY,
	SPT_FLOAT2_ARRAY,
	SPT_FLOAT3_ARRAY,
	SPT_FLOAT4_ARRAY,
	SPT_TEXTURE,
	SPT_BUFFER,
};

_NAME_END

#endif