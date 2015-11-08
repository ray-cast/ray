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

#include <ray/ogl_types.h>

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