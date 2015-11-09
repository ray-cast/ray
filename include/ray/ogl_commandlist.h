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
#ifndef _H_OGL_COMMANDLIST_H_
#define _H_OGL_COMMANDLIST_H_

#ifdef _BUILD_OPENGL

#include <ray/ogl_canvas.h>

_NAME_BEGIN

struct TerminateSequenceCommandNV
{
	GLuint  header;
};

struct NOPCommandNV
{
	GLuint  header;
};

struct DrawElementsCommandNV
{
	GLuint  header;
	GLuint  count;
	GLuint  firstIndex;
	GLuint  baseVertex;
};

struct DrawArraysCommandNV
{
	GLuint  header;
	GLuint  count;
	GLuint  first;
};

struct DrawElementsInstancedCommandNV
{
	GLuint  header;
	GLenum  mode;
	GLuint  count;
	GLuint  instanceCount;
	GLuint  firstIndex;
	GLuint  baseVertex;
	GLuint  baseInstance;
};

struct DrawArraysInstancedCommandNV
{
	GLuint  header;
	GLenum  mode;
	GLuint  count;
	GLuint  instanceCount;
	GLuint  first;
	GLuint  baseInstance;
};

struct ElementAddressCommandNV
{
	GLuint  header;
	GLuint  addressLo;
	GLuint  addressHi;
	GLuint  typeSizeInByte;
};

struct AttributeAddressCommandNV
{
	GLuint  header;
	GLuint  index;
	GLuint  addressLo;
	GLuint  addressHi;
};

struct UniformAddressCommandNV
{
	GLuint    header;
	GLushort  index;
	GLushort  stage;
	GLuint    addressLo;
	GLuint    addressHi;
};

struct BlendEnableCommand
{
	GLuint header;
	bool   enable;
};

struct BlendColorCommandNV
{
	GLuint  header;
	float   red;
	float   green;
	float   blue;
	float   alpha;
};

struct StencilRefCommandNV
{
	GLuint  header;
	GLuint  frontStencilRef;
	GLuint  backStencilRef;
};

struct LineWidthCommandNV
{
	GLuint  header;
	float   lineWidth;
};

struct PolygonOffsetCommandNV
{
	GLuint  header;
	float   scale;
	float   bias;
};

struct AlphaRefCommandNV
{
	GLuint  header;
	float   alphaRef;
};

struct ViewportCommandNV
{
	GLuint  header;
	GLuint  x;
	GLuint  y;
	GLuint  width;
	GLuint  height;
};

struct ScissorCommandNV
{
	GLuint  header;
	GLuint  x;
	GLuint  y;
	GLuint  width;
	GLuint  height;
};

struct FrontFaceCommandNV
{
	GLuint  header;
	GLuint  frontFace;  // 0 for CW, 1 for CCW
};

struct CommandSequence
{
	std::vector<GLintptr>  offsets;
	std::vector<GLsizei>   sizes;
	std::vector<GLuint>    states;
	std::vector<GLuint>    fbos;
};

enum NVTokenShaderStage
{
	NVTOKEN_STAGE_VERTEX,
	NVTOKEN_STAGE_TESS_CONTROL,
	NVTOKEN_STAGE_TESS_EVALUATION,
	NVTOKEN_STAGE_GEOMETRY,
	NVTOKEN_STAGE_FRAGMENT,
	NVTOKEN_STAGES,
};

#define GL_TERMINATE_SEQUENCE_COMMAND_NV                      0x0000
#define GL_NOP_COMMAND_NV                                     0x0001
#define GL_DRAW_ELEMENTS_COMMAND_NV                           0x0002
#define GL_DRAW_ARRAYS_COMMAND_NV                             0x0003
#define GL_DRAW_ELEMENTS_STRIP_COMMAND_NV                     0x0004
#define GL_DRAW_ARRAYS_STRIP_COMMAND_NV                       0x0005
#define GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV                 0x0006
#define GL_DRAW_ARRAYS_INSTANCED_COMMAND_NV                   0x0007
#define GL_ELEMENT_ADDRESS_COMMAND_NV                         0x0008
#define GL_ATTRIBUTE_ADDRESS_COMMAND_NV                       0x0009
#define GL_UNIFORM_ADDRESS_COMMAND_NV                         0x000a
#define GL_BLEND_COLOR_COMMAND_NV                             0x000b
#define GL_STENCIL_REF_COMMAND_NV                             0x000c
#define GL_LINE_WIDTH_COMMAND_NV                              0x000d
#define GL_POLYGON_OFFSET_COMMAND_NV                          0x000e
#define GL_ALPHA_REF_COMMAND_NV                               0x000f
#define GL_VIEWPORT_COMMAND_NV                                0x0010
#define GL_SCISSOR_COMMAND_NV                                 0x0011
#define GL_FRONT_FACE_COMMAND_NV                              0x0012
#define GL_BLEND_ENABLE_COMMAND                               0x0013
#define GL_MAX_COMMANDS_NV                                    0x0014

typedef void (GLEXT_APIENTRY *PFNGLCREATESTATESNVPROC)(GLsizei n, GLuint *states);
typedef void (GLEXT_APIENTRY *PFNGLDELETESTATESNVPROC)(GLsizei n, const GLuint *states);
typedef GLboolean(*PFNGLISSTATENVPROC)(GLuint state);
typedef void (GLEXT_APIENTRY *PFNGLSTATEPROPERTIESDYNAMICNVPROC)(GLuint state, GLsizei count, const GLint *params);
typedef void (GLEXT_APIENTRY *PFNGLSTATECAPTURENVPROC)(GLuint state, GLenum mode);
typedef void (GLEXT_APIENTRY *PFNGLDRAWCOMMANDSNVPROC)(GLenum mode, GLenum type, GLuint buffer, const GLintptr* indirects, const GLsizei* sizes, GLuint count);
typedef void (GLEXT_APIENTRY *PFNGLDRAWCOMMANDSADDRESSNVPROC)(GLenum mode, GLenum type, const GLuint64* indirects, const GLsizei* sizes, GLuint count);
typedef void (GLEXT_APIENTRY *PFNGLDRAWCOMMANDSSTATESNVPROC)(GLuint buffer, const GLintptr* indirects, const GLsizei* sizes, const GLuint* states, const GLuint* fbos, GLuint count);
typedef void (GLEXT_APIENTRY *PFNGLDRAWCOMMANDSSTATESADDRESSNVPROC)(const GLuint64* indirects, const GLsizei* sizes, const GLuint* states, const GLuint* fbos, GLuint count);
typedef void (GLEXT_APIENTRY *PFNGLCREATECOMMANDLISTSNVPROC)(GLsizei n, GLuint *lists);
typedef void (GLEXT_APIENTRY *PFNGLDELETECOMMANDLISTSNVPROC)(GLsizei n, const GLuint *lists);
typedef GLboolean(GLEXT_APIENTRY *PFNGLISCOMMANDLISTNVPROC)(GLuint list);
typedef void (GLEXT_APIENTRY *PFNGLLISTDRAWCOMMANDSSTATESCLIENTNVPROC)(GLuint list, GLuint segment, const GLvoid** indirects, const GLsizei* sizes, const GLuint* states, const GLuint* fbos, GLuint count);
typedef void (GLEXT_APIENTRY *PFNGLCOMMANDLISTSEGMENTSNVPROC)(GLuint list, GLuint segments);
typedef void (GLEXT_APIENTRY *PFNGLCOMPILECOMMANDLISTNVPROC)(GLuint list);
typedef void (GLEXT_APIENTRY *PFNGLCALLCOMMANDLISTNVPROC)(GLuint list);
typedef GLuint(GLEXT_APIENTRY *PFNGLGETCOMMANDHEADERNVPROC)(GLenum id, GLuint tokenSize);
typedef GLushort(GLEXT_APIENTRY* PFNGLGETSTAGEINDEXNVPROC)(GLenum shadertype);

extern PFNGLCREATESTATESNVPROC __glewCreateStatesNV;
extern PFNGLDELETESTATESNVPROC __glewDeleteStatesNV;
extern PFNGLISSTATENVPROC __glewIsStateNV;
extern PFNGLSTATEPROPERTIESDYNAMICNVPROC __glewStatePropertiesDynamicNV;
extern PFNGLSTATECAPTURENVPROC __glewStateCaptureNV;
extern PFNGLDRAWCOMMANDSNVPROC __glewDrawCommandsNV;
extern PFNGLDRAWCOMMANDSADDRESSNVPROC __glewDrawCommandsAddressNV;
extern PFNGLDRAWCOMMANDSSTATESNVPROC __glewDrawCommandsStatesNV;
extern PFNGLDRAWCOMMANDSSTATESADDRESSNVPROC __glewDrawCommandsStatesAddressNV;
extern PFNGLCREATECOMMANDLISTSNVPROC __glewCreateCommandListsNV;
extern PFNGLDELETECOMMANDLISTSNVPROC __glewDeleteCommandListsNV;
extern PFNGLISCOMMANDLISTNVPROC __glewIsCommandListNV;
extern PFNGLLISTDRAWCOMMANDSSTATESCLIENTNVPROC __glewListDrawCommandsStatesClientNV;
extern PFNGLCOMMANDLISTSEGMENTSNVPROC __glewCommandListSegmentsNV;
extern PFNGLCOMPILECOMMANDLISTNVPROC __glewCompileCommandListNV;
extern PFNGLCALLCOMMANDLISTNVPROC __glewCallCommandListNV;
extern PFNGLGETCOMMANDHEADERNVPROC __glewGetCommandHeaderNV;
extern PFNGLGETSTAGEINDEXNVPROC __glewGetStageIndexNV;

inline void glCreateStatesNV(GLsizei n, GLuint *states)
{
	__glewCreateStatesNV(n, states);
}

inline void glDeleteStatesNV(GLsizei n, const GLuint *states)
{
	__glewDeleteStatesNV(n, states);
}

inline GLboolean glIsStateNV(GLuint state)
{
	return __glewIsStateNV(state);
}

inline void glStatePropertiesDynamicNV(GLuint state, GLsizei count, const GLint *params)
{
	__glewStatePropertiesDynamicNV(state, count, params);
}

inline void glStateCaptureNV(GLuint state, GLenum mode)
{
	__glewStateCaptureNV(state, mode);
}

inline void glDrawCommandsNV(GLenum mode, GLenum type, GLuint buffer, const GLintptr* indirects, const GLsizei* sizes, GLuint count)
{
	__glewDrawCommandsNV(mode, type, buffer, indirects, sizes, count);
}

inline void glDrawCommandsAddressNV(GLenum mode, GLenum type, const GLuint64* indirects, const GLsizei* sizes, GLuint count)
{
	__glewDrawCommandsAddressNV(mode, type, indirects, sizes, count);
}

inline void glDrawCommandsStatesNV(GLuint buffer, const GLintptr* indirects, const GLsizei* sizes, const GLuint* states, const GLuint* fbos, GLuint count)
{
	__glewDrawCommandsStatesNV(buffer, indirects, sizes, states, fbos, count);
}

inline void glDrawCommandsStatesAddressNV(const GLuint64* indirects, const GLsizei* sizes, const GLuint* states, const GLuint* fbos, GLuint count)
{
	__glewDrawCommandsStatesAddressNV(indirects, sizes, states, fbos, count);
}

inline void glCreateCommandListsNV(GLsizei n, GLuint *lists)
{
	__glewCreateCommandListsNV(n, lists);
}

inline void glDeleteCommandListsNV(GLsizei n, const GLuint *lists)
{
	__glewDeleteCommandListsNV(n, lists);
}

inline GLboolean glIsCommandListNV(GLuint list)
{
	return __glewIsCommandListNV(list);
}

inline void glListDrawCommandsStatesClientNV(GLuint list, GLuint segment, const GLvoid** indirects, const GLsizei* sizes, const GLuint* states, const GLuint* fbos, GLuint count)
{
	__glewListDrawCommandsStatesClientNV(list, segment, indirects, sizes, states, fbos, count);
}

inline void glCommandListSegmentsNV(GLuint list, GLuint segments)
{
	__glewCommandListSegmentsNV(list, segments);
}

inline void glCompileCommandListNV(GLuint list)
{
	__glewCompileCommandListNV(list);
}

inline void glCallCommandListNV(GLuint list)
{
	__glewCallCommandListNV(list);
}

inline GLuint glGetCommandHeaderNV(GLenum tokenId, GLuint tokenSize)
{
	return __glewGetCommandHeaderNV(tokenId, tokenSize);
}

inline GLushort glGetStageIndexNV(GLenum shadertype)
{
	return __glewGetStageIndexNV(shadertype);
}

EXPORT bool initCommandListNV();

extern bool     s_nvcmdlist_bindless;
extern GLuint   s_nvcmdlist_header[GL_MAX_COMMANDS_NV];
extern GLuint   s_nvcmdlist_headerSizes[GL_MAX_COMMANDS_NV];
extern GLushort s_nvcmdlist_stages[NVTOKEN_STAGES];

template <class T>
inline size_t nvtokenEnqueue(std::string& queue, T& data)
{
	size_t offset = queue.size();
	std::string cmd = std::string((const char*)&data, sizeof(T));

	queue += cmd;

	return offset;
}

inline GLuint nvtokenHeaderSW(GLuint type, GLuint size)
{
	return type | (size << 16);
}

inline GLuint nvtokenHeaderSizeSW(GLuint header)
{
	return header >> 16;
}

inline GLenum nvtokenHeaderCommand(GLuint header)
{
	for (int i = 0; i < GL_MAX_COMMANDS_NV; i++)
	{
		if (header == s_nvcmdlist_header[i])
			return i;
	}

	assert(0 && "can't find header");
	return (GLenum)-1;
}

inline GLenum nvtokenHeaderCommandSW(GLuint header)
{
	return header & 0xFFFF;
}

_NAME_END

#endif

#endif