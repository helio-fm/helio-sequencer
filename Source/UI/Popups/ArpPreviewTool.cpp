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
#include "ArpPreviewTool.h"
#include "PianoRoll.h"
#include "SequencerOperations.h"
#include "PianoSequence.h"
#include "NoteComponent.h"
#include "CommandIDs.h"
#include "Config.h"

ArpPreviewTool::ArpPreviewTool(PianoRoll &roll,
    WeakReference<KeySignaturesSequence> harmonicContext,
    WeakReference<TimeSignaturesAggregator> timeContext,
    bool advancedMode) :
    roll(roll),
    harmonicContext(harmonicContext),
    timeContext(timeContext),
    advancedMode(advancedMode)
{
    // this code pretty much duplicates menu from PianoRollSelectionMenu,
    // but adds undos and starts/stops playback of the selected fragment

    this->mainMenu.add(MenuItem::item(Icons::close, TRANS(I18n::Menu::cancel))->withAction([this]()
    {
        this->undoIfNeeded();
        this->dismissAsync();
    }));

    const auto arps = App::Config().getArpeggiators()->getAll();
    for (int i = 0; i < arps.size(); ++i)
    {
        this->mainMenu.add(MenuItem::item(Icons::arpeggiate, arps.getUnchecked(i)->getName())->
            withSubmenuIf(this->advancedMode)->withAction([this, i]()
        {
            const auto arps = App::Config().getArpeggiators()->getAll();
            this->previewArp(arps[i], this->lastOptions, false);

            if (this->advancedMode)
            {
                this->updateContent(this->createOptionsMenu(arps[i]), MenuPanel::SlideLeft);
            }
        }));
    }

    this->updateContent(this->mainMenu, MenuPanel::SlideUp);
}

MenuPanel::Menu ArpPreviewTool::createOptionsMenu(Arpeggiator::Ptr arp)
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back,
        TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->mainMenu, MenuPanel::SlideRight);
    }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "x0.25")->toggledIf(this->lastOptions.durationMultiplier == 0.25f)->withAction([this, arp]()
        {
            this->lastOptions.durationMultiplier = 0.25f;
            this->previewArp(arp, this->lastOptions, true);
            this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
        }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "x0.5")->toggledIf(this->lastOptions.durationMultiplier == 0.5f)->withAction([this, arp]()
        {
            this->lastOptions.durationMultiplier = 0.5f;
            this->previewArp(arp, this->lastOptions, true);
            this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
        }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "x1")->toggledIf(this->lastOptions.durationMultiplier == 1.f)->withAction([this, arp]()
        {
            this->lastOptions.durationMultiplier = 1.f;
            this->previewArp(arp, this->lastOptions, true);
            this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
        }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "x2")->toggledIf(this->lastOptions.durationMultiplier == 2.f)->withAction([this, arp]()
        {
            this->lastOptions.durationMultiplier = 2.f;
            this->previewArp(arp, this->lastOptions, true);
            this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
        }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "x4")->toggledIf(this->lastOptions.durationMultiplier == 4.f)->withAction([this, arp]()
        {
            this->lastOptions.durationMultiplier = 4.f;
            this->previewArp(arp, this->lastOptions, true);
            this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
        }));

    return menu;
}

void ArpPreviewTool::previewArp(Arpeggiator::Ptr arp, const Options options, bool forceRecreate)
{
    auto &transport = this->roll.getTransport();

    if (forceRecreate || arp != this->lastChosenArp)
    {
        transport.stopPlaybackAndRecording();
        const bool needsCheckpoint = !this->hasMadeChanges;
        this->undoIfNeeded();

        SequencerOperations::arpeggiate(this->roll.getLassoOrEntireSequence(),
            this->roll.getActiveClip(),
            arp,
            this->roll.getTemperament(),
            this->harmonicContext,
            this->timeContext,
            options.durationMultiplier,
            options.randomness,
            options.reversed,
            options.limitToChord,
            true,
            needsCheckpoint);

        this->lastChosenArp = arp;
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
}

void ArpPreviewTool::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::MenuDismiss:
        this->undoIfNeeded();
        this->dismissAsync();
        break;
    default:
        MenuPanel::handleCommandMessage(commandId);
        break;
    }
}

void ArpPreviewTool::dismissAsync()
{
    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::DismissModalComponentAsync);
    }
}

void ArpPreviewTool::undoIfNeeded()
{
    if (this->hasMadeChanges)
    {
        this->roll.getActiveTrack()->getSequence()->undo();
    }
}
