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

#include "RevisionComponent.h"

//[MiscUserDefs]
#include "VersionControl.h"
#include "RevisionTreeComponent.h"
#include "SerializationKeys.h"
#include "App.h"

#if HELIO_DESKTOP
#   define REVISION_COMPONENT_WIDTH (150)
#   define REVISION_COMPONENT_HEIGHT (40)
#elif HELIO_MOBILE
#   define REVISION_COMPONENT_WIDTH (170)
#   define REVISION_COMPONENT_HEIGHT (50)
#endif

//[/MiscUserDefs]

RevisionComponent::RevisionComponent(VersionControl &owner, const VCS::Revision target, bool isHead)
    : vcs(owner),
      revision(target),
      selected(false),
      isHeadRevision(isHead),
      x(0.f),
      y(0.f),
      mod(0.f),
      shift(0.f),
      change(0.f),
      number(0),
      parent(nullptr),
      thread(nullptr),
      leftmostSibling(nullptr)
{
    addAndMakeVisible (revisionDescription = new Label (String(),
                                                        TRANS("...")));
    revisionDescription->setFont (Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    revisionDescription->setJustificationType (Justification::centred);
    revisionDescription->setEditable (false, false, false);
    revisionDescription->setColour (TextEditor::textColourId, Colours::black);
    revisionDescription->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (revisionDate = new Label (String(),
                                                 TRANS("...")));
    revisionDate->setFont (Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    revisionDate->setJustificationType (Justification::centred);
    revisionDate->setEditable (false, false, false);
    revisionDate->setColour (Label::textColourId, Colour (0x99ffffff));
    revisionDate->setColour (TextEditor::textColourId, Colours::black);
    revisionDate->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
    //this->shadow.setShadowProperties(DropShadow(Colours::white.withAlpha(0.125f), 3, Point<int> (0, 0)));
    //this->setComponentEffect(&shadow);

    //const String &message = this->revision.getProperty(Identifier(Serialization::VCS::commitMessage)).toString();
    //const int64 timestamp = this->revision.getProperty(Identifier(Serialization::VCS::commitTimeStamp));
    const String &message = this->revision.getMessage();
    const int64 timestamp = this->revision.getTimeStamp();

    this->revisionDescription->setText(message, dontSendNotification);
    this->revisionDate->setText(App::getHumanReadableDate(Time(timestamp)), dontSendNotification);

    this->setInterceptsMouseClicks(true, false);
    //this->background->setInterceptsMouseClicks(false, false);
    //this->background2->setInterceptsMouseClicks(false, false);
    this->revisionDescription->setInterceptsMouseClicks(false, false);
    this->revisionDate->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    setSize (150, 40);

    //[Constructor]
    if (!this->isHeadRevision)
    {
    //    this->background->setAlpha(0.7f);
    }

    this->setSize(REVISION_COMPONENT_WIDTH, REVISION_COMPONENT_HEIGHT);

    this->revisionDescription->setFont(Font(Font::getDefaultSansSerifFontName(), (this->getHeight() / 3.f), Font::plain));
    this->revisionDate->setFont(Font(Font::getDefaultSansSerifFontName(), (this->getHeight() / 4.f), Font::plain));

    //[/Constructor]
}

RevisionComponent::~RevisionComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    revisionDescription = nullptr;
    revisionDate = nullptr;

    //[Destructor]
    //[/Destructor]
}

void RevisionComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0x0bffffff));
    g.fillRoundedRectangle (static_cast<float> ((getWidth() / 2) - (80 / 2)), static_cast<float> ((getHeight() / 2) + 2), 80.0f, 1.0f, 10.000f);

    g.setColour (Colour (0x0d000000));
    g.fillRoundedRectangle (static_cast<float> ((getWidth() / 2) - (80 / 2)), static_cast<float> ((getHeight() / 2) + 1), 80.0f, 1.0f, 10.000f);

    g.setColour (Colour (0x25ffffff));
    g.fillRoundedRectangle (static_cast<float> ((getWidth() / 2) - (35 / 2)), static_cast<float> (getHeight() - (4 / 2)), 35.0f, 4.0f, 10.000f);

    g.setColour (Colour (0x25ffffff));
    g.fillRoundedRectangle (static_cast<float> ((getWidth() / 2) - (35 / 2)), static_cast<float> (0 - (4 / 2)), 35.0f, 4.0f, 10.000f);

    //[UserPaint] Add your own custom painting code here..

    if (this->isHeadRevision)
    {
        g.setColour(Colours::white.withAlpha(0.3f));
        g.drawRoundedRectangle (0.0f, 0.0f, static_cast<float> (getWidth()), static_cast<float> (getHeight()), 9.000f, 1.0f);
    }

    if (this->selected)
    {
        g.setColour(Colours::white.withAlpha(0.15f));
        g.drawRoundedRectangle (0.0f, 0.0f, static_cast<float> (getWidth()), static_cast<float> (getHeight()), 9.000f, 1.0f);
    }

    //[/UserPaint]
}

void RevisionComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    revisionDescription->setBounds (4, (getHeight() / 2) + 2 - 21, getWidth() - 8, 21);
    revisionDate->setBounds (4, (getHeight() / 2) + -1, getWidth() - 8, 21);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void RevisionComponent::mouseMove (const MouseEvent& e)
{
    //[UserCode_mouseMove] -- Add your code here...
    this->setMouseCursor(MouseCursor(MouseCursor::NormalCursor));
    //[/UserCode_mouseMove]
}

void RevisionComponent::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    if (RevisionTreeComponent *revTree = dynamic_cast<RevisionTreeComponent *>(this->getParentComponent()))
    {
        revTree->selectComponent(this, true);
        revTree->showTooltipFor(this, e.getMouseDownPosition(), this->revision);
    }
    //[/UserCode_mouseDown]
}


//[MiscUserCode]

void RevisionComponent::setSelected(bool selected)
{
    this->selected = selected;
    this->repaint();
}

bool RevisionComponent::isSelected() const
{
    return this->selected;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RevisionComponent" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams="VersionControl &amp;owner, const VCS::Revision target, bool isHead"
                 variableInitialisers="vcs(owner)&#10;revision(target),&#10;selected(false),&#10;isHeadRevision(isHead),&#10;x(0.f),&#10;y(0.f),&#10;mod(0.f),&#10;shift(0.f),&#10;change(0.f),&#10;number(0),&#10;parent(nullptr),&#10;thread(nullptr),&#10;leftmostSibling(nullptr)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="150" initialHeight="40">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseMove (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0Cc 2C 80 1" cornerSize="10" fill="solid: bffffff" hasStroke="0"/>
    <ROUNDRECT pos="0Cc 1C 80 1" cornerSize="10" fill="solid: d000000" hasStroke="0"/>
    <ROUNDRECT pos="0Cc 0Rc 35 4" cornerSize="10" fill="solid: 25ffffff" hasStroke="0"/>
    <ROUNDRECT pos="0Cc 0c 35 4" cornerSize="10" fill="solid: 25ffffff" hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="" id="45b178bfb039403" memberName="revisionDescription"
         virtualName="" explicitFocusOrder="0" pos="4 2Cr 8M 21" edTextCol="ff000000"
         edBkgCol="0" labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="21"
         kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="30ac314958873bc0" memberName="revisionDate" virtualName=""
         explicitFocusOrder="0" pos="4 -1C 8M 21" textCol="99ffffff" edTextCol="ff000000"
         edBkgCol="0" labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="21"
         kerning="0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
