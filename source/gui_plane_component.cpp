// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
// +----------------------------------------------------------------------
// | * Redistribution and use of _renderThread software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, _renderThread list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, _renderThread list of conditions and the
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
#if defined(_BUILD_GUI)
#include <ray/gui_plane_component.h>
#include <ray/gui_layout_component.h>
#include <ray/game_server.h>

#include <ray/render_features.h>
#include <ray/render_factory.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

__ImplementSubClass(GUIPlaneComponent, GUIBehaviourComponent, "GUIPlane")

GUIPlaneComponent::GUIPlaneComponent() noexcept
{
}

GUIPlaneComponent::~GUIPlaneComponent() noexcept
{
}

GameComponentPtr
GUIPlaneComponent::clone() const except
{
	return std::make_shared<GUIPlaneComponent>();
}

void 
GUIPlaneComponent::needUpdate(bool update) noexcept
{
}

bool 
GUIPlaneComponent::needUpdate() const noexcept
{
	bool needUpdate = false;

	auto& children = this->getGameObject()->getChildren();
	for (auto& child : children)
	{
		auto controls = child->getComponents();
		for (auto& it : controls)
		{
			if (it->isA<GUIBehaviourComponent>())
			{
				auto control = std::dynamic_pointer_cast<GUIBehaviourComponent>(it);
				if (control)
				{
					needUpdate |= control->needUpdate();
					if (needUpdate)
						break;
				}
			}
		}
	}

	return needUpdate;
}

void
GUIPlaneComponent::buildUIControl(GUILayoutComponentPtr layout) noexcept
{
	layout = this->getComponent<GUILayoutComponent>();
	layout->clear();

	buildUIControl(layout, this->getGameObject());
		
	MeshProperty mesh;
	mesh.setVertexArray(layout->getVertices());
	mesh.setTexcoordArray(layout->getCoords());
	mesh.setColorArray(layout->getColors());

	_renderbuffer = RenderFactory::createRenderBuffer(mesh);
}

void 
GUIPlaneComponent::buildUIControl(GUILayoutComponentPtr layout, GameObjectPtr object) noexcept
{
	assert(object);

	if (object->getActive())
	{
		if (this->getGameObject() != object)
		{
			auto controls = object->getComponents();
			for (auto& it : controls)
			{
				if (it->getActive())
				{
					auto control = std::dynamic_pointer_cast<GUIBehaviourComponent>(it);
					if (control)
						control->buildUIControl(layout);
				}
			}
		}

		auto& children = object->getChildren();
		for (auto& child : children)
		{
			this->buildUIControl(layout, child);
		}
	}
}

GameComponentPtr
GUIPlaneComponent::hitTest(const Vector3& raycast) noexcept
{
	auto& children = this->getGameObject()->getChildren();
	for (auto& child : children)
	{
		auto control = child->getComponent<GUIBehaviourComponent>();
		if (control->hitTest(raycast))
		{
			return control;
		}
	}

	return nullptr;
}

void 
GUIPlaneComponent::onRenderPost() noexcept
{
	auto features = this->getGameServer()->getFeature<RenderFeatures>();
	if (features)
	{
		auto pipeline = features->getRenderSystem()->getRenderPipeline();
		if (pipeline)
		{
			auto layout = this->getComponent<GUILayoutComponent>();
			if (layout)
			{
				for (auto& it : layout->getRenderIndirect())
				{
					pipeline->setMaterialPass(it.first);
					pipeline->drawMesh(_renderbuffer, it.second);
				}
			}
		}
	}
}

_NAME_END
#endif