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

//[Headers]
#include "Common.h"
//[/Headers]

#include "SequencerSidebarRight.h"

//[MiscUserDefs]
#include "Transport.h"
#include "ProjectTreeItem.h"
#include "PlayerThread.h"
#include "Icons.h"
#include "HybridRoll.h"
#include "PianoRoll.h"
#include "MidiSequence.h"
#include "MenuItemComponent.h"
#include "ProjectTimeline.h"
#include "HelioCallout.h"
#include "TimelineMenu.h"
#include "AnnotationMenu.h"
#include "TimeSignatureMenu.h"
#include "SequencerOperations.h"
#include "NotesTuningPanel.h"
#include "TriggersTrackMap.h"
#include "App.h"
#include "MainLayout.h"
#include "SequencerLayout.h"
#include "Workspace.h"
#include "CachedLabelImage.h"
#include "CommandIDs.h"
#include "ColourIDs.h"

#if HELIO_DESKTOP
#   define TOOLS_SIDEBAR_SHOWS_ANNOTATION_DETAILS (false)
#elif HELIO_MOBILE
#   define TOOLS_SIDEBAR_SHOWS_ANNOTATION_DETAILS (false)
#endif

//[/MiscUserDefs]

SequencerSidebarRight::SequencerSidebarRight(ProjectTreeItem &parent)
    : project(parent),
      lastSeekTime(0.0),
      lastTotalTime(0.0),
      timerStartSeekTime(0.0),
      timerStartSystemTime(0.0)
{
    addAndMakeVisible (listBox = new ListBox());

    addAndMakeVisible (headLine = new SeparatorHorizontalReversed());
    addAndMakeVisible (shadow = new LighterShadowUpwards());
    addAndMakeVisible (separator = new SeparatorHorizontal());
    addAndMakeVisible (totalTime = new Label (String(),
                                              TRANS("...")));
    totalTime->setFont (Font (Font::getDefaultSansSerifFontName(), 14.00f, Font::plain).withTypefaceStyle ("Regular"));
    totalTime->setJustificationType (Justification::centred);
    totalTime->setEditable (false, false, false);

    addAndMakeVisible (currentTime = new Label (String(),
                                                TRANS("...")));
    currentTime->setFont (Font (Font::getDefaultSansSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    currentTime->setJustificationType (Justification::centred);
    currentTime->setEditable (false, false, false);

    addAndMakeVisible (headShadow = new LighterShadowDownwards());
    addAndMakeVisible (annotationsButton = new MenuItemComponent (this, nullptr, MenuItem::item(Icons::menu, CommandIDs::ShowAnnotations)));

    addAndMakeVisible (playButton = new PlayButton());

    //[UserPreSize]
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, true);

    this->listBox->setModel(this);
    this->listBox->setMultipleSelectionEnabled(false);
    this->listBox->setRowHeight(SEQUENCER_SIDEBAR_ROW_HEIGHT);

    // This one doesn't change too frequently:
    //this->totalTime->setBufferedToImage(true);
    //this->totalTime->setCachedComponentImage(new CachedLabelImage(*this->totalTime));

    // TODO: remove these and show timings somewhere else
    this->totalTime->setVisible(false);
    this->currentTime->setVisible(false);

    //[/UserPreSize]

    setSize (48, 640);

    //[Constructor]
    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        Component *c = this->getChildComponent(i);
        c->setFocusContainer(false);
        c->setWantsKeyboardFocus(false);
        c->setMouseClickGrabsKeyboardFocus(false);
    }

    this->setSize(SEQUENCER_SIDEBAR_WIDTH, 640);

    this->project.getTransport().addTransportListener(this);
    this->project.getEditMode().addChangeListener(this);
    //[/Constructor]
}

SequencerSidebarRight::~SequencerSidebarRight()
{
    //[Destructor_pre]
    this->project.getEditMode().removeChangeListener(this);
    this->project.getTransport().removeTransportListener(this);
    //[/Destructor_pre]

    listBox = nullptr;
    headLine = nullptr;
    shadow = nullptr;
    separator = nullptr;
    totalTime = nullptr;
    currentTime = nullptr;
    headShadow = nullptr;
    annotationsButton = nullptr;
    playButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void SequencerSidebarRight::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    auto &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
    g.setFillType({ theme.getBgCache3(), {} });
    g.fillRect(this->getLocalBounds());
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    g.setColour(this->findColour(ColourIDs::Common::borderLineLight));
    g.drawVerticalLine(0, 0.f, float(this->getHeight()));
    //[/UserPaint]
}

void SequencerSidebarRight::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    listBox->setBounds (0, 41, getWidth() - 0, getHeight() - 113);
    headLine->setBounds (0, 39, getWidth() - 0, 2);
    shadow->setBounds (0, getHeight() - 71 - 6, getWidth() - 0, 6);
    separator->setBounds (0, getHeight() - 70 - 2, getWidth() - 0, 2);
    totalTime->setBounds ((getWidth() / 2) + 80 - (72 / 2), getHeight() - 9 - 18, 72, 18);
    currentTime->setBounds ((getWidth() / 2) + 80 - (72 / 2), getHeight() - 26 - 22, 72, 22);
    headShadow->setBounds (0, 40, getWidth() - 0, 6);
    annotationsButton->setBounds ((getWidth() / 2) - ((getWidth() - 0) / 2), 0, getWidth() - 0, 39);
    playButton->setBounds ((getWidth() / 2) - (48 / 2), getHeight() - 12 - 48, 48, 48);
    //[UserResized] Add your own custom resize handling here..
    //Logger::writeToLog("HybridRollCommandPanel updateContent");
    // a hack for themes changing
    this->listBox->updateContent();
    this->annotationsButton->resized();
    //[/UserResized]
}

void SequencerSidebarRight::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    switch (commandId)
    {
    case CommandIDs::ShowAnnotations:
    {
        const AnnotationEvent *selectedAnnotation = nullptr;
        const TimeSignatureEvent *selectedTimeSignature = nullptr;

        const ProjectTimeline *timeline = this->project.getTimeline();
        const double seekPosition = this->project.getTransport().getSeekPosition();

        if (HybridRoll *roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
        {
            const double numBeats = double(roll->getNumBeats());
            const double seekThreshold = (1.0 / numBeats) / 10.0;
            const auto annotationsSequence = timeline->getAnnotations()->getSequence();
            const auto timeSignaturesSequence = timeline->getTimeSignatures()->getSequence();

            for (int i = 0; i < annotationsSequence->size(); ++i)
            {
                if (AnnotationEvent *annotation =
                    dynamic_cast<AnnotationEvent *>(annotationsSequence->getUnchecked(i)))
                {
                    const double annotationSeekPosition = roll->getTransportPositionByBeat(annotation->getBeat());
                    if (fabs(annotationSeekPosition - seekPosition) < seekThreshold)
                    {
                        selectedAnnotation = annotation;
                        break;
                    }
                }
            }

            for (int i = 0; i < timeSignaturesSequence->size(); ++i)
            {
                if (TimeSignatureEvent *ts =
                    dynamic_cast<TimeSignatureEvent *>(timeSignaturesSequence->getUnchecked(i)))
                {
                    const double tsSeekPosition = roll->getTransportPositionByBeat(ts->getBeat());
                    if (fabs(tsSeekPosition - seekPosition) < seekThreshold)
                    {
                        selectedTimeSignature = ts;
                        break;
                    }
                }
            }
        }

        // FIXME ïðè íàæàòèè ÷òî-òî ïðîèñõîäèò ñî çâóêîì, à èíäèêàòîð åäåò äàëüøå
        // (ïîêà ïðîñòî îñòàíîâëþ âîñïðîèçâåäåíèå)
        this->project.getTransport().stopPlayback();

        if (selectedAnnotation != nullptr && TOOLS_SIDEBAR_SHOWS_ANNOTATION_DETAILS)
        {
            this->emitAnnotationsCallout(new AnnotationMenu(this->project, *selectedAnnotation));
        }
        else if (selectedTimeSignature != nullptr && TOOLS_SIDEBAR_SHOWS_ANNOTATION_DETAILS)
        {
            this->emitAnnotationsCallout(new TimeSignatureMenu(this->project, *selectedTimeSignature));
        }
        else
        {
            this->emitAnnotationsCallout(new TimelineMenu(this->project));
        }
    }
    break;

    case CommandIDs::CursorTool:
        this->project.getEditMode().setMode(HybridRollEditMode::defaultMode);
        break;

    case CommandIDs::DrawTool:
        this->project.getEditMode().setMode(HybridRollEditMode::drawMode);
        break;

    case CommandIDs::SelectionTool:
        this->project.getEditMode().setMode(HybridRollEditMode::selectionMode);
        break;

    case CommandIDs::ZoomTool:
        this->project.getEditMode().setMode(HybridRollEditMode::zoomMode);
        break;

    case CommandIDs::DragTool:
        this->project.getEditMode().setMode(HybridRollEditMode::dragMode);
        break;

    case CommandIDs::InsertSpaceTool:
        this->project.getEditMode().setMode(HybridRollEditMode::insertSpaceMode);
        break;

    case CommandIDs::WipeSpaceTool:
        this->project.getEditMode().setMode(HybridRollEditMode::wipeSpaceMode);
        break;

    case CommandIDs::ScissorsTool:
        this->project.getEditMode().setMode(HybridRollEditMode::scissorsMode);
        break;

    case CommandIDs::TweakNotesVolume:
        if (PianoRoll *roll = dynamic_cast<PianoRoll *>(this->project.getLastFocusedRoll()))
        {
            if (roll->getLassoSelection().getNumSelected() > 0)
            {
                HelioCallout::emit(new NotesTuningPanel(roll->getProject(), *roll), roll, true);
            }
            else
            {
                // TODO ! move this menu item into selection menu and get rid of useless warning
                //App::Layout().showTooltip(TRANS("warnings::emptyselection"));
            }
        }
        break;

    default:
        break;
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
void SequencerSidebarRight::recreateCommandDescriptions()
{
    this->menu.clear();

    const bool defaultMode = this->project.getEditMode().isMode(HybridRollEditMode::defaultMode);
    const bool drawMode = this->project.getEditMode().isMode(HybridRollEditMode::drawMode);
    const bool selectionMode = this->project.getEditMode().isMode(HybridRollEditMode::selectionMode);
    const bool zoomMode = this->project.getEditMode().isMode(HybridRollEditMode::zoomMode);
    const bool dragMode = this->project.getEditMode().isMode(HybridRollEditMode::dragMode);
    const bool wipeSpaceMode = this->project.getEditMode().isMode(HybridRollEditMode::wipeSpaceMode);
    const bool insertSpaceMode = this->project.getEditMode().isMode(HybridRollEditMode::insertSpaceMode);
    const bool scissorsMode = this->project.getEditMode().isMode(HybridRollEditMode::scissorsMode);

#if HELIO_MOBILE
    this->menu.add(MenuItem::item(Icons::selectionTool, CommandIDs::SelectionTool)->toggled(selectionMode));
#endif

    this->menu.add(MenuItem::item(Icons::cursorTool, CommandIDs::CursorTool)->toggled(defaultMode));
    this->menu.add(MenuItem::item(Icons::drawTool, CommandIDs::DrawTool)->toggled(drawMode));
    this->menu.add(MenuItem::item(Icons::dragTool, CommandIDs::DragTool)->toggled(dragMode));

    if (this->menuMode == PianoRollTools)
    {
        this->menu.add(MenuItem::item(Icons::wipeSpaceTool, CommandIDs::WipeSpaceTool)->toggled(wipeSpaceMode));
        this->menu.add(MenuItem::item(Icons::insertSpaceTool, CommandIDs::InsertSpaceTool)->toggled(insertSpaceMode));
    }

    this->menu.add(MenuItem::item(Icons::undo, CommandIDs::Undo));
    this->menu.add(MenuItem::item(Icons::redo, CommandIDs::Redo));

    this->menu.add(MenuItem::item(Icons::zoomIn, CommandIDs::ZoomIn));
    this->menu.add(MenuItem::item(Icons::zoomOut, CommandIDs::ZoomOut));

    //this->menu.add(MenuItem::item(Icons::volumeUp, CommandIDs::TweakNotesVolume));
    //this->menu.add(MenuItem::item(Icons::switcher, CommandIDs::MoveEventsToLayer));
    //this->menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::RefactorNotes));
    //this->menu.add(MenuItem::item(Icons::arpeggiator, CommandIDs::ArpNotes));

    //this->menu.add(MenuItem::item(Icons::copy, CommandIDs::CopyEvents));
    //this->menu.add(MenuItem::item(Icons::paste, CommandIDs::PasteEvents));
    //this->menu.add(MenuItem::item(Icons::trash, CommandIDs::DeleteEvents));
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *SequencerSidebarRight::refreshComponentForRow(int rowNumber,
    bool isRowSelected, Component *existingComponentToUpdate)
{
    if (rowNumber >= this->menu.size())
    {
        return existingComponentToUpdate;
    }

    const MenuItem::Ptr itemDescription = this->menu[rowNumber];

    if (existingComponentToUpdate != nullptr)
    {
        if (MenuItemComponent *row =
            dynamic_cast<MenuItemComponent *>(existingComponentToUpdate))
        {
            row->setSelected(isRowSelected);
            row->update(itemDescription);
        }
    }
    else
    {
        MenuItemComponent *row =
            new MenuItemComponent(this, this->listBox->getViewport(), itemDescription);
        row->setSelected(isRowSelected);
        return row;
    }

    return existingComponentToUpdate;
}

int SequencerSidebarRight::getNumRows()
{
    return this->menu.size();
}

//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void SequencerSidebarRight::handleAsyncUpdate()
{
    if (this->isTimerRunning())
    {
        const double systemTimeOffset = (Time::getMillisecondCounter() - this->timerStartSystemTime.get());
        const double currentTimeMs(this->timerStartSeekTime.get() + systemTimeOffset);
        this->currentTime->setText(Transport::getTimeString(currentTimeMs), dontSendNotification);
    }
    else
    {
        this->currentTime->setText(Transport::getTimeString(this->lastSeekTime.get()), dontSendNotification);
    }

    this->totalTime->setText(Transport::getTimeString(this->lastTotalTime.get()), dontSendNotification);
}

//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void SequencerSidebarRight::onSeek(double absolutePosition,
    double currentTimeMs, double totalTimeMs)
{
    this->lastSeekTime = currentTimeMs;
    this->lastTotalTime = totalTimeMs;
    this->triggerAsyncUpdate();
}

void SequencerSidebarRight::onTempoChanged(double newTempo) {}

void SequencerSidebarRight::onTotalTimeChanged(double timeMs)
{
    this->lastTotalTime = timeMs;
    this->totalTime->setText(Transport::getTimeString(this->lastTotalTime.get()), dontSendNotification);
}

void SequencerSidebarRight::onPlay()
{
    this->timerStartSystemTime = Time::getMillisecondCounter();
    this->timerStartSeekTime = this->lastSeekTime;
    this->startTimer(100);
    this->playButton->setPlaying(true);
}

void SequencerSidebarRight::onStop()
{
    this->stopTimer();
    this->playButton->setPlaying(false);
}

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void SequencerSidebarRight::changeListenerCallback(ChangeBroadcaster *source)
{
    this->updateModeButtons();
}

//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

void SequencerSidebarRight::timerCallback()
{
    this->triggerAsyncUpdate();
}

//===----------------------------------------------------------------------===//
// SequencerSidebarRight
//===----------------------------------------------------------------------===//

void SequencerSidebarRight::updateModeButtons()
{
    this->recreateCommandDescriptions();
    this->listBox->updateContent();
}

void SequencerSidebarRight::emitAnnotationsCallout(Component *newAnnotationsMenu)
{
    HelioCallout::emit(newAnnotationsMenu, this->annotationsButton);
}

void SequencerSidebarRight::setLinearMode()
{
    this->menuMode = PianoRollTools;
    this->recreateCommandDescriptions();
    this->listBox->updateContent();
}

void SequencerSidebarRight::setPatternMode()
{
    this->menuMode = PatternRollTools;
    this->recreateCommandDescriptions();
    this->listBox->updateContent();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SequencerSidebarRight" template="../../../Template"
                 componentName="" parentClasses="public Component, protected TransportListener, protected AsyncUpdater, protected ListBoxModel, protected ChangeListener, protected Timer"
                 constructorParams="ProjectTreeItem &amp;parent" variableInitialisers="project(parent),&#10;lastSeekTime(0.0),&#10;lastTotalTime(0.0),&#10;timerStartSeekTime(0.0),&#10;timerStartSystemTime(0.0)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="48" initialHeight="640">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="381fa571a3dfc5cd" memberName="listBox" virtualName=""
                    explicitFocusOrder="0" pos="0 41 0M 113M" class="ListBox" params=""/>
  <JUCERCOMP name="" id="28ce45d9e84b729c" memberName="headLine" virtualName=""
             explicitFocusOrder="0" pos="0 39 0M 2" sourceFile="../../Themes/SeparatorHorizontalReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="accf780c6ef7ae9e" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 71Rr 0M 6" sourceFile="../../Themes/LighterShadowUpwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="22d481533ce3ecd3" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="0 70Rr 0M 2" sourceFile="../../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <LABEL name="" id="700073f74a17c931" memberName="totalTime" virtualName=""
         explicitFocusOrder="0" pos="80Cc 9Rr 72 18" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="14.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="36"/>
  <LABEL name="" id="b9e867ece7f52ad8" memberName="currentTime" virtualName=""
         explicitFocusOrder="0" pos="80Cc 26Rr 72 22" labelText="..."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default sans-serif font" fontsize="16.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="36"/>
  <JUCERCOMP name="" id="1d398dc12e2047bd" memberName="headShadow" virtualName=""
             explicitFocusOrder="0" pos="0 40 0M 6" sourceFile="../../Themes/LighterShadowDownwards.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="34c972d7b22acf17" memberName="annotationsButton"
                    virtualName="" explicitFocusOrder="0" pos="0Cc 0 0M 39" class="MenuItemComponent"
                    params="this, nullptr, MenuItem::item(Icons::menu, CommandIDs::ShowAnnotations)"/>
  <JUCERCOMP name="" id="bb2e14336f795a57" memberName="playButton" virtualName=""
             explicitFocusOrder="0" pos="0Cc 12Rr 48 48" sourceFile="../../Common/PlayButton.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//==============================================================================
// Binary resources - be careful not to edit any of these sections!

// JUCER_RESOURCE: gray1x1_png, 150, "../../../../MainLayout/~icons/gray1x1.png"
static const unsigned char resource_SequencerSidebarRight_gray1x1_png[] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,1,0,0,0,1,8,2,0,0,0,144,119,83,222,0,0,0,9,112,72,89,115,0,0,11,19,0,0,11,
19,1,0,154,156,24,0,0,0,7,116,73,77,69,7,222,4,19,5,8,9,228,2,121,9,0,0,0,29,105,84,88,116,67,111,109,109,101,110,116,0,0,0,0,0,67,114,101,97,116,101,100,32,119,105,116,104,32,71,73,77,80,100,46,101,7,
0,0,0,12,73,68,65,84,8,215,99,136,138,138,2,0,2,32,1,15,53,60,95,243,0,0,0,0,73,69,78,68,174,66,96,130,0,0};

const char* SequencerSidebarRight::gray1x1_png = (const char*) resource_SequencerSidebarRight_gray1x1_png;
const int SequencerSidebarRight::gray1x1_pngSize = 150;
