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
#include "KeySignaturesSequence.h"
#include "HybridRoll.h"
#include "ColourIDs.h"
#include "CachedLabelImage.h"
//[/Headers]

#include "KeySignatureLargeComponent.h"

//[MiscUserDefs]
//[/MiscUserDefs]

KeySignatureLargeComponent::KeySignatureLargeComponent(KeySignaturesProjectMap<KeySignatureLargeComponent> &parent, const KeySignatureEvent &targetEvent)
    : event(targetEvent),
      editor(parent),
      anchor(targetEvent),
      textWidth(0.f),
      mouseDownWasTriggered(false)
{
    addAndMakeVisible (signatureLabel = new Label (String(),
                                                   TRANS("...")));
    signatureLabel->setFont (Font (16.00f, Font::plain).withTypefaceStyle ("Regular"));
    signatureLabel->setJustificationType (Justification::centredLeft);
    signatureLabel->setEditable (false, false, false);

    signatureLabel->setBounds (-2, 1, 192, 24);


    //[UserPreSize]
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->signatureLabel->setInterceptsMouseClicks(false, false);

    this->signatureLabel->setBufferedToImage(true);
    this->signatureLabel->setCachedComponentImage(new CachedLabelImage(*this->signatureLabel));

    this->setMouseCursor(MouseCursor::PointingHandCursor);
    //[/UserPreSize]

    setSize (128, 24);

    //[Constructor]
    //[/Constructor]
}

KeySignatureLargeComponent::~KeySignatureLargeComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    signatureLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void KeySignatureLargeComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    const Colour lineCol(this->findColour(ColourIDs::Roll::headerSnaps));
    g.setColour(lineCol);
    g.fillRect(0.f, 2.f, float(this->getWidth()), 2.f);
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void KeySignatureLargeComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void KeySignatureLargeComponent::mouseMove (const MouseEvent& e)
{
    //[UserCode_mouseMove] -- Add your code here...
    //[/UserCode_mouseMove]
}

void KeySignatureLargeComponent::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    this->mouseDownWasTriggered = true;

    if (e.mods.isLeftButtonDown())
    {
        // don't checkpoint right here, but only before the actual change
        //this->event.getSequence()->checkpoint();

        this->dragger.startDraggingComponent(this, e);
        this->draggingHadCheckpoint = false;
        this->draggingState = true;
        this->anchor = this->event;
    }
    else
    {
        this->editor.alternateActionFor(this);
        //this->editor.showContextMenuFor(this);
    }
    //[/UserCode_mouseDown]
}

void KeySignatureLargeComponent::mouseDrag (const MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    if (e.mods.isLeftButtonDown() && e.getDistanceFromDragStart() > 4)
    {
        if (this->draggingState)
        {
            this->setMouseCursor(MouseCursor::DraggingHandCursor);
            this->dragger.dragComponent(this, e, nullptr);
            const float newBeat = this->editor.getBeatByXPosition(this->getX());
            const bool beatHasChanged = (this->event.getBeat() != newBeat);

            if (beatHasChanged)
            {
                if (! this->draggingHadCheckpoint)
                {
                    this->event.getSequence()->checkpoint();
                    this->draggingHadCheckpoint = true;
                }

                Array<KeySignatureEvent> groupDragBefore, groupDragAfter;
                groupDragBefore.add(this->event);
                groupDragAfter.add(this->event.withBeat(newBeat));
                KeySignaturesSequence *sequence = static_cast<KeySignaturesSequence *>(this->event.getSequence());
                sequence->changeGroup(groupDragBefore, groupDragAfter, true);
            }
            else
            {
                this->editor.alignKeySignatureComponent(this);
            }
        }
    }
    //[/UserCode_mouseDrag]
}

void KeySignatureLargeComponent::mouseUp (const MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            this->setMouseCursor(MouseCursor::PointingHandCursor);
            this->draggingState = false;
            this->editor.onKeySignatureMoved(this);
        }

        if (e.getDistanceFromDragStart() < 10 &&
            this->mouseDownWasTriggered &&
            !this->draggingHadCheckpoint)
        {
            this->editor.onKeySignatureTapped(this);
        }
    }

    this->mouseDownWasTriggered = false;
    //[/UserCode_mouseUp]
}

void KeySignatureLargeComponent::mouseDoubleClick (const MouseEvent& e)
{
    //[UserCode_mouseDoubleClick] -- Add your code here...
    //[/UserCode_mouseDoubleClick]
}


//[MiscUserCode]

const KeySignatureEvent &KeySignatureLargeComponent::getEvent() const
{
    return this->event;
}

void KeySignatureLargeComponent::setRealBounds(const Rectangle<float> bounds)
{
    Rectangle<int> intBounds(bounds.toType<int>());
    this->boundsOffset = Rectangle<float>(bounds.getX() - float(intBounds.getX()),
                                          bounds.getY(),
                                          bounds.getWidth() - float(intBounds.getWidth()),
                                          bounds.getHeight());

    this->setBounds(intBounds);
}

float KeySignatureLargeComponent::getBeat() const
{
    return this->event.getBeat();
}

float KeySignatureLargeComponent::getTextWidth() const
{
    return this->textWidth;
}

void KeySignatureLargeComponent::updateContent()
{
    const String originalName = this->event.toString();
    if (this->eventName != originalName)
    {
        this->eventName = originalName;
        this->textWidth = float(this->signatureLabel->getFont().getStringWidth(originalName));
        this->signatureLabel->setText(originalName, dontSendNotification);
        this->repaint();
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="KeySignatureLargeComponent"
                 template="../../../../Template" componentName="" parentClasses="public Component"
                 constructorParams="KeySignaturesProjectMap&lt;KeySignatureLargeComponent&gt; &amp;parent, const KeySignatureEvent &amp;targetEvent"
                 variableInitialisers="event(targetEvent),&#10;editor(parent),&#10;anchor(targetEvent),&#10;textWidth(0.f),&#10;mouseDownWasTriggered(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="128" initialHeight="24">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
    <METHOD name="mouseMove (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDoubleClick (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="3dbd8cef4b61c2fe" memberName="signatureLabel" virtualName=""
         explicitFocusOrder="0" pos="-2 1 192 24" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="16.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
