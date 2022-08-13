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
#include "InstrumentComponent.h"
#include "InstrumentEditor.h"
#include "InstrumentEditorPin.h"
#include "PluginWindow.h"

#if PLATFORM_DESKTOP
#   define CHANNELS_NUMBER_LIMIT 12
#   define DEFAULT_PIN_SIZE (18)
#elif PLATFORM_MOBILE
#   define CHANNELS_NUMBER_LIMIT 6
#   define DEFAULT_PIN_SIZE (25)
#endif

InstrumentComponent::InstrumentComponent(WeakReference<Instrument> instrument,
    AudioProcessorGraph::NodeID nodeId) :
    instrument(instrument),
    nodeId(nodeId),
    pinSize(DEFAULT_PIN_SIZE)
{
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->setMouseCursor(MouseCursor::PointingHandCursor);
}

InstrumentComponent::~InstrumentComponent()
{
    this->deleteAllChildren();
}

void InstrumentComponent::mouseDown(const MouseEvent &e)
{
    this->originalPos = this->localPointToGlobal(Point<int>());
    this->toFront(false);
}

void InstrumentComponent::mouseDrag(const MouseEvent &e)
{
    Point<int> pos(this->originalPos + Point<int>(e.getDistanceFromDragStartX(), e.getDistanceFromDragStartY()));

    if (this->getParentComponent() != nullptr)
    {
        pos = this->getParentComponent()->getLocalPoint(nullptr, pos);
    }

    this->instrument->setNodePosition(this->nodeId,
        (pos.getX() + getWidth() / 2) / static_cast<double>(this->getParentWidth()),
        (pos.getY() + getHeight() / 2) / static_cast<double>(this->getParentHeight()));

    this->getParentEditor()->updateComponents();
    this->setMouseCursor(MouseCursor::DraggingHandCursor);
}

void InstrumentComponent::mouseUp(const MouseEvent &e)
{
    this->setMouseCursor(MouseCursor::PointingHandCursor);

    if (e.getOffsetFromDragStart().isOrigin())
    {
        if (this->instrument->isNodeStandardIOProcessor(this->nodeId) ||
            e.mods.isRightButtonDown() || e.mods.isAnyModifierKeyDown())
        {
            this->getParentEditor()->selectNode(this->nodeId, e);
            return;
        }

        this->getParentEditor()->deselectAllNodes();

        if (const auto node = this->instrument->getNodeForId(this->nodeId))
        {
            if (!PluginWindow::showWindowFor(node))
            {
                this->getParentEditor()->selectNode(this->nodeId, e);
            }
        }
    }
    else
    {
        this->getParentEditor()->updateComponents();
    }
}

bool InstrumentComponent::hitTest(int x, int y)
{
    const int xCenter = this->getWidth() / 2;
    const int yCenter = this->getHeight() / 2;

    const int dx = x - xCenter;
    const int dy = y - yCenter;
    const int r = (this->getWidth() + this->getHeight()) / 4;

    return (dx * dx) + (dy * dy) < (r * r);
}

void InstrumentComponent::paint(Graphics &g)
{
    const auto w = float(this->getWidth());
    const auto h = float(this->getHeight());

    g.setGradientFill(ColourGradient(Colour(0x59ffffff), 0.0f, h,
        Colour(0x30ffffff), w, 0.0f, true));

    g.fillEllipse(1.0f, 1.0f, w - 2.f, h - 2.f);

    g.setColour(Colour(0x5d000000));
    g.drawEllipse(1.0f, 1.0f, w - 2.f, h - 2.f, 0.5f);

    if (this->isSelected)
    {
        g.drawEllipse(5.0f, 5.0f, w - 10.f, h - 10.f, 1.5f);
    }

    g.setFont(this->font);
    g.setColour(Colours::black.withAlpha(0.75f));
    g.drawFittedText(this->getName(), getLocalBounds().reduced(this->pinSize * 2, this->pinSize), Justification::centred, 3, 1.f);
}

void InstrumentComponent::resized()
{
    const int xCenter = this->getWidth() / 2;
    const int yCenter = this->getHeight() / 2;
    const int r = (this->getWidth() + this->getHeight()) / 4 - pinSize;

    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        if (auto pin = dynamic_cast<InstrumentEditorPin *>(this->getChildComponent(i)))
        {
            const int total = pin->isInput ? this->numInputs : this->numOutputs;
            const int index = (pin->index == Instrument::midiChannelNumber) ? (total - 1) : pin->index;
            const int pinSize2 = (this->pinSize / 2);

            const float dAngle = pin->isInput ?
                270.f - ((180.f / (total + 1.f)) * (1 + index)) :
                ((180.f / (total + 1.f)) * (1 + index)) - 90.f;

            const float rAngle = (dAngle / 180.f) * MathConstants<float>::pi;
            const float dx = cosf(rAngle) * r;
            const float dy = sinf(rAngle) * r;

            pin->setBounds(xCenter + int(dx) - pinSize2, yCenter + int(dy) - pinSize2, pinSize, pinSize);
        }
    }
}

void InstrumentComponent::getPinPos(const int index, const bool isInput, float &x, float &y)
{
    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        if (auto pc = dynamic_cast<InstrumentEditorPin *>(this->getChildComponent(i)))
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

void InstrumentComponent::setSelected(bool selected)
{
    this->isSelected = selected;
    this->repaint();
}

void InstrumentComponent::update()
{
    const auto node = this->instrument->getNodeForId(nodeId);
    if (node == nullptr)
    {
        delete this;
        return;
    }

    const int newNumInputs =
        jmin(node->getProcessor()->getTotalNumInputChannels(), CHANNELS_NUMBER_LIMIT) +
        (node->getProcessor()->acceptsMidi() ? 1 : 0);

    const int newNumOutputs =
        jmin(node->getProcessor()->getTotalNumOutputChannels(), CHANNELS_NUMBER_LIMIT) +
        (node->getProcessor()->producesMidi() ? 1 : 0);

    // a hack needed for "Audio Input", "Audio Output" etc nodes:
    const String translatedName = TRANS(node->getProcessor()->getName());
    this->setName(translatedName);

    const int textWidth = this->font.getStringWidth(translatedName);
    const auto smallScreenMode = App::isRunningOnPhone();
    const auto minSize = smallScreenMode ? 120 : 180;
    const auto maxSize = smallScreenMode ? 150 : 300;
    const auto size = jlimit(minSize, maxSize, textWidth);
    this->setSize(size, size);

    {
        double x = 0.0, y = 0.0;
        this->instrument->getNodePosition(nodeId, x, y);
        this->setCentreRelative(float(x), float(y));
    }

    if (this->numInputs != newNumInputs || this->numOutputs != newNumOutputs)
    {
        this->numInputs = newNumInputs;
        this->numOutputs = newNumOutputs;

        this->deleteAllChildren();

        int i;

        for (i = 0; i < this->numInputs; ++i)
        {
            this->addAndMakeVisible(new InstrumentEditorPin(nodeId, i, true));
        }

        if (node->getProcessor()->acceptsMidi())
        {
            this->addAndMakeVisible(new InstrumentEditorPin(nodeId, Instrument::midiChannelNumber, true));
        }

        for (i = 0; i < this->numOutputs; ++i)
        {
            this->addAndMakeVisible(new InstrumentEditorPin(nodeId, i, false));
        }

        if (node->getProcessor()->producesMidi())
        {
            this->addAndMakeVisible(new InstrumentEditorPin(nodeId, Instrument::midiChannelNumber, false));
        }

        this->resized();
    }
}

InstrumentEditor *InstrumentComponent::getParentEditor() const noexcept
{
    return this->findParentComponentOfClass<InstrumentEditor>();
}

