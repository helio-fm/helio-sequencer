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
#include "TimeSignaturesLayer.h"
#include "PianoLayerTreeItem.h"
#include "ProjectTimeline.h"
#include "MidiLayer.h"
#include "App.h"

TimeSignatureCommandPanel::TimeSignatureCommandPanel(ProjectTreeItem &parentProject, 
	const TimeSignatureEvent &targetEvent) :
    project(parentProject),
    event(targetEvent)
{
    ReferenceCountedArray<CommandItem> cmds;
    cmds.add(CommandItem::withParams(Icons::ellipsis, CommandIDs::ChangeTimeSignature, TRANS("menu::timesignature::change")));
    cmds.add(CommandItem::withParams(Icons::close, CommandIDs::DeleteTimeSignature, TRANS("menu::timesignature::delete")));
    this->updateContent(cmds, SlideDown);
}

TimeSignatureCommandPanel::~TimeSignatureCommandPanel()
{
}

void TimeSignatureCommandPanel::handleCommandMessage(int commandId)
{
    if (HybridRoll *roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
    {
        if (commandId == CommandIDs::ChangeTimeSignature)
        {
            this->inputString = this->event.toString();
            
            Component *inputDialog =
            new ModalDialogInput(*this,
                                 this->inputString,
                                 TRANS("dialog::timesignature::change::caption"),
                                 TRANS("dialog::timesignature::change::proceed"),
                                 TRANS("dialog::timesignature::change::cancel"),
                                 CommandIDs::ChangeTimeSignatureConfirmed,
                                 CommandIDs::Cancel);
            
            App::Layout().showModalNonOwnedDialog(inputDialog);
            return;
        }
        if (commandId == CommandIDs::ChangeTimeSignatureConfirmed)
        {
			int numerator;
			int denominator;
			TimeSignatureEvent::parseString(this->inputString, numerator, denominator);
            Array<TimeSignatureEvent> groupBefore, groupAfter;
			groupBefore.add(this->event);
			groupAfter.add(this->event.withNumerator(numerator).withDenominator(denominator));
			TimeSignaturesLayer *autoLayer = static_cast<TimeSignaturesLayer *>(this->event.getLayer());
            autoLayer->checkpoint();
            autoLayer->changeGroup(groupBefore, groupAfter, true);
        }
        else if (commandId == CommandIDs::DeleteTimeSignature)
        {
			TimeSignaturesLayer *autoLayer = static_cast<TimeSignaturesLayer *>(this->event.getLayer());
            autoLayer->checkpoint();
            autoLayer->remove(this->event, true);
        }
		else if (commandId == CommandIDs::Cancel)
		{
			return;
		}

        roll->grabKeyboardFocus();
        this->getParentComponent()->exitModalState(0);
    }
    else
    {
        jassertfalse;
    }
}
