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

#include "EditorPanelBase.h"
#include "HeadlineItemArrow.h"
#include "HighlightedComponent.h"
#include "CachedLabelImage.h"
#include "HelioTheme.h"

class EditorPanelsSwitcher final : public Component
{
public:

    EditorPanelsSwitcher()
    {
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, true);
    }

    Function<void(int panelId, const EditorPanelBase::EventFilter &filter)> onClick;
    Function<void(const MouseEvent &e, const MouseWheelDetails &wheel)> onWheelMove;

    struct Filters final
    {
        int editorPanelIndex;
        Array<EditorPanelBase::EventFilter> eventFilters;

        bool operator== (const Filters &other) const noexcept
        {
            return this->editorPanelIndex == other.editorPanelIndex &&
                this->eventFilters == other.eventFilters;
        }
    };

    class ModeComponent final : public HighlightedComponent
    {
    public:

        ModeComponent(int panelId,
            const EditorPanelBase::EventFilter &filter) :
            panelId(panelId),
            filter(filter)
        {
            this->setWantsKeyboardFocus(false);

            this->titleLabel = make<Label>(String(), filter.name);
            this->addAndMakeVisible(this->titleLabel.get());
            this->titleLabel->setFont(Globals::UI::Fonts::M);
            this->titleLabel->setJustificationType(Justification::centredLeft);
            this->titleLabel->setInterceptsMouseClicks(false, false);
            this->titleLabel->setColour(Label::textColourId,
                findDefaultColour(Label::textColourId).withMultipliedAlpha(0.4f));

            this->titleLabel->setCachedComponentImage(new CachedLabelImage(*this->titleLabel));

            constexpr auto arrowWidth = 8;
            this->arrow = make<HeadlineItemArrow>(arrowWidth, false);
            this->addAndMakeVisible(this->arrow.get());

            constexpr auto horizontalMargin = arrowWidth + 2;
            const auto textWidth = this->titleLabel->getFont().getStringWidth(filter.name);
            this->setSize(textWidth + this->getOverlapOffset() + horizontalMargin * 2,
                EditorPanelsSwitcher::switcherHeight);
        }

        Function<void(int panelId, const EditorPanelBase::EventFilter &filter)> onClick;
        Function<void(const MouseEvent &e, const MouseWheelDetails &wheel)> onWheelMove;

        int getEditorPanelId() const noexcept
        {
            return this->panelId;
        }

        const EditorPanelBase::EventFilter &getEventFilter() const noexcept
        {
            return this->filter;
        }

        int getOverlapOffset() const noexcept
        {
            return this->arrow->getWidth();
        }

        void setSelected(bool shouldBeSelected)
        {
            if (this->isSelected == shouldBeSelected)
            {
                return;
            }

            this->isSelected = shouldBeSelected;

            const auto labelColour = findDefaultColour(Label::textColourId);
            this->titleLabel->setColour(Label::textColourId,
                labelColour.withMultipliedAlpha(this->isSelected ? 1.f : 0.4f));

            auto *cachedImage = static_cast<CachedLabelImage *>(this->titleLabel->getCachedComponentImage());
            jassert(cachedImage != nullptr);
            cachedImage->forceInvalidate();

            this->repaint();
        }

        void paint(Graphics &g) override
        {
            const auto &theme = HelioTheme::getCurrentTheme();
            g.setFillType({ theme.getBottomPanelBackground(), {} });
            g.fillPath(this->backgroundShape);

            g.setColour(this->borderColourDark);
            g.fillRect(0, 0, this->getWidth() - this->arrow->getWidth(), 1);

            g.setColour(this->borderColourLight);
            g.fillRect(1, 1, this->getWidth() - this->arrow->getWidth() - 1, 1);
        }

        bool hitTest(int x, int y) override
        {
            return this->backgroundShape.contains({float(x), float(y)});
        }

        void mouseDown(const MouseEvent &) override
        {
            if (this->onClick)
            {
                this->onClick(this->panelId, this->filter);
            }
        }
        
        void mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &wheel) override
        {
            if (this->onWheelMove)
            {
                this->onWheelMove(e, wheel);
            }
        }

        void resized() override
        {
            const auto arrowWidth = this->arrow->getWidth();

            this->titleLabel->setBounds(this->getOverlapOffset(), 0, this->getWidth() - arrowWidth, this->getHeight());

            this->arrow->setBounds(this->getWidth() - arrowWidth, 1, arrowWidth, this->getHeight() - 1);

            this->backgroundShape.clear();
            this->backgroundShape.startNewSubPath(0.f, 0.f);
            this->backgroundShape.lineTo(float(this->getWidth() - arrowWidth + 1), 0.f);
            this->backgroundShape.lineTo(float(this->getWidth()), float(this->getHeight()));
            this->backgroundShape.lineTo(0.f, float(this->getHeight()));
            this->backgroundShape.closeSubPath();

            HighlightedComponent::resized();
        }

    private:

        class Highlighter final : public Component
        {
        public:

            explicit Highlighter(const Path &backgroundShape) :
                backgroundShape(backgroundShape)
            {
                this->setWantsKeyboardFocus(false);
                this->setInterceptsMouseClicks(false, false);
            }

            void paint(Graphics &g) override
            {
                g.setColour(this->colour);
                g.fillPath(this->backgroundShape);
            }

            const Path backgroundShape;
            const Colour colour = Colours::white.withAlpha(0.05f);
        };

        Component *createHighlighterComponent() override
        {
            return this->isSelected ? nullptr :
                new Highlighter(this->backgroundShape);
        }

        bool isSelected = false;

        const int panelId;
        const EditorPanelBase::EventFilter filter;

        UniquePointer<Label> titleLabel;
        UniquePointer<Component> arrow;

        const Colour borderColourLight =
            findDefaultColour(ColourIDs::TrackScroller::borderLineLight)
                .withMultipliedAlpha(0.75f);

        const Colour borderColourDark =
            findDefaultColour(ColourIDs::TrackScroller::borderLineDark)
                .withMultipliedAlpha(0.5f);

        Path backgroundShape;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModeComponent)
    };

    void reload(const Array<Filters> &editModes)
    {
        if (this->currentFilters == editModes)
        {
            return;
        }

        this->currentFilters = editModes;

        this->modeComponents.clearQuick(true);

        if (this->currentFilters.size() == 1)
        {
            // a single item is not really a choice, display nothing
            return;
        }

        for (const auto &editMode : editModes)
        {
            for (const auto &filter : editMode.eventFilters)
            {
                auto modeComponent = make<ModeComponent>(editMode.editorPanelIndex, filter);
                this->addAndMakeVisible(modeComponent.get());
                modeComponent->toBack();
                modeComponent->onClick = this->onClick;
                modeComponent->onWheelMove = this->onWheelMove;
                this->modeComponents.add(modeComponent.release());
            }
        }

        this->resized();
    }

    void updateSelection(int selectedPanelId,
        EditorPanelBase::EventFilter selectedEventFilter)
    {
        for (const auto &component : this->modeComponents)
        {
            component->setSelected(component->getEditorPanelId() == selectedPanelId &&
                component->getEventFilter().id == selectedEventFilter.id);
        }
    }

    void resized() override
    {
        int x = 0;
        for (auto *component : this->modeComponents)
        {
            component->setTopLeftPosition(x, 0);
            x += component->getWidth() - component->getOverlapOffset();
        }
    }

    static constexpr auto switcherHeight = 27;

private:

    OwnedArray<ModeComponent> modeComponents;

    Array<Filters> currentFilters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditorPanelsSwitcher)
};
