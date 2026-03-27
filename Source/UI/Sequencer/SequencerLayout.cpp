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

SequencerLayout::SequencerLayout(ProjectNode &parentProject) noexcept :
    project(parentProject)
{
    this->setComponentID(ComponentIDs::sequencerLayoutId);
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(false);
    this->setAccessible(false);
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

    // layout for rolls and bottom maps/panels

    // visibility defaults to the piano roll and the mini-map:
    this->addAndMakeVisible(this->pianoViewport.get());
    this->addChildComponent(this->patternViewport.get()); // invisible by default
    this->addAndMakeVisible(this->scrollerShadow.get());
    this->addChildComponent(this->bottomEditorsSwitcher.get()); // invisible by default, behind the scroller
    this->addChildComponent(this->bottomEditorsScroller.get()); // invisible by default, behind the piano map
    this->addAndMakeVisible(this->bottomMapsScroller.get());
    this->patternRoll->setEnabled(false);
    this->patternRoll->setVisible(false);

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

    // ui flags

    auto *uiFlags = App::Config().getUiFlags();
    this->setAnimationsEnabled(uiFlags->areUiAnimationsEnabled());

    if (uiFlags->isProjectMapInLargeMode())
    {
        this->bottomMapsScroller->setScrollerMode(ProjectMapsScroller::ScrollerMode::Map);
    }
    else
    {
        this->scrollerModeAnimation.resetToEnd();
        this->bottomMapsScroller->setScrollerMode(ProjectMapsScroller::ScrollerMode::Scroller);
    }

    if (uiFlags->isEditorPanelVisible())
    {
        this->mapsAnimation.resetToEnd();
        this->bottomMapsScroller->setVisible(false);
        this->bottomEditorsScroller->setVisible(true);
        this->bottomEditorsSwitcher->setVisible(true);
    }

    uiFlags->addListener(this);
}

SequencerLayout::~SequencerLayout()
{
    App::Config().getUiFlags()->removeListener(this);

    this->leftSidebarShadow = nullptr;
    this->rightSidebarShadow = nullptr;
    this->rollToolsSidebar = nullptr;
    this->rollNavigationSidebar = nullptr;

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
    if (!this->isPatternRollMode())
    {
        this->switchRolls();
    }

    this->rollToolsSidebar->setPatternMode();
    this->rollNavigationSidebar->setPatternMode();

    // sync the pattern roll's selection with the piano roll's editable scope:
    this->pianoRoll->deselectAll();
    this->patternRoll->selectClip(this->pianoRoll->getActiveClip());
}

void SequencerLayout::showLinearEditor(const Clip &activeClip)
{
    jassert(activeClip.isValid());

    if (this->isPatternRollMode())
    {
        this->switchRolls();
    }

    this->rollToolsSidebar->setLinearMode();
    this->rollNavigationSidebar->setLinearMode();

    this->pianoRoll->deselectAll();
    //this->patternRoll->selectClip(activeClip);

    this->project.setEditableScope(activeClip, false);
}

RollBase *SequencerLayout::getRoll() const noexcept
{
    if (this->isPatternRollMode())
    {
        return this->patternRoll.get();
    }
    else
    {
        return this->pianoRoll.get();
    }
}

//===----------------------------------------------------------------------===//
// MultiTimer
//===----------------------------------------------------------------------===//

void SequencerLayout::timerCallback(int timerId)
{
    switch (timerId)
    {
    case AnimationTimers::rolls:
        if (this->rollsAnimation.tickAndCheckIfDone())
        {
            this->stopTimer(AnimationTimers::rolls);

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

            if (this->isShowing())
            {
                this->resized();
            }
        }
        else if (this->isShowing())
        {
            this->updateAnimatedMapsPositions();
            this->updateAnimatedRollsPositions();
        }
        break;
    case AnimationTimers::maps:
        if (this->mapsAnimation.tickAndCheckIfDone())
        {
            this->stopTimer(AnimationTimers::maps);

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
        if (this->isShowing())
        {
            this->updateAnimatedMapsPositions();
            this->updateAnimatedRollsBounds();
        }
        break;
    case AnimationTimers::scrollerMode:
        if (this->scrollerModeAnimation.tickAndCheckIfDone())
        {
            this->stopTimer(AnimationTimers::scrollerMode);
            this->scrollerModeAnimation.finish();
        }
        if (this->isShowing())
        {
            this->updateAnimatedMapsBounds();
            this->updateAnimatedRollsBounds();
        }
        break;
    default:
        break;
    }
}

void SequencerLayout::switchRolls()
{
    this->rollsAnimation.start(SequencerLayout::rollsAnimationStartSpeed);

    const bool patternRollMode = this->isPatternRollMode();
    const SafePointer<RollBase> switchTo = patternRollMode ?
        (RollBase *)this->patternRoll.get() : (RollBase *)this->pianoRoll.get();

    this->bottomMapsScroller->switchToRoll(switchTo);
    this->bottomEditorsScroller->switchToRoll(switchTo);

    // disabling the rolls prevents them from receiving keyboard events:
    this->patternRoll->setEnabled(patternRollMode);
    this->pianoRoll->setEnabled(!patternRollMode);
    this->patternRoll->setVisible(true);
    this->pianoRoll->setVisible(true);
    this->patternViewport->setVisible(true);
    this->pianoViewport->setVisible(true);

    if (this->areAnimationsEnabled() && this->isShowing())
    {
        this->resized();
        this->startTimer(AnimationTimers::rolls, this->animationsTimerInterval);
    }
    else
    {
        this->rollsAnimation.finish();
        this->timerCallback(AnimationTimers::rolls);
    }
}

void SequencerLayout::switchEditorPanels()
{
    this->mapsAnimation.start(SequencerLayout::mapsAnimationStartSpeed);

    // disabling the panels prevents them from receiving keyboard events:
    const bool editorPanelMode = this->isEditorPanelVisible();
    this->bottomEditorsScroller->setEnabled(editorPanelMode);
    this->bottomEditorsSwitcher->setEnabled(editorPanelMode);
    this->bottomMapsScroller->setEnabled(!editorPanelMode);
    this->bottomEditorsScroller->setVisible(true);
    this->bottomEditorsSwitcher->setVisible(true);
    this->bottomMapsScroller->setVisible(true);

    if (this->areAnimationsEnabled() && this->isShowing())
    {
        this->resized();
        this->startTimer(AnimationTimers::maps, this->animationsTimerInterval);
    }
    else
    {
        this->mapsAnimation.finish();
        this->timerCallback(AnimationTimers::maps);
    }
}

void SequencerLayout::switchScrollerMode()
{
    this->scrollerModeAnimation.start(SequencerLayout::scrollerModeAnimationStartSpeed);

    if (this->isFullProjectMapMode())
    {
        this->bottomMapsScroller->setScrollerMode(ProjectMapsScroller::ScrollerMode::Scroller);
    }
    else
    {
        this->bottomMapsScroller->setScrollerMode(ProjectMapsScroller::ScrollerMode::Map);
    }

    if (this->areAnimationsEnabled() && this->isShowing())
    {
        this->resized();
        this->startTimer(AnimationTimers::scrollerMode, this->animationsTimerInterval);
    }
    else
    {
        this->scrollerModeAnimation.finish();
        this->timerCallback(AnimationTimers::scrollerMode);
    }
}

void SequencerLayout::updateAnimatedRollsBounds()
{
    const auto rollsBounds = this->getRollsBounds();

    const auto scrollerHeight = Globals::UI::projectMapHeight -
        int((Globals::UI::projectMapHeight - Globals::UI::rollScrollerHeight) *
            this->scrollerModeAnimation.getPosition());

    const auto editorPanelHeight =
        int(Globals::UI::editorPanelHeight * this->mapsAnimation.getPosition());

    const auto maxBottomPanelHeight = jmax(scrollerHeight, editorPanelHeight);

    const int rollViewportHeight = rollsBounds.getHeight() - maxBottomPanelHeight + 1;
    const Rectangle<int> rollSize(rollsBounds.withBottom(rollsBounds.getBottom() - maxBottomPanelHeight));
    const int viewport1Pos = int(-this->rollsAnimation.getPosition() * rollViewportHeight);
    const int viewport2Pos = int(-this->rollsAnimation.getPosition() * rollViewportHeight + rollViewportHeight);

    this->pianoViewport->setBounds(rollSize.withY(viewport1Pos));
    this->patternViewport->setBounds(rollSize.withY(viewport2Pos));
}

void SequencerLayout::updateAnimatedRollsPositions()
{
    const auto rollsBounds = this->getRollsBounds();

    const auto scrollerHeight = Globals::UI::projectMapHeight -
        int((Globals::UI::projectMapHeight - Globals::UI::rollScrollerHeight) *
            this->scrollerModeAnimation.getPosition());

    const auto editorPanelHeight =
        int(Globals::UI::editorPanelHeight * this->mapsAnimation.getPosition());

    const auto maxBottomPanelHeight = jmax(scrollerHeight, editorPanelHeight);

    const int rollViewportHeight = rollsBounds.getHeight() - maxBottomPanelHeight + 1;
    const int viewport1Pos = int(-this->rollsAnimation.getPosition() * rollViewportHeight);
    const int viewport2Pos = int(-this->rollsAnimation.getPosition() * rollViewportHeight + rollViewportHeight);

    this->pianoViewport->setTopLeftPosition(rollsBounds.getX(), viewport1Pos);
    this->patternViewport->setTopLeftPosition(rollsBounds.getX(), viewport2Pos);
}

void SequencerLayout::updateAnimatedMapsBounds()
{
    const auto projectMapHeight = Globals::UI::projectMapHeight -
        int((Globals::UI::projectMapHeight - Globals::UI::rollScrollerHeight) *
            this->scrollerModeAnimation.getPosition());

    const auto mapsBounds = this->getRollsBounds().removeFromBottom(projectMapHeight);
    const auto panelsBounds = this->getRollsBounds().removeFromBottom(Globals::UI::editorPanelHeight);
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
        .translated(0, -SequencerLayout::scrollerShadowSize)
        .withHeight(SequencerLayout::scrollerShadowSize));
}

void SequencerLayout::updateAnimatedMapsPositions()
{
    const auto rollsBounds = this->getRollsBounds();

    const auto projectMapHeight = Globals::UI::projectMapHeight -
        int((Globals::UI::projectMapHeight - Globals::UI::rollScrollerHeight) *
            this->scrollerModeAnimation.getPosition());

    const auto mapsY = rollsBounds.getHeight() - projectMapHeight;
    const auto panelsToMapsOffset = Globals::UI::editorPanelHeight - projectMapHeight;
    const auto switcherToMapsOffset = panelsToMapsOffset + EditorPanelsSwitcher::switcherHeight;

    const int mapsPosition = roundToInt(this->mapsAnimation.getPosition() * projectMapHeight);
    const int panelsPosition = roundToInt(this->mapsAnimation.getPosition() * panelsToMapsOffset);
    const int switcherPosition = roundToInt(this->mapsAnimation.getPosition() * switcherToMapsOffset);

    this->bottomMapsScroller->setTopLeftPosition(rollsBounds.getX(), mapsY + mapsPosition);
    this->bottomEditorsScroller->setTopLeftPosition(rollsBounds.getX(), mapsY - panelsPosition);

    const auto switcherHidingOffset = roundToInt(this->rollsAnimation.getPosition() * this->bottomEditorsSwitcher->getHeight());
    this->bottomEditorsSwitcher->setTopLeftPosition(rollsBounds.getX(), mapsY - switcherPosition + switcherHidingOffset);

    this->scrollerShadow->setTopLeftPosition(rollsBounds.getX(),
        this->bottomEditorsScroller->getY() - this->scrollerShadow->getHeight());
}

inline void SequencerLayout::setAnimationsEnabled(bool shouldBeEnabled)
{
    this->animationsTimerInterval = shouldBeEnabled ? 1000 / 60 : 0;
    this->bottomMapsScroller->setAnimationsEnabled(shouldBeEnabled);
    this->bottomEditorsScroller->setAnimationsEnabled(shouldBeEnabled);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

Rectangle<int> SequencerLayout::getRollsBounds()
{
    auto localBounds = this->getLocalBounds();
    jassert(localBounds.getWidth() > 0 && localBounds.getHeight() > 0);
    localBounds.removeFromLeft(this->rollNavigationSidebar->getWidth());
    localBounds.removeFromRight(this->rollToolsSidebar->getWidth());
    return localBounds;
}

void SequencerLayout::visibilityChanged()
{
    if (this->isVisible() && !this->getLocalBounds().isEmpty())
    {
        this->resized();
    }
}

void SequencerLayout::resized()
{
    auto localBounds = this->getLocalBounds();
    jassert(localBounds.getWidth() > 0 && localBounds.getHeight() > 0);

    const auto leftSidebarWidth = this->rollNavigationSidebar->getWidth();
    const auto rightSidebarWidth = this->rollToolsSidebar->getWidth();
    this->rollNavigationSidebar->setBounds(localBounds.removeFromLeft(leftSidebarWidth));
    this->rollToolsSidebar->setBounds(localBounds.removeFromRight(rightSidebarWidth));

    this->updateAnimatedRollsBounds();
    this->updateAnimatedMapsBounds();

    if ((this->pianoRoll->getBeatWidth() * this->pianoRoll->getNumBeats()) < localBounds.getWidth())
    {
        this->pianoRoll->setBeatWidth(float(localBounds.getWidth()) / float(this->pianoRoll->getNumBeats()));
    }

    if ((this->patternRoll->getBeatWidth() * this->patternRoll->getNumBeats()) < localBounds.getWidth())
    {
        this->patternRoll->setBeatWidth(float(localBounds.getWidth()) / float(this->patternRoll->getNumBeats()));
    }

    // force update children bounds, even if the rolls have just moved
    this->pianoRoll->resized();
    this->patternRoll->resized();

    this->leftSidebarShadow->setBounds(localBounds.removeFromLeft(Globals::UI::sidebarShadowSize));
    this->rightSidebarShadow->setBounds(localBounds.removeFromRight(Globals::UI::sidebarShadowSize));
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
    case CommandIDs::RenderToOGG:
        this->proceedToRenderDialog(RenderFormat::OGG);
        return;
    case CommandIDs::SwitchBetweenRolls:
        if (!this->canAnimate(AnimationTimers::rolls))
        {
            break;
        }

        if (this->isPatternRollMode())
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
    if (this->isEditorPanelVisible() == shoudShow)
    {
        return;
    }

    this->switchEditorPanels();
}

void SequencerLayout::onProjectMapLargeModeFlagChanged(bool showFullMap)
{
    if (this->isFullProjectMapMode() == showFullMap)
    {
        return;
    }

    this->switchScrollerMode();
}

void SequencerLayout::onUiAnimationsFlagChanged(bool enabled)
{
    this->setAnimationsEnabled(enabled);
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
    {
        return;
    }

    this->pianoRoll->deserialize(root);
    this->patternRoll->deserialize(root);
}

void SequencerLayout::reset()
{
    // no need for this yet
}
