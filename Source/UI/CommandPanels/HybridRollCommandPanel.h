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

class ProjectTreeItem;
class VolumeComponent;
class VolumeCallback;

#include "TransportListener.h"
#include "CommandItemComponent.h"

#if HELIO_DESKTOP
#   define HYBRID_ROLL_COMMANDPANEL_WIDTH (80)
#   define HYBRID_ROLL_COMMANDPANEL_ROWHEIGHT (34)
#   define HYBRID_ROLL_COMMANDPANEL_SHOULD_SHOW_ANNOTATION_DETAILS (false)
#elif HELIO_MOBILE
#   define HYBRID_ROLL_COMMANDPANEL_WIDTH (77)
#   define HYBRID_ROLL_COMMANDPANEL_ROWHEIGHT (48)
#   define HYBRID_ROLL_COMMANDPANEL_SHOULD_SHOW_ANNOTATION_DETAILS (false)
#endif

class HybridRollCommandPanel : public Component,
                             protected TransportListener,
                             protected AsyncUpdater,
                             protected ListBoxModel,
                             protected ChangeListener,
                             protected Timer
{
public:

    HybridRollCommandPanel(ProjectTreeItem &parent);
    ~HybridRollCommandPanel();
    
    void handleCommandMessage (int commandId) override;
    void childrenChanged() override;
    void mouseMove (const MouseEvent& e) override;

protected:

    ProjectTreeItem &project;

    double lastSeekTime;
    double lastTotalTime;
    double timerStartSeekTime;
    double timerStartSystemTime;

    ReferenceCountedArray<CommandItem> commandDescriptions;
    void recreateCommandDescriptions();

    virtual void updateModeButtons() = 0;
    virtual void emitAnnotationsCallout(Component *newAnnotationsMenu) = 0;

    //===----------------------------------------------------------------------===//
    // ListBoxModel
    //

    int getNumRows() override;

    void paintListBoxItem(int rowNumber,
                                  Graphics &g,
                                  int width, int height,
                                  bool rowIsSelected) override;

    //===----------------------------------------------------------------------===//
    // ChangeListener
    //

    void changeListenerCallback(ChangeBroadcaster *source) override;

    //===----------------------------------------------------------------------===//
    // Timer
    //

    void timerCallback() override;

    //===----------------------------------------------------------------------===//
    // TransportListener
    //

    void onSeek(const double newPosition,
        const double currentTimeMs, const double totalTimeMs) override;
    void onTempoChanged(const double newTempo) override;
    void onTotalTimeChanged(const double timeMs) override;
    void onPlay() override;
    void onStop() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HybridRollCommandPanel)
};
