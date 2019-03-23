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

#include "NoteResizerLeft.h"

//[MiscUserDefs]
#include "HybridRoll.h"
#include "PianoRoll.h"
#include "PianoSequence.h"
#include "NoteComponent.h"
#include "SequencerOperations.h"
//[/MiscUserDefs]

NoteResizerLeft::NoteResizerLeft(HybridRoll &parentRoll)
    : roll(parentRoll)
{
    addAndMakeVisible (resizeIcon = new IconComponent (Icons::stretchLeft));


    //[UserPreSize]
    this->setAlpha(0.f);
    this->resizeIcon->setAlpha(0.5f);
    this->setMouseCursor(MouseCursor::LeftRightResizeCursor);
    this->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    setSize (64, 256);

    //[Constructor]
    //[/Constructor]
}

NoteResizerLeft::~NoteResizerLeft()
{
    //[Destructor_pre]
    Desktop::getInstance().getAnimator().animateComponent(this, this->getBounds(), 0.f, 100, true, 0.0, 0.0);
    //[/Destructor_pre]

    resizeIcon = nullptr;

    //[Destructor]
    //[/Destructor]
}

void NoteResizerLeft::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0x1affffff));
    g.fillRect (getWidth() - 2, 0, 2, getHeight() - 0);

    g.setColour (Colour (0x20ffffff));
    g.fillEllipse (static_cast<float> (getWidth() - (96 / 2)), static_cast<float> (0 - (96 / 2)), 96.0f, 96.0f);

    g.setColour (Colour (0x30ffffff));
    g.drawEllipse (static_cast<float> (getWidth() - (96 / 2)), static_cast<float> (0 - (96 / 2)), 96.0f, 96.0f, 1.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void NoteResizerLeft::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    resizeIcon->setBounds (getWidth() - 8 - 24, 8, 24, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

bool NoteResizerLeft::hitTest (int x, int y)
{
    //[UserCode_hitTest] -- Add your code here...
    const int xCenter = this->getWidth();
    const int dx = x - xCenter;
    const int r = 96 / 2;
    return (dx * dx) + (y * y) < (r * r);
    //[/UserCode_hitTest]
}

void NoteResizerLeft::mouseEnter (const MouseEvent& e)
{
    //[UserCode_mouseEnter] -- Add your code here...
    this->resizeIcon->setAlpha(1.f);
    //[/UserCode_mouseEnter]
}

void NoteResizerLeft::mouseExit (const MouseEvent& e)
{
    //[UserCode_mouseExit] -- Add your code here...
    this->resizeIcon->setAlpha(0.5f);
    //[/UserCode_mouseExit]
}

void NoteResizerLeft::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    this->dragger.startDraggingComponent(this, e);

    const Lasso &selection = this->roll.getLassoSelection();
    const float groupEndBeat = SequencerOperations::findEndBeat(selection);

    this->noteComponent = this->findLeftMostEvent(selection);

    for (int i = 0; i < selection.getNumSelected(); i++)
    {
        if (NoteComponent *note = dynamic_cast<NoteComponent *>(selection.getSelectedItem(i)))
        {
            if (selection.shouldDisplayGhostNotes())
            { note->getRoll().showGhostNoteFor(note); }

            note->startGroupScalingLeft(groupEndBeat);
        }
    }
    //[/UserCode_mouseDown]
}

void NoteResizerLeft::mouseDrag (const MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    this->dragger.dragComponent(this, e, nullptr);

    Lasso &selection = this->roll.getLassoSelection();

    float groupScaleFactor = 1.f;
    const bool scaleFactorChanged =
        this->noteComponent->getGroupScaleLeftFactor(e.withNewPosition(Point<int>(this->getWidth(), 0)).getEventRelativeTo(this->noteComponent), groupScaleFactor);

    if (scaleFactorChanged)
    {
        this->noteComponent->checkpointIfNeeded();
        Array<Note> groupDragBefore, groupDragAfter;

        for (int i = 0; i < selection.getNumSelected(); ++i)
        {
            NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            groupDragBefore.add(nc->getNote());
            groupDragAfter.add(nc->continueGroupScalingLeft(groupScaleFactor));
        }

        const auto &event = selection.getFirstAs<NoteComponent>()->getNote();
        PianoSequence *pianoLayer = static_cast<PianoSequence *>(event.getSequence());
        pianoLayer->changeGroup(groupDragBefore, groupDragAfter, true);
    }

    this->updateBounds(this->noteComponent);
    //[/UserCode_mouseDrag]
}

void NoteResizerLeft::mouseUp (const MouseEvent& e)
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

NoteComponent *NoteResizerLeft::findLeftMostEvent(const Lasso &selection)
{
    MidiEventComponent *mc = nullptr;
    float leftMostBeat = FLT_MAX;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        MidiEventComponent *const e = selection.getItemAs<MidiEventComponent>(i);

        if (leftMostBeat > e->getBeat())
        {
            leftMostBeat = e->getBeat();
            mc = e;
        }
    }

    return static_cast<NoteComponent *>(mc);
}

void NoteResizerLeft::updateBounds(NoteComponent *anchorComponent)
{
    const Lasso &selection = this->roll.getLassoSelection();
    const float groupStartBeat = (anchorComponent != nullptr) ?
                                  anchorComponent->getBeat() :
                                  SequencerOperations::findStartBeat(selection);

    const int xAnchor = this->roll.getXPositionByBeat(groupStartBeat);
    const int yAnchor = this->roll.getViewport().getViewPositionY() + HYBRID_ROLL_HEADER_HEIGHT;
    const int h = this->roll.getViewport().getViewHeight();
    this->setBounds(xAnchor - this->getWidth(), yAnchor, this->getWidth(), h);

    if (this->getAlpha() == 0.f && !this->fader.isAnimating())
    {
        this->fader.fadeIn(this, 100);
    }
}

void NoteResizerLeft::updateTopPosition()
{
    const int yAnchor = this->roll.getViewport().getViewPositionY() + HYBRID_ROLL_HEADER_HEIGHT;
    this->setTopLeftPosition(this->getX(), yAnchor);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="NoteResizerLeft" template="../../../Template"
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
    <RECT pos="0Rr 0 2 0M" fill="solid: 1affffff" hasStroke="0"/>
    <ELLIPSE pos="0Rc 0c 96 96" fill="solid: 20ffffff" hasStroke="1" stroke="1, mitered, butt"
             strokeColour="solid: 30ffffff"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="79f90a69d0b95011" memberName="resizeIcon" virtualName=""
                    explicitFocusOrder="0" pos="8Rr 8 24 24" class="IconComponent"
                    params="Icons::stretchLeft"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
