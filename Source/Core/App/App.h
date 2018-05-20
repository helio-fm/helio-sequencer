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

#include "Logger.h"
#include "Clipboard.h"

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
class UpdatesService;

class App final : public JUCEApplication,
                  private AsyncUpdater,
                  private ChangeListener // listens to TranslationManager

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
    static String getCurrentTime();
    static String getSqlFormattedTime(const Time &time);
    static String getHumanReadableDate(const Time &date);
    
    static void dismissAllModalComponents();

    void recreateLayout();
    
    //===------------------------------------------------------------------===//
    // JUCEApplication
    //===------------------------------------------------------------------===//

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

    SessionService *getSessionService() const noexcept;
    HelioTheme *getTheme() const noexcept;
    ResourceManager &getResourceManagerFor(const Identifier &id) const;

private:

    class DebugLogger logger;
    class Clipboard clipboard;

    ScopedPointer<class HelioTheme> theme;
    ScopedPointer<class Config> config;
    ScopedPointer<class Workspace> workspace;
    ScopedPointer<class MainWindow> window;
    ScopedPointer<class SessionService> sessionService;
    ScopedPointer<class UpdatesService> updatesService;

    using ResourceManagers = HashMap<Identifier, ResourceManager *, IdentifierHash>;
    ResourceManagers resourceManagers;

private:

    String getMacAddressList();

    void checkPlugin(const String &markerFile);
    void changeListenerCallback(ChangeBroadcaster *source) override;

private:

    enum RunMode
    {
        NORMAL,
        PLUGIN_CHECK,
        FONT_SERIALIZE
    };

    App::RunMode detectRunMode(const String &commandLine);
    App::RunMode runMode;
    
    void handleAsyncUpdate() override;

};
