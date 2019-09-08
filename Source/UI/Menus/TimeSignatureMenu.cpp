/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "TimeSignatureMenu.h"
#include "ProjectNode.h"
#include "MainLayout.h"
#include "ModalDialogInput.h"
#include "Icons.h"
#include "TimeSignatureEvent.h"
#include "TimeSignaturesSequence.h"
#include "PianoTrackNode.h"
#include "ProjectTimeline.h"
#include "MidiSequence.h"
#include "CommandIDs.h"

TimeSignatureMenu::TimeSignatureMenu(ProjectNode &parentProject, 
    const TimeSignatureEvent &targetEvent) :
    project(parentProject),
    event(targetEvent)
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::ellipsis,
        TRANS(I18n::Menu::timeSignatureChange))->
        closesMenu()->
        withAction([this]()
        {
            auto *sequence = static_cast<TimeSignaturesSequence *>(this->event.getSequence());
            auto inputDialog = ModalDialogInput::Presets::changeTimeSignature(this->event.toString());
            inputDialog->onOk = sequence->getEventChangeCallback(this->event);
            App::Layout().showModalDialog(std::move(inputDialog));
        }));

    menu.add(MenuItem::item(Icons::close,
        TRANS(I18n::Menu::timeSignatureDelete))->
        closesMenu()->
        withAction([this]()
        {
            auto *autoSequence = static_cast<TimeSignaturesSequence *>(this->event.getSequence());
            autoSequence->checkpoint();
            autoSequence->remove(this->event, true);
        }));

    this->updateContent(menu, SlideDown);
}
