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
#include "UndoStack.h"
#include "MidiTrack.h"
#include "Pattern.h"
#include "PianoSequence.h"
#include "AnnotationEvent.h"
#include "RollBase.h"
#include "MultiTouchController.h"
#include "Lasso.h"
#include "NoteComponent.h"
#include "SequencerOperations.h"
#include "FineTuningValueIndicator.h"
#include "PointReduction.h"
#include "ColourIDs.h"

// this defines if the entire panel is sensitive to up/down dragging,
// or if only the velocity components can be dragged:
// on mobile platforms, editing velocity is inconvenient
// when low velocities are close to the screen's edge;
// on desktop platforms, stick to the classic approach so that
// the UpDownResizeCursor only appears over velocities, not the panel
#if PLATFORM_DESKTOP
#define VELOCITY_DRAGGING_FULL_HEIGHT 0
#elif PLATFORM_MOBILE
#define VELOCITY_DRAGGING_FULL_HEIGHT 1
#endif

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
        this->setAccessible(false);

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
        return { beat, 0.f, beat, float(Globals::UI::editorPanelHeight) };
    }

    inline void updateColour()
    {
        const auto baseColour = findDefaultColour(ColourIDs::Roll::clipForeground);
        this->mainColour = this->note.getTrackColour()
            .interpolatedWith(baseColour, (this->isEditable || this->isHighlighted) ? 0.35f : 0.45f)
            .withMultipliedSaturationHSL(0.95f)
            .withAlpha(this->isEditable ? 0.9f : (this->isHighlighted ? 0.125f : 0.06f));
        this->paleColour = this->mainColour
            .brighter(this->isHighlighted ? 0.25f : 0.1f)
            .withMultipliedAlpha(this->isHighlighted ? 0.25f : 0.1f);
    }

    // the parameters are bounds within the parent panel with its full height
    void syncWithNote(float x, int y, float w, int h) noexcept
    {
        this->dx = x - floorf(x);
        this->dw = w - ceilf(w);

#if VELOCITY_DRAGGING_FULL_HEIGHT

        this->setBounds(int(floorf(x)), y, int(ceilf(w)), h);
        // in case the bounds are the same, but the velocity changed
        this->repaint();

#else

        // at least 4 pixels are visible for 0 volume events:
        const int noteHeight = jmax(4, int(h * this->getFullVelocity()));
        this->setBounds(int(floorf(x)), y + h - noteHeight, int(ceilf(w)), noteHeight);

#endif
    }

    void setEditable(bool shouldBeEditable, bool shouldBeHighlighted = false)
    {
        if (this->isEditable == shouldBeEditable &&
            this->isHighlighted == shouldBeHighlighted)
        {
            return;
        }

        this->isEditable = shouldBeEditable;
        this->isHighlighted = shouldBeHighlighted;

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
#if VELOCITY_DRAGGING_FULL_HEIGHT

        if (this->isDragging)
        {
            g.setColour(this->paleColour.withMultipliedLightness(1.25f).withAlpha(0.025f));
            g.fillRect(this->getLocalBounds());
        }

        const auto h = jmax(4.f, float(this->getHeight()) * this->getFullVelocity());
        const auto y = float(this->getHeight()) - h;

#else

        const auto h = float(this->getHeight());
        const auto y = 0.f;

#endif

        g.setColour(this->mainColour);
        g.fillRect(this->dx, y + 1.f, 1.f, h - 1.f);

        if (this->getWidth() > 2)
        {
            g.fillRect(this->dx + 1.f, y, float(this->getWidth() - 2) + this->dw, 1.f);
            g.fillRect(this->dx, y + 1.f, float(this->getWidth()) + this->dw, 2.f);
        }

        g.setColour(this->paleColour);
        g.fillRect(this->dx, y, float(this->getWidth()) + this->dw, h);
    }

    bool hitTest(int x, int y) noexcept override
    {
        return this->isEditable && Component::hitTest(x, y);
    }

    void mouseDown(const MouseEvent &e) override
    {
        jassert(this->isEditable);
        if (this->getEditor()->isMultiTouchEvent(e) ||
            e.mods.isBackButtonDown() || e.mods.isForwardButtonDown())
        {
            return;
        }

        this->getEditor()->startFineTuning(this, e);

        this->isDragging = true;
        this->repaint();

#if !VELOCITY_DRAGGING_FULL_HEIGHT
        // a hack to allow panning the roll:
        this->getEditor()->mouseDown(e);
#endif
    }

    void mouseDrag(const MouseEvent &e) override
    {
        jassert(this->isEditable);
        if (this->getEditor()->isMultiTouchEvent(e) ||
            e.mods.isBackButtonDown() || e.mods.isForwardButtonDown())
        {
            this->getEditor()->endFineTuning(this, e);

            this->isDragging = false;
            this->repaint();

            return;
        }

        this->getEditor()->continueFineTuning(this, e);

#if !VELOCITY_DRAGGING_FULL_HEIGHT
        // a hack to allow panning the roll:
        this->getEditor()->mouseDrag(e);
#endif
    }

    void mouseUp(const MouseEvent &e) override
    {
        jassert(this->isEditable);
        this->getEditor()->endFineTuning(this, e);

        this->isDragging = false;
        this->repaint();

#if !VELOCITY_DRAGGING_FULL_HEIGHT
        // a hack to allow panning the roll:
        this->getEditor()->mouseUp(e);
#endif
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
    bool isHighlighted = true;
    bool isDragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityEditorNoteComponent)
};

//===----------------------------------------------------------------------===//
// Hand-drawing helper
//===----------------------------------------------------------------------===//

class VelocityHandDrawingHelper final : public Component
{
public:

    VelocityHandDrawingHelper(VelocityEditor &editor) : editor(editor)
    {
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
        this->setAccessible(false);
    }

    const Array<Line<float>> &getCurve() const noexcept
    {
        return this->curveInBeats;
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

        // the epsilon is picked to cut ~50% of all points at any zoom level,
        // which is a small reduction so that the curve shape barely changes;
        // the point of doing it is that it filters out the noise nicely:
        const auto epsilon  = jmax(0.000001f, viewWidth / float(mySize.getX()) * 0.001f);
        this->simplifiedPositions =
            PointReduction<double>::simplify(this->rawPositions, epsilon);
        
        if (this->simplifiedPositions.size() >= 2)
        {
            this->simplifiedPositions[this->simplifiedPositions.size() - 1] = newPosition;
        }
    }

    void updateCurves()
    {
        jassert(!this->simplifiedPositions.isEmpty());

        const auto mySize = this->getSize();

        this->curveInBeats.clearQuick();
        this->curveInPixels.clear();

        auto previousPoint = (this->simplifiedPositions.getFirst() * mySize).toFloat();
        this->curveInPixels.startNewSubPath(previousPoint);

        for (int i = 1; i < this->simplifiedPositions.size(); ++i)
        {
            const auto nextPoint = (this->simplifiedPositions.getUnchecked(i) * mySize).toFloat();
            this->curveInPixels.lineTo(nextPoint);

            this->curveInBeats.add({
                { this->editor.getBeatByXPosition(previousPoint.x), previousPoint.y },
                { this->editor.getBeatByXPosition(nextPoint.x), nextPoint.y }
            });

            previousPoint = nextPoint;
        }

        static Array<float> dashes(4.f, 3.f);
        PathStrokeType(1.f).createDashedStroke(this->curveInPixels, this->curveInPixels,
            dashes.getRawDataPointer(), dashes.size());
    }

private:

    VelocityEditor &editor;

    Array<Point<double>> rawPositions;
    Array<Point<double>> simplifiedPositions; // both in absolute values, 0..1

    Path curveInPixels; // for painting
    Array<Line<float>> curveInBeats; // for convenient intersection checks

    const Colour colour = findDefaultColour(Label::textColourId);

    inline const Point<double> getSize() const noexcept
    {
        return this->getLocalBounds().getBottomRight().toDouble();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityHandDrawingHelper)
};

//===----------------------------------------------------------------------===//
// VelocityEditor
//===----------------------------------------------------------------------===//

#define VELOCITY_MAP_BATCH_REPAINT_START \
    if (this->isEnabled()) { this->setVisible(false); }

#define VELOCITY_MAP_BATCH_REPAINT_END \
    if (this->isEnabled()) { this->setVisible(true); }

VelocityEditor::VelocityEditor(ProjectNode &project, SafePointer<RollBase> roll) :
    project(project),
    roll(roll)
{
    this->setInterceptsMouseClicks(true, true);
    this->setPaintingIsUnclipped(true);
    this->setAccessible(false);

    this->volumeBlendingIndicator = make<FineTuningValueIndicator>(this->volumeBlendingAmount, "");
    this->volumeBlendingIndicator->setShouldDisplayValue(false);
    this->volumeBlendingIndicator->setSize(40, 40);
    this->addChildComponent(this->volumeBlendingIndicator.get());

    this->multiTouchController = make<MultiTouchController>(*this);
    this->addMouseListener(this->multiTouchController.get(), true);

    this->reloadAllTracks();

    this->project.addListener(this);
    this->project.getEditMode().addListener(this);
}

VelocityEditor::~VelocityEditor()
{
    this->project.getEditMode().removeListener(this);
    this->project.removeListener(this);
    this->removeMouseListener(this->multiTouchController.get());
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
            this->updateNoteComponent(e.second.get());
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
    if (this->isMultiTouchEvent(e) ||
        e.mods.isBackButtonDown() || e.mods.isForwardButtonDown())
    {
        return;
    }

    this->panningStart = e.getPosition();

    if (this->isDrawingEvent(e))
    {
        this->volumeBlendingIndicator->toFront(false);
        this->updateVolumeBlendingIndicator(e.getPosition());

        this->handDrawingHelper = make<VelocityHandDrawingHelper>(*this);
        this->addAndMakeVisible(this->handDrawingHelper.get());
        this->handDrawingHelper->setBounds(this->getLocalBounds());
        this->handDrawingHelper->setStartMousePosition(e.position);
    }
    else  if (this->isDraggingEvent(e))
    {
        // roll panning hack
        this->roll->mouseDown(e.getEventRelativeTo(this->roll));
    }
}

void VelocityEditor::mouseDrag(const MouseEvent &e)
{
    if (this->isMultiTouchEvent(e) ||
        e.mods.isBackButtonDown() || e.mods.isForwardButtonDown())
    {
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

    if (this->isDraggingEvent(e))
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->roll->mouseDrag(e
            .withNewPosition(Point<int>(e.getPosition().getX(), this->panningStart.getY()))
            .getEventRelativeTo(this->roll));
    }
}

void VelocityEditor::mouseUp(const MouseEvent &e)
{
    if (this->handDrawingHelper != nullptr)
    {
        this->volumeBlendingIndicator->setVisible(false);
        this->handDrawingHelper = nullptr;
        this->groupDragIntersections.clear();
        this->groupDragChangesBefore.clearQuick();
        this->groupDragChangesAfter.clearQuick();
        this->editingHadChanges = false;
    }

    if (this->isMultiTouchEvent(e) ||
        e.mods.isBackButtonDown() || e.mods.isForwardButtonDown())
    {
        return;
    }

    // skipping this to avoid deselection logic, we only needed panning:
    /*
    if (this->isDraggingEvent(e))
    {
        this->roll->mouseUp(e
            .withNewPosition(Point<int>(e.getPosition().getX(), this->panningStart.getY()))
            .getEventRelativeTo(this->roll));
    }
    */

    this->setMouseCursor(this->getEditMode().getCursor());
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
// EditorPanelBase
//===----------------------------------------------------------------------===//

void VelocityEditor::switchToRoll(SafePointer<RollBase> roll)
{
    this->roll = roll;
}

float VelocityEditor::getBeatByXPosition(float x) const noexcept
{
    jassert(this->getWidth() == this->roll->getWidth());
    return this->roll->getBeatByXPosition(x);
}

void VelocityEditor::setEditableClip(Optional<Clip> clip)
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

void VelocityEditor::setEditableClip(const Clip &selectedClip, const EventFilter &)
{
    this->setEditableClip(selectedClip); // event filters aren't supported here
}

void VelocityEditor::setEditableSelection(WeakReference<Lasso> selection)
{
    this->selection = selection;

    if (!this->activeClip.hasValue())
    {
        return;
    }

    const auto foundActiveMap = this->patternMap.find(*this->activeClip);
    if (foundActiveMap == this->patternMap.end())
    {
        jassertfalse;
        return;
    }

    const auto *activeMap = foundActiveMap->second.get();

    VELOCITY_MAP_BATCH_REPAINT_START

    if (selection == nullptr || selection->getNumSelected() == 0)
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
            it.second->setEditable(false, true);
        }

        for (const auto *component : *selection)
        {
            if (const auto *nc = dynamic_cast<const NoteComponent *>(component))
            {
                auto foundVolumeComponent = activeMap->find(nc->getNote());
                if (foundVolumeComponent != activeMap->end())
                {
                    foundVolumeComponent->second->setEditable(true, true);
                }
#if DEBUG
                else
                {
                    jassertfalse; // wrong activeClip? or maybe trying to select a generated note?
                }
#endif
            }
        }
    }

    VELOCITY_MAP_BATCH_REPAINT_END
}

bool VelocityEditor::canEditSequence(WeakReference<MidiSequence> sequence) const
{
    return dynamic_cast<PianoSequence *>(sequence.get()) != nullptr;
}

Array<VelocityEditor::EventFilter> VelocityEditor::getAllEventFilters() const
{
    return { {0, TRANS(I18n::Defaults::volumePanelName)} };
}

//===----------------------------------------------------------------------===//
// MultiTouchListener
//===----------------------------------------------------------------------===//

// because the roll is the upstream of move-resize events for all bottom panels,
// this will simply ensure that zooming and panning events are horizontal-only
// and pass them to the currently active roll; the same logic can be found in AutomationEditor

void VelocityEditor::multiTouchStartZooming()
{
    this->roll->multiTouchStartZooming();
}

void VelocityEditor::multiTouchContinueZooming(const Rectangle<float> &relativePositions,
    const Rectangle<float> &relativeAnchor, const Rectangle<float> &absoluteAnchor)
{
    this->roll->multiTouchContinueZooming(relativePositions, relativeAnchor, absoluteAnchor);
}

void VelocityEditor::multiTouchEndZooming(const MouseEvent &anchorEvent)
{
    this->panningStart = anchorEvent.getEventRelativeTo(this).getPosition();
    this->roll->multiTouchEndZooming(anchorEvent.getEventRelativeTo(this->roll));
}

Point<float> VelocityEditor::getMultiTouchRelativeAnchor(const MouseEvent &event)
{
    return this->roll->getMultiTouchRelativeAnchor(event
        .withNewPosition(Point<int>(event.getPosition().getX(), this->panningStart.getY()))
        .getEventRelativeTo(this->roll));
}

Point<float> VelocityEditor::getMultiTouchAbsoluteAnchor(const MouseEvent &event)
{
    return this->roll->getMultiTouchAbsoluteAnchor(event
        .withNewPosition(Point<int>(event.getPosition().getX(), this->panningStart.getY()))
        .getEventRelativeTo(this->roll));
}

bool VelocityEditor::isMultiTouchEvent(const MouseEvent &e) const noexcept
{
    return this->roll->isMultiTouchEvent(e) || this->multiTouchController->hasMultiTouch(e);
}

//===----------------------------------------------------------------------===//
// RollEditMode::Listener & edit mode helpers
//===----------------------------------------------------------------------===//

// the velocity editor supports all edit modes, except the selection mode and the knife mode,
// this hack maps all edit modes to supported ones, so that the unsupported modes
// fallback to the default mode (see similar logic in AutomationEditor)

RollEditMode VelocityEditor::getSupportedEditMode(const RollEditMode &rollMode) const noexcept
{
    if (rollMode.isMode(RollEditMode::dragMode))
    {
        return rollMode;
    }

    if (rollMode.isMode(RollEditMode::drawMode) ||
        rollMode.isMode(RollEditMode::eraseMode))
    {
        return RollEditMode::drawMode;
    }

    return RollEditMode::defaultMode;
}

RollEditMode VelocityEditor::getEditMode() const noexcept
{
    return this->getSupportedEditMode(this->project.getEditMode());
}

void VelocityEditor::onChangeEditMode(const RollEditMode &mode)
{
    const auto velocityEditMode = this->getSupportedEditMode(mode);
    const auto areChildrenEnabled = velocityEditMode.isMode(RollEditMode::defaultMode);

    this->setMouseCursor(velocityEditMode.getCursor());

    for (const auto &c : this->patternMap)
    {
        const auto &componentsMap = *c.second.get();
        for (const auto &e : componentsMap)
        {
            e.second->setInterceptsMouseClicks(areChildrenEnabled, false);
        }
    }
}

bool VelocityEditor::isDraggingEvent(const MouseEvent &e) const
{
    if (this->getEditMode().forbidsViewportDragging(e.mods)) { return false; }
    if (this->getEditMode().forcesViewportDragging(e.mods)) { return true; }
    return e.source.isTouch() || e.mods.isRightButtonDown() || e.mods.isMiddleButtonDown();
}

bool VelocityEditor::isDrawingEvent(const MouseEvent &e) const
{
    if (e.mods.isRightButtonDown() || e.mods.isMiddleButtonDown()) { return false; }
    if (this->getEditMode().forbidsAddingEvents(e.mods)) { return false; }
    if (this->getEditMode().forcesAddingEvents(e.mods)) { return true; }
    return false;
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
            const auto *targetClipId = &c.first;
            const int i = track->getPattern()->indexOfSorted(targetClipId);
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
        this->updateNoteComponent(noteComponent);
    }

    VELOCITY_MAP_BATCH_REPAINT_END
}

void VelocityEditor::onChangeClip(const Clip &clip, const Clip &newClip)
{
    if (this->activeClip == clip) // same id
    {
        this->activeClip = newClip; // new parameters
    }

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
    this->reloadAllTracks();
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
// Fine-tuning in the default edit mode
//===----------------------------------------------------------------------===//

static String getVelocityRangeView(WeakReference<Lasso> selection,
    VelocityEditorNoteComponent *draggedNote)
{
    float minFullVelocity = 1.f;
    float maxFullVelocity = 0.f;

    if (selection != nullptr && selection->getNumSelected() > 0)
    {
        for (auto *component : *selection)
        {
            jassert(dynamic_cast<const NoteComponent *>(component));
            if (auto *nc = dynamic_cast<NoteComponent *>(component))
            {
                minFullVelocity = jmin(minFullVelocity, nc->getFullVelocity());
                maxFullVelocity = jmax(maxFullVelocity, nc->getFullVelocity());
            }
        }
    }
    else
    {
        minFullVelocity = maxFullVelocity = draggedNote->getFullVelocity();
    }

    return (minFullVelocity == maxFullVelocity) ?
        String(MidiMessage::floatValueToMidiByte(minFullVelocity)) :
        String(MidiMessage::floatValueToMidiByte(minFullVelocity)) + "-\n" +
        String(MidiMessage::floatValueToMidiByte(maxFullVelocity));
}

void VelocityEditor::startFineTuning(VelocityEditorNoteComponent *target, const MouseEvent &e)
{
    if (e.mods.isLeftButtonDown())
    {
        this->fineTuningAnchor = target->getNote().getVelocity();

        if (this->selection != nullptr &&
            this->selection->getNumSelected() > 0)
        {
            SequencerOperations::startTuning(*this->selection);
        }

        this->fineTuningDragger.startDraggingComponent(target, e,
            this->fineTuningAnchor, 0.f, 1.f, 1.f / 127.f,
            FineTuningComponentDragger::Mode::DragOnlyY);

        this->fineTuningIndicator = make<FineTuningValueIndicator>(0.f, "");
        this->fineTuningIndicator->setValue(target->getNote().getVelocity(),
            getVelocityRangeView(this->selection, target));

        // adding it to grandparent to avoid clipping
        jassert(this->getParentComponent() != nullptr);
        jassert(this->getParentComponent()->getParentComponent() != nullptr);
        auto *grandParent = this->getParentComponent()->getParentComponent();

        grandParent->addAndMakeVisible(this->fineTuningIndicator.get());
        this->fineTuningIndicator->repositionAtTargetTop(target);
        this->fader.fadeIn(this->fineTuningIndicator.get(), Globals::UI::fadeInLong);

        this->editingHadChanges = false;

        // todo send midi?
    }
}

void VelocityEditor::continueFineTuning(VelocityEditorNoteComponent *target, const MouseEvent &e)
{
    if (this->fineTuningIndicator != nullptr)
    {
        this->fineTuningDragger.dragComponent(target, e);

        const auto newVelocity = this->fineTuningDragger.getValue();
        const auto velocityDelta = this->fineTuningAnchor - newVelocity;

        if (velocityDelta == 0.f)
        {
            this->updateNoteComponent(target);
            return;
        }

        if (!this->editingHadChanges)
        {
            this->project.checkpoint();
            this->editingHadChanges = true;
        }

        if (this->selection != nullptr &&
            this->selection->getNumSelected() > 0)
        {
            // for multiplication and sine mode, the factor is computed as:
            // 0 .. fineTuningVelocityAnchor    ->  -1 .. 0
            // fineTuningVelocityAnchor .. 1.0  ->   0 .. 1
            const auto velocityFactor = (newVelocity < this->fineTuningAnchor) ?
                ((newVelocity / this->fineTuningAnchor) - 1.f) :
                ((newVelocity - this->fineTuningAnchor) / (1.f - this->fineTuningAnchor));

            if (e.mods.isAltDown())
            {
                SequencerOperations::changeVolumeMultiplied(*this->selection, velocityFactor);
            }
            else if (e.mods.isShiftDown())
            {
                SequencerOperations::changeVolumeSine(*this->selection, velocityFactor);
            }
            else
            {
                SequencerOperations::changeVolumeLinear(*this->selection, velocityDelta);
            }
        }
        else
        {
            auto *sequence = static_cast<PianoSequence *>(target->getNote().getSequence());
            sequence->change(target->getNote(),
                target->getNote().withVelocity(newVelocity), true);
        }

        jassert(this->fineTuningIndicator != nullptr);
        this->fineTuningIndicator->setValue(newVelocity,
            getVelocityRangeView(this->selection, target));

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

        if (this->selection != nullptr &&
            this->selection->getNumSelected() > 0)
        {
            SequencerOperations::endTuning(*this->selection);
        }

        this->updateNoteComponent(target);

        this->editingHadChanges = false;
    }
}

//===----------------------------------------------------------------------===//
// Hand-drawing shapes in the pen mode
//===----------------------------------------------------------------------===//

void VelocityEditor::updateVolumeBlendingIndicator(const Point<int> &pos)
{
    // todo someday: display a mobile-friendly blending amount control
#if PLATFORM_DESKTOP
    this->volumeBlendingIndicator->setValue(this->volumeBlendingAmount);
    this->volumeBlendingIndicator->setCentrePosition(pos);

    if (this->volumeBlendingAmount == 1.f && this->volumeBlendingIndicator->isVisible())
    {
        this->fader.fadeOut(this->volumeBlendingIndicator.get(), Globals::UI::fadeOutLong);
    }
    else if (this->volumeBlendingAmount < 1.f && !this->volumeBlendingIndicator->isVisible())
    {
        this->fader.fadeIn(this->volumeBlendingIndicator.get(), Globals::UI::fadeInLong);
    }
#endif
}

constexpr float getVelocityByIntersection(const Point<float> &intersection)
{
    return 1.f - (intersection.y / float(Globals::UI::editorPanelHeight));
}

void VelocityEditor::applyGroupVolumeChanges()
{
    if (!this->activeClip.hasValue())
    {
        return; // no editable components
    }

    if (this->editingHadChanges)
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
        if (!this->editingHadChanges)
        {
            this->editingHadChanges = true;
            this->project.checkpoint();
        }

        sequence->changeGroup(this->groupDragChangesBefore, this->groupDragChangesAfter, true);
    }
}

//===----------------------------------------------------------------------===//
// Reloading / resizing / repainting
//===----------------------------------------------------------------------===//

void VelocityEditor::reloadAllTracks()
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
                this->updateNoteComponent(noteComponent);
            }
        }
    }
}

void VelocityEditor::updateNoteComponent(VelocityEditorNoteComponent *nc)
{
    const float rollLengthInBeats = this->rollLastBeat - this->rollFirstBeat;
    const float projectLengthInBeats = this->projectLastBeat - this->projectFirstBeat;

    const float beat = nc->getFullBeat() - this->rollFirstBeat;
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

    const float x = mapWidth * (beat / projectLengthInBeats);
    const float w = mapWidth * (nc->getLength() / projectLengthInBeats);

    nc->syncWithNote(x, 1, jmax(1.f, w), this->getHeight() - 1);
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
                this->updateNoteComponent(static_cast<VelocityEditorNoteComponent *>(component));
            }
        }

        VELOCITY_MAP_BATCH_REPAINT_END

        this->batchRepaintList.clearQuick();
    }
}
