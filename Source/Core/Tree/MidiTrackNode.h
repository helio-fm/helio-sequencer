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

#include "TreeNode.h"
#include "ProjectEventDispatcher.h"
#include "MidiTrack.h"
#include "TimeSignatureEvent.h"
#include "TrackedItem.h"
#include "Delta.h"

class Pattern;
class MidiSequence;
class ProjectNode;

class MidiTrackNode :
    public TreeNode,
    public MidiTrack,
    public ProjectEventDispatcher,
    public VCS::TrackedItem
{
public:

    MidiTrackNode(const String &name, const Identifier &type);

    String getTreePath() const noexcept;
    void setTreePath(const String &path, bool sendNotifications);

    void safeRename(const String &newName, bool sendNotifications) override;

    // wrappers around TreeNode::setSelected() to allow setting selected clip
    void setSelected(NotificationType shouldNotify = sendNotification) override;
    void setSelected(const Clip &editableScope,
        NotificationType shouldNotify = sendNotification);

    //===------------------------------------------------------------------===//
    // VCS::TrackedItem
    //===------------------------------------------------------------------===//

    String getVCSName() const override;
    SerializedData serializeClipsDelta() const;
    void resetClipsDelta(const SerializedData &state);
    Colour getRevisionDisplayColour() const override;

    //===------------------------------------------------------------------===//
    // MidiTrack
    //===------------------------------------------------------------------===//

    const String &getTrackId() const noexcept override;

    int getTrackChannel() const noexcept override;
    void setTrackChannel(int channel, bool undoable,
        NotificationType notificationType) override;

    String getTrackName() const noexcept override;
    void setTrackName(const String &val, bool undoable,
        NotificationType notificationType) override;

    Colour getTrackColour() const noexcept override;
    void setTrackColour(const Colour &val, bool undoable,
        NotificationType notificationType) override;

    String getTrackInstrumentId() const noexcept override;
    void setTrackInstrumentId(const String &val, bool undoable,
        NotificationType notificationType) override;

    int getTrackControllerNumber() const noexcept override;
    void setTrackControllerNumber(int val, NotificationType notificationType) override;

    bool hasTimeSignatureOverride() const noexcept override;
    const TimeSignatureEvent *getTimeSignatureOverride() const noexcept override;
    void setTimeSignatureOverride(const TimeSignatureEvent &ts, bool undoable,
        NotificationType notificationType) override;

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

    void dispatchChangeTrackProperties() override;
    void dispatchChangeTrackBeatRange() override;
    void dispatchChangeProjectBeatRange() override;

    void dispatchChangeActiveMidiInputInstrument();

    ProjectNode *getProject() const noexcept override;

    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    void onNodeAddToTree(bool sendNotifications) override;
    void onNodeRemoveFromTree(bool sendNotifications) override;

    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    UniquePointer<Component> createMenu() override;

protected:

    // VCS helpers used by subclasses:
    SerializedData serializePathDelta() const;
    SerializedData serializeColourDelta() const;
    SerializedData serializeChannelDelta() const;
    SerializedData serializeInstrumentDelta() const;
    SerializedData serializeTimeSignatureDelta() const;

    void resetPathDelta(const SerializedData &state);
    void resetColourDelta(const SerializedData &state);
    void resetChannelDelta(const SerializedData &state);
    void resetInstrumentDelta(const SerializedData &state);
    void resetTimeSignatureDelta(const SerializedData &state);

protected:

    ProjectNode *lastFoundParent;

    UniquePointer<MidiSequence> sequence;
    UniquePointer<Pattern> pattern;
    
    // this is set when the track is selected, but it's
    // not updated if the corresponding clip's params change
    // (would require a subscription to project events)
    Clip selectedClipId;
    // it's easier to keep it as an identifier and look up
    // the up-to-date clip reference in the pattern when needed
    // (PianoRoll and others do similar thing in onAddMidiEvent):
    const Clip &getSelectedClip() const;

protected:

    void setTrackId(const String &val) override;
    String id;

    Colour colour = Colours::white;
    int channel = 1;

    String instrumentId;
    int controllerNumber = 0;

    // used as a template by TimeSignaturesAggregator:
    TimeSignatureEvent timeSignatureOverride;

};
