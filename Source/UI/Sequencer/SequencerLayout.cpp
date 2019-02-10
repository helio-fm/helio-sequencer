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
#include "SequencerLayout.h"
#include "AutomationSequence.h"
#include "PianoRoll.h"
#include "PatternRoll.h"
#include "LassoListeners.h"
#include "ProjectNode.h"
#include "PianoTrackNode.h"
#include "PatternEditorNode.h"
#include "TrackScroller.h"
#include "PianoProjectMap.h"
#include "SerializationKeys.h"
#include "AnnotationSmallComponent.h"
#include "TimeSignatureSmallComponent.h"
#include "KeySignatureSmallComponent.h"
#include "SequencerSidebarRight.h"
#include "SequencerSidebarLeft.h"
#include "OrigamiHorizontal.h"
#include "OrigamiVertical.h"
#include "NoteComponent.h"
#include "ClipComponent.h"
#include "KnifeToolHelper.h"
#include "CutPointMark.h"
#include "RenderDialog.h"
#include "SuccessTooltip.h"
#include "Workspace.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "AudioMonitor.h"
#include "ComponentIDs.h"
#include "ColourIDs.h"

// force compile template
#include "AnnotationsProjectMap.cpp"
template class AnnotationsProjectMap<AnnotationSmallComponent>;

// force compile template
#include "TimeSignaturesProjectMap.cpp"
template class TimeSignaturesProjectMap<TimeSignatureSmallComponent>;

// force compile template
#include "KeySignaturesProjectMap.cpp"
template class KeySignaturesProjectMap<KeySignatureSmallComponent>;

#define MAX_NUM_SPLITSCREEN_EDITORS 2
#define MINIMUM_ROLLS_HEIGHT 250
#define VERTICAL_ROLLS_LAYOUT 1
#define ROLLS_ANIMATION_START_SPEED 0.3f

//===----------------------------------------------------------------------===//
// Rolls container responsible for switching between piano and pattern roll
//===----------------------------------------------------------------------===//

class RollsSwitchingProxy : public Component, private Timer
{
public:
    
    RollsSwitchingProxy(HybridRoll *targetRoll1,
        HybridRoll *targetRoll2,
        Viewport *targetViewport1,
        Viewport *targetViewport2,
        TrackScroller *targetScroller) :
        pianoRoll(targetRoll1),
        pianoViewport(targetViewport1),
        patternRoll(targetRoll2),
        patternViewport(targetViewport2),
        scroller(targetScroller),
        animationPosition(0.f),
        animationDirection(-1.f),
        animationSpeed(0.f),
        animationDeceleration(1.f)
    {
        this->setInterceptsMouseClicks(false, true);
        this->setPaintingIsUnclipped(false);

        this->addAndMakeVisible(this->pianoViewport);
        this->addAndMakeVisible(this->patternViewport);
        this->addAndMakeVisible(this->scroller);

        // Default state
        this->patternRoll->setEnabled(false);
        this->patternViewport->setVisible(false);
    }

    inline bool isPatternMode() const noexcept
    {
        return (this->animationDirection > 0.f);
    }

    void startRollSwitchAnimation()
    {
        this->animationDirection *= -1.f;
        this->animationSpeed = ROLLS_ANIMATION_START_SPEED;
        this->animationDeceleration = 1.f - this->animationSpeed;
        const bool patternMode = this->isPatternMode();
        this->scroller->switchToRoll(patternMode ? this->patternRoll : this->pianoRoll);
        // Disabling inactive prevents it from receiving keyboard events:
        this->patternRoll->setEnabled(patternMode);
        this->pianoRoll->setEnabled(!patternMode);
        this->patternRoll->setVisible(true);
        this->pianoRoll->setVisible(true);
        this->patternViewport->setVisible(true);
        this->pianoViewport->setVisible(true);
        this->resized();
        this->startTimerHz(60);
    }

    void resized() override
    {
        jassert(this->pianoRoll);
        jassert(this->pianoViewport);
        jassert(this->patternRoll);
        jassert(this->patternViewport);
        jassert(this->scroller);

        this->updateAnimatedBounds();

        Rectangle<int> r(this->getLocalBounds());
        const int scrollerHeight = MainLayout::getScrollerHeight();
        this->scroller->setBounds(r.removeFromBottom(scrollerHeight));

        if ((this->pianoRoll->getBarWidth() * this->pianoRoll->getNumBars()) < this->getWidth())
        {
            this->pianoRoll->setBarWidth(float(this->getWidth()) / float(this->pianoRoll->getNumBars()));
        }

        if ((this->patternRoll->getBarWidth() * this->patternRoll->getNumBars()) < this->getWidth())
        {
            this->patternRoll->setBarWidth(float(this->getWidth()) / float(this->patternRoll->getNumBars()));
        }

        // Force update children bounds, even if they have just moved
        this->pianoRoll->resized();
        this->patternRoll->resized();
    }

    void updateAnimatedBounds()
    {
        const Rectangle<int> r(this->getLocalBounds());
        const int scrollerHeight = MainLayout::getScrollerHeight();

#if VERTICAL_ROLLS_LAYOUT
        const float rollViewportHeight = float(r.getHeight() - scrollerHeight + 1);
        const Rectangle<int> rollSize(r.withBottom(r.getBottom() - scrollerHeight));
        const int viewport1Pos = int(-this->animationPosition * rollViewportHeight);
        const int viewport2Pos = int(-this->animationPosition * rollViewportHeight + rollViewportHeight);
        this->pianoViewport->setBounds(rollSize.withY(viewport1Pos));
        this->patternViewport->setBounds(rollSize.withY(viewport2Pos));
#else
        const float rollViewportWidth = float(r.getWidth());
        const Rectangle<int> rollSize(r.withBottom(r.getBottom() - scrollerHeight));
        const int viewport1Pos = int(this->animationPosition * rollViewportWidth);
        const int viewport2Pos = int(this->animationPosition * rollViewportWidth + rollViewportWidth);
        this->pianoViewport->setBounds(rollSize.withX(viewport1Pos));
        this->patternViewport->setBounds(rollSize.withX(viewport2Pos));
#endif
    }

    void updateAnimatedPositions()
    {
        const Rectangle<int> r(this->getLocalBounds());
        const int scrollerHeight = MainLayout::getScrollerHeight();

#if VERTICAL_ROLLS_LAYOUT
        const float rollViewportHeight = float(r.getHeight() - scrollerHeight + 1);
        const int viewport1Pos = int(-this->animationPosition * rollViewportHeight);
        const int viewport2Pos = int(-this->animationPosition * rollViewportHeight + rollViewportHeight);
        this->pianoViewport->setTopLeftPosition(0, viewport1Pos);
        this->patternViewport->setTopLeftPosition(0, viewport2Pos);
#else
        const float rollViewportWidth = float(r.getWidth());
        const int viewport1Pos = int(this->animationPosition * rollViewportWidth);
        const int viewport2Pos = int(this->animationPosition * rollViewportWidth + rollViewportWidth);
        this->pianoViewport->setTopLeftPosition(viewport1Pos, 0);
        this->patternViewport->setTopLeftPosition(viewport2Pos, 0);
#endif
    }

private:

    void timerCallback() override
    {
        this->animationPosition += this->animationDirection * this->animationSpeed;
        this->animationSpeed *= this->animationDeceleration;

        if (this->animationPosition < 0.001f ||
            this->animationPosition > 0.999f ||
            this->animationSpeed < 0.001f)
        {
            this->stopTimer();

            if (this->isPatternMode())
            { 
                this->pianoRoll->setVisible(false);
                this->pianoViewport->setVisible(false);
            }
            else
            {
                this->patternRoll->setVisible(false);
                this->patternViewport->setVisible(false);
            }

            // Push to either 0 or 1:
            this->animationPosition = jlimit(0.f, 1.f,
                this->animationPosition + this->animationDirection);

            this->resized();
        }
        else
        {
            this->updateAnimatedPositions();
        }
    }

    SafePointer<HybridRoll> pianoRoll;
    SafePointer<Viewport> pianoViewport;

    SafePointer<HybridRoll> patternRoll;
    SafePointer<Viewport> patternViewport;

    SafePointer<TrackScroller> scroller;

    // 0.f to 1.f, animates the switching between piano and pattern roll
    float animationPosition;
    float animationDirection;
    float animationSpeed;
    float animationDeceleration;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RollsSwitchingProxy)
};

//===----------------------------------------------------------------------===//
// Sequencer Itself
//===----------------------------------------------------------------------===//

SequencerLayout::SequencerLayout(ProjectNode &parentProject) :
    project(parentProject),
    pianoRoll(nullptr)
{
    this->setComponentID(ComponentIDs::sequencerLayoutId);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);
    this->setOpaque(true);

    // Create viewports, containing the rolls
    const WeakReference<AudioMonitor> clippingDetector =
        App::Workspace().getAudioCore().getMonitor();
    
    this->pianoViewport = new Viewport("Viewport One");
    this->pianoViewport->setScrollOnDragEnabled(false);
    this->pianoViewport->setInterceptsMouseClicks(false, true);
    this->pianoViewport->setScrollBarsShown(false, false);
    this->pianoViewport->setWantsKeyboardFocus(false);
    this->pianoViewport->setFocusContainer(false);
    this->pianoViewport->setPaintingIsUnclipped(true);
    
    this->pianoRoll = new PianoRoll(this->project,
        *this->pianoViewport, clippingDetector);

    this->patternViewport = new Viewport("Viewport Two");
    this->patternViewport->setScrollOnDragEnabled(false);
    this->patternViewport->setInterceptsMouseClicks(false, true);
    this->patternViewport->setScrollBarsShown(false, false);
    this->patternViewport->setWantsKeyboardFocus(false);
    this->patternViewport->setFocusContainer(false);
    this->patternViewport->setPaintingIsUnclipped(true);

    this->patternRoll = new PatternRoll(this->project,
        *this->patternViewport, clippingDetector);

    this->scroller = new TrackScroller(this->project.getTransport(), this->pianoRoll);
    this->scroller->addOwnedMap(new PianoProjectMap(this->project, *this->pianoRoll), false);
    this->scroller->addOwnedMap(new AnnotationsProjectMap<AnnotationSmallComponent>(this->project, *this->pianoRoll), false);
    this->scroller->addOwnedMap(new TimeSignaturesProjectMap<TimeSignatureSmallComponent>(this->project, *this->pianoRoll), false);
    //this->scroller->addOwnedMap(new KeySignaturesProjectMap<KeySignatureSmallComponent>(this->project, *this->pianoRoll), false);
    //this->scroller->addOwnedMap(new AutomationTrackMap(this->project, *this->roll, this->project.getDefaultTempoTrack()->getLayer()), true);

    this->pianoRoll->setBarWidth(HYBRID_ROLL_MAX_BAR_WIDTH);
    this->pianoViewport->setViewedComponent(this->pianoRoll, false);
    this->pianoRoll->addRollListener(this->scroller);

    this->patternRoll->setBarWidth(HYBRID_ROLL_MAX_BAR_WIDTH);
    this->patternViewport->setViewedComponent(this->patternRoll, false);
    this->patternRoll->addRollListener(this->scroller);

    // hard-code default y view position
    const int defaultY = (this->pianoRoll->getHeight() / 3);
    this->pianoViewport->setViewPosition(this->pianoViewport->getViewPositionX(), defaultY);
    
    // create a container with 2 editors
    this->rollContainer = new RollsSwitchingProxy(this->pianoRoll, this->patternRoll,
        this->pianoViewport, this->patternViewport,
        this->scroller);
    
    // add sidebars
    this->rollToolsSidebar = new SequencerSidebarRight(this->project);
    this->rollNavSidebar = new SequencerSidebarLeft(this->project);
    this->rollNavSidebar->setSize(SEQUENCER_SIDEBAR_WIDTH, this->getParentHeight());
    // Hopefully this doesn't crash, since sequencer layout is only created by a loaded project:
    this->rollNavSidebar->setAudioMonitor(App::Workspace().getAudioCore().getMonitor());

    // combine sidebars with editors
    this->sequencerLayout = new OrigamiVertical();
    this->sequencerLayout->addFixedPage(this->rollNavSidebar);
    this->sequencerLayout->addFlexiblePage(this->rollContainer);
    this->sequencerLayout->addShadowAtTheStart();
    this->sequencerLayout->addShadowAtTheEnd();
    this->sequencerLayout->addFixedPage(this->rollToolsSidebar);

    this->addAndMakeVisible(this->sequencerLayout);
}

SequencerLayout::~SequencerLayout()
{
    this->sequencerLayout = nullptr;
    
    this->rollToolsSidebar = nullptr;
    this->rollNavSidebar = nullptr;
    this->rollContainer = nullptr;

    this->patternRoll->removeRollListener(this->scroller);
    this->pianoRoll->removeRollListener(this->scroller);
    
    this->scroller = nullptr;

    this->patternRoll = nullptr;
    this->patternViewport = nullptr;

    this->pianoRoll = nullptr;
    this->pianoViewport = nullptr;
}

void SequencerLayout::showPatternEditor()
{
    if (! this->rollContainer->isPatternMode())
    {
        this->rollContainer->startRollSwitchAnimation();
    }

    this->rollToolsSidebar->setPatternMode();
    this->rollNavSidebar->setPatternMode();
    this->patternRoll->deselectAll();
    this->pianoRoll->deselectAll();
}

void SequencerLayout::showLinearEditor(WeakReference<MidiTrack> track)
{
    if (this->rollContainer->isPatternMode())
    {
        this->rollContainer->startRollSwitchAnimation();
    }

    this->rollToolsSidebar->setLinearMode();
    this->rollNavSidebar->setLinearMode();
    this->patternRoll->deselectAll();
    this->pianoRoll->deselectAll();

    const Clip &activeClip = this->pianoRoll->getActiveClip();
    const Clip *trackFirstClip = track->getPattern()->getClips().getFirst();
    jassert(trackFirstClip);

    const bool useActiveClip = (activeClip.getPattern() &&
        activeClip.getPattern()->getTrack() == track);

    this->pianoRoll->setEditableScope(track,
        useActiveClip ? activeClip : *trackFirstClip, false);
}

void SequencerLayout::setEditableScope(WeakReference<MidiTrack> track, const Clip &clip, bool zoomToArea)
{
    this->pianoRoll->setEditableScope(track, clip, zoomToArea);
}

HybridRoll *SequencerLayout::getRoll() const
{
    if (this->rollContainer->isPatternMode())
    { return this->patternRoll; }
    else
    { return this->pianoRoll; }
}

//===----------------------------------------------------------------------===//
// FileDragAndDropTarget
//===----------------------------------------------------------------------===//

bool SequencerLayout::isInterestedInFileDrag(const StringArray &files)
{
    File file = File(files.joinIntoString({}, 0, 1));
    return (file.hasFileExtension("mid") || file.hasFileExtension("midi"));
}

void SequencerLayout::filesDropped(const StringArray &filenames,
    int mouseX, int mouseY)
{
    if (isInterestedInFileDrag(filenames))
    {
        String filename = filenames.joinIntoString({}, 0, 1);
        DBG(filename);
        //importMidiFile(File(filename));
    }
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void SequencerLayout::resized()
{
    this->sequencerLayout->setBounds(this->getLocalBounds());

    // a hack for themes changing
    this->rollToolsSidebar->resized();
}


void SequencerLayout::proceedToRenderDialog(const String &extension)
{
    const File initialPath = File::getSpecialLocation(File::userMusicDirectory);
    const String renderFileName = this->project.getName() + "." + extension.toLowerCase();
    const String safeRenderName = File::createLegalFileName(renderFileName);

#if HELIO_DESKTOP
    FileChooser fc(TRANS("dialog::render::caption"),
        File(initialPath.getChildFile(safeRenderName)), ("*." + extension), true);

    if (fc.browseForFileToSave(true))
    {
        App::Layout().showModalComponentUnowned(new RenderDialog(this->project, fc.getResult(), extension));
    }
#else
    App::Layout().showModalComponentUnowned(new RenderDialog(this->project, initialPath.getChildFile(safeRenderName), extension));
#endif
    }

void SequencerLayout::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::ImportMidi:
        this->project.getDocument()->import("*.mid;*.midi");
        break;
    case CommandIDs::ExportMidi:
#if JUCE_IOS
        {
            const String safeName = TreeNode::createSafeName(this->project.getName()) + ".mid";
            File midiExport = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(safeName);
            this->project.exportMidi(midiExport);
            App::Layout().showTooltip(TRANS("menu::project::render::savedto") + " '" + safeName + "'");
            App::Layout().showModalComponentUnowned(new SuccessTooltip());
        }
#else
        this->project.getDocument()->exportAs("*.mid;*.midi", this->project.getName() + ".mid");
#endif
        break;
    case CommandIDs::RenderToFLAC:
        this->proceedToRenderDialog("FLAC");
        return;

    case CommandIDs::RenderToWAV:
        this->proceedToRenderDialog("WAV");
        return;
    case CommandIDs::SwitchBetweenRolls:
        if (this->rollContainer->isPatternMode())
        {
            if (this->project.getLastShownTrack() == nullptr)
            {
                this->project.selectChildOfType<PianoTrackNode>();
            }
            else
            {
                this->project.getLastShownTrack()->setSelected();
            }
        }
        else
        {
            this->project.selectChildOfType<PatternEditorNode>();
        }
        break;
    default:
        break;
    }
}

//===----------------------------------------------------------------------===//
// UI State Serialization
//===----------------------------------------------------------------------===//

ValueTree SequencerLayout::serialize() const
{
    ValueTree tree(Serialization::UI::sequencer);
    tree.appendChild(this->pianoRoll->serialize(), nullptr);
    tree.appendChild(this->patternRoll->serialize(), nullptr);
    return tree;
}

void SequencerLayout::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root = tree.hasType(Serialization::UI::sequencer) ?
        tree : tree.getChildWithName(Serialization::UI::sequencer);

    if (!root.isValid())
    { return; }
    
    this->pianoRoll->deserialize(root);
    this->patternRoll->deserialize(root);
}

void SequencerLayout::reset()
{
    // no need for this yet
}
