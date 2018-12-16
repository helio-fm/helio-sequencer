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

#pragma once

//[Headers]
#include "CenteredTooltipComponent.h"
#include "ProgressIndicator.h"

using SimpleCloseCallback = Function<void()>;
//[/Headers]


class ProgressTooltip final : public CenteredTooltipComponent
{
public:

    ProgressTooltip(bool cancellable);
    ~ProgressTooltip();

    //[UserMethods]
    SimpleCloseCallback onCancel;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void handleCommandMessage (int commandId) override;
    bool keyPressed (const KeyPress& key) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]
    bool isCancellable;
    void cancel();
    //[/UserVariables]

    UniquePointer<ProgressIndicator> progressIndicator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgressTooltip)
};
