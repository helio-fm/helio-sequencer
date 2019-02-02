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

class RecentFileSelectionComponent final : public Component
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
      isFileLoaded(false),
      isSelected(false)
{
    this->titleLabel.reset(new Label(String(),
                                      TRANS("...")));
    this->addAndMakeVisible(titleLabel.get());
    this->titleLabel->setFont(Font (Font::getDefaultSerifFontName(), 18.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType(Justification::centredRight);
    titleLabel->setEditable(false, false, false);

    this->dateLabel.reset(new Label(String(),
                                     TRANS("...")));
    this->addAndMakeVisible(dateLabel.get());
    this->dateLabel->setFont(Font (Font::getDefaultSerifFontName(), 12.00f, Font::plain).withTypefaceStyle ("Regular"));
    dateLabel->setJustificationType(Justification::centredRight);
    dateLabel->setEditable(false, false, false);

    this->activenessImage.reset(new IconComponent(Icons::project));
    this->addAndMakeVisible(activenessImage.get());

    this->remoteIndicatorImage.reset(new IconComponent(Icons::remote));
    this->addAndMakeVisible(remoteIndicatorImage.get());

    this->localIndicatorImage.reset(new IconComponent(Icons::local));
    this->addAndMakeVisible(localIndicatorImage.get());


    //[UserPreSize]
    //this->selectionComponent = new RecentFileSelectionComponent();
    //this->addChildComponent(this->selectionComponent);
    //[/UserPreSize]

    this->setSize(350, 56);

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

    titleLabel->setBounds(0, 5, getWidth() - 46, 24);
    dateLabel->setBounds(0, 27, getWidth() - 82, 16);
    activenessImage->setBounds(getWidth() - 8 - 36, (getHeight() / 2) + -4 - (36 / 2), 36, 36);
    remoteIndicatorImage->setBounds(getWidth() - 70 - 8, getHeight() - 16 - 8, 8, 8);
    localIndicatorImage->setBounds(getWidth() - 54 - 8, getHeight() - 16 - 8, 8, 8);
    //[UserResized] Add your own custom resize handling here..
    //this->selectionComponent->setBounds(this->getLocalBounds());
    //[/UserResized]
}


//[MiscUserCode]

void RecentProjectRow::setSelected(bool shouldBeSelected)
{
    if (shouldBeSelected)
    {
        if (this->isFileLoaded)
        {
            this->parentList.unloadFile(this->targetFile);
        }
        else
        {
            this->parentList.loadFile(this->targetFile);
        }
    }
}

void RecentProjectRow::updateDescription(const RecentProjectInfo::Ptr file, bool isLoaded, bool isLastRow)
{
    this->targetFile = file;
    this->isFileLoaded = isLoaded;

    this->titleLabel->setText(this->targetFile->getTitle(), dontSendNotification);
    this->dateLabel->setText(App::getHumanReadableDate(this->targetFile->getUpdatedAt()), dontSendNotification);
    this->remoteIndicatorImage->setAlpha(this->targetFile->hasRemoteCopy() ? 1.f : 0.3f);
    this->localIndicatorImage->setAlpha(this->targetFile->hasLocalCopy() ? 1.f : 0.3f);

    this->activenessImage->setAlpha(this->isFileLoaded ? 1.f : 0.7f);
    //this->titleLabel->setAlpha(this->targetFile->isLoaded ? 1.f : 0.6f);
    //this->dateLabel->setAlpha(this->targetFile->isLoaded ? 1.f : 0.5f);

    this->setAlpha(this->isFileLoaded ? 1.f : 0.5f);
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
                 variableInitialisers="DraggingListBoxComponent(parentListBox.getViewport()),&#10;parentList(parent),&#10;isFileLoaded(false),&#10;isSelected(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="350" initialHeight="56">
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="c261305e2de1ebf2" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="0 5 46M 24" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="34"/>
  <LABEL name="" id="a7e8c6a3ddd9ea22" memberName="dateLabel" virtualName=""
         explicitFocusOrder="0" pos="0 27 82M 16" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="12.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="34"/>
  <GENERICCOMPONENT name="" id="42f3ed34561654ef" memberName="activenessImage" virtualName=""
                    explicitFocusOrder="0" pos="8Rr -4Cc 36 36" class="IconComponent"
                    params="Icons::project"/>
  <GENERICCOMPONENT name="" id="785e248885091f6f" memberName="remoteIndicatorImage"
                    virtualName="" explicitFocusOrder="0" pos="70Rr 16Rr 8 8" class="IconComponent"
                    params="Icons::remote"/>
  <GENERICCOMPONENT name="" id="da3b844443a7cf14" memberName="localIndicatorImage"
                    virtualName="" explicitFocusOrder="0" pos="54Rr 16Rr 8 8" class="IconComponent"
                    params="Icons::local"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
