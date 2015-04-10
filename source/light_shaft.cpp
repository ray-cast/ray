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
#include <ray/light_shaft.h>
#include <ray/render_target.h>

#include <ray/camera.h>
#include <ray/geometry.h>
#include <ray/transform.h>
#include <ray/gameobj.h>

_NAME_BEGIN

LightShaft::LightShaft(RenderDevice& pipeline)
: RenderStage(pipeline)
{
    /*_framebuffers = std::make_shared<RenderTarget>();

    _effect = std::make_shared<RenderDevice>("light_shaft.glsl");

    _light_shaft = _effect->getTechnique("light_shaft");

    _position = _effect->getParameter("position");
    _scene_texture = _effect->getParameter("scene_texture");

    _scene_texture->assign(0);*/
}

LightShaft::~LightShaft() noexcept
{
}

void
LightShaft::render(const Scene& scene) noexcept
{
    /*
    auto lights = engine.getLightsObjects();
    if (lights.size() > 0)
    {
        std::vector<Vector2> visiable;

        auto camera = engine.getCamera();
        if (camera)
        {
            for (auto& it : lights)
            {
                auto& transform = it->getComponent<Transform>();
                if (transform)
                {
                    Vector3 v = camera->project(transform->getTranslate());
                    Vector2 project(v.x, v.y);

                    const Viewport& view = camera->getViewport();

                    project.x /= view.width;
                    project.y /= view.height;

                    if (project.x >= 0 && project.x <= 1.0 &&
                        project.y >= 0 && project.y <= 1.0)
                    {
                        visiable.push_back(project);
                    }
                }
            }

            if (visiable.size() > 0)
            {
                engine.setRenderTarget(nullptr);

                engine.setViewport(0, 0, 1366, 768);

                engine.getShadingTexture()->apply(0);

                for (auto& it : visiable)
                {
                    _position->assign(it);

                    engine.renderObject(*_light_shaft, *engine.plane());
                }

                return true;
            }
        }
    }
    */
}

_NAME_END