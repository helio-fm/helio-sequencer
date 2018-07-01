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
#include "App.h"
#include "AudioCore.h"
#include "SessionService.h"
#include "UpdatesService.h"

#include "TranslationsManager.h"
#include "ArpeggiatorsManager.h"
#include "ColourSchemesManager.h"
#include "HotkeySchemesManager.h"
#include "ArpeggiatorsManager.h"
#include "ScalesManager.h"
#include "ScriptsManager.h"

#include "HelioTheme.h"
#include "PluginScanner.h"
#include "Config.h"

#include "DocumentHelpers.h"
#include "XmlSerializer.h"

#include "MainLayout.h"
#include "Document.h"
#include "SerializationKeys.h"

#include "Icons.h"
#include "MainWindow.h"
#include "Workspace.h"
#include "RootTreeItem.h"
#include "SerializablePluginDescription.h"

//===----------------------------------------------------------------------===//
// Static
//===----------------------------------------------------------------------===//

class App &App::Helio() noexcept
{
    return *static_cast<App *>(JUCEApplicationBase::getInstance());
}

class Workspace &App::Workspace() noexcept
{
    return *static_cast<App *>(JUCEApplicationBase::getInstance())->workspace;
}

class MainLayout &App::Layout() noexcept
{
    return *static_cast<App *>(JUCEApplicationBase::getInstance())->window->layout;
}

class MainWindow &App::Window() noexcept
{
    return *static_cast<App *>(JUCEApplicationBase::getInstance())->window;
}

class Config &App::Config() noexcept
{
    return *static_cast<App *>(JUCEApplicationBase::getInstance())->config;
}

class Clipboard &App::Clipboard() noexcept
{
    return static_cast<App *>(JUCEApplicationBase::getInstance())->clipboard;
}

Point<double> App::getScreenInCm()
{
    Rectangle<int> screenArea = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    const double retinaFactor = Desktop::getInstance().getDisplays().getMainDisplay().scale;
    const double dpi = Desktop::getInstance().getDisplays().getMainDisplay().dpi;
    const double cmWidth = (screenArea.getWidth() / dpi) * retinaFactor * 2.54;
    const double cmHeight = (screenArea.getHeight() / dpi) * retinaFactor * 2.54;
    return Point<double>(cmWidth, cmHeight);
}

bool App::isRunningOnPhone()
{
    const auto cmSize = App::getScreenInCm();
    return (cmSize.x < 15.0 || cmSize.y < 8.0);
}

bool App::isRunningOnTablet()
{
#if HELIO_MOBILE
    return ! App::isRunningOnPhone();
#elif HELIO_DESKTOP
    return false;
#endif
}

bool App::isRunningOnDesktop()
{
#if HELIO_MOBILE
    return false;
#elif HELIO_DESKTOP
    return true;
#endif
}

String App::getAppReadableVersion()
{
    static String v;
    
    if (v.isEmpty())
    {
        if (String(APP_VERSION_REVISION).getIntValue() > 0)
        {
            v << APP_VERSION_MAJOR << "." << APP_VERSION_MINOR << "." << APP_VERSION_REVISION;
        }
        else
        {
            v << APP_VERSION_MAJOR << "." << APP_VERSION_MINOR;
        }

        if (String(APP_VERSION_NAME).isNotEmpty())
        {
            v << " (" << APP_VERSION_NAME << ")";
        }
    }

    return v;
}

// In sql format
String App::getCurrentTime()
{
    const Time &time = Time::getCurrentTime();
    return App::getSqlFormattedTime(time);
}

// In sql format
String App::getSqlFormattedTime(const Time &time)
{
    String timeString;
    timeString << time.getYear() << '-';

    const int month = time.getMonth() + 1;
    timeString << (month < 10 ? "0" : "") << month << '-';

    const int day = time.getDayOfMonth();
    timeString << (day < 10 ? "0" : "") << day << ' ';

    const int mins = time.getMinutes();
    timeString << time.getHours() << (mins < 10 ? ":0" : ":") << mins;

    const int secs = time.getSeconds();
    timeString << (secs < 10 ? ":0" : ":") << secs;

    return timeString.trimEnd();
}

bool datesMatchByDay(const Time &date1, const Time &date2)
{
    return (date1.getYear() == date2.getYear() &&
            date1.getDayOfYear() == date2.getDayOfYear());
}

bool dateIsToday(const Time &date)
{
    const Time &now = Time::getCurrentTime();
    return datesMatchByDay(now, date);
}

bool dateIsYesterday(const Time &date)
{
    const Time &yesterday = Time::getCurrentTime() - RelativeTime::days(1);
    return datesMatchByDay(yesterday, date);
}


String App::getHumanReadableDate(const Time &date)
{
    String timeString;

    if (dateIsToday(date))
    {
        const int mins = date.getMinutes();
        timeString << date.getHours() << (mins < 10 ? ":0" : ":") << mins;

        const int secs = date.getSeconds();
        timeString << (secs < 10 ? ":0" : ":") << secs;

        return timeString.trimEnd();
    }
    if (dateIsYesterday(date))
    {
        return TRANS("common::yesterday");
    }

    const int month = date.getMonth() + 1;
    timeString << date.getYear() << '-' << (month < 10 ? "0" : "") << month << '-';

    const int day = date.getDayOfMonth();
    timeString << (day < 10 ? "0" : "") << day;

    return timeString.trimEnd();
}

void App::recreateLayout()
{
    this->window->dismissLayoutComponent();

    if (TreeItem *root = this->workspace->getTreeRoot())
    {
        root->recreateSubtreePages();
    }

    this->window->createLayoutComponent();
}

void App::dismissAllModalComponents()
{
    while (Component *modal = Component::getCurrentlyModalComponent(0))
    {
        Logger::writeToLog("getNumCurrentlyModalComponents: " + String(Component::getNumCurrentlyModalComponents()));
        Logger::writeToLog("Dismissing the modal component: " + modal->getName());
        modal->exitModalState(0);
        // Unowned components may leak here, use with caution
    }
}

//===----------------------------------------------------------------------===//
// JUCEApplication
//===----------------------------------------------------------------------===//

void App::initialise(const String &commandLine)
{
    this->runMode = this->detectRunMode(commandLine);

    if (this->runMode == App::NORMAL)
    {
        Desktop::getInstance().setOrientationsEnabled(Desktop::rotatedClockwise + Desktop::rotatedAntiClockwise);
        
        Logger::setCurrentLogger(&this->logger);
        Logger::writeToLog("Helio v" + App::getAppReadableVersion());

        this->config = new class Config();

        this->theme = new HelioTheme();
        this->theme->initResources();
        LookAndFeel::setDefaultLookAndFeel(this->theme);
    
        // TODO: get rid of singletons someday
        using namespace Serialization;
        this->resourceManagers.set(Resources::translations, &TranslationsManager::getInstance());
        this->resourceManagers.set(Resources::arpeggiators, &ArpeggiatorsManager::getInstance());
        this->resourceManagers.set(Resources::colourSchemes, &ColourSchemesManager::getInstance());
        this->resourceManagers.set(Resources::hotkeySchemes, &HotkeySchemesManager::getInstance());
        this->resourceManagers.set(Resources::scales, &ScalesManager::getInstance());
        this->resourceManagers.set(Resources::scripts, &ScriptsManager::getInstance());

        ResourceManagers::Iterator i(this->resourceManagers);
        while (i.next())
        {
            i.getValue()->initialise();
        }

        this->workspace = new class Workspace();
        this->window = new MainWindow();

        // Prepare backend APIs communication services
        this->sessionService = new SessionService();
        this->updatesService = new UpdatesService();

        TranslationsManager::getInstance().addChangeListener(this);
        
        // Desktop versions will be initialised by InitScreen component.
#if HELIO_MOBILE
        App::Workspace().init();
        App::Layout().init();
#endif
    }
    else if (this->runMode == App::PLUGIN_CHECK)
    {
        this->checkPlugin(commandLine);
        this->quit();
    }
}

void App::shutdown()
{
    if (this->runMode == App::NORMAL)
    {
        TranslationsManager::getInstance().removeChangeListener(this);

        Logger::writeToLog("App::shutdown");

        this->updatesService = nullptr;
        this->sessionService = nullptr;

        this->window = nullptr;
        this->workspace = nullptr;

        this->theme = nullptr;
        this->config = nullptr;

        const File tempFolder(DocumentHelpers::getTemporaryFolder());
        if (tempFolder.exists())
        {
            tempFolder.deleteRecursively();
        }
        
        // Clear cache to avoid leak check to fire.
        Icons::clearPrerenderedCache();
        Icons::clearBuiltInImages();

        ResourceManagers::Iterator i(this->resourceManagers);
        while (i.next())
        {
            i.getValue()->shutdown();
        }

        this->resourceManagers.clear();
        
        Logger::setCurrentLogger(nullptr);
    }
}

const String App::getApplicationName()
{
    if (this->runMode == App::PLUGIN_CHECK)
    {
        return "Helio Plugin Check";
    }

    return "Helio";
}

const String App::getApplicationVersion()
{
    return App::getAppReadableVersion();
}

bool App::moreThanOneInstanceAllowed()
{
    return true; // to be able to check plugins
}

void App::anotherInstanceStarted(const String &commandLine)
{
    // This will get called if the user launches another copy of the app
    
    //this->getWindow()->toFront(true);
    Logger::outputDebugString("Another instance started: " + commandLine);

    //const Component *focused = Component::getCurrentlyFocusedComponent();
    //Logger::outputDebugString(focused ? focused->getName() : "");
}

void App::unhandledException(const std::exception *e, const String &file, int)
{
    Logger::writeToLog("! unhandledException: " + String(e->what()));
    jassertfalse;
}

void App::systemRequestedQuit()
{
    Logger::writeToLog("App::systemRequestedQuit");

    if (this->workspace != nullptr)
    {
        this->workspace->stopPlaybackForAllProjects();
        this->workspace->autosave();
    }

    App::dismissAllModalComponents();
    
    this->triggerAsyncUpdate();
}

void App::suspended()
{
    Logger::writeToLog("App::suspended");

    if (this->workspace != nullptr)
    {
        this->workspace->stopPlaybackForAllProjects();
        this->workspace->getAudioCore().mute();
        this->workspace->autosave();
    }
    
#if JUCE_ANDROID
    this->getWindow()->detachOpenGLContext();
#endif
}

void App::resumed()
{
    Logger::writeToLog("App::resumed");

    if (this->workspace != nullptr)
    {
        this->workspace->getAudioCore().unmute();
    }

#if JUCE_ANDROID
    this->getWindow()->attachOpenGLContext();
#endif
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

SessionService *App::getSessionService() const noexcept
{
    return this->sessionService;
}

HelioTheme *App::getTheme() const noexcept
{
    return this->theme;
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

String App::getMacAddressList()
{
    Array <MACAddress> macAddresses;
    MACAddress::findAllAddresses(macAddresses);

    StringArray addressStrings;

    for (auto && macAddresse : macAddresses)
    {
        addressStrings.add(macAddresse.toString());
    }

    return addressStrings.joinIntoString(", ");
}

App::RunMode App::detectRunMode(const String &commandLine)
{
    if (commandLine.isNotEmpty() &&
        DocumentHelpers::getTempSlot(commandLine).existsAsFile())
    {
        return App::PLUGIN_CHECK;
    }

    return App::NORMAL;
}

void App::checkPlugin(const String &markerFile)
{
#if JUCE_MAC
    Process::setDockIconVisible(false);
#endif

    const File tempFile(DocumentHelpers::getTempSlot(markerFile));

    try
    {
        if (tempFile.existsAsFile() && tempFile.getSize() < 32768)
        {
            const String pluginPath = tempFile.loadFileAsString();

            // delete the file immediately so that host will know if we crashed
            tempFile.deleteFile();

            KnownPluginList pluginList;
            OwnedArray<PluginDescription> typesFound;

            AudioPluginFormatManager formatManager;
            AudioCore::initAudioFormats(formatManager);

            for (int i = 0; i < formatManager.getNumFormats(); ++i)
            {
                const auto format = formatManager.getFormat(i);
                pluginList.scanAndAddFile(pluginPath, false, typesFound, *format);
            }

            // let host know if we haven't crashed at the moment
            if (typesFound.size() != 0)
            {
                ValueTree typesNode(Serialization::Core::instrumentsList);

                for (const auto description : typesFound)
                {
                    SerializablePluginDescription sd(description);
                    typesNode.appendChild(sd.serialize(), nullptr);
                }

                DocumentHelpers::save<XmlSerializer>(tempFile, typesNode);
            }
        }
    }
    catch (...)
    {
        JUCEApplication::quit();
    }
}

void App::handleAsyncUpdate()
{
    this->quit();
}


void App::changeListenerCallback(ChangeBroadcaster *source)
{
    Logger::writeToLog("Reloading translations");
    this->recreateLayout();
}

ResourceManager &App::getResourceManagerFor(const Identifier &id) const
{
    jassert(this->resourceManagers.contains(id));
    return *this->resourceManagers[id];
}

START_JUCE_APPLICATION(App)
