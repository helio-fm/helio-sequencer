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

#if HELIO_DESKTOP
#   define APP_VERSION_MAJOR "2"
#   define APP_VERSION_MINOR "0"
#   define APP_VERSION_REVISION "0"
#   define APP_VERSION_NAME ""
#elif HELIO_MOBILE
#   define APP_VERSION_MAJOR "2"
#   define APP_VERSION_MINOR "0"
#   define APP_VERSION_REVISION "0"
#   define APP_VERSION_NAME ""
#endif

class MainWindow;
class Workspace;
class MainLayout;
class HelioTheme;
class Config;
class ApiCore;
class AudioCore;
class SessionService;
class ResourceSyncService;

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

    static class App &Helio() noexcept;
    static class Workspace &Workspace() noexcept;
    static class MainLayout &Layout() noexcept;
    static class MainWindow &Window() noexcept;
    static class Config &Config() noexcept;
    static class Clipboard &Clipboard() noexcept;

    static Point<double> getScreenInCm();
    static bool isRunningOnPhone();
    static bool isRunningOnTablet();
    static bool isRunningOnDesktop();
    
    static String getAppReadableVersion();
    static String getHumanReadableDate(const Time &date);
    
    static void dismissAllModalComponents();

    //===------------------------------------------------------------------===//
    // JUCEApplication
    //===------------------------------------------------------------------===//

    void recreateLayout();
    void initialise(const String &commandLine) override;
    void shutdown() override;

    const String getApplicationName() override;
    const String getApplicationVersion() override;

    void unhandledException(const std::exception *e, const String &message, int) override;

    bool moreThanOneInstanceAllowed() override;
    void anotherInstanceStarted(const String &commandLine) override;
    void systemRequestedQuit() override;
    void suspended() override;
    void resumed() override;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    HelioTheme *getTheme() const noexcept;
    SessionService *getSessionService() const noexcept;
    ResourceSyncService *getResourceSyncService() const noexcept;
    ResourceManager &getResourceManagerFor(const Identifier &id) const;

private:

    class Clipboard clipboard;

    UniquePointer<class HelioTheme> theme;
    UniquePointer<class Config> config;
    UniquePointer<class Workspace> workspace;
    UniquePointer<class MainWindow> window;

    UniquePointer<class SessionService> sessionService;
    UniquePointer<class ResourceSyncService> resourceSyncService;

    ResourceManagerPool resourceManagers;

private:

    String getMacAddressList();

    void checkPlugin(const String &markerFile);
    void changeListenerCallback(ChangeBroadcaster *source) override;

private:

    enum RunMode
    {
        NORMAL,
        PLUGIN_CHECK
    };

    App::RunMode detectRunMode(const String &commandLine) const;
    App::RunMode runMode;
    
    void handleAsyncUpdate() override;

};
