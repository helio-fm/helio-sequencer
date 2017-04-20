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

class AudioCore;
class FilterInGraph;
class Instrument;

#include "Serializable.h"

class Instrument :
    public Serializable,
    public ChangeBroadcaster // уведомляет InstrumentEditorPanel
{
public:

    Instrument(AudioPluginFormatManager &formatManager, String name);

    ~Instrument() override;


    String getName() const;
    void setName(const String &name);

    String getIdAndHash() const; // эта строчка назначается слоям
    
    
    void initializeFrom(const PluginDescription &pluginDescription);
    void addNodeToFreeSpace(const PluginDescription &pluginDescription);


    // gets connected to the audiocore's device
    AudioProcessorPlayer &getProcessorPlayer() noexcept
    { return this->processorPlayer; }

    AudioProcessorGraph *getProcessorGraph() noexcept
    { return this->processorGraph; }




    //===------------------------------------------------------------------===//
    // Nodes
    //===------------------------------------------------------------------===//

    int getNumNodes() const noexcept;

    const AudioProcessorGraph::Node::Ptr getNode(const int index) const noexcept;

    const AudioProcessorGraph::Node::Ptr getNodeForId(const uint32 uid) const noexcept;

    // для него есть свой формат, который создаст его по дескрипшну
    AudioProcessorGraph::Node *addNode(Instrument *instrument, double x, double y);

    void addNodeAsync(const PluginDescription &desc,
                      double x, double y,
                      std::function<void (AudioProcessorGraph::Node *)> f);

    void removeNode(const uint32 filterUID);

    void disconnectNode(const uint32 filterUID);

    void removeIllegalConnections();

    void setNodePosition(const int nodeId, double x, double y);

    void getNodePosition(const int nodeId, double &x, double &y) const;


    //===------------------------------------------------------------------===//
    // Default nodes' id's
    //===------------------------------------------------------------------===//

    uint32 getMidiInId() const;

    uint32 getMidiOutId() const;

    uint32 getAudioInId() const;

    uint32 getAudioOutId() const;

    bool isNodeStandardInputOrOutput(uint32 nodeId) const;


    //===------------------------------------------------------------------===//
    // Connections
    //===------------------------------------------------------------------===//

    int getNumConnections() const noexcept;

    const AudioProcessorGraph::Connection *getConnection(const int index) const noexcept;

    const AudioProcessorGraph::Connection *getConnectionBetween(uint32 sourceFilterUID,
            int sourceFilterChannel, uint32 destFilterUID, int destFilterChannel) const noexcept;

    bool canConnect(uint32 sourceFilterUID, int sourceFilterChannel,
                    uint32 destFilterUID, int destFilterChannel) const noexcept;

    bool addConnection(uint32 sourceFilterUID, int sourceFilterChannel,
                       uint32 destFilterUID, int destFilterChannel);

    void removeConnection(const int index);

    void removeConnection(uint32 sourceFilterUID, int sourceFilterChannel,
                          uint32 destFilterUID, int destFilterChannel);


    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;

    void deserialize(const XmlElement &xml) override;

    void reset() override;


    /* The special channel index used to refer to a filter's midi channel.*/
    static const int midiChannelNumber;

    void initializeDefaultNodes();

protected:

    AudioProcessorGraph::Node *midiIn;

    AudioProcessorGraph::Node *midiOut;

    AudioProcessorGraph::Node *audioIn;

    AudioProcessorGraph::Node *audioOut;

    Uuid instrumentID;

    String instrumentName;
    
private:

    String getInstrumentID() const; // будет разным для всех на разных платформах
    
    String getInstrumentHash() const; // будет один для одинаковых инструментов на разных платформах
    
    AudioProcessorGraph::Node *addDefaultNode(const PluginDescription &, double x, double y);

    void configureNode(AudioProcessorGraph::Node *, const PluginDescription &, double x, double y);

    friend class Transport;

    friend class AudioCore;
    
private:

    AudioPluginFormatManager &formatManager;

    AudioProcessorPlayer processorPlayer;

    ScopedPointer<AudioProcessorGraph> processorGraph;


    uint32 lastUID;

    uint32 getNextUID() noexcept;

    XmlElement *createNodeXml(AudioProcessorGraph::Node *const node) const;
    
    void createNodeFromXml(const XmlElement &xml);

    void createNodeFromXmlAsync(const XmlElement &xml,
                                std::function<void (AudioProcessorGraph::Node *)> f);

private:

    WeakReference<Instrument>::Master masterReference;

    friend class WeakReference<Instrument>;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument);

};
