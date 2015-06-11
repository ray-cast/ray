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
#include <ray/mstream.h>
#include <ray/object.h>
#include <ray/math.h>
#include <ray/except.h>

_NAME_BEGIN

typedef std::shared_ptr<class RenderDevice> RenderDevicePtr;
typedef std::shared_ptr<class RenderCanvas> RenderCanvasPtr;
typedef std::shared_ptr<class RenderState> RenderStatePtr;
typedef std::shared_ptr<class RenderWindow> RenderWindowPtr;
typedef std::shared_ptr<class RenderBuffer> RenderBufferPtr;
typedef std::shared_ptr<class RenderTexture> RenderTexturePtr;
typedef std::shared_ptr<class MultiRenderTexture> MultiRenderTexturePtr;
typedef std::shared_ptr<class Renderer> RendererPtr;
typedef std::shared_ptr<class RenderSystem> RenderSystemPtr;
typedef std::shared_ptr<class VertexComponent> VertexComponentPtr;
typedef std::shared_ptr<class VertexBufferData> VertexBufferDataPtr;
typedef std::shared_ptr<class IndexBufferData> IndexBufferDataPtr;
typedef std::shared_ptr<class Texture> TexturePtr;
typedef std::shared_ptr<class Shader> ShaderPtr;
typedef std::shared_ptr<class ShaderAttribute> ShaderAttributePtr;
typedef std::shared_ptr<class ShaderUniform> ShaderUniformPtr;
typedef std::shared_ptr<class ShaderUniformBlock> ShaderUniformBlockPtr;
typedef std::shared_ptr<class ShaderSubroutine> ShaderSubroutinePtr;
typedef std::shared_ptr<class ShaderParam> ShaderParamPtr;
typedef std::shared_ptr<class ShaderProgram> ShaderProgramPtr;
typedef std::shared_ptr<class ShaderConstantBuffer> ShaderConstantBufferPtr;
typedef std::shared_ptr<class ShaderObject> ShaderObjectPtr;
typedef std::shared_ptr<class ShaderParamArg> ShaderParamArgPtr;
typedef std::shared_ptr<class RenderPostProcess> RenderPostProcessPtr;

typedef std::shared_ptr<class Camera> CameraPtr;
typedef std::shared_ptr<class Light> LightPtr;
typedef std::shared_ptr<class Renderable> RenderablePtr;
typedef std::shared_ptr<class RenderScene> RenderScenePtr;

typedef std::vector<VertexComponent> VertexComponents;
typedef std::vector<ShaderPtr> Shaders;
typedef std::vector<ShaderAttributePtr> ShaderAttributes;
typedef std::vector<ShaderUniformPtr> ShaderUniforms;
typedef std::vector<ShaderUniformBlockPtr> ShaderUniformBlocks;
typedef std::vector<ShaderSubroutinePtr> ShaderSubroutines;
typedef std::vector<ShaderParamPtr> ShaderParams;
typedef std::vector<ShaderParamArg> ShaderParamArgs;
typedef std::vector<CameraPtr> Cameras;

_NAME_END

#endif