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
#ifndef _H_OGL_STATE_H_
#define _H_OGL_STATE_H_

#include <ray/ogl_types.h>

_NAME_BEGIN

#define MAX_DRAWBUFFERS 8
#define MAX_CLIPPLANES 8
#define MAX_VIEWPORTS 16
#define MAX_VERTEXATTRIBS 16
#define MAX_VERTEXBINDINGS 16
#define MAX_COLORS 4
#define MAX_DIFFS 16

enum StateBits
{
    BLEND,
    COLOR_LOGIC_OP,
    CULL_FACE,
    DEPTH_CLAMP,
    DEPTH_TEST,
    DITHER,
    FRAMEBUFFER_SRGB,
    LINE_SMOOTH,
    MULTISAMPLE,
    POLYGON_OFFSET_FILL,
    POLYGON_OFFSET_LINE,
    POLYGON_OFFSET_POINT,
    POLYGON_SMOOTH,
    PRIMITIVE_RESTART,
    PRIMITIVE_RESTART_FIXED_INDEX,
    RASTERIZER_DISCARD,
    SAMPLE_ALPHA_TO_COVERAGE,
    SAMPLE_ALPHA_TO_ONE,
    SAMPLE_COVERAGE,
    SAMPLE_SHADING,
    SAMPLE_MASK,
    STENCIL_TEST,
    SCISSOR_TEST,
    TEXTURE_CUBE_MAP_SEAMLESS,
    PROGRAM_POINT_SIZE,
    NUM_STATEBITS,
};

enum StateBitsDepr
{
    DEPR_ALPHA_TEST,
    DEPR_LINE_STIPPLE,
    DEPR_POINT_SMOOTH,
    DEPR_POINT_SPRITE,
    DEPR_POLYGON_STIPPLE,
    NUM_STATEBITSDEPR,
};

enum Faces
{
    FACE_FRONT,
    FACE_BACK,
    MAX_FACES,
};

struct StateIncarnation
{
    GLuint  programIncarnation;
    GLuint  fboIncarnation;

    bool operator==(const StateIncarnation& other) const
    {
        return memcmp(this, &other, sizeof(StateIncarnation)) == 0;
    }

    bool operator!=(const StateIncarnation& other) const
    {
        return memcmp(this, &other, sizeof(StateIncarnation)) != 0;
    }

    StateIncarnation() noexcept
        : programIncarnation(0)
        , fboIncarnation(0)
    {
    }
};

class StateCap
{
public:
};

class ClipDistanceState : public StateCap
{
public:
    GLbitfield  enabled;

    ClipDistanceState()
    {
        enabled = 0;
    }

    void apply() const;
    void get();
};

class AlphaStateDepr : public StateCap
{
public:
    GLenum    mode;
    GLfloat   refvalue;

    AlphaStateDepr()
    {
        mode = GL_ALWAYS;
        refvalue = 1.0;
    }

    void apply() const;
    void get();
};

class StencilState : public StateCap
{
public:
    struct StencilOp
    {
        GLenum  fail;
        GLenum  zfail;
        GLenum  zpass;
    };

    struct StencilFunc
    {
        GLenum  func;
        GLuint  refvalue;
        GLuint  mask;
    };

    StencilFunc funcs[MAX_FACES];
    StencilOp   ops[MAX_FACES];

    StencilState()
    {
        for (GLuint i = 0; i < MAX_FACES; i++)
        {
            funcs[i].func = GL_ALWAYS;
            funcs[i].refvalue = 0;
            funcs[i].mask = (GLuint)~0;
        }
    }

    void apply() const;
    void get();
};

class BlendState : public StateCap
{
public:
    struct BlendMode
    {
        GLenum srcw;
        GLenum dstw;
        GLenum equ;
    };

    struct BlendStage
    {
        BlendMode rgb;
        BlendMode alpha;
    };

    GLbitfield  separateEnable; // only set this if you want per draw enable
    GLfloat     color[4];
    GLuint      useSeparate;    // if set uses per draw, otherwise first
    BlendStage  blends[MAX_DRAWBUFFERS];

    BlendState() noexcept
    {
        separateEnable = 0;
        useSeparate = GL_FALSE;
        for (GLuint i = 0; i < MAX_DRAWBUFFERS; i++)
        {
            blends[i].alpha.srcw = GL_ONE;
            blends[i].alpha.dstw = GL_ZERO;
            blends[i].alpha.equ = GL_FUNC_ADD;
            blends[i].rgb = blends[i].alpha;
        }
    }

    void apply() const;
    void get();
};

class DepthState : public StateCap
{
public:
    GLenum  func;
    // depth bounds for NV?

    DepthState()
    {
        func = GL_LESS;
    }

    void apply() const;
    void get();
};

class LogicState : public StateCap
{
public:
    GLenum  op;

    LogicState()
    {
        op = GL_COPY;
    }

    void apply() const;
    void get();
};

class RasterState : public StateCap
{
public:
    GLenum    frontFace;
    GLenum    cullFace;
    GLfloat   polyOffsetFactor;
    GLfloat   polyOffsetUnits;
    GLenum    polyMode;   // front and back, no separate support
    GLfloat   lineWidth;
    GLfloat   pointSize;
    GLfloat   pointFade;
    GLenum    pointSpriteOrigin;

    RasterState()
    {
        frontFace = GL_CCW;
        cullFace = GL_BACK;
        polyOffsetFactor = 0;
        polyOffsetUnits = 0;
        polyMode = GL_FILL;
        lineWidth = 1.0f;
        pointSize = 1.0f;
        pointFade = 1.0f;
        pointSpriteOrigin = GL_UPPER_LEFT;
    }

    void apply() const;
    void get();
};

class RasterStateDepr : public StateCap
{
public:
    GLint     lineStippleFactor;
    GLushort  lineStipplePattern;
    GLenum    shadeModel;
    // ignore polygonStipple

    RasterStateDepr()
    {
        lineStippleFactor = 1;
        lineStipplePattern = (GLushort)~0;
        shadeModel = GL_SMOOTH;
    }

    void apply() const;
    void get();
};

class PrimitiveState : public StateCap
{
public:
    GLuint    restartIndex;
    GLint     patchVertices;
    GLenum    provokingVertex;

    PrimitiveState()
    {
        restartIndex = (GLuint)~0;
        patchVertices = 3;
        provokingVertex = GL_LAST_VERTEX_CONVENTION;
    }

    void apply() const;
    void get();
};

class SampleState : public StateCap
{
public:
    GLfloat   coverage;
    GLboolean invert;
    GLuint    mask;

    SampleState() {
        coverage = 1.0;
        invert = GL_FALSE;
        mask = (GLuint)~0;
    }

    void apply() const;
    void get();
};

class ViewportState : public StateCap
{
public:
    struct Viewport
    {
        float x;
        float y;
        float width;
        float height;
    };

    GLuint        useSeparate;  // if set uses per view, otherwise first
    Viewport      viewports[MAX_VIEWPORTS];

    ViewportState()
    {
        useSeparate = GL_FALSE;
        for (GLuint i = 0; i < MAX_VIEWPORTS; i++)
        {
            viewports[i].x = 0;
            viewports[i].y = 0;
            viewports[i].width = 0;
            viewports[i].height = 0;
        }
    }

    void apply() const;
    void get();
};

class DepthRangeState : public StateCap
{
public:
    struct DepthRange
    {
        double  nearPlane;
        double  farPlane;
    };

    GLuint        useSeparate;  // if set uses per view, otherwise first
    DepthRange    depths[MAX_VIEWPORTS];

    DepthRangeState()
    {
        useSeparate = GL_FALSE;
        for (GLuint i = 0; i < MAX_VIEWPORTS; i++)
        {
            depths[i].nearPlane = 0;
            depths[i].farPlane = 1;
        }
    }

    void apply() const;
    void get();
};

class ScissorState : public StateCap
{
public:
    struct Scissor
    {
        GLint   x;
        GLint   y;
        GLsizei width;
        GLsizei height;
    };

    GLuint        useSeparate;    // if set uses per draw, otherwise first
    Scissor       scissor[MAX_VIEWPORTS];

    ScissorState()
    {
        useSeparate = GL_FALSE;
        for (GLuint i = 0; i < MAX_VIEWPORTS; i++)
        {
            scissor[i].x = 0;
            scissor[i].y = 0;
            scissor[i].width = 0;
            scissor[i].height = 0;
        }
    }

    void apply() const;
    void get();
};

class ScissorEnableState : public StateCap
{
public:
    GLbitfield    separateEnable; // only set this if you want per view enable

    ScissorEnableState()
    {
        separateEnable = 0;
    }

    void apply() const;
    void get();
};

class MaskState : public StateCap
{
public:
    GLuint    colormaskUseSeparate;
    GLboolean colormask[MAX_DRAWBUFFERS][MAX_COLORS];
    GLboolean depth;
    GLuint    stencil[MAX_FACES];

    MaskState()
    {
        colormaskUseSeparate = GL_FALSE;
        depth = GL_TRUE;
        stencil[FACE_FRONT] = (GLuint)~0;
        stencil[FACE_BACK] = (GLuint)~0;
        for (GLuint i = 0; i < MAX_DRAWBUFFERS; i++)
        {
            for (GLuint c = 0; c < MAX_COLORS; c++)
            {
                colormask[i][c] = GL_TRUE;
            }
        }
    }

    void apply() const;
    void get();
};

class FBOState : public StateCap
{
public:
    GLuint  fboDraw;
    GLuint  fboRead;
    GLenum  readBuffer;
    GLenum  drawBuffers[MAX_DRAWBUFFERS];
    GLuint  numBuffers;

    FBOState()
    {
        fboDraw = 0;
        fboRead = 0;
        readBuffer = GL_BACK;
        for (GLuint i = 0; i < MAX_DRAWBUFFERS; i++) {
            drawBuffers[i] = GL_NONE;
        }
        drawBuffers[0] = GL_BACK;
        numBuffers = 1;
    }

    void setFbo(GLuint fbo)
    {
        fboDraw = fbo;
        fboRead = fbo;
        readBuffer = GL_COLOR_ATTACHMENT0;
        drawBuffers[0] = GL_COLOR_ATTACHMENT0;
        numBuffers = 1;
    }

    void apply() const;
    void get();
};

class VertexEnableState : public StateCap
{
public:
    GLbitfield    enabled;

    VertexEnableState()
    {
        enabled = 0;
    }

    void apply() const;
    void get();
};

class VertexFormatState : public StateCap
{
public:
    struct VertexFormat
    {
        GLboolean normalized;

        GLuint    size;
        GLenum    type;
        GLsizei   relativeoffset;

        GLuint    binding;
    };

    struct VertexBinding
    {
        GLsizei       divisor;
        GLsizei       stride;
    };

    VertexFormat formats[MAX_VERTEXATTRIBS];
    VertexBinding bindings[MAX_VERTEXBINDINGS];

    VertexFormatState()
    {
        for (GLuint i = 0; i < MAX_VERTEXATTRIBS; i++) {
            formats[i].size = 4;
            formats[i].type = GL_FLOAT;
            formats[i].normalized = GL_FALSE;
            formats[i].relativeoffset = 0;
            formats[i].binding = i;
        }

        for (GLuint i = 0; i < MAX_VERTEXATTRIBS; i++) {
            bindings[i].divisor = 0;
            bindings[i].stride = 0;
        }
    }

    void apply() const;
    void get();
};

class VertexImmediateState : public StateCap
{
public:
    struct VertexData
    {
        union
        {
            float         floats[4];
            int           ints[4];
            unsigned int  uints[4];
        };
    };

    VertexData  data[MAX_VERTEXATTRIBS];

    VertexImmediateState()
    {
        for (GLuint i = 0; i < MAX_VERTEXATTRIBS; i++)
        {
            data[i].floats[0] = 0;
            data[i].floats[1] = 0;
            data[i].floats[2] = 0;
            data[i].floats[3] = 1;
        }
    }

    void apply() const;
    void get(); // ensure proper mode, otherwise will get garbage
};

class ProgramState : public StateCap
{
public:
    // for sake of simplicity this mechanism only support programs
    // and not program pipelines, nor use of subroutines
    GLuint    program;

    ProgramState()
    {
        program = 0;
    }

    void apply() const;
    void get();
};

class EnableState : public StateCap
{
public:
    GLbitfield      stateBits;

    EnableState()
    {
        stateBits = 0;
    }

    void apply() const;
    void get();
};

class EnableStateDepr : public StateCap
{
public:
    GLbitfield      stateBitsDepr;

    EnableStateDepr()
    {
        stateBitsDepr = 0;
    }

    void apply() const;
    void get();
};

class State
{
public:
    EnableState           enable;
    EnableStateDepr       enableDepr;
    ProgramState          program;
    ClipDistanceState     clip;
    AlphaStateDepr        alpha;
    BlendState            blend;
    DepthState            depth;
    StencilState          stencil;
    LogicState            logic;
    PrimitiveState        primitive;
    SampleState           sample;
    RasterState           raster;
    RasterStateDepr       rasterDepr;
    ViewportState         viewport;
    DepthRangeState       depthrange;
    ScissorState          scissor;
    ScissorEnableState    scissorenable;
    MaskState             mask;
    FBOState              fbo;
    VertexEnableState     vertexenable;
    VertexFormatState     vertexformat;
    VertexImmediateState  verteximm;

    // This value only exists to ease compatibility with NV_command_list
    // and is unaffected by apply or get operations, its value
    // is set during StateSystem::set
    GLenum                basePrimitiveMode;

    State()
        : basePrimitiveMode(GL_TRIANGLES)
    {
    }

    void apply() const;
    void get();
};

class StateSystem
{
public:
    typedef GLuint StateID;
    static const StateID  INVALID_ID = (StateID)0;

    struct StateDiffKey
    {
        StateID   state;
        GLuint    incarnation;
    };

    struct StateDiff
    {
        enum ContentBits
        {
            ENABLE,
            ENABLE_DEPR,
            PROGRAM,
            CLIP,
            ALPHA_DEPR,
            BLEND,
            DEPTH,
            STENCIL,
            LOGIC,
            PRIMITIVE,
            RASTER,
            RASTER_DEPR,
            VIEWPORT,
            DEPTHRANGE,
            SCISSOR,
            SCISSORENABLE,
            MASK,
            FBO,
            VERTEXENABLE,
            VERTEXFORMAT,
            VERTEXIMMEDIATE,
        };

        GLbitfield    changedContentBits;
        GLbitfield    changedStateBits;
        GLbitfield    changedStateDeprBits;
        GLbitfield    changedVertexEnable;
        GLbitfield    changedVertexImm;
        GLbitfield    changedVertexFormat;
        GLbitfield    changedVertexBinding;
        GLuint        pad;
    };

    struct StateInternal
    {
        State       state;
        GLuint      incarnation;

        int           usedDiff;
        StateDiffKey  others[MAX_DIFFS];
        StateDiff     diffs[MAX_DIFFS];

        StateInternal()
        {
            incarnation = 0;
        }
    };

public:

    void init(bool coreonly = false);
    void deinit();

    void generate(GLuint num, StateID* objects);
    void destroy(GLuint num, const StateID* objects);
    void set(StateID id, const State& state, GLenum basePrimitiveMode);
    const State& get(StateID id) const;

    void apply(StateID id); // brute force sets everything
    void apply(StateID id, StateID prev); // tries to avoid redundant, can pass INVALID_ID as previous

    void prepareTransition(StateID id, StateID prev); // can speed up state apply

private:
    void makeDiff(StateDiff& diff, const StateInternal &fromInternal, const StateInternal &toInternal);
    void applyDiff(const StateDiff& diff, const State &to);
    int prepareTransitionCache(StateID prev, StateInternal& to);

private:

    bool                          _coreonly;
    std::vector<StateID>          _freeIDs;
    std::vector<StateInternal>    _states;
};

_NAME_END

#endif