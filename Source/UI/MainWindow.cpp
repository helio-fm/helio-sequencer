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
#include "MainWindow.h"
#include "MainLayout.h"
#include "SequencerLayout.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "HelioTheme.h"
#include "BinaryData.h"
#include "ColourSchemesManager.h"
#include "App.h"

class WorkspaceAndroidProxy final : public Component
{
public:

    explicit WorkspaceAndroidProxy(MainLayout *target, int targetScale = 2) :
        workspace(target),
        scale(targetScale)
    {
        Rectangle<int> screenArea = Desktop::getInstance().getDisplays().getMainDisplay().userArea;

        const float realWidth = float(screenArea.getWidth()) / float(this->scale);
        const float realHeight = float(screenArea.getHeight()) / float(this->scale);

        const float realScaleWidth = float(screenArea.getWidth()) / realWidth;
        const float realScaleheight = float(screenArea.getHeight()) / realHeight;

        AffineTransform newTransform = this->workspace->getTransform();
        newTransform = newTransform.scale(realScaleWidth, realScaleheight);
        this->workspace->setTransform(newTransform);

        this->setPaintingIsUnclipped(true);
        this->addAndMakeVisible(this->workspace);
    }

    void resized() override
    {
        if (this->workspace != nullptr)
        {
            this->workspace->setBounds(this->getLocalBounds() / this->scale);
        }
    }

private:

    SafePointer<MainLayout> workspace;
    int scale;
};

MainWindow::MainWindow() :
    DocumentWindow("Helio", Colours::black, DocumentWindow::allButtons)
{
    this->setWantsKeyboardFocus(false);

#if HELIO_DESKTOP

    //this->setResizeLimits(568, 320, 8192, 8192); // phone size test
    this->setResizeLimits(1024, 650, 8192, 8192); // production

#if HELIO_HAS_CUSTOM_TITLEBAR
    this->setResizable(true, true);
    this->setUsingNativeTitleBar(false);
#else
    this->setResizable(true, false);
    this->setUsingNativeTitleBar(true);
#endif

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

    // HelioTheme has been set up previously in App init procedure
    if (HelioTheme *ht = dynamic_cast<HelioTheme *>(&this->getLookAndFeel()))
    {
        ht->initColours(ColourSchemesManager::getInstance().getCurrentScheme());
    }

    const String openGLState = Config::get(Serialization::Config::openGLState);

#if JUCE_MAC || JUCE_ANDROID
    const bool shouldEnableOpenGLByDefault = openGLState.isEmpty();
#else
    const bool shouldEnableOpenGLByDefault = false;
#endif

    if ((openGLState == Serialization::Config::enabledState.toString()) || shouldEnableOpenGLByDefault)
    {
        this->setOpenGLRendererEnabled(true);
    }

    this->createLayoutComponent();
    this->setVisible(true);

#if JUCE_IOS
    Desktop::getInstance().setKioskModeComponent(this);
#endif
}

MainWindow::~MainWindow()
{
    if (this->isOpenGLRendererEnabled())
    {
        this->detachOpenGLContext();
    }
    
    this->dismissLayoutComponent();
}

#if HELIO_HAS_CUSTOM_TITLEBAR
BorderSize<int> MainWindow::getBorderThickness()
{
    return BorderSize<int>(0);
}
#endif

#if HELIO_MOBILE
static bool isRunningOnReallySmallScreen()
{
    Rectangle<int> screenArea = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    const double retinaFactor = Desktop::getInstance().getDisplays().getMainDisplay().scale;
    const double dpi = Desktop::getInstance().getDisplays().getMainDisplay().dpi;
    const double cmWidth = (screenArea.getWidth() / dpi) * retinaFactor * 2.54;
    const double cmHeight = (screenArea.getHeight() / dpi) * retinaFactor * 2.54;

    return (cmWidth < 12.0 || cmHeight < 7.0);
}
#endif

bool MainWindow::isRunningOnPhone()
{
#if HELIO_MOBILE
    return isRunningOnReallySmallScreen();
#elif HELIO_DESKTOP
    return false;
#endif
}

bool MainWindow::isRunningOnTablet()
{
#if HELIO_MOBILE
    return !isRunningOnReallySmallScreen();
#elif HELIO_DESKTOP
    return false;
#endif
}

bool MainWindow::isRunningOnDesktop()
{
#if HELIO_MOBILE
    return isRunningOnReallySmallScreen();
#elif HELIO_DESKTOP
    return false;
#endif
}

void MainWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}

void MainWindow::dismissLayoutComponent()
{
    this->clearContentComponent();
    this->layout = nullptr;
}

void MainWindow::createLayoutComponent()
{
    this->layout = new MainLayout();

#if JUCE_ANDROID
    //const double dpi = Desktop::getInstance().getDisplays().getMainDisplay().dpi;
    //const bool hasRetina = (dpi > 200);

    //if (hasRetina)
    //{
    //    this->setContentOwned(new WorkspaceAndroidProxy(this->layout), false);
    //}
    //else
    {
        this->setContentNonOwned(this->layout, false);
    }
#else
    this->setContentNonOwned(this->layout, false);
#endif

    this->layout->restoreLastOpenedPage();
}

static ScopedPointer<OpenGLContext> kOpenGLContext(nullptr);
static Atomic<int> kOpenGlEnabled(0);

void MainWindow::setOpenGLRendererEnabled(bool shouldBeEnabled)
{
    if (shouldBeEnabled && (kOpenGLContext == nullptr))
    {
        this->attachOpenGLContext();
        Config::set(Serialization::Config::openGLState, Serialization::Config::enabledState.toString());
    }
    else if (!shouldBeEnabled && (kOpenGLContext != nullptr))
    {
        this->detachOpenGLContext();
        Config::set(Serialization::Config::openGLState, Serialization::Config::disabledState.toString());
    }
}

void MainWindow::attachOpenGLContext()
{
    DBG("Attaching OpenGL context.");
    kOpenGLContext = new OpenGLContext();
    kOpenGLContext->setPixelFormat(OpenGLPixelFormat(8, 8, 0, 0));
    kOpenGLContext->setMultisamplingEnabled(false);
    kOpenGLContext->attachTo(*this);
    kOpenGlEnabled = 1;
}

void MainWindow::detachOpenGLContext()
{
    DBG("Detaching OpenGL context.");
    kOpenGLContext->detach();
    kOpenGLContext = nullptr;
    kOpenGlEnabled = 0;
}

bool MainWindow::isOpenGLRendererEnabled() noexcept
{
    return kOpenGlEnabled.get() != 0;
}
