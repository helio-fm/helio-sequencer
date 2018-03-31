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
#include "AnnotationCommandPanel.h"
#include "ProjectTreeItem.h"
#include "MainLayout.h"
#include "ModalDialogInput.h"
#include "Icons.h"
#include "HybridRoll.h"
#include "AnnotationEvent.h"
#include "AnnotationsSequence.h"
#include "PianoTrackTreeItem.h"
#include "ProjectTimeline.h"
#include "MidiSequence.h"
#include "CommandIDs.h"
#include "App.h"

AnnotationCommandPanel::AnnotationCommandPanel(ProjectTreeItem &parentProject, const AnnotationEvent &targetAnnotation) :
    project(parentProject),
    annotation(targetAnnotation)
{
    CommandPanel::Items cmds;
    cmds.add(CommandItem::withParams(Icons::ellipsis, CommandIDs::RenameAnnotation, TRANS("menu::annotation::rename")));
    
    const StringPairArray colours(CommandPanel::getColoursList());
    
    for (int i = 0; i < colours.getAllKeys().size(); ++i)
    {
        const String name(colours.getAllKeys()[i]);
        const Colour colour(Colour::fromString(colours[name]));
        const bool isSelected = (colour == this->annotation.getColour());
        cmds.add(CommandItem::withParams(isSelected ? Icons::apply : Icons::colour, CommandIDs::SetAnnotationColour + i, name)->colouredWith(colour));
    }
    
    cmds.add(CommandItem::withParams(Icons::close, CommandIDs::DeleteAnnotation, TRANS("menu::annotation::delete")));
    this->updateContent(cmds, SlideDown);
}

void AnnotationCommandPanel::handleCommandMessage(int commandId)
{
    if (HybridRoll *roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
    {
        if (commandId == CommandIDs::RenameAnnotation)
        {
            ScopedPointer<ModalDialogInput> inputDialog =
                new ModalDialogInput(this->annotation.getDescription(),
                    TRANS("dialog::annotation::rename::caption"),
                    TRANS("dialog::annotation::rename::proceed"),
                    TRANS("dialog::annotation::rename::cancel"));
            
            auto sequence = static_cast<AnnotationsSequence *>(this->annotation.getSequence());
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
            const StringPairArray colours(CommandPanel::getColoursList());
            const int colourIndex = (commandId - CommandIDs::SetAnnotationColour);
            const String name(colours.getAllKeys()[colourIndex]);
            const Colour colour(Colour::fromString(colours[name]));
            
            if (colour == this->annotation.getColour())
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

        if (Component *parent = this->getParentComponent())
        {
            parent->exitModalState(0);
        }
    }
    else
    {
        jassertfalse;
    }
}
