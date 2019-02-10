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

#include "TreeNode.h"
#include "ProjectEventDispatcher.h"
#include "MidiTrack.h"
#include "TrackedItem.h"
#include "Delta.h"

class Pattern;
class MidiSequence;
class ProjectNode;
class InstrumentDescription;

class MidiTrackNode :
    public TreeNode,
    public MidiTrack,
    public ProjectEventDispatcher,
    public VCS::TrackedItem
{
public:

    explicit MidiTrackNode(const String &name, const Identifier &type);

    ~MidiTrackNode() override;

    String getXPath() const noexcept;
    void setXPath(const String &path, bool sendNotifications);

    void showPage() override;
    void safeRename(const String &newName, bool sendNotifications) override;

    void importMidi(const MidiMessageSequence &sequence, short timeFormat);

    //===------------------------------------------------------------------===//
    // VCS::TrackedItem
    //===------------------------------------------------------------------===//

    String getVCSName() const override;
    ValueTree serializeClipsDelta() const;
    void resetClipsDelta(const ValueTree &state);

    //===------------------------------------------------------------------===//
    // MidiTrack
    //===------------------------------------------------------------------===//

    const String &getTrackId() const noexcept override;
    int getTrackChannel() const noexcept override;

    String getTrackName() const noexcept override;
    void setTrackName(const String &val, bool sendNotifications) override;

    Colour getTrackColour() const noexcept override;
    void setTrackColour(const Colour &val, bool sendNotifications) override;

    String getTrackInstrumentId() const noexcept override;
    void setTrackInstrumentId(const String &val, bool sendNotifications) override;

    int getTrackControllerNumber() const noexcept override;
    void setTrackControllerNumber(int val, bool sendNotifications) override;

    bool isTrackMuted() const noexcept override;
    void setTrackMuted(bool shouldBeMuted, bool sendNotifications) override;

    MidiSequence *getSequence() const noexcept override;
    Pattern *getPattern() const noexcept override;

    //===------------------------------------------------------------------===//
    // ProjectEventDispatcher
    //===------------------------------------------------------------------===//

    void dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    void dispatchAddEvent(const MidiEvent &event) override;
    void dispatchRemoveEvent(const MidiEvent &event) override;
    void dispatchPostRemoveEvent(MidiSequence *const layer) override;

    void dispatchAddClip(const Clip &clip) override;
    void dispatchChangeClip(const Clip &oldClip, const Clip &newClip) override;
    void dispatchRemoveClip(const Clip &clip) override;
    void dispatchPostRemoveClip(Pattern *const pattern) override;

    void dispatchChangeTrackProperties(MidiTrack *const track) override;
    void dispatchChangeProjectBeatRange() override;

    ProjectNode *getProject() const noexcept override;

    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    void onItemAddedToTree(bool sendNotifications) override;
    void onItemDeletedFromTree(bool sendNotifications) override;

    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    ScopedPointer<Component> createMenu() override;

    //===------------------------------------------------------------------===//
    // Callbacks
    //===------------------------------------------------------------------===//

    Function<void(const String &text)> getRenameCallback();
    Function<void(const String &colour)> getChangeColourCallback();
    Function<void(const String &instrumentId)> getChangeInstrumentCallback();

protected:

    ProjectNode *lastFoundParent;

    ScopedPointer<MidiSequence> sequence;
    ScopedPointer<Pattern> pattern;
    
protected:

    void setTrackId(const String &val) override;
    String id;

    Colour colour;
    int channel;

    String instrumentId;
    int controllerNumber;

    bool mute;
    bool solo; // Not implemented

};
