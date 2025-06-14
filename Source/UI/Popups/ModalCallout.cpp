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
#include "ModalCallout.h"
#include "MainLayout.h"
#include "SequencerLayout.h"
#include "RollBase.h"
#include "ColourIDs.h"

static int kClickCounterOnPopupClose = 0;
static int kClickCounterOnPopupStart = 0;

ModalCallout::ModalCallout(Component *newComponent, SafePointer<Component> pointAtComponent, bool shouldAlignToMouse) :
    contentComponent(newComponent),
    targetComponent(pointAtComponent),
    alignsToMouse(shouldAlignToMouse)
{
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);

    kClickCounterOnPopupStart = Desktop::getInstance().getMouseButtonClickCounter();
        
    this->addAndMakeVisible(this->contentComponent.get());
}

ModalCallout::~ModalCallout()
{
    kClickCounterOnPopupClose = Desktop::getInstance().getMouseButtonClickCounter();
}

int ModalCallout::getBorderSize() const noexcept
{
    return int(ModalCallout::arrowSize);
}

void ModalCallout::fadeIn()
{
    App::animateComponent(this,
        this->getBounds(), 1.f, Globals::UI::fadeInLong, false, 0.0, 0.0);
}

void ModalCallout::fadeOut()
{
    const int reduceBy = 20;
    const auto offset = this->targetPoint - this->getBounds().getCentre().toFloat();
    const auto offsetNormalized = (offset / offset.getDistanceFromOrigin() * reduceBy).toInt();
    
    App::animateComponent(this,
        this->getBounds().reduced(reduceBy).translated(offsetNormalized.getX(), offsetNormalized.getY()),
        0.f, Globals::UI::fadeOutLong, true, 0.0, 0.0);
}

//===----------------------------------------------------------------------===//
// Static
//===----------------------------------------------------------------------===//

void ModalCallout::emit(Component *newComponent, Component *pointAtComponent, bool alignsToMousePosition)
{
    App::showModalComponent(make<ModalCallout>(newComponent, pointAtComponent, alignsToMousePosition));
}

int ModalCallout::numClicksSinceLastStartedPopup()
{
    return Desktop::getInstance().getMouseButtonClickCounter() - kClickCounterOnPopupStart;
}

int ModalCallout::numClicksSinceLastClosedPopup()
{
    return Desktop::getInstance().getMouseButtonClickCounter() - kClickCounterOnPopupClose;
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void ModalCallout::paint(Graphics& g)
{
    g.setColour(findDefaultColour(ColourIDs::Callout::fill));
    g.fillPath(this->outline);

    g.setColour(findDefaultColour(ColourIDs::Callout::frame));
    g.strokePath(this->outline, PathStrokeType(1.f));
}

void ModalCallout::resized()
{
    const int borderSpace = this->getBorderSize();
    this->contentComponent->setTopLeftPosition(borderSpace, borderSpace);
    this->updateShape();
}

void ModalCallout::moved()
{
    this->updateShape();
}

void ModalCallout::parentHierarchyChanged()
{
    if (auto *parent = this->getParentComponent())
    {
        const auto b = parent->getScreenBounds().toFloat();

#if PLATFORM_DESKTOP
        const auto p = Desktop::getInstance().getMainMouseSource().getScreenPosition() - b.getPosition();
#elif PLATFORM_MOBILE
        const auto p = Desktop::getInstance().getMainMouseSource().getLastMouseDownPosition() - b.getPosition();
#endif

        this->clickPointAbs = Point<float>(p.getX() / b.getWidth(), p.getY() / b.getHeight())
            .transformedBy(this->getTransform().inverted());
        
        this->findTargetPointAndUpdateBounds();
    }
}

void ModalCallout::parentSizeChanged()
{
    this->findTargetPointAndUpdateBounds();
}

void ModalCallout::childBoundsChanged(Component *)
{
    this->pointToAndFit(this->areaToPointTo, this->areaToFitIn);
}

bool ModalCallout::hitTest(int x, int y)
{
    return this->outline.contains(float(x), float(y));
}

void ModalCallout::inputAttemptWhenModal()
{
    // hack warning:
    // when you rclick/tap outside of the callout to hide it and start dragging the roll immediately,
    // JUCE never sends the mouseDown event to the roll because the modal callout is still showing,
    // and after the callout is dismissed, dragging continues with incorrect anchor
    // and the viewport position jumps away unpredictably; this check compensates for that:
    if (auto *sequencer = dynamic_cast<SequencerLayout *>(App::Layout().findChildWithID(ComponentIDs::sequencerLayoutId)))
    {
        if (auto *roll = sequencer->getRoll())
        {
            roll->resetDraggingAnchors();
        }
    }

    this->postCommandMessage(CommandIDs::DismissCallout);
}

void ModalCallout::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::DismissCallout)
    {
        this->dismiss();
    }
}

void ModalCallout::dismiss()
{
    this->fadeOut();
    delete this;
}

bool ModalCallout::keyPressed(const KeyPress &key)
{
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        // give a chance to hosted component to react to escape key:
        this->contentComponent->postCommandMessage(CommandIDs::Cancel);
        this->inputAttemptWhenModal();
        return true;
    }
    
    return true;
}

void ModalCallout::findTargetPointAndUpdateBounds()
{
    const auto pageBounds = App::Layout().getBoundsForPopups();

    if (this->alignsToMouse)
    {
        jassert(this->getParentComponent() != nullptr);
        const auto b = this->getParentComponent()->getBounds();
        Rectangle<int> clickBounds(int(b.getWidth() * this->clickPointAbs.getX()),
            int(b.getHeight() * this->clickPointAbs.getY()), 0, 0);
        const auto pointBounds = clickBounds.constrainedWithin(pageBounds);
        this->pointToAndFit(pointBounds, pageBounds);
    }
    else
    {
        auto positionInWorkspace = App::Layout().getLocalPoint(this->targetComponent, Point<int>(0, 0));
        Rectangle<int> topLevelBounds(positionInWorkspace.x, positionInWorkspace.y,
            this->targetComponent->getWidth(), this->targetComponent->getHeight());
        this->pointToAndFit(topLevelBounds, pageBounds);
    }
}

void ModalCallout::pointToAndFit(const Rectangle<int> &newAreaToPointTo, const Rectangle<int> &newAreaToFitIn)
{
    this->areaToPointTo = newAreaToPointTo;
    this->areaToFitIn = newAreaToFitIn;
    
    const int borderSpace = this->getBorderSize();

    Rectangle<int> newBounds(this->contentComponent->getWidth() + borderSpace * 2,
        this->contentComponent->getHeight() + borderSpace * 2);

    const int hw = (newBounds.getWidth() / 2);
    const int hh = (newBounds.getHeight() / 2);
    const float hwReduced = float(hw - borderSpace * 2);
    const float hhReduced = float(hh - borderSpace * 2);
    const float arrowIndent = borderSpace - arrowSize;
    
    Point<float> targets[4] =
    {
        Point<float>(float(newAreaToPointTo.getCentreX()), float(newAreaToPointTo.getBottom())),
        Point<float>(float(newAreaToPointTo.getRight()), float(newAreaToPointTo.getCentreY())),
        Point<float>(float(newAreaToPointTo.getX()), float(newAreaToPointTo.getCentreY())),
        Point<float>(float(newAreaToPointTo.getCentreX()), float(newAreaToPointTo.getY()))
    };
    
    Line<float> lines[4] =
    {
        Line<float>(targets[0].translated(-hwReduced, hh - arrowIndent), targets[0].translated(hwReduced, hh - arrowIndent)),
        Line<float>(targets[1].translated(hw - arrowIndent, -hhReduced), targets[1].translated(hw - arrowIndent, hhReduced)),
        Line<float>(targets[2].translated(-(hw - arrowIndent), -hhReduced), targets[2].translated(-(hw - arrowIndent), hhReduced)),
        Line<float>(targets[3].translated(-hwReduced, -(hh - arrowIndent)), targets[3].translated(hwReduced, -(hh - arrowIndent)))
    };
    
    const auto centrePointArea = newAreaToFitIn.reduced(hw, hh).toFloat();
    const auto targetCentre = newAreaToPointTo.getCentre().toFloat();
    
    float nearest = 1.0e9f;
    
    for (int i = 0; i < 4; ++i)
    {
        Line<float> constrainedLine(centrePointArea.getConstrainedPoint(lines[i].getStart()),
            centrePointArea.getConstrainedPoint(lines[i].getEnd()));
        
        const auto centre = constrainedLine.findNearestPointTo(targetCentre);
        auto distanceFromCentre = centre.getDistanceFrom(targets[i]);
        
        if (!centrePointArea.intersects(lines[i]))
        {
            distanceFromCentre += 1000.f;
        }
        
        if (distanceFromCentre < nearest)
        {
            nearest = distanceFromCentre;
            this->targetPoint = targets[i];
            newBounds.setPosition(int(centre.x - hw), int(centre.y - hh));
        }
    }
    
    this->setBounds(newBounds);
}

void ModalCallout::updateShape()
{
    this->repaint();
    this->outline.clear();
    
    constexpr auto innerBorderPadding = 1.f;
    const auto bodyArea = this->contentComponent->getBounds()
        .toFloat().expanded(innerBorderPadding);
    const auto maximumArea = this->getLocalBounds().toFloat();
    const auto arrowTip = this->targetPoint - this->getPosition().toFloat();
    
    this->outline.addBubble(bodyArea,
        maximumArea, arrowTip, 1.f, ModalCallout::arrowSize);
}
