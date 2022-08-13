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

#include "Meter.h"
#include "ColourIDs.h"
#include "PlayButton.h"
#include "IconButton.h"
#include "Transport.h"
#include "Instrument.h"
#include "MetronomeSynth.h"
#include "HelioTheme.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "PluginWindow.h"

class MetronomePreviewThread final : public Thread
{
public:

    MetronomePreviewThread(const Transport &transport,
        const MetronomeScheme &metronome,
        WeakReference<Instrument> instrument) :
        Thread("MetronomePreview"),
        transport(transport),
        metronome(metronome),
        instrument(instrument) {}

    void run() override
    {
        while (!this->threadShouldExit())
        {
            for (int i = 0; i < metronome.getSize(); ++i)
            {
                if (this->threadShouldExit())
                {
                    break;
                }

                const auto syllable = metronome.getSyllableAt(i);
                const auto key = MetronomeSynth::getKeyForSyllable(syllable);

                // todo someday: tempo to depend on time signature's denominator
                const auto endTime = Time::getMillisecondCounter() + 300;

                this->transport.previewKey(this->instrument, key, 1.f, float(Globals::beatsPerBar));

                while (Time::getMillisecondCounter() < endTime)
                {
                    if (this->threadShouldExit())
                    {
                        break;
                    }

                    Thread::wait(25);
                }
            }
        }

        this->transport.stopSound({});
    }

private:

    const Transport &transport;
    const MetronomeScheme metronome;
    WeakReference<Instrument> instrument;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetronomePreviewThread)
};

class MetronomeEditor final : public Component
{
public:

    MetronomeEditor(Transport &transport, WeakReference<Instrument> instrument) :
        transport(transport),
        metronomeInstrument(instrument)
    {
        this->playButton = make<PlayButton>(this);
        this->addAndMakeVisible(this->playButton.get());

        this->metronomeUiButton = make<IconButton>(Icons::metronome, CommandIDs::ShowMetronomeSettings);
        this->addAndMakeVisible(this->metronomeUiButton.get());

        this->metronome.reset(); // should be invalid by default
    }

    ~MetronomeEditor() override
    {
        if (this->metronomePreviewThread != nullptr)
        {
            this->metronomePreviewThread->stopThread(500);
        }

        this->transport.stopPlayback();
    }

    Function<void(const MetronomeScheme &metronome, int tappedSyllable)> onTap;

    void setMetronome(const MetronomeScheme &metronome)
    {
        if (this->metronome == metronome)
        {
            return;
        }

        this->metronome = metronome;

        this->buttons.clearQuick(true);

        for (int i = 0; i < metronome.getSize(); ++i)
        {
            const auto syllable = metronome.getSyllableAt(i);
            auto button = make<SyllableButton>(syllable);
            button->onTap = [i, this] {
                if (this->onTap != nullptr)
                {
                    this->onTap(this->metronome, i);
                }
            };

            this->addAndMakeVisible(button.get());
            this->buttons.add(button.release());
        }

        this->resized();
    }

    Rectangle<int> getSyllableButtonsArea() const noexcept
    {
        const auto syllablesWidth = this->buttons.size() * (buttonWidth + buttonMargin) - buttonMargin;
        return this->getLocalBounds().withSizeKeepingCentre(syllablesWidth, this->getHeight() - buttonMargin * 2);
    }

    Rectangle<int> getAllButtonsArea() const noexcept
    {
        constexpr auto helpersMargin = 45;
        return getSyllableButtonsArea().expanded(helpersMargin, 0);
    }

    void resized() override
    {
        auto syllableButtonsArea = getSyllableButtonsArea();
        auto helperButtonsArea = getAllButtonsArea();

        this->playButton->setBounds(helperButtonsArea.removeFromRight(buttonWidth).expanded(5));
        this->metronomeUiButton->setBounds(helperButtonsArea.removeFromLeft(buttonWidth));

        for (const auto &button : this->buttons)
        {
            button->setBounds(syllableButtonsArea.removeFromLeft(buttonWidth));
            syllableButtonsArea.removeFromLeft(buttonMargin);
        }
    }

    void handleCommandMessage(int commandId)
    {
        if (commandId == CommandIDs::TransportPlaybackStart)
        {
            if (this->metronomePreviewThread != nullptr)
            {
                this->metronomePreviewThread->stopThread(500);
            }

            this->metronomePreviewThread = make<MetronomePreviewThread>(this->transport,
                this->metronome, this->metronomeInstrument);

            this->metronomePreviewThread->startThread(5);
            this->playButton->setPlaying(true);
        }
        else if (commandId == CommandIDs::TransportStop)
        {
            if (this->metronomePreviewThread != nullptr)
            {
                this->metronomePreviewThread->stopThread(500);
            }

            this->playButton->setPlaying(false);
        }
        else if (commandId == CommandIDs::ShowMetronomeSettings)
        {
            if (auto *parent = this->getParentComponent())
            {
                parent->postCommandMessage(CommandIDs::DismissModalDialogAsync);
            }

            PluginWindow::showWindowFor(App::Workspace().getAudioCore().getMetronomeInstrumentId());
        }
    }

    void paint(Graphics &g) override
    {
        HelioTheme::drawFrame(g, this->getWidth(), this->getHeight(), 1.25f, 0.75f);
    }

    class SyllableButton final : public HighlightedComponent
    {
    public:

        using Syllable = MetronomeScheme::Syllable;

        SyllableButton() = delete;

        explicit SyllableButton(Syllable syllable, bool previewsNextSyllable = true) :
            previewsNextSyllable(previewsNextSyllable),
            syllable(syllable), nextSyllable(syllable) {}

        Function<void()> onTap;

        void updateSyllable(Syllable newSyllable)
        {
            this->syllable = newSyllable;
            this->repaint();
        }

        void resized() override
        {
            Path p;
            //  _____
            // /_____\ kind of shape to mimic the metronome look:
            constexpr auto slope = 1.f / 20.f;
            p.addQuadrilateral(slope, 0.f, 1.f - slope, 0.f, 1.f, 1.f, 0.f, 1.f);

            const auto w = float(this->getWidth());
            const auto wStep = w * slope;

            const auto h = float(this->getHeight());
            const auto shapeH = h / 4.f - 2;

            p.scaleToFit(0.f, h * 0.75f, w, shapeH, false);
            this->level1Shape = p;

            p.scaleToFit(wStep, h * 0.5f, w - wStep * 2.f, shapeH, false);
            this->level2Shape = p;

            p.scaleToFit(wStep * 2.f, h * 0.25f, w - wStep * 4.f, shapeH, false);
            this->level3Shape = p;

            p.scaleToFit(wStep * 3.f, 0.f, w - wStep * 6.f, shapeH, false);
            this->level4Shape = p;
        }

        void paint(Graphics &g) override
        {
            if (this->syllable == this->nextSyllable)
            {
                g.setColour(this->contoursColour);
                g.fillPath(this->level1Shape);
                g.fillPath(this->level2Shape);
                g.fillPath(this->level3Shape);
                g.fillPath(this->level4Shape);
            }
            else
            {
                g.setColour(this->paleColour);
                switch (this->nextSyllable)
                {
                    case Syllable::Oo: g.fillPath(this->level4Shape);
                    case Syllable::Pa: g.fillPath(this->level3Shape);
                    case Syllable::na: g.fillPath(this->level2Shape);
                    case Syllable::pa: g.fillPath(this->level1Shape);
                }
            }

            g.setColour(this->brightColour);
            switch (this->syllable)
            {
                case Syllable::Oo: g.fillPath(this->level4Shape);
                case Syllable::Pa: g.fillPath(this->level3Shape);
                case Syllable::na: g.fillPath(this->level2Shape);
                case Syllable::pa: g.fillPath(this->level1Shape);
            }
        }

        void mouseDown(const MouseEvent &e) override
        {
            if (this->onTap != nullptr)
            {
                this->onTap();
            }
        }

    private:

        const bool previewsNextSyllable = true;

        // for highligher component only:
        SyllableButton(Syllable syllable, Syllable nextSyllable) :
            syllable(syllable), nextSyllable(nextSyllable) {}

        Component *createHighlighterComponent() override
        {
            if (this->previewsNextSyllable)
            {
                return new SyllableButton(this->syllable,
                    MetronomeScheme::getNextSyllable(this->syllable));
            }

            return new SyllableButton(this->syllable);
        }

        Syllable syllable;
        Syllable nextSyllable; // to display the upcoming change

        Path level1Shape;
        Path level2Shape;
        Path level3Shape;
        Path level4Shape;

        const Colour brightColour =
            findDefaultColour(Label::textColourId).withMultipliedAlpha(0.6f);

        const Colour paleColour =
            findDefaultColour(Label::textColourId).withMultipliedAlpha(0.25f);

        const Colour contoursColour =
            findDefaultColour(Label::textColourId).withMultipliedAlpha(0.1f);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyllableButton)
    };

private:

    Transport &transport;
    WeakReference<Instrument> metronomeInstrument;

    MetronomeScheme metronome;

    UniquePointer<Thread> metronomePreviewThread;

    static constexpr auto buttonWidth = 18;
    static constexpr auto buttonMargin = 12;

    OwnedArray<SyllableButton> buttons;
    UniquePointer<PlayButton> playButton;
    UniquePointer<IconButton> metronomeUiButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetronomeEditor)
};
