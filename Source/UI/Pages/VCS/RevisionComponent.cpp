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

RevisionComponent::RevisionComponent(VersionControl &owner, const VCS::Revision::Ptr revision, VCS::Revision::SyncState viewState, bool isHead)
    : vcs(owner),
      revision(revision),
      isSelected(false),
      isHeadRevision(isHead),
      viewState(viewState),
      x(0.f),
      y(0.f),
      mod(0.f),
      shift(0.f),
      change(0.f),
      number(0),
      parent(nullptr),
      leftmostSibling(nullptr)
{
    this->revisionDescription.reset(new Label(String(),
                                               TRANS("...")));
    this->addAndMakeVisible(revisionDescription.get());
    this->revisionDescription->setFont(Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    revisionDescription->setJustificationType(Justification::centred);
    revisionDescription->setEditable(false, false, false);

    this->revisionDate.reset(new Label(String(),
                                        TRANS("...")));
    this->addAndMakeVisible(revisionDate.get());
    this->revisionDate->setFont(Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    revisionDate->setJustificationType(Justification::centred);
    revisionDate->setEditable(false, false, false);


    //[UserPreSize]
    const String &message = this->revision->getMessage();
    const int64 timestamp = this->revision->getTimeStamp();

    this->revisionDescription->setText(message, dontSendNotification);
    this->revisionDate->setText(App::getHumanReadableDate(Time(timestamp)), dontSendNotification);

    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->revisionDescription->setInterceptsMouseClicks(false, false);
    this->revisionDate->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    this->setSize(150, 40);

    //[Constructor]
    this->setSize(REVISION_COMPONENT_WIDTH, REVISION_COMPONENT_HEIGHT);
    this->revisionDescription->setFont(this->getHeight() / 3.f);
    this->revisionDate->setFont(this->getHeight() / 4.f);
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

    {
        float x = static_cast<float> ((getWidth() / 2) - (80 / 2)), y = static_cast<float> ((getHeight() / 2) + 2), width = 80.0f, height = 1.0f;
        Colour fillColour = Colour (0x0bffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    {
        float x = static_cast<float> ((getWidth() / 2) - (80 / 2)), y = static_cast<float> ((getHeight() / 2) + 1), width = 80.0f, height = 1.0f;
        Colour fillColour = Colour (0x0d000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    {
        float x = static_cast<float> ((getWidth() / 2) - (35 / 2)), y = static_cast<float> (getHeight() - (4 / 2)), width = 35.0f, height = 4.0f;
        Colour fillColour = Colour (0x25ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    {
        float x = static_cast<float> ((getWidth() / 2) - (35 / 2)), y = static_cast<float> (0 - (4 / 2)), width = 35.0f, height = 4.0f;
        Colour fillColour = Colour (0x25ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    //[UserPaint] Add your own custom painting code here..

    if (this->isHeadRevision)
    {
        g.setColour(Colours::white.withAlpha(0.15f));
        g.drawRoundedRectangle(0.0f, 0.0f, static_cast<float> (getWidth()), static_cast<float> (getHeight()), 9.000f, 1.0f);
    }

    if (this->isSelected)
    {
        g.setColour(Colours::white.withAlpha(0.3f));
        g.drawRoundedRectangle (0.0f, 0.0f, static_cast<float> (getWidth()), static_cast<float> (getHeight()), 9.000f, 1.0f);
    }

    switch (this->viewState)
    {
    case VCS::Revision::NoSync:

        break;
    case VCS::Revision::ShallowCopy:

        break;
    case VCS::Revision::FullSync:

        break;
    default:
        break;
    }

    //[/UserPaint]
}

void RevisionComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    revisionDescription->setBounds(4, (getHeight() / 2) + 2 - 21, getWidth() - 8, 21);
    revisionDate->setBounds(4, (getHeight() / 2) + -1, getWidth() - 8, 21);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void RevisionComponent::mouseMove (const MouseEvent& e)
{
    //[UserCode_mouseMove] -- Add your code here...
    this->setMouseCursor(MouseCursor::NormalCursor);
    //[/UserCode_mouseMove]
}

void RevisionComponent::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    if (RevisionTreeComponent *revTree = dynamic_cast<RevisionTreeComponent *>(this->getParentComponent()))
    {
        revTree->selectComponent(this, true);
    }
    //[/UserCode_mouseDown]
}


//[MiscUserCode]

void RevisionComponent::setSelected(bool selected)
{
    this->isSelected = selected;
    this->repaint();
}

RevisionComponent *RevisionComponent::getLeftmostSibling() const
{
    if (!this->leftmostSibling && this->parent)
    {
        if (this != this->parent->children.getFirst())
        {
            this->leftmostSibling = this->parent->children.getFirst();
        }
    }

    return this->leftmostSibling;
}

RevisionComponent *RevisionComponent::getLeftBrother() const
{
    RevisionComponent *n = nullptr;

    if (this->parent)
    {
        for (auto i : this->parent->children)
        {
            if (i == this) { return n; }
            n = i;
        }
    }

    return n;
}

RevisionComponent *RevisionComponent::right() const
{
    if (this->wired) { return this->wired; }
    if (this->children.size() > 0) { return this->children.getLast(); }
    return nullptr;
}

RevisionComponent *RevisionComponent::left() const
{
    if (this->wired) { return this->wired; }
    if (this->children.size() > 0) { return this->children.getFirst(); }
    return nullptr;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RevisionComponent" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="VersionControl &amp;owner, const VCS::Revision::Ptr revision, VCS::Revision::SyncState viewState, bool isHead"
                 variableInitialisers="vcs(owner)&#10;revision(revision),&#10;isSelected(false),&#10;isHeadRevision(isHead),&#10;viewState(viewState),&#10;x(0.f),&#10;y(0.f),&#10;mod(0.f),&#10;shift(0.f),&#10;change(0.f),&#10;number(0),&#10;parent(nullptr),&#10;leftmostSibling(nullptr)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="150" initialHeight="40">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseMove (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0Cc 2C 80 1" cornerSize="10.00000000000000000000" fill="solid: bffffff"
               hasStroke="0"/>
    <ROUNDRECT pos="0Cc 1C 80 1" cornerSize="10.00000000000000000000" fill="solid: d000000"
               hasStroke="0"/>
    <ROUNDRECT pos="0Cc 0Rc 35 4" cornerSize="10.00000000000000000000" fill="solid: 25ffffff"
               hasStroke="0"/>
    <ROUNDRECT pos="0Cc 0c 35 4" cornerSize="10.00000000000000000000" fill="solid: 25ffffff"
               hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="" id="45b178bfb039403" memberName="revisionDescription"
         virtualName="" explicitFocusOrder="0" pos="4 2Cr 8M 21" labelText="..."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="21.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="36"/>
  <LABEL name="" id="30ac314958873bc0" memberName="revisionDate" virtualName=""
         explicitFocusOrder="0" pos="4 -1C 8M 21" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="21.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
