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
#include "TimeSignatureCommandPanel.h"
#include "ProjectTreeItem.h"
#include "MainLayout.h"
#include "ModalDialogInput.h"
#include "Icons.h"
#include "HybridRoll.h"
#include "TimeSignatureEvent.h"
#include "TimeSignaturesSequence.h"
#include "PianoTrackTreeItem.h"
#include "ProjectTimeline.h"
#include "MidiSequence.h"
#include "App.h"

TimeSignatureCommandPanel::TimeSignatureCommandPanel(ProjectTreeItem &parentProject, 
    const TimeSignatureEvent &targetEvent) :
    project(parentProject),
    event(targetEvent)
{
    CommandPanel::Items cmds;
    cmds.add(CommandItem::withParams(Icons::ellipsis, CommandIDs::ChangeTimeSignature, TRANS("menu::timesignature::change")));
    cmds.add(CommandItem::withParams(Icons::close, CommandIDs::DeleteTimeSignature, TRANS("menu::timesignature::delete")));
    this->updateContent(cmds, SlideDown);
}

void TimeSignatureCommandPanel::handleCommandMessage(int commandId)
{
    if (HybridRoll *roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
    {
        if (commandId == CommandIDs::ChangeTimeSignature)
        {
            auto sequence = static_cast<TimeSignaturesSequence *>(this->event.getSequence());
            auto inputDialog = ModalDialogInput::Presets::changeTimeSignature(this->event.toString());
            inputDialog->onOk = sequence->getEventChangeCallback(this->event);
            App::Layout().showModalComponentUnowned(inputDialog.release());
        }
        else if (commandId == CommandIDs::DeleteTimeSignature)
        {
            TimeSignaturesSequence *autoLayer = static_cast<TimeSignaturesSequence *>(this->event.getSequence());
            autoLayer->checkpoint();
            autoLayer->remove(this->event, true);
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
