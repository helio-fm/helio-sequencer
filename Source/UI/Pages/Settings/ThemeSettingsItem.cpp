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

#include "ThemeSettingsItem.h"

//[MiscUserDefs]
#include "HelioTheme.h"
#include "ProjectNode.h"
#include "SerializationKeys.h"
#include "IconComponent.h"
#include "Workspace.h"
#include "PianoRoll.h"
#include "Config.h"
#include "SettingsListItemHighlighter.h"
#include "SettingsListItemSelection.h"
#include "ColourIDs.h"

class ThemeSettingsItemHighlighter final : public Component
{
public:

    void paint(Graphics &g) override
    {
        const float paintStartY = 4.f;
        const float paintEndY = float(this->getHeight()) - 10.f;
        const float lineStartY = paintStartY;
        const float lineEndY = paintEndY + paintStartY;

        g.setColour(Colours::white.withAlpha(0.2f));
        g.drawVerticalLine(41, lineStartY, lineEndY);
        g.drawVerticalLine(this->getWidth() - 1, lineStartY, lineEndY);
        g.drawHorizontalLine(int(lineStartY) - 1, 42.f, float(this->getWidth()) - 1.f);
        g.drawHorizontalLine(int(lineEndY), 42.f, float(this->getWidth()) - 1.f);
    }
};

class ThemeSettingsItemSelection final : public Component
{
public:

    ThemeSettingsItemSelection()
    {
        addAndMakeVisible(this->iconComponent = new IconComponent(Icons::apply));
        this->iconComponent->setAlpha(0.6f);
    }

    void resized() override
    {
        this->iconComponent->setBounds(3, (getHeight() / 2) + -1 - (28 / 2), 28, 28);
    }

private:

    ScopedPointer<IconComponent> iconComponent;
};

//[/MiscUserDefs]

ThemeSettingsItem::ThemeSettingsItem(ListBox &parentListBox)
    : DraggingListBoxComponent(parentListBox.getViewport()),
      listBox(parentListBox)
{
    addAndMakeVisible (schemeNameLabel = new Label (String(),
                                                    TRANS("...")));
    schemeNameLabel->setFont (Font (Font::getDefaultSerifFontName(), 18.00f, Font::plain).withTypefaceStyle ("Regular"));
    schemeNameLabel->setJustificationType (Justification::centredLeft);
    schemeNameLabel->setEditable (false, false, false);


    //[UserPreSize]
    this->selectionComponent = new ThemeSettingsItemSelection();
    this->addChildComponent(this->selectionComponent);
    //[/UserPreSize]

    setSize (400, 40);

    //[Constructor]
    //[/Constructor]
}

ThemeSettingsItem::~ThemeSettingsItem()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    schemeNameLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ThemeSettingsItem::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    if (this->theme != nullptr)
    {
        const float paintStartY = 4.f;
        const float paintEndY = float(this->getHeight()) - 10.f;
        const float lineStartY = paintStartY;
        const float lineEndY = paintEndY + paintStartY;

        g.setTiledImageFill(this->rollImage, 0, -3, 1.f);
        g.fillRect(200.f, paintStartY, float(this->getWidth()) - 200.f, paintEndY);

        const int barWidth = 64;
        const int dynamicGridSize = 4;
        int i = int(paintStartY / barWidth) + 2;
        const int j = int(this->getWidth() / barWidth);
        const int beatWidth = barWidth / dynamicGridSize;

        const Colour barStart = this->theme->findColour(ColourIDs::Roll::barLine);
        const Colour barBevel = this->theme->findColour(ColourIDs::Roll::barLineBevel);
        const Colour beatStart = this->theme->findColour(ColourIDs::Roll::beatLine);
        const Colour snapStart = this->theme->findColour(ColourIDs::Roll::snapLine);

        while (i <= j)
        {
            const int startX1 = barWidth * i;

            g.setColour(barStart);
            g.drawVerticalLine(startX1, lineStartY, lineEndY);

            g.setColour(barBevel);
            g.drawVerticalLine(startX1 + 1, lineStartY, lineEndY);

            g.setColour(beatStart);
            g.drawVerticalLine((barWidth * i + beatWidth * 2), lineStartY, lineEndY);

            g.setColour(snapStart);
            g.drawVerticalLine((barWidth * i + beatWidth), lineStartY, lineEndY);
            g.drawVerticalLine((barWidth * i + beatWidth * 3), lineStartY, lineEndY);

            i++;
        }

        g.setColour(this->colours->getPrimaryGradientColourA());
        g.fillRect(41.f, paintStartY, 207.f, paintEndY);

        // Outer glow
        g.setColour (this->colours->getPrimaryGradientColourA().brighter(0.2f));
        g.drawVerticalLine(41, lineStartY, lineEndY);
        g.drawVerticalLine(this->getWidth() - 1, lineStartY, lineEndY);
        g.drawHorizontalLine(int(lineStartY) - 1, 42.f, float(this->getWidth()) - 1.f);
        g.drawHorizontalLine(int(lineEndY), 42.f, float(this->getWidth()) - 1.f);

        // Inner shadow
        g.setColour(this->colours->getPrimaryGradientColourA().darker(0.05f));
        g.drawVerticalLine(42, lineStartY, lineEndY);
        g.drawVerticalLine(this->getWidth() - 2, lineStartY, lineEndY);
        g.drawHorizontalLine(int(lineStartY), 42, float(this->getWidth()) - 1.f);
        g.drawHorizontalLine(int(lineEndY) - 1, 42, float(this->getWidth()) - 1.f);

        // Roll shadow left
        g.setGradientFill (ColourGradient (Colour (0x15000000), 248.0f, 0.0f,
                                           Colour (0x00000000), 268.0f, 0.0f,  false));
        g.fillRect (248.f, paintStartY, 20.f, paintEndY);

        g.setGradientFill (ColourGradient (Colour (0x15000000), 248.0f, 0.0f,
                                           Colour (0x00000000), 260.0f, 0.0f, false));
        g.fillRect (248.f, paintStartY, 12.f, paintEndY);

        // Roll shadow right
        g.setGradientFill(ColourGradient(Colour(0x15000000), float(this->getWidth()), 0.0f,
            Colour(0x00000000), float(this->getWidth()) - 20.f, 0.0f, false));
        g.fillRect(float(this->getWidth()) - 20.f, paintStartY, 20.f, paintEndY);

        g.setGradientFill(ColourGradient(Colour(0x15000000), float(this->getWidth()), 0.0f,
            Colour(0x00000000), float(this->getWidth()) - 12.f, 0.0f, false));
        g.fillRect(float(this->getWidth()) - 12.f, paintStartY, 12.f, paintEndY);

        // Separators
        g.setColour (Colour (0x0f000000));
        g.drawVerticalLine(248, lineStartY, lineEndY);

        g.setColour (Colour (0x0bffffff));
        g.drawVerticalLine(247, lineStartY, lineEndY);

        g.setColour(Colours::black);
        Image image1(Icons::renderForTheme(*this->theme, Icons::pianoTrack, 20));
        Image image2(Icons::renderForTheme(*this->theme, Icons::submenu, 20));
        Icons::drawImageRetinaAware(image1, g, 48 + 10, (this->getHeight() / 2) - 1);
        Icons::drawImageRetinaAware(image2, g, 220 + 10, (this->getHeight() / 2) - 1);
    }

#if 0
    //[/UserPrePaint]

    {
        int x = 41, y = 4, width = 207, height = getHeight() - 10;
        Colour fillColour = Colour (0xff2a50a5);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        float x = 40.0f, y = 3.0f, width = static_cast<float> (getWidth() - 43), height = static_cast<float> (getHeight() - 8);
        Colour strokeColour = Colour (0xff234aff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 6.000f, 1.000f);
    }

    {
        int x = 248, y = 4, width = 20, height = getHeight() - 10;
        Colour fillColour1 = Colour (0x15000000), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       248.0f - 248.0f + x,
                                       0.0f - 4.0f + y,
                                       fillColour2,
                                       268.0f - 248.0f + x,
                                       0.0f - 4.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
    }

    {
        int x = 248, y = 4, width = 12, height = getHeight() - 10;
        Colour fillColour1 = Colour (0x15000000), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       248.0f - 248.0f + x,
                                       0.0f - 4.0f + y,
                                       fillColour2,
                                       260.0f - 248.0f + x,
                                       0.0f - 4.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
    }

    {
        int x = 248, y = 4, width = 1, height = getHeight() - 10;
        Colour fillColour = Colour (0x0b000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 247, y = 4, width = 1, height = getHeight() - 10;
        Colour fillColour = Colour (0x09ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
#endif
    //[/UserPaint]
}

void ThemeSettingsItem::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    schemeNameLabel->setBounds (67, 2, 174, getHeight() - 6);
    //[UserResized] Add your own custom resize handling here..
    this->selectionComponent->setBounds(this->getLocalBounds());
    //[/UserResized]
}


//[MiscUserCode]
void ThemeSettingsItem::applyTheme(const ColourScheme::Ptr colours)
{
    if (auto *ht = dynamic_cast<HelioTheme *>(&this->getLookAndFeel()))
    {
        App::Config().getColourSchemes()->setCurrent(colours);
        ht->initColours(colours);
        ht->updateBackgroundRenders(true);
        SafePointer<Component> window = this->getTopLevelComponent();
        if (window != nullptr)
        {
            window->resized();
            window->repaint();
        }
        App::recreateLayout();
    }
}

void ThemeSettingsItem::setSelected(bool shouldBeSelected)
{
    if (shouldBeSelected)
    {
        this->applyTheme(this->colours);
    }
}

void ThemeSettingsItem::updateDescription(bool isLastRowInList,
    bool isCurrentTheme, const ColourScheme::Ptr colours)
{
    if (isCurrentTheme)
    {
        this->selectionAnimator.fadeIn(this->selectionComponent, 150);
    }
    else
    {
        this->selectionAnimator.fadeOut(this->selectionComponent, 50);
    }

    if (this->colours != nullptr &&
        this->colours->getResourceId() == colours->getResourceId())
    {
        return;
    }

    this->colours = colours;
    this->theme = new HelioTheme();
    this->theme->initColours(colours);

    this->schemeNameLabel->setText("\"" + colours->getName() + "\"", dontSendNotification);
    this->schemeNameLabel->setColour(Label::textColourId, this->theme->findColour(Label::textColourId));

    this->rollImage = PianoRoll::renderRowsPattern(*this->theme, Scale::getNaturalMajorScale(), 0, 7);

    this->repaint();
}

Component *ThemeSettingsItem::createHighlighterComponent()
{
    return new ThemeSettingsItemHighlighter();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ThemeSettingsItem" template="../../../Template"
                 componentName="" parentClasses="public DraggingListBoxComponent"
                 constructorParams="ListBox &amp;parentListBox" variableInitialisers="DraggingListBoxComponent(parentListBox.getViewport()),&#10;listBox(parentListBox)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="400" initialHeight="40">
  <BACKGROUND backgroundColour="0">
    <RECT pos="41 4 207 10M" fill="solid: ff2a50a5" hasStroke="0"/>
    <ROUNDRECT pos="40 3 43M 8M" cornerSize="6.00000000000000000000" fill="solid: 0"
               hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: ff234aff"/>
    <RECT pos="248 4 20 10M" fill="linear: 248 0, 268 0, 0=15000000, 1=0"
          hasStroke="0"/>
    <RECT pos="248 4 12 10M" fill="linear: 248 0, 260 0, 0=15000000, 1=0"
          hasStroke="0"/>
    <RECT pos="248 4 1 10M" fill="solid: b000000" hasStroke="0"/>
    <RECT pos="247 4 1 10M" fill="solid: 9ffffff" hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="" id="c261305e2de1ebf2" memberName="schemeNameLabel" virtualName=""
         explicitFocusOrder="0" pos="67 2 174 6M" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
