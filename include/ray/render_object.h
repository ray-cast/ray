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
#ifndef _H_RENDER_OBJECT_H_
#define _H_RENDER_OBJECT_H_

#include <ray/material.h>

_NAME_BEGIN

class EXPORT RenderListener
{
public:
    virtual void onWillRenderObject() noexcept = 0;
    virtual void onRenderObject() noexcept = 0;
};

class RenderDataManager;
class EXPORT RenderObject
{
public:
    RenderObject() noexcept;
    virtual ~RenderObject() noexcept;

    virtual void setLayer(int layer) noexcept;
    virtual int getLayer() const noexcept;

    virtual void setCastShadow(bool enable) noexcept;
    virtual void setReceiveShadow(bool enable) noexcept;
    virtual void setRenderScene(RenderScenePtr scene) noexcept = 0;
    virtual void setRenderListener(RenderListener* listener) noexcept;
    virtual void setBoundingBox(const Bound& bound) noexcept;

    virtual void setTransform(const Matrix4x4& m) noexcept;
    virtual void setTransformInverse(const Matrix4x4& m) noexcept;
    virtual void setTransformInverseTranspose(const Matrix4x4& m) noexcept;

    virtual bool getReceiveShadow() const noexcept;
    virtual bool getCastShadow() const noexcept;

    virtual RenderScenePtr getRenderScene() const noexcept = 0;
    virtual RenderListener* getRenderListener() noexcept;

    virtual const Matrix4x4& getTransform() const noexcept;
    virtual const Matrix4x4& getTransformInverse() const noexcept;
    virtual const Matrix4x4& getTransformInverseTranspose() const noexcept;

    virtual const Bound& getBoundingBox() const noexcept;
    virtual const Bound& getBoundingBoxInWorld() const noexcept;

    virtual void setMaterial(MaterialPtr material) noexcept;
    virtual MaterialPtr getMaterial() noexcept;

    virtual void setRenderBuffer(RenderBufferPtr geometry, RenderablePtr renderable) noexcept;
    virtual void setRenderBuffer(RenderBufferPtr geometry, VertexType type) noexcept;

    virtual RenderBufferPtr getRenderBuffer() noexcept;
    virtual RenderablePtr getRenderable() noexcept;

private:

    void updateBoundingBoxInWorld() const noexcept;

private:

    int _layer;

    bool _isCastShadow;
    bool _isReceiveShadow;

    Matrix4x4 _transform;
    Matrix4x4 _transformInverse;
    Matrix4x4 _transformInverseTranspose;

    Bound _boundingBox;

    mutable bool _isNeedUpdate;
    mutable Bound _worldBoundingxBox;

    RenderListener* _renderListener;
};

_NAME_END

#endif