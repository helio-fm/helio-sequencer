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

#pragma once

// UI flags is a special scope of configuration that I want to be observable in the runtime,
// and have callbacks and setters defined in a more explicit manner - i.e. not just set(k, v);
// some of these flags are to be toggled by hotkeys in runtime affecting several observers

class UserInterfaceFlags final : public Serializable, private Timer
{
public:

    UserInterfaceFlags() = default;

    struct MouseWheelFlags final
    {
        bool usePanningByDefault = true;
        bool useVerticalPanningByDefault = true;
        bool useVerticalZoomingByDefault = false;
    };

    class Listener
    {
    public:

        Listener() = default;
        virtual ~Listener() = default;

        virtual void onScalesHighlightingFlagChanged(bool enabled) {}
        virtual void onNoteNameGuidesFlagChanged(bool enabled) {}
        virtual void onUseFixedDoFlagChanged(bool enabled) {}

        virtual void onOpenGlRendererFlagChanged(bool enabled) {}
        virtual void onNativeTitleBarFlagChanged(bool enabled) {}

        virtual void onEditorPanelVisibilityFlagChanged(bool visible) {}
        virtual void onProjectMapLargeModeFlagChanged(bool showFullMap) {}

        virtual void onFollowPlayheadFlagChanged(bool following) {}
        virtual void onUiAnimationsFlagChanged(bool enabled) {}
        virtual void onLockZoomLevelFlagChanged(bool zoomLocked) {}
        virtual void onMouseWheelFlagsChanged(MouseWheelFlags flags) {}

        virtual void onUiScaleChanged(float scale) {}

        virtual void onMetronomeFlagChanged(bool enabled) {}
        virtual void onSidebarWidthChanged(int left, int right) {}
    };

    //===------------------------------------------------------------------===//
    // Flag listeners
    //===------------------------------------------------------------------===//

    void addListener(Listener *listener);
    void removeListener(Listener *listener);
    void removeAllListeners();

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    bool isScalesHighlightingEnabled() const noexcept;
    void setScalesHighlightingEnabled(bool enabled);

    bool areNoteNameGuidesEnabled() const noexcept;
    void setNoteNameGuidesEnabled(bool enabled);

    bool isUsingFixedDoNotation() const noexcept;
    void setUseFixedDoNotation(bool enabled);

    bool isOpenGlRendererEnabled() const noexcept;
    void setOpenGlRendererEnabled(bool enabled);

    bool isNativeTitleBarEnabled() const noexcept;
    void setNativeTitleBarEnabled(bool enabled);

    bool isEditorPanelVisible() const noexcept;
    void setEditorPanelVisible(bool visible);
    void toggleEditorPanelVisibility();

    bool isProjectMapInLargeMode() const noexcept;
    void setProjectMapLargeMode(bool largeMode);
    void toggleProjectMapLargeMode();

    bool areExperimentalFeaturesEnabled() const noexcept;

    bool isFollowingPlayhead() const noexcept;
    void setFollowingPlayhead(bool following);

    bool areUiAnimationsEnabled() const noexcept;
    void setUiAnimationsEnabled(bool enabled);

    bool isZoomLevelLocked() const noexcept;
    void setZoomLevelLocked(bool locked);
    void toggleLockZoomLevel();

    int getLeftSidebarWidth() const noexcept;
    void setLeftSidebarWidth(int width);
    int getRightSidebarWidth() const noexcept;
    void setRightSidebarWidth(int width);

    void setMouseWheelUsePanningByDefault(bool usePanning);
    void setMouseWheelUseVerticalPanningByDefault(bool useVerticalPanning);
    void setMouseWheelUseVerticalZoomingByDefault(bool useVerticalZooming);
    MouseWheelFlags getMouseWheelFlags() const noexcept;

    void setUiScaleFactor(float scale);
    float getUiScaleFactor() const noexcept;
    AffineTransform getScaledTransformFor(Component *component) const;

    // not a "UI flag", obviously, but it fits here well
    bool isMetronomeEnabled() const noexcept;
    void toggleMetronome();

    KnownPluginList::SortMethod getPluginSorting() const noexcept;
    bool isPluginSortingForwards() const noexcept;
    void setPluginSorting(KnownPluginList::SortMethod sorting, bool forwards);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    bool noteNameGuides = true;
    bool scalesHighlighting = true;
    bool useFixedDoNotation = false;

    bool editorPanelVisible = false;

#if PLATFORM_MOBILE
    bool projectMapLargeMode = false;
#elif PLATFORM_DESKTOP
    bool projectMapLargeMode = true;
#endif

#if JUCE_ANDROID
    // OpenGL seems to be the only sensible option on Android:
    bool useOpenGLRenderer = true;
    bool useNativeTitleBar = true;
#elif JUCE_IOS
    // CoreGraphics renderer is faster anyway:
    bool useOpenGLRenderer = false;
    bool useNativeTitleBar = true;
#elif JUCE_MAC
    // On macOS, always use native bar and disable that check box in the settings
    bool useNativeTitleBar = true;
    bool useOpenGLRenderer = false;
#elif JUCE_LINUX
    // On Linux, allow switching modes, but use native title bar by default
    // (custom one is still usable but can cause issues with various window managers)
    bool useNativeTitleBar = true;
    bool useOpenGLRenderer = false;
#else
    // On Windows, allow switching modes, and use custom title bar by default
    bool useNativeTitleBar = false;
    bool useOpenGLRenderer = false;
#endif

    bool experimentalFeaturesOn = false;
    bool followPlayhead = false;
    bool rollAnimationsEnabled = true;
    bool zoomLevelLocked = false;

    int leftSidebarWidth = Globals::UI::sidebarWidth;
    int rightSidebarWidth = Globals::UI::sidebarWidth;

    MouseWheelFlags mouseWheelFlags;

    float uiScaleFactor = 1.f;
    static constexpr auto minUiScaleFactor = 1.f;
    static constexpr auto maxUiScaleFactor = 3.f;

    bool metronomeEnabled = false;

#if JUCE_IOS
    bool pluginSortingForwards = false;
#else
    bool pluginSortingForwards = true;
#endif
    KnownPluginList::SortMethod pluginSorting =
        KnownPluginList::SortMethod::sortByFormat;

private:

    // all these options are expected to be toggled by hotkeys,
    // so let's have a sensible delay before we serialize anything:
    static constexpr auto saveTimeoutMs = 2000;

    void timerCallback() override;
    ListenerList<Listener> listeners;
};
