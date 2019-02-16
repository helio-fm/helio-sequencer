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
#include "ColourIDs.h"
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
      wired(nullptr),
      leftmostSibling(nullptr)
{
    this->revisionDescription.reset(new Label(String(),
                                               TRANS("...")));
    this->addAndMakeVisible(revisionDescription.get());
    this->revisionDescription->setFont(Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    revisionDescription->setJustificationType(Justification::centred);
    revisionDescription->setEditable(false, false, false);

    this->revisionDate.reset(new Label(String(),
                                        TRANS("...")));
    this->addAndMakeVisible(revisionDate.get());
    this->revisionDate->setFont(Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    revisionDate->setJustificationType(Justification::centred);
    revisionDate->setEditable(false, false, false);

    this->line2.reset(new SeparatorHorizontalFadingReversed());
    this->addAndMakeVisible(line2.get());
    this->line3.reset(new SeparatorHorizontalFading());
    this->addAndMakeVisible(line3.get());
    this->remoteIndicatorImage.reset(new IconComponent(Icons::remote));
    this->addAndMakeVisible(remoteIndicatorImage.get());

    this->localIndicatorImage.reset(new IconComponent(Icons::local));
    this->addAndMakeVisible(localIndicatorImage.get());


    //[UserPreSize]
    const auto &message = this->revision->getMessage();
    const auto timestamp = this->revision->getTimeStamp();

    this->revisionDescription->setText(message, dontSendNotification);
    this->revisionDate->setText(App::getHumanReadableDate(Time(timestamp)), dontSendNotification);

    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->revisionDate->setInterceptsMouseClicks(false, false);
    this->revisionDescription->setInterceptsMouseClicks(false, false);

    switch (this->viewState)
    {
    case VCS::Revision::NoSync:
        this->remoteIndicatorImage->setAlpha(0.15f);
        break;
    case VCS::Revision::ShallowCopy:
        this->localIndicatorImage->setAlpha(0.15f);
        break;
    case VCS::Revision::FullSync:
    default:
        break;
    }
    //[/UserPreSize]

    this->setSize(150, 50);

    //[Constructor]
    //[/Constructor]
}

RevisionComponent::~RevisionComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    revisionDescription = nullptr;
    revisionDate = nullptr;
    line2 = nullptr;
    line3 = nullptr;
    remoteIndicatorImage = nullptr;
    localIndicatorImage = nullptr;

    //[Destructor]
    //[/Destructor]
}

void RevisionComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    g.setColour(findDefaultColour(ColourIDs::VersionControl::outline));

    if (this->isHeadRevision)
    {
        g.drawRoundedRectangle(0.0f, 0.0f, float(this->getWidth()), float(this->getHeight()), 9.000f, 1.0f);
    }

    if (this->isSelected)
    {
        g.fillRoundedRectangle(1.0f, 1.0f, float(this->getWidth() - 2), float(this->getHeight() - 2), 7.000f);
    }
    //[/UserPaint]
}

void RevisionComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    revisionDescription->setBounds((getWidth() / 2) - ((getWidth() - 8) / 2), 1, getWidth() - 8, 20);
    revisionDate->setBounds((getWidth() / 2) - ((getWidth() - 32) / 2), 20, getWidth() - 32, 14);
    line2->setBounds((getWidth() / 2) - ((getWidth() - 16) / 2), 0, getWidth() - 16, 2);
    line3->setBounds((getWidth() / 2) - ((getWidth() - 16) / 2), getHeight() - 2, getWidth() - 16, 2);
    remoteIndicatorImage->setBounds((getWidth() / 2) + -8 - (8 / 2), 37, 8, 8);
    localIndicatorImage->setBounds((getWidth() / 2) + 8 - (8 / 2), 37, 8, 8);
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
    if (this->children.size() > 0) { return this->children.getLast(); }
    return this->wired;
}

RevisionComponent *RevisionComponent::left() const
{
    if (this->children.size() > 0) { return this->children.getFirst(); }
    return this->wired;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RevisionComponent" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="VersionControl &amp;owner, const VCS::Revision::Ptr revision, VCS::Revision::SyncState viewState, bool isHead"
                 variableInitialisers="vcs(owner)&#10;revision(revision),&#10;isSelected(false),&#10;isHeadRevision(isHead),&#10;viewState(viewState),&#10;x(0.f),&#10;y(0.f),&#10;mod(0.f),&#10;shift(0.f),&#10;change(0.f),&#10;number(0),&#10;parent(nullptr),&#10;wired(nullptr),&#10;leftmostSibling(nullptr)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="150" initialHeight="50">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseMove (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="45b178bfb039403" memberName="revisionDescription"
         virtualName="" explicitFocusOrder="0" pos="0Cc 1 8M 20" labelText="..."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="36"/>
  <LABEL name="" id="30ac314958873bc0" memberName="revisionDate" virtualName=""
         explicitFocusOrder="0" pos="0Cc 20 32M 14" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="12.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="36"/>
  <JUCERCOMP name="" id="b33ea1fc25edc01e" memberName="line2" virtualName=""
             explicitFocusOrder="0" pos="0Cc 0 16M 2" sourceFile="../../Themes/SeparatorHorizontalFadingReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="d06175cead2055bc" memberName="line3" virtualName=""
             explicitFocusOrder="0" pos="0Cc 0Rr 16M 2" sourceFile="../../Themes/SeparatorHorizontalFading.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="785e248885091f6f" memberName="remoteIndicatorImage"
                    virtualName="" explicitFocusOrder="0" pos="-8Cc 37 8 8" class="IconComponent"
                    params="Icons::remote"/>
  <GENERICCOMPONENT name="" id="da3b844443a7cf14" memberName="localIndicatorImage"
                    virtualName="" explicitFocusOrder="0" pos="8Cc 37 8 8" class="IconComponent"
                    params="Icons::local"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
