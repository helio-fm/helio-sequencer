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

#include "UndoAction.h"

//===----------------------------------------------------------------------===//
// Change Colour
//===----------------------------------------------------------------------===//

class MidiLayerChangeColourAction : public UndoAction
{
public:
    
    explicit MidiLayerChangeColourAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    MidiLayerChangeColourAction(ProjectTreeItem &project,
                                String layerId,
                                const Colour &newColour);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:
    
    String layerId;
    
    Colour colourBefore;
    Colour colourAfter;
    
    JUCE_DECLARE_NON_COPYABLE(MidiLayerChangeColourAction)
};


//===----------------------------------------------------------------------===//
// Change Instrument
//===----------------------------------------------------------------------===//

class MidiLayerChangeInstrumentAction : public UndoAction
{
public:
    
    explicit MidiLayerChangeInstrumentAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    MidiLayerChangeInstrumentAction(ProjectTreeItem &project,
                                    String layerId,
                                    String newInstrumentId);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:
    
    String layerId;
    
    String instrumentIdBefore;
    String instrumentIdAfter;
    
    JUCE_DECLARE_NON_COPYABLE(MidiLayerChangeInstrumentAction)
};


//===----------------------------------------------------------------------===//
// Mute/Unmute
//===----------------------------------------------------------------------===//

class MidiLayerMuteAction : public UndoAction
{
public:
    
    explicit MidiLayerMuteAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    MidiLayerMuteAction(ProjectTreeItem &project,
                        String layerId,
                        bool shouldBeMuted);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:
    
    String layerId;
    
    bool muteStateBefore;
    bool muteStateAfter;
    
    JUCE_DECLARE_NON_COPYABLE(MidiLayerMuteAction)
};
