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

#include "Common.h"
#include "MenuPanel.h"

#include "MainLayout.h"
#include "ProjectNode.h"
#include "PlayerThread.h"
#include "Icons.h"
#include "HybridRoll.h"
#include "MidiSequence.h"
#include "MenuItemComponent.h"

MenuPanel::MenuPanel() :
    lastAnimationType(AnimationType::None),
    shouldResizeToFitContent(false)
{
    this->setInterceptsMouseClicks(false, true);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->listBox.reset(new ListBox());
    this->addAndMakeVisible(this->listBox.get());

    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        Component *c = this->getChildComponent(i);
        c->setMouseClickGrabsKeyboardFocus(false);
    }
}

void MenuPanel::resized()
{
    this->listBox->setBounds(this->getMenuBounds());

    // Parent component may reposition menu at any time,
    // need to update content bounds animation:
    if (this->commandDescriptions.size() != 0)
    {
        this->updateContent(this->commandDescriptions,
            this->lastAnimationType, this->shouldResizeToFitContent,
            this->customFooter.get());
    }
}

void MenuPanel::handleCommandMessage (int commandId)
{
    if (this->getParentComponent() != nullptr)
    {
        // The default behavior is to pass command up the hierarchy,
        // but subclasses may override handleCommandMessage to process the command instead.
        this->getParentComponent()->postCommandMessage(commandId);
    }
}

class ColourSorter
{
public:
    static int compareElements(const String &a, const String &b)
    {
        const auto ca = Colour::fromString(a);
        const auto cb = Colour::fromString(b);
        return (a < b) ? -1 : (a > b ? 1 : 0);
    }
};

// Hardcoded for now
StringPairArray MenuPanel::getColoursList()
{
    StringPairArray c;
    //c.set(TRANS("colours::none"),           Colours::transparentWhite.toString());
    //c.set(TRANS("colours::black"),          Colours::black.toString());
    c.set(TRANS("colours::white"),          Colours::white.toString());
    c.set(TRANS("colours::red"),            Colours::red.toString());
    c.set(TRANS("colours::crimson"),        Colours::crimson.toString());
    c.set(TRANS("colours::deeppink"),       Colours::deeppink.toString());
    c.set(TRANS("colours::darkviolet"),     Colours::darkviolet.toString());
    c.set(TRANS("colours::blueviolet"),     Colours::blueviolet.toString());
    c.set(TRANS("colours::blue"),           Colours::blue.toString());
    c.set(TRANS("colours::royalblue"),      Colours::royalblue.toString());
    c.set(TRANS("colours::springgreen"),    Colours::springgreen.toString());
    c.set(TRANS("colours::lime"),           Colours::lime.toString());
    c.set(TRANS("colours::greenyellow"),    Colours::greenyellow.toString());
    c.set(TRANS("colours::gold"),           Colours::gold.toString());
    c.set(TRANS("colours::darkorange"),     Colours::darkorange.toString());
    c.set(TRANS("colours::tomato"),         Colours::tomato.toString());
    c.set(TRANS("colours::orangered"),      Colours::orangered.toString());
    return c;
}

#define ANIM_TIME_MS 175
#define FADE_ALPHA 0.5f
#define TOPLEVEL_HEIGHT_MARGINS 170

void MenuPanel::updateContent(const Menu &commands, AnimationType animationType,
    bool adjustsWidth, Component *newFooter)
{
    this->lastAnimationType = animationType;
    this->shouldResizeToFitContent = adjustsWidth;

    // If has new commands, fade out old list and create a new one
    const bool receivedNewCommands = commands != this->commandDescriptions;
    if (receivedNewCommands)
    {
        this->commandDescriptions = commands;
        if (animationType == Fading)
        {
            this->animator.fadeOut(this->listBox.get(), ANIM_TIME_MS);
        }
        else if (animationType == SlideLeft)
        {
            const auto fb(this->listBox->getBounds().translated(-this->listBox->getWidth(), 0));
            this->animator.animateComponent(this->listBox.get(), fb, FADE_ALPHA, ANIM_TIME_MS, true, 1.0, 0.0);
        }
        else if (animationType == SlideRight)
        {
            const auto fb(this->listBox->getBounds().translated(this->listBox->getWidth(), 0));
            this->animator.animateComponent(this->listBox.get(), fb, FADE_ALPHA, ANIM_TIME_MS, true, 1.0, 0.0);
        }
        else if (animationType == SlideUp)
        {
            const auto fb(this->listBox->getBounds().translated(0, -this->listBox->getHeight()));
            this->animator.animateComponent(this->listBox.get(), fb, FADE_ALPHA, 200, true, 1.0, 0.0);
        }
        else if (animationType == SlideDown)
        {
            const auto fb(this->listBox->getBounds().translated(0, this->listBox->getHeight()));
            this->animator.animateComponent(this->listBox.get(), fb, FADE_ALPHA, 200, true, 1.0, 0.0);
        }

        this->removeChildComponent(this->listBox.get());

        this->listBox.reset(new ListBox());
        this->listBox->setModel(this);
        this->listBox->setMultipleSelectionEnabled(false);
        this->listBox->setRowHeight(COMMAND_PANEL_BUTTON_HEIGHT);
        this->listBox->updateContent();
        this->addAndMakeVisible(this->listBox.get());
    }

    if (this->customFooter.get() != newFooter)
    {
        this->customFooter.reset(newFooter);
        this->addAndMakeVisible(this->customFooter.get());
    }

    // If no new command received, just update animation:
    if (animationType == Fading)
    {
        this->listBox->setBounds(this->getMenuBounds());
        this->animator.fadeIn(this->listBox.get(), ANIM_TIME_MS);
        if (this->customFooter.get() != nullptr)
        {
            this->customFooter->setBounds(this->getFooterBounds());
            this->animator.fadeIn(this->customFooter.get(), ANIM_TIME_MS);
        }
    }
    else if (animationType == SlideLeft)
    {
        this->listBox->setAlpha(FADE_ALPHA);
        this->listBox->setBounds(this->getMenuBounds().translated(this->getWidth(), 0));
        this->animator.animateComponent(this->listBox.get(), this->getMenuBounds(), 1.f, ANIM_TIME_MS, false, 1.0, 0.0);
        if (this->customFooter.get() != nullptr)
        {
            this->customFooter->setAlpha(FADE_ALPHA);
            this->customFooter->setBounds(this->getFooterBounds().translated(this->getWidth(), 0));
            this->animator.animateComponent(this->customFooter.get(), this->getFooterBounds(), 1.f, ANIM_TIME_MS, false, 1.0, 0.0);
        }
    }
    else if (animationType == SlideRight)
    {
        this->listBox->setAlpha(FADE_ALPHA);
        this->listBox->setBounds(this->getMenuBounds().translated(-this->getWidth(), 0));
        this->animator.animateComponent(this->listBox.get(), this->getMenuBounds(), 1.f, ANIM_TIME_MS, false, 1.0, 0.0);
        if (this->customFooter.get() != nullptr)
        {
            this->customFooter->setAlpha(FADE_ALPHA);
            this->customFooter->setBounds(this->getFooterBounds().translated(-this->getWidth(), 0));
            this->animator.animateComponent(this->customFooter.get(), this->getFooterBounds(), 1.f, ANIM_TIME_MS, false, 1.0, 0.0);
        }
    }
    else if (animationType == SlideUp)
    {
        this->listBox->setAlpha(FADE_ALPHA);
        this->listBox->setBounds(this->getMenuBounds().translated(0, this->getHeight()));
        this->animator.animateComponent(this->listBox.get(), this->getMenuBounds(), 1.f, ANIM_TIME_MS, false, 1.0, 0.0);
        if (this->customFooter.get() != nullptr)
        {
            this->customFooter->setAlpha(FADE_ALPHA);
            this->customFooter->setBounds(this->getFooterBounds().translated(0, this->getHeight()));
            this->animator.animateComponent(this->customFooter.get(), this->getFooterBounds(), 1.f, ANIM_TIME_MS, false, 1.0, 0.0);
        }
    }
    else if (animationType == SlideDown)
    {
        this->listBox->setAlpha(FADE_ALPHA);
        this->listBox->setBounds(this->getMenuBounds().translated(0, -this->getHeight()));
        this->animator.animateComponent(this->listBox.get(), this->getMenuBounds(), 1.f, ANIM_TIME_MS, false, 1.0, 0.0);
        if (this->customFooter.get() != nullptr)
        {
            this->customFooter->setAlpha(FADE_ALPHA);
            this->customFooter->setBounds(this->getFooterBounds().translated(0, -this->getHeight()));
            this->animator.animateComponent(this->customFooter.get(), this->getFooterBounds(), 1.f, ANIM_TIME_MS, false, 1.0, 0.0);
        }
    }
    else
    {
        this->listBox->setBounds(this->getMenuBounds());
        if (this->customFooter.get() != nullptr)
        {
            this->customFooter->setBounds(this->getFooterBounds());
        }
    }

    if (this->shouldResizeToFitContent && receivedNewCommands)
    {
        ScopedPointer<MenuItemComponent> tempItem(new MenuItemComponent(nullptr, nullptr, MenuItem::empty()));
        Font stringFont(tempItem->getFont());

        const int menuHeight = jmax(this->getHeight(), commands.size() * COMMAND_PANEL_BUTTON_HEIGHT);
        const int maxMenuHeight = COMMAND_PANEL_BUTTON_HEIGHT * 12; // Hard-coded for now:

        int estimatedWidth = 0;
        for (const auto &command : commands)
        {
            const int stringWidth = stringFont.getStringWidth(command->commandText) +
                stringFont.getStringWidth(command->hotkeyText);

            if (estimatedWidth < stringWidth)
            {
                estimatedWidth = stringWidth;
            }
        }

        const int newWidth = jmax(estimatedWidth + int(COMMAND_PANEL_BUTTON_HEIGHT * 2.1f), this->getWidth());
        const int newHeight = jmin(menuHeight, maxMenuHeight) + this->getFooterHeight();
        this->setSize(newWidth, newHeight);
    }
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

int MenuPanel::getNumRows()
{
    return this->commandDescriptions.size();
}

Component *MenuPanel::refreshComponentForRow(int rowNumber,
    bool isRowSelected, Component *existingComponentToUpdate)
{
    if (rowNumber >= this->commandDescriptions.size())
    {
        return existingComponentToUpdate;
    }

    const auto itemDescription = this->commandDescriptions[rowNumber];

    if (existingComponentToUpdate != nullptr)
    {
        if (auto *row = dynamic_cast<MenuItemComponent *>(existingComponentToUpdate))
        {
            row->setSelected(isRowSelected);
            row->update(itemDescription);
        }
    }
    else
    {
        auto *row = new MenuItemComponent(this, this->listBox->getViewport(), itemDescription);
        row->setSelected(isRowSelected);
        return row;
    }

    return existingComponentToUpdate;
}

Rectangle<int> MenuPanel::getMenuBounds() const
{
    return this->getLocalBounds().withHeight(this->getHeight() - this->getFooterHeight());
}

Rectangle<int> MenuPanel::getFooterBounds() const
{
    const int fh = this->getFooterHeight();
    return { 0, this->getHeight() - fh, this->getWidth(), fh };
}

int MenuPanel::getFooterHeight() const
{
    return (this->customFooter != nullptr) ? this->customFooter->getHeight() : 0;
}
