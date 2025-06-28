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
#include "InstrumentComponent.h"
#include "InstrumentEditor.h"
#include "InstrumentEditorPin.h"
#include "PluginWindow.h"
#include "HelioTheme.h"

InstrumentComponent::InstrumentComponent(WeakReference<Instrument> instrument,
    AudioProcessorGraph::NodeID nodeId) :
    instrument(instrument),
    nodeId(nodeId)
{
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
}

InstrumentComponent::~InstrumentComponent() = default;

void InstrumentComponent::mouseDown(const MouseEvent &e)
{
    this->originalPos = this->localPointToGlobal(Point<int>());
    this->toFront(false);
}

void InstrumentComponent::mouseDrag(const MouseEvent &e)
{
    auto pos = this->originalPos +
        Point<int>(e.getDistanceFromDragStartX(), e.getDistanceFromDragStartY());

    if (this->getParentComponent() != nullptr)
    {
        pos = this->getParentComponent()->getLocalPoint(nullptr, pos);
    }

    this->instrument->setNodePosition(this->nodeId,
        double(pos.getX() + this->getWidth() / 2) / double(this->getParentWidth()),
        double(pos.getY() + this->getHeight() / 2) / double(this->getParentHeight()));

    this->getParentEditor()->updateComponents();
}

void InstrumentComponent::mouseUp(const MouseEvent &e)
{
    if (e.source.hasMovedSignificantlySincePressed())
    {
        this->getParentEditor()->updateComponents();
        return;
    }

#if PLATFORM_DESKTOP
    if (e.mods.isRightButtonDown() ||
        e.mods.isAnyModifierKeyDown() ||
        this->instrument->isNodeStandardIOProcessor(this->nodeId))
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

#elif PLATFORM_MOBILE
    this->getParentEditor()->selectNode(this->nodeId, e);
#endif
}

bool InstrumentComponent::hitTest(int x, int y)
{
    const int dx = x - (this->getWidth() / 2);
    const int dy = y - (this->getHeight() / 2);
    const int r = (this->getWidth() + this->getHeight()) / 4;
    return (dx * dx) + (dy * dy) < (r * r);
}

void InstrumentComponent::paint(Graphics &g)
{
    const auto w = float(this->getWidth());
    const auto h = float(this->getHeight());

    g.setColour(this->fillColour);
    g.fillEllipse(1.f, 1.f, w - 2.f, h - 2.f);

    g.setColour(this->outlineColour);
    g.drawEllipse(1.f, 1.f, w - 2.f, h - 2.f, 0.75f);

    if (this->isSelected)
    {
        g.drawEllipse(5.f, 5.f, w - 10.f, h - 10.f, 1.5f);
    }

    g.setFont(this->font);
    g.setColour(this->textColour);

    const auto area = this->getLocalBounds().
        reduced(this->pinSize, this->pinSize / 2);

    HelioTheme::drawFittedText(g,
        this->getName(),
        area.getX(), area.getY(), area.getWidth(), area.getHeight(),
        Justification::centred, 3, 1.f);
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
        UniquePointer<Component> deleter(this);
        return;
    }

#if PLATFORM_DESKTOP
    constexpr auto maxChannels = 12;
#elif PLATFORM_MOBILE
    constexpr auto maxChannels = 6;
#endif

    const int newNumInputs =
        jmin(node->getProcessor()->getTotalNumInputChannels(), maxChannels) +
        (node->getProcessor()->acceptsMidi() ? 1 : 0);

    const int newNumOutputs =
        jmin(node->getProcessor()->getTotalNumOutputChannels(), maxChannels) +
        (node->getProcessor()->producesMidi() ? 1 : 0);

    // a hack needed for "Audio Input", "Audio Output" etc nodes:
    const String translatedName = TRANS(node->getProcessor()->getName());
    this->setName(translatedName);

    const int textWidth = this->font.getStringWidth(translatedName);
    const auto smallScreenMode = App::isRunningOnPhone();
    const auto minSize = smallScreenMode ? 130 : 180;
    const auto maxSize = smallScreenMode ? 170 : 300;
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

        this->pinComponents.clearQuick(true);

        for (int i = 0; i < this->numInputs; ++i)
        {
            auto pin = make<InstrumentEditorPin>(nodeId, i, true);
            this->addAndMakeVisible(pin.get());
            this->pinComponents.add(move(pin));
        }

        if (node->getProcessor()->acceptsMidi())
        {
            auto pin = make<InstrumentEditorPin>(nodeId, Instrument::midiChannelNumber, true);
            this->addAndMakeVisible(pin.get());
            this->pinComponents.add(move(pin));
        }

        for (int i = 0; i < this->numOutputs; ++i)
        {
            auto pin = make<InstrumentEditorPin>(nodeId, i, false);
            this->addAndMakeVisible(pin.get());
            this->pinComponents.add(move(pin));
        }

        if (node->getProcessor()->producesMidi())
        {
            auto pin = make<InstrumentEditorPin>(nodeId, Instrument::midiChannelNumber, false);
            this->addAndMakeVisible(pin.get());
            this->pinComponents.add(move(pin));
        }

        this->resized();
    }
}

InstrumentEditor *InstrumentComponent::getParentEditor() const noexcept
{
    return this->findParentComponentOfClass<InstrumentEditor>();
}

