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
#include "PluginWindow.h"

#include "Workspace.h"
#include "AudioCore.h"

static OwnedArray<PluginWindow> activePluginWindows;

PluginWindow::PluginWindow(Component *uiComponent,
    AudioProcessorGraph::Node::Ptr owner,
    bool shouldMimicComponent) : 
    DocumentWindow(uiComponent->getName(), Colours::darkgrey, DocumentWindow::closeButton),
    owner(owner)
{
    this->setSize(640, 480);
    this->setContentOwned(uiComponent, true);
    this->setTopLeftPosition(100 + Random::getSystemRandom().nextInt(500),
                             100 + Random::getSystemRandom().nextInt(500));
    
    if (shouldMimicComponent)
    {
        this->setUsingNativeTitleBar(false);
        this->setTitleBarHeight(0);
        this->setVisible(false);
    }
    else
    {
        this->setUsingNativeTitleBar(true);
        this->setResizable(true, false);
        this->setVisible(true);
    }

    activePluginWindows.add(this);
}

PluginWindow::~PluginWindow()
{
    activePluginWindows.removeObject(this, false);
    this->clearContentComponent();
}

void PluginWindow::closeButtonPressed()
{
    delete this;
}

void PluginWindow::closeCurrentlyOpenWindowsFor(const AudioProcessorGraph::NodeID nodeId)
{
    for (const auto *window : activePluginWindows)
    {
        if (window->owner->nodeID == nodeId)
        {
            activePluginWindows.removeObject(window, true);
            return;
        }
    }
}

void PluginWindow::closeAllCurrentlyOpenWindows()
{
    for (int i = activePluginWindows.size(); --i >= 0;)
    {
        delete activePluginWindows.getUnchecked(i);
    }
}

constexpr bool autoScaleOptionAvailable =
#if JUCE_WINDOWS && JUCE_WIN_PER_MONITOR_DPI_AWARE
    true;
#else
    false;
#endif

PluginWindow *PluginWindow::getWindowFor(AudioProcessorGraph::Node::Ptr node, bool shouldMimicComponent)
{
    for (auto *window : activePluginWindows)
    {
        if (window->owner == node)
        {
            return window;
        }
    }

    const auto makeDPIAwarenessDisabler = [](const PluginDescription &desc)
    {
        const bool isAutoScaleAvailable = autoScaleOptionAvailable && desc.pluginFormatName.containsIgnoreCase("VST");
        return isAutoScaleAvailable ? make<ScopedDPIAwarenessDisabler>() : nullptr;
    };

    UniquePointer<ScopedDPIAwarenessDisabler> scopedDpiDisabler;
    if (auto *plugin = dynamic_cast<AudioPluginInstance *>(node->getProcessor()))
    {
        scopedDpiDisabler = makeDPIAwarenessDisabler(plugin->getPluginDescription());
    }
    
    AudioProcessorEditor *ui = nullptr;

    if (node->getProcessor()->hasEditor())
    {
        ui = node->getProcessor()->createEditorIfNeeded();
    }

    if (ui == nullptr && !node->getProcessor()->getParameters().isEmpty())
    {
        ui = new GenericAudioProcessorEditor(*node->getProcessor());
    }

    if (ui != nullptr)
    {
        auto *plugin = dynamic_cast<AudioPluginInstance *>(node->getProcessor());
        
        if (plugin != nullptr)
        {
            ui->setName(plugin->getName());
        }
        
        return new PluginWindow(ui, node, shouldMimicComponent);
    }
    
    return nullptr;
}

PluginWindow *PluginWindow::getWindowFor(const String &instrumentId)
{
    if (auto *instrument = App::Workspace().getAudioCore().findInstrumentById(instrumentId))
    {
        if (auto node = instrument->findMainPluginNode())
        {
            return PluginWindow::getWindowFor(node, false);
        }
    }

    return nullptr;
}

bool PluginWindow::showWindowFor(const String &instrumentId)
{
    if (auto *window = PluginWindow::getWindowFor(instrumentId))
    {
        // this callAsync trick is needed, because this may be called by a modal component,
        // and after invoking this callback, it will dismiss, focusing the host window,
        // and pushing the plugin window in the background, which looks silly;
        MessageManager::callAsync([window]() {
            // so we have to bring it to front asynchronously just in case:
            window->toFront(true);
        });

        return true;
    }

    return false;
}
