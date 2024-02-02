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
#include "Instrument.h"
#include "PluginWindow.h"
#include "InternalIODevicesPluginFormat.h"
#include "SerializablePluginDescription.h"
#include "SerializationKeys.h"
#include "DefaultSynthAudioPlugin.h"
#include "MetronomeSynthAudioPlugin.h"
#include "BuiltInSynthsPluginFormat.h"
#include "KeyboardMapping.h"
#include "Workspace.h"
#include "AudioCore.h"

Instrument::Instrument(AudioPluginFormatManager &formatManager, const String &name) :
    formatManager(formatManager),
    instrumentName(name),
    instrumentId()
{
    this->keyboardMapping = make<KeyboardMapping>();
    this->processorGraph = make<AudioProcessorGraph>();
    this->audioCallback.setProcessor(this->processorGraph.get());
}

Instrument::~Instrument()
{
    this->audioCallback.setProcessor(nullptr);
    
    PluginWindow::closeAllCurrentlyOpenWindows();

    this->processorGraph->clear();
    this->processorGraph = nullptr;
}

String Instrument::getName() const noexcept
{
    return this->instrumentName;
}

void Instrument::setName(const String &name)
{
    this->instrumentName = name;
}

String Instrument::getInstrumentId() const noexcept
{
    return this->instrumentId.toString();
}

String Instrument::getInstrumentHash() const
{
    String idAndHash;
    const int numNodes = this->processorGraph->getNumNodes();
    
    for (int i = 0; i < numNodes; ++i)
    {
        idAndHash += this->processorGraph->getNode(i)->
            properties[Serialization::Audio::nodeHash].toString();
    }
    
    return String(constexprHash(idAndHash.toUTF8()));
}

String Instrument::getIdAndHash() const
{
    return this->getInstrumentId() + this->getInstrumentHash();
}

bool Instrument::isValid() const noexcept
{
    return this->instrumentName.isNotEmpty() &&
        this->lastValidStateFallback.isEmpty();
}

bool Instrument::isDefaultInstrument() const noexcept
{
    const auto mainNode = this->findMainPluginNode();
    if (mainNode == nullptr)
    {
        return false;
    }

    return dynamic_cast<DefaultSynthAudioPlugin *>(mainNode->getProcessor()) != nullptr;
}

bool Instrument::isMetronomeInstrument() const noexcept
{
    const auto mainNode = this->findMainPluginNode();
    if (mainNode == nullptr)
    {
        return false;
    }

    return dynamic_cast<MetronomeSynthAudioPlugin *>(mainNode->getProcessor()) != nullptr;
}

// checks if the instrument only consists of standard IO nodes,
// and if midi-in and midi-out processor nodes are present and connected
bool Instrument::isMidiOutputInstrument() const noexcept
{
    AudioProcessorGraph::Node::Ptr midiIn = nullptr;
    AudioProcessorGraph::Node::Ptr midiOut = nullptr;

    for (int i = 0; i < this->getNumNodes(); ++i)
    {
        const auto node = this->getNode(i);
        if (auto standardProcessor = dynamic_cast<AudioProcessorGraph::AudioGraphIOProcessor *>(node->getProcessor()))
        {
            if (standardProcessor->getType() == AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode)
            {
                midiIn = node;
            }
            else if (standardProcessor->getType() == AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode)
            {
                midiOut = node;
            }
        }
        else
        {
            return false; // not expected anything but standard I/O in this instrument
        }
    }

    return midiIn != nullptr && midiOut != nullptr && this->hasMidiConnection(midiIn, midiOut);
}

void Instrument::initializeMidiOutputInstrument()
{
    this->processorGraph->clear();

    InternalIODevicesPluginFormat f;

    const auto midiIn = this->addNode(*f.getDescriptionFor(
        InternalIODevicesPluginFormat::Type::midiInput), 0.2f, 0.5f);

    const auto midiOut = this->addNode(*f.getDescriptionFor(
        InternalIODevicesPluginFormat::Type::midiOutput), 0.8f, 0.5f);

    this->addConnection(midiIn->nodeID,
        Instrument::midiChannelNumber,
        midiOut->nodeID,
        Instrument::midiChannelNumber);
}

void Instrument::initializeFrom(const PluginDescription &pluginDescription, InitializationCallback initCallback)
{
    this->processorGraph->clear();

    this->addNodeAsync(pluginDescription, 0.5f, 0.5f, 
        [initCallback, this](AudioProcessorGraph::Node::Ptr instrument)
        {
            if (instrument == nullptr) { return; }

            InternalIODevicesPluginFormat f;
            const auto audioIn = this->addNode(*f.getDescriptionFor(
                InternalIODevicesPluginFormat::Type::audioInput), 0.1f, 0.15f);

            const auto audioOut = this->addNode(*f.getDescriptionFor(
                InternalIODevicesPluginFormat::Type::audioOutput), 0.9f, 0.15f);

            const auto midiIn = this->addNode(*f.getDescriptionFor(
                InternalIODevicesPluginFormat::Type::midiInput), 0.1f, 0.85f);

            const auto midiOut = this->addNode(*f.getDescriptionFor(
                InternalIODevicesPluginFormat::Type::midiOutput), 0.9f, 0.85f);

            for (int i = 0; i < instrument->getProcessor()->getTotalNumInputChannels(); ++i)
            {
                this->addConnection(audioIn->nodeID, i, instrument->nodeID, i);
            }

            if (instrument->getProcessor()->acceptsMidi())
            {
                this->addConnection(midiIn->nodeID,
                    Instrument::midiChannelNumber,
                    instrument->nodeID,
                    Instrument::midiChannelNumber);
            }

            for (int i = 0; i < instrument->getProcessor()->getTotalNumOutputChannels(); ++i)
            {
                this->addConnection(instrument->nodeID, i, audioOut->nodeID, i);
            }

            if (instrument->getProcessor()->producesMidi())
            {
                this->addConnection(instrument->nodeID,
                    Instrument::midiChannelNumber,
                    midiOut->nodeID,
                    Instrument::midiChannelNumber);
            }

            initCallback(this);
            this->sendChangeMessage();
        });
}

void Instrument::addNodeToFreeSpace(const PluginDescription &pluginDescription, InitializationCallback initCallback)
{
    Random r;
    float x = 0.15f + r.nextFloat() * 0.7f;
    float y = 0.15f + r.nextFloat() * 0.7f;

    this->addNodeAsync(pluginDescription, x, y, [initCallback, this](AudioProcessorGraph::Node::Ptr node)
    {
        if (node != nullptr)
        {
            this->lastValidStateFallback = {}; // the instrument is now valid
            initCallback(this);
            this->sendChangeMessage();
        }
    });
}


//===----------------------------------------------------------------------===//
// Nodes
//===----------------------------------------------------------------------===//

int Instrument::getNumNodes() const noexcept
{
    return this->processorGraph->getNumNodes();
}

const AudioProcessorGraph::Node::Ptr Instrument::getNode(int index) const noexcept
{
    return this->processorGraph->getNode(index);
}

const AudioProcessorGraph::Node::Ptr Instrument::getNodeForId(AudioProcessorGraph::NodeID uid) const noexcept
{
    return this->processorGraph->getNodeForId(uid);
}

bool Instrument::contains(AudioProcessorGraph::Node::Ptr node) const noexcept
{
    for (int i = 0; i < this->getNumNodes(); ++i)
    {
        if (this->getNode(i) == node)
        {
            return true;
        }
    }

    return false;
}

const AudioProcessorGraph::Node::Ptr Instrument::findMainPluginNode() const
{
    AudioProcessorGraph::Node::Ptr pluginNode = nullptr;

    for (int i = 0; i < this->getNumNodes(); ++i)
    {
        const auto node = this->getNode(i);

        if (this->isNodeStandardIOProcessor(node))
        {
            continue;
        }

        if (pluginNode == nullptr)
        {
            pluginNode = node;
        }
        else
        {
            // there are more than one plugin nodes in the instrument:
            return nullptr;
        }
    }

    return pluginNode;
}

void Instrument::addNodeAsync(const PluginDescription &desc, double x, double y, AddNodeCallback f)
{
    const auto callback = [this, desc, x, y, f]
    (UniquePointer<AudioPluginInstance> instance, const String &error)
    {
        AudioProcessorGraph::Node::Ptr node = nullptr;

        if (instance != nullptr)
        {
            node = this->processorGraph->addNode(move(instance));
        }

        if (node == nullptr)
        {
            f(nullptr);
            return;
        }

        this->configureNode(node, desc, x, y);
        f(node);
    };

    this->formatManager.createPluginInstanceAsync(desc,
        this->processorGraph->getSampleRate(),
        this->processorGraph->getBlockSize(),
        callback);
}

void Instrument::removeNode(AudioProcessorGraph::NodeID id)
{
    PluginWindow::closeCurrentlyOpenWindowsFor(id);
    this->processorGraph->removeNode(id);
    this->sendChangeMessage();
}

void Instrument::disconnectNode(AudioProcessorGraph::NodeID id)
{
    this->processorGraph->disconnectNode(id);
    this->sendChangeMessage();
}

void Instrument::removeAllConnectionsForNode(AudioProcessorGraph::Node::Ptr node)
{
    for (const auto &c : this->getConnections())
    {
        if (c.source.nodeID == node->nodeID || c.destination.nodeID == node->nodeID)
        {
            this->removeConnection(c);
        }
    }
}

void Instrument::removeIllegalConnections()
{
    this->processorGraph->removeIllegalConnections();
    this->sendChangeMessage();
}

void Instrument::setNodePosition(AudioProcessorGraph::NodeID id, double x, double y)
{
    const AudioProcessorGraph::Node::Ptr n(this->processorGraph->getNodeForId(id));

    if (n != nullptr)
    {
        n->properties.set(Serialization::UI::positionX, jlimit(0.0, 1.0, x));
        n->properties.set(Serialization::UI::positionY, jlimit(0.0, 1.0, y));
    }
}

void Instrument::getNodePosition(AudioProcessorGraph::NodeID id, double &x, double &y) const
{
    x = y = 0;
    const AudioProcessorGraph::Node::Ptr n(this->processorGraph->getNodeForId(id));
    if (n != nullptr)
    {
        x = (double) n->properties[Serialization::UI::positionX];
        y = (double) n->properties[Serialization::UI::positionY];
    }
}

bool Instrument::isNodeStandardIOProcessor(AudioProcessorGraph::NodeID nodeId) const
{
    if (const auto node = this->getNodeForId(nodeId))
    {
        return this->isNodeStandardIOProcessor(node);
    }

    return false;
}

bool Instrument::isNodeStandardIOProcessor(AudioProcessorGraph::Node::Ptr node) const
{
    return (nullptr != dynamic_cast<AudioProcessorGraph::AudioGraphIOProcessor *>(node->getProcessor()));
}

Array<AudioProcessorGraph::Node::Ptr> Instrument::findMidiAcceptors() const
{
    Array<AudioProcessorGraph::Node::Ptr> nodes;

    for (int i = 0; i < this->getNumNodes(); ++i)
    {
        const auto node = this->getNode(i);
        if (node->getProcessor()->acceptsMidi())
        {
            nodes.add(node);
        }
    }

    return nodes;
}

Array<AudioProcessorGraph::Node::Ptr> Instrument::findMidiProducers() const
{
    Array<AudioProcessorGraph::Node::Ptr> nodes;

    for (int i = 0; i < this->getNumNodes(); ++i)
    {
        const auto node = this->getNode(i);
        if (node->getProcessor()->producesMidi())
        {
            nodes.add(node);
        }
    }

    return nodes;
}

Array<AudioProcessorGraph::Node::Ptr> Instrument::findAudioAcceptors() const
{
    Array<AudioProcessorGraph::Node::Ptr> nodes;

    for (int i = 0; i < this->getNumNodes(); ++i)
    {
        const auto node = this->getNode(i);
        if (node->getProcessor()->getTotalNumInputChannels() > 0)
        {
            nodes.add(node);
        }
    }

    return nodes;
}

Array<AudioProcessorGraph::Node::Ptr> Instrument::findAudioProducers() const
{
    Array<AudioProcessorGraph::Node::Ptr> nodes;

    for (int i = 0; i < this->getNumNodes(); ++i)
    {
        const auto node = this->getNode(i);
        if (node->getProcessor()->getTotalNumOutputChannels() > 0)
        {
            nodes.add(node);
        }
    }

    return nodes;
}

bool Instrument::hasMidiConnection(AudioProcessorGraph::Node::Ptr src,
    AudioProcessorGraph::Node::Ptr dest) const noexcept
{
    jassert(src != nullptr && dest != nullptr);

    for (const auto &c : this->getConnections())
    {
        if (c.source.nodeID == src->nodeID && c.destination.nodeID == dest->nodeID &&
            c.source.channelIndex == Instrument::midiChannelNumber &&
            c.destination.channelIndex == Instrument::midiChannelNumber)
        {
            return true;
        }
    }

    return false;
}

bool Instrument::hasAudioConnection(AudioProcessorGraph::Node::Ptr src,
    AudioProcessorGraph::Node::Ptr dest) const noexcept
{
    jassert(src != nullptr && dest != nullptr);

    for (const auto &c : this->getConnections())
    {
        if (c.source.nodeID == src->nodeID && c.destination.nodeID == dest->nodeID &&
            c.source.channelIndex != Instrument::midiChannelNumber &&
            c.destination.channelIndex != Instrument::midiChannelNumber)
        {
            return true;
        }
    }

    return false;
}

bool Instrument::hasConnectionsFor(AudioProcessorGraph::Node::Ptr node) const noexcept
{
    for (const auto &c : this->getConnections())
    {
        if (c.source.nodeID == node->nodeID || c.destination.nodeID == node->nodeID)
        {
            return true;
        }
    }

    return false;
}

//===----------------------------------------------------------------------===//
// Connections
//===----------------------------------------------------------------------===//

std::vector<AudioProcessorGraph::Connection> Instrument::getConnections() const noexcept
{
    return this->processorGraph->getConnections();
}

bool Instrument::isConnected(AudioProcessorGraph::Connection connection) const noexcept
{
    return this->processorGraph->isConnected(connection);
}

bool Instrument::canConnect(AudioProcessorGraph::Connection connection) const noexcept
{
    return this->processorGraph->canConnect(connection);
}

bool Instrument::addConnection(AudioProcessorGraph::NodeID sourceID, int sourceChannel,
    AudioProcessorGraph::NodeID destinationID, int destinationChannel)
{
    AudioProcessorGraph::NodeAndChannel source;
    source.nodeID = sourceID;
    source.channelIndex = sourceChannel;

    AudioProcessorGraph::NodeAndChannel destination;
    destination.nodeID = destinationID;
    destination.channelIndex = destinationChannel;

    AudioProcessorGraph::Connection c(source, destination);
    if (this->processorGraph->addConnection(c))
    {
        this->sendChangeMessage();
        return true;
    }

    return false;
}

void Instrument::removeConnection(AudioProcessorGraph::Connection connection)
{
    this->processorGraph->removeConnection(connection);
    this->sendChangeMessage();
}

void Instrument::reset()
{
    PluginWindow::closeAllCurrentlyOpenWindows();
    this->processorGraph->clear();
    this->instrumentName.clear();
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData Instrument::serialize() const
{
    using namespace Serialization;

    if (!this->lastValidStateFallback.isEmpty())
    {
        // the instrument hasn't been loaded correctly before:
        return this->lastValidStateFallback;
    }

    SerializedData tree(Audio::instrument);
    tree.setProperty(Audio::instrumentId, this->instrumentId.toString());
    tree.setProperty(Audio::instrumentName, this->instrumentName);

    tree.appendChild(this->keyboardMapping->serialize());

    const int numNodes = this->processorGraph->getNumNodes();
    for (int i = 0; i < numNodes; ++i)
    {
        tree.appendChild(this->serializeNode(this->processorGraph->getNode(i)));
    }

    for (const auto &c : this->getConnections())
    {
        SerializedData e(Audio::connection);
        e.setProperty(Audio::sourceNodeId, static_cast<int>(c.source.nodeID.uid));
        e.setProperty(Audio::sourceChannel, c.source.channelIndex);
        e.setProperty(Audio::destinationNodeId, static_cast<int>(c.destination.nodeID.uid));
        e.setProperty(Audio::destinationChannel, c.destination.channelIndex);
        tree.appendChild(e);
    }

    return tree;
}

SerializedData Instrument::serializeNode(AudioProcessorGraph::Node::Ptr node) const
{
    if (auto *plugin = dynamic_cast<AudioPluginInstance *>(node->getProcessor()))
    {
        using namespace Serialization;

        SerializedData tree(Audio::node);
        tree.setProperty(Audio::nodeId, static_cast<int>(node->nodeID.uid));
        tree.setProperty(Audio::nodeHash, node->properties[Audio::nodeHash].toString());
        tree.setProperty(UI::positionX, node->properties[UI::positionX].toString());
        tree.setProperty(UI::positionY, node->properties[UI::positionY].toString());

        SerializablePluginDescription pd;
        plugin->fillInPluginDescription(pd);

        tree.appendChild(pd.serialize());

        MemoryBlock m;
        node->getProcessor()->getStateInformation(m);
        tree.setProperty(Serialization::Audio::pluginState, m.toBase64Encoding());

        return tree;
    }

    return {};
}

void Instrument::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization;

    const auto root = data.hasType(Audio::instrument) ?
        data : data.getChildWithName(Audio::instrument);

    if (!root.isValid() || root.getNumChildren() == 0) { return; }

    this->lastValidStateFallback = root.createCopy();

    this->instrumentId = root.getProperty(Audio::instrumentId, this->instrumentId.toString());
    this->instrumentName = root.getProperty(Audio::instrumentName, this->instrumentName);

    this->keyboardMapping->deserialize(root);

    // Fill up the connections info for further processing
    struct ConnectionDescription final
    {
        const uint32 sourceNodeId;
        const uint32 destinationNodeId;
        const int sourceChannel;
        const int destinationChannel;
    };
    
    Array<ConnectionDescription> connectionDescriptions;
    
    forEachChildWithType(root, e, Audio::connection)
    {
        const uint32 sourceNodeId = int(e.getProperty(Audio::sourceNodeId));
        const uint32 destinationNodeId = int(e.getProperty(Audio::destinationNodeId));
        connectionDescriptions.add({
            sourceNodeId,
            destinationNodeId,
            e.getProperty(Audio::sourceChannel),
            e.getProperty(Audio::destinationChannel)
        });
    }

    // we'll try to load simple plugins as early as possible, i.e. synchronously;
    // these are standard i/o nodes and built-in plugins for now, but maybe we'll
    // add more conditions later, e.g. will only load AUv3 plugins asynchronously?
    Array<SerializedData> nodesToDeserializeAsync;
    int numNodesInDescription = 0;
    forEachChildWithType(root, nodeState, Serialization::Audio::node)
    {
        SerializablePluginDescription desc;
        desc.deserialize(nodeState.getChild(0)); // "node"/"plugin"

        numNodesInDescription++;

        if (desc.pluginFormatName == BuiltInSynthsPluginFormat::formatName ||
            desc.pluginFormatName == InternalIODevicesPluginFormat::formatName)
        {
            String error;
            auto instance = this->formatManager.createPluginInstance(desc,
                this->processorGraph->getSampleRate(),
                this->processorGraph->getBlockSize(),
                error);

            AudioProcessorGraph::Node::Ptr node = nullptr;
            if (error.isEmpty())
            {
                node = this->addNode(move(instance), nodeState);
            }

            if (node == nullptr)
            {
                // failed for some reason, try non-blocking loading:
                nodesToDeserializeAsync.add(nodeState);
            }
        }
        else
        {
            nodesToDeserializeAsync.add(nodeState);
        }
    }

    this->deserializeNodesAsync(nodesToDeserializeAsync,
        [this, numNodesInDescription, connectionDescriptions]()
    {
        for (const auto &connectionInfo : connectionDescriptions)
        {
            this->addConnection(AudioProcessorGraph::NodeID(connectionInfo.sourceNodeId),
                connectionInfo.sourceChannel,
                AudioProcessorGraph::NodeID(connectionInfo.destinationNodeId),
                connectionInfo.destinationChannel);
        }

        this->processorGraph->removeIllegalConnections();

        // here we want to test if we have the instrument loaded correctly, and,
        // if not, remember the serialized data, so we can save it later,
        // instead of a messed up state we'll have on deserialization failure
        // (because of, e.g. user switching between 64-bit and 32-bit versions);
        // the simplest way to test that is to check if the processor graph
        // now contains the same number of nodes as the description:
        if (this->getNumNodes() == numNodesInDescription)
        {
            this->lastValidStateFallback = {};
        }

        this->sendChangeMessage();
    });
}

void Instrument::deserializeNodesAsync(Array<SerializedData> nodesToDeserialize,
    DeserializeNodesCallback allDoneCallback)
{
    using namespace Serialization;
    if (nodesToDeserialize.isEmpty())
    {
        allDoneCallback();
        return;
    }

    const auto tree = nodesToDeserialize.removeAndReturn(0);

    SerializablePluginDescription desc;
    desc.deserialize(tree.getChild(0)); // "node"/"plugin"
    
    const auto callback = [this, nodesToDeserialize, tree, allDoneCallback]
    (UniquePointer<AudioPluginInstance> instance, const String &error)
    {
        this->addNode(move(instance), tree);
        this->deserializeNodesAsync(nodesToDeserialize, allDoneCallback);
    };

    this->formatManager.createPluginInstanceAsync(desc,
        this->processorGraph->getSampleRate(),
        this->processorGraph->getBlockSize(),
        callback);
}

AudioProcessorGraph::Node::Ptr Instrument::addNode(const PluginDescription &desc, double x, double y)
{
    String errorMessage;
    auto instance = this->formatManager.createPluginInstance(desc,
        this->processorGraph->getSampleRate(),
        this->processorGraph->getBlockSize(),
        errorMessage);

    AudioProcessorGraph::Node::Ptr node = nullptr;
    
    if (instance != nullptr)
    {
        node = this->processorGraph->addNode(move(instance));
    }
    
    if (node != nullptr)
    {
        this->configureNode(node, desc, x, y);
        this->sendChangeMessage();
        return node;
    }
    
    return nullptr;
}

AudioProcessorGraph::Node::Ptr Instrument::addNode(UniquePointer<AudioPluginInstance> instance,
    const SerializedData &data)
{
    if (instance == nullptr)
    {
        return nullptr;
    }

    using namespace Serialization;

    MemoryBlock nodeStateBlock;
    const String state = data.getProperty(Audio::pluginState);
    if (state.isNotEmpty())
    {
        nodeStateBlock.fromBase64Encoding(state);
    }

    const uint32 nodeUid = int(data.getProperty(Audio::nodeId));
    const String nodeHash = data.getProperty(Audio::nodeHash);
    const double nodeX = data.getProperty(UI::positionX);
    const double nodeY = data.getProperty(UI::positionY);

    AudioProcessorGraph::NodeID nodeId(nodeUid);
    AudioProcessorGraph::Node::Ptr node(this->processorGraph->addNode(move(instance), nodeId));

    if (node == nullptr)
    {
        return nullptr;
    }

    if (nodeStateBlock.getSize() > 0)
    {
        node->getProcessor()->
            setStateInformation(nodeStateBlock.getData(),
                static_cast<int>(nodeStateBlock.getSize()));
    }

    Uuid fallbackRandomHash;
    const auto hash = nodeHash.isNotEmpty() ? nodeHash : fallbackRandomHash.toString();
    node->properties.set(Audio::nodeHash, hash);
    node->properties.set(UI::positionX, nodeX);
    node->properties.set(UI::positionY, nodeY);

    return node;
}

void Instrument::configureNode(AudioProcessorGraph::Node::Ptr node,
    const PluginDescription &desc, double x, double y)
{
    // make a hash from a general instrument description
    const String descriptionString = (desc.name +
                                      desc.category +
                                      desc.descriptiveName +
                                      desc.manufacturerName +
                                      desc.pluginFormatName +
                                      String(desc.numInputChannels) +
                                      String(desc.numOutputChannels));
    
    const String nodeHash = String(constexprHash(descriptionString.toUTF8()));
    
    node->properties.set(Serialization::Audio::nodeHash, nodeHash);
    node->properties.set(Serialization::UI::positionX, x);
    node->properties.set(Serialization::UI::positionY, y);
}

void Instrument::AudioCallback::setProcessor(AudioProcessor *const newOne)
{
    if (this->processor != newOne)
    {
        if (newOne != nullptr && this->sampleRate > 0 && this->blockSize > 0)
        {
            newOne->setPlayConfigDetails(this->numInputChans, this->numOutputChans, this->sampleRate, this->blockSize);
            newOne->setProcessingPrecision(AudioProcessor::singlePrecision);
            newOne->prepareToPlay(this->sampleRate, this->blockSize);
        }

        AudioProcessor *oldOne;

        {
            const ScopedLock sl(this->lock);
            oldOne = this->isPrepared ? this->processor : nullptr;
            this->processor = newOne;
            this->isPrepared = true;
        }

        if (oldOne != nullptr)
        {
            oldOne->releaseResources();
        }
    }
}

void Instrument::AudioCallback::audioDeviceIOCallback(const float** const inputChannelData,
    const int numInputChannels, float **const outputChannelData,
    const int numOutputChannels, const int numSamples)
{
    jassert(this->sampleRate > 0 && this->blockSize > 0);

    this->incomingMidi.clear();
    this->messageCollector.removeNextBlockOfMessages(this->incomingMidi, numSamples);
    int totalNumChans = 0;

    if (numInputChannels > numOutputChannels)
    {
        this->tempBuffer.setSize(numInputChannels - numOutputChannels, numSamples, false, false, true);

        for (int i = 0; i < numOutputChannels; ++i)
        {
            this->channels[totalNumChans] = outputChannelData[i];
            memcpy(this->channels[totalNumChans], inputChannelData[i], sizeof(float) * (size_t)numSamples);
            ++totalNumChans;
        }

        for (int i = numOutputChannels; i < numInputChannels; ++i)
        {
            this->channels[totalNumChans] = this->tempBuffer.getWritePointer(i - numOutputChannels);
            memcpy(this->channels[totalNumChans], inputChannelData[i], sizeof(float) * (size_t)numSamples);
            ++totalNumChans;
        }
    }
    else
    {
        for (int i = 0; i < numInputChannels; ++i)
        {
            this->channels[totalNumChans] = outputChannelData[i];
            memcpy(this->channels[totalNumChans], inputChannelData[i], sizeof(float) * (size_t)numSamples);
            ++totalNumChans;
        }

        for (int i = numInputChannels; i < numOutputChannels; ++i)
        {
            this->channels[totalNumChans] = outputChannelData[i];
            zeromem(this->channels[totalNumChans], sizeof(float) * (size_t)numSamples);
            ++totalNumChans;
        }
    }

    AudioBuffer<float> buffer(this->channels, totalNumChans, numSamples);

    {
        const ScopedLock sl(this->lock);

        if (this->processor != nullptr)
        {
            const ScopedLock sl2(this->processor->getCallbackLock());

            if (!this->processor->isSuspended())
            {
                this->processor->processBlock(buffer, this->incomingMidi);

                // if the MIDI message buffer is not empty here,
                // the processor wants to send events to MIDI output:
                for (const auto &metadata : this->incomingMidi)
                {
                    const auto message = metadata.getMessage();

                    // we'll filter out meta events, because some instruments
                    // may misinterpret them as random notes/controllers:
                    if (!message.isMetaEvent())
                    {
                        App::Workspace().getAudioCore().sendMessageToMidiOutputNow(message);
                    }
                }

                return;
            }
        }
    }

    for (int i = 0; i < numOutputChannels; ++i)
    {
        FloatVectorOperations::clear(outputChannelData[i], numSamples);
    }
}

void Instrument::AudioCallback::audioDeviceAboutToStart(AudioIODevice *const device)
{
    const auto newSampleRate = device->getCurrentSampleRate();
    const auto newBlockSize = device->getCurrentBufferSizeSamples();
    const auto numChansIn = device->getActiveInputChannels().countNumberOfSetBits();
    const auto numChansOut = device->getActiveOutputChannels().countNumberOfSetBits();

    const ScopedLock sl(this->lock);

    this->sampleRate = newSampleRate;
    this->blockSize = newBlockSize;
    this->numInputChans = numChansIn;
    this->numOutputChans = numChansOut;

    this->messageCollector.reset(sampleRate);
    this->channels.calloc(jmax(numChansIn, numChansOut) + 2);

    if (this->processor != nullptr)
    {
        if (this->isPrepared)
        {
            this->processor->releaseResources();
        }

        auto *oldProcessor = this->processor;
        this->setProcessor(nullptr);
        this->setProcessor(oldProcessor);
    }
}

void Instrument::AudioCallback::audioDeviceStopped()
{
    const ScopedLock sl(this->lock);

    if (this->processor != nullptr && this->isPrepared)
    {
        this->processor->releaseResources();
    }

    this->sampleRate = 0.0;
    this->blockSize = 0;
    this->isPrepared = false;
    this->tempBuffer.setSize(1, 1);
}

void Instrument::AudioCallback::handleIncomingMidiMessage(MidiInput *, const MidiMessage &message)
{
    this->messageCollector.addMessageToQueue(message);
}
