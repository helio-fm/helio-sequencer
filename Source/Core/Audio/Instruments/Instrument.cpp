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
#include "Instrument.h"
#include "PluginWindow.h"
#include "InternalPluginFormat.h"
#include "PluginSmartDescription.h"
#include "SerializationKeys.h"

const int Instrument::midiChannelNumber = 0x1000;

Instrument::Instrument(AudioPluginFormatManager &formatManager, String name) :
    formatManager(formatManager),
    instrumentName(std::move(name)),
    lastUID(0),
    instrumentID()
{
    this->processorGraph = new AudioProcessorGraph();
    this->initializeDefaultNodes();
    this->processorPlayer.setProcessor(this->processorGraph);
}

Instrument::~Instrument()
{
    this->masterReference.clear();
    this->processorPlayer.setProcessor(nullptr);
    
    PluginWindow::closeAllCurrentlyOpenWindows();
    this->processorGraph->clear();
    this->processorGraph = nullptr;
}


String Instrument::getName() const
{
    return this->instrumentName;
}

void Instrument::setName(const String &name)
{
    this->instrumentName = name;
}

String Instrument::getInstrumentID() const
{
    return this->instrumentID.toString();
}

String Instrument::getInstrumentHash() const
{
    // для одного и того же инструмента на разных платформах этот хэш будет одинаковым
    // но если создать два инструмента с одним и тем же плагином - хэш тоже будет одинаковым
    // поэтому в слое мы храним id и хэш
    
    //const double t1 = Time::getMillisecondCounterHiRes();
    
    String iID;
    const int numNodes = this->processorGraph->getNumNodes();
    
    for (int i = 0; i < numNodes; ++i)
    {
        const String &nodeHash = this->processorGraph->getNode(i)->properties["hash"].toString();
        iID += nodeHash;
    }
    
    const String &hash = MD5(iID.toUTF8()).toHexString();
    
    //const double t2 = Time::getMillisecondCounterHiRes();
    //Logger::writeToLog(String(t2 - t1));
    
    return hash;
}

String Instrument::getIdAndHash() const
{
    return this->getInstrumentID() + this->getInstrumentHash();
}


void Instrument::initializeFrom(const PluginDescription &pluginDescription)
{
    this->processorGraph->clear();
    this->initializeDefaultNodes();
    
    this->addNodeAsync(pluginDescription, 0.5f, 0.5f, 
        [&](AudioProcessorGraph::Node::Ptr instrument)
        {
            if (instrument == nullptr) { return; }
                           
            // ограничить 2мя?
            for (int i = 0; i < instrument->getProcessor()->getTotalNumInputChannels(); ++i)
            {
                this->addConnection(this->audioIn->nodeID, i, instrument->nodeID, i);
            }
                           
            if (instrument->getProcessor()->acceptsMidi())
            {
                this->addConnection(this->midiIn->nodeID, Instrument::midiChannelNumber, instrument->nodeID, Instrument::midiChannelNumber);
            }
                           
            for (int i = 0; i < instrument->getProcessor()->getTotalNumOutputChannels(); ++i)
            {
                this->addConnection(instrument->nodeID, i, this->audioOut->nodeID, i);
            }
                           
            if (instrument->getProcessor()->producesMidi())
            {
                this->addConnection(instrument->nodeID, Instrument::midiChannelNumber, this->midiOut->nodeID, Instrument::midiChannelNumber);
            }
                           
            this->sendChangeMessage();
        });
}

void Instrument::addNodeToFreeSpace(const PluginDescription &pluginDescription)
{
    // TODO: calc free space
    float x = 0.5f;
    float y = 0.5f;

    this->addNodeAsync(pluginDescription, x, y, [this](AudioProcessorGraph::Node::Ptr node)
    {
        if (node != nullptr)
        {
            this->sendChangeMessage();
        }
    });
}

AudioProcessorGraph::NodeID Instrument::getNextUID() noexcept
{
    return ++lastUID;
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

void Instrument::addNodeAsync(const PluginDescription &desc,
    double x, double y,
    Function<void (AudioProcessorGraph::Node::Ptr)> f)
{
    this->formatManager.
    createPluginInstanceAsync(desc,
        this->processorGraph->getSampleRate(),
        this->processorGraph->getBlockSize(),
        [this, desc, x, y, f](AudioPluginInstance *instance, const String &error)
    {
        AudioProcessorGraph::Node::Ptr node = nullptr;

        if (instance != nullptr)
        {
            node = this->processorGraph->addNode(instance);
        }

        if (node == nullptr)
        {
            f(nullptr);
            return;
        }

        this->configureNode(node, desc, x, y);
        this->sendChangeMessage();

        f(node);
    });
}

AudioProcessorGraph::Node::Ptr Instrument::addNode(Instrument *instrument, double x, double y)
{
    AudioProcessorGraph::Node::Ptr node =
        this->processorGraph->addNode(instrument->getProcessorGraph());
    
    if (node != nullptr)
    {
        node->properties.set("x", x);
        node->properties.set("y", y);
        this->sendChangeMessage();
    }

    return node;
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
        n->properties.set("x", jlimit(0.0, 1.0, x));
        n->properties.set("y", jlimit(0.0, 1.0, y));
    }
}

void Instrument::getNodePosition(AudioProcessorGraph::NodeID id, double &x, double &y) const
{
    x = y = 0;

    const AudioProcessorGraph::Node::Ptr n(this->processorGraph->getNodeForId(id));

    if (n != nullptr)
    {
        x = (double) n->properties ["x"];
        y = (double) n->properties ["y"];
    }
}


//===----------------------------------------------------------------------===//
// Default nodes' id's
//===----------------------------------------------------------------------===//

AudioProcessorGraph::NodeID Instrument::getMidiInId() const
{
    jassert(this->midiIn);
    return this->midiIn->nodeID;
}

AudioProcessorGraph::NodeID Instrument::getMidiOutId() const
{
    jassert(this->midiOut);
    return this->midiOut->nodeID;
}

AudioProcessorGraph::NodeID Instrument::getAudioInId() const
{
    jassert(this->audioIn);
    return this->audioIn->nodeID;
}

AudioProcessorGraph::NodeID Instrument::getAudioOutId() const
{
    jassert(this->audioOut);
    return this->audioOut->nodeID;
}

bool Instrument::isNodeStandardInputOrOutput(AudioProcessorGraph::NodeID nodeId) const
{
    return nodeId == this->getMidiInId()
        || nodeId == this->getMidiOutId()
        || nodeId == this->getAudioInId()
        || nodeId == this->getAudioOutId();
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

bool Instrument::addConnection(
    AudioProcessorGraph::NodeID sourceID,
    int sourceChannel,
    AudioProcessorGraph::NodeID destinationID,
    int destinationChannel)
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
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree Instrument::serialize() const
{
    ValueTree tree(Serialization::Core::instrument);
    tree.setProperty(Serialization::Core::instrumentId, this->instrumentID.toString());
    tree.setProperty(Serialization::Core::instrumentName, this->instrumentName);

    const int numNodes = this->processorGraph->getNumNodes();
    for (int i = 0; i < numNodes; ++i)
    {
        tree.appendChild(this->serializeNode(this->processorGraph->getNode(i)));
    }

    for (const auto &c : this->getConnections())
    {
        ValueTree e(Serialization::Core::instrumentConnection);
        e.setProperty("srcFilter", static_cast<int>(c.source.nodeID));
        e.setProperty("srcChannel", c.source.channelIndex);
        e.setProperty("dstFilter", static_cast<int>(c.destination.nodeID));
        e.setProperty("dstChannel", c.destination.channelIndex);
        tree.appendChild(e);
    }

    return tree;
}

void Instrument::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root = tree.hasType(Serialization::Core::instrument) ?
        tree : tree.getChildWithName(Serialization::Core::instrument);

    if (!root.isValid())
    { return; }

    this->instrumentID = root.getProperty(Serialization::Core::instrumentId, this->instrumentID.toString());
    this->instrumentName = root.getProperty(Serialization::Core::instrumentName, this->instrumentName);

    // Well this hack of an incredible ugliness
    // is here to handle loading of async-loaded AUv3 plugins
    
    // Fill up the connections info for further processing
    struct ConnectionDescription
    {
        uint32 srcFilter;
        uint32 dstFilter;
        int srcChannel;
        int dstChannel;
    };
    
    Array<ConnectionDescription> connectionDescriptions;
    
    forEachValueTreeChildWithType(root, e, Serialization::Core::instrumentConnection)
    {
        connectionDescriptions.add({
            static_cast<int>(e.getProperty("srcFilter")),
            static_cast<int>(e.getProperty("dstFilter")),
            e.getProperty("srcChannel"),
            e.getProperty("dstChannel")
        });
    }
    
    forEachValueTreeChildWithType(root, e, Serialization::Core::instrumentNode)
    {
        this->deserializeNodeAsync(e,
            [this, connectionDescriptions](AudioProcessorGraph::Node::Ptr)
            {
                // Try to create as many connections as possible
                for (const auto &connectionInfo : connectionDescriptions)
                {
                    this->addConnection(connectionInfo.srcFilter, connectionInfo.srcChannel,
                                        connectionInfo.dstFilter, connectionInfo.dstChannel);
                }
                                         
                this->processorGraph->removeIllegalConnections();
                this->sendChangeMessage();
            });
    }
}

ValueTree Instrument::serializeNode(AudioProcessorGraph::Node::Ptr node) const
{
    if (AudioPluginInstance *plugin = dynamic_cast<AudioPluginInstance *>(node->getProcessor()))
    {
        ValueTree tree(Serialization::Core::instrumentNode);
        tree.setProperty("uid", static_cast<int>(node->nodeID));
        tree.setProperty("x", node->properties["x"].toString());
        tree.setProperty("y", node->properties["y"].toString());
        tree.setProperty("hash", node->properties["hash"].toString());
        tree.setProperty("uiLastX", node->properties["uiLastX"].toString());
        tree.setProperty("uiLastY", node->properties["uiLastY"].toString());

        PluginSmartDescription pd;
        plugin->fillInPluginDescription(pd);

        tree.appendChild(pd.serialize());

        ValueTree state(Serialization::Core::pluginState);

        MemoryBlock m;
        node->getProcessor()->getStateInformation(m);
        state->addTextElement(m.toBase64Encoding());
        tree.appendChild(state);

        return tree;
    }
    
    return {};
}

void Instrument::deserializeNodeAsync(const ValueTree &tree,
    Function<void (AudioProcessorGraph::Node::Ptr)> f)
{
    PluginSmartDescription pd;
    for (const auto &e : tree)
    {
        pd.deserialize(e);
        if (pd.isValid()) { break; }
    }
    
    MemoryBlock nodeStateBlock;
    const auto state = tree.getChildWithName(Serialization::Core::pluginState);
    if (state.isValid())
    {
        nodeStateBlock.fromBase64Encoding(state->getAllSubText());
    }
    
    const int nodeUid = tree.getProperty("uid");
    const String nodeHash = tree.getProperty("hash");
    const double nodeX = tree.getProperty("x");
    const double nodeY = tree.getProperty("y");
    const double nodeLastX = tree.getProperty("uiLastX");
    const double nodeLastY = tree.getProperty("uiLastY");
    
    formatManager.
    createPluginInstanceAsync(pd,
        this->processorGraph->getSampleRate(),
        this->processorGraph->getBlockSize(),
        [this, nodeStateBlock, nodeUid, nodeHash, nodeX, nodeY, nodeLastX, nodeLastY, f]
        (AudioPluginInstance *instance, const String &error)
        {
            if (instance == nullptr)
            {
                f(nullptr);
                return;
            }

            AudioProcessorGraph::Node::Ptr node(this->processorGraph->addNode(instance, nodeUid));

            if (nodeStateBlock.getSize() > 0)
            {
                node->getProcessor()->
                    setStateInformation(nodeStateBlock.getData(),
                        static_cast<int>(nodeStateBlock.getSize()));
            }

            Uuid fallbackRandomHash;
            node->properties.set("x", nodeX);
            node->properties.set("y", nodeY);
            node->properties.set("hash", nodeHash.isNotEmpty() ? nodeHash : fallbackRandomHash.toString());
            node->properties.set("uiLastX", nodeLastX);
            node->properties.set("uiLastY", nodeLastY);

            f(node);
        });
}

void Instrument::deserializeNode(const ValueTree &tree)
{
    PluginSmartDescription pd;
    for (const auto &e : tree)
    {
        pd.deserialize(e);
        if (pd.isValid()) { break; }
    }
    
    String errorMessage;

    AudioPluginInstance *instance =
        formatManager.createPluginInstance(pd,
                                           this->processorGraph->getSampleRate(),
                                           this->processorGraph->getBlockSize(),
                                           errorMessage);

    if (instance == nullptr)
    {
        return;
    }

    const int nodeUid = tree.getProperty("uid");
    AudioProcessorGraph::Node::Ptr node(this->processorGraph->addNode(instance, nodeUid));

    const auto state = tree.getChildWithName(Serialization::Core::pluginState);
    if (state.isValid())
    {
        MemoryBlock m;
        m.fromBase64Encoding(state->getAllSubText());
        node->getProcessor()->setStateInformation(m.getData(), static_cast<int>( m.getSize()));
    }

    const String &hash = tree.getProperty("hash");
    Uuid fallbackRandomHash;
    
    node->properties.set("x", tree.getProperty("x"));
    node->properties.set("y", tree.getProperty("y"));
    node->properties.set("hash", hash.isNotEmpty() ? hash : fallbackRandomHash.toString());
    node->properties.set("uiLastX", tree.getProperty("uiLastX"));
    node->properties.set("uiLastY", tree.getProperty("uiLastY"));
}

void Instrument::initializeDefaultNodes()
{
    InternalPluginFormat internalFormat;
    this->audioIn = this->addDefaultNode(*internalFormat.getDescriptionFor(InternalPluginFormat::audioInputFilter), 0.1f, 0.15f);
    this->midiIn = this->addDefaultNode(*internalFormat.getDescriptionFor(InternalPluginFormat::midiInputFilter), 0.1f, 0.85f);
    this->audioOut = this->addDefaultNode(*internalFormat.getDescriptionFor(InternalPluginFormat::audioOutputFilter), 0.9f, 0.15f);
    this->midiOut = this->addDefaultNode(*internalFormat.getDescriptionFor(InternalPluginFormat::midiOutputFilter), 0.9f, 0.85f);
}

AudioProcessorGraph::Node::Ptr Instrument::addDefaultNode(
    const PluginDescription &desc, double x, double y)
{
    String errorMessage;
    AudioPluginInstance *instance =
    formatManager.createPluginInstance(desc,
        this->processorGraph->getSampleRate(),
        this->processorGraph->getBlockSize(),
        errorMessage);
    
    AudioProcessorGraph::Node::Ptr node = nullptr;
    
    if (instance != nullptr)
    {
        node = this->processorGraph->addNode(instance);
    }
    
    if (node != nullptr)
    {
        this->configureNode(node, desc, x, y);
        this->sendChangeMessage();
        return node;
    }
    
    
    return nullptr;
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
                                      // version dependency is evil
                                      //desc.version +
                                      String(desc.numInputChannels) +
                                      String(desc.numOutputChannels));
    
    const String nodeHash = MD5(descriptionString.toUTF8()).toHexString();
    
    node->properties.set("hash", nodeHash);
    node->properties.set("x", x);
    node->properties.set("y", y);
}
