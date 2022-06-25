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
#include "Transport.h"
#include "Instrument.h"
#include "MetronomeSynth.h"

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
            for (const auto syllable : this->metronome.syllables)
            {
                if (this->threadShouldExit())
                {
                    break;
                }

                this->transport.stopSound({});

                // fixme: tempo to depend on time signature's denominator
                Thread::wait(25);

                const auto key = MetronomeSynth::getKeyForSyllable(syllable);

                this->transport.previewKey(this->instrument, key,
                    Globals::Defaults::previewNoteVelocity,
                    Globals::Defaults::previewNoteLength);

                int c = 400;
                while (c > 0)
                {
                    const auto a = Time::getMillisecondCounter();
                    Thread::wait(25);
                    const auto b = Time::getMillisecondCounter();
                    c -= (b - a);

                    if (this->threadShouldExit())
                    {
                        break;
                    }
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

        for (int i = 0; i < metronome.syllables.size(); ++i)
        {
            const auto syllable = metronome.syllables[i];
            auto button = make<SyllabeButton>(syllable, syllable);
            button->onTap = [i, this]
            {
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

    void resized() override
    {
        const auto buttonsWidth =
            this->buttons.size() * (buttonWidth + buttonMargin);

        const auto xOffset = (this->getWidth() - buttonsWidth) / 2 - (buttonWidth / 2);

        int x = 0;
        for (const auto &button : this->buttons)
        {
            const int w = buttonsWidth / this->buttons.size();
            const auto margin = (w - buttonWidth) / 2;
            button->setBounds(xOffset + x + margin, 0, w - margin * 2, this->getHeight());
            x += w;
        }

        this->playButton->setBounds(xOffset + buttonsWidth + buttonMargin,
            0, buttonWidth, this->getHeight());
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
    }

    static constexpr auto buttonWidth = 24;
    static constexpr auto buttonMargin = 16;

private:

    Transport &transport;
    WeakReference<Instrument> metronomeInstrument;

    MetronomeScheme metronome;

    UniquePointer<Thread> metronomePreviewThread;

    class SyllabeButton final : public HighlightedComponent
    {
    public:

        using Syllable = MetronomeScheme::Syllable;

        SyllabeButton() = delete;
        SyllabeButton(Syllable syllable, Syllable nextSyllable) :
            syllable(syllable), nextSyllable(nextSyllable) {}

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
            constexpr auto slope = 1.f / 15.f;
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

        Component *createHighlighterComponent() override
        {
            return new SyllabeButton(this->syllable,
                MetronomeScheme::getNextSyllable(this->syllable));
        }

        Syllable syllable;
        Syllable nextSyllable; // to display what changes when tapped

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

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyllabeButton)
    };

    OwnedArray<SyllabeButton> buttons;
    UniquePointer<PlayButton> playButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetronomeEditor)
};
