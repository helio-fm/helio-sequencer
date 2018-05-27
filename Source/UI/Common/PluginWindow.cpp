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

static Array<PluginWindow *> activePluginWindows;

PluginWindow::PluginWindow(Component *const uiComp,
    AudioProcessorGraph::Node::Ptr owner,
    bool isGeneric,
    bool shouldMimicComponent)
: 
    DocumentWindow(uiComp->getName(), Colours::black, DocumentWindow::closeButton),
    owner(owner),
    isGeneric(isGeneric)
{
    this->setSize(400, 300);
    this->setContentOwned(uiComp, true);
    this->setTopLeftPosition(100 + Random::getSystemRandom().nextInt(500),
                             100 + Random::getSystemRandom().nextInt(500));
    
    if (shouldMimicComponent)
    {
        this->setVisible(false);
        this->setUsingNativeTitleBar(false);
        this->setTitleBarHeight(0);
    }
    else
    {
        this->setVisible(true);
        this->setUsingNativeTitleBar(true);
    }

    activePluginWindows.add(this);
}

void PluginWindow::closeCurrentlyOpenWindowsFor(const AudioProcessorGraph::NodeID nodeId)
{
    for (int i = activePluginWindows.size(); --i >= 0;) {
        if (activePluginWindows.getUnchecked(i)->owner->nodeID == nodeId) {
            delete activePluginWindows.getUnchecked(i);
        }
    }
}

void PluginWindow::closeAllCurrentlyOpenWindows()
{
    for (int i = activePluginWindows.size(); --i >= 0;) {
        delete activePluginWindows.getUnchecked(i);
    }
}

PluginWindow *PluginWindow::getWindowFor(AudioProcessorGraph::Node::Ptr node,
    bool useGenericView,
    bool shouldMimicComponent)
{
    for (int i = activePluginWindows.size(); --i >= 0;)
    {
        if (activePluginWindows.getUnchecked(i)->owner == node
            && activePluginWindows.getUnchecked(i)->isGeneric == useGenericView)
        {
            return activePluginWindows.getUnchecked(i);
        }
    }
    
    AudioProcessorEditor *ui = nullptr;
    
    if (! useGenericView)
    {
        ui = node->getProcessor()->createEditorIfNeeded();
        
        if (ui == nullptr)
        {
            useGenericView = true;
        }
    }
    
    if (useGenericView &&
        !node->getProcessor()->getParameters().isEmpty())
    {
        ui = new GenericAudioProcessorEditor(node->getProcessor());
    }
    
    if (ui != nullptr)
    {
        AudioPluginInstance *const plugin = dynamic_cast<AudioPluginInstance *>(node->getProcessor());
        
        if (plugin != nullptr)
        {
            ui->setName(plugin->getName());
        }
        
        return new PluginWindow(ui, node, useGenericView, shouldMimicComponent);
    }
    
    return nullptr;
}

PluginWindow::~PluginWindow()
{
    activePluginWindows.removeAllInstancesOf(this);
    clearContentComponent();
}

void PluginWindow::closeButtonPressed()
{
    delete this;
}
