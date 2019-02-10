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
#include "ProjectNode.h"
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

SequencerSidebarRight::SequencerSidebarRight(ProjectNode &parent)
    : project(parent),
      lastSeekTime(0.0),
      lastTotalTime(0.0),
      timerStartSeekTime(0.0),
      timerStartSystemTime(0.0)
{
    this->listBox.reset(new ListBox());
    this->addAndMakeVisible(listBox.get());

    this->headLine.reset(new SeparatorHorizontalReversed());
    this->addAndMakeVisible(headLine.get());
    this->shadow.reset(new ShadowUpwards(Light));
    this->addAndMakeVisible(shadow.get());
    this->separator.reset(new SeparatorHorizontal());
    this->addAndMakeVisible(separator.get());
    this->totalTime.reset(new Label(String(),
                                     TRANS("...")));
    this->addAndMakeVisible(totalTime.get());
    this->totalTime->setFont(Font (Font::getDefaultSansSerifFontName(), 14.00f, Font::plain).withTypefaceStyle ("Regular"));
    totalTime->setJustificationType(Justification::centred);
    totalTime->setEditable(false, false, false);

    this->currentTime.reset(new Label(String(),
                                       TRANS("...")));
    this->addAndMakeVisible(currentTime.get());
    this->currentTime->setFont(Font (Font::getDefaultSansSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    currentTime->setJustificationType(Justification::centred);
    currentTime->setEditable(false, false, false);

    this->headShadow.reset(new ShadowDownwards(Light));
    this->addAndMakeVisible(headShadow.get());
    this->annotationsButton.reset(new MenuItemComponent(this, nullptr, MenuItem::item(Icons::ellipsis, CommandIDs::ShowAnnotations)));
    this->addAndMakeVisible(annotationsButton.get());

    this->playButton.reset(new PlayButton(nullptr));
    this->addAndMakeVisible(playButton.get());

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

    this->setSize(48, 640);

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
    g.setFillType({ theme.getBgCacheC(), {} });
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

    listBox->setBounds(0, 41, getWidth() - 0, getHeight() - 113);
    headLine->setBounds(0, 39, getWidth() - 0, 2);
    shadow->setBounds(0, getHeight() - 71 - 6, getWidth() - 0, 6);
    separator->setBounds(0, getHeight() - 70 - 2, getWidth() - 0, 2);
    totalTime->setBounds((getWidth() / 2) + 80 - (72 / 2), getHeight() - 9 - 18, 72, 18);
    currentTime->setBounds((getWidth() / 2) + 80 - (72 / 2), getHeight() - 26 - 22, 72, 22);
    headShadow->setBounds(0, 40, getWidth() - 0, 6);
    annotationsButton->setBounds((getWidth() / 2) - ((getWidth() - 0) / 2), 0, getWidth() - 0, 39);
    playButton->setBounds((getWidth() / 2) - (48 / 2), getHeight() - 12 - 48, 48, 48);
    //[UserResized] Add your own custom resize handling here..
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
    default:
        break;
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
void SequencerSidebarRight::recreateMenu()
{
    this->menu.clear();

    const bool defaultMode = this->project.getEditMode().isMode(HybridRollEditMode::defaultMode);
    const bool drawMode = this->project.getEditMode().isMode(HybridRollEditMode::drawMode);
    const bool selectionMode = this->project.getEditMode().isMode(HybridRollEditMode::selectionMode);
    const bool zoomMode = this->project.getEditMode().isMode(HybridRollEditMode::zoomMode);
    const bool dragMode = this->project.getEditMode().isMode(HybridRollEditMode::dragMode);
    const bool scissorsMode = this->project.getEditMode().isMode(HybridRollEditMode::knifeMode);
    const bool eraserMode = this->project.getEditMode().isMode(HybridRollEditMode::eraserMode);

#if HELIO_MOBILE
    this->menu.add(MenuItem::item(Icons::selectionTool, CommandIDs::EditModeSelect)->toggled(selectionMode));
#endif

    this->menu.add(MenuItem::item(Icons::cursorTool, CommandIDs::EditModeDefault)->toggled(defaultMode));
    this->menu.add(MenuItem::item(Icons::drawTool, CommandIDs::EditModeDraw)->toggled(drawMode));
    this->menu.add(MenuItem::item(Icons::dragTool, CommandIDs::EditModePan)->toggled(dragMode));
    this->menu.add(MenuItem::item(Icons::cutterTool, CommandIDs::EditModeKnife)->toggled(scissorsMode));
    //this->menu.add(MenuItem::item(Icons::eraserTool, CommandIDs::EditModeEraser)->toggled(eraserMode));

    if (this->menuMode == PianoRollTools)
    {
        this->menu.add(MenuItem::item(Icons::volume, CommandIDs::ShowVolumePanel));
        this->menu.add(MenuItem::item(Icons::chordBuilder, CommandIDs::ShowChordPanel));
        this->menu.add(MenuItem::item(Icons::arpeggiate, CommandIDs::ShowArpeggiatorsPanel));
        //this->menu.add(MenuItem::item(Icons::script, CommandIDs::RunScriptTransform));

        this->menu.add(MenuItem::item(Icons::copy, CommandIDs::CopyEvents));
        this->menu.add(MenuItem::item(Icons::paste, CommandIDs::PasteEvents));
        this->menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteEvents));
    }
    else if (this->menuMode == PatternRollTools)
    {
        this->menu.add(MenuItem::item(Icons::copy, CommandIDs::CopyClips));
        this->menu.add(MenuItem::item(Icons::paste, CommandIDs::PasteClips));
        this->menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteClips));
    }

    this->menu.add(MenuItem::item(Icons::undo, CommandIDs::Undo));
    this->menu.add(MenuItem::item(Icons::redo, CommandIDs::Redo));
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

void SequencerSidebarRight::onTempoChanged(double msPerQuarter) {}

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
    this->recreateMenu();
    this->listBox->updateContent();
}

void SequencerSidebarRight::emitAnnotationsCallout(Component *newAnnotationsMenu)
{
    HelioCallout::emit(newAnnotationsMenu, this->annotationsButton.get());
}

void SequencerSidebarRight::setLinearMode()
{
    this->menuMode = PianoRollTools;
    this->recreateMenu();
    this->listBox->updateContent();
}

void SequencerSidebarRight::setPatternMode()
{
    this->menuMode = PatternRollTools;
    this->recreateMenu();
    this->listBox->updateContent();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SequencerSidebarRight" template="../../../Template"
                 componentName="" parentClasses="public Component, protected TransportListener, protected AsyncUpdater, protected ListBoxModel, protected ChangeListener, protected Timer"
                 constructorParams="ProjectNode &amp;parent" variableInitialisers="project(parent),&#10;lastSeekTime(0.0),&#10;lastTotalTime(0.0),&#10;timerStartSeekTime(0.0),&#10;timerStartSystemTime(0.0)"
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
             explicitFocusOrder="0" pos="0 71Rr 0M 6" sourceFile="../../Themes/ShadowUpwards.cpp"
             constructorParams="Light"/>
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
             explicitFocusOrder="0" pos="0 40 0M 6" sourceFile="../../Themes/ShadowDownwards.cpp"
             constructorParams="Light"/>
  <GENERICCOMPONENT name="" id="34c972d7b22acf17" memberName="annotationsButton"
                    virtualName="" explicitFocusOrder="0" pos="0Cc 0 0M 39" class="MenuItemComponent"
                    params="this, nullptr, MenuItem::item(Icons::ellipsis, CommandIDs::ShowAnnotations)"/>
  <JUCERCOMP name="" id="bb2e14336f795a57" memberName="playButton" virtualName=""
             explicitFocusOrder="0" pos="0Cc 12Rr 48 48" sourceFile="../../Common/PlayButton.cpp"
             constructorParams="nullptr"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
