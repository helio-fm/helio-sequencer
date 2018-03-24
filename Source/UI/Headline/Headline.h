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
#include "TreeItem.h"
#include "TreeNavigationHistory.h"

class HeadlineItem;
//[/Headers]

#include "../Themes/PanelBackgroundB.h"
#include "../Themes/SeparatorHorizontalReversed.h"
#include "HeadlineNavigationPanel.h"

class Headline final : public Component,
                       public AsyncUpdater
{
public:

    Headline();
    ~Headline();

    //[UserMethods]

    void syncWithTree(TreeNavigationHistory &history, WeakReference<TreeItem> root);

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]

    // A way to receive a single coalesced update from multiple signaling sub-items:
    void handleAsyncUpdate() override;

    ComponentAnimator animator;
    OwnedArray<HeadlineItem> chain;

    //[/UserVariables]

    ScopedPointer<PanelBackgroundB> bg;
    ScopedPointer<SeparatorHorizontalReversed> separator;
    ScopedPointer<HeadlineNavigationPanel> navPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Headline)
};
