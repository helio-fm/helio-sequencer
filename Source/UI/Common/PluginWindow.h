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

class PluginWindow final : public DocumentWindow
{
public:

    ~PluginWindow() override;

    static PluginWindow *getWindowFor(AudioProcessorGraph::Node::Ptr node, bool shouldMimicComponent);
    static PluginWindow *getWindowFor(const String &instrumentId);

    static bool showWindowFor(const String &instrumentId);

    static void closeCurrentlyOpenWindowsFor(const AudioProcessorGraph::NodeID nodeId);
    static void closeAllCurrentlyOpenWindows();

    void closeButtonPressed() override;

private:

    float getDesktopScaleFactor() const override { return 1.f; }

    PluginWindow(Component *uiComp, AudioProcessorGraph::Node::Ptr owner, bool shouldMimicComponent);
    
    const AudioProcessorGraph::Node::Ptr owner;

};
