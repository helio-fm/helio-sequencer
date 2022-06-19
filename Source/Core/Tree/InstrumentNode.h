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

class Instrument;
class InstrumentEditor;

class AudioPlugin;
class AudioPluginEditor;

#include "TreeNode.h"

class InstrumentNode final : public TreeNode
{
public:

    explicit InstrumentNode(Instrument *targetInstrument = nullptr);
    ~InstrumentNode() override;

    void recreateChildrenEditors();
    void notifyOrchestraChanged();

    Image getIcon() const noexcept override;

    void showPage() override;
    void safeRename(const String &newName, bool sendNotifications) override;
    String getName() const noexcept override;

    //===------------------------------------------------------------------===//
    // Instrument
    //===------------------------------------------------------------------===//

    WeakReference<Instrument> getInstrument() const noexcept;
    TreeNode *findAudioPluginEditorForNodeId(AudioProcessorGraph::NodeID nodeId) const;
    String getInstrumentIdAndHash() const;
    
    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    UniquePointer<Component> createMenu() override;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;

    //===------------------------------------------------------------------===//
    // Callbacks
    //===------------------------------------------------------------------===//

    Function<void(const String &text)> getRenameCallback()
    {
        return [this](const String &text)
        {
            this->safeRename(text, true);
        };
    }

private:

    void initInstrumentEditor();
    void removeInstrumentEditor();

    UniquePointer<InstrumentEditor> instrumentEditor;
    WeakReference<Instrument> instrument;

    JUCE_DECLARE_WEAK_REFERENCEABLE(InstrumentNode)
};

//===----------------------------------------------------------------------===//
// Children editor nodes
//===----------------------------------------------------------------------===//

class KeyboardMappingNode final : public TreeNode
{
public:

    explicit KeyboardMappingNode(WeakReference<Instrument> instrument);

    String getName() const noexcept override;
    Image getIcon() const noexcept override;
    bool hasMenu() const noexcept override;
    UniquePointer<Component> createMenu() override;
    void showPage() override;

private:

    WeakReference<Instrument> instrument;
    UniquePointer<Component> keyboardMappingPage;

};

class AudioPluginNode final : public TreeNode
{
public:

    AudioPluginNode(AudioProcessorGraph::NodeID pluginID, const String &name);

    Image getIcon() const noexcept override;
    AudioProcessorGraph::NodeID getNodeId() const noexcept;

    bool hasMenu() const noexcept override;
    UniquePointer<Component> createMenu() override;

    void showPage() override;

private:

    UniquePointer<Component> audioPluginEditor;
    const AudioProcessorGraph::NodeID nodeId;

};
