/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "ThemeSettingsItem.h"
#include "IconComponent.h"
#include "HighlightingScheme.h"
#include "PianoRoll.h"
#include "Config.h"
#include "ColourIDs.h"

class ThemeSettingsItemHighlighter final : public Component
{
public:

    ThemeSettingsItemHighlighter()
    {
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        const float gridStartX = 218.f;
        const float paintStartY = 4.f;
        const float paintEndY = float(this->getHeight()) - 10.f;
        const float lineStartY = paintStartY;
        const float lineEndY = paintEndY + paintStartY;

        g.setColour(Colours::white.withAlpha(0.025f));
        g.fillRect(0.f, paintStartY, gridStartX, paintEndY);

        g.setColour(Colours::black.withAlpha(0.4f));
        g.drawVerticalLine(1, lineStartY + 1, lineEndY - 1);
        g.drawVerticalLine(this->getWidth() - 2, lineStartY + 1, lineEndY - 1);
        g.drawHorizontalLine(int(lineStartY), 2.f, float(this->getWidth()) - 2.f);
        g.drawHorizontalLine(int(lineEndY) - 1, 2.f, float(this->getWidth()) - 2.f);

        g.setColour(Colours::white.withAlpha(0.2f));
        g.drawVerticalLine(0, lineStartY, lineEndY);
        g.drawVerticalLine(this->getWidth() - 1, lineStartY, lineEndY);
        g.drawHorizontalLine(int(lineStartY) - 1, 1.f, float(this->getWidth()) - 1.f);
        g.drawHorizontalLine(int(lineEndY), 1.f, float(this->getWidth()) - 1.f);
    }
};

ThemeSettingsItem::ThemeSettingsItem(ListBox &parentListBox) :
    DraggingListBoxComponent(parentListBox.getViewport()),
    listBox(parentListBox)
{
    this->setPaintingIsUnclipped(true);

    this->schemeNameLabel = make<Label>();
    this->addAndMakeVisible(this->schemeNameLabel.get());
    this->schemeNameLabel->setFont(Globals::UI::Fonts::M);
    this->schemeNameLabel->setJustificationType(Justification::centredLeft);
}

ThemeSettingsItem::~ThemeSettingsItem() = default;

void ThemeSettingsItem::paint(Graphics &g)
{
    if (this->theme == nullptr)
    {
        return;
    }

    const int gridStartX = 218;
    const int paintStartY = 4;
    const int paintEndY = this->getHeight() - 10;
    const int lineStartY = paintStartY;
    const int lineEndY = paintEndY + paintStartY;

    g.setTiledImageFill(this->rollImage, 0, -3, 1.f);
    g.fillRect(200, paintStartY, this->getWidth() - 200, paintEndY);

    const int barWidth = 64;
    const int dynamicGridSize = 4;
    const int beatWidth = barWidth / dynamicGridSize;

    const Colour barStart = this->theme->findColour(ColourIDs::Roll::barLine);
    const Colour barBevel = this->theme->findColour(ColourIDs::Roll::barLineBevel);
    const Colour beatStart = this->theme->findColour(ColourIDs::Roll::beatLine);
    const Colour snapStart = this->theme->findColour(ColourIDs::Roll::snapLine);

    int i = 2;
    const int j = this->getWidth() / barWidth;
    while (i <= j)
    {
        const int startX1 = barWidth * i;

        g.setColour(barStart);
        g.drawVerticalLine(startX1, float(lineStartY), float(lineEndY));

        g.setColour(barBevel);
        g.drawVerticalLine(startX1 + 1, float(lineStartY), float(lineEndY));

        g.setColour(beatStart);
        g.drawVerticalLine((barWidth * i + beatWidth * 2), float(lineStartY), float(lineEndY));

        g.setColour(snapStart);
        g.drawVerticalLine((barWidth * i + beatWidth), float(lineStartY), float(lineEndY));
        g.drawVerticalLine((barWidth * i + beatWidth * 3), float(lineStartY), float(lineEndY));

        i++;
    }

    g.setColour(this->colours->getPageFillColour());
    g.fillRect(0, paintStartY, gridStartX, paintEndY);

    // Outer glow
    g.setColour(this->colours->getPageFillColour().brighter(0.25f));
    g.drawVerticalLine(0, float(lineStartY), float(lineEndY));
    g.drawVerticalLine(this->getWidth() - 1, float(lineStartY), float(lineEndY));
    g.drawHorizontalLine(lineStartY - 1, 1.f, float(this->getWidth()) - 1.f);
    g.drawHorizontalLine(lineEndY, 1.f, float(this->getWidth()) - 1.f);

    // Inner shadow
    g.setColour(this->colours->getPageFillColour().darker(0.05f));
    g.drawVerticalLine(1, float(lineStartY), float(lineEndY));
    g.drawVerticalLine(this->getWidth() - 2, float(lineStartY), float(lineEndY));
    g.drawHorizontalLine(lineStartY, 1.f, float(this->getWidth()) - 1.f);
    g.drawHorizontalLine(lineEndY - 1, 1.f, float(this->getWidth()) - 1.f);

    // Roll shadow left
    g.setGradientFill(ColourGradient(Colour(0x0c000000), float(gridStartX), 0.f,
        Colours::transparentBlack, float(gridStartX) + 20.f, 0.f, false));
    g.fillRect(gridStartX, paintStartY, 20, paintEndY);

    g.setGradientFill(ColourGradient(Colour(0x0c000000), float(gridStartX), 0.f,
        Colours::transparentBlack, float(gridStartX) + 10.f, 0.f, false));
    g.fillRect(gridStartX, paintStartY, 10, paintEndY);

    // Roll shadow right
    g.setGradientFill(ColourGradient(Colour(0x0c000000), float(this->getWidth()), 0.f,
        Colours::transparentBlack, float(this->getWidth()) - 20.f, 0.f, false));
    g.fillRect(this->getWidth() - 20, paintStartY, 20, paintEndY);

    g.setGradientFill(ColourGradient(Colour(0x0c000000), float(this->getWidth()), 0.f,
        Colours::transparentBlack, float(this->getWidth()) - 10.f, 0.f, false));
    g.fillRect(this->getWidth() - 10, paintStartY, 12, paintEndY);

    // Separators
    g.setColour(Colour(0x0f000000));
    g.drawVerticalLine(gridStartX, float(lineStartY), float(lineEndY));

    g.setColour(Colour(0x0bffffff));
    g.drawVerticalLine(gridStartX - 1, float(lineStartY), float(lineEndY));

    g.setColour(Colours::black);
    Icons::drawImageRetinaAware(this->icon1, g, 17, (this->getHeight() / 2) - 1);

    g.setColour(Colours::black.withAlpha(this->isCurrentTheme ? 1.f : 0.5f));
    Icons::drawImageRetinaAware(this->icon2, g, 201, (this->getHeight() / 2) - 1);
}

void ThemeSettingsItem::resized()
{
    this->schemeNameLabel->setBounds(26, 2, 180, this->getHeight() - 6);
}

void ThemeSettingsItem::applyTheme(const ColourScheme::Ptr colours)
{
    auto &ht = dynamic_cast<HelioTheme &>(LookAndFeel::getDefaultLookAndFeel());
    App::Config().getColourSchemes()->setCurrent(colours);
    ht.initColours(colours);
    SafePointer<Component> window = this->getTopLevelComponent();
    if (window != nullptr)
    {
        window->resized();
        window->repaint();
    }
    App::recreateLayout();
}

void ThemeSettingsItem::setSelected(bool shouldBeSelected)
{
    if (shouldBeSelected && !this->isCurrentTheme)
    {
        this->applyTheme(this->colours);
    }
}

void ThemeSettingsItem::updateDescription(bool isLastRowInList, bool isCurrent, const ColourScheme::Ptr colours)
{
    if (this->colours != nullptr &&
        this->colours->getResourceId() == colours->getResourceId() &&
        this->isCurrentTheme == isCurrent)
    {
        return;
    }

    this->isCurrentTheme = isCurrent;

    this->colours = colours;
    this->theme = make<HelioTheme>();
    this->theme->initColours(colours);

    this->schemeNameLabel->setText(colours->getName(), dontSendNotification);
    this->schemeNameLabel->setColour(Label::textColourId, this->theme->findColour(Label::textColourId));

    this->rollImage = HighlightingScheme::renderRowsPattern(*this->theme,
        Temperament::makeTwelveToneEqualTemperament(), Scale::makeNaturalMajorScale(),
        0, PianoRoll::minRowHeight);

    this->icon1 = Icons::renderForTheme(*this->theme, this->isCurrentTheme ? Icons::apply : Icons::pianoTrack, 20);
    this->icon2 = Icons::renderForTheme(*this->theme, Icons::submenu, 20);

    this->repaint();
}

Component *ThemeSettingsItem::createHighlighterComponent()
{
    return new ThemeSettingsItemHighlighter();
}
