/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "ProjectNode.h"
#include "ProjectMetadata.h"
#include "RescalePreviewTool.h"
#include "PianoRoll.h"
#include "SequencerOperations.h"
#include "NoteComponent.h"
#include "PianoSequence.h"
#include "KeySignaturesSequence.h"
#include "CommandIDs.h"
#include "Config.h"

RescalePreviewTool::RescalePreviewTool(PianoRoll &roll,
    WeakReference<KeySignaturesSequence> harmonicContext) :
    roll(roll),
    harmonicContext(harmonicContext)
{
    // this code pretty much duplicates menu from PianoRollSelectionMenu,
    // but adds undos and starts/stops playback of the selected fragment

    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::close, TRANS(I18n::Menu::cancel))->withAction([this]()
    {
        this->undoIfNeeded();
        this->dismissCalloutAsync();
    }));

    const auto scales = App::Config().getScales()->getAll();
    for (int i = 0; i < scales.size(); ++i)
    {
        if (scales.getUnchecked(i)->getBasePeriod() != this->roll.getPeriodSize())
        {
            continue;
        }

        menu.add(MenuItem::item(Icons::arpeggiate,
            scales.getUnchecked(i)->getLocalizedName())->withAction([this, i]()
        {
            auto &transport = this->roll.getTransport();
            const auto scales = App::Config().getScales()->getAll();
            if (!scales[i]->isEquivalentTo(this->lastChosenScale))
            {
                transport.stopPlaybackAndRecording();
                const bool needsCheckpoint = !this->hasMadeChanges;
                this->undoIfNeeded();

                SequencerOperations::rescale(this->roll.getLassoOrEntireSequence(),
                    this->roll.getActiveClip(), this->harmonicContext, scales[i],
                    true, needsCheckpoint);

                this->lastChosenScale = scales[i];
                this->hasMadeChanges = true;
            }

            if (transport.isPlaying())
            {
                transport.stopPlaybackAndRecording();
            }
            else
            {
                const auto playbackStartBeat = this->roll.getActiveClip().getBeat() +
                    SequencerOperations::findStartBeat(this->roll.getLassoOrEntireSequence()) - 0.001f;

                const auto playbackEndBeat = this->roll.getActiveClip().getBeat() +
                    SequencerOperations::findEndBeat(this->roll.getLassoOrEntireSequence()) - 0.001f;

                transport.startPlaybackFragment(playbackStartBeat, playbackEndBeat, true);
            }
        }));
    }

    this->updateContent(menu, MenuPanel::SlideUp);
}

void RescalePreviewTool::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::MenuDismiss:
        this->undoIfNeeded();
        this->dismissCalloutAsync();
        break;
    default:
        MenuPanel::handleCommandMessage(commandId);
        break;
    }
}

void RescalePreviewTool::dismissCalloutAsync()
{
    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::DismissModalComponentAsync);
    }
}

void RescalePreviewTool::undoIfNeeded()
{
    if (this->hasMadeChanges)
    {
        this->roll.getActiveTrack()->getSequence()->undo();
    }
}

// One-shot rescale menu
QuickRescaleMenu::QuickRescaleMenu(const ProjectNode &project,
    const KeySignatureEvent &event, float endBeat) :
    project(project),
    event(event),
    endBeat(endBeat)
{
    MenuPanel::Menu menu;

    int currentScaleIndex = -1;
    const auto scales = App::Config().getScales()->getAll();
    for (int i = 0; i < scales.size(); ++i)
    {
        auto scale = scales.getUnchecked(i);
        if (scale->getBasePeriod() !=
            this->project.getProjectInfo()->getTemperament()->getPeriodSize())
        {
            continue;
        }

        const auto isEquivalentToCurrent =
            scale->isEquivalentTo(this->event.getScale());
        const auto isCurrentScale = isEquivalentToCurrent &&
            scale->getUnlocalizedName() == this->event.getScale()->getUnlocalizedName();

        if (isCurrentScale)
        {
            currentScaleIndex = menu.size();
        }

        menu.add(MenuItem::item(Icons::arpeggiate,
            scales.getUnchecked(i)->getLocalizedName())->
            disabledIf(isEquivalentToCurrent)->
            withAction([this, i]()
        {
            const auto scales = App::Config().getScales()->getAll();
            jassert(!scales[i]->isEquivalentTo(this->event.getScale()));

            const bool hasMadeChanges = 
                SequencerOperations::rescale(this->project, this->event.getBeat(), this->endBeat,
                    this->event.getRootKey(), this->event.getScale(), scales[i],
                    true, true);

            auto *keySequence = static_cast<KeySignaturesSequence *>(this->event.getSequence());
            if (!hasMadeChanges)
            {
                keySequence->checkpoint();
            }

            keySequence->change(this->event, this->event.withScale(scales[i]), true);

            this->dismissCalloutAsync();
        }));
    }

    this->updateContent(menu, MenuPanel::SlideDown, true, currentScaleIndex);
}

void QuickRescaleMenu::dismissCalloutAsync()
{
    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::DismissModalComponentAsync);
    }
}
