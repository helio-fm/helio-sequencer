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
#include "SerializationKeys.h"
#include "Config.h"

//===----------------------------------------------------------------------===//
// Global UI state
//===----------------------------------------------------------------------===//

bool UserInterfaceFlags::isScalesHighlightingEnabled() const noexcept
{
    return this->scalesHighlighting;
}

void UserInterfaceFlags::setScalesHighlightingEnabled(bool enabled)
{
    if (this->scalesHighlighting == enabled)
    {
        return;
    }

    this->scalesHighlighting = enabled;
    this->listeners.call(&Listener::onScalesHighlightingFlagChanged, this->scalesHighlighting);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

bool UserInterfaceFlags::areNoteNameGuidesEnabled() const noexcept
{
    return this->noteNameGuides;
}

void UserInterfaceFlags::setNoteNameGuidesEnabled(bool enabled)
{
    if (this->noteNameGuides == enabled)
    {
        return;
    }

    this->noteNameGuides = enabled;
    this->listeners.call(&Listener::onNoteNameGuidesFlagChanged, this->noteNameGuides);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

bool UserInterfaceFlags::isUsingFixedDoNotation() const noexcept
{
    return this->useFixedDoNotation;
}

void UserInterfaceFlags::setUseFixedDoNotation(bool enabled)
{
    if (this->useFixedDoNotation == enabled)
    {
        return;
    }

    this->useFixedDoNotation = enabled;
    this->listeners.call(&Listener::onUseFixedDoFlagChanged, this->useFixedDoNotation);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

bool UserInterfaceFlags::isOpenGlRendererEnabled() const noexcept
{
    return this->useOpenGLRenderer;
}

void UserInterfaceFlags::setOpenGlRendererEnabled(bool enabled)
{
    if (this->useOpenGLRenderer == enabled)
    {
        return;
    }

    this->useOpenGLRenderer = enabled;
    this->listeners.call(&Listener::onOpenGlRendererFlagChanged, this->useOpenGLRenderer);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);

}

bool UserInterfaceFlags::isNativeTitleBarEnabled() const noexcept
{
    return this->useNativeTitleBar;
}

void UserInterfaceFlags::setNativeTitleBarEnabled(bool enabled)
{
    if (this->useNativeTitleBar == enabled)
    {
        return;
    }

    this->useNativeTitleBar = enabled;
    this->listeners.call(&Listener::onNativeTitleBarFlagChanged, this->useNativeTitleBar);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

bool UserInterfaceFlags::isEditorPanelVisible() const noexcept
{
    return this->editorPanelVisible;
}

void UserInterfaceFlags::setEditorPanelVisible(bool visible)
{
    if (this->editorPanelVisible == visible)
    {
        return;
    }

    this->editorPanelVisible = visible;
    this->listeners.call(&Listener::onEditorPanelVisibilityFlagChanged, this->editorPanelVisible);
    // not saving this flag
}

void UserInterfaceFlags::toggleEditorPanelVisibility()
{
    this->setEditorPanelVisible(!this->editorPanelVisible);
}

bool UserInterfaceFlags::isProjectMapInLargeMode() const noexcept
{
    return this->projectMapLargeMode;
}

void UserInterfaceFlags::setProjectMapLargeMode(bool largeMode)
{
    if (this->projectMapLargeMode == largeMode)
    {
        return;
    }

    this->projectMapLargeMode = largeMode;
    this->listeners.call(&Listener::onProjectMapLargeModeFlagChanged, this->projectMapLargeMode);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

void UserInterfaceFlags::toggleProjectMapLargeMode()
{
    this->setProjectMapLargeMode(!this->projectMapLargeMode);
}

bool UserInterfaceFlags::areExperimentalFeaturesEnabled() const noexcept
{
    return this->experimentalFeaturesOn;
}

bool UserInterfaceFlags::isFollowingPlayhead() const noexcept
{
    return this->followPlayhead;
}

void UserInterfaceFlags::setFollowingPlayhead(bool following)
{
    if (this->followPlayhead == following)
    {
        return;
    }

    this->followPlayhead = following;
    this->listeners.call(&Listener::onFollowPlayheadFlagChanged, this->followPlayhead);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

bool UserInterfaceFlags::areUiAnimationsEnabled() const noexcept
{
    return this->rollAnimationsEnabled;
}

void UserInterfaceFlags::setUiAnimationsEnabled(bool enabled)
{
    if (this->rollAnimationsEnabled == enabled)
    {
        return;
    }

    this->rollAnimationsEnabled = enabled;
    this->listeners.call(&Listener::onUiAnimationsFlagChanged, this->rollAnimationsEnabled);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

bool UserInterfaceFlags::isZoomLevelLocked() const noexcept
{
    return this->zoomLevelLocked;
}

void UserInterfaceFlags::setZoomLevelLocked(bool locked)
{
    if (this->zoomLevelLocked == locked)
    {
        return;
    }

    this->zoomLevelLocked = locked;
    this->listeners.call(&Listener::onLockZoomLevelFlagChanged, this->zoomLevelLocked);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

void UserInterfaceFlags::toggleLockZoomLevel()
{
    this->setZoomLevelLocked(!this->zoomLevelLocked);
}

int UserInterfaceFlags::getLeftSidebarWidth() const noexcept
{
    return this->leftSidebarWidth;
}

void UserInterfaceFlags::setLeftSidebarWidth(int width)
{
    if (this->leftSidebarWidth == width)
    {
        return;
    }

    this->leftSidebarWidth = width;
    this->listeners.call(&Listener::onSidebarWidthChanged,
        this->leftSidebarWidth, this->rightSidebarWidth);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

int UserInterfaceFlags::getRightSidebarWidth() const noexcept
{
    return this->rightSidebarWidth;
}

void UserInterfaceFlags::setRightSidebarWidth(int width)
{
    if (this->rightSidebarWidth == width)
    {
        return;
    }

    this->rightSidebarWidth = width;
    this->listeners.call(&Listener::onSidebarWidthChanged,
        this->leftSidebarWidth, this->rightSidebarWidth);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

void UserInterfaceFlags::setMouseWheelUsePanningByDefault(bool usePanning)
{
    if (this->mouseWheelFlags.usePanningByDefault == usePanning)
    {
        return;
    }

    this->mouseWheelFlags.usePanningByDefault = usePanning;
    this->listeners.call(&Listener::onMouseWheelFlagsChanged, this->mouseWheelFlags);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

void UserInterfaceFlags::setMouseWheelUseVerticalPanningByDefault(bool useVerticalPanning)
{
    if (this->mouseWheelFlags.useVerticalPanningByDefault == useVerticalPanning)
    {
        return;
    }

    this->mouseWheelFlags.useVerticalPanningByDefault = useVerticalPanning;
    this->listeners.call(&Listener::onMouseWheelFlagsChanged, this->mouseWheelFlags);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

void UserInterfaceFlags::setMouseWheelUseVerticalZoomingByDefault(bool useVerticalZooming)
{
    if (this->mouseWheelFlags.useVerticalZoomingByDefault == useVerticalZooming)
    {
        return;
    }

    this->mouseWheelFlags.useVerticalZoomingByDefault = useVerticalZooming;
    this->listeners.call(&Listener::onMouseWheelFlagsChanged, this->mouseWheelFlags);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

UserInterfaceFlags::MouseWheelFlags UserInterfaceFlags::getMouseWheelFlags() const noexcept
{
    return this->mouseWheelFlags;
}

bool UserInterfaceFlags::isMetronomeEnabled() const noexcept
{
    return this->metronomeEnabled;
}

void UserInterfaceFlags::toggleMetronome()
{
    this->metronomeEnabled = !this->metronomeEnabled;
    this->listeners.call(&Listener::onMetronomeFlagChanged, this->metronomeEnabled);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

void UserInterfaceFlags::setUiScaleFactor(float scale)
{
    if (this->uiScaleFactor == scale)
    {
        return;
    }

    this->uiScaleFactor = jlimit(UserInterfaceFlags::minUiScaleFactor, UserInterfaceFlags::maxUiScaleFactor, scale);
    this->listeners.call(&Listener::onUiScaleChanged, this->uiScaleFactor);
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

float UserInterfaceFlags::getUiScaleFactor() const noexcept
{
    return this->uiScaleFactor;
}

AffineTransform UserInterfaceFlags::getScaledTransformFor(Component *component) const
{
    return component->getTransform().scaled(this->uiScaleFactor, this->uiScaleFactor);
}

KnownPluginList::SortMethod UserInterfaceFlags::getPluginSorting() const noexcept
{
    return this->pluginSorting;
}

bool UserInterfaceFlags::isPluginSortingForwards() const noexcept
{
    return this->pluginSortingForwards;
}

void UserInterfaceFlags::setPluginSorting(KnownPluginList::SortMethod sorting, bool forwards)
{
    if (this->pluginSorting == sorting && this->pluginSortingForwards == forwards)
    {
        return;
    }

    this->pluginSorting = sorting;
    this->pluginSortingForwards = forwards;
    // no listener event here, not needed yet
    this->startTimer(UserInterfaceFlags::saveTimeoutMs);
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData UserInterfaceFlags::serialize() const
{
    using namespace Serialization;
    SerializedData tree(UI::Flags::uiFlags);
    
    tree.setProperty(UI::Flags::noteNameGuides, this->noteNameGuides);
    tree.setProperty(UI::Flags::scalesHighlighting, this->scalesHighlighting);
    tree.setProperty(UI::Flags::useFixedDoNotation, this->useFixedDoNotation);

    tree.setProperty(UI::Flags::openGlRenderer, this->useOpenGLRenderer);
    tree.setProperty(UI::Flags::nativeTitleBar, this->useNativeTitleBar);
    tree.setProperty(UI::Flags::followPlayhead, this->followPlayhead);
    tree.setProperty(UI::Flags::animations, this->rollAnimationsEnabled);
    tree.setProperty(UI::Flags::lockZoomLevel, this->zoomLevelLocked);
    tree.setProperty(UI::Flags::showFullProjectMap, this->projectMapLargeMode);
    tree.setProperty(UI::Flags::uiScaleFactor, this->uiScaleFactor);

    if (this->leftSidebarWidth > Globals::UI::sidebarWidth)
    {
        tree.setProperty(UI::Flags::leftSidebarWidth, this->leftSidebarWidth);
    }

    if (this->rightSidebarWidth > Globals::UI::sidebarWidth)
    {
        tree.setProperty(UI::Flags::rightSidebarWidth, this->rightSidebarWidth);
    }

    tree.setProperty(UI::Flags::mouseWheelAltMode, this->mouseWheelFlags.usePanningByDefault);
    tree.setProperty(UI::Flags::mouseWheelVerticalPanningByDefault, this->mouseWheelFlags.useVerticalPanningByDefault);
    tree.setProperty(UI::Flags::mouseWheelVerticalZoomingByDefault, this->mouseWheelFlags.useVerticalZoomingByDefault);

    tree.setProperty(UI::Flags::metronomeEnabled, this->metronomeEnabled);

    const auto pluginSortingValue = int(this->pluginSorting) * (this->pluginSortingForwards ? 1 : -1);
    tree.setProperty(UI::Flags::pluginsSorting, pluginSortingValue);

    // skips experimentalFeaturesOn, it's read only

    return tree;
}

void UserInterfaceFlags::deserialize(const SerializedData &data)
{
    using namespace Serialization;

    const auto root = data.hasType(UI::Flags::uiFlags) ?
        data : data.getChildWithName(UI::Flags::uiFlags);

    if (!root.isValid())
    {
        return;
    }

    this->noteNameGuides = root.getProperty(UI::Flags::noteNameGuides, this->noteNameGuides);
    this->scalesHighlighting = root.getProperty(UI::Flags::scalesHighlighting, this->scalesHighlighting);
    this->useFixedDoNotation = root.getProperty(UI::Flags::useFixedDoNotation, this->useFixedDoNotation);

    this->useOpenGLRenderer = root.getProperty(UI::Flags::openGlRenderer, this->useOpenGLRenderer);
    this->useNativeTitleBar = root.getProperty(UI::Flags::nativeTitleBar, this->useNativeTitleBar);
    this->followPlayhead = root.getProperty(UI::Flags::followPlayhead, this->followPlayhead);
    this->rollAnimationsEnabled = root.getProperty(UI::Flags::animations, this->rollAnimationsEnabled);
    this->zoomLevelLocked = root.getProperty(UI::Flags::lockZoomLevel, this->zoomLevelLocked);
    this->projectMapLargeMode = root.getProperty(UI::Flags::showFullProjectMap, this->projectMapLargeMode);

    this->uiScaleFactor = root.getProperty(UI::Flags::uiScaleFactor, this->uiScaleFactor);
    this->uiScaleFactor = jlimit(UserInterfaceFlags::minUiScaleFactor, UserInterfaceFlags::maxUiScaleFactor, this->uiScaleFactor);

    if (App::mayHaveDisplayNotch())
    {
        this->leftSidebarWidth = root.getProperty(UI::Flags::leftSidebarWidth, this->leftSidebarWidth);
        this->rightSidebarWidth = root.getProperty(UI::Flags::rightSidebarWidth, this->rightSidebarWidth);
    }

    this->mouseWheelFlags.usePanningByDefault =
        root.getProperty(UI::Flags::mouseWheelAltMode, this->mouseWheelFlags.usePanningByDefault);

    this->mouseWheelFlags.useVerticalPanningByDefault =
        root.getProperty(UI::Flags::mouseWheelVerticalPanningByDefault, false);
    this->mouseWheelFlags.useVerticalZoomingByDefault =
        root.getProperty(UI::Flags::mouseWheelVerticalZoomingByDefault, false);

    this->metronomeEnabled = root.getProperty(UI::Flags::metronomeEnabled, this->metronomeEnabled);

    const int pluginsSortMethod = root.getProperty(UI::Flags::pluginsSorting, int(this->pluginSorting));
    this->pluginSorting = KnownPluginList::SortMethod(abs(pluginsSortMethod));
    this->pluginSortingForwards = pluginsSortMethod >= 0;

    this->editorPanelVisible = false; // not serializing that

    this->experimentalFeaturesOn = root.getProperty(UI::Flags::experimentalFeaturesOn, this->experimentalFeaturesOn);
}

void UserInterfaceFlags::reset() {}

//===----------------------------------------------------------------------===//
// Delayed save callback
//===----------------------------------------------------------------------===//

void UserInterfaceFlags::timerCallback()
{
    this->stopTimer();
    App::Config().save(this, Serialization::Config::activeUiFlags);
}

//===----------------------------------------------------------------------===//
// Flag listeners
//===----------------------------------------------------------------------===//

void UserInterfaceFlags::addListener(Listener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->listeners.add(listener);
}

void UserInterfaceFlags::removeListener(Listener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->listeners.remove(listener);
}

void UserInterfaceFlags::removeAllListeners()
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->listeners.clear();
}
