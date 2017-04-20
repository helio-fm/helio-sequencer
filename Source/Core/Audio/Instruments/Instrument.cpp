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
    
    this->addNodeAsync(pluginDescription, 0.5f, 0.5f, [&](AudioProcessorGraph::Node *instrument)
                       {
                           if (instrument == nullptr) { return; }
                           
                           // ограничить 2мя?
                           for (int i = 0; i < instrument->getProcessor()->getTotalNumInputChannels(); ++i)
                           {
                               this->addConnection(this->audioIn->nodeId, i, instrument->nodeId, i);
                           }
                           
                           if (instrument->getProcessor()->acceptsMidi())
                           {
                               this->addConnection(this->midiIn->nodeId, Instrument::midiChannelNumber, instrument->nodeId, Instrument::midiChannelNumber);
                           }
                           
                           for (int i = 0; i < instrument->getProcessor()->getTotalNumOutputChannels(); ++i)
                           {
                               this->addConnection(instrument->nodeId, i, this->audioOut->nodeId, i);
                           }
                           
                           if (instrument->getProcessor()->producesMidi())
                           {
                               this->addConnection(instrument->nodeId, Instrument::midiChannelNumber, this->midiOut->nodeId, Instrument::midiChannelNumber);
                           }
                           
                           this->sendChangeMessage();
                       });
}

void Instrument::addNodeToFreeSpace(const PluginDescription &pluginDescription)
{
    // TODO: calc free space
    float x = 0.5f;
    float y = 0.5f;

    this->addNodeAsync(pluginDescription, x, y, [this](AudioProcessorGraph::Node *node)
    {
        if (node != nullptr)
        {
            this->sendChangeMessage();
        }
    });
}


uint32 Instrument::getNextUID() noexcept
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

const AudioProcessorGraph::Node::Ptr Instrument::getNode(const int index) const noexcept
{
    return this->processorGraph->getNode(index);
}

const AudioProcessorGraph::Node::Ptr Instrument::getNodeForId(const uint32 uid) const noexcept
{
    return this->processorGraph->getNodeForId(uid);
}

void Instrument::addNodeAsync(const PluginDescription &desc,
                              double x, double y,
                              std::function<void (AudioProcessorGraph::Node *)> f)
{
    this->formatManager.
    createPluginInstanceAsync(desc,
                              this->processorGraph->getSampleRate(),
                              this->processorGraph->getBlockSize(),
                              [this, desc, x, y, f](AudioPluginInstance *instance, const String &error)
                              {
                                  AudioProcessorGraph::Node *node = nullptr;
                                  
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

AudioProcessorGraph::Node *Instrument::addNode(Instrument *instrument, double x, double y)
{
    AudioProcessorGraph::Node *node = this->processorGraph->addNode(instrument->getProcessorGraph());
    
    if (node != nullptr)
    {
        node->properties.set("x", x);
        node->properties.set("y", y);
        this->sendChangeMessage();
    }

    return node;
}

void Instrument::removeNode(const uint32 id)
{
    PluginWindow::closeCurrentlyOpenWindowsFor(id);
    this->processorGraph->removeNode(id);
    this->sendChangeMessage();
}

void Instrument::disconnectNode(const uint32 id)
{
    this->processorGraph->disconnectNode(id);
    this->sendChangeMessage();
}

void Instrument::removeIllegalConnections()
{
    this->processorGraph->removeIllegalConnections();
    this->sendChangeMessage();
}

void Instrument::setNodePosition(const int nodeId, double x, double y)
{
    const AudioProcessorGraph::Node::Ptr n(this->processorGraph->getNodeForId(nodeId));

    if (n != nullptr)
    {
        n->properties.set("x", jlimit(0.0, 1.0, x));
        n->properties.set("y", jlimit(0.0, 1.0, y));
    }
}

void Instrument::getNodePosition(const int nodeId, double &x, double &y) const
{
    x = y = 0;

    const AudioProcessorGraph::Node::Ptr n(this->processorGraph->getNodeForId(nodeId));

    if (n != nullptr)
    {
        x = (double) n->properties ["x"];
        y = (double) n->properties ["y"];
    }
}


//===----------------------------------------------------------------------===//
// Default nodes' id's
//===----------------------------------------------------------------------===//

uint32 Instrument::getMidiInId() const
{
    jassert(this->midiIn);
    return this->midiIn->nodeId;
}

uint32 Instrument::getMidiOutId() const
{
    jassert(this->midiOut);
    return this->midiOut->nodeId;
}

uint32 Instrument::getAudioInId() const
{
    jassert(this->audioIn);
    return this->audioIn->nodeId;
}

uint32 Instrument::getAudioOutId() const
{
    jassert(this->audioOut);
    return this->audioOut->nodeId;
}

bool Instrument::isNodeStandardInputOrOutput(uint32 nodeId) const
{
    return nodeId == this->getMidiInId()
        || nodeId == this->getMidiOutId()
        || nodeId == this->getAudioInId()
        || nodeId == this->getAudioOutId();
}


//===----------------------------------------------------------------------===//
// Connections
//===----------------------------------------------------------------------===//

int Instrument::getNumConnections() const noexcept
{
    return this->processorGraph->getNumConnections();
}

const AudioProcessorGraph::Connection *Instrument::getConnection(const int index) const noexcept
{
    return this->processorGraph->getConnection(index);
}

const AudioProcessorGraph::Connection *Instrument::getConnectionBetween(
    uint32 sourceFilterUID, int sourceFilterChannel,
    uint32 destFilterUID, int destFilterChannel) const noexcept
{
    return this->processorGraph->getConnectionBetween(sourceFilterUID,
            sourceFilterChannel, destFilterUID, destFilterChannel);
}

bool Instrument::canConnect(uint32 sourceFilterUID, int sourceFilterChannel,
                            uint32 destFilterUID, int destFilterChannel) const noexcept
{
    return this->processorGraph->canConnect(sourceFilterUID, sourceFilterChannel,
                                           destFilterUID, destFilterChannel);
}

bool Instrument::addConnection(uint32 sourceFilterUID, int sourceFilterChannel,
                               uint32 destFilterUID, int destFilterChannel)
{
    const bool result =
        this->processorGraph->addConnection(sourceFilterUID, sourceFilterChannel,
                                            destFilterUID, destFilterChannel);

    if (result)
    { this->sendChangeMessage(); }

    return result;
}

void Instrument::removeConnection(const int index)
{
    this->processorGraph->removeConnection(index);
    this->sendChangeMessage();
}

void Instrument::removeConnection(uint32 sourceFilterUID, int sourceFilterChannel,
                                  uint32 destFilterUID, int destFilterChannel)
{
    this->processorGraph->removeConnection(sourceFilterUID, sourceFilterChannel,
                                          destFilterUID, destFilterChannel);
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

XmlElement *Instrument::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::instrument);
    xml->setAttribute(Serialization::Core::instrumentId, this->instrumentID.toString());
    xml->setAttribute(Serialization::Core::instrumentName, this->instrumentName);

    const int numNodes = this->processorGraph->getNumNodes();
    for (int i = 0; i < numNodes; ++i)
    {
        xml->addChildElement(this->createNodeXml(this->processorGraph->getNode(i)));
    }

    const int numConnections = this->processorGraph->getNumConnections();
    for (int i = 0; i < numConnections; ++i)
    {
        const AudioProcessorGraph::Connection *const fc = this->processorGraph->getConnection(i);

        auto e = new XmlElement(Serialization::Core::instrumentConnection);
        e->setAttribute("srcFilter", static_cast<int>( fc->sourceNodeId));
        e->setAttribute("srcChannel", fc->sourceChannelIndex);
        e->setAttribute("dstFilter", static_cast<int>( fc->destNodeId));
        e->setAttribute("dstChannel", fc->destChannelIndex);

        xml->addChildElement(e);
    }

    return xml;
}

void Instrument::deserialize(const XmlElement &xml)
{
    this->reset();

    const XmlElement *mainSlot = (xml.getTagName() == Serialization::Core::instrument) ?
                                 &xml : xml.getChildByName(Serialization::Core::instrument);

    if (mainSlot == nullptr)
    { return; }

    this->instrumentID = mainSlot->getStringAttribute(Serialization::Core::instrumentId, this->instrumentID.toString());
    this->instrumentName = mainSlot->getStringAttribute(Serialization::Core::instrumentName, this->instrumentName);

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
    
    forEachXmlChildElementWithTagName(*mainSlot, e, Serialization::Core::instrumentConnection)
    {
        connectionDescriptions.add({
            static_cast<uint32>(e->getIntAttribute("srcFilter")),
            static_cast<uint32>(e->getIntAttribute("dstFilter")),
            e->getIntAttribute("srcChannel"),
            e->getIntAttribute("dstChannel")
        });
    }
    
    forEachXmlChildElementWithTagName(*mainSlot, e, Serialization::Core::instrumentNode)
    {
        this->createNodeFromXmlAsync(*e, [this, connectionDescriptions](AudioProcessorGraph::Node *)
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

//    forEachXmlChildElementWithTagName(*mainSlot, e, Serialization::Core::instrumentConnection)
//    {
//        this->addConnection(static_cast<uint32>( e->getIntAttribute("srcFilter")),
//                            e->getIntAttribute("srcChannel"),
//                            static_cast<uint32>( e->getIntAttribute("dstFilter")),
//                            e->getIntAttribute("dstChannel"));
//    }
//
//    this->processorGraph->removeIllegalConnections();
//
//    this->sendChangeMessage();
}

XmlElement *Instrument::createNodeXml(AudioProcessorGraph::Node *const node) const
{
    if (AudioPluginInstance *plugin = dynamic_cast<AudioPluginInstance *>(node->getProcessor()))
    {
        auto e = new XmlElement(Serialization::Core::instrumentNode);
        e->setAttribute("uid", static_cast<int>(node->nodeId));
        e->setAttribute("x", node->properties["x"].toString());
        e->setAttribute("y", node->properties["y"].toString());
        e->setAttribute("hash", node->properties["hash"].toString());
        e->setAttribute("uiLastX", node->properties["uiLastX"].toString());
        e->setAttribute("uiLastY", node->properties["uiLastY"].toString());

        PluginSmartDescription pd;
        plugin->fillInPluginDescription(pd);

        e->addChildElement(pd.createXml());

        auto state = new XmlElement(Serialization::Core::pluginState);

        MemoryBlock m;
        node->getProcessor()->getStateInformation(m);
        state->addTextElement(m.toBase64Encoding());
        e->addChildElement(state);

        return e;
    }
    
    return nullptr;
}

void Instrument::createNodeFromXmlAsync(const XmlElement &xml,
                                        std::function<void (AudioProcessorGraph::Node *)> f)
{
    PluginSmartDescription pd;
    
    forEachXmlChildElement(xml, e)
    {
        if (pd.loadFromXml(*e))
        { break; }
    }
    
    MemoryBlock nodeStateBlock;
    const XmlElement *const state = xml.getChildByName(Serialization::Core::pluginState);
    if (state != nullptr)
    {
        nodeStateBlock.fromBase64Encoding(state->getAllSubText());
    }
    
    const uint32 nodeUid = xml.getIntAttribute("uid");
    const String nodeHash = xml.getStringAttribute("hash");
    const double nodeX = xml.getDoubleAttribute("x");
    const double nodeY = xml.getDoubleAttribute("y");
    const double nodeLastX = xml.getDoubleAttribute("uiLastX");
    const double nodeLastY = xml.getDoubleAttribute("uiLastY");
    
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

void Instrument::createNodeFromXml(const XmlElement &xml)
{
    PluginSmartDescription pd;

    forEachXmlChildElement(xml, e)
    {
        if (pd.loadFromXml(*e))
        { break; }
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

    AudioProcessorGraph::Node::Ptr node(this->processorGraph->addNode(instance, xml.getIntAttribute("uid")));

    const XmlElement *const state = xml.getChildByName(Serialization::Core::pluginState);

    if (state != nullptr)
    {
        MemoryBlock m;
        m.fromBase64Encoding(state->getAllSubText());
        node->getProcessor()->setStateInformation(m.getData(), static_cast<int>( m.getSize()));
    }

    const String& hash = xml.getStringAttribute("hash");
    Uuid fallbackRandomHash;
    
    node->properties.set("x", xml.getDoubleAttribute("x"));
    node->properties.set("y", xml.getDoubleAttribute("y"));
    node->properties.set("hash", hash.isNotEmpty() ? hash : fallbackRandomHash.toString());
    node->properties.set("uiLastX", xml.getIntAttribute("uiLastX"));
    node->properties.set("uiLastY", xml.getIntAttribute("uiLastY"));
}

void Instrument::initializeDefaultNodes()
{
    InternalPluginFormat internalFormat;
    this->audioIn = this->addDefaultNode(*internalFormat.getDescriptionFor(InternalPluginFormat::audioInputFilter), 0.1f, 0.15f);
    this->midiIn = this->addDefaultNode(*internalFormat.getDescriptionFor(InternalPluginFormat::midiInputFilter), 0.1f, 0.85f);
    this->audioOut = this->addDefaultNode(*internalFormat.getDescriptionFor(InternalPluginFormat::audioOutputFilter), 0.9f, 0.15f);
    this->midiOut = this->addDefaultNode(*internalFormat.getDescriptionFor(InternalPluginFormat::midiOutputFilter), 0.9f, 0.85f);
}

AudioProcessorGraph::Node *Instrument::addDefaultNode(const PluginDescription &desc, double x, double y)
{
    String errorMessage;
    AudioPluginInstance *instance =
    formatManager.createPluginInstance(desc,
                                       this->processorGraph->getSampleRate(),
                                       this->processorGraph->getBlockSize(),
                                       errorMessage);
    
    AudioProcessorGraph::Node *node = nullptr;
    
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

void Instrument::configureNode(AudioProcessorGraph::Node *node, const PluginDescription &desc, double x, double y)
{
    // здесь надо собрать информацию о ноде из описания плагина, сделать хэш и запомнить его
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


//===----------------------------------------------------------------------===//
// Thread
//===----------------------------------------------------------------------===//

//void Instrument::run()
//{
//    // что делаем, если поток уже запущен?
//    // что делаем при выходе?
//    // что делаем с локами процессорграфа? введем переменную isReady?
//    //
//    if (this->initializationMode == FROM_PLUGIN)
//    {
//        //
//    }
//    else if (this->initializationMode == FROM_XML)
//    {
//        //
//    }
//}


