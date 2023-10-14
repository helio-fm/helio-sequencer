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

#include "KeySignatureComponent.h"
#include "KeySignatureEvent.h"
#include "ProjectListener.h"
#include "ProjectMapsScroller.h"

class RollBase;
class ProjectNode;

class KeySignaturesProjectMap final :
    public ProjectMapsScroller::ScrolledComponent,
    public ProjectListener
{
public:
    
    enum class Type : int8 { Large, Small };

    KeySignaturesProjectMap(ProjectNode &parentProject,
        SafePointer<RollBase> roll, Type type);

    ~KeySignaturesProjectMap() override;

    void alignKeySignatureComponent(KeySignatureComponent *nc);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeMidiEvent(const MidiEvent &oldEvent,
        const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;

    // Assuming the timeline has no patterns/clips:
    void onAddClip(const Clip &clip) override {}
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override {}
    void onRemoveClip(const Clip &clip) override {}

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;

    void onChangeProjectInfo(const ProjectMetadata *info) override;
    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override;
    void onBeforeReloadProjectContent() override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

    //===------------------------------------------------------------------===//
    // Stuff for children
    //===------------------------------------------------------------------===//

    void onKeySignatureSelected(KeySignatureComponent *nc);
    void onKeySignatureMainAction(KeySignatureComponent *nc);
    void onKeySignatureAltAction(KeySignatureComponent *nc);
    float getBeatByXPosition(int x) const;
    
private:

    void reloadTrackMap();
    void applyKeySignatureBounds(KeySignatureComponent *nc, KeySignatureComponent *nextOne = nullptr);
    void keySignatureTapAction(KeySignatureComponent *nc, bool altMode);

    KeySignatureComponent *getPreviousEventComponent(int indexOfSorted) const;
    KeySignatureComponent *getNextEventComponent(int indexOfSorted) const;

    const StringArray &getProjectKeyNames() const noexcept;
    
private:

    float projectFirstBeat = 0.f;
    float projectLastBeat = Globals::Defaults::projectLength;

    float rollFirstBeat = 0.f;
    float rollLastBeat = Globals::Defaults::projectLength;
    
    int keyboardSize = Globals::twelveToneKeyboardSize;

    ProjectNode &project;

    ComponentAnimator animator;

    const Type type;
    KeySignatureComponent *createComponent(const KeySignatureEvent &keySignature);

    OwnedArray<KeySignatureComponent> keySignatureComponents;
    FlatHashMap<KeySignatureEvent, KeySignatureComponent *, MidiEventHash> keySignaturesMap;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeySignaturesProjectMap)
};
