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

//[Headers]
#include "Common.h"
//[/Headers]

#include "NoteResizerRight.h"

//[MiscUserDefs]
#include "HybridRoll.h"
#include "PianoRoll.h"
#include "PianoSequence.h"
#include "SequencerOperations.h"
#include "NoteComponent.h"
//[/MiscUserDefs]

NoteResizerRight::NoteResizerRight(HybridRoll &parentRoll)
    : roll(parentRoll)
{
    this->resizeIcon.reset(new IconComponent(Icons::stretchRight));
    this->addAndMakeVisible(resizeIcon.get());

    resizeIcon->setBounds(8, 8, 24, 24);


    //[UserPreSize]
    this->setAlpha(0.f);
    this->resizeIcon->setAlpha(0.5f);
    this->setMouseCursor(MouseCursor::LeftRightResizeCursor);
    this->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    this->setSize(64, 256);

    //[Constructor]
    //[/Constructor]
}

NoteResizerRight::~NoteResizerRight()
{
    //[Destructor_pre]
    Desktop::getInstance().getAnimator().animateComponent(this, this->getBounds(), 0.f, 100, true, 0.0, 0.0);
    //[/Destructor_pre]

    resizeIcon = nullptr;

    //[Destructor]
    //[/Destructor]
}

void NoteResizerRight::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = 2, height = getHeight() - 0;
        Colour fillColour = Colour (0x1affffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        float x = static_cast<float> (0 - (96 / 2)), y = static_cast<float> (0 - (96 / 2)), width = 96.0f, height = 96.0f;
        Colour fillColour = Colour (0x20ffffff);
        Colour strokeColour = Colour (0x30ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillEllipse (x, y, width, height);
        g.setColour (strokeColour);
        g.drawEllipse (x, y, width, height, 1.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void NoteResizerRight::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

bool NoteResizerRight::hitTest (int x, int y)
{
    //[UserCode_hitTest] -- Add your code here...
    const int r = 96 / 2;
    return (x * x) + (y * y) < (r * r);
    //[/UserCode_hitTest]
}

void NoteResizerRight::mouseEnter (const MouseEvent& e)
{
    //[UserCode_mouseEnter] -- Add your code here...
    this->resizeIcon->setAlpha(1.f);
    //[/UserCode_mouseEnter]
}

void NoteResizerRight::mouseExit (const MouseEvent& e)
{
    //[UserCode_mouseExit] -- Add your code here...
    this->resizeIcon->setAlpha(0.5f);
    //[/UserCode_mouseExit]
}

void NoteResizerRight::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    this->dragger.startDraggingComponent(this, e);

    const Lasso &selection = this->roll.getLassoSelection();
    const float groupStartBeat = SequencerOperations::findStartBeat(selection);

    this->noteComponent = this->findRightMostEvent(selection);

    for (int i = 0; i < selection.getNumSelected(); i++)
    {
        if (NoteComponent *note = dynamic_cast<NoteComponent *>(selection.getSelectedItem(i)))
        {
            if (selection.shouldDisplayGhostNotes())
            { note->getRoll().showGhostNoteFor(note); }

            note->startGroupScalingRight(groupStartBeat);
        }
    }
    //[/UserCode_mouseDown]
}

void NoteResizerRight::mouseDrag (const MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    this->dragger.dragComponent(this, e, nullptr);

    Lasso &selection = this->roll.getLassoSelection();

    float groupScaleFactor = 1.f;
    const bool scaleFactorChanged =
        this->noteComponent->getGroupScaleRightFactor(
            e.withNewPosition(Point<int>(0, 0))
            .getEventRelativeTo(this->noteComponent), groupScaleFactor);

    if (scaleFactorChanged)
    {
        this->noteComponent->checkpointIfNeeded();
        Array<Note> groupDragBefore, groupDragAfter;

        for (int i = 0; i < selection.getNumSelected(); ++i)
        {
            NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            groupDragBefore.add(nc->getNote());
            groupDragAfter.add(nc->continueGroupScalingRight(groupScaleFactor));
        }

        const auto &event = selection.getFirstAs<NoteComponent>()->getNote();
        PianoSequence *pianoLayer = static_cast<PianoSequence *>(event.getSequence());
        pianoLayer->changeGroup(groupDragBefore, groupDragAfter, true);
    }

    this->updateBounds(this->noteComponent);
    //[/UserCode_mouseDrag]
}

void NoteResizerRight::mouseUp (const MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    const Lasso &selection = this->roll.getLassoSelection();

    for (int i = 0; i < selection.getNumSelected(); i++)
    {
        NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        nc->getRoll().hideAllGhostNotes();
        nc->endGroupScalingLeft();
    }
    //[/UserCode_mouseUp]
}


//[MiscUserCode]

NoteComponent *NoteResizerRight::findRightMostEvent(const Lasso &selection)
{
    NoteComponent *mc = nullptr;
    float rightMostBeat = -FLT_MAX;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        NoteComponent *currentEvent = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        const float beatPlusLength = currentEvent->getBeat() + currentEvent->getLength();

        if (rightMostBeat < beatPlusLength)
        {
            rightMostBeat = beatPlusLength;
            mc = currentEvent;
        }
    }

    return mc;
}

void NoteResizerRight::updateBounds(NoteComponent *anchorComponent)
{
    const Lasso &selection = this->roll.getLassoSelection();
    const float groupEndBeat = (anchorComponent != nullptr) ?
                                (anchorComponent->getBeat() + anchorComponent->getLength()) :
                                SequencerOperations::findEndBeat(selection);

    const int xAnchor = this->roll.getXPositionByBeat(groupEndBeat);
    const int yAnchor = this->roll.getViewport().getViewPositionY() + HybridRoll::headerHeight;
    const int h = this->roll.getViewport().getViewHeight();
    this->setBounds(xAnchor, yAnchor, this->getWidth(), h);

    if (this->getAlpha() == 0.f && !this->fader.isAnimating())
    {
        this->fader.fadeIn(this, 100);
    }
}

void NoteResizerRight::updateTopPosition()
{
    const int yAnchor = this->roll.getViewport().getViewPositionY() + HybridRoll::headerHeight;
    this->setTopLeftPosition(this->getX(), yAnchor);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="NoteResizerRight" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="HybridRoll &amp;parentRoll"
                 variableInitialisers="roll(parentRoll)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="64"
                 initialHeight="256">
  <METHODS>
    <METHOD name="mouseEnter (const MouseEvent&amp; e)"/>
    <METHOD name="mouseExit (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
    <METHOD name="hitTest (int x, int y)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 0 2 0M" fill="solid: 1affffff" hasStroke="0"/>
    <ELLIPSE pos="0c 0c 96 96" fill="solid: 20ffffff" hasStroke="1" stroke="1, mitered, butt"
             strokeColour="solid: 30ffffff"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="79f90a69d0b95011" memberName="resizeIcon" virtualName=""
                    explicitFocusOrder="0" pos="8 8 24 24" class="IconComponent"
                    params="Icons::stretchRight"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
