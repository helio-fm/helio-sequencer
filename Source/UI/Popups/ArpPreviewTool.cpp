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
#include "ArpPreviewTool.h"
#include "PianoRoll.h"
#include "Transport.h"
#include "MidiTrack.h"
#include "Lasso.h"
#include "SequencerOperations.h"
#include "PianoSequence.h"
#include "NoteComponent.h"
#include "CommandIDs.h"
#include "Config.h"

ArpPreviewTool *ArpPreviewTool::createWithinContext(PianoRoll &roll,
    WeakReference<MidiTrack> keySignatures)
{
    if (roll.getLassoSelection().getNumSelected() > 1)
    {
        const bool advancedMode =
            Desktop::getInstance().getMainMouseSource()
                .getCurrentModifiers().isAnyModifierKeyDown();

        Note::Key key;
        Scale::Ptr scale = nullptr;
        const Clip &clip = roll.getLassoSelection().getFirstAs<NoteComponent>()->getClip();
        if (!SequencerOperations::findHarmonicContext(roll.getLassoSelection(),
            clip, keySignatures, scale, key))
        {
            DBG("Warning: harmonic context could not be detected");
            return new ArpPreviewTool(roll, 0, Scale::getNaturalMajorScale(), advancedMode);
        }

        return new ArpPreviewTool(roll, key, scale, advancedMode);
    }

    return nullptr;
}

ArpPreviewTool::ArpPreviewTool(PianoRoll &roll, Note::Key keyContext,
    Scale::Ptr scaleContext, bool advancedMode) :
    roll(roll),
    keyContext(keyContext),
    scaleContext(scaleContext),
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
            if (this->roll.getLassoSelection().getNumSelected() < 2 || this->scaleContext == nullptr)
            {
                jassertfalse;
                return;
            }

            const auto arps = App::Config().getArpeggiators()->getAll();

            if (this->advancedMode)
            {
                this->updateContent(this->createOptionsMenu(arps[i]), MenuPanel::SlideLeft);
                return;
            }

            this->previewArp(arps[i], this->lastOptions, false);
        }));
    }

    this->updateContent(this->mainMenu, MenuPanel::SlideUp);
}

MenuPanel::Menu ArpPreviewTool::createOptionsMenu(Arpeggiator::Ptr arp)
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back,
        TRANS(I18n::Menu::back))->withTimer()->withAction([this]()
    {
        this->updateContent(this->mainMenu, MenuPanel::SlideRight);
    }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "Reversed")->toggled(this->lastOptions.reversed)->withAction([this, arp]()
    {
        this->lastOptions.reversed = !this->lastOptions.reversed;
        this->previewArp(arp, this->lastOptions, true);
        this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
    }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "Limit to chord")->toggled(this->lastOptions.limitToChord)->withAction([this, arp]()
    {
        this->lastOptions.limitToChord = !this->lastOptions.limitToChord;
        this->previewArp(arp, this->lastOptions, true);
        this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
    }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "Duration x0.5")->toggled(this->lastOptions.durationMultiplier == 0.5f)->withAction([this, arp]()
    {
        this->lastOptions.durationMultiplier = 0.5f;
        this->previewArp(arp, this->lastOptions, true);
        this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
    }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "Duration x0.75")->toggled(this->lastOptions.durationMultiplier == 0.75f)->withAction([this, arp]()
    {
        this->lastOptions.durationMultiplier = 0.75f;
        this->previewArp(arp, this->lastOptions, true);
        this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
    }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "Duration x1")->toggled(this->lastOptions.durationMultiplier == 1.f)->withAction([this, arp]()
    {
        this->lastOptions.durationMultiplier = 1.f;
        this->previewArp(arp, this->lastOptions, true);
        this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
    }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "Duration x1.5")->toggled(this->lastOptions.durationMultiplier == 1.5f)->withAction([this, arp]()
    {
        this->lastOptions.durationMultiplier = 1.5f;
        this->previewArp(arp, this->lastOptions, true);
        this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
    }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "Duration x2")->toggled(this->lastOptions.durationMultiplier == 2.f)->withAction([this, arp]()
    {
        this->lastOptions.durationMultiplier = 2.f;
        this->previewArp(arp, this->lastOptions, true);
        this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
    }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "Randomness x0")->toggled(this->lastOptions.randomness == 0.f)->withAction([this, arp]()
    {
        this->lastOptions.randomness = 0.f;
        this->previewArp(arp, this->lastOptions, true);
        this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
    }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "Randomness x0.5")->toggled(this->lastOptions.randomness == 0.5f)->withAction([this, arp]()
    {
        this->lastOptions.randomness = 0.5f;
        this->previewArp(arp, this->lastOptions, true);
        this->updateContent(this->createOptionsMenu(arp), MenuPanel::None);
    }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        "Randomness x0.75")->toggled(this->lastOptions.randomness == 0.75f)->withAction([this, arp]()
    {
        this->lastOptions.randomness = 0.75f;
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
        transport.stopPlayback();
        const bool needsCheckpoint = !this->hasMadeChanges;
        this->undoIfNeeded();

        SequencerOperations::arpeggiate(this->roll.getLassoSelection(),
            this->scaleContext, this->keyContext, arp,
            options.durationMultiplier, options.randomness,
            options.reversed, options.limitToChord,
            needsCheckpoint);

        this->lastChosenArp = arp;
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
        transport.startPlaybackFragment(firstBeat - 0.001f, lastBeat);
    }
}

void ArpPreviewTool::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::Cancel)
    {
        this->undoIfNeeded();
    }
}

void ArpPreviewTool::dismissAsync()
{
    if (auto *parent = this->getParentComponent())
    {
        parent->postCommandMessage(CommandIDs::HideCallout);
    }
}

void ArpPreviewTool::undoIfNeeded()
{
    if (this->hasMadeChanges)
    {
        this->roll.getActiveTrack()->getSequence()->undo();
    }
}
