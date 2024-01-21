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
        this->setPaintingIsUnclipped(true);

        this->iconComponent = make<IconComponent>(Icons::apply);
        this->addAndMakeVisible(this->iconComponent.get());
        this->iconComponent->setIconAlphaMultiplier(0.7f);
    }

    void resized() override
    {
        constexpr auto size = 28;
        this->iconComponent->setBounds(3,
            (this->getHeight() / 2) - (size / 2) - 1, size, size);
    }

private:

    UniquePointer<IconComponent> iconComponent;
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

    this->selectionComponent = make<ThemeSettingsItemSelection>();
    this->addChildComponent(this->selectionComponent.get());
}

ThemeSettingsItem::~ThemeSettingsItem() = default;

void ThemeSettingsItem::paint(Graphics &g)
{
    if (this->theme == nullptr)
    {
        return;
    }

    const float paintStartY = 4.f;
    const float paintEndY = float(this->getHeight()) - 10.f;
    const float lineStartY = paintStartY;
    const float lineEndY = paintEndY + paintStartY;

    g.setTiledImageFill(this->rollImage, 0, -3, 1.f);
    g.fillRect(200.f, paintStartY, float(this->getWidth()) - 200.f, paintEndY);

    const int barWidth = 64;
    const int dynamicGridSize = 4;
    int i = int(paintStartY / barWidth) + 2;
    const int j = this->getWidth() / barWidth;
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

    g.setColour(this->colours->getPageFillColour());
    g.fillRect(41.f, paintStartY, 207.f, paintEndY);

    // Outer glow
    g.setColour (this->colours->getPageFillColour().brighter(0.2f));
    g.drawVerticalLine(41, lineStartY, lineEndY);
    g.drawVerticalLine(this->getWidth() - 1, lineStartY, lineEndY);
    g.drawHorizontalLine(int(lineStartY) - 1, 42.f, float(this->getWidth()) - 1.f);
    g.drawHorizontalLine(int(lineEndY), 42.f, float(this->getWidth()) - 1.f);

    // Inner shadow
    g.setColour(this->colours->getPageFillColour().darker(0.05f));
    g.drawVerticalLine(42, lineStartY, lineEndY);
    g.drawVerticalLine(this->getWidth() - 2, lineStartY, lineEndY);
    g.drawHorizontalLine(int(lineStartY), 42, float(this->getWidth()) - 1.f);
    g.drawHorizontalLine(int(lineEndY) - 1, 42, float(this->getWidth()) - 1.f);

    // Roll shadow left
    g.setGradientFill(ColourGradient(Colour(0x15000000), 248.0f, 0.0f,
        Colour(0x00000000), 268.0f, 0.0f,  false));
    g.fillRect(248.f, paintStartY, 20.f, paintEndY);

    g.setGradientFill(ColourGradient (Colour (0x15000000), 248.0f, 0.0f,
        Colour (0x00000000), 260.0f, 0.0f, false));
    g.fillRect(248.f, paintStartY, 12.f, paintEndY);

    // Roll shadow right
    g.setGradientFill(ColourGradient(Colour(0x15000000), float(this->getWidth()), 0.0f,
        Colour(0x00000000), float(this->getWidth()) - 20.f, 0.0f, false));
    g.fillRect(float(this->getWidth()) - 20.f, paintStartY, 20.f, paintEndY);

    g.setGradientFill(ColourGradient(Colour(0x15000000), float(this->getWidth()), 0.0f,
        Colour(0x00000000), float(this->getWidth()) - 12.f, 0.0f, false));
    g.fillRect(float(this->getWidth()) - 12.f, paintStartY, 12.f, paintEndY);

    // Separators
    g.setColour(Colour(0x0f000000));
    g.drawVerticalLine(248, lineStartY, lineEndY);

    g.setColour(Colour(0x0bffffff));
    g.drawVerticalLine(247, lineStartY, lineEndY);

    g.setColour(Colours::black);
    Icons::drawImageRetinaAware(this->icon1, g, 48 + 10, (this->getHeight() / 2) - 1);
    Icons::drawImageRetinaAware(this->icon2, g, 220 + 10, (this->getHeight() / 2) - 1);
}

void ThemeSettingsItem::resized()
{
    this->schemeNameLabel->setBounds(67, 2, 174, this->getHeight() - 6);
    this->selectionComponent->setBounds(this->getLocalBounds());
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
        this->selectionAnimator.fadeIn(this->selectionComponent.get(), Globals::UI::fadeInShort);
    }
    else
    {
        this->selectionAnimator.fadeOut(this->selectionComponent.get(), Globals::UI::fadeOutShort);
    }

    if (this->colours != nullptr &&
        this->colours->getResourceId() == colours->getResourceId())
    {
        return;
    }

    this->colours = colours;
    this->theme = make<HelioTheme>();
    this->theme->initColours(colours);

    this->schemeNameLabel->setText("\"" + colours->getName() + "\"", dontSendNotification);
    this->schemeNameLabel->setColour(Label::textColourId, this->theme->findColour(Label::textColourId));

    this->rollImage = HighlightingScheme::renderRowsPattern(*this->theme,
        Temperament::getTwelveToneEqualTemperament(), Scale::makeNaturalMajorScale(),
        0, PianoRoll::minRowHeight);

    this->icon1 = Icons::renderForTheme(*this->theme, Icons::pianoTrack, 20);
    this->icon2 = Icons::renderForTheme(*this->theme, Icons::submenu, 20);

    this->repaint();
}

Component *ThemeSettingsItem::createHighlighterComponent()
{
    return new ThemeSettingsItemHighlighter();
}

