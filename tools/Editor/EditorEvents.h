// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2017-2018.
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
#ifndef _H_EDITOR_EVENTS_H_
#define _H_EDITOR_EVENTS_H_

#include "UIParams.h"
#include "EditorAssetItems.h"

#include <ray/material.h>
#include <ray/game_object.h>

struct EditorEvents
{
	std::function<bool()> onUVMapperCancel;
	std::function<bool(const GuiParams&, ray::util::string::pointer&)> onUVMapperWillStart;
	std::function<bool(const GuiParams&, float&, ray::util::string::pointer&)> onUVMapperProcess;

	std::function<bool()> onLightMassCancel;
	std::function<bool(const GuiParams&, ray::util::string::pointer&)> onLightMassWillStart;
	std::function<bool(const GuiParams&, float&, ray::util::string::pointer&)> onLightMassProcess;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onLightMassSave;

	std::function<bool(const ray::GameObjects*&)> onFetchMeshes;
	std::function<bool(const ray::GameObjects*&)> onFetchCamera;
	std::function<bool(const ray::GameObjects*&)> onFetchLights;
	std::function<bool(const ray::GameObjects*&)> onFetchLightProbes;
	std::function<bool(const EditorAssetItems*&)> onFetchMaterials;
	std::function<bool(const EditorAssetItems*&)> onFetchTextures;

	std::function<bool(const ray::GameObject*)> onSeletedCamera;
	std::function<bool(const ray::GameObject*)> onSeletedLight;
	std::function<bool(const ray::GameObject*)> onSeletedLightProbe;
	std::function<bool(const ray::GameObject*, std::size_t)> onSeletedMesh;

	std::function<bool(const ray::GameObject*, std::size_t subset)> onTransformObject;

	std::function<void(const ray::float4&, bool)> onMouseHoveringCamera;

	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onProjectOpen;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onProjectSave;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onProjectSaveAs;

	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onImportModel;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onImportIES;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onImportTexture;
	std::function<bool(ray::util::string::const_pointer, ray::util::string::pointer&)> onImportMaterial;

	std::function<bool(ray::util::string::const_pointer, std::size_t, ray::util::string::pointer&)> onExportModel;
	std::function<bool(ray::util::string::const_pointer, std::size_t, ray::util::string::pointer&)> onExportIES;
	std::function<bool(ray::util::string::const_pointer, std::size_t, ray::util::string::pointer&)> onExportTexture;
	std::function<bool(ray::util::string::const_pointer, std::size_t, ray::util::string::pointer&)> onExportMaterial;
};

#endif