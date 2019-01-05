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
#include "HybridRoll.h"
#include "Transport.h"
#include "Lasso.h"

#include "ScalesManager.h"
#include "SequencerOperations.h"
#include "NoteComponent.h"
#include "PianoSequence.h"

#include "CommandIDs.h"

RescalePreviewTool *RescalePreviewTool::createWithinContext(Lasso &lasso,
    WeakReference<MidiTrack> keySignatures, HybridRoll &roll)
{
    if (lasso.getNumSelected() > 0)
    {
        Note::Key key;
        Scale::Ptr scale = nullptr;
        const Clip &clip = lasso.getFirstAs<NoteComponent>()->getClip();
        if (!SequencerOperations::findHarmonicContext(lasso, clip, keySignatures, scale, key))
        {
            DBG("Warning: harmonic context could not be detected");
            return nullptr;
        }

        return new RescalePreviewTool(lasso, roll, key, scale);
    }

    return nullptr;
}

RescalePreviewTool::RescalePreviewTool(Lasso &lasso, HybridRoll &roll,
    Note::Key keyContext, Scale::Ptr scaleContext) :
    lasso(lasso),
    roll(roll),
    keyContext(keyContext),
    scaleContext(scaleContext),
    hasMadeChanges(false),
    lastChosenScale(nullptr)
{
    // this code pretty much duplicates menu from PianoRollSelectionMenu,
    // but adds undos and starts/stops playback of the selected fragment

    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS("menu::cancel"))->withAction([this]()
    {
        this->undoIfNeeded();
        this->dismissAsync();
    }));

    const auto scales = ScalesManager::getInstance().getScales();
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
            const auto scales = ScalesManager::getInstance().getScales();
            if (scales[i] != this->lastChosenScale && !transport.isPlaying())
            {
                const bool needsCheckpoint = !this->hasMadeChanges;
                this->undoIfNeeded();

                SequencerOperations::rescale(this->lasso, this->scaleContext, scales[i], needsCheckpoint);

                this->lastChosenScale = scales[i];
                this->hasMadeChanges = true;

                const auto firstBeat = SequencerOperations::findStartBeat(this->lasso);
                const auto lastBeat = SequencerOperations::findEndBeat(this->lasso);
                const auto loopStart = this->roll.getTransportPositionByBeat(firstBeat);
                const auto loopEnd = this->roll.getTransportPositionByBeat(lastBeat);
                transport.startPlaybackLooped(loopStart, loopEnd);
            }
            else
            {
                transport.stopPlayback();
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
        auto *sequence = SequencerOperations::getPianoSequence(this->lasso);
        sequence->undo();
    }
}
