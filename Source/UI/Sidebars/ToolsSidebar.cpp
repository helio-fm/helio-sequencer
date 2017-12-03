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

#include "ToolsSidebar.h"

//[MiscUserDefs]
#include "Transport.h"
#include "ToolsSidebar.h"
#include "ProjectTreeItem.h"
#include "PlayerThread.h"
#include "Icons.h"
#include "HybridRoll.h"
#include "PianoRoll.h"
#include "MidiSequence.h"
#include "InternalClipboard.h"
#include "CommandItemComponent.h"
#include "ProjectTimeline.h"
#include "HelioCallout.h"
#include "TimelineCommandPanel.h"
#include "AnnotationCommandPanel.h"
#include "TimeSignatureCommandPanel.h"
#include "PianoRollToolbox.h"
#include "NotesTuningPanel.h"
#include "TriggersTrackMap.h"
#include "App.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "CommandIDs.h"

#if HELIO_DESKTOP
#   define TOOLS_SIDEBAR_SHOWS_ANNOTATION_DETAILS (false)
#elif HELIO_MOBILE
#   define TOOLS_SIDEBAR_SHOWS_ANNOTATION_DETAILS (false)
#endif

//[/MiscUserDefs]

ToolsSidebar::ToolsSidebar(ProjectTreeItem &parent)
    : project(parent),
      lastSeekTime(0.0),
      lastTotalTime(0.0),
      timerStartSeekTime(0.0),
      timerStartSystemTime(0.0)
{
    addAndMakeVisible (headBg = new PanelBackgroundC());
    addAndMakeVisible (bodyBg = new PanelBackgroundC());
    addAndMakeVisible (listBox = new ListBox());

    addAndMakeVisible (headLine = new SeparatorHorizontalReversed());
    addAndMakeVisible (shadow = new LighterShadowUpwards());
    addAndMakeVisible (gradient2 = new GradientVertical());
    addAndMakeVisible (separator = new SeparatorHorizontal());
    addAndMakeVisible (totalTime = new Label (String(),
                                              TRANS("...")));
    totalTime->setFont (Font (Font::getDefaultSansSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    totalTime->setJustificationType (Justification::centred);
    totalTime->setEditable (false, false, false);
    totalTime->setColour (Label::textColourId, Colour (0x77ffffff));
    totalTime->setColour (TextEditor::textColourId, Colours::black);
    totalTime->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (currentTime = new Label (String(),
                                                TRANS("...")));
    currentTime->setFont (Font (Font::getDefaultSansSerifFontName(), 28.00f, Font::plain).withTypefaceStyle ("Regular"));
    currentTime->setJustificationType (Justification::centred);
    currentTime->setEditable (false, false, false);
    currentTime->setColour (Label::textColourId, Colour (0x77ffffff));
    currentTime->setColour (TextEditor::textColourId, Colours::black);
    currentTime->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (playButton = new PlayButton());
    addAndMakeVisible (headShadow = new LighterShadowDownwards());
    addAndMakeVisible (gradient = new GradientVerticalReversed());
    addAndMakeVisible (annotationsButton = new CommandItemComponent (this, nullptr, CommandItem::withParams(Icons::menu, CommandIDs::ShowAnnotations)));


    //[UserPreSize]
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, true);

    this->recreateCommandDescriptions();

    this->currentTime->setFont(Font(Font::getDefaultSansSerifFontName(), 22.00f, Font::plain));

    this->listBox->setModel(this);
    this->listBox->setMultipleSelectionEnabled(false);
    this->listBox->setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    this->listBox->setRowHeight(TOOLS_SIDEBAR_ROW_HEIGHT);
    //[/UserPreSize]

    setSize (64, 640);

    //[Constructor]
    this->setSize(TOOLS_SIDEBAR_WIDTH, 320);

    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        Component *c = this->getChildComponent(i);
        c->setFocusContainer(false);
        c->setWantsKeyboardFocus(false);
        c->setMouseClickGrabsKeyboardFocus(false);
    }

    this->setSize(TOOLS_SIDEBAR_WIDTH, 640);

    this->project.getTransport().addTransportListener(this);
    this->project.getEditMode().addChangeListener(this);
    //[/Constructor]
}

ToolsSidebar::~ToolsSidebar()
{
    //[Destructor_pre]
    this->project.getEditMode().removeChangeListener(this);
    this->project.getTransport().removeTransportListener(this);
    //[/Destructor_pre]

    headBg = nullptr;
    bodyBg = nullptr;
    listBox = nullptr;
    headLine = nullptr;
    shadow = nullptr;
    gradient2 = nullptr;
    separator = nullptr;
    totalTime = nullptr;
    currentTime = nullptr;
    playButton = nullptr;
    headShadow = nullptr;
    gradient = nullptr;
    annotationsButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ToolsSidebar::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ToolsSidebar::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    headBg->setBounds (0, 0, getWidth() - 0, 48);
    bodyBg->setBounds (0, 48, getWidth() - 0, getHeight() - 48);
    listBox->setBounds (0, 49, getWidth() - 0, getHeight() - 177);
    headLine->setBounds (0, 47, getWidth() - 0, 2);
    shadow->setBounds (0, getHeight() - 127 - 6, getWidth() - 0, 6);
    gradient2->setBounds (0, getHeight() - 127, getWidth() - 0, 127);
    separator->setBounds (0, getHeight() - 126 - 2, getWidth() - 0, 2);
    totalTime->setBounds ((getWidth() / 2) - (72 / 2), getHeight() - 18 - (28 / 2), 72, 28);
    currentTime->setBounds ((getWidth() / 2) - (72 / 2), getHeight() - 46 - (32 / 2), 72, 32);
    playButton->setBounds ((getWidth() / 2) - ((getWidth() - 0) / 2), getHeight() - 92 - (64 / 2), getWidth() - 0, 64);
    headShadow->setBounds (0, 48, getWidth() - 0, 6);
    gradient->setBounds (0, 0, getWidth() - 0, 47);
    annotationsButton->setBounds ((getWidth() / 2) - ((getWidth() - 0) / 2), 0, getWidth() - 0, 45);
    //[UserResized] Add your own custom resize handling here..
    //Logger::writeToLog("HybridRollCommandPanel updateContent");
    // a hack for themes changing
    this->listBox->updateContent();
    this->annotationsButton->resized();

#if HELIO_DESKTOP
    playButton->setBounds ((getWidth() / 2) - ((getWidth() - 0) / 2), getHeight() - 88 - (64 / 2), getWidth() - 0, 48);
    totalTime->setBounds ((getWidth() / 2) - (72 / 2), getHeight() - 28 - (28 / 2), 72, 28);
    currentTime->setBounds ((getWidth() / 2) - (72 / 2), getHeight() - 54 - (32 / 2), 72, 32);
    annotationsButton->setBounds ((getWidth() / 2) - ((getWidth() - 0) / 2), 28 - (36 / 2), getWidth() - 0, 36);
#endif

    //[/UserResized]
}

void ToolsSidebar::handleCommandMessage (int commandId)
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
            this->emitAnnotationsCallout(new AnnotationCommandPanel(this->project, *selectedAnnotation));
        }
        else if (selectedTimeSignature != nullptr && TOOLS_SIDEBAR_SHOWS_ANNOTATION_DETAILS)
        {
            this->emitAnnotationsCallout(new TimeSignatureCommandPanel(this->project, *selectedTimeSignature));
        }
        else
        {
            this->emitAnnotationsCallout(new TimelineCommandPanel(this->project));
        }
    }
    break;

    case CommandIDs::TransportStartPlayback:
        if (HybridRoll *roll = this->project.getLastFocusedRoll())
        {
            this->project.getTransport().startPlayback();
            roll->startFollowingPlayhead();
        }
        break;

    case CommandIDs::TransportPausePlayback:
        if (HybridRoll *roll = this->project.getLastFocusedRoll())
        {
            this->project.getTransport().stopPlayback();
            roll->stopFollowingPlayhead();
        }
        break;

    case CommandIDs::DeleteEvents:
        if (PianoRoll *roll = dynamic_cast<PianoRoll *>(this->project.getLastFocusedRoll()))
        {
            roll->deleteSelection();
        }
        break;

    case CommandIDs::CopyEvents:
        if (HybridRoll *roll = this->project.getLastFocusedRoll())
        {
            InternalClipboard::copy(*roll);
        }
        break;

    case CommandIDs::PasteEvents:
        if (HybridRoll *roll = this->project.getLastFocusedRoll())
        {
            InternalClipboard::paste(*roll);
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


    case CommandIDs::ZoomIn:
        if (HybridRoll *roll = this->project.getLastFocusedRoll())
        {
            roll->zoomInImpulse();
        }
        break;

    case CommandIDs::ZoomOut:
        if (HybridRoll *roll = this->project.getLastFocusedRoll())
        {
            roll->zoomOutImpulse();
        }
        break;


    case CommandIDs::Undo:
        this->project.undo();
        break;

    case CommandIDs::Redo:
        this->project.redo();
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
                App::Helio()->showTooltip(TRANS("warnings::emptyselection"));
            }
        }
        break;

    default:
        break;
    }
    //[/UserCode_handleCommandMessage]
}

void ToolsSidebar::childrenChanged()
{
    //[UserCode_childrenChanged] -- Add your code here...
    //this->updateButtonsImages();
    //[/UserCode_childrenChanged]
}

void ToolsSidebar::mouseMove (const MouseEvent& e)
{
    //[UserCode_mouseMove] -- Add your code here...
    //[/UserCode_mouseMove]
}


//[MiscUserCode]
void ToolsSidebar::paintOverChildren(Graphics& g)
{
    g.setColour(findColour(HelioTheme::resizerLineColourId));
    g.drawVerticalLine(0, 0.f, float(this->getHeight()));
}

void ToolsSidebar::recreateCommandDescriptions()
{
    this->commandDescriptions.clear();

    const bool defaultMode = this->project.getEditMode().isMode(HybridRollEditMode::defaultMode);
    const bool drawMode = this->project.getEditMode().isMode(HybridRollEditMode::drawMode);
    const bool selectionMode = this->project.getEditMode().isMode(HybridRollEditMode::selectionMode);
    const bool zoomMode = this->project.getEditMode().isMode(HybridRollEditMode::zoomMode);
    const bool dragMode = this->project.getEditMode().isMode(HybridRollEditMode::dragMode);
    const bool wipeSpaceMode = this->project.getEditMode().isMode(HybridRollEditMode::wipeSpaceMode);
    const bool insertSpaceMode = this->project.getEditMode().isMode(HybridRollEditMode::insertSpaceMode);
    const bool scissorsMode = this->project.getEditMode().isMode(HybridRollEditMode::scissorsMode);

    this->commandDescriptions.add(CommandItem::withParams(Icons::cursorTool, CommandIDs::CursorTool)->toggled(defaultMode));
    this->commandDescriptions.add(CommandItem::withParams(Icons::drawTool, CommandIDs::DrawTool)->toggled(drawMode));
    this->commandDescriptions.add(CommandItem::withParams(Icons::selectionTool, CommandIDs::SelectionTool)->toggled(selectionMode));
    //this->commandDescriptions.add(CommandItem::withParams(Icons::zoomTool, CommandIDs::ZoomTool)->toggled(zoomMode));
    this->commandDescriptions.add(CommandItem::withParams(Icons::dragTool, CommandIDs::DragTool)->toggled(dragMode));
    this->commandDescriptions.add(CommandItem::withParams(Icons::wipeScapeTool, CommandIDs::WipeSpaceTool)->toggled(wipeSpaceMode));
    this->commandDescriptions.add(CommandItem::withParams(Icons::insertSpaceTool, CommandIDs::InsertSpaceTool)->toggled(insertSpaceMode));

    //this->commandDescriptions.add(CommandItem::withParams(Icons::zoomIn, CommandIDs::ZoomIn));
    //this->commandDescriptions.add(CommandItem::withParams(Icons::zoomOut, CommandIDs::ZoomOut));
    this->commandDescriptions.add(CommandItem::withParams(Icons::undo, CommandIDs::Undo));
    this->commandDescriptions.add(CommandItem::withParams(Icons::redo, CommandIDs::Redo));

    //this->commandDescriptions.add(CommandItem::withParams(Icons::volumeUp, CommandIDs::TweakNotesVolume));
    //this->commandDescriptions.add(CommandItem::withParams(Icons::switcher, CommandIDs::MoveEventsToLayer));
    //this->commandDescriptions.add(CommandItem::withParams(Icons::ellipsis, CommandIDs::RefactorNotes));
    //this->commandDescriptions.add(CommandItem::withParams(Icons::arpeggiator, CommandIDs::ArpNotes));

    //this->commandDescriptions.add(CommandItem::withParams(Icons::copy, CommandIDs::CopyEvents));
    //this->commandDescriptions.add(CommandItem::withParams(Icons::paste, CommandIDs::PasteEvents));
    //this->commandDescriptions.add(CommandItem::withParams(Icons::trash, CommandIDs::DeleteEvents));
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *ToolsSidebar::refreshComponentForRow(int rowNumber,
    bool isRowSelected, Component *existingComponentToUpdate)
{
    if (rowNumber >= this->commandDescriptions.size())
    {
        return existingComponentToUpdate;
    }

    const CommandItem::Ptr itemDescription = this->commandDescriptions[rowNumber];

    if (existingComponentToUpdate != nullptr)
    {
        if (CommandItemComponent *row = 
            dynamic_cast<CommandItemComponent *>(existingComponentToUpdate))
        {
            row->setSelected(isRowSelected);
            row->update(itemDescription);
        }
    }
    else
    {
        CommandItemComponent *row = 
            new CommandItemComponent(this, this->listBox->getViewport(), itemDescription);
        row->setSelected(isRowSelected);
        return row;
    }

    return existingComponentToUpdate;
}

int ToolsSidebar::getNumRows()
{
    return this->commandDescriptions.size();
}

void ToolsSidebar::paintListBoxItem(int rowNumber,
    Graphics &g, int width, int height, bool rowIsSelected)
{
}

//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void ToolsSidebar::handleAsyncUpdate()
{
    if (this->isTimerRunning())
    {
        const double systemTimeOffset = (Time::getMillisecondCounter() - this->timerStartSystemTime);
        const double currentTimeMs(this->timerStartSeekTime + systemTimeOffset);
        this->currentTime->setText(Transport::getTimeString(currentTimeMs), dontSendNotification);
    }
    else
    {
        this->currentTime->setText(Transport::getTimeString(this->lastSeekTime), dontSendNotification);
    }

    this->totalTime->setText(Transport::getTimeString(this->lastTotalTime), dontSendNotification);
}

//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void ToolsSidebar::onSeek(double absolutePosition,
    double currentTimeMs, double totalTimeMs)
{
    this->lastSeekTime = currentTimeMs; // todo locks?
    this->lastTotalTime = totalTimeMs;
    this->triggerAsyncUpdate();
}

void ToolsSidebar::onTempoChanged(double newTempo)
{

}

void ToolsSidebar::onTotalTimeChanged(double timeMs)
{
    this->lastTotalTime = timeMs;
    this->totalTime->setText(Transport::getTimeString(this->lastTotalTime), dontSendNotification);
}

void ToolsSidebar::onPlay()
{
    this->timerStartSystemTime = Time::getMillisecondCounter();
    this->timerStartSeekTime = this->lastSeekTime;
    this->startTimer(100);
    this->playButton->setPlaying(true);
}

void ToolsSidebar::onStop()
{
    this->stopTimer();
    this->playButton->setPlaying(false);
}

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void ToolsSidebar::changeListenerCallback(ChangeBroadcaster *source)
{
    this->updateModeButtons();
}

//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

void ToolsSidebar::timerCallback()
{
    this->triggerAsyncUpdate();
}

//===----------------------------------------------------------------------===//
// ToolsSidebar
//===----------------------------------------------------------------------===//

void ToolsSidebar::updateModeButtons()
{
    this->recreateCommandDescriptions();
    this->listBox->updateContent();
}

void ToolsSidebar::emitAnnotationsCallout(Component *newAnnotationsMenu)
{
    HelioCallout::emit(newAnnotationsMenu, this->annotationsButton);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ToolsSidebar" template="../../Template"
                 componentName="" parentClasses="public Component, protected TransportListener, protected AsyncUpdater, protected ListBoxModel, protected ChangeListener, protected Timer"
                 constructorParams="ProjectTreeItem &amp;parent" variableInitialisers="project(parent),&#10;lastSeekTime(0.0),&#10;lastTotalTime(0.0),&#10;timerStartSeekTime(0.0),&#10;timerStartSystemTime(0.0)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="64" initialHeight="640">
  <METHODS>
    <METHOD name="childrenChanged()"/>
    <METHOD name="mouseMove (const MouseEvent&amp; e)"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="8f95c631b98e644b" memberName="headBg" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 48" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="19597a6a5daad55d" memberName="bodyBg" virtualName=""
             explicitFocusOrder="0" pos="0 48 0M 48M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="381fa571a3dfc5cd" memberName="listBox" virtualName=""
                    explicitFocusOrder="0" pos="0 49 0M 177M" class="ListBox" params=""/>
  <JUCERCOMP name="" id="28ce45d9e84b729c" memberName="headLine" virtualName=""
             explicitFocusOrder="0" pos="0 47 0M 2" sourceFile="../Themes/SeparatorHorizontalReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="accf780c6ef7ae9e" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 127Rr 0M 6" sourceFile="../Themes/LighterShadowUpwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="15cecf620af99284" memberName="gradient2" virtualName=""
             explicitFocusOrder="0" pos="0 0Rr 0M 127" sourceFile="../Themes/GradientVertical.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="22d481533ce3ecd3" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="0 126Rr 0M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <LABEL name="" id="700073f74a17c931" memberName="totalTime" virtualName=""
         explicitFocusOrder="0" pos="0Cc 18Rc 72 28" textCol="77ffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="16" kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="b9e867ece7f52ad8" memberName="currentTime" virtualName=""
         explicitFocusOrder="0" pos="0Cc 46Rc 72 32" textCol="77ffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="28" kerning="0" bold="0" italic="0" justification="36"/>
  <JUCERCOMP name="" id="bb2e14336f795a57" memberName="playButton" virtualName=""
             explicitFocusOrder="0" pos="0Cc 92Rc 0M 64" sourceFile="../Common/PlayButton.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1d398dc12e2047bd" memberName="headShadow" virtualName=""
             explicitFocusOrder="0" pos="0 48 0M 6" sourceFile="../Themes/LighterShadowDownwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="f09d886c97d1c017" memberName="gradient" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 47" sourceFile="../Themes/GradientVerticalReversed.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="34c972d7b22acf17" memberName="annotationsButton"
                    virtualName="" explicitFocusOrder="0" pos="0Cc 0 0M 45" class="CommandItemComponent"
                    params="this, nullptr, CommandItem::withParams(Icons::menu, CommandIDs::ShowAnnotations)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//==============================================================================
// Binary resources - be careful not to edit any of these sections!

// JUCER_RESOURCE: gray1x1_png, 150, "../../../../MainLayout/~icons/gray1x1.png"
static const unsigned char resource_ToolsSidebar_gray1x1_png[] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,1,0,0,0,1,8,2,0,0,0,144,119,83,222,0,0,0,9,112,72,89,115,0,0,11,19,0,0,11,19,1,0,
154,156,24,0,0,0,7,116,73,77,69,7,222,4,19,5,8,9,228,2,121,9,0,0,0,29,105,84,88,116,67,111,109,109,101,110,116,0,0,0,0,0,67,114,101,97,116,101,100,32,119,105,116,104,32,71,73,77,80,100,46,101,7,0,0,0,
12,73,68,65,84,8,215,99,136,138,138,2,0,2,32,1,15,53,60,95,243,0,0,0,0,73,69,78,68,174,66,96,130,0,0};

const char* ToolsSidebar::gray1x1_png = (const char*) resource_ToolsSidebar_gray1x1_png;
const int ToolsSidebar::gray1x1_pngSize = 150;
