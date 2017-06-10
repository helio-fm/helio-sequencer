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

class TreeItem;
class AudioMonitor;
class MidiLayerTreeItem;
class LongTapController;

#include "LongTapListener.h"

#define TREE_COMPACT_WIDTH           (48)
#define TREE_DEFAULT_WIDTH           (200)
#define TREE_PHONE_WIDTH             (48)

class TreePanel : public Component,
                  public LongTapListener
{
public:
    
    TreePanel();
    ~TreePanel() override;
    
    virtual void setRoot(TreeItem *rootItem) = 0;
    virtual void setRootItemPanelSelected(bool shouldBeSelected) = 0;
    virtual void setAudioMonitor(AudioMonitor *audioMonitor) = 0;
    virtual Rectangle<int> getWorkingArea() = 0;
    
    void showRenameLayerDialogAsync(MidiLayerTreeItem *targetItem);
    void emitRollover(Component *newTargetComponent, const String &headerTitle);
    bool isCompactMode() const;
    
    void handleCommandMessage(int commandId) override;
    
    void longTapEvent(const MouseEvent &e) override;

protected:
    
    TreeItem *root;
    
    ScopedPointer<LongTapController> longTapRecognizer;
    
    MidiLayerTreeItem *lastRenamedItem;
    String renameString;
    
    void dismissCurrentRollover();
    ComponentAnimator rolloverFader;
    ScopedPointer<Component> currentRollover;

};
