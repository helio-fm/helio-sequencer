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

#include "Common.h"
#include "VelocityProjectMap.h"
#include "PlayerThread.h"
#include "ProjectNode.h"
#include "MidiTrack.h"
#include "Pattern.h"
#include "PianoSequence.h"
#include "AnnotationEvent.h"
#include "RollBase.h"
#include "Lasso.h"
#include "ColourIDs.h"
#include "NoteComponent.h"
#include "FineTuningValueIndicator.h"

#define VELOCITY_MAP_BULK_REPAINT_START \
    if (this->isEnabled()) { this->setVisible(false); }

#define VELOCITY_MAP_BULK_REPAINT_END \
    if (this->isEnabled()) { this->setVisible(true); }

//===----------------------------------------------------------------------===//
// Child level component
//===----------------------------------------------------------------------===//

class VelocityMapNoteComponent final : public Component
{
public:

    VelocityMapNoteComponent(const Note &note, const Clip &clip) :
        note(note),
        clip(clip)
    {
        this->updateColour();
        this->setInterceptsMouseClicks(true, false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
        this->setMouseCursor(MouseCursor::UpDownResizeCursor);
    }

    inline float getBeat() const noexcept
    {
        return this->note.getBeat() + this->clip.getBeat();
    }

    inline float getLength() const noexcept
    {
        return this->note.getLength();
    }

    inline float getVelocity() const noexcept
    {
        return this->note.getVelocity() * this->clip.getVelocity();
    }

    Line<float> getStartLine() const noexcept
    {
        const float x = this->getX() + this->dx;
        return { x, 0.f, x, float(Globals::UI::levelsMapHeight) };
    }

    Line<float> getEndLine() const noexcept
    {
        const float x = this->getX() + this->dx + this->getWidth() + this->dw;
        return { x, 0.f, x, float(Globals::UI::levelsMapHeight) };
    }

    inline void updateColour()
    {
        const Colour baseColour(findDefaultColour(ColourIDs::Roll::noteFill));
        this->mainColour = this->note.getTrackColour()
            .interpolatedWith(baseColour, this->editable ? 0.35f : 0.5f)
            .withAlpha(this->editable ? 0.75f : 0.065f);
        this->paleColour = this->mainColour
            .brighter(0.1f)
            .withMultipliedAlpha(0.1f);
    }

    void setRealBounds(float x, int y, float w, int h) noexcept
    {
        this->dx = x - floorf(x);
        this->dw = w - ceilf(w);
        this->setBounds(int(floorf(x)), y, int(ceilf(w)), h);
    }

    bool isEditable() const noexcept
    {
        return this->editable;
    }

    void setEditable(bool editable)
    {
        if (this->editable == editable)
        {
            return;
        }

        this->editable = editable;

        this->setEnabled(editable);
        this->updateColour();

        if (this->editable)
        {
            // toBack() and toFront() use indexOf(this) so calling them sucks
            this->toFront(false);
        }
    }

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) noexcept override
    {
        g.setColour(this->mainColour);
        g.fillRect(this->dx, 1.f, 1.f, float(this->getHeight() - 1));
        g.fillRect(this->dx + 1.f, 0.f, float(this->getWidth() - 2) + this->dw, 1.f);
        g.fillRect(this->dx, 1.f, float(this->getWidth()) + this->dw, 2.f);

        g.setColour(this->paleColour);
        g.fillRect(this->dx, 0.f, float(this->getWidth()) + this->dw, float(this->getHeight()));
    }

    bool hitTest(int, int y) noexcept override
    {
        constexpr auto dragAreaSize = 5;
        return this->editable && y <= dragAreaSize;
    }

    void mouseDown(const MouseEvent &e) override
    {
        if (e.mods.isLeftButtonDown())
        {
            this->note.getSequence()->checkpoint();
            this->velocityAnchor = this->getVelocity();
        }
    }

    void mouseDrag(const MouseEvent &e) override
    {
        const auto newVelocity = jlimit(0.f, 1.f, this->velocityAnchor -
            float(e.getDistanceFromDragStartY()) / float(Globals::UI::levelsMapHeight));

        static_cast<PianoSequence *>(this->note.getSequence())->
            change(this->note, this->note.withVelocity(newVelocity), true);
    }

private:

    friend class VelocityProjectMap;

    VelocityProjectMap *getParentMap() const
    {
        jassert(this->getParentComponent());
        return static_cast<VelocityProjectMap *>(this->getParentComponent());
    }

    const Note &note;
    const Clip &clip;

    Colour mainColour;
    Colour paleColour;

    float dx = 0.f;
    float dw = 0.f;

    float velocityAnchor = 0.f;
    bool editable = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityMapNoteComponent)
};

//===----------------------------------------------------------------------===//
// Dragging helper
//===----------------------------------------------------------------------===//

class VelocityLevelDraggingHelper final : public Component
{
public:

    VelocityLevelDraggingHelper(VelocityProjectMap &map) : map(map)
    {
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
    }

    const Line<float> &getLine() const noexcept
    {
        return this->line;
    }

    const Line<float> &getExtendedLine() const noexcept
    {
        return this->lineExtended;
    }

    void paint(Graphics &g) override
    {
        g.setColour(findDefaultColour(Label::textColourId));
        g.fillPath(this->linePath);

        // display dashed lines just a bit longer on both ends,
        // so that it's more clear how it cuts the velocity levels:
        g.fillPath(this->dashPath1);
        g.fillPath(this->dashPath2);
    }

    void setStartPosition(const Point<float> &mousePos)
    {
        this->startPosition = mousePos.toDouble() / this->getParentSize();
    }

    void setEndPosition(const Point<float> &mousePos)
    {
        this->endPosition = mousePos.toDouble() / this->getParentSize();
    }

    void updateBounds()
    {
        static const int dashMargin = 37;
        static const int margin = dashMargin + 2;
        static const float lineThickness = 1.f;
        static Array<float> dashes(4.f, 3.f);

        const Point<double> parentSize(this->getParentSize());
        const auto start = (this->startPosition * parentSize).toFloat();
        const auto end = (this->endPosition * parentSize).toFloat();
        const auto x1 = jmin(start.getX(), end.getX());
        const auto x2 = jmax(start.getX(), end.getX());
        const auto y1 = jmin(start.getY(), end.getY());
        const auto y2 = jmax(start.getY(), end.getY());
        this->line = { start, end };

        if (this->line.getLength() == 0)
        {
            return;
        }

        static constexpr auto lineExtent = 1000;
        this->lineExtended = { this->line.getPointAlongLine(-lineExtent),
            this->line.getPointAlongLine(this->line.getLength() + lineExtent) };

        const Point<float> startOffset(x1 - margin, y1 - margin);

        this->linePath.clear();
        this->linePath.startNewSubPath(this->line.getPointAlongLine(0, lineThickness) - startOffset);
        this->linePath.lineTo(this->line.getPointAlongLine(0, -lineThickness) - startOffset);
        this->linePath.lineTo(this->line.reversed().getPointAlongLine(0, lineThickness) - startOffset);
        this->linePath.lineTo(this->line.reversed().getPointAlongLine(0, -lineThickness) - startOffset);
        this->linePath.closeSubPath();

        this->dashPath1.clear();
        this->dashPath1.startNewSubPath(this->line.getStart() - startOffset);
        this->dashPath1.lineTo(this->line.getPointAlongLine(-dashMargin) - startOffset);
        PathStrokeType(1.f).createDashedStroke(this->dashPath1, this->dashPath1,
            dashes.getRawDataPointer(), dashes.size());

        this->dashPath2.clear();
        this->dashPath2.startNewSubPath(this->line.getEnd() - startOffset);
        this->dashPath2.lineTo(this->line.getPointAlongLine(this->line.getLength() + dashMargin) - startOffset);
        PathStrokeType(1.f).createDashedStroke(this->dashPath2, this->dashPath2,
            dashes.getRawDataPointer(), dashes.size());

        this->setBounds(int(x1) - margin, int(y1) - margin,
            int(x2 - x1) + margin * 2, int(y2 - y1) + margin * 2);
    }

private:

    VelocityProjectMap &map;

    Point<double> startPosition;
    Point<double> endPosition;

    Line<float> line;
    Line<float> lineExtended;

    Path linePath;
    Path dashPath1;
    Path dashPath2;

    const Point<double> getParentSize() const
    {
        if (const auto *p = this->getParentComponent())
        {
            return { double(p->getWidth()), double(p->getHeight()) };
        }

        return { 1.0, 1.0 };
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityLevelDraggingHelper)
};

//===----------------------------------------------------------------------===//
// The map itself
//===----------------------------------------------------------------------===//

VelocityProjectMap::VelocityProjectMap(ProjectNode &parentProject, RollBase &parentRoll) :
    project(parentProject),
    roll(parentRoll)
{
    this->setInterceptsMouseClicks(true, true);
    this->setPaintingIsUnclipped(true);

    this->volumeBlendingIndicator = make<FineTuningValueIndicator>(this->volumeBlendingAmount, "");
    this->volumeBlendingIndicator->setDisplayValue(false);
    this->volumeBlendingIndicator->setSize(40, 40);
    this->addChildComponent(this->volumeBlendingIndicator.get());

    this->reloadTrackMap();

    this->project.addListener(this);
    this->roll.getLassoSelection().addChangeListener(this);
}

VelocityProjectMap::~VelocityProjectMap()
{
    this->roll.getLassoSelection().removeChangeListener(this);
    this->project.removeListener(this);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void VelocityProjectMap::resized()
{
    VELOCITY_MAP_BULK_REPAINT_START

    for (const auto &c : this->patternMap)
    {
        const auto sequenceMap = c.second.get();
        for (const auto &e : *sequenceMap)
        {
            jassert(e.second.get());
            this->applyNoteBounds(e.second.get());
        }
    }

    if (this->dragHelper != nullptr)
    {
        this->dragHelper->updateBounds();
    }

    VELOCITY_MAP_BULK_REPAINT_END
}

void VelocityProjectMap::mouseDown(const MouseEvent &e)
{
    if (e.mods.isLeftButtonDown())
    {
        this->volumeBlendingIndicator->toFront(false);
        this->updateVolumeBlendingIndicator(e.getPosition());

        this->dragHelper = make<VelocityLevelDraggingHelper>(*this);
        this->addAndMakeVisible(this->dragHelper.get());
        this->dragHelper->setStartPosition(e.position);
        this->dragHelper->setEndPosition(e.position);
    }
}

constexpr float getVelocityByIntersection(const Point<float> &intersection)
{
    return 1.f - (intersection.y / float(Globals::UI::levelsMapHeight));
}

void VelocityProjectMap::mouseDrag(const MouseEvent &e)
{
    if (this->dragHelper != nullptr)
    {
        this->updateVolumeBlendingIndicator(e.getPosition());
        this->dragHelper->setEndPosition(e.position);
        this->dragHelper->updateBounds();
        this->applyVolumeChanges();
    }
}

void VelocityProjectMap::mouseUp(const MouseEvent &e)
{
    if (this->dragHelper != nullptr)
    {
        this->volumeBlendingIndicator->setVisible(false);
        this->dragHelper = nullptr;
        this->dragIntersections.clear();
        this->dragChangedNotes.clearQuick();
        this->dragChanges.clearQuick();
        this->dragHasChanges = false;
    }
}

void VelocityProjectMap::mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &wheel)
{
    if (this->dragHelper != nullptr)
    {
        static constexpr auto wheelSensivity = 5.f;
        const float delta = wheel.deltaY * (wheel.isReversed ? -1.f : 1.f);
        const float newBlendingAmount = this->volumeBlendingAmount + delta / wheelSensivity;
        this->volumeBlendingAmount = jlimit(0.f, 1.f, newBlendingAmount);
        this->updateVolumeBlendingIndicator(e.getPosition());
        this->applyVolumeChanges();
    }
    else
    {
        this->roll.mouseWheelMove(e.getEventRelativeTo(&this->roll), wheel);
    }
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

#define forEachSequenceMapOfGivenTrack(map, child, track) \
    for (const auto &child : map) \
        if (child.first.getPattern()->getTrack() == track)

void VelocityProjectMap::onChangeMidiEvent(const MidiEvent &e1, const MidiEvent &e2)
{
    if (e1.isTypeOf(MidiEvent::Type::Note))
    {
        const Note &note = static_cast<const Note &>(e1);
        const Note &newNote = static_cast<const Note &>(e2);
        const auto *track = newNote.getSequence()->getTrack();

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &sequenceMap = *c.second.get();
            if (auto *component = sequenceMap[note].release())
            {
                sequenceMap.erase(note);
                sequenceMap[newNote] = UniquePointer<VelocityMapNoteComponent>(component);
                this->triggerBatchRepaintFor(component);
            }
        }
    }
}

void VelocityProjectMap::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::Note))
    {
        const Note &note = static_cast<const Note &>(event);
        const auto *track = note.getSequence()->getTrack();

        VELOCITY_MAP_BULK_REPAINT_START

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &componentsMap = *c.second.get();
            const int i = track->getPattern()->indexOfSorted(&c.first);
            jassert(i >= 0);

            const auto *clip = track->getPattern()->getUnchecked(i);
            const bool editable = this->activeClip == *clip;

            auto *noteComponent = new VelocityMapNoteComponent(note, *clip);
            noteComponent->setEditable(editable);
            componentsMap[note] = UniquePointer<VelocityMapNoteComponent>(noteComponent);
            this->addAndMakeVisible(noteComponent);
            this->triggerBatchRepaintFor(noteComponent);
        }

        VELOCITY_MAP_BULK_REPAINT_END
    }
}

void VelocityProjectMap::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::Note))
    {
        const Note &note = static_cast<const Note &>(event);
        const auto *track = note.getSequence()->getTrack();

        VELOCITY_MAP_BULK_REPAINT_START

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &sequenceMap = *c.second.get();
            if (sequenceMap.contains(note))
            {
                sequenceMap.erase(note);
            }
        }

        VELOCITY_MAP_BULK_REPAINT_END
    }
}

void VelocityProjectMap::onAddClip(const Clip &clip)
{
    const SequenceMap *referenceMap = nullptr;
    const auto *track = clip.getPattern()->getTrack();
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }

    forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
    {
        // Found a sequence map for the same track
        referenceMap = c.second.get();
        break;
    }

    if (referenceMap == nullptr)
    {
        jassertfalse;
        return;
    }

    auto *sequenceMap = new SequenceMap();
    this->patternMap[clip] = UniquePointer<SequenceMap>(sequenceMap);

    VELOCITY_MAP_BULK_REPAINT_START

    for (const auto &e : *referenceMap)
    {
        // reference the same note as neighbor components:
        const auto &note = e.second.get()->note;
        const bool editable = this->activeClip == clip;

        auto *noteComponent = new VelocityMapNoteComponent(note, clip);
        noteComponent->setEditable(editable);

        (*sequenceMap)[note] = UniquePointer<VelocityMapNoteComponent>(noteComponent);
        this->addAndMakeVisible(noteComponent);
        this->applyNoteBounds(noteComponent);
    }

    VELOCITY_MAP_BULK_REPAINT_END
}

void VelocityProjectMap::onChangeClip(const Clip &clip, const Clip &newClip)
{
    if (this->patternMap.contains(clip))
    {
        // Set new key for existing sequence map
        auto *sequenceMap = this->patternMap[clip].release();
        this->patternMap.erase(clip);
        this->patternMap[newClip] = UniquePointer<SequenceMap>(sequenceMap);

        // And update all components within it, as their beats should change
        for (const auto &e : *sequenceMap)
        {
            this->batchRepaintList.add(e.second.get());
        }

        this->triggerAsyncUpdate();
    }
}

void VelocityProjectMap::onRemoveClip(const Clip &clip)
{
    VELOCITY_MAP_BULK_REPAINT_START

    if (this->patternMap.contains(clip))
    {
        this->patternMap.erase(clip);
    }

    VELOCITY_MAP_BULK_REPAINT_END
}

void VelocityProjectMap::onChangeTrackProperties(MidiTrack *const track)
{
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }

    VELOCITY_MAP_BULK_REPAINT_START

    for (const auto &c : this->patternMap)
    {
        const auto &componentsMap = *c.second.get();
        for (const auto &e : componentsMap)
        {
            e.second->updateColour();
        }
    }

    VELOCITY_MAP_BULK_REPAINT_END

    this->repaint();
}

void VelocityProjectMap::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->reloadTrackMap();
}

void VelocityProjectMap::onAddTrack(MidiTrack *const track)
{
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }

    VELOCITY_MAP_BULK_REPAINT_START
    this->loadTrack(track);
    VELOCITY_MAP_BULK_REPAINT_END
}

void VelocityProjectMap::onRemoveTrack(MidiTrack *const track)
{
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }

    for (int i = 0; i < track->getPattern()->size(); ++i)
    {
        const auto &clip = *track->getPattern()->getUnchecked(i);
        if (this->patternMap.contains(clip))
        {
            this->patternMap.erase(clip);
        }
    }
}

void VelocityProjectMap::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;

    if (this->rollFirstBeat > firstBeat || this->rollLastBeat < lastBeat)
    {
        this->rollFirstBeat = jmin(firstBeat, this->rollFirstBeat);
        this->rollLastBeat = jmax(lastBeat, this->rollLastBeat);
    }
}

void VelocityProjectMap::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    if (this->rollFirstBeat != firstBeat || this->rollLastBeat != lastBeat)
    {
        this->rollFirstBeat = firstBeat;
        this->rollLastBeat = lastBeat;
    }
}

void VelocityProjectMap::onChangeViewEditableScope(MidiTrack *const, const Clip &clip, bool)
{
    if (this->activeClip == clip)
    {
        return;
    }

    this->activeClip = clip;

    VELOCITY_MAP_BULK_REPAINT_START

    for (const auto &c : this->patternMap)
    {
        const bool editable = this->activeClip == c.first;
        const auto &componentsMap = *c.second.get();
        for (const auto &e : componentsMap)
        {
            e.second->setEditable(editable);
        }
    }

    VELOCITY_MAP_BULK_REPAINT_END
}

void VelocityProjectMap::changeListenerCallback(ChangeBroadcaster *source)
{
    // for convenience, let's set selected items as editable

    jassert(dynamic_cast<Lasso *>(source));
    const auto *selection = static_cast<Lasso *>(source);

    const auto activeMapIt = this->patternMap.find(this->activeClip);
    if (activeMapIt == this->patternMap.end())
    {
        jassertfalse;
        return;
    }

    const auto *activeMap = activeMapIt->second.get();

    VELOCITY_MAP_BULK_REPAINT_START

    if (selection->getNumSelected() == 0)
    {
        for (const auto &e : *activeMap)
        {
            e.second->setEditable(true);
        }
    }
    else
    {
        for (const auto &e : *activeMap)
        {
            e.second->setEditable(false);
        }

        for (const auto *e : *selection)
        {
            // assuming we've subscribed only on a piano roll's lasso changes
            const auto *nc = static_cast<const NoteComponent *>(e);
            activeMap->at(nc->getNote())->setEditable(true);
        }
    }

    VELOCITY_MAP_BULK_REPAINT_END
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void VelocityProjectMap::updateVolumeBlendingIndicator(const Point<int> &pos)
{
    if (this->volumeBlendingAmount == 1.f && this->volumeBlendingIndicator->isVisible())
    {
        this->fader.fadeOut(this->volumeBlendingIndicator.get(), Globals::UI::fadeOutLong);
    }
    else if (this->volumeBlendingAmount < 1.f && !this->volumeBlendingIndicator->isVisible())
    {
        this->fader.fadeIn(this->volumeBlendingIndicator.get(), Globals::UI::fadeInLong);
    }

    //this->volumeBlendingIndicator->setVisible(this->volumeBlendingAmount != 1.f);
    this->volumeBlendingIndicator->setValue(this->volumeBlendingAmount);
    this->volumeBlendingIndicator->setCentrePosition(pos);
}

void VelocityProjectMap::applyVolumeChanges()
{
    const auto *activeMap = this->patternMap.at(this->activeClip).get();
    jassert(activeMap);

    // this is where things start looking a bit dirty:
    // to update notes velocities on the fly, we use undo/redo actions (as always),
    // but we don't want to have lots of those actions in undo transaction in the end,
    // there should only be one action, which holds original notes and final new parameters;
    // the undo action is smart enough to turn a -> b, b -> c into a -> c,
    // but it only works within exactly the same group of notes,
    // which may - and will - change as the user drags the helper around,
    // so we are to track moments when the group changes and undo the current transaction

    bool shouldUndo = false;
    Point<float> intersectionA;
    Point<float> intersectionB;

    jassert(this->dragHelper);
    const auto &dragLine = this->dragHelper->getLine();
    const auto &dragLineExt = this->dragHelper->getExtendedLine();
    const bool ascending = (dragLine.getStartX() <= dragLine.getEndX() && dragLine.getStartY() >= dragLine.getEndY())
        || (dragLine.getStartX() > dragLine.getEndX() && dragLine.getStartY() < dragLine.getEndY());

    for (const auto &i : *activeMap)
    {
        if (!i.second->isEditable())
        {
            continue;
        }

        const bool ia = dragLine.intersects(i.second->getStartLine(), intersectionA);
        const bool ib = dragLine.intersects(i.second->getEndLine(), intersectionB);
        const bool hasIntersection = ia || ib;

        float intersectionVelocity = 0.f;
        if (ascending)
        {
            if (ia)
            {
                intersectionVelocity = getVelocityByIntersection(intersectionA);
            }
            else
            {
                dragLineExt.intersects(i.second->getStartLine(), intersectionA);
                intersectionVelocity = getVelocityByIntersection(intersectionA);
            }
        }
        else
        {
            if (ib)
            {
                intersectionVelocity = getVelocityByIntersection(intersectionB);
            }
            else
            {
                dragLineExt.intersects(i.second->getEndLine(), intersectionB);
                intersectionVelocity = getVelocityByIntersection(intersectionB);
            }
        }

        auto existingIntersection = this->dragIntersections.find(i.first);
        if (hasIntersection && existingIntersection == this->dragIntersections.end())
        {
            // found new intersection
            shouldUndo = true;
            this->dragIntersections.emplace(i.first, intersectionVelocity);
        }
        else if (!hasIntersection && existingIntersection != this->dragIntersections.end())
        {
            // lost existing intersection
            shouldUndo = true;
            this->dragIntersections.erase(existingIntersection);
        }
        else if (hasIntersection)
        {
            this->dragIntersections[i.first] = intersectionVelocity;
        }
    }

    // filling up arrays all the time on mouse drag - kinda sucks, nah?
    this->dragChangedNotes.clearQuick();
    this->dragChanges.clearQuick();

    for (const auto &i : this->dragIntersections)
    {
        const auto newVelocity = (i.second * this->volumeBlendingAmount) +
            (i.first.getVelocity() * (1.f - this->volumeBlendingAmount));

        this->dragChangedNotes.add(i.first);
        this->dragChanges.add(i.first.withVelocity(newVelocity));
    }

    auto *sequence = static_cast<PianoSequence *>(this->activeClip.getPattern()->getTrack()->getSequence());

    if (shouldUndo && this->dragHasChanges)
    {
        sequence->undoCurrentTransactionOnly();
    }

    if (!this->dragChangedNotes.isEmpty())
    {
        if (!this->dragHasChanges)
        {
            this->dragHasChanges = true;
            sequence->checkpoint();
        }

        sequence->changeGroup(this->dragChangedNotes, this->dragChanges, true);
    }
}

void VelocityProjectMap::reloadTrackMap()
{
    this->patternMap.clear();

    VELOCITY_MAP_BULK_REPAINT_START

    const auto &tracks = this->project.getTracks();
    for (const auto *track : tracks)
    {
        if (dynamic_cast<const PianoSequence *>(track->getSequence()))
        {
            this->loadTrack(track);
        }
    }

    VELOCITY_MAP_BULK_REPAINT_END
}

void VelocityProjectMap::loadTrack(const MidiTrack *const track)
{
    if (track->getPattern() == nullptr)
    {
        return;
    }

    for (int i = 0; i < track->getPattern()->size(); ++i)
    {
        const Clip *clip = track->getPattern()->getUnchecked(i);
        const bool editable = this->activeClip == *clip;

        auto *sequenceMap = new SequenceMap();
        this->patternMap[*clip] = UniquePointer<SequenceMap>(sequenceMap);

        for (int j = 0; j < track->getSequence()->size(); ++j)
        {
            const MidiEvent *event = track->getSequence()->getUnchecked(j);
            if (event->isTypeOf(MidiEvent::Type::Note))
            {
                const auto *note = static_cast<const Note *>(event);
                auto *noteComponent = new VelocityMapNoteComponent(*note, *clip);
                noteComponent->setEditable(editable);
                (*sequenceMap)[*note] = UniquePointer<VelocityMapNoteComponent>(noteComponent);
                this->addAndMakeVisible(noteComponent);
                this->applyNoteBounds(noteComponent);
            }
        }
    }
}

void VelocityProjectMap::applyNoteBounds(VelocityMapNoteComponent *nc)
{
    const float rollLengthInBeats = (this->rollLastBeat - this->rollFirstBeat);
    const float projectLengthInBeats = (this->projectLastBeat - this->projectFirstBeat);

    const float beat = (nc->getBeat() - this->rollFirstBeat);
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

    const float x = (mapWidth * (beat / projectLengthInBeats));
    const float w = (mapWidth * (nc->getLength() / projectLengthInBeats));

    // at least 4 pixels are visible for 0 volume events:
    const int h = jmax(4, int(this->getHeight() * nc->getVelocity()));
    nc->setRealBounds(x, this->getHeight() - h, jmax(1.f, w), h);
}

void VelocityProjectMap::triggerBatchRepaintFor(VelocityMapNoteComponent *target)
{
    this->batchRepaintList.add(target);
    this->triggerAsyncUpdate();
}

void VelocityProjectMap::handleAsyncUpdate()
{
    // batch repaint & resize stuff
    if (this->batchRepaintList.size() > 0)
    {
        VELOCITY_MAP_BULK_REPAINT_START

        for (int i = 0; i < this->batchRepaintList.size(); ++i)
        {
            // There are still many cases when a scheduled component is deleted at this time:
            if (Component *component = this->batchRepaintList.getUnchecked(i))
            {
                this->applyNoteBounds(static_cast<VelocityMapNoteComponent *>(component));
            }
        }

        VELOCITY_MAP_BULK_REPAINT_END

        this->batchRepaintList.clearQuick();
    }
}
