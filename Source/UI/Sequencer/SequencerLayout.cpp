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
#include "PianoRoll.h"
#include "PatternRoll.h"
#include "LassoListeners.h"
#include "ProjectNode.h"
#include "PianoTrackNode.h"
#include "PatternEditorNode.h"
#include "ProjectMapScroller.h"
#include "LevelsMapScroller.h"
#include "PianoProjectMap.h"
#include "VelocityProjectMap.h"
#include "SerializationKeys.h"
#include "SequencerSidebarRight.h"
#include "SequencerSidebarLeft.h"
#include "OrigamiVertical.h"
#include "ShadowUpwards.h"
#include "NoteComponent.h"
#include "ClipComponent.h"
#include "KnifeToolHelper.h"
#include "CutPointMark.h"
#include "MergingEventsConnector.h"
#include "RenderDialog.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "AudioMonitor.h"
#include "Config.h"
#include "ComponentIDs.h"
#include "CommandIDs.h"

//===----------------------------------------------------------------------===//
// Rolls container responsible for switching between piano and pattern roll
//===----------------------------------------------------------------------===//

class RollsSwitchingProxy final : public Component, private MultiTimer
{
public:
    
    enum Timers
    {
        rolls = 0,
        maps = 1,
        scrollerMode = 2
    };

    RollsSwitchingProxy(RollBase *targetRoll1,
        RollBase *targetRoll2,
        Viewport *targetViewport1,
        Viewport *targetViewport2,
        ProjectMapScroller *targetMapScroller,
        LevelsMapScroller *targetLevelsScroller,
        Component *scrollerShadow) :
        pianoRoll(targetRoll1),
        pianoViewport(targetViewport1),
        patternRoll(targetRoll2),
        patternViewport(targetViewport2),
        pianoScroller(targetMapScroller),
        levelsScroller(targetLevelsScroller),
        scrollerShadow(scrollerShadow)
    {
        this->setPaintingIsUnclipped(false);
        this->setInterceptsMouseClicks(false, true);

        this->addAndMakeVisible(this->pianoViewport);
        this->addChildComponent(this->patternViewport); // invisible by default
        this->addChildComponent(this->levelsScroller); // invisible by default, behind piano map
        this->addAndMakeVisible(this->pianoScroller);
        this->addAndMakeVisible(this->scrollerShadow);

        this->patternRoll->setEnabled(false);

        // the volume map's visiblilty is not persistent,
        // but the mini-map's state is, let's fix it right here
        const auto fullMiniMap = App::Config().getUiFlags()->isFullProjectMapVisible();
        this->pianoScroller->setScrollerMode(fullMiniMap ?
            ProjectMapScroller::ScrollerMode::Map : ProjectMapScroller::ScrollerMode::Scroller);
        // the way I'm working with animations here is kinda frustrating,
        // but I'm out of ideas and time, todo refactor that someday
        if (!fullMiniMap)
        {
            this->scrollerModeAnimation.resetToEnd();
        }
    }

    inline bool canAnimate(Timers timer) const noexcept
    {
        switch (timer)
        {
        case Timers::rolls:
            return this->rollsAnimation.canRestart();
        case Timers::maps:
            return this->mapsAnimation.canRestart();
        case Timers::scrollerMode:
            return this->scrollerModeAnimation.canRestart();
        }
        return false;
    }

    // it's not very cool that these two methods rely on
    // animations state to tell what's going on in the UI:
    inline bool isPatternMode() const noexcept
    {
        return this->rollsAnimation.getDirection() > 0.f;
    }

    inline bool isVolumeMapMode() const
    {
        return this->mapsAnimation.getDirection() > 0.f;
    }

    inline bool isFullProjectMapMode() const
    {
        return this->pianoScroller->getScrollerMode() ==
            ProjectMapScroller::ScrollerMode::Map;
    }

    void setAnimationsEnabled(bool animationsEnabled)
    {
        this->animationsTimerInterval = animationsEnabled ? 1000 / 60 : 0;
        this->pianoScroller->setAnimationsEnabled(animationsEnabled);
    }

    bool areAnimationsEnabled() const noexcept
    {
        return this->animationsTimerInterval > 0;
    }

    void startRollSwitchAnimation()
    {
        this->rollsAnimation.start(RollsSwitchingProxy::rollsAnimationStartSpeed);
        const bool patternMode = this->isPatternMode();
        this->pianoScroller->switchToRoll(patternMode ? this->patternRoll : this->pianoRoll);
        // Disabling the inactive controls prevents them from receiving keyboard events:
        this->patternRoll->setEnabled(patternMode);
        this->pianoRoll->setEnabled(!patternMode);
        this->patternRoll->setVisible(true);
        this->pianoRoll->setVisible(true);
        this->patternViewport->setVisible(true);
        this->pianoViewport->setVisible(true);
        if (this->areAnimationsEnabled())
        {
            this->resized();
            this->startTimer(Timers::rolls, this->animationsTimerInterval);
        }
        else
        {
            // here we want to switch the rolls immediately, if animations are disabled,
            // because it's the slowest and the most resource-hungry animation in the app
            this->rollsAnimation.finish();
            this->timerCallback(Timers::rolls);
        }
    }

    void startMapSwitchAnimation()
    {
        this->mapsAnimation.start(RollsSwitchingProxy::mapsAnimationStartSpeed);
        const bool levelsMode = this->isVolumeMapMode();
        // Disabling the inactive controls prevents them from receiving keyboard events:
        this->levelsScroller->setEnabled(levelsMode);
        this->pianoScroller->setEnabled(!levelsMode);
        this->levelsScroller->setVisible(true);
        this->pianoScroller->setVisible(true);
        this->resized();
        this->startTimer(Timers::maps, this->animationsTimerInterval);
    }

    void startScrollerModeSwitchAnimation()
    {
        this->scrollerModeAnimation.start(RollsSwitchingProxy::scrollerModeAnimationStartSpeed);
        if (this->isFullProjectMapMode())
        {
            this->pianoScroller->setScrollerMode(ProjectMapScroller::ScrollerMode::Scroller);
        }
        else
        {
            this->pianoScroller->setScrollerMode(ProjectMapScroller::ScrollerMode::Map);
        }

        this->startTimer(Timers::scrollerMode, this->animationsTimerInterval);
    }

    void resized() override
    {
        this->updateAnimatedRollsBounds();
        this->updateAnimatedMapsBounds();

        if ((this->pianoRoll->getBeatWidth() * this->pianoRoll->getNumBeats()) < this->getWidth())
        {
            this->pianoRoll->setBeatWidth(float(this->getWidth()) / float(this->pianoRoll->getNumBeats()));
        }

        if ((this->patternRoll->getBeatWidth() * this->patternRoll->getNumBeats()) < this->getWidth())
        {
            this->patternRoll->setBeatWidth(float(this->getWidth()) / float(this->patternRoll->getNumBeats()));
        }

        // Force update children bounds, even if they have just moved
        this->pianoRoll->resized();
        this->patternRoll->resized();
    }

private:

    int animationsTimerInterval = 1000 / 60;

    void updateAnimatedRollsBounds()
    {
        const auto scrollerHeight = Globals::UI::projectMapHeight -
            int((Globals::UI::projectMapHeight - Globals::UI::rollScrollerHeight) *
                this->scrollerModeAnimation.getPosition());

        const auto r = this->getLocalBounds();
        const float rollViewportHeight = float(r.getHeight() - scrollerHeight + 1);
        const Rectangle<int> rollSize(r.withBottom(r.getBottom() - scrollerHeight));
        const int viewport1Pos = int(-this->rollsAnimation.getPosition() * rollViewportHeight);
        const int viewport2Pos = int(-this->rollsAnimation.getPosition() * rollViewportHeight + rollViewportHeight);
        this->pianoViewport->setBounds(rollSize.withY(viewport1Pos));
        this->patternViewport->setBounds(rollSize.withY(viewport2Pos));
    }

    void updateAnimatedRollsPositions()
    {
        const auto scrollerHeight = Globals::UI::projectMapHeight -
            int((Globals::UI::projectMapHeight - Globals::UI::rollScrollerHeight) *
                this->scrollerModeAnimation.getPosition());

        const float rollViewportHeight = float(this->getHeight() - scrollerHeight + 1);
        const int viewport1Pos = int(-this->rollsAnimation.getPosition() * rollViewportHeight);
        const int viewport2Pos = int(-this->rollsAnimation.getPosition() * rollViewportHeight + rollViewportHeight);
        this->pianoViewport->setTopLeftPosition(0, viewport1Pos);
        this->patternViewport->setTopLeftPosition(0, viewport2Pos);
    }

    void updateAnimatedMapsBounds()
    {
        const auto projectMapHeight = Globals::UI::projectMapHeight -
            int((Globals::UI::projectMapHeight - Globals::UI::rollScrollerHeight) *
                this->scrollerModeAnimation.getPosition());

        const auto pianoRect = this->getLocalBounds().removeFromBottom(projectMapHeight);
        const auto levelsRect = this->getLocalBounds().removeFromBottom(Globals::UI::levelsMapHeight);
        const auto levelsFullOffset = Globals::UI::levelsMapHeight - projectMapHeight;

        const int pianoMapPos = int(this->mapsAnimation.getPosition() * projectMapHeight);
        const int levelsMapPos = int(this->mapsAnimation.getPosition() * levelsFullOffset);

        this->pianoScroller->setBounds(pianoRect.translated(0, pianoMapPos));
        this->levelsScroller->setBounds(levelsRect.translated(0, levelsFullOffset - levelsMapPos));

        this->scrollerShadow->setBounds(0,
            this->levelsScroller->getY() - RollsSwitchingProxy::scrollerShadowSize,
            this->getWidth(), RollsSwitchingProxy::scrollerShadowSize);
    }

    void updateAnimatedMapsPositions()
    {
        const auto projectMapHeight = Globals::UI::projectMapHeight -
            int((Globals::UI::projectMapHeight - Globals::UI::rollScrollerHeight) *
                this->scrollerModeAnimation.getPosition());

        const auto pianoMapY = this->getHeight() - projectMapHeight;
        const auto levelsFullOffset = Globals::UI::levelsMapHeight - projectMapHeight;

        const int pianoMapPos = int(this->mapsAnimation.getPosition() * projectMapHeight);
        const int levelsMapPos = int(this->mapsAnimation.getPosition() * levelsFullOffset);

        this->pianoScroller->setTopLeftPosition(0, pianoMapY + pianoMapPos);
        this->levelsScroller->setTopLeftPosition(0, pianoMapY - levelsMapPos);

        this->scrollerShadow->setTopLeftPosition(0,
            pianoMapY - levelsMapPos - RollsSwitchingProxy::scrollerShadowSize);
    }

    void timerCallback(int timerId) override
    {
        switch (timerId)
        {
        case Timers::rolls:
            if (this->rollsAnimation.tickAndCheckIfDone())
            {
                this->stopTimer(Timers::rolls);

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

                this->rollsAnimation.finish();
                this->resized();
            }

            this->updateAnimatedRollsPositions();
            break;

        case Timers::maps:

            if (this->mapsAnimation.tickAndCheckIfDone())
            {
                this->stopTimer(Timers::maps);

                if (this->isVolumeMapMode())
                {
                    this->pianoScroller->setVisible(false);
                }
                else
                {
                    this->levelsScroller->setVisible(false);
                }

                this->mapsAnimation.finish();
            }

            this->updateAnimatedMapsPositions();
            break;

        case Timers::scrollerMode:
            if (this->scrollerModeAnimation.tickAndCheckIfDone())
            {
                this->stopTimer(Timers::scrollerMode);
                this->scrollerModeAnimation.finish();
            }

            this->updateAnimatedMapsBounds();
            this->updateAnimatedRollsBounds();
            break;

        default:
            break;
        }
    }

    SafePointer<RollBase> pianoRoll;
    SafePointer<Viewport> pianoViewport;

    SafePointer<RollBase> patternRoll;
    SafePointer<Viewport> patternViewport;

    SafePointer<ProjectMapScroller> pianoScroller;
    SafePointer<LevelsMapScroller> levelsScroller;
    SafePointer<Component> scrollerShadow;

    static constexpr auto scrollerShadowSize = 16;
    static constexpr auto rollsAnimationStartSpeed = 0.4f;
    static constexpr auto mapsAnimationStartSpeed = 0.35f;
    static constexpr auto scrollerModeAnimationStartSpeed = 0.5f;

    class ToggleAnimation final
    {
    public:

        void start(float startSpeed)
        {
            this->direction *= -1.f;
            this->speed = startSpeed;
            this->deceleration = 1.f - this->speed;
        }

        bool tickAndCheckIfDone()
        {
            this->position = this->position + (this->direction * this->speed);
            this->speed *= this->deceleration;
            return this->position < 0.001f ||
                this->position > 0.999f ||
                this->speed < 0.001f;
        }

        void finish()
        {
            // push to either 0 or 1:
            this->position = (jlimit(0.f, 1.f, this->position + this->direction));
        }

        bool canRestart() const
        {
            // only allow restarting animation when most of previous animation is done
            // (feels both responsive enough and not glitch-ish)
            return (this->direction > 0.f && this->position > 0.85f) ||
                (this->direction < 0.f && this->position < 0.15f);
        }

        float getDirection() const noexcept { return direction; }
        float getPosition() const noexcept { return position; }

        void resetToStart() noexcept
        {
            this->position = 0.f;
            this->direction = -1.f;
        }

        void resetToEnd() noexcept
        {
            this->position = 1.f;
            this->direction = 1.f;
        }

    private:

        // 0.f to 1.f, animates the switching between piano and pattern roll
        float position = 0.f;
        float direction = -1.f;
        float speed = 0.f;
        float deceleration = 1.f;
    };

    ToggleAnimation rollsAnimation;
    ToggleAnimation mapsAnimation;
    ToggleAnimation scrollerModeAnimation;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RollsSwitchingProxy)
};

//===----------------------------------------------------------------------===//
// Sequencer Itself
//===----------------------------------------------------------------------===//

SequencerLayout::SequencerLayout(ProjectNode &parentProject) :
    project(parentProject)
{
    this->setComponentID(ComponentIDs::sequencerLayoutId);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);
    this->setOpaque(true);

    // Create viewports, containing the rolls
    const WeakReference<AudioMonitor> clippingDetector =
        App::Workspace().getAudioCore().getMonitor();
    
    this->pianoViewport = make<Viewport>();
    this->pianoViewport->setScrollOnDragEnabled(false);
    this->pianoViewport->setInterceptsMouseClicks(false, true);
    this->pianoViewport->setScrollBarsShown(false, false);
    this->pianoViewport->setWantsKeyboardFocus(false);
    this->pianoViewport->setFocusContainerType(Component::FocusContainerType::none);
    this->pianoViewport->setPaintingIsUnclipped(true);
    
    this->pianoRoll = make<PianoRoll>(this->project,
        *this->pianoViewport, clippingDetector);

    this->patternViewport = make<Viewport>();
    this->patternViewport->setScrollOnDragEnabled(false);
    this->patternViewport->setInterceptsMouseClicks(false, true);
    this->patternViewport->setScrollBarsShown(false, false);
    this->patternViewport->setWantsKeyboardFocus(false);
    this->patternViewport->setFocusContainerType(Component::FocusContainerType::none);
    this->patternViewport->setPaintingIsUnclipped(true);

    this->patternRoll = make<PatternRoll>(this->project,
        *this->patternViewport, clippingDetector);

    this->mapScroller = make<ProjectMapScroller>(this->project.getTransport(), this->pianoRoll.get());
    this->mapScroller->addOwnedMap(new PianoProjectMap(this->project, *this->pianoRoll), false);
    this->mapScroller->addOwnedMap(new AnnotationsProjectMap(this->project,
        *this->pianoRoll, AnnotationsProjectMap::Type::Small), false);
    this->mapScroller->addOwnedMap(new TimeSignaturesProjectMap(this->project,
        *this->pianoRoll, TimeSignaturesProjectMap::Type::Small), false);
    //this->mapScroller->addOwnedMap(new KeySignaturesProjectMap(this->project,
    //    *this->pianoRoll, KeySignaturesProjectMap::Type::Small), false);

    this->levelsScroller = make<LevelsMapScroller>(this->pianoRoll.get());
    this->levelsScroller->addOwnedMap(new VelocityProjectMap(this->project, *this->pianoRoll));

    this->scrollerShadow = make<ShadowUpwards>(ShadowType::Normal);

    constexpr auto defaultBeatWidth = 48.f;

    this->pianoRoll->setBeatWidth(defaultBeatWidth);
    this->pianoViewport->setViewedComponent(this->pianoRoll.get(), false);
    this->pianoRoll->addRollListener(this->mapScroller.get());
    this->pianoRoll->addRollListener(this->levelsScroller.get());

    this->patternRoll->setBeatWidth(defaultBeatWidth);
    this->patternViewport->setViewedComponent(this->patternRoll.get(), false);
    this->patternRoll->addRollListener(this->mapScroller.get());
    
    // create a container with 2 editors and 2 types of project map scroller
    this->rollContainer = make<RollsSwitchingProxy>(this->pianoRoll.get(), this->patternRoll.get(),
        this->pianoViewport.get(), this->patternViewport.get(),
        this->mapScroller.get(), this->levelsScroller.get(),
        this->scrollerShadow.get());
    
    const auto hasAnimations = App::Config().getUiFlags()->areUiAnimationsEnabled();
    this->rollContainer->setAnimationsEnabled(hasAnimations);

    // add sidebars
    this->rollToolsSidebar = make<SequencerSidebarRight>(this->project);
    this->rollToolsSidebar->setSize(Globals::UI::sidebarWidth, this->getParentHeight());

    this->rollNavSidebar = make<SequencerSidebarLeft>();
    this->rollNavSidebar->setSize(Globals::UI::sidebarWidth, this->getParentHeight());
    // Hopefully this doesn't crash, since sequencer layout is only created by a loaded project:
    this->rollNavSidebar->setAudioMonitor(App::Workspace().getAudioCore().getMonitor());

    // combine sidebars with editors
    this->sequencerLayout = make<OrigamiVertical>();
    this->sequencerLayout->addFixedPage(this->rollNavSidebar.get());
    this->sequencerLayout->addFlexiblePage(this->rollContainer.get());
    this->sequencerLayout->addShadowAtTheStart();
    this->sequencerLayout->addShadowAtTheEnd();
    this->sequencerLayout->addFixedPage(this->rollToolsSidebar.get());

    this->addAndMakeVisible(this->sequencerLayout.get());

    App::Config().getUiFlags()->addListener(this);
}

SequencerLayout::~SequencerLayout()
{
    App::Config().getUiFlags()->removeListener(this);

    this->sequencerLayout = nullptr;
    
    this->rollToolsSidebar = nullptr;
    this->rollNavSidebar = nullptr;
    this->rollContainer = nullptr;

    this->patternRoll->removeRollListener(this->mapScroller.get());
    this->pianoRoll->removeRollListener(this->levelsScroller.get());
    this->pianoRoll->removeRollListener(this->mapScroller.get());
    
    this->scrollerShadow = nullptr;
    this->levelsScroller = nullptr;
    this->mapScroller = nullptr;

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

    // Switch to piano map as levels map doesn't make sense in patterns mode
    auto *uiFlags = App::Config().getUiFlags();
    if (uiFlags->isVelocityMapVisible())
    {
        uiFlags->setVelocityMapVisible(false);
    }

    this->rollToolsSidebar->setPatternMode();
    this->rollNavSidebar->setPatternMode();

    // sync pattern roll selection with piano roll edit scope:
    this->patternRoll->selectClip(this->pianoRoll->getActiveClip());
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

    //this->patternRoll->selectClip(this->pianoRoll->getActiveClip());
    this->pianoRoll->deselectAll();

    const Clip &activeClip = this->pianoRoll->getActiveClip();
    const Clip *trackFirstClip = track->getPattern()->getClips().getFirst();
    jassert(trackFirstClip);

    const bool useActiveClip = (activeClip.getPattern() &&
        activeClip.getPattern()->getTrack() == track);

    this->project.setEditableScope(useActiveClip ? activeClip : *trackFirstClip, false);
}

RollBase *SequencerLayout::getRoll() const noexcept
{
    if (this->rollContainer->isPatternMode())
    {
        return this->patternRoll.get();
    }
    else
    {
        return this->pianoRoll.get();
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

void SequencerLayout::proceedToRenderDialog(RenderFormat format)
{
    // this code nearly duplicates RenderDialog::launchFileChooser(),
    // and the reason is that I want to simplify the workflow from user's perspective,
    // so the dialog is shown only after selecting a target file (if any)
    const auto extension = getExtensionForRenderFormat(format);

    const auto defaultFileName = File::createLegalFileName(this->project.getName() + "." + extension);
    auto defaultPath = File::getSpecialLocation(File::userMusicDirectory).getFullPathName();
#if PLATFORM_DESKTOP
    defaultPath = App::Config().getProperty(Serialization::UI::lastRenderPath, defaultPath);
#endif

    this->renderTargetFileChooser = make<FileChooser>(TRANS(I18n::Dialog::renderCaption),
        File(defaultPath).getChildFile(defaultFileName),
        "*." + extension, true);

    DocumentHelpers::showFileChooser(this->renderTargetFileChooser,
        Globals::UI::FileChooser::forFileToSave,
        [this, format](URL &url)
    {
        // todo someday: render to any stream, not only local files
        if (url.isLocalFile())
        {
            App::showModalComponent(make<RenderDialog>(this->project, url, format));
        }
    });
}

void SequencerLayout::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::ImportMidi:
        this->project.getDocument()->import("*.mid;*.midi");
        break;
    case CommandIDs::ExportMidi:
        this->project.getDocument()->exportAs("*.mid;*.midi", this->project.getName() + ".mid");
        break;
    case CommandIDs::RenderToFLAC:
        this->proceedToRenderDialog(RenderFormat::FLAC);
        return;
    case CommandIDs::RenderToWAV:
        this->proceedToRenderDialog(RenderFormat::WAV);
        return;
    case CommandIDs::SwitchBetweenRolls:
        if (!this->rollContainer->canAnimate(RollsSwitchingProxy::Timers::rolls))
        {
            break;
        }

        if (this->rollContainer->isPatternMode())
        {
            if (this->project.getLastShownTrack() == nullptr)
            {
                this->project.selectFirstChildOfType<PianoTrackNode>();
            }
            else
            {
                this->project.getLastShownTrack()->setSelected();
            }
        }
        else
        {
            this->project.selectFirstChildOfType<PatternEditorNode>();
        }
        break;
    default:
        break;
    }
}

//===----------------------------------------------------------------------===//
// UserInterfaceFlags::Listener
//===----------------------------------------------------------------------===//

void SequencerLayout::onVelocityMapVisibilityFlagChanged(bool shoudShow)
{
    const bool alreadyShowing = this->rollContainer->isVolumeMapMode();
    if ((alreadyShowing && shoudShow) || (!alreadyShowing && !shoudShow))
    {
        return;
    }

    this->rollContainer->startMapSwitchAnimation();
}

void SequencerLayout::onProjectMapVisibilityFlagChanged(bool showFullMap)
{
    const bool alreadyShowing = this->rollContainer->isFullProjectMapMode();
    if ((alreadyShowing && showFullMap) || (!alreadyShowing && !showFullMap))
    {
        return;
    }

    this->rollContainer->startScrollerModeSwitchAnimation();
}

void SequencerLayout::onUiAnimationsFlagChanged(bool enabled)
{
    this->rollContainer->setAnimationsEnabled(enabled);
}

//===----------------------------------------------------------------------===//
// UI State Serialization
//===----------------------------------------------------------------------===//

SerializedData SequencerLayout::serialize() const
{
    SerializedData tree(Serialization::UI::sequencer);
    tree.appendChild(this->pianoRoll->serialize());
    tree.appendChild(this->patternRoll->serialize());
    return tree;
}

void SequencerLayout::deserialize(const SerializedData &data)
{
    this->reset();

    const auto root = data.hasType(Serialization::UI::sequencer) ?
        data : data.getChildWithName(Serialization::UI::sequencer);

    if (!root.isValid())
    { return; }
    
    this->pianoRoll->deserialize(root);
    this->patternRoll->deserialize(root);
}

void SequencerLayout::reset()
{
    // no need for this yet
}
