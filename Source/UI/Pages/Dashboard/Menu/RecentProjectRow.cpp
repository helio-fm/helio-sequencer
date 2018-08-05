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

#include "RecentProjectRow.h"

//[MiscUserDefs]

#include "DashboardMenu.h"
#include "HelioTheme.h"
#include "Icons.h"
#include "IconComponent.h"
#include "App.h"


class RecentFileSelectionComponent : public Component
{
public:

    RecentFileSelectionComponent()
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(Colours::white.withAlpha(0.025f));
        g.fillRoundedRectangle (5.0f, 1.0f, static_cast<float> (getWidth() - 10), static_cast<float> (getHeight() - 9), 2.000f);
    }
};

//[/MiscUserDefs]

RecentProjectRow::RecentProjectRow(DashboardMenu &parent, ListBox &parentListBox)
    : DraggingListBoxComponent(parentListBox.getViewport()),
      parentList(parent),
      isSelected(false)
{
    addAndMakeVisible (titleLabel = new Label (String(),
                                               TRANS("...")));
    titleLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType (Justification::centredLeft);
    titleLabel->setEditable (false, false, false);

    addAndMakeVisible (dateLabel = new Label (String(),
                                              TRANS("...")));
    dateLabel->setFont (Font (Font::getDefaultSerifFontName(), 12.00f, Font::plain).withTypefaceStyle ("Regular"));
    dateLabel->setJustificationType (Justification::centredLeft);
    dateLabel->setEditable (false, false, false);

    addAndMakeVisible (activenessImage = new IconComponent (Icons::project));

    addAndMakeVisible (remoteIndicatorImage = new IconComponent (Icons::remote));

    addAndMakeVisible (localIndicatorImage = new IconComponent (Icons::local));


    //[UserPreSize]
    //this->selectionComponent = new RecentFileSelectionComponent();
    //this->addChildComponent(this->selectionComponent);
    //[/UserPreSize]

    setSize (350, 56);

    //[Constructor]
    //[/Constructor]
}

RecentProjectRow::~RecentProjectRow()
{
    //[Destructor_pre]
    //this->selectionComponent = nullptr;
    //[/Destructor_pre]

    titleLabel = nullptr;
    dateLabel = nullptr;
    activenessImage = nullptr;
    remoteIndicatorImage = nullptr;
    localIndicatorImage = nullptr;

    //[Destructor]
    //[/Destructor]
}

void RecentProjectRow::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void RecentProjectRow::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    titleLabel->setBounds (54, 5, getWidth() - 84, 24);
    dateLabel->setBounds (54, 28, getWidth() - 144, 16);
    activenessImage->setBounds (32 - (40 / 2), (getHeight() / 2) + -4 - (40 / 2), 40, 40);
    remoteIndicatorImage->setBounds (getWidth() - -62 - 24, getHeight() - 12 - 24, 24, 24);
    localIndicatorImage->setBounds (getWidth() - -32 - 24, getHeight() - 12 - 24, 24, 24);
    //[UserResized] Add your own custom resize handling here..
    //this->selectionComponent->setBounds(this->getLocalBounds());
    //[/UserResized]
}


//[MiscUserCode]

void RecentProjectRow::setSelected(bool shouldBeSelected)
{
    if (shouldBeSelected)
    {
        if (this->targetFile->isLoaded)
        {
            this->parentList.unloadFile(this->targetFile);
        }
        else
        {
            this->parentList.loadFile(this->targetFile);
        }
    }
}

void RecentProjectRow::updateDescription(bool isLastRow, const RecentFileDescription::Ptr file)
{
    //Logger::writeToLog("Updating info for: " + file->title);
    this->targetFile = file;

    this->titleLabel->setText(this->targetFile->title, dontSendNotification);
    this->dateLabel->setText(App::getHumanReadableDate(Time(this->targetFile->lastModifiedTime)), dontSendNotification);
    this->remoteIndicatorImage->setAlpha(this->targetFile->hasRemoteCopy ? 1.f : 0.4f);
    this->localIndicatorImage->setAlpha(this->targetFile->hasLocalCopy ? 1.f : 0.4f);

    this->activenessImage->setAlpha(this->targetFile->isLoaded ? 1.f : 0.7f);
    //this->titleLabel->setAlpha(this->targetFile->isLoaded ? 1.f : 0.6f);
    //this->dateLabel->setAlpha(this->targetFile->isLoaded ? 1.f : 0.5f);

    this->setAlpha(this->targetFile->isLoaded ? 1.f : 0.55f);
}

Component *RecentProjectRow::createHighlighterComponent()
{
    return new RecentFileSelectionComponent();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RecentProjectRow" template="../../../../Template"
                 componentName="" parentClasses="public DraggingListBoxComponent"
                 constructorParams="DashboardMenu &amp;parent, ListBox &amp;parentListBox"
                 variableInitialisers="DraggingListBoxComponent(parentListBox.getViewport()),&#10;parentList(parent),&#10;isSelected(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="350" initialHeight="56">
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="c261305e2de1ebf2" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="54 5 84M 24" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="a7e8c6a3ddd9ea22" memberName="dateLabel" virtualName=""
         explicitFocusOrder="0" pos="54 28 144M 16" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="12" kerning="0" bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="" id="42f3ed34561654ef" memberName="activenessImage" virtualName=""
                    explicitFocusOrder="0" pos="32c -4Cc 40 40" class="IconComponent"
                    params="Icons::project"/>
  <GENERICCOMPONENT name="" id="785e248885091f6f" memberName="remoteIndicatorImage"
                    virtualName="" explicitFocusOrder="0" pos="-62Rr 12Rr 24 24"
                    class="IconComponent" params="Icons::remote"/>
  <GENERICCOMPONENT name="" id="da3b844443a7cf14" memberName="localIndicatorImage"
                    virtualName="" explicitFocusOrder="0" pos="-32Rr 12Rr 24 24"
                    class="IconComponent" params="Icons::local"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
