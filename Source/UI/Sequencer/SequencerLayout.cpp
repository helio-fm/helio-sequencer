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
#include "SequencerLayout.h"
#include "PianoRoll.h"
#include "PatternRoll.h"
#include "LassoListeners.h"
#include "ProjectNode.h"
#include "PianoTrackNode.h"
#include "PatternEditorNode.h"
#include "PianoProjectMap.h"
#include "ProjectMapsScroller.h"
#include "EditorPanelsScroller.h"
#include "EditorPanelsSwitcher.h"
#include "VelocityEditor.h"
#include "AutomationEditor.h"
#include "SequencerSidebarRight.h"
#include "SequencerSidebarLeft.h"
#include "ShadowUpwards.h"
#include "ShadowRightwards.h"
#include "ShadowLeftwards.h"
#include "NoteComponent.h"
#include "ClipComponent.h"
#include "KnifeToolHelper.h"
#include "CutPointMark.h"
#include "MergingEventsConnector.h"
#include "RenderDialog.h"
#include "DocumentHelpers.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "AudioMonitor.h"
#include "Config.h"
#include "SerializationKeys.h"
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
        ProjectMapsScroller *bottomMapsScroller,
        EditorPanelsScroller *bottomEditorsScroller,
        EditorPanelsSwitcher *bottomEditorsSwitcher,
        Component *scrollerShadow) :
        pianoRoll(targetRoll1),
        pianoViewport(targetViewport1),
        patternRoll(targetRoll2),
        patternViewport(targetViewport2),
        bottomMapsScroller(bottomMapsScroller),
        bottomEditorsScroller(bottomEditorsScroller),
        bottomEditorsSwitcher(bottomEditorsSwitcher),
        scrollerShadow(scrollerShadow)
    {
        this->setFocusContainerType(Component::FocusContainerType::none);
        this->setWantsKeyboardFocus(false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setPaintingIsUnclipped(false);
        this->setInterceptsMouseClicks(false, true);

        this->addAndMakeVisible(this->pianoViewport);
        this->addChildComponent(this->patternViewport); // invisible by default
        this->addAndMakeVisible(this->scrollerShadow);
        this->addChildComponent(this->bottomEditorsSwitcher); // invisible by default, behind the scroller
        this->addChildComponent(this->bottomEditorsScroller); // invisible by default, behind the piano map
        this->addAndMakeVisible(this->bottomMapsScroller);

        this->patternRoll->setEnabled(false);
        this->patternRoll->setVisible(false);

        const auto shouldShowFullMiniMap = App::Config().getUiFlags()->isProjectMapInLargeMode();
        this->bottomMapsScroller->setScrollerMode(shouldShowFullMiniMap ?
            ProjectMapsScroller::ScrollerMode::Map : ProjectMapsScroller::ScrollerMode::Scroller);

        // the way I'm working with animations here is kinda frustrating,
        // but I'm out of ideas and time, todo refactor that someday
        if (!shouldShowFullMiniMap)
        {
            this->scrollerModeAnimation.resetToEnd();
        }

        if (App::Config().getUiFlags()->isEditorPanelVisible())
        {
            this->mapsAnimation.resetToEnd();
            this->bottomMapsScroller->setVisible(false);
            this->bottomEditorsScroller->setVisible(true);
            this->bottomEditorsSwitcher->setVisible(true);
        }

        this->setSize(256, 256); // not 0
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

    inline bool isPianoRollMode() const noexcept
    {
        return this->rollsAnimation.isInDefaultState();
    }

    inline bool isPatternRollMode() const noexcept
    {
        return !this->isPianoRollMode();
    }

    inline bool isProjectMapVisible() const
    {
        return this->mapsAnimation.isInDefaultState();
    }

    inline bool isEditorPanelVisible() const
    {
        return !this->isProjectMapVisible();
    }

    inline bool isFullProjectMapMode() const
    {
        return this->bottomMapsScroller->getScrollerMode() ==
            ProjectMapsScroller::ScrollerMode::Map;
    }

    void setAnimationsEnabled(bool shouldBeEnabled)
    {
        this->animationsTimerInterval = shouldBeEnabled ? 1000 / 60 : 0;
        this->bottomMapsScroller->setAnimationsEnabled(shouldBeEnabled);
        this->bottomEditorsScroller->setAnimationsEnabled(shouldBeEnabled);
    }

    bool areAnimationsEnabled() const noexcept
    {
        return this->animationsTimerInterval > 0;
    }

    void startRollSwitchAnimation()
    {
        this->rollsAnimation.start(RollsSwitchingProxy::rollsAnimationStartSpeed);

        const bool patternRollMode = this->isPatternRollMode();
        this->bottomMapsScroller->switchToRoll(patternRollMode ? this->patternRoll : this->pianoRoll);
        this->bottomEditorsScroller->switchToRoll(patternRollMode ? this->patternRoll : this->pianoRoll);

        // Disabling the rolls prevents them from receiving keyboard events:
        this->patternRoll->setEnabled(patternRollMode);
        this->pianoRoll->setEnabled(!patternRollMode);
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
            this->rollsAnimation.finish();
            this->timerCallback(Timers::rolls);
        }
    }

    void startMapSwitchAnimation()
    {
        this->mapsAnimation.start(RollsSwitchingProxy::mapsAnimationStartSpeed);

        // Disabling the panels prevents them from receiving keyboard events:
        const bool editorPanelMode = this->isEditorPanelVisible();
        this->bottomEditorsScroller->setEnabled(editorPanelMode);
        this->bottomEditorsSwitcher->setEnabled(editorPanelMode);
        this->bottomMapsScroller->setEnabled(!editorPanelMode);
        this->bottomEditorsScroller->setVisible(true);
        this->bottomEditorsSwitcher->setVisible(true);
        this->bottomMapsScroller->setVisible(true);

        this->resized();
        this->startTimer(Timers::maps, this->animationsTimerInterval);
    }

    void startScrollerModeSwitchAnimation()
    {
        this->scrollerModeAnimation.start(RollsSwitchingProxy::scrollerModeAnimationStartSpeed);
        if (this->isFullProjectMapMode())
        {
            this->bottomMapsScroller->setScrollerMode(ProjectMapsScroller::ScrollerMode::Scroller);
        }
        else
        {
            this->bottomMapsScroller->setScrollerMode(ProjectMapsScroller::ScrollerMode::Map);
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

        const auto editorPanelHeight =
            int(Globals::UI::editorPanelHeight * this->mapsAnimation.getPosition());

        const auto maxBottomPanelHeight = jmax(scrollerHeight, editorPanelHeight);

        const auto r = this->getLocalBounds();
        const int rollViewportHeight = r.getHeight() - maxBottomPanelHeight + 1;
        const Rectangle<int> rollSize(r.withBottom(r.getBottom() - maxBottomPanelHeight));
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

        const auto editorPanelHeight =
            int(Globals::UI::editorPanelHeight * this->mapsAnimation.getPosition());

        const auto maxBottomPanelHeight = jmax(scrollerHeight, editorPanelHeight);

        const int rollViewportHeight = this->getHeight() - maxBottomPanelHeight + 1;
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

        const auto mapsBounds = this->getLocalBounds().removeFromBottom(projectMapHeight);
        const auto panelsBounds = this->getLocalBounds().removeFromBottom(Globals::UI::editorPanelHeight);
        const auto switcherBounds = panelsBounds
            .translated(0, -EditorPanelsSwitcher::switcherHeight)
            .withHeight(EditorPanelsSwitcher::switcherHeight);

        const auto panelsToMapsOffset = Globals::UI::editorPanelHeight - projectMapHeight;
        const auto switcherToMapsOffset = panelsToMapsOffset + EditorPanelsSwitcher::switcherHeight;

        const int mapsPosition = roundToInt(this->mapsAnimation.getPosition() * projectMapHeight);
        const int panelsPosition = roundToInt(this->mapsAnimation.getPosition() * panelsToMapsOffset);
        const int switcherPosition = roundToInt(this->mapsAnimation.getPosition() * switcherToMapsOffset);

        this->bottomMapsScroller->setBounds(mapsBounds.translated(0, mapsPosition));
        this->bottomEditorsScroller->setBounds(panelsBounds.translated(0, panelsToMapsOffset - panelsPosition));

        const auto switcherHidingOffset = roundToInt(this->rollsAnimation.getPosition() * this->bottomEditorsSwitcher->getHeight());
        this->bottomEditorsSwitcher->setBounds(switcherBounds.translated(0, switcherToMapsOffset - switcherPosition + switcherHidingOffset));

        this->scrollerShadow->setBounds(this->bottomEditorsScroller->getBounds()
            .translated(0, -RollsSwitchingProxy::scrollerShadowSize)
            .withHeight(RollsSwitchingProxy::scrollerShadowSize));
    }

    void updateAnimatedMapsPositions()
    {
        const auto projectMapHeight = Globals::UI::projectMapHeight -
            int((Globals::UI::projectMapHeight - Globals::UI::rollScrollerHeight) *
                this->scrollerModeAnimation.getPosition());

        const auto mapsY = this->getHeight() - projectMapHeight;
        const auto panelsToMapsOffset = Globals::UI::editorPanelHeight - projectMapHeight;
        const auto switcherToMapsOffset = panelsToMapsOffset + EditorPanelsSwitcher::switcherHeight;

        const int mapsPosition = roundToInt(this->mapsAnimation.getPosition() * projectMapHeight);
        const int panelsPosition = roundToInt(this->mapsAnimation.getPosition() * panelsToMapsOffset);
        const int switcherPosition = roundToInt(this->mapsAnimation.getPosition() * switcherToMapsOffset);

        this->bottomMapsScroller->setTopLeftPosition(0, mapsY + mapsPosition);
        this->bottomEditorsScroller->setTopLeftPosition(0, mapsY - panelsPosition);

        const auto switcherHidingOffset = roundToInt(this->rollsAnimation.getPosition() * this->bottomEditorsSwitcher->getHeight());
        this->bottomEditorsSwitcher->setTopLeftPosition(0, mapsY - switcherPosition + switcherHidingOffset);

        this->scrollerShadow->setTopLeftPosition(0,
            this->bottomEditorsScroller->getY() - this->scrollerShadow->getHeight());
    }

    void timerCallback(int timerId) override
    {
        switch (timerId)
        {
        case Timers::rolls:
            if (this->rollsAnimation.tickAndCheckIfDone())
            {
                this->stopTimer(Timers::rolls);

                if (this->isPatternRollMode())
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

            this->updateAnimatedMapsPositions();
            this->updateAnimatedRollsPositions();
            break;

        case Timers::maps:

            if (this->mapsAnimation.tickAndCheckIfDone())
            {
                this->stopTimer(Timers::maps);

                if (this->isEditorPanelVisible())
                {
                    this->bottomMapsScroller->setVisible(false);
                }
                else
                {
                    this->bottomEditorsScroller->setVisible(false);
                    this->bottomEditorsSwitcher->setVisible(false);
                }

                this->mapsAnimation.finish();
            }

            this->updateAnimatedMapsPositions();
            this->updateAnimatedRollsBounds();
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

    SafePointer<ProjectMapsScroller> bottomMapsScroller;
    SafePointer<EditorPanelsScroller> bottomEditorsScroller;
    SafePointer<EditorPanelsSwitcher> bottomEditorsSwitcher;
    SafePointer<Component> scrollerShadow;

    static constexpr auto scrollerShadowSize = 12;
    static constexpr auto rollsAnimationStartSpeed = 0.4f;
    static constexpr auto mapsAnimationStartSpeed = 0.35f;
    static constexpr auto scrollerModeAnimationStartSpeed = 0.5f;

    class ToggleAnimation final
    {
    public:

        void start(float startSpeed)
        {
            this->direction *= -1.0;
            this->speed = startSpeed;
            this->deceleration = 1.0 - this->speed;
        }

        bool tickAndCheckIfDone()
        {
            this->position = this->position + (this->direction * this->speed);
            this->speed *= this->deceleration;
            return this->position < 0.0005 ||
                this->position > 0.9995 ||
                this->speed < 0.0005;
        }

        void finish()
        {
            // push to either 0 or 1:
            this->position = jlimit(0.0, 1.0, this->position + this->direction);
        }

        bool canRestart() const
        {
            // only allow restarting the animation when the previous animation
            // is close to be done so it doesn't feel glitchy but still responsive
            return (this->direction > 0.0 && this->position > 0.85) ||
                (this->direction < 0.0 && this->position < 0.15);
        }

        double isInDefaultState() const noexcept { return this->direction < 0.0; }
        double getPosition() const noexcept { return this->position; }

        void resetToStart() noexcept
        {
            this->position = 0.0;
            this->direction = -1.0;
        }

        void resetToEnd() noexcept
        {
            this->position = 1.f;
            this->direction = 1.f;
        }

    private:

        // 0 to 1, animates the switching between piano and pattern roll
        double position = 0.0;
        double direction = -1.0;
        double speed = 0.0;
        double deceleration = 1.0;
    };

    ToggleAnimation rollsAnimation;
    ToggleAnimation mapsAnimation;
    ToggleAnimation scrollerModeAnimation;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RollsSwitchingProxy)
};

//===----------------------------------------------------------------------===//
// SequencerLayout
//===----------------------------------------------------------------------===//

SequencerLayout::SequencerLayout(ProjectNode &parentProject) :
    project(parentProject)
{
    this->setComponentID(ComponentIDs::sequencerLayoutId);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);
    this->setOpaque(true);

    // make both rolls

    const WeakReference<AudioMonitor> clippingDetector =
        App::Workspace().getAudioCore().getMonitor();

    this->pianoViewport = make<Viewport>();
    this->pianoViewport->setScrollOnDragMode(Viewport::ScrollOnDragMode::never);
    this->pianoViewport->setInterceptsMouseClicks(false, true);
    this->pianoViewport->setScrollBarsShown(false, false);
    this->pianoViewport->setWantsKeyboardFocus(false);
    this->pianoViewport->setFocusContainerType(Component::FocusContainerType::none);
    this->pianoViewport->setPaintingIsUnclipped(true);
    
    this->pianoRoll = make<PianoRoll>(this->project, *this->pianoViewport, clippingDetector);
    this->pianoViewport->setViewedComponent(this->pianoRoll.get(), false);

    this->patternViewport = make<Viewport>();
    this->patternViewport->setScrollOnDragMode(Viewport::ScrollOnDragMode::never);
    this->patternViewport->setInterceptsMouseClicks(false, true);
    this->patternViewport->setScrollBarsShown(false, false);
    this->patternViewport->setWantsKeyboardFocus(false);
    this->patternViewport->setFocusContainerType(Component::FocusContainerType::none);
    this->patternViewport->setPaintingIsUnclipped(true);

    this->patternRoll = make<PatternRoll>(this->project, *this->patternViewport, clippingDetector);
    this->patternViewport->setViewedComponent(this->patternRoll.get(), false);

    // bottom panels

    SafePointer<RollBase> defaultRoll = this->pianoRoll.get();

    this->bottomMapsScroller = make<ProjectMapsScroller>(this->project, defaultRoll);
    this->bottomMapsScroller->addOwnedMap<PianoProjectMap>(this->project);
    this->bottomMapsScroller->addOwnedMap<AnnotationsProjectMap>(this->project, defaultRoll, AnnotationsProjectMap::Type::Small);
    this->bottomMapsScroller->addOwnedMap<TimeSignaturesProjectMap>(this->project, defaultRoll, TimeSignaturesProjectMap::Type::Small);
    //this->bottomMapsScroller->addOwnedMap<KeySignaturesProjectMap>(this->project, defaultRoll, KeySignaturesProjectMap::Type::Small);

    this->pianoRoll->addRollListener(this->bottomMapsScroller.get());
    this->patternRoll->addRollListener(this->bottomMapsScroller.get());

    this->bottomEditorsSwitcher = make<EditorPanelsSwitcher>();
    this->bottomEditorsScroller = make<EditorPanelsScroller>(this->project, defaultRoll, this->bottomEditorsSwitcher.get());
    this->bottomEditorsScroller->addOwnedEditorPanel<VelocityEditor>(this->project, defaultRoll);
    this->bottomEditorsScroller->addOwnedEditorPanel<AutomationEditor>(this->project, defaultRoll);

    this->pianoRoll->addRollListener(this->bottomEditorsScroller.get());
    this->patternRoll->addRollListener(this->bottomEditorsScroller.get());

    this->scrollerShadow = make<ShadowUpwards>(ShadowType::Light);
    
    // a container with 2 rolls and 2 types of bottom scroller panel

    this->rollContainer = make<RollsSwitchingProxy>(this->pianoRoll.get(), this->patternRoll.get(),
        this->pianoViewport.get(), this->patternViewport.get(),
        this->bottomMapsScroller.get(), this->bottomEditorsScroller.get(),
        this->bottomEditorsSwitcher.get(), this->scrollerShadow.get());
    
    const auto hasAnimations = App::Config().getUiFlags()->areUiAnimationsEnabled();
    this->rollContainer->setAnimationsEnabled(hasAnimations);
    this->addAndMakeVisible(this->rollContainer.get());

    // sidebars

    this->rollToolsSidebar = make<SequencerSidebarRight>(this->project);
    this->addAndMakeVisible(this->rollToolsSidebar.get());

    this->rollNavigationSidebar = make<SequencerSidebarLeft>();
    this->rollNavigationSidebar->setAudioMonitor(App::Workspace().getAudioCore().getMonitor());
    this->addAndMakeVisible(this->rollNavigationSidebar.get());

    this->leftSidebarShadow = make<ShadowRightwards>(ShadowType::Light);
    this->addAndMakeVisible(this->leftSidebarShadow.get());
    this->rightSidebarShadow = make<ShadowLeftwards>(ShadowType::Light);
    this->addAndMakeVisible(this->rightSidebarShadow.get());

    App::Config().getUiFlags()->addListener(this);
}

SequencerLayout::~SequencerLayout()
{
    App::Config().getUiFlags()->removeListener(this);
    
    this->leftSidebarShadow = nullptr;
    this->rightSidebarShadow = nullptr;
    this->rollToolsSidebar = nullptr;
    this->rollNavigationSidebar = nullptr;
    this->rollContainer = nullptr;

    this->patternRoll->removeRollListener(this->bottomEditorsScroller.get());
    this->patternRoll->removeRollListener(this->bottomMapsScroller.get());
    this->pianoRoll->removeRollListener(this->bottomEditorsScroller.get());
    this->pianoRoll->removeRollListener(this->bottomMapsScroller.get());
    
    this->scrollerShadow = nullptr;
    this->bottomEditorsScroller = nullptr;
    this->bottomEditorsSwitcher = nullptr;
    this->bottomMapsScroller = nullptr;

    this->patternRoll = nullptr;
    this->patternViewport = nullptr;

    this->pianoRoll = nullptr;
    this->pianoViewport = nullptr;
}

void SequencerLayout::showPatternEditor()
{
    if (!this->rollContainer->isPatternRollMode())
    {
        this->rollContainer->startRollSwitchAnimation();
    }

    this->rollToolsSidebar->setPatternMode();
    this->rollNavigationSidebar->setPatternMode();

    // sync the pattern roll's selection with the piano roll's editable scope:
    this->patternRoll->selectClip(this->pianoRoll->getActiveClip());
    this->pianoRoll->deselectAll();
}

void SequencerLayout::showLinearEditor(const Clip &activeClip)
{
    jassert(activeClip.isValid());

    if (this->rollContainer->isPatternRollMode())
    {
        this->rollContainer->startRollSwitchAnimation();
    }

    this->rollToolsSidebar->setLinearMode();
    this->rollNavigationSidebar->setLinearMode();

    //this->patternRoll->selectClip(activeClip);
    this->pianoRoll->deselectAll();

    this->project.setEditableScope(activeClip, false);
}

RollBase *SequencerLayout::getRoll() const noexcept
{
    if (this->rollContainer->isPatternRollMode())
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
    auto localBounds = this->getLocalBounds();

    const auto leftSidebarWidth = this->rollNavigationSidebar->getWidth();
    const auto rightSidebarWidth = this->rollToolsSidebar->getWidth();
    this->rollNavigationSidebar->setBounds(localBounds.removeFromLeft(leftSidebarWidth));
    this->rollToolsSidebar->setBounds(localBounds.removeFromRight(rightSidebarWidth));
    // a hack for themes changing
    this->rollToolsSidebar->resized();
    
    this->rollContainer->setBounds(localBounds);
    
    constexpr auto sidebarShadowSize = Globals::UI::rollShadowSize * 2;
    this->leftSidebarShadow->setBounds(localBounds.removeFromLeft(sidebarShadowSize));
    this->rightSidebarShadow->setBounds(localBounds.removeFromRight(sidebarShadowSize));
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
        App::showModalComponent(make<RenderDialog>(this->project, url, format));
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

        if (this->rollContainer->isPatternRollMode())
        {
            if (this->project.getLastShownTrack() != nullptr)
            {
                this->project.getLastShownTrack()->setSelected();
            }
            else
            {
                this->project.selectFirstChildOfType<PianoTrackNode>();
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

void SequencerLayout::onEditorPanelVisibilityFlagChanged(bool shoudShow)
{
    const bool alreadyShowing = this->rollContainer->isEditorPanelVisible();
    if ((alreadyShowing && shoudShow) || (!alreadyShowing && !shoudShow))
    {
        return;
    }

    this->rollContainer->startMapSwitchAnimation();
}

void SequencerLayout::onProjectMapLargeModeFlagChanged(bool showFullMap)
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
