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

#include "CenteredTooltipComponent.h"
#include "ProgressIndicator.h"

using SimpleCloseCallback = Function<void()>;

class ProgressTooltip final : public CenteredTooltipComponent
{
public:

    ProgressTooltip(bool cancellable);

    static UniquePointer<ProgressTooltip> cancellable(SimpleCloseCallback callback)
    {
        auto tooltip = make<ProgressTooltip>(true);
        tooltip->onCancel = callback;
        return tooltip;
    }

    void paint(Graphics &g) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void handleCommandMessage (int commandId) override;
    bool keyPressed (const KeyPress& key) override;
    void inputAttemptWhenModal() override;

private:

    static constexpr auto tooltipSize = 96;
    static constexpr auto imageSize = 64;

    SimpleCloseCallback onCancel;
    const bool isCancellable;
    void cancel();

    UniquePointer<ProgressIndicator> progressIndicator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgressTooltip)
};
