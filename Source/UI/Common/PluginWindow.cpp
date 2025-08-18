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
#include "PluginWindow.h"
#include "Instrument.h"
#include "Workspace.h"
#include "RootNode.h"
#include "InstrumentNode.h"
#include "AudioCore.h"

static AudioProcessorEditor *createProcessorEditor(AudioProcessor &processor)
{
    if (processor.hasEditor())
    {
        if (auto *ui = processor.createEditorIfNeeded())
        {
            return ui;
        }
    }
    
    if (!processor.getParameters().isEmpty())
    {
        return new GenericAudioProcessorEditor(processor);
    }

    jassertfalse;
    return nullptr;
}

static OwnedArray<PluginWindow> activePluginWindows;

PluginWindow::PluginWindow(AudioProcessorGraph::Node::Ptr owner) : 
    DocumentWindow(owner->getProcessor()->getName(), Colours::darkgrey, DocumentWindow::closeButton),
    owner(owner)
{
    this->setSize(640, 480);
    this->setUsingNativeTitleBar(true);

    jassert(owner->getProcessor() != nullptr);
    if (auto *editor = createProcessorEditor(*owner->getProcessor()))
    {
        this->setContentOwned(editor, true);

        this->setResizable(true, false);
        this->setTopLeftPosition(100 + Random::getSystemRandom().nextInt(500),
            100 + Random::getSystemRandom().nextInt(500));

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
    UniquePointer<PluginWindow> deleter(this);
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
    activePluginWindows.clear();
}

PluginWindow *PluginWindow::getWindowFor(AudioProcessorGraph::Node::Ptr node)
{
    for (auto *window : activePluginWindows)
    {
        if (window->owner == node)
        {
            return window;
        }
    }

    UniquePointer<ScopedDPIAwarenessDisabler> scopedDpiDisabler;
    if (auto *plugin = dynamic_cast<AudioPluginInstance *>(node->getProcessor()))
    {
        scopedDpiDisabler = Instrument::makeDPIAwarenessDisabler(plugin->getPluginDescription());
        auto window = make<PluginWindow>(node);
        if (window->isVisible()) // if created an editor successfully
        {
            return window.release();
        }
    }
    
    return nullptr;
}

bool PluginWindow::showWindowFor(const String &instrumentId)
{
    if (auto *instrument = App::Workspace().getAudioCore().findInstrumentById(instrumentId))
    {
        if (auto mainNode = instrument->findFirstMidiReceiver())
        {
            return PluginWindow::showWindowFor(mainNode);
        }
    }

    return false;
}

bool PluginWindow::showWindowFor(AudioProcessorGraph::Node::Ptr node)
{
#if PLATFORM_DESKTOP

    if (auto *window = PluginWindow::getWindowFor(node))
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

#elif PLATFORM_MOBILE

    // on mobile platofrms we're not showing a separate window,
    // instead we create a temporary page in the workspace tree,
    // and show the plugin UI there as a child component

    const auto instrumentNodes =
        App::Workspace().getTreeRoot()->findChildrenOfType<InstrumentNode>();

    for (auto *instrumentNode : instrumentNodes)
    {
        if (instrumentNode->getInstrument()->contains(node))
        {
            instrumentNode->recreateChildrenEditors();
            if (auto *audioPluginNode = instrumentNode->findAudioPluginEditorForNodeId(node->nodeID))
            {
                audioPluginNode->setSelected();
                return true;
            }
        }
    }

#endif

    return false;
}
