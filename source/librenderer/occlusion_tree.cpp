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
#include <ray/occlusion_tree.h>
#include <stack>

_NAME_BEGIN

OcclusionObject::OcclusionObject() noexcept
    : _isVirtualLeaf(false)
{
}

bool
OcclusionObject::isVirtualLeaf() const noexcept
{
    return _isVirtualLeaf;
}

const Bound&
OcclusionObject::getBoundingxBox() const noexcept
{
    return _boundingBox;
}

void 
OcclusionTree::addOcclusionObject(OcclusionObjectPtr object) noexcept
{
    _occlusionObjects.push_back(object);
    _boundingBox.encapsulate(object->getBoundingxBox());
}

bool
OcclusionTree::isVisiable(OcclusionObjectPtr object) noexcept
{
    return _frustum.contains(object->getBoundingxBox().sphere());
}

void 
OcclusionTree::collectObjects(OcclusionObjectPtr root, OcclusionObjectContainer& container) noexcept
{
    std::stack<OcclusionObject*> stack;

    stack.push(root);

    while (!stack.empty())
    {
        auto object = stack.top();
        stack.pop();

        container.push_back(object);

        if (!object->isVirtualLeaf())
        {
            OcclusionObjectIterator* iterator = dynamic_cast<OcclusionObjectIterator*>(object);

            stack.push(iterator->frist());
            stack.push(iterator->second());
        }
    }
}

void 
OcclusionTree::computeBounds(OcclusionObjectPtr root) noexcept
{
    OcclusionObjectContainer container;
    this->collectObjects(_root, container);

    for (auto& it : container)
    {

    }
}

void 
OcclusionTree::computeLeaves(OcclusionObjectPtr node) noexcept
{
    if (node->isLeaf())
    {
        node->_leaves = 1;
    }
    else
    {
        OcclusionObjectPtr f = dynamic_cast<OcclusionObjectIterator*>(node)->frist();
        OcclusionObjectPtr s = dynamic_cast<OcclusionObjectIterator*>(node)->second();

        computeLeaves(f);
        computeLeaves(s);

        node->_leaves = f->_leaves + s->_leaves;
    }
}

_NAME_END