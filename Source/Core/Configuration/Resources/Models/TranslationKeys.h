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

#pragma once

namespace I18n
{
    // ofc hashes may collide, but hopefully they don't;
    // translations deserialization has assertions for that

    using Key = uint32;

    namespace Common
    {
        static constexpr auto conjunction = constexprHash("common::and");
        static constexpr auto yesterday = constexprHash("common::yesterday");
        static constexpr auto supportProject = constexprHash("common::support");
        static constexpr auto updateProceed = constexprHash("update::proceed");
        static constexpr auto networkError = constexprHash("common::networkerror");
    }

    namespace Defaults
    {
        static constexpr auto newProjectFirstCommit = constexprHash("defaults::newproject::firstcommit");
        static constexpr auto newProjectName = constexprHash("defaults::newproject::name");
        static constexpr auto midiTrackName = constexprHash("defaults::newtrack::name");
        static constexpr auto tempoTrackName = constexprHash("defaults::tempotrack::name");
    }

    namespace Dialog
    {
        static constexpr auto cancel = constexprHash("dialog::common::cancel");
        static constexpr auto apply = constexprHash("dialog::common::apply");
        static constexpr auto delete_ = constexprHash("dialog::common::delete");
        static constexpr auto add = constexprHash("dialog::common::add");
        static constexpr auto save = constexprHash("dialog::common::save");

        static constexpr auto addArpCaption = constexprHash("dialog::addarp::caption");
        static constexpr auto addArpProceed = constexprHash("dialog::addarp::proceed");
        static constexpr auto addTrackCaption = constexprHash("dialog::addtrack::caption");
        static constexpr auto annotationAddCaption = constexprHash("dialog::annotation::add::caption");
        static constexpr auto annotationEditCaption = constexprHash("dialog::annotation::edit::caption");
        static constexpr auto annotationRenameCancel = constexprHash("dialog::annotation::rename::cancel");
        static constexpr auto annotationRenameCaption = constexprHash("dialog::annotation::rename::caption");
        static constexpr auto annotationRenameProceed = constexprHash("dialog::annotation::rename::proceed");
        static constexpr auto authGithub = constexprHash("dialog::auth::github");
        static constexpr auto deleteProjectCaption = constexprHash("dialog::deleteproject::caption");
        static constexpr auto deleteProjectConfirmCaption = constexprHash("dialog::deleteproject::confirm::caption");
        static constexpr auto documentExport = constexprHash("dialog::document::export");
        static constexpr auto documentExportDone = constexprHash("dialog::document::export::done");
        static constexpr auto documentImport = constexprHash("dialog::document::import");
        static constexpr auto documentLoad = constexprHash("dialog::document::load");
        static constexpr auto documentSave = constexprHash("dialog::document::save");
        static constexpr auto instrumentRenameCaption = constexprHash("dialog::instrument::rename::caption");
        static constexpr auto instrumentRenameProceed = constexprHash("dialog::instrument::rename::proceed");
        static constexpr auto keySignatureAddCaption = constexprHash("dialog::keysignature::add::caption");
        static constexpr auto keySignatureEditCaption = constexprHash("dialog::keysignature::edit::caption");
        static constexpr auto openglCaption = constexprHash("dialog::opengl::caption");
        static constexpr auto openglProceed = constexprHash("dialog::opengl::proceed");
        static constexpr auto renameTrackCaption = constexprHash("dialog::renametrack::caption");
        static constexpr auto renameTrackProceed = constexprHash("dialog::renametrack::proceed");
        static constexpr auto renderAbort = constexprHash("dialog::render::abort");
        static constexpr auto renderCaption = constexprHash("dialog::render::caption");
        static constexpr auto renderProceed = constexprHash("dialog::render::proceed");
        static constexpr auto scanFolderCaption = constexprHash("dialog::scanfolder::caption");
        static constexpr auto timeSignatureAddCaption = constexprHash("dialog::timesignature::add::caption");
        static constexpr auto timeSignatureEditCaption = constexprHash("dialog::timesignature::edit::caption");
        static constexpr auto vcsCheckoutProceed = constexprHash("dialog::vcs::checkout::proceed");
        static constexpr auto vcsCheckoutWarning = constexprHash("dialog::vcs::checkout::warning");
        static constexpr auto vcsCommitCaption = constexprHash("dialog::vcs::commit::caption");
        static constexpr auto vcsCommitProceed = constexprHash("dialog::vcs::commit::proceed");
        static constexpr auto vcsResetCaption = constexprHash("dialog::vcs::reset::caption");
        static constexpr auto vcsResetProceed = constexprHash("dialog::vcs::reset::proceed");
        static constexpr auto workspaceCreateProjectCaption = constexprHash("dialog::workspace::createproject::caption");
        static constexpr auto setTempoCaption = constexprHash("dialog::tempo::caption");
        static constexpr auto setTempoTapLabel = constexprHash("dialog::tempo::tap");
    }

    namespace Menu
    {
        static constexpr auto back = constexprHash("menu::back");
        static constexpr auto cancel = constexprHash("menu::cancel");

        static constexpr auto copy = constexprHash("menu::copy");
        static constexpr auto cut = constexprHash("menu::cut");
        static constexpr auto delete_ = constexprHash("menu::delete");
        static constexpr auto paste = constexprHash("menu::paste");

        static constexpr auto presets = constexprHash("menu::presets");
        static constexpr auto savePreset = constexprHash("menu::savepreset");

        static constexpr auto annotationAdd = constexprHash("menu::annotation::add");
        static constexpr auto annotationDelete = constexprHash("menu::annotation::delete");
        static constexpr auto annotationRename = constexprHash("menu::annotation::rename");
        static constexpr auto arpeggiatorsCreate = constexprHash("menu::arpeggiators::create");
        static constexpr auto groupByColour = constexprHash("menu::groupby::colour");
        static constexpr auto groupByInstrument = constexprHash("menu::groupby::instrument");
        static constexpr auto groupByName = constexprHash("menu::groupby::name");
        static constexpr auto groupByNone = constexprHash("menu::groupby::none");

        static constexpr auto instrumentAddEffect = constexprHash("menu::instrument::addeffect");
        static constexpr auto instrumentAdd = constexprHash("menu::instrument::addinstrument");
        static constexpr auto instrumentDelete = constexprHash("menu::instrument::delete");
        static constexpr auto instrumentRename = constexprHash("menu::instrument::rename");
        static constexpr auto instrumentSetColour = constexprHash("menu::instrument::setcolour");
        static constexpr auto instrumentSetIcon = constexprHash("menu::instrument::seticon");
        static constexpr auto instrumentShowEditor = constexprHash("menu::instrument::showeditor");
        static constexpr auto instrumentShowWindow = constexprHash("menu::instrument::showui");
        static constexpr auto instrumentsReload = constexprHash("menu::instruments::reload");
        static constexpr auto instrumentsScanFolder = constexprHash("menu::instruments::scanfolder");

        static constexpr auto keyboardMappingEdit = constexprHash("menu::kbm::edit");
        static constexpr auto keyboardMappingLoadScala = constexprHash("menu::kbm::load");
        static constexpr auto keyboardMappingReset = constexprHash("menu::kbm::reset");

        static constexpr auto keySignatureAdd = constexprHash("menu::keysignature::add");

        static constexpr auto refactoringInverseDown = constexprHash("menu::refactoring::inversedown");
        static constexpr auto refactoringInverseUp = constexprHash("menu::refactoring::inverseup");
        static constexpr auto refactoringMelodicInversion = constexprHash("menu::refactoring::inversion");
        static constexpr auto refactoringRetrograde = constexprHash("menu::refactoring::retrograde");
        static constexpr auto refactoringCleanup = constexprHash("menu::refactoring::cleanup");
        static constexpr auto refactoringInScaleTransposeUp = constexprHash("menu::refactoring::inscalekeyup");
        static constexpr auto refactoringInScaleTransposeDown = constexprHash("menu::refactoring::inscalekeydown");

        namespace Project
        {
            static constexpr auto addAutomation = constexprHash("menu::project::addautomation");
            static constexpr auto addItems = constexprHash("menu::project::additems");
            static constexpr auto addTrack = constexprHash("menu::project::addlayer");
            static constexpr auto addTempo = constexprHash("menu::project::addtempo");
            static constexpr auto changeInstrument = constexprHash("menu::project::change::instrument");
            static constexpr auto changeTemperament = constexprHash("menu::project::change::tmpr");
            static constexpr auto convertTemperament = constexprHash("menu::project::convert::tmpr");
            static constexpr auto deleteConfirm = constexprHash("menu::project::delete");
            static constexpr auto deleteCancelled = constexprHash("menu::project::delete::cancelled");
            static constexpr auto editorLinear = constexprHash("menu::project::editor::linear");
            static constexpr auto editorPattern = constexprHash("menu::project::editor::pattern");
            static constexpr auto editorVcs = constexprHash("menu::project::editor::vcs");
            static constexpr auto importMidi = constexprHash("menu::project::import::midi");
            static constexpr auto refactor = constexprHash("menu::project::refactor");
            static constexpr auto render = constexprHash("menu::project::render");
            static constexpr auto renderFlac = constexprHash("menu::project::render::flac");
            static constexpr auto renderMidi = constexprHash("menu::project::render::midi");
            static constexpr auto renderSavedTo = constexprHash("menu::project::render::savedto");
            static constexpr auto renderWav = constexprHash("menu::project::render::wav");
            static constexpr auto transposeDown = constexprHash("menu::project::transpose::down");
            static constexpr auto transposeUp = constexprHash("menu::project::transpose::up");
            static constexpr auto unload = constexprHash("menu::project::unload");
        }

        namespace Selection
        {
            static constexpr auto clipsEdit = constexprHash("menu::selection::clips::edit");
            static constexpr auto clipsTransposeDown = constexprHash("menu::selection::clips::transpose::down");
            static constexpr auto clipsTransposeUp = constexprHash("menu::selection::clips::transpose::up");
            static constexpr auto notes = constexprHash("menu::selection::notes");
            static constexpr auto notesArpeggiate = constexprHash("menu::selection::notes::arpeggiate");
            static constexpr auto notesDivisions = constexprHash("menu::selection::notes::divisions");
            static constexpr auto notesQuantizeTo = constexprHash("menu::selection::notes::quantize");
            static constexpr auto notesMoveTo = constexprHash("menu::selection::notes::move");
            static constexpr auto notesRefactor = constexprHash("menu::selection::notes::refactor");
            static constexpr auto notesRescale = constexprHash("menu::selection::notes::rescale");
            static constexpr auto notesToTrack = constexprHash("menu::selection::notes::totrack");
            static constexpr auto pluginInit = constexprHash("menu::selection::plugin::init");
            static constexpr auto pluginPlug = constexprHash("menu::selection::plugin::plug");
            static constexpr auto pluginRemove = constexprHash("menu::selection::plugin::remove");
            static constexpr auto routeDisconnect = constexprHash("menu::selection::route::disconnect");
            static constexpr auto routeGetAudio = constexprHash("menu::selection::route::getaudio");
            static constexpr auto routeGetMidi = constexprHash("menu::selection::route::getmidi");
            static constexpr auto routeRemove = constexprHash("menu::selection::route::remove");
            static constexpr auto routeSendaudio = constexprHash("menu::selection::route::sendaudio");
            static constexpr auto routeSendmidi = constexprHash("menu::selection::route::sendmidi");
            static constexpr auto vcsCheckout = constexprHash("menu::selection::vcs::checkout");
            static constexpr auto vcsCommit = constexprHash("menu::selection::vcs::commit");
            static constexpr auto vcsHistory = constexprHash("menu::selection::vcs::history");
            static constexpr auto vcsPull = constexprHash("menu::selection::vcs::pull");
            static constexpr auto vcsPush = constexprHash("menu::selection::vcs::push");
            static constexpr auto vcsReset = constexprHash("menu::selection::vcs::reset");
            static constexpr auto vcsSelectAll = constexprHash("menu::selection::vcs::selectall");
            static constexpr auto vcsSelectNone = constexprHash("menu::selection::vcs::selectnone");
            static constexpr auto vcsStage = constexprHash("menu::selection::vcs::stage");
        }

        static constexpr auto timeSignatureAdd = constexprHash("menu::timesignature::add");
        static constexpr auto timeSignatureChange = constexprHash("menu::timesignature::change");
        static constexpr auto timeSignatureDelete = constexprHash("menu::timesignature::delete");
        static constexpr auto trackChangeInstrument = constexprHash("menu::track::change::instrument");
        static constexpr auto trackDelete = constexprHash("menu::track::delete");
        static constexpr auto trackRename = constexprHash("menu::track::rename");
        static constexpr auto trackDuplicate = constexprHash("menu::track::duplicate");
        static constexpr auto trackSelectall = constexprHash("menu::track::selectall");
        static constexpr auto tuplet1 = constexprHash("menu::tuplet::1");
        static constexpr auto tuplet2 = constexprHash("menu::tuplet::2");
        static constexpr auto tuplet3 = constexprHash("menu::tuplet::3");
        static constexpr auto tuplet4 = constexprHash("menu::tuplet::4");
        static constexpr auto tuplet5 = constexprHash("menu::tuplet::5");
        static constexpr auto tuplet6 = constexprHash("menu::tuplet::6");
        static constexpr auto tuplet7 = constexprHash("menu::tuplet::7");
        static constexpr auto tuplet8 = constexprHash("menu::tuplet::8");
        static constexpr auto tuplet9 = constexprHash("menu::tuplet::9");
        static constexpr auto quantizeTo1_1 = constexprHash("menu::quantize::1");
        static constexpr auto quantizeTo1_2 = constexprHash("menu::quantize::2");
        static constexpr auto quantizeTo1_4 = constexprHash("menu::quantize::4");
        static constexpr auto quantizeTo1_8 = constexprHash("menu::quantize::8");
        static constexpr auto quantizeTo1_16 = constexprHash("menu::quantize::16");
        static constexpr auto quantizeTo1_32 = constexprHash("menu::quantize::32");
        static constexpr auto mute = constexprHash("menu::mute");
        static constexpr auto solo = constexprHash("menu::solo");
        static constexpr auto unmute = constexprHash("menu::unmute");
        static constexpr auto unsolo = constexprHash("menu::unsolo");
        static constexpr auto setOneTempo = constexprHash("menu::onetempo");

        static constexpr auto vcsChangesHide = constexprHash("menu::vcs::changes::hide");
        static constexpr auto vcsChangesShow = constexprHash("menu::vcs::changes::show");
        static constexpr auto vcsChangesToggle = constexprHash("menu::vcs::changes::toggle");

        static constexpr auto vcsCommitAll = constexprHash("menu::vcs::commitall");
        static constexpr auto vcsResetAll = constexprHash("menu::vcs::resetall");
        static constexpr auto vcsSyncAll = constexprHash("menu::vcs::syncall");
        static constexpr auto workspaceProjectCreate = constexprHash("menu::workspace::project::create");
        static constexpr auto workspaceProjectOpen = constexprHash("menu::workspace::project::open");
    }

    namespace Page
    {
        static constexpr auto orchestraCategory = constexprHash("page::orchestra::category");
        static constexpr auto orchestraFormat = constexprHash("page::orchestra::format");
        static constexpr auto orchestraInstruments = constexprHash("page::orchestra::instruments");
        static constexpr auto orchestraPlugins = constexprHash("page::orchestra::plugins");
        static constexpr auto orchestraVendorandname = constexprHash("page::orchestra::vendorandname");
        static constexpr auto projectAuthor = constexprHash("page::project::author");
        static constexpr auto projectDefaultAuthor = constexprHash("page::project::default::author");
        static constexpr auto projectDefaultLicense = constexprHash("page::project::default::license");
        static constexpr auto projectDefaultValueDesktop = constexprHash("page::project::default::value::desktop");
        static constexpr auto projectDefaultValueMobile = constexprHash("page::project::default::value::mobile");
        static constexpr auto projectDescription = constexprHash("page::project::description");
        static constexpr auto projectDuration = constexprHash("page::project::duration");
        static constexpr auto projectFilelocation = constexprHash("page::project::filelocation");
        static constexpr auto projectLicense = constexprHash("page::project::license");
        static constexpr auto projectStartdate = constexprHash("page::project::startdate");
        static constexpr auto projectStatsContent = constexprHash("page::project::stats::content");
        static constexpr auto projectStatsVcs = constexprHash("page::project::stats::vcs");
        static constexpr auto projectTitle = constexprHash("page::project::title");
        static constexpr auto projectTemperament = constexprHash("page::project::temperament");
    }

    namespace Popup
    {
        static constexpr auto cancelled = constexprHash("popup::cancelled");
        static constexpr auto chordFunction1 = constexprHash("popup::chord::function::1");
        static constexpr auto chordFunction2 = constexprHash("popup::chord::function::2");
        static constexpr auto chordFunction3 = constexprHash("popup::chord::function::3");
        static constexpr auto chordFunction4 = constexprHash("popup::chord::function::4");
        static constexpr auto chordFunction5 = constexprHash("popup::chord::function::5");
        static constexpr auto chordFunction6 = constexprHash("popup::chord::function::6");
        static constexpr auto chordFunction7 = constexprHash("popup::chord::function::7");
        static constexpr auto chordRootKey = constexprHash("popup::chord::rootkey");
    }

    namespace Settings
    {
        static constexpr auto audio = constexprHash("settings::audio");
        static constexpr auto audioBufferSize = constexprHash("settings::audio::buffersize");
        static constexpr auto audioDevice = constexprHash("settings::audio::device");
        static constexpr auto audioDriver = constexprHash("settings::audio::driver");
        static constexpr auto audioSampleRate = constexprHash("settings::audio::samplerate");
        static constexpr auto midiRecord = constexprHash("settings::midi::record");
        static constexpr auto midiOutput = constexprHash("settings::midi::output");
        static constexpr auto midiOutputNone = constexprHash("settings::midi::output::none");
        static constexpr auto midiNoDevicesFound = constexprHash("settings::midi::nodevices");
        static constexpr auto midiRemap12ToneKeyboard = constexprHash("settings::midi::remap12tone");
        static constexpr auto languageHelp = constexprHash("settings::language::help");
        static constexpr auto rendererOpengl = constexprHash("settings::renderer::opengl");
        static constexpr auto sync = constexprHash("settings::sync");
        static constexpr auto restartRequired = constexprHash("settings::restart");
        static constexpr auto ui = constexprHash("settings::ui");
        static constexpr auto uiFont = constexprHash("settings::ui::font");
        static constexpr auto nativeTitleBar = constexprHash("settings::ui::nativebar");
        static constexpr auto uiAnimations = constexprHash("settings::ui::animations");
        static constexpr auto mouseWheelPanningByDefault = constexprHash("settings::ui::wheel::panning");
        static constexpr auto mouseWheelVerticalPanningByDefault = constexprHash("settings::ui::wheel::verticalpan");
        static constexpr auto mouseWheelVerticalZoomingByDefault = constexprHash("settings::ui::wheel::verticalzoom");
        static constexpr auto checkForUpdates = constexprHash("settings:checkupdates");
    }

    namespace Tree
    {
        static constexpr auto instruments = constexprHash("tree::instruments");
        static constexpr auto patterns = constexprHash("tree::patterns");
        static constexpr auto root = constexprHash("tree::root");
        static constexpr auto settings = constexprHash("tree::settings");
        static constexpr auto vcs = constexprHash("tree::vcs");
        static constexpr auto keyboardMapping = constexprHash("tree::keyMap");
    }

    namespace Instruments
    {
        static constexpr auto defultSynthTitle = constexprHash("instruments::defaultsynth::title");
        static constexpr auto metronomeTitle = constexprHash("instruments::metronome::title");
        static constexpr auto metronomeBuiltInSoundPlaceholder = constexprHash("instruments::metronome::builtin");
    }

    namespace VCS
    {
        static constexpr auto deltaTypeAdded = constexprHash("vcs::delta::type::added");
        static constexpr auto deltaTypeChanged = constexprHash("vcs::delta::type::changed");
        static constexpr auto deltaTypeRemoved = constexprHash("vcs::delta::type::removed");
        static constexpr auto historyCaption = constexprHash("vcs::history::caption");
        static constexpr auto stageCaption = constexprHash("vcs::stage::caption");
        static constexpr auto syncDone = constexprHash("vcs::sync::done");
        static constexpr auto syncUptodate = constexprHash("vcs::sync::uptodate");

        static constexpr auto warningCannotCommit = constexprHash("vcs::warning::cannotcommit");
        static constexpr auto warningCannotReset = constexprHash("vcs::warning::cannotreset");
        static constexpr auto warningCannotRevert = constexprHash("vcs::warning::cannotrevert");

        static constexpr auto allChangesStashed = constexprHash("vcs::stage::stashed");
        static constexpr auto allChangesRestored = constexprHash("vcs::stage::unstashed");

        static constexpr auto projectMetadata = constexprHash("vcs::items::projectinfo");
        static constexpr auto projectTimeline = constexprHash("vcs::items::timeline");
    }

    namespace CommandPalette
    {
        static constexpr auto projects = constexprHash("console::projects");
        static constexpr auto timeline = constexprHash("console::timeline");
        static constexpr auto chordBuilder = constexprHash("console::chordbuilder");
        static constexpr auto moveNotes = constexprHash("console::movenotes");

        static constexpr auto chordSuggestion = constexprHash("chord::suggestion");
        static constexpr auto chordGenerate = constexprHash("chord::compile");

        static constexpr auto toggleMute = constexprHash("toggle::mute");
        static constexpr auto toggleSolo = constexprHash("toggle::solo");

        static constexpr auto toggleScalesHighlighting = constexprHash("toggle::scaleshl");
        static constexpr auto toggleNoteNameGuides = constexprHash("toggle::noteguides");
        static constexpr auto toggleLoopOverSelection = constexprHash("toggle::loopselection");
    }

    namespace Tooltips
    {
        static constexpr auto hotkey = constexprHash("tooltip::hotkey");

        static constexpr auto switchRolls = constexprHash("tooltip::switchrolls");

        static constexpr auto zoomIn = constexprHash("tooltip::zoomin");
        static constexpr auto zoomOut = constexprHash("tooltip::zoomout");
        static constexpr auto zoomToFit = constexprHash("tooltip::zoomtofit");
        static constexpr auto jumpToNextAnchor = constexprHash("tooltip::jumpnext");
        static constexpr auto jumpToPrevAnchor = constexprHash("tooltip::jumpprev");
        static constexpr auto toggleScalesHighlighting = constexprHash("tooltip::scaleshl");
        static constexpr auto toggleNoteGuides = constexprHash("tooltip::noteguides");
        static constexpr auto toggleVolumePanel = constexprHash("tooltip::volumepanel");
        static constexpr auto toggleMiniMap = constexprHash("tooltip::minimap");

        static constexpr auto togglePlaybackLoop = constexprHash("tooltip::toggleloop");

        static constexpr auto editModeCursor = constexprHash("tooltip::edit::cursor");
        static constexpr auto editModePen = constexprHash("tooltip::edit::pen");
        static constexpr auto editModeDrag = constexprHash("tooltip::edit::drag");
        static constexpr auto editModeKnife = constexprHash("tooltip::edit::cut");
        static constexpr auto chordTool = constexprHash("tooltip::chordtool");
        static constexpr auto arpeggiators = constexprHash("tooltip::arps");
        static constexpr auto addTrack = constexprHash("tooltip::addtrack");
        static constexpr auto metronome = constexprHash("tooltip::metronome");

        static constexpr auto recordingMode = constexprHash("tooltip::recording");
        static constexpr auto playbackMode = constexprHash("tooltip::playback");
    }
} // namespace Translations
