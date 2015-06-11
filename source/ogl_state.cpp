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
#include <ray/ogl_state.h>

_NAME_BEGIN

GLenum s_stateEnums[StateBits::NUM_STATEBITS] =
{
    GL_BLEND,
    GL_COLOR_LOGIC_OP,
    GL_CULL_FACE,
    GL_DEPTH_CLAMP,
    GL_DEPTH_TEST,
    GL_DITHER,
    GL_FRAMEBUFFER_SRGB,
    GL_LINE_SMOOTH,
    GL_MULTISAMPLE,
    GL_POLYGON_OFFSET_FILL,
    GL_POLYGON_OFFSET_LINE,
    GL_POLYGON_OFFSET_POINT,
    GL_POLYGON_SMOOTH,
    GL_PRIMITIVE_RESTART,
    GL_PRIMITIVE_RESTART_FIXED_INDEX,
    GL_RASTERIZER_DISCARD,
    GL_SAMPLE_ALPHA_TO_COVERAGE,
    GL_SAMPLE_ALPHA_TO_ONE,
    GL_SAMPLE_COVERAGE,
    GL_SAMPLE_SHADING,
    GL_SAMPLE_MASK,
    GL_STENCIL_TEST,
    GL_SCISSOR_TEST,
    GL_TEXTURE_CUBE_MAP_SEAMLESS,
    GL_PROGRAM_POINT_SIZE,
};

GLenum s_stateEnumsDepr[StateBitsDepr::NUM_STATEBITSDEPR] =
{
    GL_ALPHA_TEST,
    GL_LINE_STIPPLE,
    GL_POINT_SMOOTH,
    GL_POINT_SPRITE,
    GL_POLYGON_STIPPLE,
};

bool isBitSet(GLbitfield bits, GLuint key)
{
    return  (bits & (1 << key)) ? true : false;
}

void setBit(GLbitfield& bits, GLuint key)
{
    bits |= (1 << key);
}

GLbitfield getBit(GLuint key)
{
    return (1 << key);
}

GLboolean setBitState(GLbitfield& bits, GLuint key, GLboolean state)
{
    if (state)  bits |= (1 << key);
    else        bits &= ~(1 << key);
    return state;
}

void
ClipDistanceState::apply() const
{
    for (GLuint i = 0; i < MAX_CLIPPLANES; i++)
    {
        if (isBitSet(enabled, i))
            glEnable(GL_CLIP_DISTANCE0 + i);
        else
            glDisable(GL_CLIP_DISTANCE0 + i);
    }
}

void
ClipDistanceState::get()
{
    enabled = 0;
    for (GLuint i = 0; i < MAX_CLIPPLANES; i++)
    {
        setBitState(enabled, i, glIsEnabled(GL_CLIP_DISTANCE0 + i));
    }
}

void
AlphaStateDepr::apply() const
{
    glAlphaFunc(mode, refvalue);
}

void
AlphaStateDepr::get()
{
    glGetIntegerv(GL_ALPHA_TEST_FUNC, (GLint*)&mode);
    glGetFloatv(GL_ALPHA_TEST_REF, &refvalue);
}

void
StencilState::apply() const
{
    glStencilFuncSeparate(GL_FRONT, funcs[FACE_FRONT].func, funcs[FACE_FRONT].refvalue, funcs[FACE_FRONT].mask);
    glStencilFuncSeparate(GL_BACK, funcs[FACE_BACK].func, funcs[FACE_BACK].refvalue, funcs[FACE_BACK].mask);
    glStencilOpSeparate(GL_FRONT, ops[FACE_FRONT].fail, ops[FACE_FRONT].zfail, ops[FACE_FRONT].zpass);
    glStencilOpSeparate(GL_BACK, ops[FACE_BACK].fail, ops[FACE_BACK].zfail, ops[FACE_BACK].zpass);
}

void
StencilState::get()
{
    glGetIntegerv(GL_STENCIL_FUNC, (GLint*)&funcs[FACE_FRONT].func);
    glGetIntegerv(GL_STENCIL_REF, (GLint*)&funcs[FACE_FRONT].refvalue);
    glGetIntegerv(GL_STENCIL_VALUE_MASK, (GLint*)&funcs[FACE_FRONT].mask);

    glGetIntegerv(GL_STENCIL_BACK_FUNC, (GLint*)&funcs[FACE_BACK].func);
    glGetIntegerv(GL_STENCIL_BACK_REF, (GLint*)&funcs[FACE_BACK].refvalue);
    glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, (GLint*)&funcs[FACE_BACK].mask);

    glGetIntegerv(GL_STENCIL_FAIL, (GLint*)&ops[FACE_FRONT].fail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint*)&ops[FACE_FRONT].zfail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint*)&ops[FACE_FRONT].zpass);

    glGetIntegerv(GL_STENCIL_BACK_FAIL, (GLint*)&ops[FACE_BACK].fail);
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, (GLint*)&ops[FACE_BACK].zfail);
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, (GLint*)&ops[FACE_BACK].zpass);
}

void
BlendState::apply() const
{
    if (separateEnable)
    {
        for (GLuint i = 0; i < MAX_DRAWBUFFERS; i++)
        {
            if (isBitSet(separateEnable, i))
                glEnablei(GL_BLEND, i);
            else
                glDisablei(GL_BLEND, i);
        }
    }

    if (useSeparate)
    {
        for (GLuint i = 0; i < MAX_DRAWBUFFERS; i++)
        {
            glBlendFuncSeparatei(i, blends[i].rgb.srcw, blends[i].rgb.dstw, blends[i].alpha.srcw, blends[i].alpha.dstw);
            glBlendEquationSeparatei(i, blends[i].rgb.equ, blends[i].alpha.equ);
        }
    }
    else
    {
        glBlendFuncSeparate(blends[0].rgb.srcw, blends[0].rgb.dstw, blends[0].alpha.srcw, blends[0].alpha.dstw);
        glBlendEquationSeparate(blends[0].rgb.equ, blends[0].alpha.equ);
    }

    glBlendColor(color[0], color[1], color[2], color[3]);
}

void
BlendState::get()
{
    GLuint stateSet = 0;
    separateEnable = 0;
    for (GLuint i = 0; i < MAX_DRAWBUFFERS; i++)
    {
        if (setBitState(separateEnable, i, glIsEnabledi(GL_BLEND, i))) stateSet++;
    }

    if (stateSet == MAX_DRAWBUFFERS)
    {
        separateEnable = 0;
    }

    GLuint numEqual = 1;
    for (GLuint i = 0; i < MAX_DRAWBUFFERS; i++)
    {
        glGetIntegeri_v(GL_BLEND_SRC_RGB, i, (GLint*)&blends[i].rgb.srcw);
        glGetIntegeri_v(GL_BLEND_DST_RGB, i, (GLint*)&blends[i].rgb.dstw);
        glGetIntegeri_v(GL_BLEND_EQUATION_RGB, i, (GLint*)&blends[i].rgb.equ);

        glGetIntegeri_v(GL_BLEND_SRC_ALPHA, i, (GLint*)&blends[i].alpha.srcw);
        glGetIntegeri_v(GL_BLEND_DST_ALPHA, i, (GLint*)&blends[i].alpha.dstw);
        glGetIntegeri_v(GL_BLEND_EQUATION_ALPHA, i, (GLint*)&blends[i].alpha.equ);

        if (i > 1 &&
            memcmp(&blends[i].rgb, &blends[i - 1].rgb, sizeof(blends[i].rgb)) == 0 &&
            memcmp(&blends[i].alpha, &blends[i - 1].alpha, sizeof(blends[i].alpha)) == 0)
        {
            numEqual++;
        }
    }

    useSeparate = numEqual != MAX_DRAWBUFFERS;

    glGetFloatv(GL_BLEND_COLOR, color);
}

void
DepthState::apply() const
{
    glDepthFunc(func);
}

void
DepthState::get()
{
    glGetIntegerv(GL_DEPTH_FUNC, (GLint*)&func);
}

void
LogicState::apply() const
{
    glLogicOp(op);
}

void
LogicState::get()
{
    glGetIntegerv(GL_LOGIC_OP_MODE, (GLint*)&op);
}

void
RasterState::apply() const
{
    glFrontFace(frontFace);
    glCullFace(cullFace);
    glPolygonOffset(polyOffsetFactor, polyOffsetUnits);
    glPolygonMode(GL_FRONT_AND_BACK, polyMode);
    glLineWidth(lineWidth);
    glPointSize(pointSize);
    glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, pointFade);
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, pointSpriteOrigin);
}

void
RasterState::get()
{
    glGetIntegerv(GL_FRONT_FACE, (GLint*)&frontFace);
    glGetIntegerv(GL_CULL_FACE_MODE, (GLint*)&cullFace);
    glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &polyOffsetFactor);
    glGetFloatv(GL_POLYGON_OFFSET_UNITS, &polyOffsetUnits);
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);
    glGetFloatv(GL_POINT_SIZE, &pointSize);
    glGetFloatv(GL_POINT_FADE_THRESHOLD_SIZE, &pointFade);
    glGetIntegerv(GL_POINT_SPRITE_COORD_ORIGIN, (GLint*)&pointSpriteOrigin);
}

void
RasterStateDepr::apply() const
{
    glLineStipple(lineStippleFactor, lineStipplePattern);
    glShadeModel(shadeModel);
}

void
RasterStateDepr::get()
{
    GLint pattern = 0;
    glGetIntegerv(GL_LINE_STIPPLE_PATTERN, &pattern);
    lineStipplePattern = pattern;

    glGetIntegerv(GL_LINE_STIPPLE_REPEAT, (GLint*)&lineStippleFactor);
    glGetIntegerv(GL_SHADE_MODEL, (GLint*)&shadeModel);
}

void
PrimitiveState::apply() const
{
    glPrimitiveRestartIndex(restartIndex);
    glProvokingVertex(provokingVertex);
    glPatchParameteri(GL_PATCH_VERTICES, patchVertices);
}

void
PrimitiveState::get()
{
    glGetIntegerv(GL_PRIMITIVE_RESTART_INDEX, (GLint*)&restartIndex);
    glGetIntegerv(GL_PROVOKING_VERTEX, (GLint*)&provokingVertex);
    glGetIntegerv(GL_PATCH_VERTICES, (GLint*)&patchVertices);
}

void
SampleState::apply() const
{
    glSampleCoverage(coverage, invert);
    glSampleMaski(0, mask);
}

void
SampleState::get()
{
    glGetIntegerv(GL_SAMPLE_COVERAGE_VALUE, (GLint*)&coverage);
    glGetIntegerv(GL_SAMPLE_COVERAGE_INVERT, (GLint*)&invert);
    glGetIntegeri_v(GL_SAMPLE_MASK_VALUE, 0, (GLint*)&mask);
}

void
ViewportState::apply() const
{
    if (useSeparate)
    {
        glViewportArrayv(0, MAX_VIEWPORTS, &viewports[0].x);
    }
    else
    {
        glViewport(GLint(viewports[0].x), GLint(viewports[0].y), GLsizei(viewports[0].width), GLsizei(viewports[0].height));
    }
}

void
ViewportState::get()
{
    int numEqual = 1;
    for (GLuint i = 0; i < MAX_VIEWPORTS; i++)
    {
        glGetFloati_v(GL_VIEWPORT, i, &viewports[i].x);
        if (i > 0 && memcmp(&viewports[i], &viewports[i - 1], sizeof(viewports[i])) == 0)
        {
            numEqual++;
        }
    }

    useSeparate = (numEqual != MAX_VIEWPORTS);
}

void
DepthRangeState::apply() const
{
    if (useSeparate)
    {
        glDepthRangeArrayv(0, MAX_VIEWPORTS, &depths[0].nearPlane);
    }
    else
    {
        glDepthRange(depths[0].nearPlane, depths[0].farPlane);
    }
}

void
DepthRangeState::get()
{
    GLuint numEqual = 1;
    for (GLuint i = 0; i < MAX_VIEWPORTS; i++)
    {
        glGetDoublei_v(GL_DEPTH_RANGE, i, &depths[i].nearPlane);
        if (i > 0 && memcmp(&depths[i], &depths[i - 1], sizeof(depths[i])) == 0)
        {
            numEqual++;
        }
    }

    useSeparate = (numEqual != MAX_VIEWPORTS);
}

void
ScissorState::apply() const
{
    if (useSeparate) {
        glScissorArrayv(0, MAX_VIEWPORTS, &scissor[0].x);
    }
    else {
        glScissor(scissor[0].x, scissor[0].y, scissor[0].width, scissor[0].height);
    }
}

void
ScissorState::get()
{
    GLuint numEqual = 1;
    for (GLuint i = 0; i < MAX_VIEWPORTS; i++) {
        glGetIntegeri_v(GL_SCISSOR_BOX, i, &scissor[i].x);
        if (i > 0 && memcmp(&scissor[i], &scissor[i - 1], sizeof(scissor[i])) == 0) {
            numEqual++;
        }
    }

    useSeparate = (numEqual != MAX_VIEWPORTS);
}

void
ScissorEnableState::apply() const
{
    if (separateEnable)
    {
        for (GLuint i = 0; i < MAX_VIEWPORTS; i++)
        {
            if (isBitSet(separateEnable, i))
                glEnablei(GL_SCISSOR_TEST, i);
            else
                glDisablei(GL_SCISSOR_TEST, i);
        }
    }
}

void
ScissorEnableState::get()
{
    GLuint stateSet = 0;
    separateEnable = 0;
    for (GLuint i = 0; i < MAX_DRAWBUFFERS; i++)
    {
        if (setBitState(separateEnable, i, glIsEnabledi(GL_BLEND, i))) stateSet++;
    }
    if (stateSet == MAX_DRAWBUFFERS)
    {
        separateEnable = 0;
    }
}

void
MaskState::apply() const
{
    if (colormaskUseSeparate) {
        for (GLuint i = 0; i < MAX_DRAWBUFFERS; i++) {
            glColorMaski(i, colormask[i][0], colormask[i][1], colormask[i][2], colormask[i][3]);
        }
    }
    else {
        glColorMask(colormask[0][0], colormask[0][1], colormask[0][2], colormask[0][3]);
    }
    glDepthMask(depth);
    glStencilMaskSeparate(GL_FRONT, stencil[FACE_FRONT]);
    glStencilMaskSeparate(GL_BACK, stencil[FACE_BACK]);
}

void
MaskState::get()
{
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depth);
    glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint*)&stencil[FACE_FRONT]);
    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, (GLint*)&stencil[FACE_BACK]);

    int numEqual = 1;
    for (GLuint i = 0; i < MAX_DRAWBUFFERS; i++)
    {
        glGetBooleani_v(GL_COLOR_WRITEMASK, i, colormask[i]);

        if (i > 0 && memcmp(colormask[i], colormask[i - 1], sizeof(colormask[i])) == 0)
        {
            numEqual++;
        }
    }

    colormaskUseSeparate = numEqual != MAX_DRAWBUFFERS;
}

void
FBOState::apply() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboDraw);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboRead);

    glDrawBuffers(numBuffers, drawBuffers);
    glReadBuffer(readBuffer);
}

void
FBOState::get()
{
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&fboDraw);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint*)&fboRead);

    glGetIntegerv(GL_READ_BUFFER, (GLint*)&readBuffer);

    for (int i = 0; i < MAX_DRAWBUFFERS; i++)
    {
        glGetIntegerv(GL_DRAW_BUFFER0 + i, (GLint*)&drawBuffers[i]);
        if (drawBuffers[i] != GL_NONE)
        {
            numBuffers = i + 1;
        }
    }
}

void
VertexEnableState::apply() const
{
    for (GLuint i = 0; i < MAX_VERTEXATTRIBS; i++)
    {
        if (isBitSet(enabled, i))
            glEnableVertexAttribArray(i);
        else
            glDisableVertexAttribArray(i);
    }
}

void
VertexEnableState::get()
{
    enabled = 0;
    for (GLuint i = 0; i < MAX_VERTEXATTRIBS; i++)
    {
        GLint status;
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, (GLint*)&status);
        setBitState(enabled, i, status);
    }
}

void
VertexFormatState::apply() const
{
    for (GLuint i = 0; i < MAX_VERTEXATTRIBS; i++)
    {
        switch (formats[i].mode)
        {
        case VertexDataType::GPU_DATATYPE_FLOAT:
            glVertexAttribFormat(i, formats[i].size, formats[i].type, formats[i].normalized, formats[i].relativeoffset);
            break;
        case VertexDataType::GPU_DATATYPE_INT:
        case VertexDataType::GPU_DATATYPE_UNSIGNED_INT:
            glVertexAttribIFormat(i, formats[i].size, formats[i].type, formats[i].relativeoffset);
            break;
        }

        glVertexAttribBinding(i, formats[i].binding);
    }

    for (GLuint i = 0; i < MAX_VERTEXBINDINGS; i++)
    {
        glVertexBindingDivisor(i, bindings[i].divisor);
        glBindVertexBuffer(i, 0, 0, bindings[i].stride);
    }
}

void
VertexFormatState::get()
{
    for (GLuint i = 0; i < MAX_VERTEXATTRIBS; i++)
    {
        GLint status = 0;
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_RELATIVE_OFFSET, (GLint*)&formats[i].relativeoffset);
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, (GLint*)&formats[i].size);
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, (GLint*)&formats[i].type);
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, (GLint*)&status);
        formats[i].normalized = status;
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_INTEGER, (GLint*)&status);
        if (status)
            formats[i].mode = VertexDataType::GPU_DATATYPE_INT;
        else
            formats[i].mode = VertexDataType::GPU_DATATYPE_FLOAT;

        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_BINDING, (GLint*)&formats[i].binding);
    }

    for (GLuint i = 0; i < MAX_VERTEXBINDINGS; i++)
    {
        glGetIntegeri_v(GL_VERTEX_BINDING_DIVISOR, i, (GLint*)&bindings[i].divisor);
        glGetIntegeri_v(GL_VERTEX_BINDING_STRIDE, i, (GLint*)&bindings[i].stride);
    }
}

void
VertexImmediateState::apply() const
{
    for (GLuint i = 0; i < MAX_VERTEXATTRIBS; i++)
    {
        switch (data[i].mode)
        {
        case VertexDataType::GPU_DATATYPE_FLOAT:
            glVertexAttrib4fv(i, data[i].floats);
            break;
        case VertexDataType::GPU_DATATYPE_INT:
            glVertexAttribI4iv(i, data[i].ints);
            break;
        case VertexDataType::GPU_DATATYPE_UNSIGNED_INT:
            glVertexAttribI4uiv(i, data[i].uints);
            break;
        }
    }
}

void
VertexImmediateState::get()
{
    for (GLuint i = 0; i < MAX_VERTEXATTRIBS; i++)
    {
        switch (data[i].mode)
        {
        case VertexDataType::GPU_DATATYPE_FLOAT:
            glGetVertexAttribfv(i, GL_CURRENT_VERTEX_ATTRIB, data[i].floats);
            break;
        case VertexDataType::GPU_DATATYPE_INT:
            glGetVertexAttribIiv(i, GL_CURRENT_VERTEX_ATTRIB, data[i].ints);
            break;
        case VertexDataType::GPU_DATATYPE_UNSIGNED_INT:
            glGetVertexAttribIuiv(i, GL_CURRENT_VERTEX_ATTRIB, data[i].uints);
            break;
        }
    }
}

void
ProgramState::apply() const
{
    glUseProgram(program);
}

void
ProgramState::get()
{
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&program);
}

void
EnableState::apply() const
{
    for (GLuint i = 0; i < NUM_STATEBITS; i++)
    {
        if (isBitSet(stateBits, i))
            glEnable(s_stateEnums[i]);
        else
            glDisable(s_stateEnums[i]);
    }
}

void
EnableState::get()
{
    for (GLuint i = 0; i < NUM_STATEBITS; i++)
    {
        setBitState(stateBits, i, glIsEnabled(s_stateEnums[i]));
    }
}

void
EnableStateDepr::apply() const
{
    for (GLuint i = 0; i < NUM_STATEBITSDEPR; i++)
    {
        if (isBitSet(stateBitsDepr, i))
            glEnable(s_stateEnumsDepr[i]);
        else
            glDisable(s_stateEnumsDepr[i]);
    }
}

void
EnableStateDepr::get()
{
    for (GLuint i = 0; i < NUM_STATEBITSDEPR; i++)
    {
        setBitState(stateBitsDepr, i, glIsEnabled(s_stateEnumsDepr[i]));
    }
}

void
State::apply() const
{
    enable.apply();
    enableDepr.apply();
    program.apply();
    clip.apply();
    alpha.apply();
    blend.apply();
    depth.apply();
    stencil.apply();
    logic.apply();
    primitive.apply();
    sample.apply();
    raster.apply();
    rasterDepr.apply();
    /*if (!isBitSet(dynamicState,DYNAMIC_VIEWPORT)){
    viewport.apply(system);
    }*/
    depthrange.apply();
    /*if (!isBitSet(dynamicState,DYNAMIC_SCISSOR)){
    scissor.apply(system);
    }*/
    scissorenable.apply();
    mask.apply();
    fbo.apply();
    vertexenable.apply();
    vertexformat.apply();
    verteximm.apply();
}

void
State::get()
{
    enable.get();
    enableDepr.get();
    program.get();
    clip.get();
    alpha.get();
    blend.get();
    depth.get();
    stencil.get();
    logic.get();
    primitive.get();
    sample.get();
    raster.get();
    rasterDepr.get();
    //viewport.get();
    depthrange.get();
    //scissor.get();
    scissorenable.get();
    mask.get();
    fbo.get();
    vertexenable.get();
    vertexformat.get();
    verteximm.get();
}

void
StateSystem::init(bool coreonly)
{
    _coreonly = coreonly;
}

void
StateSystem::deinit()
{
    _states.resize(0);
    _freeIDs.resize(0);
}

void
StateSystem::generate(GLuint num, StateID* objects)
{
    GLuint i;
    for (i = 0; i < num && !_freeIDs.empty(); i++)
    {
        objects[i] = _freeIDs.back();
        _freeIDs.pop_back();
    }

    GLuint begin = GLuint(_states.size());

    if (i < num)
    {
        _states.resize(begin + num - i);
    }

    for (i = i; i < num; i++)
    {
        objects[i] = begin + i;
    }
}

void
StateSystem::destroy(GLuint num, const StateID* objects)
{
    for (GLuint i = 0; i < num; i++)
    {
        _freeIDs.push_back(objects[i]);
    }
}

void
StateSystem::set(StateID id, const State& state, GLenum basePrimitiveMode)
{
    StateInternal& intstate = _states[id];
    intstate.incarnation++;
    intstate.state = state;
    intstate.state.basePrimitiveMode = basePrimitiveMode;
    intstate.usedDiff = 0;

    for (int i = 0; i < MAX_DIFFS; i++)
    {
        intstate.others[i].state = INVALID_ID;
    }
}

const
State& StateSystem::get(StateID id) const
{
    return _states[id].state;
}

int
StateSystem::prepareTransitionCache(StateID prev, StateInternal& to)
{
    StateInternal& from = _states[prev];

    int index = -1;

    for (int i = 0; i < MAX_DIFFS; i++)
    {
        if (to.others[i].state == prev && to.others[i].incarnation == from.incarnation)
        {
            index = i;
            break;
        }
    }

    if (index < 0)
    {
        index = to.usedDiff;
        to.usedDiff = (to.usedDiff + 1) % MAX_DIFFS;

        to.others[index].state = prev;
        to.others[index].incarnation = from.incarnation;

        makeDiff(to.diffs[index], from, to);
    }

    return index;
}

void
StateSystem::apply(StateID id)
{
    _states[id].state.apply();
}

void
StateSystem::apply(StateID id, StateID prev)
{
    StateInternal& to = _states[id];

    if (prev == INVALID_ID) {
        apply(id);
        return;
    }

    int index = prepareTransitionCache(prev, to);

    applyDiff(to.diffs[index], to.state);
}

void
StateSystem::applyDiff(const StateDiff& diff, const State &state)
{
    if (isBitSet(diff.changedContentBits, StateDiff::ENABLE))
        state.enable.apply();
    if (!_coreonly && isBitSet(diff.changedContentBits, StateDiff::ENABLE_DEPR))
        state.enableDepr.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::PROGRAM))
        state.program.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::CLIP))
        state.clip.apply();
    if (!_coreonly && isBitSet(diff.changedContentBits, StateDiff::ALPHA_DEPR))
        state.alpha.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::BLEND))
        state.blend.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::DEPTH))
        state.depth.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::STENCIL))
        state.stencil.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::LOGIC))
        state.logic.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::PRIMITIVE))
        state.primitive.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::RASTER))
        state.raster.apply();
    if (!_coreonly && isBitSet(diff.changedContentBits, StateDiff::RASTER_DEPR))
        state.rasterDepr.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::VIEWPORT))
        state.viewport.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::DEPTHRANGE))
        state.depthrange.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::SCISSOR))
        state.scissor.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::SCISSORENABLE))
        state.scissorenable.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::MASK))
        state.mask.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::FBO))
        state.fbo.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::VERTEXENABLE))
        state.vertexenable.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::VERTEXFORMAT))
        state.vertexformat.apply();
    if (isBitSet(diff.changedContentBits, StateDiff::VERTEXIMMEDIATE))
        state.verteximm.apply();
}

void
StateSystem::makeDiff(StateDiff& diff, const StateInternal &fromInternal, const StateInternal &toInternal)
{
    const State &from = fromInternal.state;
    const State &to = toInternal.state;

    diff.changedStateBits = from.enable.stateBits ^ to.enable.stateBits;
    diff.changedStateDeprBits = from.enableDepr.stateBitsDepr ^ to.enableDepr.stateBitsDepr;
    diff.changedContentBits = 0;

    if (memcmp(&from.enable, &to.enable, sizeof(from.enable)) != 0)
        setBit(diff.changedContentBits, StateDiff::ENABLE);
    if (memcmp(&from.enableDepr, &to.enableDepr, sizeof(from.enableDepr)) != 0)
        setBit(diff.changedContentBits, StateDiff::ENABLE_DEPR);
    if (memcmp(&from.program, &to.program, sizeof(from.program)) != 0)
        setBit(diff.changedContentBits, StateDiff::PROGRAM);
    if (memcmp(&from.clip, &to.clip, sizeof(from.clip)) != 0)
        setBit(diff.changedContentBits, StateDiff::CLIP);
    if (memcmp(&from.alpha, &to.alpha, sizeof(from.alpha)) != 0)
        setBit(diff.changedContentBits, StateDiff::ALPHA_DEPR);
    if (memcmp(&from.blend, &to.blend, sizeof(from.blend)) != 0)
        setBit(diff.changedContentBits, StateDiff::BLEND);
    if (memcmp(&from.depth, &to.depth, sizeof(from.depth)) != 0)
        setBit(diff.changedContentBits, StateDiff::DEPTH);
    if (memcmp(&from.stencil, &to.stencil, sizeof(from.stencil)) != 0)
        setBit(diff.changedContentBits, StateDiff::STENCIL);
    if (memcmp(&from.logic, &to.logic, sizeof(from.logic)) != 0)
        setBit(diff.changedContentBits, StateDiff::LOGIC);
    if (memcmp(&from.primitive, &to.primitive, sizeof(from.primitive)) != 0)
        setBit(diff.changedContentBits, StateDiff::PRIMITIVE);
    if (memcmp(&from.raster, &to.raster, sizeof(from.raster)) != 0)
        setBit(diff.changedContentBits, StateDiff::RASTER);
    if (memcmp(&from.rasterDepr, &to.rasterDepr, sizeof(from.rasterDepr)) != 0)
        setBit(diff.changedContentBits, StateDiff::RASTER_DEPR);
    if (memcmp(&from.viewport, &to.viewport, sizeof(from.viewport)) != 0)
        setBit(diff.changedContentBits, StateDiff::VIEWPORT);
    if (memcmp(&from.depth, &to.depth, sizeof(from.depth)) != 0)
        setBit(diff.changedContentBits, StateDiff::DEPTHRANGE);
    if (memcmp(&from.scissor, &to.scissor, sizeof(from.scissor)) != 0)
        setBit(diff.changedContentBits, StateDiff::SCISSOR);
    if (memcmp(&from.scissorenable, &to.scissorenable, sizeof(from.scissorenable)) != 0)
        setBit(diff.changedContentBits, StateDiff::SCISSORENABLE);
    if (memcmp(&from.mask, &to.mask, sizeof(from.mask)) != 0)
        setBit(diff.changedContentBits, StateDiff::MASK);
    if (memcmp(&from.fbo, &to.fbo, sizeof(from.fbo)) != 0)
        setBit(diff.changedContentBits, StateDiff::FBO);

    // special case vertex stuff, more likely to change then rest

    diff.changedVertexEnable = from.vertexenable.enabled ^ to.vertexenable.enabled;
    diff.changedVertexImm = 0;
    diff.changedVertexFormat = 0;

    for (GLint i = 0; i < MAX_VERTEXATTRIBS; i++)
    {
        if (memcmp(&from.vertexformat.formats[i], &to.vertexformat.formats[i], sizeof(to.vertexformat.formats[i])) != 0)
            setBit(diff.changedVertexFormat, i);

        if (memcmp(&from.verteximm.data[i], &to.verteximm.data[i], sizeof(to.verteximm.data[i])) != 0)
            setBit(diff.changedVertexImm, i);
    }

    diff.changedVertexBinding = 0;
    for (GLint i = 0; i < MAX_VERTEXBINDINGS; i++)
    {
        if (memcmp(&from.vertexformat.bindings[i], &to.vertexformat.bindings[i], sizeof(to.vertexformat.bindings[i])) != 0)
            setBit(diff.changedVertexBinding, i);
    }

    if (diff.changedVertexEnable)
        setBit(diff.changedContentBits, StateDiff::VERTEXENABLE);

    if (diff.changedVertexBinding || diff.changedVertexFormat)
        setBit(diff.changedContentBits, StateDiff::VERTEXFORMAT);

    if (diff.changedVertexImm)
        setBit(diff.changedContentBits, StateDiff::VERTEXIMMEDIATE);
}

void
StateSystem::prepareTransition(StateID id, StateID prev)
{
    prepareTransitionCache(prev, _states[id]);
}

_NAME_END