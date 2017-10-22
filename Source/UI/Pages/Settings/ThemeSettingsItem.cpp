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
#include "ProjectTreeItem.h"
#include "SerializationKeys.h"
#include "IconComponent.h"
#include "Workspace.h"
#include "PianoRoll.h"
#include "Config.h"
#include "App.h"

#include "SettingsListItemHighlighter.h"
#include "SettingsListItemSelection.h"
#include "ColourSchemeManager.h"

class ThemeSettingsItemHighlighter : public Component
{
public:

    void paint(Graphics& g) override
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

class ThemeSettingsItemSelection : public Component
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
    schemeNameLabel->setColour (TextEditor::textColourId, Colours::black);
    schemeNameLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (separator = new SeparatorHorizontal());

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
    separator = nullptr;

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

        const Colour barStart = this->theme->findColour(HybridRoll::barLineColourId);
        const Colour barBevel = this->theme->findColour(HybridRoll::barLineBevelColourId);
        const Colour beatStart = this->theme->findColour(HybridRoll::beatLineColourId);
        const Colour snapStart = this->theme->findColour(HybridRoll::snapLineColourId);

        while (i <= j)
        {
            // show every x'th
            if (i % 1 == 0)
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
            }

            i++;
        }

        g.setColour(this->colours.getPrimaryGradientColourA());
        g.fillRect(41.f, paintStartY, 207.f, paintEndY);

        // Outer glow
        g.setColour (this->colours.getPrimaryGradientColourA().brighter(0.2f));
        g.drawVerticalLine(41, lineStartY, lineEndY);
        g.drawVerticalLine(this->getWidth() - 1, lineStartY, lineEndY);
        g.drawHorizontalLine(int(lineStartY) - 1, 42.f, float(this->getWidth()) - 1.f);
        g.drawHorizontalLine(int(lineEndY), 42.f, float(this->getWidth()) - 1.f);

        // Inner shadow
        g.setColour(this->colours.getPrimaryGradientColourA().darker(0.05f));
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
        Image image1(Icons::findByName(Icons::layer, 20, *this->theme));
        Image image2(Icons::findByName(Icons::right, 20, *this->theme));
        Icons::drawImageRetinaAware(image1, g, 48 + 10, (this->getHeight() / 2) - 1);
        Icons::drawImageRetinaAware(image2, g, 220 + 10, (this->getHeight() / 2) - 1);
    }

#if 0
    //[/UserPrePaint]

    g.setColour (Colour (0xff2a50a5));
    g.fillRect (41, 4, 207, getHeight() - 10);

    g.setColour (Colour (0xff234aff));
    g.drawRoundedRectangle (40.0f, 3.0f, static_cast<float> (getWidth() - 43), static_cast<float> (getHeight() - 8), 6.000f, 1.000f);

    g.setGradientFill (ColourGradient (Colour (0x15000000),
                                       248.0f, 0.0f,
                                       Colour (0x00000000),
                                       268.0f, 0.0f,
                                       false));
    g.fillRect (248, 4, 20, getHeight() - 10);

    g.setGradientFill (ColourGradient (Colour (0x15000000),
                                       248.0f, 0.0f,
                                       Colour (0x00000000),
                                       260.0f, 0.0f,
                                       false));
    g.fillRect (248, 4, 12, getHeight() - 10);

    g.setColour (Colour (0x0b000000));
    g.fillRect (248, 4, 1, getHeight() - 10);

    g.setColour (Colour (0x09ffffff));
    g.fillRect (247, 4, 1, getHeight() - 10);

    //[UserPaint] Add your own custom painting code here..
#endif
    //[/UserPaint]
}

void ThemeSettingsItem::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    schemeNameLabel->setBounds (67, 2, 174, getHeight() - 6);
    separator->setBounds (56, getHeight() - -8 - 2, getWidth() - 80, 2);
    //[UserResized] Add your own custom resize handling here..
    this->selectionComponent->setBounds(this->getLocalBounds());
    //[/UserResized]
}


//[MiscUserCode]
void ThemeSettingsItem::applyTheme(const ColourScheme &colours)
{
    if (HelioTheme *ht = dynamic_cast<HelioTheme *>(&this->getLookAndFeel()))
    {
        ColourSchemeManager::getInstance().setCurrentScheme(this->colours);
        ht->initColours(colours);
        ht->updateBackgroundRenders(true);
        if (this->getTopLevelComponent() != nullptr)
        {
            this->getTopLevelComponent()->resized();
            this->getTopLevelComponent()->repaint();
        }
        App::Helio()->recreateLayout();
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
    bool isCurrentTheme, const ColourScheme &colours)
{
    this->separator->setVisible(!isLastRowInList);
    if (isCurrentTheme)
    {
        this->selectionAnimator.fadeIn(this->selectionComponent, 150);
    }
    else
    {
        this->selectionAnimator.fadeOut(this->selectionComponent, 50);
    }

    if (this->colours == colours)
    {
        return;
    }

    this->colours = colours;
    this->theme = new HelioTheme();
    this->theme->initColours(colours);

    this->schemeNameLabel->setText("\"" + colours.getName() + "\"", dontSendNotification);
    this->schemeNameLabel->setColour(Label::textColourId, this->theme->findColour(Label::textColourId));

    this->rollImage = PianoRoll::renderRowsPattern(*this->theme, 7);
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

<JUCER_COMPONENT documentType="Component" className="ThemeSettingsItem" template="../../Template"
                 componentName="" parentClasses="public DraggingListBoxComponent"
                 constructorParams="ListBox &amp;parentListBox" variableInitialisers="DraggingListBoxComponent(parentListBox.getViewport()),&#10;listBox(parentListBox)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="400" initialHeight="40">
  <BACKGROUND backgroundColour="0">
    <RECT pos="41 4 207 10M" fill="solid: ff2a50a5" hasStroke="0"/>
    <ROUNDRECT pos="40 3 43M 8M" cornerSize="6" fill="solid: 0" hasStroke="1"
               stroke="1, mitered, butt" strokeColour="solid: ff234aff"/>
    <RECT pos="248 4 20 10M" fill="linear: 248 0, 268 0, 0=15000000, 1=0"
          hasStroke="0"/>
    <RECT pos="248 4 12 10M" fill="linear: 248 0, 260 0, 0=15000000, 1=0"
          hasStroke="0"/>
    <RECT pos="248 4 1 10M" fill="solid: b000000" hasStroke="0"/>
    <RECT pos="247 4 1 10M" fill="solid: 9ffffff" hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="" id="c261305e2de1ebf2" memberName="schemeNameLabel" virtualName=""
         explicitFocusOrder="0" pos="67 2 174 6M" edTextCol="ff000000"
         edBkgCol="0" labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="18"
         kerning="0" bold="0" italic="0" justification="33"/>
  <JUCERCOMP name="" id="6f5a73e394d91c2a" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="56 -8Rr 80M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
