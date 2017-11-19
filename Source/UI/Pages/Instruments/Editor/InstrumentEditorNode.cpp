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
#include "InstrumentEditorNode.h"
#include "Instrument.h"
#include "InstrumentEditor.h"
#include "InstrumentEditorPin.h"
#include "PluginWindow.h"

#include "App.h"
#include "Workspace.h"
#include "RootTreeItem.h"
#include "InstrumentTreeItem.h"
#include "AudioPluginTreeItem.h"

#if HELIO_DESKTOP
#   define AUDIO_PLUGIN_RUNS_IN_SEPARATE_WINDOW 1
#   define CHANNELS_NUMBER_LIMIT 12
#   define PIN_SIZE (16)
#elif HELIO_MOBILE
#   define AUDIO_PLUGIN_RUNS_IN_SEPARATE_WINDOW 0
#   define CHANNELS_NUMBER_LIMIT 6
#   define PIN_SIZE (25)
#endif

InstrumentEditorNode::InstrumentEditorNode(Instrument &graph, const uint32 filterID) :
    instrument(graph),
    filterID(filterID),
    numInputs(0),
    numOutputs(0),
    pinSize(PIN_SIZE),
    font(Font(Font::getDefaultSansSerifFontName(), 21.f, Font::plain)),
    numIns(0),
    numOuts(0)
{
    this->setWantsKeyboardFocus(false);
    this->setMouseCursor(MouseCursor::PointingHandCursor);
    this->setSize(250, 100);
}

InstrumentEditorNode::~InstrumentEditorNode()
{
    this->deleteAllChildren();
}

void InstrumentEditorNode::mouseDown(const MouseEvent &e)
{
    originalPos = this->localPointToGlobal(Point<int>());
    this->toFront(false);
}

void InstrumentEditorNode::mouseDrag(const MouseEvent &e)
{
    //if (!e.mods.isPopupMenu())
    {
        Point<int> pos(originalPos + Point<int>(e.getDistanceFromDragStartX(), e.getDistanceFromDragStartY()));

        if (this->getParentComponent() != nullptr)
        { pos = this->getParentComponent()->getLocalPoint(nullptr, pos); }

        this->instrument.setNodePosition(this->filterID,
            (pos.getX() + getWidth() / 2) / static_cast<double>(this->getParentWidth()),
            (pos.getY() + getHeight() / 2) / static_cast<double>(this->getParentHeight()));

        this->getGraphPanel()->updateComponents();
        
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
    }
}

void InstrumentEditorNode::mouseUp(const MouseEvent &e)
{
    this->setMouseCursor(MouseCursor::PointingHandCursor);

    if (e.mouseWasClicked() && e.getDistanceFromDragStart() < 2)
    {
        if (this->instrument.isNodeStandardInputOrOutput(this->filterID))
        {
            return;
        }

        if (const AudioProcessorGraph::Node::Ptr f = this->instrument.getNodeForId(this->filterID))
        {
#if AUDIO_PLUGIN_RUNS_IN_SEPARATE_WINDOW

            if (PluginWindow *const w = PluginWindow::getWindowFor(f, false, false))
            {
                w->toFront(true);
            }

#else

            const auto instrumentTreeItems =
                App::Workspace().getTreeRoot()->
                findChildrenOfType<InstrumentTreeItem>();

            for (const auto instrumentTreeItem : instrumentTreeItems)
            {
                if (instrumentTreeItem->getInstrument() == &this->instrument)
                {
                    instrumentTreeItem->updateChildrenEditors();
                    if (TreeItem *audioPluginTreeItem =
                        instrumentTreeItem->findAudioPluginEditorForNodeId(this->filterID))
                    {
                        audioPluginTreeItem->setSelected(true, true);
                        return;
                    }
                }
            }

#endif
        }
    }
    else if (!e.mouseWasClicked())
    {
        this->getGraphPanel()->updateComponents();
    }
}

bool InstrumentEditorNode::hitTest(int x, int y)
{
    const int xCenter = this->getWidth() / 2;
    const int yCenter = this->getHeight() / 2;

    const int dx = x - xCenter;
    const int dy = y - yCenter;
    const int r = (this->getWidth() + this->getHeight()) / 4;

    return (dx * dx) + (dy * dy) < (r * r);
}

void InstrumentEditorNode::paint(Graphics &g)
{
    g.setGradientFill(ColourGradient(Colour(0x59ffffff),
                                     0.0f, static_cast<float>(getHeight()),
                                     Colour(0x30ffffff),
                                     static_cast<float>(getWidth()), 0.0f,
                                     true));

    g.fillEllipse(1.0f, 1.0f, static_cast<float>(getWidth() - 2), static_cast<float>(getHeight() - 2));

    g.setColour(Colour(0x5d000000));
    g.drawEllipse(1.0f, 1.0f, static_cast<float>(getWidth() - 2), static_cast<float>(getHeight() - 2), 0.500f);


    g.setColour(Colours::black.withAlpha(0.75f));
    g.setFont(font);
    g.drawFittedText(this->getName(), getLocalBounds().reduced(this->pinSize * 2, this->pinSize), Justification::centred, 3, 1.f);
}

void InstrumentEditorNode::resized()
{
    const int xCenter = this->getWidth() / 2;
    const int yCenter = this->getHeight() / 2;
    const int r = (this->getWidth() + this->getHeight()) / 4 - pinSize;

    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        if (InstrumentEditorPin *const pin = dynamic_cast <InstrumentEditorPin *>(getChildComponent(i)))
        {
            const int total = pin->isInput ? this->numIns : this->numOuts;
            const int index = (pin->index == Instrument::midiChannelNumber) ? (total - 1) : pin->index;
            const int pinSize2 = (this->pinSize / 2);

            const float dAngle = pin->isInput ?
                                  270.f - ((180.f / (total + 1.f)) * (1 + index)) :
                                  ((180.f / (total + 1.f)) * (1 + index)) - 90.f;

            const float rAngle = (dAngle / 180.f) * float_Pi;
            const float dx = cos(rAngle) * r;
            const float dy = sin(rAngle) * r;

            pin->setBounds(xCenter + int(dx) - pinSize2, yCenter + int(dy) - pinSize2, pinSize, pinSize);
        }
    }
}

void InstrumentEditorNode::getPinPos(const int index, const bool isInput, float &x, float &y)
{
    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        if (InstrumentEditorPin *const pc = dynamic_cast <InstrumentEditorPin *>(getChildComponent(i)))
        {
            if (pc->index == index && isInput == pc->isInput)
            {
                x = this->getX() + pc->getX() + pc->getWidth() * 0.5f;
                y = this->getY() + pc->getY() + pc->getHeight() * 0.5f;
                break;
            }
        }
    }
}

void InstrumentEditorNode::update()
{
    const AudioProcessorGraph::Node::Ptr f(this->instrument.getNodeForId(filterID));

    if (f == nullptr)
    {
        delete this;
        return;
    }

    this->numIns = jmin(f->getProcessor()->getTotalNumInputChannels(), CHANNELS_NUMBER_LIMIT);

    if (f->getProcessor()->acceptsMidi())
    { ++this->numIns; }

    this->numOuts = jmin(f->getProcessor()->getTotalNumOutputChannels(), CHANNELS_NUMBER_LIMIT);

    if (f->getProcessor()->producesMidi())
    { ++this->numOuts; }

    int w = 190;
    //int h = jmax(80, (jmax(this->numIns, this->numOuts) + 1) * 20);

    // a hack needed for "Audio Input", "Audio Output" etc nodes:
    const String translatedName = TRANS(f->getProcessor()->getName());

    const int textWidth = font.getStringWidth(translatedName);
    w = jmax(w, 16 + jmin(textWidth, 300));

    //if (textWidth > 300) { h = 100; }

    this->setSize(w, w);

    this->setName(translatedName);

    {
        double x, y;
        this->instrument.getNodePosition(filterID, x, y);
        setCentreRelative(static_cast<float>( x), static_cast<float>( y));
    }

    if (this->numIns != numInputs || this->numOuts != numOutputs)
    {
        numInputs = this->numIns;
        numOutputs = this->numOuts;

        this->deleteAllChildren();

        int i;

        for (i = 0; i < this->numIns; ++i)
        { this->addAndMakeVisible(new InstrumentEditorPin(this->instrument, filterID, i, true)); }

        if (f->getProcessor()->acceptsMidi())
        { this->addAndMakeVisible(new InstrumentEditorPin(this->instrument, filterID, Instrument::midiChannelNumber, true)); }

        for (i = 0; i < this->numOuts; ++i)
        { this->addAndMakeVisible(new InstrumentEditorPin(this->instrument, filterID, i, false)); }

        if (f->getProcessor()->producesMidi())
        { this->addAndMakeVisible(new InstrumentEditorPin(this->instrument, filterID, Instrument::midiChannelNumber, false)); }

        this->resized();
    }
}

InstrumentEditor *InstrumentEditorNode::getGraphPanel() const noexcept
{
    return this->findParentComponentOfClass<InstrumentEditor>();
}

