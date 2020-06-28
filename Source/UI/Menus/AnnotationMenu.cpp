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
#include "AnnotationMenu.h"
#include "ProjectNode.h"
#include "ModalDialogInput.h"
#include "Icons.h"
#include "AnnotationEvent.h"
#include "AnnotationsSequence.h"
#include "PianoTrackNode.h"
#include "ProjectTimeline.h"
#include "MidiSequence.h"
#include "CommandIDs.h"
#include "ColourIDs.h"

AnnotationMenu::AnnotationMenu(ProjectNode &parentProject, const AnnotationEvent &targetAnnotation) :
    project(parentProject),
    annotation(targetAnnotation)
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::ellipsis,
        TRANS(I18n::Menu::annotationRename))->
        closesMenu()->
        withAction([this]()
        {
            auto *sequence = static_cast<AnnotationsSequence *>(this->annotation.getSequence());
            auto inputDialog = ModalDialogInput::Presets::renameAnnotation(this->annotation.getDescription());
            inputDialog->onOk = sequence->getEventRenameCallback(this->annotation);
            App::showModalComponent(move(inputDialog));
        }));
    
    menu.add(MenuItem::item(Icons::close,
        TRANS(I18n::Menu::annotationDelete))->
        closesMenu()->
        withAction([this]()
        {
            auto *sequence = static_cast<AnnotationsSequence *>(this->annotation.getSequence());
            sequence->checkpoint();
            sequence->remove(this->annotation, true);
        }));

    this->updateContent(menu, SlideDown);
}
