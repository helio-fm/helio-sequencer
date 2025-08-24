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
#include "SoundFontSynthAudioPlugin.h"
#include "BuiltInSynthsPluginFormat.h"
#include "DocumentHelpers.h"
#include "ModeIndicatorComponent.h"
#include "MobileComboBox.h"
#include "PlayButton.h"
#include "IconButton.h"
#include "SerializationKeys.h"
#include "Workspace.h"
#include "HelioTheme.h"

const String SoundFontSynthAudioPlugin::instrumentId = "<soundfont-player>";
const String SoundFontSynthAudioPlugin::instrumentName = "SoundFont Player";

//===----------------------------------------------------------------------===//
// A simple UI allowing to pick a file and switch between presets, if any
//===----------------------------------------------------------------------===//

class SoundFontSynthEditor final : public AudioProcessorEditor
{
public:

    explicit SoundFontSynthEditor(WeakReference<SoundFontSynthAudioPlugin> soundFontPlugin) :
        AudioProcessorEditor(soundFontPlugin),
        audioPlugin(soundFontPlugin)
    {
        this->filePathEditor = HelioTheme::makeSingleLineTextEditor(false);
        this->addAndMakeVisible(this->filePathEditor.get());

        this->browseButton = make<IconButton>(Icons::browse, CommandIDs::Browse);
        this->browseButton->setMouseCursor(MouseCursor::PointingHandCursor);
        this->addAndMakeVisible(this->browseButton.get());

        // non-editable text editor instead of a label just to have a nice frame
        this->programNameLabel = HelioTheme::makeSingleLineTextEditor(false);
        this->programNameLabel->setMouseCursor(MouseCursor::PointingHandCursor);
        this->addAndMakeVisible(this->programNameLabel.get());

        this->programsComboBox = make<MobileComboBox::Container>();
        this->addAndMakeVisible(this->programsComboBox.get());

        this->syncDataWithAudioPlugin();
        this->setSize(640, 120);
    }

    void syncDataWithAudioPlugin()
    {
        const auto numPrograms = this->audioPlugin->getNumPrograms();

        const bool hasPresetSelection = numPrograms > 1;
        this->programNameLabel->setVisible(hasPresetSelection);
        this->programNameLabel->setText(this->audioPlugin->getCurrentProgramName(), dontSendNotification);

        MenuPanel::Menu programsMenu;
        for (int i = 0; i < numPrograms; ++i)
        {
            const auto programName = this->audioPlugin->getProgramName(i);
            programsMenu.add(MenuItem::item(Icons::empty, CommandIDs::SelectPreset + i, programName));
        }

        const auto programsMenuCurrentItem = [this]()
        {
            jassert(this->audioPlugin->getNumPrograms() > 0);
            return jmax(0,
                jmin(this->audioPlugin->getNumPrograms(),
                    this->audioPlugin->getSynthParameters().programIndex));
        };

        this->programsComboBox->initWith(this->programNameLabel.get(),
            programsMenu, move(programsMenuCurrentItem), true);

        const auto synthParams = this->audioPlugin->getSynthParameters();
        if (synthParams.filePath.isNotEmpty())
        {
            this->filePathEditor->setAlpha(1.f);
            this->filePathEditor->setInterceptsMouseClicks(true, true);
#if JUCE_IOS
            this->filePathEditor->setText(File(synthParams.filePath).getFileName());
#else
            this->filePathEditor->setText(synthParams.filePath);
#endif
        }
        else
        {
            this->filePathEditor->setAlpha(0.75f);
            this->filePathEditor->setInterceptsMouseClicks(false, false);
            this->filePathEditor->setText("...");
        }
    }

    void resized() override
    {
        const auto getRowArea = [this](float proportionOfHeight, int height, int padding = 30)
        {
            const auto area = this->getLocalBounds().reduced(padding);
            const auto y = area.proportionOfHeight(proportionOfHeight);
            return area.withHeight(height).translated(0, y - height / 2);
        };

        constexpr auto rowHeight = 32;
        constexpr auto iconWidth = 30;
        constexpr auto iconMarginX = 8;
        constexpr auto paddingX = 6;

        auto browseFileArea = getRowArea(0.15f, rowHeight);
        this->browseButton->setBounds(browseFileArea.removeFromRight(iconWidth).reduced(paddingX, 2).translated(iconMarginX, 0));
        this->filePathEditor->setBounds(browseFileArea.reduced(paddingX, 0));

        auto selectProgramArea = getRowArea(0.7f, rowHeight);
        this->programNameLabel->setBounds(selectProgramArea.reduced(paddingX, 0));
        this->programsComboBox->setBounds(this->getLocalBounds().reduced(2));
    }

    void paint(Graphics &g) override
    {
        const auto &theme = HelioTheme::getCurrentTheme();
        g.setFillType({ theme.getPageBackgroundA(), {} });
        g.fillRect(this->getLocalBounds());
    }

    void handleCommandMessage(int commandId) override
    {
        const auto synthParams = this->audioPlugin->getSynthParameters();

        if (commandId == CommandIDs::Browse)
        {
            this->fileChooser = make<FileChooser>(TRANS(I18n::Dialog::documentLoad),
                this->lastUsedDirectory, ("*.sf2;*.sf3;*.sf4;*.sbk"), true);

            DocumentHelpers::showFileChooser(this->fileChooser,
                Globals::UI::FileChooser::forFileToOpen,
                [this](URL &url)
                {
                    if (!url.isLocalFile() || !url.getLocalFile().exists())
                    {
                        return; // or reset?
                    }

                    const auto file = url.getLocalFile();
                    this->lastUsedDirectory = file.getParentDirectory().getFullPathName();

                    const auto newParams = this->audioPlugin->getSynthParameters()
                        .withSoundFontFile(file.getFullPathName());

                    this->audioPlugin->applySynthParameters(newParams);
                    this->syncDataWithAudioPlugin();
                    App::Workspace().autosave();
                });
        }
        else
        {
            const int presetIndex = commandId - CommandIDs::SelectPreset;
            if (presetIndex >= 0 && presetIndex < this->audioPlugin->getNumPrograms())
            {
                const auto newParams = this->audioPlugin->getSynthParameters().withProgramIndex(presetIndex);
                this->audioPlugin->applySynthParameters(newParams);
                this->syncDataWithAudioPlugin();
                App::Workspace().autosave();
            }
        }
    }

private:

    WeakReference<SoundFontSynthAudioPlugin> audioPlugin;

    // soundfont file picker
    UniquePointer<TextEditor> filePathEditor;
    UniquePointer<IconButton> browseButton;

    UniquePointer<FileChooser> fileChooser;
    String lastUsedDirectory = File::getCurrentWorkingDirectory().getFullPathName();

    // preset selection
    UniquePointer<TextEditor> programNameLabel;
    UniquePointer<MobileComboBox::Container> programsComboBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFontSynthEditor)
};

//===----------------------------------------------------------------------===//
// SoundFontSynthAudioPlugin
//===----------------------------------------------------------------------===//

SoundFontSynthAudioPlugin::SoundFontSynthAudioPlugin()
{
    this->setPlayConfigDetails(0, 2, this->getSampleRate(), this->getBlockSize());
}

void SoundFontSynthAudioPlugin::fillInPluginDescription(PluginDescription &description) const
{
    description.name = this->getName();
    description.descriptiveName = description.name;
    description.uniqueId = description.name.hashCode();
    description.category = "Synth";
    description.pluginFormatName = BuiltInSynthsPluginFormat::formatName;
    description.fileOrIdentifier = BuiltInSynthsPluginFormat::formatIdentifier;
    description.manufacturerName = "Built-in";
    description.version = "1.0";
    description.isInstrument = true;
    description.numInputChannels = this->getTotalNumInputChannels();
    description.numOutputChannels = this->getTotalNumOutputChannels();
}

const String SoundFontSynthAudioPlugin::getName() const
{
    return SoundFontSynthAudioPlugin::instrumentName;
}

void SoundFontSynthAudioPlugin::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
    buffer.clear(0, buffer.getNumSamples());
    this->synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void SoundFontSynthAudioPlugin::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    this->synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SoundFontSynthAudioPlugin::reset()
{
    this->synth.allNotesOff(0, true);
}

void SoundFontSynthAudioPlugin::releaseResources() {}
double SoundFontSynthAudioPlugin::getTailLengthSeconds() const { return 3.0; } // hopefully enough?

bool SoundFontSynthAudioPlugin::acceptsMidi() const { return true; }
bool SoundFontSynthAudioPlugin::producesMidi() const { return false; }

bool SoundFontSynthAudioPlugin::hasEditor() const { return true; }
AudioProcessorEditor *SoundFontSynthAudioPlugin::createEditor()
{
    return new SoundFontSynthEditor(this);
}

//===----------------------------------------------------------------------===//
// Presets
//===----------------------------------------------------------------------===//

int SoundFontSynthAudioPlugin::getNumPrograms()
{
    return this->synth.getNumPrograms();
}

int SoundFontSynthAudioPlugin::getCurrentProgram()
{
    return this->synth.getCurrentProgram();
}

void SoundFontSynthAudioPlugin::setCurrentProgram(int index) 
{
    this->synth.setCurrentProgram(index);
}

const String SoundFontSynthAudioPlugin::getProgramName(int index)
{
    return this->synth.getProgramName(index);
}

const String SoundFontSynthAudioPlugin::getCurrentProgramName()
{
    return this->getProgramName(this->getCurrentProgram());
}

void SoundFontSynthAudioPlugin::changeProgramName(int index, const String &newName)
{
    return this->synth.changeProgramName(index, newName);
}

//===----------------------------------------------------------------------===//
// Parameters
//===----------------------------------------------------------------------===//

void SoundFontSynthAudioPlugin::getStateInformation(MemoryBlock &destData)
{
    const auto state = this->synthParameters.serialize();

    String stateAsString;
    if (this->serializer.saveToString(stateAsString, state).ok())
    {
        MemoryOutputStream outStream(destData, false);
        outStream.writeString(stateAsString);
    }
}

void SoundFontSynthAudioPlugin::setStateInformation(const void *data, int sizeInBytes)
{
    MemoryInputStream inStream(data, sizeInBytes, false);
    const auto stateAsString = inStream.readString();
    const auto state = this->serializer.loadFromString(stateAsString);

    SoundFontSynth::Parameters newParameters;
    newParameters.deserialize(state);

    this->applySynthParameters(newParameters);
}

void SoundFontSynthAudioPlugin::applySynthParameters(const SoundFontSynth::Parameters &newParameters)
{
    if (this->synthParameters.filePath != newParameters.filePath)
    {
        this->synth.initSynth(newParameters);
    }
    else if (this->synthParameters.programIndex != newParameters.programIndex)
    {
        this->setCurrentProgram(newParameters.programIndex);
    }

    this->synthParameters = newParameters;
    // the program # could have been reset to 0 if it was incorrect
    this->synthParameters.programIndex = this->getCurrentProgram();
}

const SoundFontSynth::Parameters &SoundFontSynthAudioPlugin::getSynthParameters() const noexcept
{
    return this->synthParameters;
}
