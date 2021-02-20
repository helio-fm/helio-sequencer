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

#pragma once

#include "ProjectNode.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"
#include "PianoTrackActions.h"
#include "AutomationTrackActions.h"
#include "TrackPropertiesDialog.h"
#include "SequencerOperations.h"

struct InteractiveActions final
{
    // Shows the "new track" dialog, assumes that the checkpoint with
    // the given checkpointId is already made - this is to allow the caller
    // making any additional custom actions in the same transaction,
    // which will be undone if the "cancel" button is pressed

    static void addNewTrack(ProjectNode &project,
        MidiTrackNode *trackPreset, UndoActionId checkpointId,
        bool shouldFocusOnNewTrack, const String &defaultTrackName,
        const String &dialogTitle, const String &dialogOk)
    {
        if (trackPreset == nullptr ||
            trackPreset->getSequence() == nullptr ||
            trackPreset->getPattern() == nullptr ||
            trackPreset->getPattern()->getClips().isEmpty())
        {
            jassertfalse;
            return;
        }

        const auto trackId = trackPreset->getTrackId();
        const auto trackTemplate = trackPreset->serialize();

        if (dynamic_cast<PianoTrackNode *>(trackPreset))
        {
            addNewTrack<PianoTrackInsertAction>(project,
                trackTemplate, trackId, checkpointId,
                shouldFocusOnNewTrack, defaultTrackName,
                dialogTitle, dialogOk);
        }
        else if (dynamic_cast<AutomationTrackNode *>(trackPreset))
        {
            addNewTrack<AutomationTrackInsertAction>(project,
                trackTemplate, trackId, checkpointId,
                shouldFocusOnNewTrack, defaultTrackName,
                dialogTitle, dialogOk);
        }
        else
        {
            jassertfalse;
        }
    }

    template<typename TAction>
    static void addNewTrack(ProjectNode &project,
        const SerializedData &trackTemplate, const String &trackId,
        UndoActionId checkpointId, bool shouldFocusOnNewTrack,
        const String &defaultTrackName,
        const String &dialogTitle, const String &dialogOk)
    {
        static_assert(std::is_base_of<PianoTrackInsertAction, TAction>::value ||
            std::is_base_of<AutomationTrackInsertAction, TAction>::value,
            "Action must be either PianoTrackInsertAction or AutomationTrackInsertAction");

        const auto newName =
            SequencerOperations::generateNextNameForNewTrack(defaultTrackName,
                project.getAllTrackNames());

        WeakReference<UndoStack> undoStack = project.getUndoStack();

        undoStack->perform(new TAction(project, &project, trackTemplate, newName));

        auto *newlyAddedTrack = project.findTrackById<MidiTrackNode>(trackId);

        if (shouldFocusOnNewTrack)
        {
            auto *tracksSingleClip = newlyAddedTrack->getPattern()->getUnchecked(0);
            project.setEditableScope(newlyAddedTrack, *tracksSingleClip, false);
        }

        auto dialog = make<TrackPropertiesDialog>(project,
            newlyAddedTrack, dialogTitle, dialogOk);

        dialog->onCancel = [undoStack]()
        {
            jassert(undoStack != nullptr);
            undoStack->undo();
        };

        dialog->onOk = [undoStack, checkpointId]()
        {
            jassert(undoStack != nullptr);
            if (checkpointId != UndoActionIDs::None)
            {
                undoStack->mergeTransactionsUpTo(checkpointId);
            }
        };

        App::showModalComponent(move(dialog));
    }
};
