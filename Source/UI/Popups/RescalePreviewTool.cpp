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
#include "RescalePreviewTool.h"
#include "PianoRoll.h"
#include "Transport.h"
#include "MidiTrack.h"
#include "Lasso.h"
#include "SequencerOperations.h"
#include "NoteComponent.h"
#include "PianoSequence.h"
#include "CommandIDs.h"
#include "Config.h"

RescalePreviewTool *RescalePreviewTool::createWithinContext(PianoRoll &roll,
    WeakReference<MidiTrack> keySignatures)
{
    if (roll.getLassoSelection().getNumSelected() > 0)
    {
        Note::Key key;
        Scale::Ptr scale = nullptr;
        const Clip &clip = roll.getLassoSelection().getFirstAs<NoteComponent>()->getClip();
        if (!SequencerOperations::findHarmonicContext(roll.getLassoSelection(), clip, keySignatures, scale, key))
        {
            DBG("Warning: harmonic context could not be detected");
            return new RescalePreviewTool(roll, 0, Scale::getNaturalMajorScale());
        }

        return new RescalePreviewTool(roll, key, scale);
    }

    return nullptr;
}

RescalePreviewTool::RescalePreviewTool(PianoRoll &roll,
    Note::Key keyContext, Scale::Ptr scaleContext) :
    roll(roll),
    keyContext(keyContext),
    scaleContext(scaleContext),
    hasMadeChanges(false),
    lastChosenScale(nullptr)
{
    // this code pretty much duplicates menu from PianoRollSelectionMenu,
    // but adds undos and starts/stops playback of the selected fragment

    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::close, TRANS("menu::cancel"))->withAction([this]()
    {
        this->undoIfNeeded();
        this->dismissAsync();
    }));

    const auto scales = App::Config().getScales()->getAll();
    for (int i = 0; i < scales.size(); ++i)
    {
        menu.add(MenuItem::item(Icons::arpeggiate,
            scales.getUnchecked(i)->getLocalizedName())->withAction([this, i]()
        {
            if (this->scaleContext == nullptr)
            {
                jassertfalse;
                return;
            }

            auto &transport = this->roll.getTransport();
            const auto scales = App::Config().getScales()->getAll();
            if (!scales[i]->isEquivalentTo(this->lastChosenScale))
            {
                transport.stopPlayback();
                const bool needsCheckpoint = !this->hasMadeChanges;
                this->undoIfNeeded();

                SequencerOperations::rescale(this->roll.getLassoSelection(),
                    this->scaleContext, scales[i], needsCheckpoint);

                this->lastChosenScale = scales[i];
                this->hasMadeChanges = true;
            }

            if (transport.isPlaying())
            {
                transport.stopPlayback();
            }
            else
            {
                const auto firstBeat = this->roll.getLassoStartBeat();
                const auto lastBeat = this->roll.getLassoEndBeat();
                const auto fragmentStart = this->roll.getTransportPositionByBeat(firstBeat);
                const auto fragmentEnd = this->roll.getTransportPositionByBeat(lastBeat);
                transport.startPlaybackFragment(fragmentStart - 0.001, fragmentEnd);
            }
        }));
    }

    this->updateContent(menu, MenuPanel::SlideUp);
}

void RescalePreviewTool::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::Cancel)
    {
        this->undoIfNeeded();
    }
}

void RescalePreviewTool::dismissAsync()
{
    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::HideCallout);
    }
}

void RescalePreviewTool::undoIfNeeded()
{
    if (this->hasMadeChanges)
    {
        this->roll.getActiveTrack()->getSequence()->undo();
    }
}
