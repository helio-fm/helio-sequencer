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
#include "MainLayout.h"
#include "ModalDialogInput.h"
#include "Icons.h"
#include "HybridRoll.h"
#include "AnnotationEvent.h"
#include "AnnotationsSequence.h"
#include "PianoTrackNode.h"
#include "ProjectTimeline.h"
#include "MidiSequence.h"
#include "CommandIDs.h"

AnnotationMenu::AnnotationMenu(ProjectNode &parentProject, const AnnotationEvent &targetAnnotation) :
    project(parentProject),
    annotation(targetAnnotation)
{
    MenuPanel::Menu cmds;
    cmds.add(MenuItem::item(Icons::ellipsis, CommandIDs::RenameAnnotation, TRANS("menu::annotation::rename")));
    
    const StringPairArray colours(MenuPanel::getColoursList());
    
    for (int i = 0; i < colours.getAllKeys().size(); ++i)
    {
        const String name(colours.getAllKeys()[i]);
        const Colour colour(Colour::fromString(colours[name]));
        const bool isSelected = (colour == this->annotation.getTrackColour());
        cmds.add(MenuItem::item(isSelected ? Icons::apply : Icons::colour, CommandIDs::SetAnnotationColour + i, name)->colouredWith(colour));
    }
    
    cmds.add(MenuItem::item(Icons::close, CommandIDs::DeleteAnnotation, TRANS("menu::annotation::delete")));
    this->updateContent(cmds, SlideDown);
}

void AnnotationMenu::handleCommandMessage(int commandId)
{
    if (HybridRoll *roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
    {
        if (commandId == CommandIDs::RenameAnnotation)
        {
            auto sequence = static_cast<AnnotationsSequence *>(this->annotation.getSequence());
            auto inputDialog = ModalDialogInput::Presets::renameAnnotation(this->annotation.getDescription());
            inputDialog->onOk = sequence->getEventRenameCallback(this->annotation);
            App::Layout().showModalComponentUnowned(inputDialog.release());
        }
        else if (commandId == CommandIDs::DeleteAnnotation)
        {
            AnnotationsSequence *autoLayer = static_cast<AnnotationsSequence *>(this->annotation.getSequence());
            autoLayer->checkpoint();
            autoLayer->remove(this->annotation, true);
        }
        else
        {
            const StringPairArray colours(MenuPanel::getColoursList());
            const int colourIndex = (commandId - CommandIDs::SetAnnotationColour);
            const String name(colours.getAllKeys()[colourIndex]);
            const Colour colour(Colour::fromString(colours[name]));
            
            if (colour == this->annotation.getTrackColour())
            {
                return;
            }

            Array<AnnotationEvent> groupDragBefore, groupDragAfter;
            groupDragBefore.add(this->annotation);
            groupDragAfter.add(this->annotation.withColour(colour));
            AnnotationsSequence *autoLayer = static_cast<AnnotationsSequence *>(this->annotation.getSequence());
            autoLayer->checkpoint();
            autoLayer->changeGroup(groupDragBefore, groupDragAfter, true);
        }

        this->dismiss();
    }
    else
    {
        jassertfalse;
    }
}
