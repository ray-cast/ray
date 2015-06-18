// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/geometry.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

Geometry::Geometry() noexcept
{
}

Geometry::~Geometry() noexcept
{
}

void
Geometry::setMaterial(MaterialPtr material) noexcept
{
    _material = material;
}

MaterialPtr
Geometry::getMaterial() noexcept
{
    return _material;
}

void
Geometry::collection(RenderDataManager& manager) noexcept
{
    if (_material)
    {
        auto& techiniques = _material->getTechs();
        for (auto& it : techiniques)
        {
            if (it)
            {
                manager.addRenderData(it->getRenderQueue(), this);
            }
        }
    }

    if (this->getCastShadow())
    {
        manager.addRenderData(RenderQueue::Shadow, this);
    }
}

void
Geometry::setRenderBuffer(RenderBufferPtr geometry, RenderablePtr renderable) noexcept
{
    assert(geometry);
    assert(renderable);

    _geometry = geometry;
    _renderable = renderable;
}

void
Geometry::setRenderBuffer(RenderBufferPtr geometry, VertexType type) noexcept
{
    assert(geometry);

    _geometry = geometry;

    if (!_renderable)
        _renderable = std::make_shared<Renderable>();

    _renderable->type = type;
    _renderable->startVertice = 0;
    _renderable->startIndice = 0;
    _renderable->numVertices = geometry->getNumVertices();
    _renderable->numIndices = geometry->getNumIndices();
    _renderable->numInstances = 0;
}

RenderBufferPtr
Geometry::getRenderBuffer() noexcept
{
    return _geometry;
}

RenderablePtr
Geometry::getRenderable() noexcept
{
    return _renderable;
}

void
Geometry::render(RenderPipeline& renderer, RenderQueue queue, RenderPass passType, MaterialPassPtr _pass) noexcept
{
    auto semantic = Material::getMaterialSemantic();
    semantic->setMatrixParam(GlobalMatrixSemantic::matModel, this->getTransform());
    semantic->setMatrixParam(GlobalMatrixSemantic::matModelInverse, this->getTransformInverse());
    semantic->setMatrixParam(GlobalMatrixSemantic::matModelInverseTranspose, this->getTransformInverseTranspose());

    auto pass = _pass ? _pass : _material->getTech(queue)->getPass(passType);

    if (pass)
    {
        if (!_pass)
        {
            RenderStencilState stencil = pass->getRenderState()->getStencilState();
            stencil.stencilEnable = true;
            stencil.stencilPass = StencilOperation::STENCILOP_REPLACE;
            stencil.stencilFunc = CompareFunction::GPU_ALWAYS;
            stencil.stencilRef = 1 << this->getLayer();
            stencil.stencilReadMask = 0xFFFF;

            pass->getRenderState()->setStencilState(stencil);
        }

        renderer.setRenderState(pass->getRenderState());
        renderer.setShaderObject(pass->getShaderObject());

        renderer.drawMesh(_geometry, *_renderable);
    }
}

_NAME_END