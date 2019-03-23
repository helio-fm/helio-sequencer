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
class AudioCore;

#include "TreeNode.h"

class InstrumentNode final : public TreeNode
{
public:

    explicit InstrumentNode(Instrument *targetInstrument = nullptr);
    ~InstrumentNode() override;

    void updateChildrenEditors();

    Image getIcon() const noexcept override;

    void showPage() override;
    void safeRename(const String &newName, bool sendNotifications) override;

    //===------------------------------------------------------------------===//
    // Instrument
    //===------------------------------------------------------------------===//

    WeakReference<Instrument> getInstrument() const noexcept;
    Array<uint32> getInstrumentNodeIds() const;
    bool hasInstrumentWithNodeId(uint32 nodeId) const;
    TreeNode *findAudioPluginEditorForNodeId(AudioProcessorGraph::NodeID nodeId) const;
    String getInstrumentIdAndHash() const;
    
    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    ScopedPointer<Component> createMenu() override;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;

    //===------------------------------------------------------------------===//
    // Callbacks
    //===------------------------------------------------------------------===//

    Function<void(const String &text)> getRenameCallback();

private:

    void initInstrumentEditor();
    void removeInstrumentEditor();
    void notifyOrchestraChanged();

    ScopedPointer<InstrumentEditor> instrumentEditor;
    WeakReference<Instrument> instrument;
    WeakReference<AudioCore> audioCore;

    JUCE_DECLARE_WEAK_REFERENCEABLE(InstrumentNode)
};
