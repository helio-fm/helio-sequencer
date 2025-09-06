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
#include "App.h"
#include "AudioCore.h"
#include "HelioTheme.h"
#include "Config.h"
#include "Icons.h"

#include "DocumentHelpers.h"
#include "XmlSerializer.h"
#include "SerializationKeys.h"
#include "SerializablePluginDescription.h"

#include "MainLayout.h"
#include "ScaledComponentProxy.h"
#include "Workspace.h"
#include "RootNode.h"

//===----------------------------------------------------------------------===//
// Window
//===----------------------------------------------------------------------===//

class MainWindow final : public DocumentWindow
{
public:

    MainWindow() : DocumentWindow("Helio", Colours::darkgrey, DocumentWindow::allButtons) {}

    void initialise(bool enableOpenGl, bool useNativeTitleBar)
    {
        this->setWantsKeyboardFocus(false);
        this->setOpaque(true);

#if PLATFORM_DESKTOP

        this->setDropShadowEnabled(false);
        this->setUsingNativeTitleBar(useNativeTitleBar);

        const bool hasResizableCorner = !useNativeTitleBar;
        this->setResizable(true, hasResizableCorner);

        if (this->resizableCorner != nullptr)
        {
            this->resizableCorner->setRepaintsOnMouseActivity(false);
            this->resizableCorner->setPaintingIsUnclipped(true);
            this->resizableCorner->setBufferedToImage(true);
        }

        const auto defaultBounds = Rectangle<int>(
            int(0.1f * this->getParentWidth()),
            int(0.1f * this->getParentHeight()),
            jmin(1280, int(0.85f * this->getParentWidth())),
            jmin(768, int(0.85f * this->getParentHeight())));

        const auto windowBounds =
            App::Config().getWindowBounds().orFallback(defaultBounds);

        // 568, 320 for small phone size test
        constexpr auto minWidth = 1024;
        constexpr auto minHeight= 650;

        this->setBounds(windowBounds.withSize(
            jmax(windowBounds.getWidth(), minWidth),
            jmax(windowBounds.getHeight(), minHeight)));

        this->setResizeLimits(minWidth, minHeight, 8192, 8192);

#endif

#if PLATFORM_MOBILE
        Desktop::getInstance().setKioskModeComponent(this);
#endif

        this->recreateLayoutComponent();

#if PLATFORM_DESKTOP
        if (App::Config().isWindowMaximised())
        {
            this->setFullScreen(true);
        }
#endif

        this->setVisible(true);

        if (enableOpenGl)
        {
            this->attachOpenGLContext();
        }
    }

    ~MainWindow() override
    {
#if PLATFORM_DESKTOP
        if (!this->isFullScreen())
        {
            App::Config().setWindowBounds(this->getBounds());
        }

        App::Config().setWindowMaximised(this->isFullScreen());
#endif

        this->detachOpenGLContextIfAny();
        this->clearContentComponent();
    }

    void moved() override
    {
        DocumentWindow::moved();

#if PLATFORM_DESKTOP
        if (this->isVisible() && this->isOnDesktop() && !this->isFullScreen())
        {
            App::Config().setWindowBounds(this->getBounds());
        }
#endif
    }

    void resized() override
    {
        DocumentWindow::resized();

        if (this->header != nullptr)
        {
            this->header->setBounds(0, 1, this->getWidth(), this->header->getHeight());
        }

#if PLATFORM_DESKTOP
        if (this->isVisible() && this->isOnDesktop() && !this->isFullScreen())
        {
            App::Config().setWindowBounds(this->getBounds());
        }
#endif
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
        if (this->openGLContext != nullptr &&
            this->openGLContext->isAttached())
        {
            return;
        }

        DBG("Attaching OpenGL context");
        this->openGLContext = make<OpenGLContext>();
        this->openGLContext->setPixelFormat(OpenGLPixelFormat(8, 8, 0, 0));
        this->openGLContext->setMultisamplingEnabled(false);
        this->openGLContext->setTextureMagnificationFilter(OpenGLContext::nearest);
        this->openGLContext->attachTo(*this);
    }

    void detachOpenGLContextIfAny()
    {
        if (this->openGLContext != nullptr)
        {
            DBG("Detaching OpenGL context");
            this->openGLContext->detach();
            this->openGLContext = nullptr;
        }
    }

    void recreateLayoutComponent()
    {
        this->layout = make<MainLayout>();

        if (App::Config().getUiFlags()->getUiScaleFactor() != 1.f)
        {
            // just using this->layout->setTransform(...) won't work here, have to use a proxy component instead:
            this->setContentOwned(new ScaledComponentProxy(this->layout.get()), false);
        }
        else
        {
            this->setContentNonOwned(this->layout.get(), false);
        }

        this->layout->restoreLastOpenedPage();

#if PLATFORM_DESKTOP
        if (this->resizableCorner != nullptr)
        {
            this->setResizable(false, false);
            this->setResizable(true, true); // recreates the component
            if (this->resizableCorner != nullptr)
            {
                this->resizableCorner->setRepaintsOnMouseActivity(false);
                this->resizableCorner->setPaintingIsUnclipped(true);
                this->resizableCorner->setBufferedToImage(true);
            }
        }
#endif
    }

    void setTitleComponent(WeakReference<Component> component)
    {
        this->header = component;

        auto *uiFlags = App::Config().getUiFlags();
        if (uiFlags->getUiScaleFactor() != 1.f)
        {
            this->header->setTransform(uiFlags->getScaledTransformFor(this->header.get()));
        }

        this->setTitleBarHeight(int(this->header->getHeight() * uiFlags->getUiScaleFactor()) + 1);
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

class Clipboard &App::Clipboard() noexcept
{
    return static_cast<App *>(getInstance())->clipboard;
}

static Point<double> getScreenInCm()
{
#if JUCE_UNIT_TESTS
    return { 30.0, 20.0 };
#else
    const auto *mainDisplay = Desktop::getInstance().getDisplays().getPrimaryDisplay();
    jassert(mainDisplay != nullptr);
    const auto screenArea = mainDisplay->userArea;
    const auto cmWidth = (screenArea.getWidth() / mainDisplay->dpi) * mainDisplay->scale * 2.54;
    const auto cmHeight = (screenArea.getHeight() / mainDisplay->dpi) * mainDisplay->scale * 2.54;
    return { cmWidth, cmHeight };
#endif
}

bool App::isRunningOnPhone()
{
#if JUCE_IOS
    static const bool isPhone =
        SystemStats::getDeviceDescription().containsIgnoreCase("iphone");
    return isPhone;
#else
    static const auto cmScreenSize = getScreenInCm();
    static const bool isPhone =
        jmax(cmScreenSize.x, cmScreenSize.y) < 13.0 ||
        jmin(cmScreenSize.x, cmScreenSize.y) < 8.0;
    return isPhone;
#endif
}

bool App::isRunningOnTablet()
{
#if PLATFORM_MOBILE
    return ! App::isRunningOnPhone();
#elif PLATFORM_DESKTOP
    return false;
#endif
}

bool App::mayHaveDisplayNotch()
{
#if PLATFORM_DESKTOP
    return false;
#elif PLATFORM_MOBILE
#if JUCE_IOS
    // iPads don't have the display notch:
    return App::isRunningOnPhone();
#else
    return true;
#endif
#endif
}

String App::translate(I18n::Key singular)
{
    return static_cast<App *>(getInstance())->config->
        getTranslations()->translate(singular);
}

String App::translate(const String &singular)
{
    return static_cast<App *>(getInstance())->config->
        getTranslations()->translate(singular);
}

String App::translate(const char *singular)
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

static bool datesMatchByDay(const Time &date1, const Time &date2)
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

Rectangle<int> App::getWindowBounds()
{
    auto *window = static_cast<App *>(getInstance())->window.get();
    return window->getBounds();
}

void App::recreateLayout()
{
    auto *window = static_cast<App *>(getInstance())->window.get();
    auto *workspace = static_cast<App *>(getInstance())->workspace.get();

    workspace->recreateCommandPaletteActions();
    if (auto *root = workspace->getTreeRoot())
    {
        root->recreateSubtreePages();
    }

    window->recreateLayoutComponent();
}

//===----------------------------------------------------------------------===//
// Modal components
//===----------------------------------------------------------------------===//

void App::showModalComponent(UniquePointer<Component> target)
{
    App::dismissAllModalComponents();

    auto *window = static_cast<App *>(getInstance())->window.get();

    auto *uiFlags = App::Config().getUiFlags();
    if (uiFlags->getUiScaleFactor() != 1.f)
    {
        target->setTransform(uiFlags->getScaledTransformFor(target.get()));
    }

    window->addChildComponent(target.get());

    target->setAlpha(0.f);
    App::animateComponent(target.get(), target->getBounds(), 1.f,
        Globals::UI::fadeInShort, false, 1.0, 0.0);

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

void App::animateComponent(Component *component,
    const Rectangle<int> &finalBounds, float finalAlpha,
    int millisecondsToTake, bool useProxyComponent,
    double startSpeed, double endSpeed)
{
    if (component == nullptr)
    {
        jassertfalse;
        return;
    }

    if (App::Config().getUiFlags()->areUiAnimationsEnabled())
    {
        Desktop::getInstance().getAnimator().animateComponent(component,
            finalBounds, finalAlpha, millisecondsToTake, useProxyComponent, startSpeed, endSpeed);
    }
    else
    {
        component->setAlpha(finalAlpha);
        component->setBounds(finalBounds);
        component->setVisible(finalAlpha > 0);
    }
}

void App::fadeInComponent(Component *component, int millisecondsToTake)
{
    if (component == nullptr)
    {
        jassertfalse;
        return;
    }

    if (!(component->isVisible() && component->getAlpha() == 1.f))
    {
        component->setAlpha(0.f);
        component->setVisible(true);
        App::animateComponent(component, component->getBounds(), 1.f, millisecondsToTake, false, 1.0, 0.0);
    }
}

void App::fadeOutComponent(Component *component, int millisecondsToTake)
{
    if (component == nullptr)
    {
        jassertfalse;
        return;
    }

    if (component->isVisible())
    {
        App::animateComponent(component, component->getBounds(), 0.f, millisecondsToTake, true, 0.0, 1.0);
        component->setVisible(false);
    }
}

void App::cancelAnimation(Component *component)
{
    Desktop::getInstance().getAnimator().cancelAnimation(component, false);
}

bool App::isOpenGLRendererEnabled() noexcept
{
    return static_cast<App *>(getInstance())->window->openGLContext != nullptr;
}

bool App::isWorkspaceInitialized() noexcept
{
    auto *self = dynamic_cast<App *>(getInstance());
    return self != nullptr &&
        self->workspace != nullptr && self->workspace->isInitialized();
}

bool App::isUsingNativeTitleBar() noexcept
{
#if JUCE_WINDOWS || JUCE_LINUX
    return static_cast<App *>(getInstance())->window->isUsingNativeTitleBar();
#else
    return true;
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
    if (commandLine.isNotEmpty() &&
        DocumentHelpers::getTempSlot(commandLine).existsAsFile())
    {
        this->runMode = RunMode::PluginCheck;
    }

    if (this->runMode == RunMode::Normal)
    {
#if JUCE_MAC
        Process::setDockIconVisible(true);
#endif

        DBG("Helio v" + App::getAppReadableVersion());

#if !JUCE_IOS
        const auto album = Desktop::rotatedClockwise + Desktop::rotatedAntiClockwise;
        Desktop::getInstance().setOrientationsEnabled(album);
#endif
        
        this->config = make<class Config>();
        this->config->initResources();

        this->theme = make<HelioTheme>();
        LookAndFeel::setDefaultLookAndFeel(this->theme.get());
        this->theme->initResources();
        this->theme->initColours(this->config->getColourSchemes()->getCurrent());

#if JUCE_UNIT_TESTS

        DBG("===");

        // for unit tests, we want the app and config/resources initialized
        // (we don't need a window and a workspace though)
        UnitTestRunner runner;

        // we don't want to run JUCE's unit tests, just the ones in our category:
        runner.runTestsInCategory(UnitTestCategories::helio,
            Random::getSystemRandom().nextInt64());

        for (int i = 0; i < runner.getNumResults(); ++i)
        {
            if (runner.getResult(i)->failures > 0)
            {
                throw new std::exception();
            }
        }

        this->quit();

        // a hack to allow messages get cleaned up to avoid leaks:
        const auto endTime = Time::currentTimeMillis() + 50;
        while (Time::currentTimeMillis() < endTime)
        {
            Thread::sleep(1);
        }

        DBG("===");

#else

        // if this is not a unit test runner, proceed as normal:

        this->workspace = make<class Workspace>();
        
        const auto shouldEnableOpenGL = this->config->getUiFlags()->isOpenGlRendererEnabled();
        const auto shouldUseNativeTitleBar = this->config->getUiFlags()->isNativeTitleBarEnabled();

        this->window = make<MainWindow>();
        this->window->initialise(shouldEnableOpenGL, shouldUseNativeTitleBar);

        this->config->getUiFlags()->addListener(this);

#endif
    }
    else if (this->runMode == RunMode::PluginCheck)
    {
#if JUCE_MAC
        Process::setDockIconVisible(false);
#endif

        this->checkPlugin(commandLine);
        this->quit();
    }
}

void App::shutdown()
{
    if (this->runMode == RunMode::Normal)
    {
        Desktop::getInstance().getAnimator().cancelAllAnimations(false);

        this->config->getUiFlags()->removeListener(this);

        DBG("Shutting down");

        this->window = nullptr;
        
        if (this->workspace != nullptr)
        {
            this->workspace->shutdown();
            this->workspace = nullptr;
        }

        this->theme = nullptr;
        this->config = nullptr;

        const File tempFolder(DocumentHelpers::getTemporaryFolder());
        if (tempFolder.exists())
        {
            tempFolder.deleteRecursively();
        }
        
        Icons::clearPrerenderedCache();
        Icons::clearBuiltInImages();
    }
}

const String App::getApplicationName()
{
    if (this->runMode == RunMode::PluginCheck)
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
    DBG("Another instance started: " + commandLine);
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
#if !JUCE_IOS
    // on iOS we have the background audio capability
    if (this->workspace != nullptr)
    {
        this->workspace->stopPlaybackForAllProjects();
        this->workspace->autosave();
    }
#endif
    
#if JUCE_ANDROID
    this->window->detachOpenGLContextIfAny();
#endif
}

void App::resumed()
{
#if JUCE_ANDROID
    this->window->attachOpenGLContext();
#endif
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void App::checkPlugin(const String &markerFile)
{
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
    JUCEApplication::quit();
}

//===----------------------------------------------------------------------===//
// UserInterfaceFlags::Listener
//===----------------------------------------------------------------------===//

void App::onOpenGlRendererFlagChanged(bool shouldBeEnabled)
{
    if (App::isOpenGLRendererEnabled() == shouldBeEnabled)
    {
        return;
    }

    auto *window = static_cast<App *>(getInstance())->window.get();

    if (shouldBeEnabled)
    {
        window->attachOpenGLContext();
    }
    else
    {
        window->detachOpenGLContextIfAny();
    }
}

void App::onNativeTitleBarFlagChanged(bool shouldUseNativeTitleBar)
{
#if JUCE_WINDOWS || JUCE_LINUX
    if (App::isUsingNativeTitleBar() == shouldUseNativeTitleBar)
    {
        return;
    }

#   if JUCE_WINDOWS

    // just re-creating a window helps to avoid glitches:
    const bool hasOpenGl = App::isOpenGLRendererEnabled();
    auto *self = static_cast<App *>(getInstance());
    self->window = make<MainWindow>();
    self->window->initialise(hasOpenGl, shouldUseNativeTitleBar);

#   elif JUCE_LINUX

    // on Linux, however, it's not that simple, as always:
    // on some builds and some systems re-creating a window,
    // or simply removing it from the desktop and adding it back
    // will (or will not, depending on your luck) segfault in XLockDisplay,
    // which is given an invalid display pointer, which wasn't initialized
    // properly by XOpenDisplay, apparently because-f-you-that's-why;
    // the stacktrace lead me to this comment in juce_linux_X11.cpp:134 -
    // "on some systems XOpenDisplay will occasionally fail". great.

    App::Layout().showTooltip(TRANS(I18n::Settings::restartRequired),
        MainLayout::TooltipIcon::None);

#   endif

#else
    jassertfalse; // should never hit that
#endif
}

void App::onUiScaleChanged(float scale)
{
#if JUCE_LINUX

    App::Layout().showTooltip(TRANS(I18n::Settings::restartRequired),
        MainLayout::TooltipIcon::None);
    return;

#endif

    Icons::clearPrerenderedCache();

    if (auto *root = this->workspace->getTreeRoot())
    {
        root->recreateSubtreePages(); // resets cached label images
    }

    const bool hasOpenGl = App::isOpenGLRendererEnabled();
    const bool hasNativeTitleBar = App::isUsingNativeTitleBar();

    auto *self = static_cast<App *>(getInstance());
    
#if JUCE_IOS
    // iOS will crash without this:
    Desktop::getInstance().setKioskModeComponent(nullptr);
    // (on Android it will do nothing except blink the screen)
#endif

    self->window = make<MainWindow>();
    self->window->initialise(hasOpenGl, hasNativeTitleBar);
}

//===----------------------------------------------------------------------===//
// Main
//===----------------------------------------------------------------------===//

START_JUCE_APPLICATION(App)

//===----------------------------------------------------------------------===//
// Tests
//===----------------------------------------------------------------------===//

#if JUCE_UNIT_TESTS

// A quick and dirty way to check if all the build systems, as well as
// all the scripts/configs/resources contain the same build version.
// Some places aren't updated automatically, and I tend to overlook that.

class VersionConsistencyTests final : public UnitTest
{
public:

    VersionConsistencyTests() :
        UnitTest("Build systems consistency tests", UnitTestCategories::helio) {}

    void runTest() override
    {
        const auto versionString = String(ProjectInfo::versionString);

        beginTest("Versions consistency");

        auto dir = File::getCurrentWorkingDirectory();
        while (!dir.isRoot())
        {
            const auto projectsChild = dir.getChildFile("Projects");
            if (projectsChild.isDirectory())
            {
                this->projectsDirectory = projectsChild;
                break;
            }

            dir = dir.getParentDirectory();
        }

        expect(this->projectsDirectory.isDirectory(), "Projects directory not found");

        this->checkBuildSystem("Android manifest",
            "Android/app/src/main/AndroidManifest.xml",
            "versionCode=\"" + versionString.removeCharacters(".") + "\"");

        this->checkBuildSystem(".deb package control file",
            "Deployment/Linux/Debian/x64/DEBIAN/control",
            "Version: " + versionString);

        this->checkBuildSystem("AppStream file",
            "Deployment/Linux/fm.helio.Workstation.metainfo.xml",
            "version=\"" + versionString + "\"");

        this->checkBuildSystem("Inno Setup script",
            "Deployment/Windows/setup.iss",
            "\"" + versionString + "\"");

        this->checkBuildSystem("iOS project plist",
            "iOS/Info-App.plist",
            "<string>" + versionString);

        this->checkBuildSystem("macOS project plist",
            "macOS/Info-App.plist",
            "<string>" + versionString);

        this->checkBuildSystem("VS2017 project resource file",
            "VisualStudio2017/resources.rc",
            versionString);

        this->checkBuildSystem("VS2019 project resource file",
            "VisualStudio2019/resources.rc",
            versionString);
    }

    void checkBuildSystem(const String &buildSystemName,
        const String &relativeTargetFilePath, const String &stringToFind)
    {
        const auto target = this->projectsDirectory.getChildFile(relativeTargetFilePath);
        expect(target.existsAsFile(), buildSystemName + " not found");

        if (target.existsAsFile())
        {
            const auto content = target.loadFileAsString();
            expect(content.contains(stringToFind),
                buildSystemName + " is expected to contain " + stringToFind);
        }
    }

private:

    File projectsDirectory;
};

static VersionConsistencyTests versionConsistencyTests;

class BenchmarkPlayground final : public UnitTest
{
public:

    BenchmarkPlayground() :
        UnitTest("Benchmark playground", UnitTestCategories::helio) {}

    void runTest() override
    {
        this->beginTest("Glyph arrangement caching on/off benchmark");

        const int iterations = 32;
        const auto text = Uuid().toString();

        Image img(Image::PixelFormat::ARGB, 1024, 1024, true);
        Graphics g(img);
        g.setFont(Font(Globals::UI::Fonts::S));

        const auto t1 = Time::getMillisecondCounterHiRes();

        for (int i = 0; i < iterations; ++i)
        {
            for (int w = 0; w < 256; ++w)
            {
                HelioTheme::drawText(g, text,
                    Rectangle<float>(0.f, 10.f, float(w), 100.f), Justification::centred);
            }
        }

        const auto t2 = Time::getMillisecondCounterHiRes();

        this->logMessage("GlyphArrangementCache off: " + String(t2 - t1) + "ms");

        const auto t3 = Time::getMillisecondCounterHiRes();

        for (int i = 0; i < iterations; ++i)
        {
            for (int w = 0; w < 256; ++w)
            {
                g.drawText(text,
                    Rectangle<float>(0.f, 10.f, float(w), 100.f), Justification::centred);
            }
        }

        const auto t4 = Time::getMillisecondCounterHiRes();

        this->logMessage("GlyphArrangementCache on: " + String(t4 - t3) + "ms");

        this->expect((t2 - t1) < (t4 - t3),
            "No caching works up to 2x faster, at least on my machine");
    }
};

//static BenchmarkPlayground benchmarkPlayground;

#endif
