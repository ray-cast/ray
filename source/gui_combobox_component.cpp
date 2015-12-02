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
#include <ray/gui_combobox_component.h>
#include <ray/gui_editbox_component.h>
#include <ray/gui_combobox.h>
#include <ray/gui_system.h>

_NAME_BEGIN

__ImplementSubClass(GuiComboBoxComponent, GuiWidgetComponent, "GuiCombo")

GuiComboBoxComponent::GuiComboBoxComponent() noexcept
{
	_comboBox = GuiSystem::instance()->createWidget<GuiComboBox>();
	_comboBox->create();

	_edit = std::make_shared<GuiEditBoxComponent>(_comboBox->getGuiEditBox());
	_editObject = std::make_shared<GameObject>();
	_editObject->addComponent(_edit);

	this->setGuiWidget(_comboBox);
}

GuiComboBoxComponent::~GuiComboBoxComponent() noexcept
{
}

void
GuiComboBoxComponent::onAttach() except
{
	assert(_editObject);
	_editObject->setParent(this->getGameObject());
}

void
GuiComboBoxComponent::onDetach() except
{
	assert(_editObject);
	_editObject->setParent(nullptr);
}

void
GuiComboBoxComponent::load(iarchive& reader) noexcept
{
	_edit->load(reader);
}

void
GuiComboBoxComponent::save(oarchive& write) noexcept
{
	_edit->save(write);
}

GameComponentPtr
GuiComboBoxComponent::clone() const except
{
	return std::make_shared<GuiComboBoxComponent>();
}

_NAME_END

#endif