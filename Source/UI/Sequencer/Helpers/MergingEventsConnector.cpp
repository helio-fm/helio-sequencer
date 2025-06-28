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
#include "MergingEventsConnector.h"
#include "PianoClipComponent.h"
#include "NoteComponent.h"
#include "ColourIDs.h"

//===----------------------------------------------------------------------===//
// Base
//===----------------------------------------------------------------------===//

MergingEventsConnector::MergingEventsConnector(SafePointer<Component> sourceComponent, Point<float> startPosition) :
    sourceComponent(sourceComponent),
    startPositionAbs(startPosition),
    endPositionAbs(startPosition) {}

MergingEventsConnector::~MergingEventsConnector() = default;

void MergingEventsConnector::setEndPosition(Point<float> position)
{
    this->endPositionAbs = position;
    this->updateBounds();
    this->repaint();
}

Component *MergingEventsConnector::getSourceComponent() const noexcept
{
    return this->sourceComponent;
}

void MergingEventsConnector::setTargetComponent(SafePointer<Component> component) noexcept
{
    this->targetComponent = component;
    this->repaint();
}

Component *MergingEventsConnector::getTargetComponent() const noexcept
{
    return this->targetComponent;
}

Point<float> MergingEventsConnector::getStartPosition() const noexcept
{
    const auto *parent = this->getParentComponent();
    jassert(parent != nullptr);

    return parent->getLocalBounds().toFloat().getRelativePoint(this->startPositionAbs.x, this->startPositionAbs.y);
}

Point<float> MergingEventsConnector::getEndPosition() const noexcept
{
    const auto *parent = this->getParentComponent();
    jassert(parent != nullptr);

    return parent->getLocalBounds().toFloat().getRelativePoint(this->endPositionAbs.x, this->endPositionAbs.y);
}

void MergingEventsConnector::parentSizeChanged()
{
    this->updateBounds();
    this->repaint();
    Component::parentSizeChanged();
}

void MergingEventsConnector::updateBounds()
{
    const Rectangle<int> bounds(this->getStartPosition().toInt(), this->getEndPosition().toInt());
    // DBG("  x: " + String(bounds.getX()) + ", y:" + String(bounds.getY()));
    // DBG("  w: " + String(bounds.getWidth()) + ", h:" + String(bounds.getHeight()));
    this->setBounds(bounds.expanded(20));
}

//===----------------------------------------------------------------------===//
// MergingNotesConnector
//===----------------------------------------------------------------------===//

MergingNotesConnector::MergingNotesConnector(SafePointer<Component> sourceComponent, Point<float> startPosition) :
    MergingEventsConnector(sourceComponent, startPosition)
{
    if (auto *nc = dynamic_cast<NoteComponent *>(sourceComponent.getComponent()))
    {
        this->colour = nc->getNote().getTrackColour().
            interpolatedWith(findDefaultColour(ColourIDs::Roll::cuttingGuide), 0.4f).
            brighter(0.1f);
    }
}

bool MergingNotesConnector::canMergeInto(SafePointer<Component> component)
{
    if (component == this->sourceComponent)
    {
        return false; // can't merge a note into itself
    }

    auto *sourceNC = dynamic_cast<NoteComponent *>(this->sourceComponent.getComponent());
    auto *targetNC = dynamic_cast<NoteComponent *>(component.getComponent());
    if (sourceNC == nullptr || targetNC == nullptr)
    {
        return false;
    }

    return sourceNC->getClip().getId() == targetNC->getClip().getId() &&
        sourceNC->getClip().getPattern() == targetNC->getClip().getPattern() &&
        sourceNC->getNote().getKey() == targetNC->getNote().getKey();
}

void MergingNotesConnector::paint(Graphics &g)
{
    const auto topLeft = this->getBounds().getTopLeft().toFloat();
    const auto dragStart = this->getStartPosition() - topLeft;
    const auto dragEnd = this->getEndPosition() - topLeft;

    // assuming we have the same parent with target components
    jassert(this->getParentComponent() == this->sourceComponent->getParentComponent());

    const auto sourceNoteBounds = this->sourceComponent->getBounds().toFloat() - topLeft;

    const bool backwardDirection = dragStart.x > dragEnd.x;
    const auto sourceX = backwardDirection ?
        sourceNoteBounds.getX() : sourceNoteBounds.getRight();

    if ((backwardDirection && sourceX < dragEnd.x) ||
        (!backwardDirection && sourceX > dragEnd.x))
    {
        return;
    }

    const bool readyToMerge = this->targetComponent != nullptr;
    const auto targetNoteBounds = readyToMerge ?
        this->targetComponent->getBounds().toFloat() - topLeft : Rectangle<float>();

    const auto targetX = readyToMerge ?
        (backwardDirection ? targetNoteBounds.getRight() : targetNoteBounds.getX()) :
        dragEnd.x;

    const auto noteHeight = sourceNoteBounds.getHeight();
    const auto drawHeight = jmax(6.f, noteHeight - 6.f);
    const auto drawCentreY = sourceNoteBounds.getCentreY();
    const auto drawY = drawCentreY - drawHeight / 2;

    if (readyToMerge)
    {
        g.setColour(this->colour);
        const auto drawStartX = jmin(sourceX, targetX);
        const auto drawEndX = jmax(sourceX, targetX) - 1.f;
        if (drawStartX < drawEndX)
        {
            g.fillRect(drawStartX, drawY, drawEndX - drawStartX, drawHeight);

            g.fillRect(drawStartX, drawY - 1.f, 1.f, drawHeight + 2.f);
            g.fillRect(drawEndX, drawY - 1.f, 1.f, drawHeight + 2.f);

            g.setColour(Colours::black.withAlpha(0.5f));
            g.fillRect(drawStartX - 1.f, drawY - 1.f, 1.f, drawHeight + 2.f);
            g.fillRect(drawEndX + 1.f, drawY - 1.f, 1.f, drawHeight + 2.f);
        }
        else
        {
            g.fillRect(drawStartX - 2.f, drawCentreY - noteHeight / 2.f,
                drawEndX - drawStartX + 5.f, noteHeight);
        }
    }
    else
    {
        g.setColour(Colours::black.withAlpha(0.4f));
        g.drawArrow(Line<float>(sourceX, drawCentreY, targetX, drawCentreY), drawHeight + 1.f, noteHeight + 1.f, 6);

        g.setColour(this->colour);
        g.drawArrow(Line<float>(sourceX, drawCentreY, targetX, drawCentreY), drawHeight - 1.f, noteHeight, 6);
    }

    //#if DEBUG
    //g.setColour(Colours::blanchedalmond.withAlpha(0.1f));
    //g.fillAll();
    //#endif
}

//===----------------------------------------------------------------------===//
// MergingClipsConnector
//===----------------------------------------------------------------------===//

MergingClipsConnector::MergingClipsConnector(SafePointer<Component> sourceComponent, Point<float> startPosition) :
    MergingEventsConnector(sourceComponent, startPosition)
{
    if (auto *clipComponent = dynamic_cast<ClipComponent *>(sourceComponent.getComponent()))
    {
        this->startColour = clipComponent->getClip().getTrackColour().
            interpolatedWith(findDefaultColour(ColourIDs::Roll::cuttingGuide), 0.5f);

        this->endColour = this->startColour;
    }
}

void MergingClipsConnector::setTargetComponent(SafePointer<Component> component) noexcept
{
    if (auto *clipComponent = dynamic_cast<ClipComponent *>(component.getComponent()))
    {
        this->endColour = clipComponent->getClip().getTrackColour().
            interpolatedWith(findDefaultColour(ColourIDs::Roll::cuttingGuide), 0.5f);
    }
    else
    {
        this->endColour = this->startColour;
    }

    MergingEventsConnector::setTargetComponent(component);
}

bool MergingClipsConnector::canMergeInto(SafePointer<Component> component)
{
    if (component == this->sourceComponent)
    {
        return false; // can't merge a clip into itself
    }

    // piano clips can only be merged with piano clips
    if (auto *pianoCC = dynamic_cast<PianoClipComponent *>(this->sourceComponent.getComponent()))
    {
        return nullptr != dynamic_cast<PianoClipComponent *>(component.getComponent());
    }

    // assuming the source is automation clip here,
    // let's simply check if controller numbers are the same
    auto *sourceCC = dynamic_cast<ClipComponent *>(this->sourceComponent.getComponent());
    auto *targetCC = dynamic_cast<ClipComponent *>(component.getComponent());
    if (sourceCC == nullptr || targetCC == nullptr)
    {
        jassertfalse;
        return false;
    }

    return sourceCC->getClip().getTrackControllerNumber() ==
           targetCC->getClip().getTrackControllerNumber();
}

void MergingClipsConnector::paint(Graphics &g)
{
    const auto topLeft = this->getBounds().getTopLeft().toFloat();
    const auto start = this->getStartPosition() - topLeft;
    const auto end = this->getEndPosition() - topLeft;

    const auto distanceSqr = end.getDistanceSquaredFrom(start);
    const auto brushSize = jmin(distanceSqr / 100.f, 7.f);

    g.setGradientFill(ColourGradient(this->startColour, start, this->endColour, end, false));
    g.drawArrow(Line<float>(start, end), 1.f, brushSize, brushSize * 2.5f);

    g.fillRect(start.x - brushSize, start.y, brushSize * 2.f, 1.f);
    g.fillRect(start.x, start.y - brushSize, 1.f, brushSize * 2.f);

    //#if DEBUG
    //g.setColour(Colours::blanchedalmond.withAlpha(0.1f));
    //g.fillAll();
    //#endif
}
