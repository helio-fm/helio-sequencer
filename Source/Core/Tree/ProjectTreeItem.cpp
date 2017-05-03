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
#include "ProjectTreeItem.h"

#include "TreeItemChildrenSerializer.h"
#include "RootTreeItem.h"
#include "LayerGroupTreeItem.h"
#include "PianoLayerTreeItem.h"
#include "AutomationLayerTreeItem.h"
#include "MainLayout.h"
#include "Document.h"
#include "ProjectListener.h"
#include "ProjectPageDefault.h"
#include "ProjectPagePhone.h"

#include "AudioCore.h"
#include "PlayerThread.h"

#include "MidiEditor.h"
#include "MidiEvent.h"
#include "MidiLayer.h"
#include "PianoLayer.h"
#include "AutomationLayer.h"
#include "Icons.h"
#include "ProjectInfo.h"
#include "ProjectTimeline.h"
#include "DataEncoder.h"

#include "TrackedItem.h"
#include "VersionControlTreeItem.h"
#include "VersionControl.h"
#include "RecentFilesList.h"
#include "MidiRoll.h"
#include "Autosaver.h"

#include "HelioTheme.h"
#include "ProjectCommandPanel.h"
#include "UndoStack.h"

#include "Workspace.h"
#include "App.h"

#include "Config.h"
#include "SerializationKeys.h"


ProjectTreeItem::ProjectTreeItem(const String &name) :
DocumentOwner(App::Workspace(), name, "hp"),
    TreeItem(name)
{
    this->initialize();
}

ProjectTreeItem::ProjectTreeItem(const File &existingFile) :
    DocumentOwner(App::Workspace(), existingFile),
    TreeItem(existingFile.getFileNameWithoutExtension())
{
    this->initialize();
}

void ProjectTreeItem::initialize()
{
    this->isLayersHashOutdated = true;
    
    this->undoStack = new UndoStack(*this);
    
    this->autosaver = new Autosaver(*this);

    this->transport = new Transport(App::Workspace().getAudioCore());
    this->addListener(this->transport);
    
    this->recentFilesList = &App::Workspace().getRecentFilesList();
    
    this->info = new ProjectInfo(*this);
    this->vcsItems.add(this->info);
    
    this->timeline = new ProjectTimeline(*this, "Project Timeline");
    this->vcsItems.add(this->timeline);

    this->transport->seekToPosition(0.0);
    
    this->recreatePage();
}


ProjectTreeItem::~ProjectTreeItem()
{
    // the main policy: all data is to be autosaved
    this->getDocument()->save();
    
    this->transport->stopPlayback();
    this->transport->stopRender();

    // remember as the recent file
    if (this->recentFilesList != nullptr)
    {
        this->recentFilesList->
        onProjectStateChanged(this->getName(),
                              this->getDocument()->getFullPath(),
                              this->getId(),
                              false);
    }

    this->projectSettings = nullptr;

    this->removeAllListeners();
    this->editor = nullptr;

    this->timeline = nullptr;
    this->info = nullptr;

    this->removeListener(this->transport);
    this->transport = nullptr;

    this->autosaver = nullptr;
}

void ProjectTreeItem::deletePermanently()
{
    if (VersionControlTreeItem *vcsTreeItem = this->findChildOfType<VersionControlTreeItem>())
    {
        vcsTreeItem->deletePermanentlyFromRemoteRepo();
    }
    else
    {
        // normally, this should never happen
        File localProjectFile(this->getDocument()->getFullPath());
        App::Workspace().unloadProjectById(this->getId());
        localProjectFile.deleteFile();
        
        if (this->recentFilesList != nullptr)
        {
            this->recentFilesList->cleanup();
        }
    }
}


String ProjectTreeItem::getId() const
{
    VersionControlTreeItem *vcsTreeItem =
        this->findChildOfType<VersionControlTreeItem>();

    if (vcsTreeItem != nullptr)
    {
        return vcsTreeItem->getId();
    }

    return "";
}

String ProjectTreeItem::getStats() const
{
    Array<LayerTreeItem *> layerItems(this->findChildrenOfType<LayerTreeItem>());
    
    int numEvents = 0;
    int numLayers = layerItems.size();

    for (int i = 0; i < numLayers; ++i)
    {
        numEvents += layerItems[i]->getLayer()->size();
    }

    return String(TRANS_PLURAL("{x} layers", numLayers) + " " + TRANS("common::and") + " " + TRANS_PLURAL("{x} events", numEvents));
}

Transport &ProjectTreeItem::getTransport() const noexcept
{
    jassert(this->transport);
    return (*this->transport);
}

ProjectInfo *ProjectTreeItem::getProjectInfo() const noexcept
{
    jassert(this->info);
    return this->info;
}

ProjectTimeline *ProjectTreeItem::getTimeline() const noexcept
{
    jassert(this->timeline);
    return this->timeline;
}

MidiRoll *ProjectTreeItem::getLastFocusedRoll() const
{
    // todo!
    
    return this->editor->getRoll();
    
//    if (this->origami != nullptr)
//    {
//        if (Component *lastFocused = this->origami->getLastFocusedComponent())
//        {
//            return dynamic_cast<MidiRoll *>(lastFocused);
//        }
//    }
//    
//    return nullptr;
}

Colour ProjectTreeItem::getColour() const
{
    return Colour(0xffa489ff);
}

Image ProjectTreeItem::getIcon() const
{
    return Icons::findByName(Icons::project, TREE_LARGE_ICON_HEIGHT);
}

void ProjectTreeItem::showPage()
{
    this->projectSettings->updateContent();
    App::Layout().showPage(this->projectSettings, this);
}

void ProjectTreeItem::onRename(const String &newName)
{
    if (newName == this->getName())
    {
        return;
    }
    
    // notify recent files list
    if (this->recentFilesList != nullptr)
    {
        this->recentFilesList->removeById(this->getId());
    }

    //TreeItem::onRename(newName);
    //this->getDocument()->renameFile(this->getName());

    this->setName(newName);
    
    this->getDocument()->renameFile(newName);
    TreeItem::notifySubtreeMoved(this);

    // notify recent files list
    if (this->recentFilesList != nullptr)
    {
        this->recentFilesList->
        onProjectStateChanged(this->getName(),
                              this->getDocument()->getFullPath(),
                              this->getId(),
                              true);
    }
    
    // notify workspace's document that he has been changed and needs to save
    App::Workspace().sendChangeMessage();

    this->repaintItem();
}

void ProjectTreeItem::repaintEditor()
{
    this->editor->resized();
    this->editor->repaint();
}

void ProjectTreeItem::recreatePage()
{
    if (this->editor || this->projectSettings)
    {
        this->savePageState();
    }
    
    this->editor = new MidiEditor(*this);
    
    if (App::isRunningOnPhone())
    {
        this->projectSettings = new ProjectPagePhone(*this);
    }
    else
    {
        this->projectSettings = new ProjectPageDefault(*this);
    }
    
    this->broadcastBeatRangeChanged(); // let rolls update themselves
    this->loadPageState();
}

void ProjectTreeItem::savePageState() const
{
    ScopedPointer<XmlElement> editorStateNode(this->editor->serialize());
    Config::set(Serialization::UI::editorState, editorStateNode);
}

void ProjectTreeItem::loadPageState()
{
    ScopedPointer<XmlElement> editorStateNode(Config::getXml(Serialization::UI::editorState));
    if (editorStateNode != nullptr) {
        this->editor->deserialize(*editorStateNode);
    }
}

void ProjectTreeItem::showEditor(MidiLayer *layer) // todo +param: Component *caller
{
    const Array<MidiLayer *> layers(this->getLayersList());

    for (int i = 0; i < layers.size(); ++i)
    {
        const MidiLayer *layerIter = layers.getUnchecked(i);

        if (layerIter == layer)
        {
            if (TreeItem *item = dynamic_cast<TreeItem *>(layer->getOwner()))
            {
                this->showEditor(layer, item);
                return;
            }
        }
    }
}

void ProjectTreeItem::showEditor(MidiLayer *activeLayer, TreeItem *source)
{
    if (PianoLayer *pianoLayer = dynamic_cast<PianoLayer *>(activeLayer))
    {
        // todo collect selected pianotreeitems
        Array<PianoLayerTreeItem *> pianoTreeItems = this->findChildrenOfType<PianoLayerTreeItem>(true);
        Array<MidiLayer *> pianoLayers;

        for (int i = 0; i < pianoTreeItems.size(); ++i)
        {
            pianoLayers.add(pianoTreeItems.getUnchecked(i)->getLayer());
        }
        
        this->editor->setActiveMidiLayers(pianoLayers, activeLayer); // before
        
        App::Layout().showPage(this->editor, source);
        this->editor->grabKeyboardFocus();
    }
    else if (AutomationLayer *autoLayer = dynamic_cast<AutomationLayer *>(activeLayer))
    {
        const bool editorWasShown = this->editor->toggleShowAutomationEditor(autoLayer);
        source->setGreyedOut(!editorWasShown);
        this->editor->grabKeyboardFocus();
    }
}

void ProjectTreeItem::hideEditor(MidiLayer *activeLayer, TreeItem *source)
{
    if (AutomationLayer *autoLayer = dynamic_cast<AutomationLayer *>(activeLayer))
    {
        this->editor->hideAutomationEditor(autoLayer);
        source->setGreyedOut(true);
    }
}

void ProjectTreeItem::showEditorsGroup(Array<MidiLayer *> layersGroup, TreeItem *source)
{
    Array<MidiLayer *> layers(this->getLayersList());

    if (layersGroup.size() == 0)
    {
        // todo show dummy component?
        this->showPage();
        return;
    }
    if (layersGroup.size() == 1)
    {
        // single-editor
        this->showEditor(layersGroup[0], source);
        return;
    }
    
    // сейчас здесь баг - когда удаляешь слой из группы, фокус не меняется,
    // потому что здесь ничего не происходит,
    // активный слой указывает на невалидный пойнтер
    // todo mark first layer, select others and show show all layers
    
//    this->origami = new OrigamiVertical();
//    int editorIndex = 0;
//    MidiEditor *firstFoundEditor = nullptr;
//===----------------------------------------------------------------------===//
//    for (int i = 0; i < myLayers.size(); ++i)
//    {
//        const bool needsShadow = (editorIndex != (layersGroup.size() - 1));
//        MidiLayer *layerIter = myLayers.getUnchecked(i);
//===----------------------------------------------------------------------===//
//        if (layersGroup.contains(layerIter))
//        {
//            if (TreeItem *item = dynamic_cast<TreeItem *>(layerIter->getOwner()))
//            {
//                if (dynamic_cast<PianoLayer *>(layerIter))
//                {
//                    if (editorIndex >= this->splitscreenPianoEditors.size())
//                    { break; }
//===----------------------------------------------------------------------===//
//                    MidiEditor *foundEditor = this->splitscreenPianoEditors[editorIndex];
//                    this->origami->addPage(foundEditor, false, needsShadow);
//                    foundEditor->setActiveMidiLayer(layerIter);
//===----------------------------------------------------------------------===//
//                    firstFoundEditor = (firstFoundEditor == nullptr) ? foundEditor : firstFoundEditor;
//                }
//                else if (dynamic_cast<AutomationLayer *>(layerIter))
//                {
//                    // todo!
//                    
//                    //if (editorIndex >= this->splitscreenAutoEditors.size())
//                    //{ break; }
//===----------------------------------------------------------------------===//
//                    //MidiEditor *foundEditor = this->splitscreenAutoEditors[editorIndex];
//                    //this->origami->addPage(foundEditor, false, needsShadow);
//                    //foundEditor->setActiveMidiLayer(layerIter);
//===----------------------------------------------------------------------===//
//                    //firstFoundEditor = (firstFoundEditor == nullptr) ? foundEditor : firstFoundEditor;
//                }
//===----------------------------------------------------------------------===//
//                editorIndex += 1;
//            }
//        }
//    }
//===----------------------------------------------------------------------===//
//    this->origami->addPage(this->midiRollCommandPanel, false, false, true);
//    this->workspace.showPage(this->origami, source);
//===----------------------------------------------------------------------===//
//    firstFoundEditor->grabKeyboardFocus();
}

void ProjectTreeItem::updateActiveGroupEditors()
{
    Array<LayerGroupTreeItem *> myGroups(this->findChildrenOfType<LayerGroupTreeItem>());

    for (int i = 0; i < myGroups.size(); ++i)
    {
        LayerGroupTreeItem *group = myGroups.getUnchecked(i);

        if (group->isMarkerVisible())
        {
            group->showPage();
            return;
        }
    }
}


//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

Component *ProjectTreeItem::createItemMenu()
{
    return new ProjectCommandPanel(*this, CommandPanel::SlideRight);
}


//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

void ProjectTreeItem::onItemMoved()
{
    this->broadcastInfoChanged(this->info);
}

bool ProjectTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    //if (TreeView *treeView = dynamic_cast<TreeView *>(dragSourceDetails.sourceComponent.get()))
    //{
    //    TreeItem *selected = TreeItem::getSelectedItem(treeView);

    //if (TreeItem::isNodeInChildren(selected, this))
    //{ return false; }

    return (dragSourceDetails.description == Serialization::Core::layer) ||
           (dragSourceDetails.description == Serialization::Core::layerGroup);

    //}
}


//===----------------------------------------------------------------------===//
// Project
//===----------------------------------------------------------------------===//

Array<MidiLayer *> ProjectTreeItem::getLayersList() const
{
    ScopedReadLock lock(this->layersListLock);

    Array<MidiLayer *> layers;
    this->collectLayers(layers);
    layers.add(this->timeline->getLayer()); // explicitly add the only non-tree-owned layer
    return layers;
}

Array<MidiLayer *> ProjectTreeItem::getSelectedLayersList() const
{
    ScopedReadLock lock(this->layersListLock);
    
    Array<MidiLayer *> layers;
    this->collectLayers(layers, true);
    return layers;
}

void ProjectTreeItem::collectLayers(Array<MidiLayer *> &resultArray, bool onlySelectedLayers) const
{
    Array<LayerTreeItem *> layerItems = this->findChildrenOfType<LayerTreeItem>();
    
    for (int i = 0; i < layerItems.size(); ++i)
    {
        if (layerItems.getUnchecked(i)->isSelected() || !onlySelectedLayers)
        {
            resultArray.add(layerItems.getUnchecked(i)->getLayer());
        }
    }
}

Point<float> ProjectTreeItem::getTrackRangeInBeats() const
{
    float lastBeat = -FLT_MAX;
    float firstBeat = FLT_MAX;
    const float defaultNumBeats = DEFAULT_NUM_BARS * NUM_BEATS_IN_BAR;

    Array<MidiLayer *> layers;
    this->collectLayers(layers);

    for (auto layer : layers)
    {
        const float layerFirstBeat = layer->getFirstBeat();
        const float layerLastBeat = layer->getLastBeat();
        //Logger::writeToLog(">  " + String(layerFirstBeat) + " : " + String(layerLastBeat));
        firstBeat = jmin(firstBeat, layerFirstBeat);
        lastBeat = jmax(lastBeat, layerLastBeat);
    }
    
    if (firstBeat == FLT_MAX)
    {
        firstBeat = 0;
    }
    else if (firstBeat > lastBeat)
    {
        firstBeat = lastBeat - defaultNumBeats;
    }
    
    if ((lastBeat - firstBeat) < defaultNumBeats)
    {
        lastBeat = firstBeat + defaultNumBeats;
    }

    //Logger::writeToLog(">> " + String(firstBeat) + " : " + String(lastBeat));

    return Point<float>(firstBeat, lastBeat);
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *ProjectTreeItem::serialize() const
{
    this->getDocument()->save(); // todo remove? will save on delete

    auto xml = new XmlElement(Serialization::Core::treeItem);
    xml->setAttribute("type", Serialization::Core::project);
    xml->setAttribute("fullPath", this->getDocument()->getFullPath());
    xml->setAttribute("relativePath", this->getDocument()->getRelativePath());
    return xml;
}

void ProjectTreeItem::deserialize(const XmlElement &xml)
{
    this->reset();

    const String& type = xml.getStringAttribute("type");

    if (type != Serialization::Core::project) { return; }

    const String &fullPath = xml.getStringAttribute("fullPath");
    const String &relativePath = xml.getStringAttribute("relativePath");
    
    File relativeFile =
    File(App::Workspace().getDocument()->getFile().
         getParentDirectory().getChildFile(relativePath));
    
    if (!File(fullPath).existsAsFile() && !relativeFile.existsAsFile())
    {
        delete this;
        return;
    }
    
    this->getDocument()->load(fullPath, relativePath);
}

void ProjectTreeItem::reset()
{
    this->transport->seekToPosition(0.f);
    this->vcsItems.clear();
    this->vcsItems.add(this->info);
    this->vcsItems.add(this->timeline);
    this->undoStack->clearUndoHistory();
    TreeItem::reset();
}


XmlElement *ProjectTreeItem::save() const
{
    auto xml = new XmlElement(Serialization::Core::project);
    xml->setAttribute("name", this->name);

    xml->addChildElement(this->info->serialize());
    xml->addChildElement(this->timeline->serialize());
    
    //xml->addChildElement(this->player->serialize()); // todo instead of:
    xml->setAttribute("seek", this->transport->getSeekPosition());
    
    // UI state is now stored in config
    //xml->addChildElement(this->editor->serialize());

    xml->addChildElement(this->undoStack->serialize());
    
    TreeItemChildrenSerializer::serializeChildren(*this, *xml);

    this->savePageState();

    return xml;
}

void ProjectTreeItem::load(const XmlElement &xml)
{
    this->reset();

    const XmlElement *root = xml.hasTagName(Serialization::Core::project) ?
                             &xml : xml.getChildByName(Serialization::Core::project);

    if (root == nullptr) { return; }

    this->setName(root->getStringAttribute("name"));

    this->info->deserialize(*root);
    this->timeline->deserialize(*root);

    TreeItemChildrenSerializer::deserializeChildren(*this, *root);

    this->broadcastBeatRangeChanged();

    //this->transport->deserialize(*root); // todo

    // UI state is now stored in config
    //this->editor->deserialize(*root);
    
    this->undoStack->deserialize(*root);
    
    const float seek = float(root->getDoubleAttribute("seek", 0.f));
    this->transport->seekToPosition(seek);
}

void ProjectTreeItem::importMidi(File &file)
{
    MidiFile tempFile;
    ScopedPointer<InputStream> in(new FileInputStream(file));
    const bool readOk = tempFile.readFrom(*in);
    
    if (!readOk)
    {
        DBG("Midi file appears corrupted");
        return;
    }
    
    if (tempFile.getTimeFormat() <= 0)
    {
        DBG("SMPTE format timing is not yet supported");
        return;
    }
    
    // ‚‡ÊÌÓ.
    //tempFile.convertTimestampTicksToSeconds();
    
    for (int trackNum = 0; trackNum < tempFile.getNumTracks(); trackNum++)
    {
        const MidiMessageSequence *currentTrack = tempFile.getTrack(trackNum);
        const String trackName = "Track " + String(trackNum);
        LayerTreeItem *layer = new PianoLayerTreeItem(trackName);
        this->addChildTreeItem(layer);
        layer->importMidi(*currentTrack);
    }
    
    this->broadcastBeatRangeChanged();
    this->getDocument()->save();
}

//===----------------------------------------------------------------------===//
// ProjectListeners management
//===----------------------------------------------------------------------===//

void ProjectTreeItem::addListener(ProjectListener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->changeListeners.add(listener);
}

void ProjectTreeItem::removeListener(ProjectListener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->changeListeners.remove(listener);
}

void ProjectTreeItem::removeAllListeners()
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->changeListeners.clear();
}


//===----------------------------------------------------------------------===//
// Broadcaster
//===----------------------------------------------------------------------===//

void ProjectTreeItem::broadcastEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    //if (this->changeListeners.size() == 0) { return; }
    this->changeListeners.call(&ProjectListener::onEventChanged, oldEvent, newEvent);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastEventAdded(const MidiEvent &event)
{
    this->changeListeners.call(&ProjectListener::onEventAdded, event);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastEventRemoved(const MidiEvent &event)
{
    this->changeListeners.call(&ProjectListener::onEventRemoved, event);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastEventRemovedPostAction(const MidiLayer *layer)
{
    this->changeListeners.call(&ProjectListener::onEventRemovedPostAction, layer);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastLayerChanged(const MidiLayer *layer)
{
    this->changeListeners.call(&ProjectListener::onLayerChanged, layer);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastLayerAdded(const MidiLayer *layer)
{
    this->isLayersHashOutdated = true;
    this->registerVcsItem(layer);
    this->changeListeners.call(&ProjectListener::onLayerAdded, layer);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastLayerRemoved(const MidiLayer *layer)
{
    this->isLayersHashOutdated = true;
    this->unregisterVcsItem(layer);
    this->changeListeners.call(&ProjectListener::onLayerRemoved, layer);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastLayerMoved(const MidiLayer *layer)
{
    this->changeListeners.call(&ProjectListener::onLayerMoved, layer);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastInfoChanged(const ProjectInfo *info)
{
    this->changeListeners.call(&ProjectListener::onInfoChanged, info);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastBeatRangeChanged()
{
    const Point<float> &beatRange = this->getTrackRangeInBeats();
    const float &firstBeat = beatRange.getX();
    const float &lastBeat = beatRange.getY();
    
    // грязный хак %(
    // changeListeners.call итерирует список от конца к началу
    // и транспорт обновляет позицию индикатора позже, чем роллы
    // и, если ролл ресайзится, индикатор дергается
    // пусть лучше транспорт обновит индикатор 2 раза, но гарантированно перед остальными
    this->transport->onProjectBeatRangeChanged(firstBeat, lastBeat);
    
    this->changeListeners.call(&ProjectListener::onProjectBeatRangeChanged, firstBeat, lastBeat);
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// DocumentOwner
//===----------------------------------------------------------------------===//

bool ProjectTreeItem::onDocumentLoad(File &file)
{
    if (file.existsAsFile())
    {
        ScopedPointer<XmlElement> xml(DataEncoder::loadObfuscated(file));

        if (xml)
        {
            this->load(*xml);
            return true;
        }
    }

    return false;
}

void ProjectTreeItem::onDocumentDidLoad(File &file)
{
    if (this->recentFilesList != nullptr)
    {
        this->recentFilesList->
        onProjectStateChanged(this->getName(),
                              this->getDocument()->getFullPath(),
                              this->getId(),
                              true);
    }
}

bool ProjectTreeItem::onDocumentSave(File &file)
{
    ScopedPointer<XmlElement> xml(this->save());
    return DataEncoder::saveObfuscated(file, xml);
}

void ProjectTreeItem::onDocumentImport(File &file)
{
    if (file.hasFileExtension("mid") || file.hasFileExtension("midi"))
    {
        this->importMidi(file);
    }
}

bool ProjectTreeItem::onDocumentExport(File &file)
{
    if (file.hasFileExtension("mid") || file.hasFileExtension("midi"))
    {
        this->exportMidi(file);
        return true;
    }

    return false;
}

void ProjectTreeItem::exportMidi(File &file) const
{
    MidiFile tempFile;
    tempFile.setTicksPerQuarterNote(Transport::millisecondsPerBeat);
    
    const Array<MidiLayer *> &layers = this->getLayersList();
    
    for (auto layer : layers)
    {
        tempFile.addTrack(layer->exportMidi());
    }
    
    ScopedPointer<OutputStream> out(new FileOutputStream(file));
    tempFile.writeTo(*out);
}


//===----------------------------------------------------------------------===//
// VCS::TrackedItemsSource
//===----------------------------------------------------------------------===//

int ProjectTreeItem::getNumTrackedItems()
{
    ScopedReadLock lock(this->vcsInfoLock);
    return this->vcsItems.size();
}

VCS::TrackedItem *ProjectTreeItem::getTrackedItem(int index)
{
    ScopedReadLock lock(this->vcsInfoLock);
    return const_cast<VCS::TrackedItem *>(this->vcsItems[index]);
}

VCS::TrackedItem *ProjectTreeItem::initTrackedItem(const String &type, const Uuid &id)
{
    if (type == Serialization::Core::pianoLayer)
    {
        LayerTreeItem *layer = new PianoLayerTreeItem("empty");
        layer->setVCSUuid(id);
        this->addChildTreeItem(layer);
        return layer;
    }
    if (type == Serialization::Core::autoLayer)
    {
        LayerTreeItem *layer = new AutomationLayerTreeItem("empty");
        layer->setVCSUuid(id);
        this->addChildTreeItem(layer);
        return layer;
    }
    else if (type == Serialization::Core::projectInfo)
    {
        this->info->setVCSUuid(id);
        return this->info;
    }
    else if (type == Serialization::Core::projectTimeline)
    {
        this->timeline->setVCSUuid(id);
        return this->timeline;
    }
    
    return nullptr;
}

bool ProjectTreeItem::deleteTrackedItem(VCS::TrackedItem *item)
{
    if (dynamic_cast<LayerTreeItem *>(item))
    {
        delete item;
    }

    return true;
}



//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void ProjectTreeItem::changeListenerCallback(ChangeBroadcaster *source)
{
    if (VersionControl *vcs = dynamic_cast<VersionControl *>(source))
    {
        //Logger::writeToLog("ProjectTreeItem :: vcs changed, saving " + vcs->getParentName());
        DocumentOwner::sendChangeMessage();
        //this->getDocument()->save();
        
        // todo! здесь есть баг в ios
        // (или везде)
        // после коммита вцс начинает строить дифф, и в это же время из основного потока мы все сохраняем
        // в какой-то момент пак (баг именно в нем, видимо) не отдает данные дельты
//#if HELIO_DESKTOP
//        this->getDocument()->forceSave();
//#endif
    }
}



void ProjectTreeItem::registerVcsItem(const MidiLayer *layer)
{
    if (const VCS::TrackedItem *item = dynamic_cast<const VCS::TrackedItem *>(layer->getOwner()))
    {
        ScopedWriteLock lock(this->vcsInfoLock);
        this->vcsItems.add(item);
    }
}

void ProjectTreeItem::unregisterVcsItem(const MidiLayer *layer)
{
    if (const VCS::TrackedItem *item = dynamic_cast<const VCS::TrackedItem *>(layer->getOwner()))
    {
        ScopedWriteLock lock(this->vcsInfoLock);
        this->vcsItems.removeAllInstancesOf(item);
    }
}

void ProjectTreeItem::rebuildLayersHashIfNeeded()
{
    if (this->isLayersHashOutdated)
    {
        this->layersHash.clear();
        this->layersHash.set(this->timeline->getLayer()->getLayerIdAsString(), this->timeline->getLayer());
        
        Array<LayerTreeItem *> children = this->findChildrenOfType<LayerTreeItem>();
        
        for (int i = 0; i < children.size(); ++i)
        {
            MidiLayer *layer = children.getUnchecked(i)->getLayer();
            this->layersHash.set(layer->getLayerIdAsString(), layer);
        }
        
        this->isLayersHashOutdated = false;
    }
}
