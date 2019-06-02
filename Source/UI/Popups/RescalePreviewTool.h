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

class Lasso;
class MidiTrack;
class PianoRoll;
class ProjectNode;

#include "MenuPanel.h"
#include "Scale.h"
#include "Note.h"
#include "KeySignatureEvent.h"

class RescalePreviewTool final : public MenuPanel
{
public:

    RescalePreviewTool(SafePointer<PianoRoll> roll,
        Note::Key keyContext, Scale::Ptr scaleContext);

    static RescalePreviewTool *createWithinSelectionAndContext(SafePointer<PianoRoll> roll,
        WeakReference<MidiTrack> keySignatures);

    void handleCommandMessage(int commandId) override;

private:

    void dismissAsync();
    void undoIfNeeded();

    SafePointer<PianoRoll> roll;

    Note::Key keyContext;
    Scale::Ptr scaleContext;

    bool hasMadeChanges = false;
    Scale::Ptr lastChosenScale;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RescalePreviewTool)
};


class QuickRescaleMenu final : public MenuPanel
{
public:

    QuickRescaleMenu(const ProjectNode &project,
        const KeySignatureEvent &event, float endBeat);

private:

    const ProjectNode &project;
    const KeySignatureEvent &event;
    const float endBeat;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuickRescaleMenu)
};
