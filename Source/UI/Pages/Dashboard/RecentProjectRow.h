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

#pragma once

#include "Common.h"
#include "RecentProjectInfo.h"
#include "DraggingListBoxComponent.h"
#include "RecentProjectRow.h"
#include "DashboardMenu.h"
#include "IconComponent.h"
#include "Icons.h"

class RecentProjectRow final : public DraggingListBoxComponent
{
public:

    RecentProjectRow(DashboardMenu &parent, ListBox &parentListBox) :
        DraggingListBoxComponent(parentListBox.getViewport()),
        parentList(parent)
    {
        this->titleLabel = make<Label>();
        this->addAndMakeVisible(this->titleLabel.get());
        this->titleLabel->setFont(Globals::UI::Fonts::M);
        this->titleLabel->setJustificationType(Justification::centredRight);
        this->titleLabel->setEditable(false, false, false);

        this->dateLabel = make<Label>();
        this->addAndMakeVisible(this->dateLabel.get());
        this->dateLabel->setFont(Font(12.f, Font::plain));
        this->dateLabel->setJustificationType(Justification::centredRight);
        this->dateLabel->setEditable(false, false, false);

        this->activenessImage = make<IconComponent>(Icons::project);
        this->addAndMakeVisible(this->activenessImage.get());

        this->setSize(350, 56);
    }

    ~RecentProjectRow() = default;

    void setSelected(bool shouldBeSelected) override
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

    void updateDescription(const RecentProjectInfo::Ptr file, bool isLoaded, bool isLastRow)
    {
        this->targetFile = file;
        this->isFileLoaded = isLoaded;

        this->titleLabel->setText(this->targetFile->getTitle(), dontSendNotification);
        this->dateLabel->setText(App::getHumanReadableDate(this->targetFile->getUpdatedAt()), dontSendNotification);

        const float totalAlpha = this->isFileLoaded ? 1.f : 0.5f;
        const float loadIndicatorAlpha = totalAlpha * (this->isFileLoaded ? 1.f : 0.5f);
        this->activenessImage->setIconAlphaMultiplier(loadIndicatorAlpha);

        this->titleLabel->setColour(Label::textColourId,
            findDefaultColour(Label::textColourId).withMultipliedAlpha(totalAlpha));

        this->dateLabel->setColour(Label::textColourId,
            findDefaultColour(Label::textColourId).withMultipliedAlpha(totalAlpha));
    }

    void resized() override
    {
        this->titleLabel->setBounds(0, 5, this->getWidth() - 46, 24);
        this->dateLabel->setBounds(0, 27, this->getWidth() - 46, 16);
        this->activenessImage->setBounds(this->getWidth() - 44, (this->getHeight() / 2) - 4 - 18, 36, 36);
    }

private:

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
            g.fillRoundedRectangle (5.f, 1.0f,
                float(this->getWidth() - 10), float(this->getHeight() - 9), 2.f);
        }
    };

    Component *createHighlighterComponent() override
    {
        return new RecentFileSelectionComponent();
    }

    DashboardMenu &parentList;
    RecentProjectInfo::Ptr targetFile;

    bool isFileLoaded = false;
    bool isSelected = false;

    UniquePointer<Label> titleLabel;
    UniquePointer<Label> dateLabel;
    UniquePointer<IconComponent> activenessImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecentProjectRow)
};
