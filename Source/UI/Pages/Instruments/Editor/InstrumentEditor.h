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

#pragma once

#include "Instrument.h"
#include "HeadlineItemDataSource.h"
#include "ComponentFader.h"

class HeadlineContextMenuController;
class InstrumentComponent;
class InstrumentEditorConnector;
class InstrumentEditorPin;
class AudioCore;

class InstrumentEditor final :
    public Component,
    public ChangeListener,
    public HeadlineItemDataSource
{
public:

    InstrumentEditor(WeakReference<Instrument> instrument, WeakReference<AudioCore> audioCoreRef);
    ~InstrumentEditor() override;

    void deselectAllNodes();
    void selectNode(AudioProcessorGraph::NodeID id, const MouseEvent &e);
    void updateComponents();

    InstrumentComponent *getComponentForNode(AudioProcessorGraph::NodeID id) const;
    InstrumentEditorConnector *getComponentForConnection(AudioProcessorGraph::Connection conn) const;
    InstrumentEditorPin *findPinAt(const int x, const int y) const;

    void mouseDown(const MouseEvent &e) override;
    void resized() override;

    // Listens to instrument and audio device
    void changeListenerCallback(ChangeBroadcaster *) override;

    //===------------------------------------------------------------------===//
    // HeadlineItemDataSource
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    UniquePointer<Component> createMenu() override;
    Image getIcon() const override;
    String getName() const override;
    bool canBeSelectedAsMenuItem() const override;

private:

    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    friend class InstrumentEditorPin;
    friend class InstrumentEditorConnector;

    void beginConnectorDrag(AudioProcessorGraph::NodeID sourceID, int sourceChannel,
        AudioProcessorGraph::NodeID destinationID, int destinationChannel,
        const MouseEvent &e);
    void dragConnector(const MouseEvent &e);
    void endDraggingConnector(const MouseEvent &e);

    ComponentFader fader;
    WeakReference<Instrument> instrument;
    UniquePointer<Component> background;
    UniquePointer<InstrumentEditorConnector> draggingConnector;
    WeakReference<AudioCore> audioCore;

    AudioProcessorGraph::NodeID selectedNode;

    UniquePointer<HeadlineContextMenuController> contextMenuController;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentEditor)
};
