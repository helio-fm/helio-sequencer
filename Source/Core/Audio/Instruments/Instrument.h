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

class KeyboardMapping;

class Instrument final :
    public Serializable,
    public ChangeBroadcaster // notifies InstrumentEditor
{
public:

    Instrument(AudioPluginFormatManager &formatManager, const String &name);
    ~Instrument() override;

    String getName() const noexcept;
    void setName(const String &name);

    // midi tracks use this to identify their instruments
    String getIdAndHash() const;
    bool isValid() const noexcept;

    bool isDefaultInstrument() const noexcept;
    bool isMetronomeInstrument() const noexcept;
    bool isMidiOutputInstrument() const noexcept;

    using InitializationCallback = Function<void(Instrument *)>;

    using IOProcessor = AudioProcessorGraph::AudioGraphIOProcessor;

    void initializeMidiOutputInstrument();
    void initializeBuiltInInstrument(const PluginDescription &pluginDescription);
    void initializeFrom(const PluginDescription &pluginDescription, InitializationCallback initCallback);
    void addNodeToFreeSpace(const PluginDescription &pluginDescription, InitializationCallback initCallback);

    static UniquePointer<ScopedDPIAwarenessDisabler>
        makeDPIAwarenessDisabler(const PluginDescription &description);

    class AudioCallback final : public AudioIODeviceCallback, public MidiInputCallback
    {
    public:

        AudioCallback() = default;

        void setProcessor(AudioProcessor *processor);
        MidiMessageCollector &getMidiMessageCollector() noexcept { return messageCollector; }

        void audioDeviceIOCallback(const float **, int, float **, int, int) override;
        void audioDeviceAboutToStart(AudioIODevice *) override;
        void audioDeviceStopped() override;
        void handleIncomingMidiMessage(MidiInput *, const MidiMessage &) override;

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

    AudioCallback &getProcessorPlayer() noexcept
    {
        return this->audioCallback;
    }

    AudioProcessorGraph *getProcessorGraph() const noexcept
    {
        return this->processorGraph.get();
    }

    KeyboardMapping *getKeyboardMapping() const noexcept
    {
        return this->keyboardMapping.get();
    }

    //===------------------------------------------------------------------===//
    // Nodes
    //===------------------------------------------------------------------===//

    int getNumNodes() const noexcept;
    const AudioProcessorGraph::Node::Ptr getNode(int index) const noexcept;
    const AudioProcessorGraph::Node::Ptr getNodeForId(AudioProcessorGraph::NodeID uid) const noexcept;
    bool contains(AudioProcessorGraph::Node::Ptr node) const noexcept;

    const AudioProcessorGraph::Node::Ptr findFirstMidiReceiver() const;

    void removeNode(AudioProcessorGraph::NodeID id);
    void disconnectNode(AudioProcessorGraph::NodeID id);

    void removeAllConnectionsForNode(AudioProcessorGraph::Node::Ptr node);

    void setNodePosition(AudioProcessorGraph::NodeID id, double x, double y);
    void getNodePosition(AudioProcessorGraph::NodeID id, double &x, double &y) const;

    bool isNodeStandardIOProcessor(AudioProcessorGraph::NodeID nodeId) const;
    bool isNodeStandardIOProcessor(AudioProcessorGraph::Node::Ptr node) const;

    // results include standard IO nodes:
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

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

    // The special channel index used to refer to a filter's midi channel
    static constexpr int midiChannelNumber = 0x1000;
    
protected:

    using AddNodeCallback = Function<void(AudioProcessorGraph::Node::Ptr)>;
    void addNodeAsync(const PluginDescription &desc, double x, double y, AddNodeCallback f);
    void removeIllegalConnections();
    void initializeIODevicesFor(AudioProcessorGraph::Node::Ptr mainNode);

    Uuid instrumentId;
    String instrumentName;
    
private:

    String getInstrumentId() const noexcept; // will differ between platforms
    String getInstrumentHash() const; // should be the same on all platforms
    
    AudioProcessorGraph::Node::Ptr addNode(const PluginDescription &, double x, double y);
    AudioProcessorGraph::Node::Ptr addNode(UniquePointer<AudioPluginInstance> instance, const SerializedData &data);
    void configureNode(AudioProcessorGraph::Node::Ptr, const PluginDescription &, double x, double y);

    friend class Transport;
    friend class AudioCore;
    
private:

    AudioPluginFormatManager &formatManager;
    Instrument::AudioCallback audioCallback;
    UniquePointer<AudioProcessorGraph> processorGraph;

    SerializedData serializeNode(AudioProcessorGraph::Node::Ptr node) const;

    using DeserializeNodesCallback = Function<void()>;
    void deserializeNodesAsync(Array<SerializedData> nodesToDeserialize, DeserializeNodesCallback f);

    SerializedData lastValidStateFallback;

private:

    UniquePointer<KeyboardMapping> keyboardMapping;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
    JUCE_DECLARE_WEAK_REFERENCEABLE(Instrument)
};
