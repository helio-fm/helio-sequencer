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

#include "Serializable.h"
#include "MidiLayerOwner.h"
#include "Clip.h"

class ProjectTreeItem;
class UndoStack;

// A sorted array of clips
class Pattern : public Serializable
{
public:

    explicit Pattern(MidiLayerOwner &parent);
    ~Pattern() override;

    MidiLayerOwner *getOwner() const;
    
    //===------------------------------------------------------------------===//
    // Undoing
    //===------------------------------------------------------------------===//

    void checkpoint();
    void undo();
    void redo();
    void clearUndoHistory();
	
	//===------------------------------------------------------------------===//
	// Clips
	//===------------------------------------------------------------------===//

	inline Array<Clip> &getClips() noexcept;
	bool insert(Clip clip, const bool undoable);
	bool remove(Clip clip, const bool undoable);
	bool change(Clip clip, Clip newClip, const bool undoable);

    //===------------------------------------------------------------------===//
    // Track editing
    //===------------------------------------------------------------------===//

    // This one is for import and checkout procedures.
	// Does not notify anybody to prevent notification hell.
    // Always call notifyLayerChanged() when you're done using it.
    virtual void silentImport(const MidiEvent &eventToImport) = 0;
	
    //===------------------------------------------------------------------===//
    // Array wrapper
    //===------------------------------------------------------------------===//

    void sort();

    inline int size() const
    { return this->clips.size(); }

    inline Clip* begin() const noexcept
    { return this->clips.begin(); }
    
    inline Clip* end() const noexcept
    { return this->clips.end(); }
    
    inline Clip getUnchecked(const int index) const
    { return this->clips.getUnchecked(index); }

    inline int indexOfSorted(const Clip &event) const
    {
        jassert(this->midiEvents[this->midiEvents.indexOfSorted(*event, event)] == event);
        return this->clips.indexOfSorted(event, event);
    }

    //===------------------------------------------------------------------===//
    // Events change listener
    //===------------------------------------------------------------------===//

    void notifyClipChanged(const Clip &oldClip, const Clip &newClip);
    void notifyClipAdded(const Clip &clip);
    void notifyClipRemoved(const Clip &clip);
	void notifyPatternChanged();

	//===------------------------------------------------------------------===//
	// Serializable
	//===------------------------------------------------------------------===//

	XmlElement *serialize() const override;
	void deserialize(const XmlElement &xml) override;
	void reset() override;

protected:

	inline Uuid getPatternId() const noexcept;
	inline String getPatternIdAsString() const;

	void clearQuick();

	ProjectTreeItem *getProject();
	UndoStack *getUndoStack();

	Uuid id;
	Array<Clip> clips;

private:
	
    MidiLayerOwner &owner;

private:
    
    WeakReference<Pattern>::Master masterReference;
    friend class WeakReference<Pattern>;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pattern);
};
