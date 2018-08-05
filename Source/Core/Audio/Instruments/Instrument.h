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

class Instrument final :
    public Serializable,
    public ChangeBroadcaster // notifies InstrumentEditorPanel
{
public:

    Instrument(AudioPluginFormatManager &formatManager, const String &name);
    ~Instrument() override;

    String getName() const;
    void setName(const String &name);

    // midi tracks use this to identify their instruments
    String getIdAndHash() const;
    bool isValid() const noexcept;

    using InitializationCallback = Function<void(Instrument *)>;
    using AddNodeCallback = Function<void(AudioProcessorGraph::Node::Ptr)>;

    void initializeFrom(const PluginDescription &pluginDescription, InitializationCallback initCallback);
    void addNodeToFreeSpace(const PluginDescription &pluginDescription, InitializationCallback initCallback);

    class AudioCallback final : public AudioIODeviceCallback, public MidiInputCallback
    {
    public:

        AudioCallback() = default;

        void setProcessor(AudioProcessor *processor);
        MidiMessageCollector &getMidiMessageCollector() noexcept { return messageCollector; }

        void audioDeviceIOCallback(const float **, int, float **, int, int) override;
        void audioDeviceAboutToStart(AudioIODevice *) override;
        void audioDeviceStopped() override;
        void handleIncomingMidiMessage(MidiInput *, const MidiMessage&) override;

    private:

        AudioProcessor *processor = nullptr;
        CriticalSection lock;
        double sampleRate = 0;
        int blockSize = 0;
        bool isPrepared = false;

        int numInputChans = 0;
        int numOutputChans = 0;
        HeapBlock<float *> channels;
        AudioBuffer<float> tempBuffer;

        MidiBuffer incomingMidi;
        MidiMessageCollector messageCollector;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCallback)
    };

    // gets connected to the audio-core device
    AudioCallback &getProcessorPlayer() noexcept
    { return this->audioCallback; }

    AudioProcessorGraph *getProcessorGraph() noexcept
    { return this->processorGraph; }

    //===------------------------------------------------------------------===//
    // Nodes
    //===------------------------------------------------------------------===//

    int getNumNodes() const noexcept;
    const AudioProcessorGraph::Node::Ptr getNode(int index) const noexcept;
    const AudioProcessorGraph::Node::Ptr getNodeForId(AudioProcessorGraph::NodeID uid) const noexcept;
    
    void removeNode(AudioProcessorGraph::NodeID id);
    void disconnectNode(AudioProcessorGraph::NodeID id);

    void removeAllConnectionsForNode(AudioProcessorGraph::Node::Ptr node);

    void setNodePosition(AudioProcessorGraph::NodeID id, double x, double y);
    void getNodePosition(AudioProcessorGraph::NodeID id, double &x, double &y) const;

    bool isNodeStandardIOProcessor(AudioProcessorGraph::NodeID nodeId) const;
    bool isNodeStandardIOProcessor(AudioProcessorGraph::Node::Ptr node) const;

    // Standard IO nodes included:
    Array<AudioProcessorGraph::Node::Ptr> findMidiAcceptors() const;
    Array<AudioProcessorGraph::Node::Ptr> findMidiProducers() const;
    Array<AudioProcessorGraph::Node::Ptr> findAudioAcceptors() const;
    Array<AudioProcessorGraph::Node::Ptr> findAudioProducers() const;

    //===------------------------------------------------------------------===//
    // Connections
    //===------------------------------------------------------------------===//

    bool hasMidiConnection(AudioProcessorGraph::Node::Ptr src, AudioProcessorGraph::Node::Ptr dest) const noexcept;
    bool hasAudioConnection(AudioProcessorGraph::Node::Ptr src, AudioProcessorGraph::Node::Ptr dest) const noexcept;
    bool hasConnectionsFor(AudioProcessorGraph::Node::Ptr node) const noexcept;

    std::vector<AudioProcessorGraph::Connection> getConnections() const noexcept;
    bool isConnected(AudioProcessorGraph::Connection connection) const noexcept;
    bool canConnect(AudioProcessorGraph::Connection connection) const noexcept;

    void removeConnection(AudioProcessorGraph::Connection connection);
    bool addConnection(AudioProcessorGraph::NodeID sourceID, int sourceChannel,
        AudioProcessorGraph::NodeID destinationID, int destinationChannel);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

    /* The special channel index used to refer to a filter's midi channel.*/
    static const int midiChannelNumber;
    
protected:

    AudioProcessorGraph::Node::Ptr addNode(Instrument *instrument, double x, double y);
    void addNodeAsync(const PluginDescription &desc, double x, double y, AddNodeCallback f);
    void removeIllegalConnections();

    Uuid instrumentID;
    String instrumentName;
    
private:

    String getInstrumentID() const; // will differ between platforms
    String getInstrumentHash() const; // should be the same on all platforms
    
    AudioProcessorGraph::Node::Ptr addNode(const PluginDescription &, double x, double y);
    void configureNode(AudioProcessorGraph::Node::Ptr, const PluginDescription &, double x, double y);

    friend class Transport;
    friend class AudioCore;
    
private:

    AudioPluginFormatManager &formatManager;
    Instrument::AudioCallback audioCallback;
    ScopedPointer<AudioProcessorGraph> processorGraph;

    ValueTree serializeNode(AudioProcessorGraph::Node::Ptr node) const;
    void deserializeNode(const ValueTree &tree);
    void deserializeNodeAsync(const ValueTree &tree, AddNodeCallback f);

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
    JUCE_DECLARE_WEAK_REFERENCEABLE(Instrument)
};
