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
// Single note velocity component
//===----------------------------------------------------------------------===//

class VelocityEditorNoteComponent final : public Component
{
public:

    VelocityEditorNoteComponent(const Note &note, const Clip &clip) :
        note(note),
        clip(clip)
    {
        this->setInterceptsMouseClicks(true, false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
        this->setMouseCursor(MouseCursor::UpDownResizeCursor);
        this->updateColour();
    }

    inline const Note &getNote() const noexcept
    {
        return this->note;
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

    inline float getFullVelocity() const noexcept
    {
        return this->note.getVelocity() * this->clip.getVelocity();
    }

    inline float getFullBeat() const noexcept
    {
        return this->note.getBeat() + this->clip.getBeat();
    }

    Line<float> getBeatIntersectionLine() const noexcept
    {
        const auto beat = this->getFullBeat();
        return { beat, 0.f, beat, float(Globals::UI::levelsMapHeight) };
    }

    inline void updateColour()
    {
        const auto baseColour = findDefaultColour(ColourIDs::Roll::noteFill);
        this->mainColour = this->note.getTrackColour()
            .interpolatedWith(baseColour, this->isEditable ? 0.35f : 0.5f)
            .withAlpha(this->isEditable ? 0.75f : 0.065f);
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

    void setEditable(bool shouldBeEditable)
    {
        if (this->isEditable == shouldBeEditable)
        {
            return;
        }

        this->isEditable = shouldBeEditable;

        this->updateColour();

        if (this->isEditable)
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
        return this->isEditable;
    }

    void mouseDown(const MouseEvent &e) override
    {
        jassert(this->isEditable);
        this->getEditor()->startFineTuning(this, e);
    }

    void mouseDrag(const MouseEvent &e) override
    {
        jassert(this->isEditable);
        this->getEditor()->continueFineTuning(this, e);
    }

    void mouseUp(const MouseEvent &e) override
    {
        jassert(this->isEditable);
        this->getEditor()->endFineTuning(this, e);
    }

private:

    friend class VelocityEditor;

    VelocityEditor *getEditor() const
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

    bool isEditable = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityEditorNoteComponent)
};

//===----------------------------------------------------------------------===//
// Dragging helper
//===----------------------------------------------------------------------===//

class VelocityHandDrawingHelper final : public Component
{
public:

    VelocityHandDrawingHelper(VelocityEditor &map) : map(map)
    {
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
    }

    const Array<Line<float>> &getCurve() const noexcept
    {
        return this->curveInBeatVelocity;
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->colour);
        g.fillPath(this->curveInPixels);
    }

    void setStartMousePosition(const Point<float> &mousePos)
    {
        this->rawPositions.clearQuick();
        this->simplifiedPositions.clearQuick();
        this->lastSimplifiedRawPointIndex = 0;

        const auto newPosition = mousePos.toDouble() / this->getSize();

        this->rawPositions.add(newPosition);
        this->simplifiedPositions.add(newPosition);
    }

    void addMousePosition(const Point<float> &mousePos, const float viewWidth)
    {
        jassert(!this->rawPositions.isEmpty());
        jassert(!this->simplifiedPositions.isEmpty());

        const auto mySize = this->getSize();
        const auto newPosition = mousePos.toDouble() / mySize;

        // pre-filtering: add it to raw positions if it's more than a couple of pixels away
        constexpr auto prefilterThreshold = 1;
        const auto lastPositionInPixels = this->rawPositions.getLast() * mySize;
        const auto d = lastPositionInPixels.getDistanceFrom(mousePos.toDouble());
        if (d > prefilterThreshold)
        {
            this->rawPositions.add(newPosition);
        }

        // "lazy" reduction: update the simplified curve only when raw positions are piling up
        constexpr auto lazyReductionThreshold = 3;
        if (this->rawPositions.size() - this->lastSimplifiedRawPointIndex > lazyReductionThreshold)
        {
            // the epsilon is picked to cut ~50% of all points at any zoom level,
            // which is a small reduction so that the curve shape barely changes;
            // the point of doing it is that it filters out the noise nicely:
            const auto epsilon  = jmax(0.000001f, viewWidth / float(mySize.getX()) * 0.001f);
            this->simplifiedPositions =
                PointReduction<double>::simplify(this->rawPositions, epsilon);

            //DBG("Epsilon " + String(epsilon) +
            //    " reduced " + String(this->rawPositions.size()) +
            //    " points to " + String(this->simplifiedPositions.size()));

            this->lastSimplifiedRawPointIndex = this->rawPositions.size() - 1;
        }
        
        if (this->simplifiedPositions.size() >= 2)
        {
            this->simplifiedPositions[this->simplifiedPositions.size() - 1] = newPosition;
        }
    }

    void updateCurves()
    {
        jassert(!this->simplifiedPositions.isEmpty());

        const auto mySize = this->getSize();

        this->curveInBeatVelocity.clearQuick();
        this->curveInPixels.clear();

        auto previousPoint = (this->simplifiedPositions.getFirst() * mySize).toFloat();
        this->curveInPixels.startNewSubPath(previousPoint);

        for (int i = 1; i < this->simplifiedPositions.size(); ++i)
        {
            const auto nextPoint = (this->simplifiedPositions.getUnchecked(i) * mySize).toFloat();
            this->curveInPixels.lineTo(nextPoint);

            this->curveInBeatVelocity.add({
                {this->map.getBeatByXPosition(previousPoint.x), previousPoint.y},
                {this->map.getBeatByXPosition(nextPoint.x), nextPoint.y}});

            previousPoint = nextPoint;
        }

        static const float lineThickness = 1.f;
        static Array<float> dashes(4.f, 3.f);
        PathStrokeType(1.f).createDashedStroke(this->curveInPixels, this->curveInPixels,
            dashes.getRawDataPointer(), dashes.size());
    }

private:

    VelocityEditor &map;

    Array<Point<double>> rawPositions;
    Array<Point<double>> simplifiedPositions; // both in absolute values, 0..1
    int lastSimplifiedRawPointIndex = 0;

    Path curveInPixels; // for painting
    Array<Line<float>> curveInBeatVelocity; // for convenient intersection checks

    const Colour colour = findDefaultColour(Label::textColourId);

    inline const Point<double> getSize() const noexcept
    {
        return this->getLocalBounds().getBottomRight().toDouble();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityHandDrawingHelper)
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
    this->volumeBlendingIndicator->setShouldDisplayValue(false);
    this->volumeBlendingIndicator->setSize(40, 40);
    this->addChildComponent(this->volumeBlendingIndicator.get());

    this->reloadTrackMap();

    this->project.addListener(this);
}

VelocityEditor::~VelocityEditor()
{
    this->project.removeListener(this);
}

float VelocityEditor::getBeatByXPosition(float x) const noexcept
{
    jassert(this->getWidth() == this->roll->getWidth());
    return this->roll->getBeatByXPosition(x);
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

    if (this->handDrawingHelper != nullptr)
    {
        this->handDrawingHelper->setBounds(this->getLocalBounds());
        this->handDrawingHelper->updateCurves();
    }

    VELOCITY_MAP_BATCH_REPAINT_END
}

void VelocityEditor::mouseDown(const MouseEvent &e)
{
    if (this->roll->hasMultiTouch(e))
    {
        return;
    }

    // roll panning hack
    if (this->roll->isViewportDragEvent(e))
    {
        this->roll->mouseDown(e.getEventRelativeTo(this->roll));
        return;
    }

    this->volumeBlendingIndicator->toFront(false);
    this->updateVolumeBlendingIndicator(e.getPosition());

    this->handDrawingHelper = make<VelocityHandDrawingHelper>(*this);
    this->addAndMakeVisible(this->handDrawingHelper.get());
    this->handDrawingHelper->setBounds(this->getLocalBounds());
    this->handDrawingHelper->setStartMousePosition(e.position);
}

void VelocityEditor::mouseDrag(const MouseEvent &e)
{
    if (this->roll->hasMultiTouch(e))
    {
        return;
    }

    if (this->roll->isViewportDragEvent(e))
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->roll->mouseDrag(e.getEventRelativeTo(this->roll));
        return;
    }

    if (this->handDrawingHelper != nullptr)
    {
        this->updateVolumeBlendingIndicator(e.getPosition());
        this->handDrawingHelper->addMousePosition(e.position,
            float(this->roll->getViewport().getViewWidth()));
        this->handDrawingHelper->updateCurves();
        this->handDrawingHelper->repaint();
        this->applyGroupVolumeChanges();
    }
}

void VelocityEditor::mouseUp(const MouseEvent &e)
{
    // no multi-touch check here, need to exit the editing mode (if any) even in multi-touch
    //if (this->roll->hasMultiTouch(e)) { return; }

    if (this->roll->isViewportDragEvent(e))
    {
        this->setMouseCursor(MouseCursor::NormalCursor);
        this->roll->mouseUp(e.getEventRelativeTo(this->roll));
        return;
    }

    if (this->handDrawingHelper != nullptr)
    {
        this->volumeBlendingIndicator->setVisible(false);
        this->handDrawingHelper = nullptr;
        this->groupDragIntersections.clear();
        this->groupDragChangesBefore.clearQuick();
        this->groupDragChangesAfter.clearQuick();
        this->groupDragHasChanges = false;
    }
}

void VelocityEditor::mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &wheel)
{
    if (this->handDrawingHelper != nullptr)
    {
        static constexpr auto wheelSensivity = 5.f;
        const float delta = wheel.deltaY * (wheel.isReversed ? -1.f : 1.f);
        const float newBlendingAmount = this->volumeBlendingAmount + delta / wheelSensivity;
        this->volumeBlendingAmount = jlimit(0.f, 1.f, newBlendingAmount);
        this->updateVolumeBlendingIndicator(e.getPosition());
        this->applyGroupVolumeChanges();
    }
    else
    {
        this->roll->mouseWheelMove(e.getEventRelativeTo(this->roll), wheel);
    }
}

//===----------------------------------------------------------------------===//
// ScrolledComponent
//===----------------------------------------------------------------------===//

void VelocityEditor::switchToRoll(SafePointer<RollBase> roll)
{
    this->roll = roll;
}

void VelocityEditor::setEditableScope(Optional<Clip> clip)
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

void VelocityEditor::setEditableScope(WeakReference<Lasso> selection)
{
    jassert(this->activeClip.hasValue());
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
            jassert(dynamic_cast<const NoteComponent *>(component) != nullptr);
            const auto *nc = dynamic_cast<const NoteComponent *>(component);
            activeMap->at(nc->getNote())->setEditable(true);
        }
    }

    VELOCITY_MAP_BATCH_REPAINT_END
}

bool VelocityEditor::canEditSequence(WeakReference<MidiSequence> sequence) const
{
    return dynamic_cast<PianoSequence *>(sequence.get()) != nullptr;
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
            const auto *targetClipParams = &c.first;
            const int i = track->getPattern()->indexOfSorted(targetClipParams);
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
// Fine-tuning a single note
//===----------------------------------------------------------------------===//

void VelocityEditor::startFineTuning(VelocityEditorNoteComponent *target, const MouseEvent &e)
{
    if (e.mods.isLeftButtonDown())
    {
        this->project.checkpoint();
        this->fineTuningVelocityAnchor = target->getNote().getVelocity();

        this->fineTuningDragger.startDraggingComponent(target, e,
            this->fineTuningVelocityAnchor,
            0.f, 1.f, VelocityEditor::fineTuningStep,
            FineTuningComponentDragger::Mode::DragOnlyY);

        this->fineTuningIndicator = make<FineTuningValueIndicator>(0.f, "");

        const auto finalMidiVelocity = MidiMessage::floatValueToMidiByte(target->getFullVelocity());
        this->fineTuningIndicator->setValue(target->getNote().getVelocity(), int(finalMidiVelocity));

        // adding it to grandparent to avoid clipping
        jassert(this->getParentComponent() != nullptr);
        jassert(this->getParentComponent()->getParentComponent() != nullptr);
        auto *grandParent = this->getParentComponent()->getParentComponent();

        grandParent->addAndMakeVisible(this->fineTuningIndicator.get());
        this->fineTuningIndicator->repositionAtTargetTop(target);
        this->fader.fadeIn(this->fineTuningIndicator.get(), Globals::UI::fadeInLong);

        // todo send midi?
    }
}

void VelocityEditor::continueFineTuning(VelocityEditorNoteComponent *target, const MouseEvent &e)
{
    if (this->fineTuningIndicator != nullptr)
    {
        this->fineTuningDragger.dragComponent(target, e);

        // fine-tuning by default, simple dragging with mod keys
        const auto newNoteVelocity = e.mods.isAnyModifierKeyDown() ?
            jlimit(0.f, 1.f, this->fineTuningVelocityAnchor -
                float(e.getDistanceFromDragStartY()) / float(Globals::UI::levelsMapHeight)) :
            this->fineTuningDragger.getValue();

        const bool velocityChanged = target->getNote().getVelocity() != newNoteVelocity;

        if (velocityChanged)
        {
            this->setMouseCursor(MouseCursor::DraggingHandCursor);
            auto *sequence = static_cast<PianoSequence *>(target->getNote().getSequence());
            sequence->change(target->getNote(), target->getNote().withVelocity(newNoteVelocity), true);
        }
        else
        {
            this->applyNoteBounds(target);
        }

        jassert(this->fineTuningIndicator != nullptr);
        const auto finalMidiVelocity = MidiMessage::floatValueToMidiByte(target->getFullVelocity());
        this->fineTuningIndicator->setValue(newNoteVelocity, int(finalMidiVelocity));
        this->fineTuningIndicator->repositionAtTargetTop(target);
    }
}

void VelocityEditor::endFineTuning(VelocityEditorNoteComponent *target, const MouseEvent &e)
{
    if (this->fineTuningIndicator != nullptr)
    {
        this->fader.fadeOut(this->fineTuningIndicator.get(), Globals::UI::fadeOutLong);
        this->fineTuningIndicator = nullptr;

        this->fineTuningDragger.endDraggingComponent(target, e);

        this->applyNoteBounds(target);
    }
}

//===----------------------------------------------------------------------===//
// Adjusting a group of notes
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

constexpr float getVelocityByIntersection(const Point<float> &intersection)
{
    return 1.f - (intersection.y / float(Globals::UI::levelsMapHeight));
}

void VelocityEditor::applyGroupVolumeChanges()
{
    if (!this->activeClip.hasValue())
    {
        return; // no editable components
    }

    if (this->groupDragHasChanges)
    {
        // for simple actions, like dragging a note, the undo action logic
        // can coalease multiple changes into one, e.g. a->b, b->c becomes a->c,
        // but here drawing a curve affects variable number of events,
        // so coalescing is non-trivial, and the simplest way to keep
        // the undo stack nice and clean is to undo the changes we've made before,
        // and then apply new changes, hopefully it isn't too slow:
        this->project.getUndoStack()->undoCurrentTransactionOnly();
    }

    const auto activeClip = *this->activeClip;
    if (!this->patternMap.contains(activeClip))
    {
        jassertfalse;
        return;
    }

    const auto *activeMap = this->patternMap.at(activeClip).get();

    Point<float> intersectionPoint;

    this->groupDragIntersections.clear();

    jassert(this->handDrawingHelper != nullptr);

    for (const auto &i : *activeMap)
    {
        if (!i.second->isEditable)
        {
            continue;
        }

        for (const auto &line : this->handDrawingHelper->getCurve())
        {
            if (line.intersects(i.second->getBeatIntersectionLine(), intersectionPoint))
            {
                this->groupDragIntersections[i.first] = getVelocityByIntersection(intersectionPoint);
                break;
            }
        }
    }

    this->groupDragChangesBefore.clearQuick();
    this->groupDragChangesAfter.clearQuick();

    for (const auto &i : this->groupDragIntersections)
    {
        const auto newVelocity = (i.second * this->volumeBlendingAmount) +
            (i.first.getVelocity() * (1.f - this->volumeBlendingAmount));

        if (newVelocity != i.first.getVelocity())
        {
            this->groupDragChangesBefore.add(i.first);
            this->groupDragChangesAfter.add(i.first.withVelocity(newVelocity));
        }
    }

    auto *sequence = static_cast<PianoSequence *>(activeClip.getPattern()->getTrack()->getSequence());

    if (!this->groupDragChangesBefore.isEmpty())
    {
        if (!this->groupDragHasChanges)
        {
            this->groupDragHasChanges = true;
            this->project.checkpoint();
        }

        sequence->changeGroup(this->groupDragChangesBefore, this->groupDragChangesAfter, true);
    }
}

//===----------------------------------------------------------------------===//
// Reloading / resizing / repainting
//===----------------------------------------------------------------------===//

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
        jassertfalse;
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

    const float beat = nc->getFullBeat() - this->rollFirstBeat;
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

    const float x = mapWidth * (beat / projectLengthInBeats);
    const float w = mapWidth * (nc->getLength() / projectLengthInBeats);

    // at least 4 pixels are visible for 0 volume events:
    const int h = jmax(4, int(this->getHeight() * nc->getFullVelocity()));
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
