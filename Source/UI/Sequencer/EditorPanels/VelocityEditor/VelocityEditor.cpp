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
#include "VelocityEditor.h"
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

#define VELOCITY_MAP_BATCH_REPAINT_START \
    if (this->isEnabled()) { this->setVisible(false); }

#define VELOCITY_MAP_BATCH_REPAINT_END \
    if (this->isEnabled()) { this->setVisible(true); }

//===----------------------------------------------------------------------===//
// Child level component
//===----------------------------------------------------------------------===//

class VelocityEditorNoteComponent final : public Component
{
public:

    VelocityEditorNoteComponent(const Note &note, const Clip &clip) :
        note(note),
        clip(clip)
    {
        this->updateColour();
        this->setInterceptsMouseClicks(true, false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
        this->setMouseCursor(MouseCursor::UpDownResizeCursor);
    }

    inline float getNoteBeat() const noexcept
    {
        return this->note.getBeat();
    }

    inline float getClipBeat() const noexcept
    {
        return this->clip.getBeat();
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
        const auto baseColour = findDefaultColour(ColourIDs::Roll::noteFill);
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

        if (this->getWidth() > 2)
        {
            g.fillRect(this->dx + 1.f, 0.f, float(this->getWidth() - 2) + this->dw, 1.f);
            g.fillRect(this->dx, 1.f, float(this->getWidth()) + this->dw, 2.f);
        }

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

    friend class VelocityEditor;

    VelocityEditor *getParentMap() const
    {
        jassert(this->getParentComponent());
        return static_cast<VelocityEditor *>(this->getParentComponent());
    }

    const Note &note;
    const Clip &clip;

    Colour mainColour;
    Colour paleColour;

    float dx = 0.f;
    float dw = 0.f;

    float velocityAnchor = 0.f;
    bool editable = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityEditorNoteComponent)
};

//===----------------------------------------------------------------------===//
// Dragging helper
//===----------------------------------------------------------------------===//

class VelocityLevelDraggingHelper final : public Component
{
public:

    VelocityLevelDraggingHelper(VelocityEditor &map) : map(map)
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

    VelocityEditor &map;

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

VelocityEditor::VelocityEditor(ProjectNode &project, SafePointer<RollBase> roll) :
    project(project),
    roll(roll)
{
    this->setInterceptsMouseClicks(true, true);
    this->setPaintingIsUnclipped(true);

    this->volumeBlendingIndicator = make<FineTuningValueIndicator>(this->volumeBlendingAmount, "");
    this->volumeBlendingIndicator->setDisplayValue(false);
    this->volumeBlendingIndicator->setSize(40, 40);
    this->addChildComponent(this->volumeBlendingIndicator.get());

    this->reloadTrackMap();

    this->project.addListener(this);
    this->roll->getLassoSelection().addChangeListener(this);
}

VelocityEditor::~VelocityEditor()
{
    this->roll->getLassoSelection().removeChangeListener(this);
    this->project.removeListener(this);
}
void VelocityEditor::switchToRoll(SafePointer<RollBase> roll)
{
    this->roll->getLassoSelection().removeChangeListener(this);
    this->roll = roll;
    this->roll->getLassoSelection().addChangeListener(this);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void VelocityEditor::resized()
{
    VELOCITY_MAP_BATCH_REPAINT_START

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

    VELOCITY_MAP_BATCH_REPAINT_END
}

void VelocityEditor::mouseDown(const MouseEvent &e)
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

void VelocityEditor::mouseDrag(const MouseEvent &e)
{
    if (this->dragHelper != nullptr)
    {
        this->updateVolumeBlendingIndicator(e.getPosition());
        this->dragHelper->setEndPosition(e.position);
        this->dragHelper->updateBounds();
        this->applyVolumeChanges();
    }
}

void VelocityEditor::mouseUp(const MouseEvent &e)
{
    if (this->dragHelper != nullptr)
    {
        this->volumeBlendingIndicator->setVisible(false);
        this->dragHelper = nullptr;
        this->dragIntersections.clear();
        this->dragChangesBefore.clearQuick();
        this->dragChangesAfter.clearQuick();
        this->dragHasChanges = false;
    }
}

void VelocityEditor::mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &wheel)
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
        this->roll->mouseWheelMove(e.getEventRelativeTo(this->roll), wheel);
    }
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

#define forEachSequenceMapOfGivenTrack(map, child, track) \
    for (const auto &child : map) \
        if (child.first.getPattern()->getTrack() == track)

void VelocityEditor::onChangeMidiEvent(const MidiEvent &e1, const MidiEvent &e2)
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
                sequenceMap[newNote] = UniquePointer<VelocityEditorNoteComponent>(component);
                this->triggerBatchRepaintFor(component);
            }
        }
    }
}

void VelocityEditor::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::Note))
    {
        const Note &note = static_cast<const Note &>(event);
        const auto *track = note.getSequence()->getTrack();

        VELOCITY_MAP_BATCH_REPAINT_START

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &componentsMap = *c.second.get();
            const int i = track->getPattern()->indexOfSorted(&c.first);
            jassert(i >= 0);

            const auto *clip = track->getPattern()->getUnchecked(i);
            const bool isEditable = this->activeClip == *clip;

            auto *noteComponent = new VelocityEditorNoteComponent(note, *clip);
            noteComponent->setEditable(isEditable);
            componentsMap[note] = UniquePointer<VelocityEditorNoteComponent>(noteComponent);
            this->addAndMakeVisible(noteComponent);
            this->triggerBatchRepaintFor(noteComponent);
        }

        VELOCITY_MAP_BATCH_REPAINT_END
    }
}

void VelocityEditor::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::Note))
    {
        const Note &note = static_cast<const Note &>(event);
        const auto *track = note.getSequence()->getTrack();

        VELOCITY_MAP_BATCH_REPAINT_START

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &sequenceMap = *c.second.get();
            if (sequenceMap.contains(note))
            {
                sequenceMap.erase(note);
            }
        }

        VELOCITY_MAP_BATCH_REPAINT_END
    }
}

void VelocityEditor::onAddClip(const Clip &clip)
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

    VELOCITY_MAP_BATCH_REPAINT_START

    for (const auto &e : *referenceMap)
    {
        const auto &note = e.second.get()->note;
        const bool isEditable = this->activeClip == clip;

        auto *noteComponent = new VelocityEditorNoteComponent(note, clip);
        noteComponent->setEditable(isEditable);

        (*sequenceMap)[note] = UniquePointer<VelocityEditorNoteComponent>(noteComponent);
        this->addAndMakeVisible(noteComponent);
        this->applyNoteBounds(noteComponent);
    }

    VELOCITY_MAP_BATCH_REPAINT_END
}

void VelocityEditor::onChangeClip(const Clip &clip, const Clip &newClip)
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

void VelocityEditor::onRemoveClip(const Clip &clip)
{
    VELOCITY_MAP_BATCH_REPAINT_START

    if (this->patternMap.contains(clip))
    {
        this->patternMap.erase(clip);
    }

    VELOCITY_MAP_BATCH_REPAINT_END
}

void VelocityEditor::onChangeTrackProperties(MidiTrack *const track)
{
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }

    VELOCITY_MAP_BATCH_REPAINT_START

    for (const auto &c : this->patternMap)
    {
        const auto &componentsMap = *c.second.get();
        for (const auto &e : componentsMap)
        {
            e.second->updateColour();
        }
    }

    VELOCITY_MAP_BATCH_REPAINT_END

    this->repaint();
}

void VelocityEditor::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->reloadTrackMap();
}

void VelocityEditor::onAddTrack(MidiTrack *const track)
{
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }

    VELOCITY_MAP_BATCH_REPAINT_START
    this->loadTrack(track);
    VELOCITY_MAP_BATCH_REPAINT_END
}

void VelocityEditor::onRemoveTrack(MidiTrack *const track)
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

void VelocityEditor::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;

    if (this->rollFirstBeat > firstBeat || this->rollLastBeat < lastBeat)
    {
        this->rollFirstBeat = jmin(firstBeat, this->rollFirstBeat);
        this->rollLastBeat = jmax(lastBeat, this->rollLastBeat);
    }
}

void VelocityEditor::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    if (this->rollFirstBeat != firstBeat || this->rollLastBeat != lastBeat)
    {
        this->rollFirstBeat = firstBeat;
        this->rollLastBeat = lastBeat;
    }
}

//===----------------------------------------------------------------------===//
// Editable scope selection
//===----------------------------------------------------------------------===//

// Only called when the piano roll is showing
void VelocityEditor::onChangeViewEditableScope(MidiTrack *const, const Clip &clip, bool)
{
    if (this->activeClip == clip)
    {
        return;
    }

    this->activeClip = clip;

    VELOCITY_MAP_BATCH_REPAINT_START

    for (const auto &c : this->patternMap)
    {
        const bool isEditable = c.first == clip;
        const auto &componentsMap = *c.second.get();
        for (const auto &e : componentsMap)
        {
            e.second->setEditable(isEditable);
        }
    }

    VELOCITY_MAP_BATCH_REPAINT_END
}

// Can be called by both the piano roll and the pattern roll
void VelocityEditor::changeListenerCallback(ChangeBroadcaster *source)
{
    jassert(dynamic_cast<Lasso *>(source));
    const auto *selection = static_cast<Lasso *>(source);

    if (dynamic_cast<PianoRoll *>(this->roll.getComponent()))
    {
        assert(this->activeClip.hasValue());
        const auto activeMapIt = this->patternMap.find(*this->activeClip);
        if (activeMapIt == this->patternMap.end())
        {
            jassertfalse;
            return;
        }

        const auto *activeMap = activeMapIt->second.get();

        VELOCITY_MAP_BATCH_REPAINT_START

        if (selection->getNumSelected() == 0)
        {
            // no selection: the entire clip is editable
            for (const auto &it : *activeMap)
            {
                it.second->setEditable(true);
            }
        }
        else
        {
            for (const auto &it : *activeMap)
            {
                it.second->setEditable(false);
            }

            for (const auto *component : *selection)
            {
                assert(dynamic_cast<const NoteComponent *>(component) != nullptr);
                const auto *nc = dynamic_cast<const NoteComponent *>(component);
                activeMap->at(nc->getNote())->setEditable(true);
            }
        }

        VELOCITY_MAP_BATCH_REPAINT_END
    }
    else if (dynamic_cast<PatternRoll *>(this->roll.getComponent()))
    {
        if (selection->getNumSelected() == 1)
        {
            const auto *cc = selection->getFirstAs<ClipComponent>();
            this->activeClip = cc->getClip();
        }
        else
        {
            // simply disallow editing multiple clips at once,
            // some of them may be the instances of the same track
            this->activeClip = {};
        }

        VELOCITY_MAP_BATCH_REPAINT_START

        for (const auto &it : this->patternMap)
        {
            const auto isEditable = this->activeClip == it.first;
            const auto sequenceMap = it.second.get();
            for (const auto &e : *sequenceMap)
            {
                jassert(e.second.get());
                e.second->setEditable(isEditable);
            }
        }

        VELOCITY_MAP_BATCH_REPAINT_END
    }
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void VelocityEditor::updateVolumeBlendingIndicator(const Point<int> &pos)
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

void VelocityEditor::applyVolumeChanges()
{
    if (!this->activeClip.hasValue())
    {
        return; // no editable components
    }

    const auto activeClip = *this->activeClip;
    const auto *activeMap = this->patternMap.at(activeClip).get();
    jassert(activeMap);

    // this is where things start looking a bit dirty:
    // to update notes velocities on the fly, we use undo/redo actions (as always),
    // but we don't want to have lots of those actions in the undo transaction in the end,
    // there should only be one action which holds the original parameters and the final parameters;
    // the undo action is smart enough to turn a -> b, b -> c into a -> c,
    // but it only works within the same group of notes, which may and will change
    // as the user drags the helper around, so we track moments
    // when the notes group changes and undo the current transaction

    bool shouldUndo = false;
    Point<float> intersectionA;
    Point<float> intersectionB;

    jassert(this->dragHelper);
    const auto &dragLine = this->dragHelper->getLine();
    const auto &dragLineExt = this->dragHelper->getExtendedLine();
    const bool ascending = (dragLine.getStartX() <= dragLine.getEndX() && dragLine.getStartY() >= dragLine.getEndY()) ||
        (dragLine.getStartX() > dragLine.getEndX() && dragLine.getStartY() < dragLine.getEndY());

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

    this->dragChangesBefore.clearQuick();
    this->dragChangesAfter.clearQuick();

    for (const auto &i : this->dragIntersections)
    {
        const auto newVelocity = (i.second * this->volumeBlendingAmount) +
            (i.first.getVelocity() * (1.f - this->volumeBlendingAmount));

        this->dragChangesBefore.add(i.first);
        this->dragChangesAfter.add(i.first.withVelocity(newVelocity));
    }

    auto *sequence = static_cast<PianoSequence *>(activeClip.getPattern()->getTrack()->getSequence());

    if (shouldUndo && this->dragHasChanges)
    {
        this->project.getUndoStack()->undoCurrentTransactionOnly();
    }

    if (!this->dragChangesBefore.isEmpty())
    {
        if (!this->dragHasChanges)
        {
            this->dragHasChanges = true;
            this->project.checkpoint();
        }

        sequence->changeGroup(this->dragChangesBefore, this->dragChangesAfter, true);
    }
}

void VelocityEditor::reloadTrackMap()
{
    this->patternMap.clear();

    VELOCITY_MAP_BATCH_REPAINT_START

    for (const auto *track : this->project.getTracks())
    {
        if (dynamic_cast<const PianoSequence *>(track->getSequence()))
        {
            this->loadTrack(track);
        }
    }

    VELOCITY_MAP_BATCH_REPAINT_END
}

void VelocityEditor::loadTrack(const MidiTrack *const track)
{
    if (track->getPattern() == nullptr)
    {
        return;
    }

    for (int i = 0; i < track->getPattern()->size(); ++i)
    {
        const auto *clip = track->getPattern()->getUnchecked(i);
        const bool isEditable = this->activeClip == *clip;

        auto *sequenceMap = new SequenceMap();
        this->patternMap[*clip] = UniquePointer<SequenceMap>(sequenceMap);

        for (int j = 0; j < track->getSequence()->size(); ++j)
        {
            const auto *event = track->getSequence()->getUnchecked(j);
            if (event->isTypeOf(MidiEvent::Type::Note))
            {
                const auto *note = static_cast<const Note *>(event);
                auto *noteComponent = new VelocityEditorNoteComponent(*note, *clip);
                noteComponent->setEditable(isEditable);
                (*sequenceMap)[*note] = UniquePointer<VelocityEditorNoteComponent>(noteComponent);
                this->addAndMakeVisible(noteComponent);
                this->applyNoteBounds(noteComponent);
            }
        }
    }
}

void VelocityEditor::applyNoteBounds(VelocityEditorNoteComponent *nc)
{
    const float rollLengthInBeats = this->rollLastBeat - this->rollFirstBeat;
    const float projectLengthInBeats = this->projectLastBeat - this->projectFirstBeat;

    const float beat = nc->getNoteBeat() + nc->getClipBeat() - this->rollFirstBeat;
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

    const float x = mapWidth * (beat / projectLengthInBeats);
    const float w = mapWidth * (nc->getLength() / projectLengthInBeats);

    // at least 4 pixels are visible for 0 volume events:
    const int h = jmax(4, int(this->getHeight() * nc->getVelocity()));
    nc->setRealBounds(x, this->getHeight() - h, jmax(1.f, w), h);
}

void VelocityEditor::triggerBatchRepaintFor(VelocityEditorNoteComponent *target)
{
    this->batchRepaintList.add(target);
    this->triggerAsyncUpdate();
}

void VelocityEditor::handleAsyncUpdate()
{
    // batch repaint & resize stuff
    if (this->batchRepaintList.size() > 0)
    {
        VELOCITY_MAP_BATCH_REPAINT_START

        for (int i = 0; i < this->batchRepaintList.size(); ++i)
        {
            // There are still many cases when a component
            // scheduled for repainting/repositioning is deleted at this time:
            if (Component *component = this->batchRepaintList.getUnchecked(i))
            {
                this->applyNoteBounds(static_cast<VelocityEditorNoteComponent *>(component));
            }
        }

        VELOCITY_MAP_BATCH_REPAINT_END

        this->batchRepaintList.clearQuick();
    }
}
