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

class Config;
class Network;
class Workspace;
class MainWindow;
class MainLayout;

class Clipboard final
{
public:

    Clipboard() = default;
    void copy(const ValueTree &data, bool mirrorToSystemClipboard = false);
    const ValueTree &getData() const noexcept;

private:

    String getCurrentContentAsString() const;
    ValueTree clipboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Clipboard)
    JUCE_PREVENT_HEAP_ALLOCATION
};

class App final : public JUCEApplication,
                  private AsyncUpdater,
                  private ChangeListener
{
public:

    //===------------------------------------------------------------------===//
    // Static
    //===------------------------------------------------------------------===//

    static class Config &Config() noexcept;
    static class Network &Network() noexcept;
    static class MainLayout &Layout() noexcept;
    static class Workspace &Workspace() noexcept;
    static class Clipboard &Clipboard() noexcept;

    static bool isRunningOnPhone();
    static bool isRunningOnTablet();
    static bool isRunningOnDesktop();
    
    static String getDeviceId();
    static String getAppReadableVersion();
    static String getHumanReadableDate(const Time &date);

    static String translate(const String &singular);
    static String translate(const String &plural, int64 number);

    static void recreateLayout();
    static void dismissAllModalComponents();

    static bool isOpenGLRendererEnabled() noexcept;
    static void setOpenGLRendererEnabled(bool shouldBeEnabled);
    
private:

    //===------------------------------------------------------------------===//
    // JUCEApplication
    //===------------------------------------------------------------------===//

    void initialise(const String &commandLine) override;
    void shutdown() override;

    const String getApplicationName() override;
    const String getApplicationVersion() override;

    bool moreThanOneInstanceAllowed() override;
    void anotherInstanceStarted(const String &) override;
    void unhandledException(const std::exception *, const String &, int) override;
    void systemRequestedQuit() override;
    void suspended() override;
    void resumed() override;

private:

    class Clipboard clipboard;

    UniquePointer<class LookAndFeel> theme;
    UniquePointer<class Config> config;
    UniquePointer<class Workspace> workspace;
    UniquePointer<class MainWindow> window;
    UniquePointer<class Network> network;

private:
    
    void checkPlugin(const String &markerFile);
    void changeListenerCallback(ChangeBroadcaster *source) override;

    enum RunMode
    {
        NORMAL,
        PLUGIN_CHECK
    };

    App::RunMode runMode;
    
    void handleAsyncUpdate() override;
};
