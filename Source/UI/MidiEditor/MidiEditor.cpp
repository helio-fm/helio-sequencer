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
#include "MidiEditor.h"
#include "MidiLayer.h"
#include "AutomationLayer.h"
#include "PianoRoll.h"
#include "ProjectTreeItem.h"
#include "TrackScroller.h"
#include "PianoTrackMap.h"
#include "AutomationTrackMap.h"
#include "TriggersTrackMap.h"
#include "SerializationKeys.h"
#include "AnnotationSmallComponent.h"
#include "OrigamiHorizontal.h"
#include "OrigamiVertical.h"
#include "MidiRollCommandPanelPhone.h"
#include "MidiRollCommandPanelDefault.h"
#include "AutomationsCommandPanel.h"
#include "PanelBackgroundC.h"

#include "App.h"
#include "Workspace.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "AudioMonitor.h"

// force compile template
#include "AnnotationsMap/AnnotationsTrackMap.cpp"
template class AnnotationsTrackMap<AnnotationSmallComponent>;


#define MAX_NUM_SPLITSCREEN_EDITORS 2
#define MINIMUM_ROLLS_HEIGHT 250


//===----------------------------------------------------------------------===//
// Splitter
//===----------------------------------------------------------------------===//

class MidiEditorSplitContainer : public Component, private Timer
{
public:
    
    class ResizeConstrainer : public ComponentBoundsConstrainer
    {
    public:
        explicit ResizeConstrainer(MidiEditorSplitContainer &splitterRef) : splitter(splitterRef) { }
        
        void applyBoundsToComponent(Component *component, const Rectangle<int> &bounds) override
        {
            ComponentBoundsConstrainer::applyBoundsToComponent(component, bounds);
            this->splitter.resizedByUser();
        }
        
    private:
        MidiEditorSplitContainer &splitter;
    };
    
    MidiEditorSplitContainer(Component *targetRoll,
                             Origami *targetAutomations) :
        roll(targetRoll),
        automations(targetAutomations),
        deltaH(0.f)
    {
        this->setFocusContainer(false);
        this->setWantsKeyboardFocus(false);
        
        this->automations->setFocusContainer(false);
        this->automations->setWantsKeyboardFocus(false);
        
        this->setInterceptsMouseClicks(false, true);
        this->setOpaque(true);
        
        this->constrainer = new ResizeConstrainer(*this);
        this->constrainer->setMinimumHeight(MINIMUM_ROLLS_HEIGHT);
        this->constrainer->setMaximumHeight(1024);
        
        this->resizer = new ResizableEdgeComponent(this->automations, this->constrainer, ResizableEdgeComponent::bottomEdge);
        this->resizer->setAlwaysOnTop(true);
        this->resizer->setInterceptsMouseClicks(true, true);
        
        this->resizer->setFocusContainer(false);
        this->resizer->setWantsKeyboardFocus(false);

        this->addAndMakeVisible(this->roll);
        this->addAndMakeVisible(this->automations);
        this->addAndMakeVisible(this->resizer);
    }
    
    ~MidiEditorSplitContainer() override
    {
        this->resizer = nullptr;
        this->constrainer = nullptr;
    }
    
    void focusGained(FocusChangeType cause) override
    {
        if (Origami *parentOrigami = this->findParentComponentOfClass<Origami>())
        {
            parentOrigami->focusOfChildComponentChanged(cause);
        }
    }
    
    void paint(Graphics &g) override
    {
        const Colour backCol(this->findColour(MidiRoll::headerColourId).darker(0.05f));
        g.fillAll(backCol);
    }
    
    void resized() override
    {
        const int autosMinHeight = this->automations->getMinimumCommonSize();
        const int autosMaxHeight = jmin(this->automations->getMaximumCommonSize(), this->getHeight() - MINIMUM_ROLLS_HEIGHT);
        
        const int rollMinHeight = this->getHeight() - autosMaxHeight;
        const int rollMaxHeight = this->getHeight() - autosMinHeight;
        
        const int newRollHeight = jmin(rollMaxHeight, jmax(rollMinHeight, this->roll->getHeight()));
        const int newAutosHeight = jmin(autosMaxHeight, jmax(autosMinHeight, this->automations->getHeight()));
        
        this->constrainer->setMinimumHeight(autosMinHeight);
        this->constrainer->setMaximumHeight(autosMaxHeight);
        
        Rectangle<int> r(this->getLocalBounds().withTop(int(-this->deltaH)));
        
        this->automations->setBounds(r.removeFromTop(newAutosHeight));
        this->resizer->setBounds(r.removeFromTop(1));
        this->roll->setBounds(r);
    }
    
    void resizedByUser()
    {
        this->resized();
        // this->roll->toFront(true);
    }
    
    void resizeAutomations(int heightOffset)
    {
        if (heightOffset != 0)
        {
            this->deltaH = float(heightOffset);
            this->automations->setSize(this->automations->getWidth(), this->automations->getHeight() + heightOffset);
            this->automations->setTopLeftPosition(0, this->automations->getY() - heightOffset);
            this->startTimer(17);
        }
    }
    
private:
    
    void timerCallback() override
    {
        this->deltaH = this->deltaH / 1.6f;
        this->resized();
        
        if (fabs(this->deltaH) < 0.1f)
        {
            this->deltaH = 0.f;
            this->stopTimer();
        }

        // this->roll->toFront(true);
    }
    
    float deltaH;
    
    SafePointer<Component> roll;

    SafePointer<Origami> automations;

    ScopedPointer<ResizableEdgeComponent> resizer;
    
    ScopedPointer<ResizeConstrainer> constrainer;
    
};


//===----------------------------------------------------------------------===//
// Automations Container
//===----------------------------------------------------------------------===//

class AutomationTrackMapProxy : public Component, public MidiRollListener, private AsyncUpdater
{
public:
    
    AutomationTrackMapProxy(MidiRoll &parentRoll,
                            Component *newOwnedComponent) :
        roll(parentRoll),
        target(newOwnedComponent)
    {
        this->setFocusContainer(false);
        this->setWantsKeyboardFocus(false);

        this->setInterceptsMouseClicks(false, true);
        this->setOpaque(true);

        this->roll.addRollListener(this);
        this->addAndMakeVisible(this->target);
        
        //this->commandPanel = new AutomationsCommandPanel();
        //this->addAndMakeVisible(this->commandPanel);
        //this->commandPanel->setAlwaysOnTop(true);
        //
        //this->commandPanel->setFocusContainer(false);
        //this->commandPanel->setWantsKeyboardFocus(false);

        this->setSize(newOwnedComponent->getWidth(), newOwnedComponent->getHeight());
    }
    
    ~AutomationTrackMapProxy() override
    {
        //this->removeChildComponent(this->commandPanel);
        this->removeChildComponent(this->target);
        this->roll.removeRollListener(this);
    }
    
    void resized() override
    {
        //Logger::writeToLog("AutomationTrackMapProxy resized " + String(this->getHeight()));
        //this->commandPanel->setBounds(this->getLocalBounds().removeFromRight(MIDIROLL_COMMANDPANEL_WIDTH));
        this->updateTargetBounds();
    }
    
    void paint(Graphics &g) override
    {
        const int numBars = this->roll.getNumBars();
        const float barWidth = this->roll.getBarWidth();
        
        int dynamicGridSize = NUM_BEATS_IN_BAR;
        int showEvery = 1;
        
        MidiRoll::getGridMultipliers(barWidth, dynamicGridSize, showEvery);
        
        const int zeroCanvasOffset = int(this->roll.getFirstBar() * barWidth);
        const int paintStartX = int(this->roll.getViewport().getViewPositionX() + zeroCanvasOffset);
        const int paintEndX = this->roll.getViewport().getViewPositionX() + this->roll.getViewport().getViewWidth() + zeroCanvasOffset;
        const int paintWidth = paintEndX - paintStartX;
        
        const Colour backCol(this->findColour(MidiRoll::headerColourId));
        const Colour frontCol(backCol.contrasting().withMultipliedAlpha(0.5f));
        
        g.setGradientFill(ColourGradient(backCol,
                                         0.f,
                                         4.f,
                                         backCol.darker(0.06f),
                                         0.f,
                                         float(this->getHeight() - 4), false));
        
        g.fillAll();
        
        g.setColour(frontCol);
        
        int i = int(paintStartX / barWidth) - showEvery;
        const int j = int(paintEndX / barWidth);
        const float beatWidth = barWidth / float(dynamicGridSize);
        
        while (i <= j)
        {
            // show every x'th
            if (i % showEvery == 0)
            {
                const float startX1 = float(barWidth * i) - paintStartX;
                g.drawLine(startX1, 0.f, startX1, float(this->getHeight() - 1), 0.4f);
                
                for (int k = 1; k < dynamicGridSize; k++)
                {
                    const float startX2 = (barWidth * i + beatWidth * showEvery * k) - paintStartX;
                    g.drawLine(startX2, 0.f, startX2, float(this->getHeight() - 1), 0.1f);
                }
            }
            
            i++;
        }
        
        g.setColour(Colours::white.withAlpha(0.07f));
        g.drawHorizontalLine(this->getHeight() - 1, 0.f, float(this->getWidth()));
        
        g.setColour(Colours::black.withAlpha(0.11f));
        g.drawHorizontalLine(this->getHeight() - 2, 0.f, float(this->getWidth()));
    }
    
    void onMidiRollMoved(MidiRoll *targetRoll) override
    {
        //this->triggerAsyncUpdate();
        // or
        this->updateTargetPosition();
    }
    
    void onMidiRollResized(MidiRoll *targetRoll) override
    {
        //this->triggerAsyncUpdate();
        // or
        this->updateTargetBounds();
    }
    
    MidiRoll &getRoll()
    {
        return this->roll;
    }
    
private:
    
    MidiRoll &roll;
    
    ScopedPointer<Component> target;
    
    //ScopedPointer<Component> commandPanel;
    
    void handleAsyncUpdate() override
    {
        this->updateTargetBounds();
    }
    
    void updateTargetPosition()
    {
        const int x = this->roll.getViewport().getViewPositionX();
        this->target->setTopLeftPosition(-x, 0);
    }
    
    void updateTargetBounds()
    {
        const int x = this->roll.getViewport().getViewPositionX();
        this->target->setBounds(this->getLocalBounds().withWidth(this->roll.getWidth()).withX(-x));
    }
};


//===----------------------------------------------------------------------===//
// Rolls Container
//===----------------------------------------------------------------------===//

class PianoRollProxy : public Component
{
public:
    
    PianoRollProxy(MidiRoll *targetRoll,
                   Viewport *targetViewport,
                   TrackScroller *targetScroller) :
    roll(targetRoll),
    viewport(targetViewport),
    scroller(targetScroller)
    {
        this->setFocusContainer(false);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, true);
        
        this->addAndMakeVisible(this->viewport);
        this->addAndMakeVisible(this->scroller);
    }
    
    ~PianoRollProxy() override
    {
        this->removeAllChildren();
    }
    
    void resized() override
    {
        jassert(this->roll);
        jassert(this->viewport);
        jassert(this->scroller);
        
        Rectangle<int> r(this->getLocalBounds());
        const int scrollerHeight = MainLayout::getScrollerHeight();
        this->viewport->setBounds(r.withBottom(r.getBottom() - scrollerHeight));
        this->scroller->setBounds(r.removeFromBottom(scrollerHeight));
        
        if ((this->roll->getBarWidth() * this->roll->getNumBars()) < this->getWidth())
        {
            this->roll->setBarWidth(float(this->getWidth()) / float(this->roll->getNumBars()));
        }
        
        this->roll->resized();
    }
    
private:
    
    SafePointer<MidiRoll> roll;
    SafePointer<Viewport> viewport;
    SafePointer<TrackScroller> scroller;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRollProxy)
};


//===----------------------------------------------------------------------===//
// Editor Itself
//===----------------------------------------------------------------------===//

MidiEditor::MidiEditor(ProjectTreeItem &parentProject) :
    project(parentProject),
    roll(nullptr)
{
    this->setName("MidiEditor");

    // создаем вьюпорт, кидаем в него ролл и добавляем скроллер снизу
    this->viewport = new Viewport("Viewport One");
    this->viewport->setInterceptsMouseClicks(false, true);
    this->viewport->setScrollBarsShown(false, false);
    this->viewport->setWantsKeyboardFocus(false);
    this->viewport->setFocusContainer(false);

    const WeakReference<AudioMonitor> clippingDetector =
        App::Workspace().getAudioCore().getMonitor();
    
    this->roll = new PianoRoll(this->project,
                               *this->viewport,
                               clippingDetector);

    this->scroller = new TrackScroller(this->project.getTransport(), *this->roll);
    this->scroller->addOwnedMap(new PianoTrackMap(this->project, *this->roll), false);
    this->scroller->addOwnedMap(new AnnotationsTrackMap<AnnotationSmallComponent>(this->project, *this->roll), false);
    //this->scroller->addOwnedMap(new AutomationTrackMap(this->project, *this->roll, this->project.getDefaultTempoTrack()->getLayer()), true);

    this->roll->setBarWidth(MAX_BAR_WIDTH);
    this->roll->setSnapQuantize(8);


    this->viewport->setViewedComponent(this->roll, false);
    this->roll->addRollListener(this->scroller);

    // захардкодим дефолтную позицию по y (не с самого верха, так стремно)
    const int defaultY = (this->roll->getHeight() / 3);
    this->viewport->setViewPosition(this->viewport->getViewPositionX(), defaultY);
    
    
    // затем помещаем их в контейнер
    this->rollContainer = new PianoRollProxy(this->roll, this->viewport, this->scroller);
    
    // создаем тулбар и компонуем его с контейнером
    if (App::isRunningOnPhone())
    {
        this->rollCommandPanel = new MidiRollCommandPanelPhone(this->project);
    }
    else
    {
        this->rollCommandPanel = new MidiRollCommandPanelDefault(this->project);
    }

    // TODO we may have multiple roll editors here
    //this->rollsOrigami = new OrigamiVertical();
    //this->rollsOrigami->addPage(this->rollContainer, false, false, false);
    // TODO fix focus troubles
    //this->rollsOrigami->setFocusContainer(false);
    //this->rollsOrigami->setWantsKeyboardFocus(false);
    
    // создаем стек редакторов автоматизации
    this->automationsOrigami = new OrigamiHorizontal();
    
    // и объединяем все редакторы в один лейаут
    //this->allEditorsContainer = new MidiEditorSplitContainer(this->rollsOrigami, this->automationsOrigami);
    this->allEditorsContainer = new MidiEditorSplitContainer(this->rollContainer, this->automationsOrigami);

    // добавляем тулбар справа
    this->allEditorsAndCommandPanel = new OrigamiVertical();
    this->allEditorsAndCommandPanel->addPage(this->allEditorsContainer, true, true, false);
    this->allEditorsAndCommandPanel->addPage(this->rollCommandPanel, false, false, true);

    this->addAndMakeVisible(this->allEditorsAndCommandPanel);

    this->setWantsKeyboardFocus(false);
    this->setFocusContainer(false);
}

MidiEditor::~MidiEditor()
{
    this->allEditorsAndCommandPanel = nullptr;
    this->allEditorsContainer = nullptr;
    
    this->automationsOrigami = nullptr;
    
    //this->rollsOrigami = nullptr;
    this->rollCommandPanel = nullptr;
    this->rollContainer = nullptr;
    
    this->roll->removeRollListener(this->scroller);
    //this->roll->removeAllChangeListeners();
    
    this->roll = nullptr;
    this->scroller = nullptr;
    this->viewport = nullptr;
}

void MidiEditor::setActiveMidiLayers(Array<MidiLayer *> tracks, MidiLayer *primaryTrack)
{
    //Logger::writeToLog("MidiEditor::setActiveMidiLayers");
    this->roll->setActiveMidiLayers(tracks, primaryTrack);
    this->roll->grabKeyboardFocus();
}

void MidiEditor::hideAutomationEditor(AutomationLayer *targetLayer)
{
    const String &layerId = targetLayer->getLayerId().toString();
    
    if (this->automationEditorsLinks.contains(layerId))
    {
        this->toggleShowAutomationEditor(targetLayer);
    }
}

bool MidiEditor::toggleShowAutomationEditor(AutomationLayer *targetLayer)
{
    const String &layerId = targetLayer->getLayerId().toString();
    
    // special case for the tempo track - let's show it on the scroller
    //if (targetLayer->isTempoLayer())
    //{
    //    AutomationTrackMap *existingTrackMap = this->scroller->findOwnedMapOfType<AutomationTrackMap>();
    //    const bool addTrackMode = (existingTrackMap == nullptr);
    //    
    //    if (addTrackMode)
    //    {
    //        AutomationTrackMapCommon *newTrackMap = new AutomationTrackMap(this->project, *this->roll, targetLayer);
    //        newTrackMap->reloadTrack();
    //        this->scroller->addOwnedMap(newTrackMap, true);
    //    }
    //    else
    //    {
    //        this->scroller->removeOwnedMap(existingTrackMap);
    //    }
    //    
    //    return addTrackMode;
    //}
    
    if (targetLayer->isSustainPedalLayer())
    {
        TriggersTrackMap *existingTrackMap = this->roll->findOwnedMapOfType<TriggersTrackMap>();
        const bool addTrackMode = (existingTrackMap == nullptr);
        
        if (addTrackMode)
        {
            TriggersTrackMap *newTrackMap = new TriggersTrackMap(this->project, *this->roll, targetLayer);
            newTrackMap->reloadTrack();
            this->roll->addOwnedMap(newTrackMap);
        }
        else
        {
            this->roll->removeOwnedMap(existingTrackMap);
        }
        
        return addTrackMode;
    }
    
    if (this->automationEditorsLinks.contains(layerId))
    {
        AutomationTrackMapProxy *trackMapProxy = this->automationEditorsLinks[layerId];

        this->automationsOrigami->removePageContaining(trackMapProxy);

        // здесь уменьшить высоту automationsOrigami на высоту trackMapProxy
        this->allEditorsContainer->resizeAutomations(-trackMapProxy->getHeight());
        this->allEditorsContainer->resized();
        
        this->automationEditorsLinks.remove(layerId);
        this->automationEditors.removeObject(trackMapProxy);
        
        this->resized();
        return false;
    }
    
    AutomationTrackMapCommon *newTrackMap = nullptr;
    
    if (targetLayer->isOnOffLayer())
    {
        newTrackMap = new TriggersTrackMap(this->project, *this->roll, targetLayer);
    }
    else
    {
        newTrackMap = new AutomationTrackMap(this->project, *this->roll, targetLayer);
    }
    
    if (newTrackMap != nullptr)
    {
        newTrackMap->reloadTrack();
        auto newTrackMapProxy = new AutomationTrackMapProxy(*this->roll, newTrackMap);
        
        this->automationEditors.add(newTrackMapProxy);
        this->automationEditorsLinks.set(layerId, newTrackMapProxy);
        
        // здесь увеличить размер automationsOrigami на высоту newTrackMap
        this->allEditorsContainer->resizeAutomations(newTrackMap->getHeight());
        
        const int hNormal = newTrackMap->getHeight();
        const int hMax = targetLayer->isOnOffLayer() ? hNormal : (hNormal * 3);
        const bool isFixedSize = targetLayer->isOnOffLayer();
        this->automationsOrigami->addPage(newTrackMapProxy, false, false, isFixedSize, hNormal, hMax, 0);
        
        this->resized();
        return true;
    }

    return false;
}

MidiRoll *MidiEditor::getRoll() const
{
    return this->roll;
}

//===----------------------------------------------------------------------===//
// FileDragAndDropTarget
//===----------------------------------------------------------------------===//

bool MidiEditor::isInterestedInFileDrag(const StringArray &files)
{
    File file = File(files.joinIntoString(String::empty, 0, 1));
    return (file.hasFileExtension("mid") || file.hasFileExtension("midi"));
}

void MidiEditor::filesDropped(const juce::StringArray &filenames,
                              int mouseX, int mouseY)
{
    if (isInterestedInFileDrag(filenames))
    {
        String filename = filenames.joinIntoString(String::empty, 0, 1);
        Logger::writeToLog(filename);
        //importMidiFile(File(filename));
    }
}


//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void MidiEditor::resized()
{
    this->allEditorsAndCommandPanel->setBounds(this->getLocalBounds());

    // a hack for themes changing
    this->rollCommandPanel->resized();
}

// void MidiEditor::broughtToFront()
// {
    // this->roll->toFront(true);
// }


//===----------------------------------------------------------------------===//
// UI State Serialization
//===----------------------------------------------------------------------===//

XmlElement *MidiEditor::serialize() const
{
    // задел на будущее, типа
    auto xml = new XmlElement(Serialization::Core::editor);
    xml->addChildElement(this->roll->serialize());
    return xml;
}

void MidiEditor::deserialize(const XmlElement &xml)
{
    this->reset();

    const XmlElement *root = (xml.getTagName() == Serialization::Core::editor) ?
                             &xml : xml.getChildByName(Serialization::Core::editor);

    if (root == nullptr)
    { return; }

    if (XmlElement *firstChild = root->getFirstChildElement())
    {
        this->roll->deserialize(*firstChild);
    }
}

void MidiEditor::reset()
{
    // no need for this yet
}
