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
#include "TrackGroupTreeItem.h"
#include "PianoTrackTreeItem.h"
#include "AutomationTrackTreeItem.h"
#include "MainLayout.h"
#include "Document.h"
#include "ProjectListener.h"
#include "ProjectPageDefault.h"
#include "ProjectPagePhone.h"

#include "AudioCore.h"
#include "PlayerThread.h"

#include "SequencerLayout.h"
#include "MidiEvent.h"
#include "MidiSequence.h"
#include "PianoSequence.h"
#include "AutomationSequence.h"
#include "Icons.h"
#include "ProjectInfo.h"
#include "ProjectTimeline.h"
#include "DataEncoder.h"

#include "TrackedItem.h"
#include "VersionControlTreeItem.h"
#include "VersionControl.h"
#include "RecentFilesList.h"
#include "HybridRoll.h"
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
    Array<MidiTrackTreeItem *> layerItems(this->findChildrenOfType<MidiTrackTreeItem>());
    
    int numEvents = 0;
    int numLayers = layerItems.size();

    for (int i = 0; i < numLayers; ++i)
    {
        numEvents += layerItems[i]->getSequence()->size();
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

HybridRollEditMode &ProjectTreeItem::getEditMode() noexcept
{
    return this->rollEditMode;
}

HybridRoll *ProjectTreeItem::getLastFocusedRoll() const
{
    // todo!
    return this->editor->getRoll();
    
//    if (this->origami != nullptr)
//    {
//        if (Component *lastFocused = this->origami->getLastFocusedComponent())
//        {
//            return dynamic_cast<HybridRoll *>(lastFocused);
//        }
//    }
//    
//    return nullptr;
}

Pattern *ProjectTreeItem::findPatternByTrackId(const String &uuid)
{
    // TODO implement
    return nullptr;
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

void ProjectTreeItem::safeRename(const String &newName)
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

    this->name = newName;
    
    this->getDocument()->renameFile(newName);
    this->broadcastChangeProjectInfo(this->info);

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

void ProjectTreeItem::recreatePage()
{
    if (this->editor || this->projectSettings)
    {
        this->savePageState();
    }
    
    this->editor = new SequencerLayout(*this);
    
    if (App::isRunningOnPhone())
    {
        this->projectSettings = new ProjectPagePhone(*this);
    }
    else
    {
        this->projectSettings = new ProjectPageDefault(*this);
    }
    
    this->broadcastChangeProjectBeatRange(); // let rolls update themselves
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

void ProjectTreeItem::showEditor(MidiSequence *activeLayer, TreeItem *source)
{
    if (PianoSequence *pianoLayer = dynamic_cast<PianoSequence *>(activeLayer))
    {
        // todo collect selected pianotreeitems
        Array<PianoTrackTreeItem *> pianoTreeItems = this->findChildrenOfType<PianoTrackTreeItem>(true);
        Array<MidiSequence *> pianoLayers;

        for (int i = 0; i < pianoTreeItems.size(); ++i)
        {
            pianoLayers.add(pianoTreeItems.getUnchecked(i)->getSequence());
        }
        
        this->editor->setActiveMidiLayers(pianoLayers, activeLayer); // before
        App::Layout().showPage(this->editor, source);
    }
    else if (AutomationSequence *autoLayer = dynamic_cast<AutomationSequence *>(activeLayer))
    {
        const bool editorWasShown = this->editor->toggleShowAutomationEditor(autoLayer);
        source->setGreyedOut(!editorWasShown);
    }
}

void ProjectTreeItem::hideEditor(MidiSequence *activeLayer, TreeItem *source)
{
    if (AutomationSequence *autoLayer = dynamic_cast<AutomationSequence *>(activeLayer))
    {
        this->editor->hideAutomationEditor(autoLayer);
        source->setGreyedOut(true);
    }
}

void ProjectTreeItem::showEditorsGroup(Array<MidiSequence *> layersGroup, TreeItem *source)
{
    //const auto tracks(this->getTracks());

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
//
//    for (int i = 0; i < myLayers.size(); ++i)
//    {
//        const bool needsShadow = (editorIndex != (layersGroup.size() - 1));
//        MidiSequence *layerIter = myLayers.getUnchecked(i);
//
//        if (layersGroup.contains(layerIter))
//        {
//            if (TreeItem *item = dynamic_cast<TreeItem *>(layerIter->getOwner()))
//            {
//                if (dynamic_cast<PianoSequence *>(layerIter))
//                {
//                    if (editorIndex >= this->splitscreenPianoEditors.size())
//                    { break; }
//
//                    MidiEditor *foundEditor = this->splitscreenPianoEditors[editorIndex];
//                    this->origami->addPage(foundEditor, false, needsShadow);
//                    foundEditor->setActiveMidiLayer(layerIter);
//
//                    firstFoundEditor = (firstFoundEditor == nullptr) ? foundEditor : firstFoundEditor;
//                }
//                else if (dynamic_cast<AutomationSequence *>(layerIter))
//                {
//                    // todo!
//                    
//                    //if (editorIndex >= this->splitscreenAutoEditors.size())
//                    //{ break; }
//
//                    //MidiEditor *foundEditor = this->splitscreenAutoEditors[editorIndex];
//                    //this->origami->addPage(foundEditor, false, needsShadow);
//                    //foundEditor->setActiveMidiLayer(layerIter);
//
//                    //firstFoundEditor = (firstFoundEditor == nullptr) ? foundEditor : firstFoundEditor;
//                }
//
//                editorIndex += 1;
//            }
//        }
//    }
//
//    this->origami->addPage(this->midiRollCommandPanel, false, false, true);
//    this->workspace.showPage(this->origami, source);
}

void ProjectTreeItem::updateActiveGroupEditors()
{
    Array<TrackGroupTreeItem *> myGroups(this->findChildrenOfType<TrackGroupTreeItem>());

    for (int i = 0; i < myGroups.size(); ++i)
    {
        TrackGroupTreeItem *group = myGroups.getUnchecked(i);

        if (group->isMarkerVisible())
        {
            group->showPage();
            return;
        }
    }
}

void ProjectTreeItem::activateLayer(MidiSequence* sequence, bool selectOthers, bool deselectOthers)
{
    if (selectOthers)
    {
        if (PianoTrackTreeItem *item =
            this->findTrackById<PianoTrackTreeItem>(sequence->getTrackId()))
        {
            PianoTrackTreeItem::selectAllPianoSiblings(item);
        }
    }
    else
    {
        if (PianoTrackTreeItem *item =
            this->findTrackById<PianoTrackTreeItem>(sequence->getTrackId()))
        {
            item->setSelected(false, false);
            item->setSelected(true, deselectOthers);
        }
    }
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

ScopedPointer<Component> ProjectTreeItem::createItemMenu()
{
    return new ProjectCommandPanel(*this, CommandPanel::SlideRight);
}


//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

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
// Undos
//===----------------------------------------------------------------------===//

UndoStack *ProjectTreeItem::getUndoStack() const noexcept
{
    return this->undoStack.get();
}

void ProjectTreeItem::checkpoint()
{
    this->getUndoStack()->beginNewTransaction(String::empty);
}

void ProjectTreeItem::undo()
{
    if (this->getUndoStack()->canUndo())
    {
        this->checkpoint();
        this->getUndoStack()->undo();
    }
}

void ProjectTreeItem::redo()
{
    if (this->getUndoStack()->canRedo())
    {
        this->getUndoStack()->redo();
    }
}

void ProjectTreeItem::clearUndoHistory()
{
    this->getUndoStack()->clearUndoHistory();
}

//===----------------------------------------------------------------------===//
// Project
//===----------------------------------------------------------------------===//

Array<MidiTrack *> ProjectTreeItem::getTracks() const
{
    ScopedReadLock lock(this->tracksListLock);
    Array<MidiTrack *> tracks;

    // now get all layers inside a tree hierarchy
    this->collectTracks(tracks);
    
    // and explicitly add the only non-tree-owned layers
    tracks.add(this->timeline->getAnnotations());
    tracks.add(this->timeline->getTimeSignatures());

    return tracks;
}

Array<MidiTrack *> ProjectTreeItem::getSelectedTracks() const
{
    ScopedReadLock lock(this->tracksListLock);
    Array<MidiTrack *> tracks;
    this->collectTracks(tracks, true);
    return tracks;
}

void ProjectTreeItem::collectTracks(Array<MidiTrack *> &resultArray, bool onlySelected /*= false*/) const
{
    const Array<MidiTrackTreeItem *> treeItems =
        this->findChildrenOfType<MidiTrackTreeItem>();
    
    for (int i = 0; i < treeItems.size(); ++i)
    {
        if (treeItems.getUnchecked(i)->isSelected() || !onlySelected)
        {
            resultArray.add(treeItems.getUnchecked(i));
        }
    }
}

Point<float> ProjectTreeItem::getProjectRangeInBeats() const
{
    float lastBeat = -FLT_MAX;
    float firstBeat = FLT_MAX;
    const float defaultNumBeats = DEFAULT_NUM_BARS * NUM_BEATS_IN_BAR;

    Array<MidiTrack *> tracks;
    this->collectTracks(tracks);

    for (auto track : tracks)
    {
        const float layerFirstBeat = track->getSequence()->getFirstBeat();
        const float layerLastBeat = track->getSequence()->getLastBeat();
        //Logger::writeToLog(">  " + String(layerFirstBeat) + " : " + String(layerLastBeat));
        firstBeat = jmin(firstBeat, layerFirstBeat);
        lastBeat = jmax(lastBeat, layerLastBeat);

        // TODO also take into account patterns!!!!!!!
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

    this->name = root->getStringAttribute("name", this->name);

    this->info->deserialize(*root);
    this->timeline->deserialize(*root);

    TreeItemChildrenSerializer::deserializeChildren(*this, *root);

    const auto range = this->broadcastChangeProjectBeatRange();

    // a hack to add some margin to project beat range,
    // then to round beats to nearest bars
    // because rolls' view ranges are rounded to bars
    const float r = float(NUM_BEATS_IN_BAR);
    const float viewStartWithMArgin = range.getX() - r;
    const float viewEndWithMArgin = range.getY() + r;
    const float viewFirstBeat = floorf(viewStartWithMArgin / r) * r;
    const float viewLastBeat = ceilf(viewEndWithMArgin / r) * r;
    this->broadcastChangeViewBeatRange(viewFirstBeat, viewLastBeat);

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
        MidiTrackTreeItem *layer = new PianoTrackTreeItem(trackName);
        this->addChildTreeItem(layer);
        layer->importMidi(*currentTrack);
    }
    
    this->broadcastChangeProjectBeatRange();
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

void ProjectTreeItem::broadcastChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    this->changeListeners.call(&ProjectListener::onChangeMidiEvent, oldEvent, newEvent);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastAddEvent(const MidiEvent &event)
{
    this->changeListeners.call(&ProjectListener::onAddMidiEvent, event);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastRemoveEvent(const MidiEvent &event)
{
    this->changeListeners.call(&ProjectListener::onRemoveMidiEvent, event);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastPostRemoveEvent(MidiSequence *const layer)
{
    this->changeListeners.call(&ProjectListener::onPostRemoveMidiEvent, layer);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastAddTrack(MidiTrack *const track)
{
    this->isLayersHashOutdated = true;
    this->registerVcsItem(track->getSequence());
    this->registerVcsItem(track->getPattern());
    this->changeListeners.call(&ProjectListener::onAddTrack, track);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastRemoveTrack(MidiTrack *const track)
{
    this->isLayersHashOutdated = true;
    this->unregisterVcsItem(track->getSequence());
    this->unregisterVcsItem(track->getPattern());
    this->changeListeners.call(&ProjectListener::onRemoveTrack, track);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastChangeTrackProperties(MidiTrack *const track)
{
    this->changeListeners.call(&ProjectListener::onChangeTrackProperties, track);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastResetTrackContent(MidiTrack *const track)
{
    this->changeListeners.call(&ProjectListener::onResetTrackContent, track);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastAddClip(const Clip &clip)
{
    this->changeListeners.call(&ProjectListener::onAddClip, clip);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastChangeClip(const Clip &oldClip, const Clip &newClip)
{
    this->changeListeners.call(&ProjectListener::onChangeClip, oldClip, newClip);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastRemoveClip(const Clip &clip)
{
    this->changeListeners.call(&ProjectListener::onRemoveClip, clip);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastPostRemoveClip(Pattern *const pattern)
{
    this->changeListeners.call(&ProjectListener::onPostRemoveClip, pattern);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastChangeProjectInfo(const ProjectInfo *info)
{
    this->changeListeners.call(&ProjectListener::onChangeProjectInfo, info);
    this->sendChangeMessage();
}

Point<float> ProjectTreeItem::broadcastChangeProjectBeatRange()
{
    const Point<float> &beatRange = this->getProjectRangeInBeats();
    const float &firstBeat = beatRange.getX();
    const float &lastBeat = beatRange.getY();
    
    // грязный хак %(
    // changeListeners.call итерирует список от конца к началу
    // и транспорт обновляет позицию индикатора позже, чем роллы
    // и, если ролл ресайзится, индикатор дергается
    // пусть лучше транспорт обновит индикатор 2 раза, но гарантированно перед остальными
    this->transport->onChangeProjectBeatRange(firstBeat, lastBeat);
    
    this->changeListeners.call(&ProjectListener::onChangeProjectBeatRange, firstBeat, lastBeat);
    this->sendChangeMessage();

    return beatRange;
}

void ProjectTreeItem::broadcastChangeViewBeatRange(float firstBeat, float lastBeat)
{
    this->changeListeners.call(&ProjectListener::onChangeViewBeatRange, firstBeat, lastBeat);
    // this->sendChangeMessage(); the project itself didn't change, so dont call this
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
    
    const auto &tracks = this->getTracks();
    
    for (auto track : tracks)
    {
        // TODO patterns!
        tempFile.addTrack(track->getSequence()->exportMidi());
    }
    
    ScopedPointer<OutputStream> out(new FileOutputStream(file));
    tempFile.writeTo(*out);
}


//===----------------------------------------------------------------------===//
// VCS::TrackedItemsSource
//===----------------------------------------------------------------------===//

String ProjectTreeItem::getVCSName() const
{
    return this->getName();
}

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
        MidiTrackTreeItem *layer = new PianoTrackTreeItem("empty");
        layer->setVCSUuid(id);
        this->addChildTreeItem(layer);
        return layer;
    }
    if (type == Serialization::Core::autoLayer)
    {
        MidiTrackTreeItem *layer = new AutomationTrackTreeItem("empty");
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
    if (dynamic_cast<MidiTrackTreeItem *>(item))
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



void ProjectTreeItem::registerVcsItem(const MidiSequence *layer)
{
    const auto children = this->findChildrenOfType<MidiTrackTreeItem>();
    for (MidiTrackTreeItem *item : children)
    {
        if (item->getSequence() == layer)
        {
            ScopedWriteLock lock(this->vcsInfoLock);
            this->vcsItems.addIfNotAlreadyThere(item);
        }
    }
}

void ProjectTreeItem::registerVcsItem(const Pattern *pattern)
{
    const auto children = this->findChildrenOfType<MidiTrackTreeItem>();
    for (MidiTrackTreeItem *item : children)
    {
        if (item->getPattern() == pattern)
        {
            ScopedWriteLock lock(this->vcsInfoLock);
            this->vcsItems.addIfNotAlreadyThere(item);
        }
    }
}

void ProjectTreeItem::unregisterVcsItem(const MidiSequence *layer)
{
    const auto children = this->findChildrenOfType<MidiTrackTreeItem>();
    for (MidiTrackTreeItem *item : children)
    {
        if (item->getSequence() == layer)
        {
            ScopedWriteLock lock(this->vcsInfoLock);
            this->vcsItems.addIfNotAlreadyThere(item);
        }
    }
}

void ProjectTreeItem::unregisterVcsItem(const Pattern *pattern)
{
    const auto children = this->findChildrenOfType<MidiTrackTreeItem>();
    for (MidiTrackTreeItem *item : children)
    {
        if (item->getPattern() == pattern)
        {
            ScopedWriteLock lock(this->vcsInfoLock);
            this->vcsItems.addIfNotAlreadyThere(item);
        }
    }
}

void ProjectTreeItem::rebuildSequencesHashIfNeeded()
{
    if (this->isLayersHashOutdated)
    {
        this->sequencesHash.clear();

        this->sequencesHash.set(this->timeline->getAnnotations()->getTrackId().toString(), 
            this->timeline->getAnnotations()->getSequence());

        this->sequencesHash.set(this->timeline->getTimeSignatures()->getTrackId().toString(),
            this->timeline->getTimeSignatures()->getSequence());
        
        Array<MidiTrack *> children = this->findChildrenOfType<MidiTrack>();
        
        for (int i = 0; i < children.size(); ++i)
        {
            const MidiTrack *track = children.getUnchecked(i);
            this->sequencesHash.set(track->getTrackId().toString(), track->getSequence());
        }
        
        this->isLayersHashOutdated = false;
    }
}
