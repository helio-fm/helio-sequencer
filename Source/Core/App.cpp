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
#include "Network.h"

#include "HelioTheme.h"
#include "PluginScanner.h"
#include "Config.h"

#include "DocumentHelpers.h"
#include "XmlSerializer.h"

#include "MainLayout.h"
#include "Document.h"
#include "SerializationKeys.h"

#include "Icons.h"
#include "Workspace.h"
#include "RootNode.h"
#include "SerializablePluginDescription.h"

//===----------------------------------------------------------------------===//
// Window
//===----------------------------------------------------------------------===//

class MainWindow final : public DocumentWindow
{
public:

    MainWindow() : DocumentWindow("Helio", Colours::darkgrey, DocumentWindow::allButtons) {}

    // the reason for this method to exist is that heavy-weight constructor is evil:
    void init(bool enableOpenGl, bool useNativeTitleBar)
    {
        this->setWantsKeyboardFocus(false);
        this->setOpaque(true);

#if HELIO_DESKTOP

        //this->setResizeLimits(568, 320, 8192, 8192); // phone size test
        this->setResizeLimits(1024, 650, 8192, 8192); // production

        this->setUseNativeTitleBar(useNativeTitleBar);
        
        this->setBounds(int(0.1f * this->getParentWidth()),
            int(0.1f * this->getParentHeight()),
            jmin(1280, int(0.85f * this->getParentWidth())),
            jmin(768, int(0.85f * this->getParentHeight())));

#endif

#if JUCE_IOS
        this->setVisible(false);
#endif

#if JUCE_ANDROID
        this->setFullScreen(true);
        Desktop::getInstance().setKioskModeComponent(this);
#endif

        if (enableOpenGl)
        {
            this->attachOpenGLContext();
        }

        this->createLayoutComponent();
        this->setVisible(true);

#if JUCE_IOS
        Desktop::getInstance().setKioskModeComponent(this);
#endif
    }

    ~MainWindow() override
    {
        this->detachOpenGLContextIfAny();
        this->dismissLayoutComponent();
    }

    void resized() override
    {
        DocumentWindow::resized();

        if (this->header != nullptr)
        {
            this->header->setBounds(0, 1, this->getWidth(), this->header->getHeight());
        }
    }

    void paint(Graphics &g) override
    {
        if (!this->isUsingNativeTitleBar())
        {
            DocumentWindow::paint(g);
        }
    }

    // Overridden to avoid assertions in ResizableWindow:
#if JUCE_DEBUG
    void addChildComponent(Component *child, int zOrder = -1)
    {
        Component::addChildComponent(child, zOrder);
    }

    void addAndMakeVisible(Component *child, int zOrder = -1)
    {
        Component::addAndMakeVisible(child, zOrder);
    }
#endif

private:

    BorderSize<int> getBorderThickness() override
    {
        return BorderSize<int>(0);
    }

    void closeButtonPressed() override
    {
        JUCEApplication::getInstance()->systemRequestedQuit();
    }

    void attachOpenGLContext()
    {
        DBG("Attaching OpenGL context.");
        this->openGLContext.reset(new OpenGLContext());
        this->openGLContext->setPixelFormat(OpenGLPixelFormat(8, 8, 0, 0));
        this->openGLContext->setMultisamplingEnabled(false);
        this->openGLContext->attachTo(*this);
    }

    void detachOpenGLContextIfAny()
    {
        if (this->openGLContext != nullptr)
        {
            DBG("Detaching OpenGL context.");
            this->openGLContext->detach();
            this->openGLContext = nullptr;
        }
    }

    void dismissLayoutComponent()
    {
        this->clearContentComponent();
        this->layout = nullptr;
    }

    void createLayoutComponent()
    {
        this->layout.reset(new MainLayout());
        // optionally, in future:
        // this->setContentOwned(new ScaledComponentProxy(this->layout), false);
        this->setContentNonOwned(this->layout.get(), false);
        this->layout->restoreLastOpenedPage();
    }

    void setUseNativeTitleBar(bool useNativeTitleBar)
    {
        const bool hasResizableCorner = !useNativeTitleBar;
        this->setResizable(true, hasResizableCorner);
        this->setUsingNativeTitleBar(useNativeTitleBar);
        if (this->resizableCorner != nullptr) {
            this->resizableCorner->setRepaintsOnMouseActivity(false);
            this->resizableCorner->setPaintingIsUnclipped(true);
            this->resizableCorner->setBufferedToImage(true);
        }
    }

    void setTitleComponent(WeakReference<Component> component)
    {
        this->header = component;
        this->setTitleBarHeight(this->header->getHeight() + 1);
        Component::addAndMakeVisible(this->header, -1);
        this->resized();
    }

    WeakReference<Component> header;

    UniquePointer<MainLayout> layout;
    UniquePointer<OpenGLContext> openGLContext;
    
    friend class App;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};


//===----------------------------------------------------------------------===//
// Clipboard
//===----------------------------------------------------------------------===//

String Clipboard::getCurrentContentAsString() const
{
    if (this->clipboard.isValid())
    {
        String text;
        static XmlSerializer serializer;
        serializer.saveToString(text, this->clipboard);
        return text;
    }

    return {};
}

const SerializedData &Clipboard::getData() const noexcept
{
    return this->clipboard;
}

void Clipboard::copy(const SerializedData &data, bool mirrorToSystemClipboard /*= false*/)
{
    this->clipboard = data;

    if (mirrorToSystemClipboard)
    {
        SystemClipboard::copyTextToClipboard(this->getCurrentContentAsString());
    }
}


//===----------------------------------------------------------------------===//
// App
//===----------------------------------------------------------------------===//

class Workspace &App::Workspace() noexcept
{
    return *static_cast<App *>(getInstance())->workspace;
}

class MainLayout &App::Layout() noexcept
{
    return *static_cast<App *>(getInstance())->window->layout;
}

class Config &App::Config() noexcept
{
    return *static_cast<App *>(getInstance())->config;
}

class Network &App::Network() noexcept
{
    return *static_cast<App *>(getInstance())->network;
}

class Clipboard &App::Clipboard() noexcept
{
    return static_cast<App *>(getInstance())->clipboard;
}

static Point<double> getScreenInCm()
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
    const auto cmSize = getScreenInCm();
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

String App::getDeviceId()
{
    static String kDeviceId;

    if (kDeviceId.isEmpty())
    {
        const auto &ids = SystemStats::getDeviceIdentifiers();
        if (!ids.isEmpty())
        {
            kDeviceId = String(constexprHash(ids.joinIntoString({}).toUTF8()));
        }
        else
        {
            const String systemStats =
                SystemStats::getLogonName() +
                SystemStats::getComputerName() +
                SystemStats::getOperatingSystemName() +
                SystemStats::getCpuVendor();

            kDeviceId = String(constexprHash(systemStats.toUTF8()));
        }
    }

    return kDeviceId;
}

String App::translate(const Identifier &singular)
{
    return static_cast<App *>(getInstance())->config->
        getTranslations()->translate(singular.toString());
}

String App::translate(const String &singular)
{
    return static_cast<App *>(getInstance())->config->
        getTranslations()->translate(singular);
}

String App::translate(const char* singular)
{
    return static_cast<App *>(getInstance())->config->
        getTranslations()->translate(singular);
}

String App::translate(const String &plural, int64 number)
{
    return static_cast<App *>(getInstance())->config->
        getTranslations()->translate(plural, number);
}

String App::getAppReadableVersion()
{
    static String v;
    
    if (v.isEmpty())
    {
        v << ProjectInfo::versionString;

#if JUCE_64BIT
        v << " (64-bit)";
#elif JUCE_32BIT
        v << " (32-bit)";
#endif
    }

    return v;
}

bool datesMatchByDay(const Time &date1, const Time &date2)
{
    return (date1.getYear() == date2.getYear() &&
            date1.getDayOfYear() == date2.getDayOfYear());
}

static bool dateIsToday(const Time &date)
{
    const Time &now = Time::getCurrentTime();
    return datesMatchByDay(now, date);
}

static bool dateIsYesterday(const Time &date)
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
    else if (dateIsYesterday(date))
    {
        return TRANS(I18n::Common::yesterday);
    }

    const int month = date.getMonth() + 1;
    timeString << date.getYear() << '-' << (month < 10 ? "0" : "") << month << '-';

    const int day = date.getDayOfMonth();
    timeString << (day < 10 ? "0" : "") << day;

    return timeString.trimEnd();
}

void App::recreateLayout()
{
    auto *window = static_cast<App *>(getInstance())->window.get();
    auto *workspace = static_cast<App *>(getInstance())->workspace.get();

    window->dismissLayoutComponent();

    if (auto *root = workspace->getTreeRoot())
    {
        root->recreateSubtreePages();
    }

    window->createLayoutComponent();
}

//===----------------------------------------------------------------------===//
// Modal components
//===----------------------------------------------------------------------===//

void App::showModalComponent(UniquePointer<Component> target)
{
    // showing a dialog when another one is still present is kinda suspicious
    //jassert(Component::getCurrentlyModalComponent() == nullptr);

    App::dismissAllModalComponents();

    auto *window = static_cast<App *>(getInstance())->window.get();
    window->addChildComponent(target.get());

    Desktop::getInstance().getAnimator().animateComponent(target.get(),
        target->getBounds(), 1.f, LONG_FADE_TIME, false, 0.0, 0.0);

    target->toFront(false);
    target->enterModalState(true, nullptr, true);

    // modal components are unowned (which sucks, but we still need
    // to let modal dialogs delete themselves when they want to):
    target.release();
}

void App::dismissAllModalComponents()
{
    while (auto *modal = Component::getCurrentlyModalComponent(0))
    {
        //DBG("Dismissing a modal component");
        UniquePointer<Component> deleter(modal);
    }
}

void App::setOpenGLRendererEnabled(bool shouldBeEnabled)
{
    if (isOpenGLRendererEnabled() == shouldBeEnabled)
    {
        return;
    }

    auto *window = static_cast<App *>(getInstance())->window.get();
    auto *config = static_cast<App *>(getInstance())->config.get();

    if (shouldBeEnabled)
    {
        window->attachOpenGLContext();
        config->setProperty(Serialization::Config::openGLState,
            Serialization::Config::enabledState.toString());
    }
    else
    {
        window->detachOpenGLContextIfAny();
        config->setProperty(Serialization::Config::openGLState,
            Serialization::Config::disabledState.toString());
    }
}

bool App::isOpenGLRendererEnabled() noexcept
{
    return static_cast<App *>(getInstance())->window->openGLContext != nullptr;
}

bool App::isUsingNativeTitleBar()
{
#if JUCE_WINDOWS || JUCE_LINUX
    return static_cast<App *>(getInstance())->window->isUsingNativeTitleBar();
#else
    return true;
#endif
}

void App::setUsingNativeTitleBar(bool shouldUseNative)
{
#if JUCE_WINDOWS || JUCE_LINUX
    if (isUsingNativeTitleBar() == shouldUseNative)
    {
        return;
    }

    auto *window = static_cast<App *>(getInstance())->window.get();
    auto *config = static_cast<App *>(getInstance())->config.get();

    config->setProperty(Serialization::Config::nativeTitleBar,
        shouldUseNative ?
        Serialization::Config::enabledState.toString() :
        Serialization::Config::disabledState.toString());

    window->dismissLayoutComponent();
    window->setUseNativeTitleBar(shouldUseNative);
    window->createLayoutComponent();

#else
    jassertfalse; // should never hit that
#endif
}

void App::setTitleBarComponent(WeakReference<Component> component)
{
    jassert(! isUsingNativeTitleBar());
    auto *window = static_cast<App *>(getInstance())->window.get();
    window->setTitleComponent(component);
}

//===----------------------------------------------------------------------===//
// JUCEApplication
//===----------------------------------------------------------------------===//

void App::initialise(const String &commandLine)
{
    this->runMode = App::NORMAL;
    if (commandLine.isNotEmpty() &&
        DocumentHelpers::getTempSlot(commandLine).existsAsFile())
    {
        this->runMode = App::PLUGIN_CHECK;
    }

    if (this->runMode == App::NORMAL)
    {
        DBG("Helio v" + App::getAppReadableVersion());

        const auto album = Desktop::rotatedClockwise + Desktop::rotatedAntiClockwise;
        Desktop::getInstance().setOrientationsEnabled(album);
        
        this->config.reset(new class Config());
        this->config->initResources();

        UniquePointer<HelioTheme> helioTheme(new HelioTheme());
        helioTheme->initResources();
        helioTheme->initColours(this->config->getColourSchemes()->getCurrent());

        this->theme.reset(helioTheme.release());
        LookAndFeel::setDefaultLookAndFeel(this->theme.get());
    
        this->workspace.reset(new class Workspace());

#if JUCE_ANDROID
        // OpenGL seems to be the only sensible option on Android:
        const bool shouldEnableOpenGL = true;
        const bool shouldUseNativeTitleBar = true;
#elif JUCE_IOS
        // CoreGraphics renderer is faster anyway:
        const bool shouldEnableOpenGL = false;
        const bool shouldUseNativeTitleBar = true;
#else
        const auto enabledState = Serialization::Config::enabledState.toString();

        const auto opeGlState = this->config->getProperty(Serialization::Config::openGLState);
        const bool shouldEnableOpenGL = opeGlState.isEmpty() || opeGlState == enabledState;

        const auto titleBarState = this->config->getProperty(Serialization::Config::nativeTitleBar);
        const bool shouldUseNativeTitleBar = titleBarState == enabledState;
#endif

        this->window.reset(new MainWindow());
        this->window->init(shouldEnableOpenGL, shouldUseNativeTitleBar);

        this->network.reset(new class Network(*this->workspace.get()));

        // see the comment in changeListenerCallback
        //this->config->getTranslations()->addChangeListener(this);
        
        // Desktop versions will be initialised by InitScreen component.
#if HELIO_MOBILE
        App::Workspace().init();
        App::Layout().show();
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
        //this->config->getTranslations()->removeChangeListener(this);

        DBG("App::shutdown");

        this->window = nullptr;

        this->network = nullptr;

        this->workspace->shutdown();
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
    
    //this->window->toFront(true);
    Logger::outputDebugString("Another instance started: " + commandLine);

    //const Component *focused = Component::getCurrentlyFocusedComponent();
    //Logger::outputDebugString(focused ? focused->getName() : "");
}

void App::systemRequestedQuit()
{
    if (this->workspace != nullptr)
    {
        this->workspace->stopPlaybackForAllProjects();
    }

    App::dismissAllModalComponents();
    this->triggerAsyncUpdate();
}

void App::suspended()
{
    if (this->workspace != nullptr)
    {
        this->workspace->stopPlaybackForAllProjects();
        this->workspace->getAudioCore().setCanSleepAfter(0);
        this->workspace->autosave();
    }
    
#if JUCE_ANDROID
    this->window->detachOpenGLContextIfAny();
#endif
}

void App::resumed()
{
    if (this->workspace != nullptr)
    {
        this->workspace->getAudioCore().setAwake();
    }

#if JUCE_ANDROID
    this->window->attachOpenGLContext();
#endif
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

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
                SerializedData typesNode(Serialization::Core::instrumentsList);

                for (const auto *description : typesFound)
                {
                    const SerializablePluginDescription sd(*description);
                    typesNode.appendChild(sd.serialize());
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
    // after some testing I find this behaviour not great at all, i.e. unpredictable and glitchy
    // maybe instead this should start a timer and check if there are no modal components and
    // no user activity for some time - and then recreate the layout:
    //DBG("Reloading translations");
    //this->recreateLayout();
}

START_JUCE_APPLICATION(App)
