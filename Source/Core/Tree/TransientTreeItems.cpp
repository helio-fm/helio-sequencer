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
#include "TransientTreeItems.h"
#include "CommandPanel.h"
#include "CommandIDs.h"
#include "Icons.h"
#include "App.h"
#include "Workspace.h"

//===----------------------------------------------------------------------===//
// PianoRoll Selection Menu
//===----------------------------------------------------------------------===//

class PianoRollSelectionCommandPanel : public CommandPanel
{
public:

    PianoRollSelectionCommandPanel::PianoRollSelectionCommandPanel(MidiTrackTreeItem &parentLayer) :
        layerItem(parentLayer)
    {
        this->initDefaultCommands();
    }

    void handleCommandMessage(int commandId)
    {
        switch (commandId)
        {
        case CommandIDs::SelectAllEvents:
            if (ProjectTreeItem *project = this->layerItem.getProject())
            {
                if (HybridRoll *roll = dynamic_cast<HybridRoll *>(project->getLastFocusedRoll()))
                {
                    roll->selectAll();
                }
            }

            this->exit();
            break;
        default:
            this->exit();
            break;
        }
    }

private:

    void initDefaultCommands()
    {
        ReferenceCountedArray<CommandItem> cmds;
        cmds.add(CommandItem::withParams(Icons::copy, CommandIDs::CopyEvents, TRANS("menu::selection::piano::copy")));
        cmds.add(CommandItem::withParams(Icons::cut, CommandIDs::CutEvents, TRANS("menu::selection::piano::cut")));
        cmds.add(CommandItem::withParams(Icons::trash, CommandIDs::DeleteEvents, TRANS("menu::selection::piano::delete")));
        //cmds.add(CommandItem::withParams(Icons::trash, CommandIDs::RefactorRemoveOverlaps, TRANS("menu::selection::piano::cleanup")));
        this->updateContent(cmds, CommandPanel::SlideRight);
    }
};

Image PianoRollSelectionTreeItem::getIcon() const
{
    return Icons::findByName(Icons::empty, TREE_LARGE_ICON_HEIGHT);
}

String PianoRollSelectionTreeItem::getCaption() const
{
    return TRANS("tree::selection::piano");
}

Component *PianoRollSelectionTreeItem::createItemMenu()
{
    return new PianoRollSelectionCommandPanel(*this, CommandPanel::SlideRight);
}

//===----------------------------------------------------------------------===//
// PatternRoll Selection Menu
//===----------------------------------------------------------------------===//

class PatternRollSelectionCommandPanel : public CommandPanel
{
public:

};

Image PatternRollSelectionTreeItem::getIcon() const
{
    return Icons::findByName(Icons::empty, TREE_LARGE_ICON_HEIGHT);
}

String PatternRollSelectionTreeItem::getCaption() const
{
    return TRANS("tree::selection::pattern");
}

Component *PatternRollSelectionTreeItem::createItemMenu()
{
    return new PatternRollSelectionCommandPanel(*this, CommandPanel::SlideRight);
}
