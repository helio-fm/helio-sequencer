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
        bool usePanningByDefault = false;
        bool useVerticalDirectionByDefault = false;
    };

    class Listener
    {
    public:
        Listener() = default;
        virtual ~Listener() = default;
        virtual void onScalesHighlightingFlagChanged(bool enabled) {}
        virtual void onNoteNameGuidesFlagChanged(bool enabled) {}

        virtual void onOpenGlRendererFlagChanged(bool enabled) {}
        virtual void onNativeTitleBarFlagChanged(bool enabled) {}

        virtual void onVelocityMapVisibilityFlagChanged(bool visible) {}
        virtual void onProjectMapVisibilityFlagChanged(bool showFullMap) {}

        virtual void onUiAnimationsFlagChanged(bool enabled) {}
        virtual void onMouseWheelFlagsChanged(MouseWheelFlags flags) {}
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

    bool isNoteNameGuidesEnabled() const noexcept;
    void setNoteNameGuidesEnabled(bool enabled);

    bool isOpenGlRendererEnabled() const noexcept;
    void setOpenGlRendererEnabled(bool enabled);

    bool isNativeTitleBarEnabled() const noexcept;
    void setNativeTitleBarEnabled(bool enabled);

    bool isVelocityMapVisible() const noexcept;
    void setVelocityMapVisible(bool visible);
    void toggleVelocityMapVisibility();

    bool isFullProjectMapVisible() const noexcept;
    void setFullProjectMapVisible(bool visible);
    void toggleFullProjectMapVisibility();

    bool areExperimentalFeaturesEnabled() const noexcept;

    bool areUiAnimationsEnabled() const noexcept;
    void setUiAnimationsEnabled(bool enabled);

    void setMouseWheelUsePanningByDefault(bool usePanning);
    void setMouseWheelUseVerticalDirectionByDefault(bool useVerticalDirection);
    MouseWheelFlags getMouseWheelFlags() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    bool noteNameGuides = false;
    bool scalesHighlighting = true;

    bool velocityMapVisible = false;
    bool fullProjectMapVisible = true;

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
    bool rollAnimationsEnabled = true;

    MouseWheelFlags mouseWheelFlags;

private:

    // all these options are expected to be toggled by hotkeys,
    // so let's have a sensible delay before we serialize anything:
    static constexpr auto saveTimeoutMs = 3000;

    void timerCallback() override;
    ListenerList<Listener> listeners;
};
