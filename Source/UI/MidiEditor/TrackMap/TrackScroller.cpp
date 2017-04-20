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
#include "TrackScroller.h"
#include "TrackScrollerScreen.h"
#include "TransportIndicator.h"
#include "Transport.h"
#include "MidiRoll.h"
#include "PanelBackgroundC.h"
#include "Origami.h"

#include "App.h"

#include "ProjectTreeItem.h"
#include "PianoTrackMap.h"
#include <float.h>


TrackScroller::TrackScroller(Transport &transportRef,
                             MidiRoll &rollRef) :
    transport(transportRef),
    roll(rollRef),
    mapShouldGetStretched(true)
{
    this->background = new PanelBackgroundC();
    this->addAndMakeVisible(this->background);

    this->setOpaque(true);

    this->indicator = new TransportIndicator(this->roll, this->transport);
//    this->trackImage->addAndMakeVisible(this->indicator);
    
    this->helperRectangle = new HorizontalDragHelper(*this);
    this->addAndMakeVisible(this->helperRectangle);
    
    this->screenRange = new TrackScrollerScreen(*this);
    this->addAndMakeVisible(this->screenRange);
    
    this->resized();
}

TrackScroller::~TrackScroller()
{
    this->background = nullptr;
    this->disconnectIndicator();
//    this->trackImage->removeChildComponent(this->indicator);
}

void TrackScroller::addOwnedMap(Component *newTrackMap, bool shouldBringToFront)
{
    this->trackMaps.add(newTrackMap);
    this->addAndMakeVisible(newTrackMap);
    
    // всегда прикреплен к 1й карте
    if (this->trackMaps.size() == 1)
    {
        this->disconnectIndicator();
        newTrackMap->addAndMakeVisible(this->indicator);
    }
    
    // fade-in if not the first child
    if (this->trackMaps.size() > 1)
    {
        newTrackMap->setVisible(false);
        this->fader.fadeIn(newTrackMap, 200);
    }
    
    if (shouldBringToFront)
    {
        this->helperRectangle->toFront(false);
        this->screenRange->toFront(false);
        newTrackMap->toFront(false);
    }
    else
    {
        newTrackMap->toFront(false);
        this->helperRectangle->toFront(false);
        this->screenRange->toFront(false);
    }
    
    this->resized();
}

void TrackScroller::removeOwnedMap(Component *existingTrackMap)
{
    if (this->trackMaps.contains(existingTrackMap))
    {
        // fadeout causes weird OpenGL errors for the large maps:
        //this->fader.fadeOut(existingTrackMap, 150);
        this->removeChildComponent(existingTrackMap);
        this->trackMaps.removeObject(existingTrackMap);

        this->resized();
    }
}

void TrackScroller::disconnectIndicator()
{
    if (this->indicator->getParentComponent())
    {
        this->indicator->getParentComponent()->removeChildComponent(this->indicator);
    }
}


//===----------------------------------------------------------------------===//
// TrackScroller
//===----------------------------------------------------------------------===//

void TrackScroller::horizontalDragByUser(Component *component, const Rectangle<int> &bounds)
{
    const Rectangle<float> &screenRangeBounds = this->screenRange->getRealBounds();
    this->screenRange->setRealBounds(screenRangeBounds.withX(float(component->getX())));
    this->xMoveByUser();
}

void TrackScroller::xyMoveByUser()
{
    const Rectangle<float> &screenRangeBounds = this->screenRange->getRealBounds();
    
    const float &mw = float(this->getWidth()) - screenRangeBounds.getWidth();
    const float &propX = screenRangeBounds.getTopLeft().getX() / mw;
    const float &mh = float(this->getHeight()) - screenRangeBounds.getHeight();
    const float &propY = screenRangeBounds.getTopLeft().getY() / mh;
    
    // fixes for headerheight delta
    const float &hh = float(MIDIROLL_HEADER_HEIGHT);
    const float &rollHeight = float(this->roll.getHeight());
    const float &propY2 = roundf(((rollHeight - hh) * propY) - hh) / rollHeight;
    this->roll.panProportionally(propX, propY2);
    
//    this->roll.panProportionally(propX, propY);
    
    const Rectangle<float> &p = this->getIndicatorBounds();
    const Rectangle<int> &hp = p.toType<int>();
    this->helperRectangle->setBounds(hp.withTop(0).withBottom(this->getHeight()));
    this->screenRange->setRealBounds(p);
    
    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
    }
}

void TrackScroller::xMoveByUser()
{
    const Rectangle<float> &screenRangeBounds = this->screenRange->getRealBounds();
    
    const float &mw = float(this->getWidth()) - screenRangeBounds.getWidth();
    const float &propX = screenRangeBounds.getTopLeft().getX() / mw;
    const float &propY = float(this->roll.getViewport().getViewPositionY()) /
                         float(this->roll.getHeight() - this->roll.getViewport().getViewHeight());
    
    this->roll.panProportionally(propX, propY);
    
    const Rectangle<float> &p = this->getIndicatorBounds();
    const Rectangle<int> &hp = p.toType<int>();
    this->helperRectangle->setBounds(hp.withTop(0).withBottom(this->getHeight()));
    this->screenRange->setRealBounds(p);
    
    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
    }
}

void TrackScroller::resizeByUser()
{
    const float &w = float(this->screenRange->getWidth());
    const float &h = float(this->screenRange->getHeight());
    
    const float &mw = float(this->getWidth());
    const float &propX = (w / mw);
    
    const float &mh = float(this->getHeight());
    const float &propY = (h / mh);
    
    const Point<float> proportional(propX, propY);
    
    this->roll.zoomAbsolute(proportional);
    
    
    const Rectangle<float> &p = this->getIndicatorBounds();
    const Rectangle<int> &hp = p.toType<int>();
    this->helperRectangle->setBounds(hp.withTop(0).withBottom(this->getHeight()));
    this->screenRange->setRealBounds(p);
    
    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
    }
}

void TrackScroller::toggleStretchingMapAlaSublime()
{
    this->mapShouldGetStretched = !this->mapShouldGetStretched;
    this->resized();
}


//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void TrackScroller::resized()
{
    const Rectangle<float> &p = this->getIndicatorBounds();
    const Rectangle<int> &hp = p.toType<int>();
    this->helperRectangle->setBounds(hp.withTop(0).withBottom(this->getHeight()));
    this->screenRange->setRealBounds(p);
    
    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
    }
    
    this->background->setBounds(0, 0, this->getWidth(), this->getHeight());
}

void TrackScroller::paintOverChildren(Graphics& g)
{
//    g.setColour(findColour(Origami::resizerLineColourId));
//    g.drawHorizontalLine(0, 0.f, float(this->getWidth()));
//    g.setColour(findColour(Origami::resizerLineColourId).withMultipliedAlpha(0.5f));
//    g.drawHorizontalLine(1, 0.f, float(this->getWidth()));
    
    g.setColour(Colours::black.withAlpha(0.35f));
    g.drawHorizontalLine(0, 0.f, float(this->getWidth()));
    
    g.setColour(Colours::white.withAlpha(0.025f));
    g.drawHorizontalLine(1, 0.f, float(this->getWidth()));
}

void TrackScroller::mouseDown(const MouseEvent &event)
{
//    if (this->mapShouldGetStretched)
//    {
//        const MouseEvent e2(event.getEventRelativeTo(this->trackImage));
//        
//        const Point<float> absClickPos(float(e2.getPosition().getX()) / float(this->trackImage->getWidth()),
//                                       float(e2.getPosition().getY()) / float(this->trackImage->getHeight()));
//        
//        const Point<float> newScreenPos(this->getWidth() * absClickPos.getX(),
//                                        this->getHeight() * absClickPos.getY());
//        
//        Logger::writeToLog(String(newScreenPos.getX()) + ":" + String(newScreenPos.getY()));
//        
//        this->screenRange->setRealBounds(this->screenRange->getRealBounds().withCentre(newScreenPos));
//        this->screenRange->setCentrePosition(newScreenPos.getX(), newScreenPos.getY());
//        this->scrollerMovedByUser();
//    }
}

void TrackScroller::mouseDrag(const MouseEvent &event)
{
    if (! this->mapShouldGetStretched)
    {
        //Logger::writeToLog(String(newScreenPos.getX()) + ":" + String(newScreenPos.getY()));
        this->screenRange->setRealBounds(this->screenRange->getRealBounds().withCentre(event.position));
        //this->screenRange->setCentrePosition(newScreenPos.getX(), newScreenPos.getY());
        this->xyMoveByUser();
    }
}

void TrackScroller::mouseUp(const MouseEvent &event)
{
    if (event.getDistanceFromDragStart() < 5)
    {
        this->toggleStretchingMapAlaSublime();
    }
}

void TrackScroller::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    this->roll.mouseWheelMove(event.getEventRelativeTo(&this->roll), wheel);
}


////===----------------------------------------------------------------------===//
//// ChangeListener
////
//===----------------------------------------------------------------------===//
//void TrackScroller::changeListenerCallback(ChangeBroadcaster *source)
//{
//    const Rectangle<float> &p = this->getIndicatorBounds();
//    const Rectangle<int> &hp = p.toType<int>();
//    this->helperRectangle->setBounds(hp.withTop(0).withBottom(this->getHeight()));
//    this->screenRange->setRealBounds(p);
//    
//    for (int i = 0; i < this->trackMaps.size(); ++i)
//    {
//        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
//        this->trackMaps.getUnchecked(i)->resized(); // as roll resizes, force maps to update
//    }
//    
//    this->indicator->parentSizeChanged(); // a hack: also update indicator position
//}


//===----------------------------------------------------------------------===//
// MidiRollListener
//===----------------------------------------------------------------------===//

void TrackScroller::onMidiRollMoved(MidiRoll *targetRoll)
{
    this->triggerAsyncUpdate();
}

void TrackScroller::onMidiRollResized(MidiRoll *targetRoll)
{
    this->triggerAsyncUpdate();
}

void TrackScroller::handleAsyncUpdate()
{
    const Rectangle<float> &p = this->getIndicatorBounds();
    const Rectangle<int> &hp = p.toType<int>();
    this->helperRectangle->setBounds(hp.withTop(0).withBottom(this->getHeight()));
    this->screenRange->setRealBounds(p);
    
    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
        this->trackMaps.getUnchecked(i)->resized(); // as roll resizes, force maps to update
    }
    
    this->indicator->parentSizeChanged(); // a hack: also update indicator position
}


//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

#define INDICATOR_FIXED_WIDTH (150)

Rectangle<float> TrackScroller::getIndicatorBounds() const
{
    const float &viewX = float(this->roll.getViewport().getViewPositionX());
    const float &viewWidth = float(this->roll.getViewport().getViewWidth());
    const float &rollWidth = float(this->roll.getWidth());
    const float &rollInvisibleArea = rollWidth - viewWidth;
    const float &trackWidth = float(this->getWidth());
    const float &trackInvisibleArea = float(this->getWidth() - INDICATOR_FIXED_WIDTH);
    const float &mapWidth = ((INDICATOR_FIXED_WIDTH * rollWidth) / viewWidth);
    
    const float &zoomFactorY = this->roll.getZoomFactorY();
    //const float &headerHeight = 0; //float(MIDIROLL_HEADER_HEIGHT);
    const float &headerHeight = float(MIDIROLL_HEADER_HEIGHT);
    const float &rollHeight = float(this->roll.getHeight() - headerHeight);
    const float &viewY = float(this->roll.getViewport().getViewPositionY() + headerHeight);
    const float &trackHeight = float(this->getHeight());
    const float &rY = roundf(trackHeight * (viewY / rollHeight));
    const float &rH = (trackHeight * zoomFactorY);
    
    if (mapWidth <= trackWidth || !this->mapShouldGetStretched)
    {
        const float &rX = ((trackWidth * viewX) / rollWidth);
        const float &rW = (trackWidth * this->roll.getZoomFactorX());
        return Rectangle<float>(rX, rY, rW, rH);
    }
    
    
        const float &rX = ((trackInvisibleArea * viewX) / jmax(rollInvisibleArea, viewWidth));
        const float &rW = INDICATOR_FIXED_WIDTH;
        return Rectangle<float>(rX, rY, rW, rH);
    
}

Rectangle<int> TrackScroller::getMapBounds() const
{
    const float &viewX = float(this->roll.getViewport().getViewPositionX());
    const float &viewWidth = float(this->roll.getViewport().getViewWidth());
    const float &rollWidth = float(this->roll.getWidth());
    const float &rollInvisibleArea = rollWidth - viewWidth;
    const float &trackWidth = float(this->getWidth());
    const float &trackInvisibleArea = float(this->getWidth() - INDICATOR_FIXED_WIDTH);
    const float &mapWidth = ((INDICATOR_FIXED_WIDTH * rollWidth) / viewWidth);
    
    if (mapWidth <= trackWidth || !this->mapShouldGetStretched)
    {
        return Rectangle<int>(0, 0, int(trackWidth), this->getHeight());
    }
    
    
        const float rX = ((trackInvisibleArea * viewX) / jmax(rollInvisibleArea, viewWidth));
        const float dX = (viewX * mapWidth) / rollWidth;
        return Rectangle<int>(int(rX - dX), 0, int(mapWidth), this->getHeight());
    
}

void TrackScroller::HorizontalDragHelper::
MoveConstrainer::applyBoundsToComponent(Component *component, const Rectangle<int> &bounds)
{
    ComponentBoundsConstrainer::applyBoundsToComponent(component, bounds);
    this->scroller.horizontalDragByUser(component, bounds);
}

