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
#include <ray/occlusion_query.h>
#include <gl/glew.h>

_NAME_BEGIN

OcclusionQuery::OcclusionQuery() noexcept
{
    glGenQueries(1, (GLuint*)&_queryID);
}

OcclusionQuery::~OcclusionQuery() noexcept
{
    glDeleteQueries(1, (GLuint*)&_queryID);
}

bool
OcclusionQuery::isAvailable() const noexcept
{
    GLint available = GL_FALSE;
    ::glGetQueryObjectiv(_queryID, GL_QUERY_RESULT_AVAILABLE, &available);
    return available ? true : false;
}

int
OcclusionQuery::getQueryResult() const noexcept
{
    GLint sampleCount = 1;
    ::glGetQueryObjectiv(_queryID, GL_QUERY_RESULT, &sampleCount);
    return sampleCount;
}

int
OcclusionQuery::getQueryInstnace() const noexcept
{
    return _queryID;
}

void
OcclusionQuery::queryBegin() const noexcept
{
    ::glBeginQuery(GL_SAMPLES_PASSED, _queryID);
}

void
OcclusionQuery::queryEnd() const noexcept
{
    ::glEndQuery(GL_SAMPLES_PASSED_ARB);
}

void 
OcclusionQuery::addObject(OcclusionObjectPtr object) noexcept
{
    _objects.push_back(object);
}

OcclusionObjectPtr
OcclusionQuery::getObject(std::size_t i) const noexcept
{
    return _objects[i];
}

OcclusionObjectContainer& 
OcclusionQuery::getObjects() noexcept
{
    return _objects;
}

const OcclusionObjectContainer&
OcclusionQuery::getObjects() const noexcept
{
    return _objects;
}

std::size_t
OcclusionQuery::getObjectCount() const noexcept
{
    return _objects.size();
}

void 
OcclusionQuery::reset() noexcept
{
    _objects.clear();
}

OcclusionQueryPool::OcclusionQueryPool() noexcept
{
}

OcclusionQueryPool::~OcclusionQueryPool() noexcept
{
    for (auto& it : _occlusionQueries)
    {
        delete it;
    }
}

OcclusionQuery* 
OcclusionQueryPool::requestQuery() noexcept
{
    OcclusionQuery* query = nullptr;
    if (_current == _occlusionQueries.size())
    {
        query = new OcclusionQuery;
        _occlusionQueries.push_back(query);
    }
    else
    {
        query = _occlusionQueries[_current];
        query->reset();
    }

    _current++;

    return query;
}

_NAME_END