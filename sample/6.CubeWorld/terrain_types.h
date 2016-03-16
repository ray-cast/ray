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
#ifndef _H_TERRAIN_TYPES_H_
#define _H_TERRAIN_TYPES_H_

#include <ray/game_application.h>
#include <ray/game_server.h>
#include <ray/game_object.h>
#include <ray/camera_component.h>
#include <ray/mesh_component.h>
#include <ray/input_feature.h>
#include <ray/render_feature.h>
#include <ray/physics_features.h>
#include <ray/queue.h>
#include <ray/format.h>
#include <ray/thread.h>

typedef std::shared_ptr<class TerrainComponent> TerrainPtr;
typedef std::shared_ptr<class TerrainMap> TerrainMapPtr;
typedef std::shared_ptr<class TerrainChunk> TerrainChunkPtr;
typedef std::shared_ptr<class TerrainItem> TerrainItemPtr;
typedef std::shared_ptr<class TerrainObject> TerrainObjectPtr;

typedef std::vector<TerrainItemPtr> TerrainItems;
typedef std::vector<TerrainObjectPtr> TerrainObjects;
typedef std::vector<class TerrainData> TerrainDatas;

typedef std::int8_t BlockPosition;
typedef std::int16_t InstanceID;

#endif