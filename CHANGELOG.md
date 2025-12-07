Helio features and updates
--------------------------

All notable changes to this project will be documented in this file.
For more detailed info, please see the git log.

## Version 3.17
 - Removed update checks and project sync features from desktop builds, the app is now offline on both desktop and mobile platforms
 - Added initial support for keyboard-only editing in the sequencer, more hotkeys for menus, pop-ups, and dialogs
 - More UI performance improvements
 - Added more translations
 - Desktop platforms: added support for back/forward mouse buttons, fixed VST3 plugin windowing issues, misc fixes in plugin scanning
 - Mobile platforms: enabled an option to render to OGG format, QoL fixes for the velocity editor panel
 - Android: fixed layout flickering on app start
 - iOS: enabled all screen orientations on iPads

## Version 3.16
 - Improved UI performance in the piano roll, fixed glitches, added x1.25 UI scaling option
 - Tidied up the built-in scales list and added more microtonal scales
 - Transposing an entire project via project refactoring menu now also updates all key signatures 
 - Added next/previous preset buttons for the key/time signature dialogs, and the built-in SoundFont player's preset selection
 - Fixed parametric modifiers not updating when the harmonic context changes
 - Mobile platforms: made dialog boxes fit better on small screens
 - iOS: new projects are always created in the app's documents folder to avoid confusion, fixed crashes on adding AudioUnit effects to instruments

## Version 3.15
 - Added an option to display note names in either German or Fixed Do notation (C-D-E or Do-Re-Mi); flats/sharps are now displayed as icons rather than ASCII symbols
 - Added two colour schemes with higher contrast
 - Desktop platforms: window position and size are persisted, fixed undo issues in the chord compiler
 - macOS: fixed scanning custom plugin folders and font switching issues
 - Mobile platforms: sidebars can be stretched sideways to avoid being cut-off by the display notch, scrolling the settings page should now be more convenient
 - iOS: project files were inaccessible via the file picker, and the SoundFont player lost track of SoundFont files between sessions, fixed a couple of iOS-specific crashes
 - Misc QoL tweaks: better support for the "no UI animations" mode, the built-in SoundFont player displays the program selection control as a combo-box, better support for the knife tool

## Version 3.14
 - The built-in SoundFont player now supports microtonal temperaments with no setup
 - The built-in SoundFont player supports SF3 format with OGG/FLAC sample compression
 - Fixed edge cases when converting between temperaments
 - Fixed MIDI export for offbeat tracks
 - Orchestra pit: the plugins list is now searchable, and table sorting is persistent
 - Mobile platforms: added scrollable scale editor for microtonal temperaments for phone screens, added sidebar buttons for tuning and transposing clips in the pattern roll, enabled background audio on iOS
 - Fixed misc annoyances: fixed crashes on pasting empty/invalid data, fixed undo issues after VCS checkouts, fixed edge cases when switching the editable scope

## Version 3.13
 - Added initial support for parametric modifiers: arpeggiators and most of the refactoring options can now be applied in a non-destructive way
 - Reworked note naming to be more compatible with traditional notation language, at least for chromatic scales in 12-tone temperament
 - Fixed arpeggiators so that they finally work as intended
 - Fixed sending events to MIDI output, also the 'MIDI output' instrument is now created automatically
 - Implemented drawing freeform lasso with Control-dragging
 - Added new UI flags to toggle following playhead, and to lock current zoom level and prevent auto-zooming
 - Double-pressing Space/Enter or long-tapping the play button speeds up playback by 1.5x
 - MIDI import now groups the imported events into tracks by channels and controller numbers
 - More QoL fixes for mobile platforms, more SoundFont and LV2 bugs fixed, more UI tidying up

## Version 3.12
 - This is mostly a mobile fixes release: reworked layouts for phone screens, fixed SoundFont player bugs, fixed import/export crashes, added adaptive and themed icons on Android, reworked multi-touch (better late than never)
 - Added automations editor panel supporting hand-drawing custom curves with the pen tool
 - Volume editor panel now also supports hand-drawing custom ramps and is more consistent with edit modes
 - Both the piano roll and the pattern roll can display the volume editor and the automations editor
 - Added support for MIDI channels (better late than never)
 - Piano roll: the lasso component cannot be zero-width or zero-height, dragged notes snap to the original note's beat, the default note length and volume are persisted
 - Version control: switching to another version now works like a hard reset
 - Added commands to shift tempo tracks 1 BPM up/down (hotkeys Shift + and Shift -)
 - Fixed re-adjusting MIDI input from 12-tone keyboards for microtonality
 - The number of saved undo actions is now configurable
 - Minor feature cutting: removed the updates info control, the legacy undocumented scale preview panel and the alternative spectrogram view

## Version 3.11
 - Added built-in SoundFont player instrument
 - Migrated to JUCE 7 and enabled experimental support for hosting LV2 plugins
 - Microtonality: added built-in 26-edo temperament, fixed Alt + Up/Down transposition hotkeys for microtonal layouts, fixed Pianoteq keyboard mapping presets to match A440 on default settings
 - Piano roll: added staccato and legato commands (Alt + S and Alt + L hotkeys), clicking a single note makes that note's velocity and length the default, holding Alt disables snapping to barlines
 - Pattern roll: improved solo mode indication (implicitly muted tracks are dimmed)
 - Tempo dialog: adjusting the tempo with mouse wheel
 - Render dialog: preview the waveform while rendering
 - Fixed regressions in computing time codes and in the audio rendering code

## Version 3.10
 - Added built-in metronome, toggled by Control + M hotkey, each time signature now has its own editable metronome scheme
 - Added an option to select the MIDI output device
 - Added an option to scale the UI x1.5 or x2
 - Made the update checks optional
 - Updated the app icon
 - Fixed renaming or recoloring multiple tracks in the pattern roll
 - Minor fixes: double click on a tempo track opens "set one tempo" dialog, fixed some grid arrangement glitches and inaccurate time signature alignment, fixed zooming out in large projects, fixed inactive notes blocking double-clicks on the roll

## Version 3.9
 - Added per-track time signatures: hotkey Alt + F2 to add a time signature anchored to track's position
 - Made retrograde hotkey work in the pattern roll, re-ordering all selected clips backwards for each row
 - Implemented auto-fitting the roll view range by zooming it out until it "clicks"
 - Added support for nonoctave tunings (only affect the built-in instrument at the moment)
 - Minor tweaks: more conventional right click menu behavior on multiple selection, improved aligning the dragged clips in the pattern roll, long-overdue UI fixes for smaller phone screens

## Version 3.8
 - Added in-scale transposition commands, hotkeys Alt + Up and Alt + Down
 - Implemented recording MIDI from a 12-tone keyboard in any temperament
 - Improved the pen tool: delete notes and clips with right mouse button
 - Improved the knife tool: merge two clips into one or join notes with right mouse button (long tap on mobile platforms)
 - Added "save preset" button in the key signature editor
 - Mini-map improvements: draw a zoom region with left mouse button and pan the stretched mini-map with right mouse button
 - Minor tweaks: allowed "natural" scrolling in the rolls on macOS, improved file choosers for mobile platforms, updated 19-edo settings, re-ordered the edit modes a bit

## Version 3.7
 - Made the playback loop markers draggable
 - Added 19-edo to built-in temperaments
 - Added keyboard mapping presets menu (with some built-in multi-channel mappings for Pianoteq), allowing to save your own presets
 - Lasso improvements: added a cool selection range dashed indicator in the roll headers and fixed selection behavior when deselecting items with Alt
 - Mouse wheel control improvements: added support for mice with dual-axis wheels and added a separate "vertical zooming by default" UI option
 - Fixed missing note-offs when editing, which broke some VST3 plugins
 - Facelifted the default color schemes 
 - Minor convenience tweaks: global zooming with Alt modifier key, toggling mini-map mode with a click at scroller's free area, transposing by perfect fifth or equivalent with Control + Shift + Up/Down hotkeys

## Version 3.6
 - Fixed VST3 plugins showing black screen and made minor improvements in instrument management
 - Fixed loading the invalid audio settings (which caused issues when using JACK)
 - Fixed the audio setting page not always updating correctly
 - Many minor UI tweaks: made "move notes" menus more convenient so that closer clips are shown first, fixed tooltips in the chord tool, fixed updating with the editable scope in velocity map, fixed the pen tool behavior on mobile platforms

## Version 3.5
 - Disabled all network-related features on mobile platforms
 - Added two UI flags for better control over mouse wheel behavior, notably for using mouse wheel for panning instead of zooming by default
 - Added UI option to disable most of the editor animations
 - Added UI flag to change the bottom mini-map appearance (toggled by B hotkey)
 - Added tooltips to sidebar buttons
 - UI improvements: fixed the pattern roll not updating with time signature changes, prettied up the cut point marks for the knife tool and note resizers on mobile platforms
 - Implemented duplicating automation tracks in the pattern roll
 - Fixed the project length being calculated incorrectly for rendering
 - Fixed font scanning so the app shouldn't freeze on the very first run

## Version 3.4
 - Added horizontal scrolling using shift + mouse wheel
 - When exporting MIDI, the track segments are grouped in the same way as you see them grouped in the pattern roll
 - All instances of selected clips in the pattern roll are now highlighted with the dashed header
 - Added '+' button on the tools sidebar, which shows the new track dialog, or the menu to choose the instrument first; using the quill tool on any empty row will invoke the same menu or dialog
 - Fixed minor UI glitches on MIDI import and checkout, fixed the track range indicator not always updated properly, tweaked hotkeys a bit
 - Added a menu to move selected notes to another track, also available in the command palette with ':' shortcut
 - Better support for file choosers on mobile platforms to allow exporting MIDI file to a custom location, e.g. iCloud container

## Version 3.3
 - Added initial support for custom temperaments, the default list includes 22-EDO and 31-EDO; added commands to switch temperament or convert a piece into any other temperament
 - Added keyboard mapping page for instruments, allowing to overcome the limits of MIDI 1.0 for microtonal music, or just to re-map any key for whatever purposes
 - Forgive me father, for I have synthed: removed the built-in piano samples to reduce binary size (my OCD appreciates this immensely), and added a simple sine-like synth which works in any n-tone equal temperament out of the box; it sucks, but hopefully it will be improved in future
 - Added a command to switch to the most visible track/clip in the viewport, hotkey F3
 - Fixed note previewing for some instruments: now it sends note-off events after a delay
 - Minor UI tweaks: made animations a bit shorter and sharper, fixed several focus issues when deleting tracks, the resizable edge in note components now depends on the note width

## Version 3.2
 - Added tempo dialog with "tap tempo" control, invoked either by clicking at tempo track nodes, or by hitting "Set one tempo" menu item in project->refactor menu or tempo track context menu
 - Fixed the old and super annoying rows tiling bug in OpenGL mode
 - Context menus in the sequencer are now only available via long right-click: instant rclick menu often gets in the way when misclicking / trying to switch to another track
 - Playback: will compute states of all present CC automations at the playhead position and apply them when playback starts, which helps a lot if you use pedal tracks or similar automations
 - Orchestra pit: if some instrument hasn't loaded properly (e.g. plugin file missing), then the last valid instrument settings will be preserved in hope that it can be loaded correctly in the next launches; invalid instruments are shown as greyed out in the orchestra pit page, they also can be deleted, or made valid by adding a plugin node
 - Key signature dialog: right-click on any key will play the key
 - Lots of UI updates: tidied up the project info page and the dashboard, updated the colours list and the breadcrumbs control

## Version 3.1
 - Added right-click context menus in the sequencer, version control and orchestra pit pages: the menu content is always the same as in the breadcrumbs control tail menu
 - Added a command to bring up the current instrument's window, hotkey F4
 - Fixed some more UI glitches - most notably, focusing the piano roll scope on a track
 - Minor bugfixes in MIDI recording

## Version 3.0
 - Added initial support for MIDI recording, hotkey F12
 - Added initial support for looping the playback over the selected scope, hotkey F11
 - Added quantization commands
 - Made annotations lengths adjustable
 - Added timeout for scanning fonts on the first run to avoid the app hanging on some systems
 - Minor UI updates for a prettier look and less glitching; added the command palette button in the headline and the timeline repeat sign
 - More tech debt has been paid down, this part of the update will hopefully be invisible
 - Added some documentation

## Version 2.5
 - This is mostly a bugfix release: addressed some UX inconveniences and fixed a stupid crash
 - Added hotkeys for tweaking lengths of selected notes; also adding a note with a modifier key inserts a new note in a dragging mode
 - Made all hotkey-based beat/length-shift actions depend on the current zoom level
 - Implemented duplicating tracks in an interactive manner
 - Made all dialogs draggable
 - Enabled hardened runtime for macOS builds

## Version 2.4
 - Added the command palette aka goto anything, toggled by ~ hotkey: the default mode is used for quick access to commands for the current context, the '/' mode is used for quick access to projects list, '@' mode to go to any timeline event or focus on any clip, and '!' mode to construct chords from the text description (still experimental)
 - Added an option to display note names for a visual cue, toggled by the default hotkey 'g' (for 'guides')
 - Added an option to disable scales highlighting, toggled by the default hotkey 'h' (for 'highlighting')
 - Removed some shame from my heart by refactoring ugly bits of serialization code, also removed the legacy serializer which definitely did not spark joy, thus dropped support for v1 file format
 - Made it possible to control volume ramping amount with mouse wheel in the velocity map
 - Fixed some issues with switching between system/custom title bar
 - Fine-tuned the performance and piano roll behaviour
 - Added notarization for macOS builds

## Version 2.3
 - Added support for aug/dim chords descriptions
 - Allow switching between custom and native title bar on Windows and Linux
 - Also display breadcrumbs in the title bar if possible to save some screen space
 - Added retrograde and melodic inversion to selection refactoring menu
 - Added shift-drag-to-copy for all timeline events
 - Go to next/previous anchor now jumps over clips in pattern mode
 - Reworked midi import, should fix importing controller tracks, key/time signatures and track names
 - Made plugins search less likely to stuck and cancellable with a click or escape keypress

## Version 2.2
 - Long-overdue implementation of inline velocity map/editor (toggled by V button)
 - Arpeggiators can use advanced options, like length multiplier, inversion, randomness level (shift/ctrl/atl + arp button)
 - Refactored instruments managements to fix rare crashes on the orchestra pit page
 - Fixed shift-drag-to-copy for notes, which was broken even before v2 release
 - Fixed automation curves interpolation
 - Some refactorings for lesser memory usage

## Version 2.1
 - Clips now can be muted/soloed
 - Patterns can be grouped by name/colour/instrument
 - Notes can be split into triplets/quadruplets/quintuplets/etc
 - Binary size is much smaller and more optimized overall, due to unity build

## Version 2.0
 - Spent entire 2018 paying off the tech debt of version 1 (or tech mortgage, if you will), which was pretty much of a POC
 - Changed bundle ids to more consistent ones
 - Rewritten the backend side API's from scratch
 - LOTS of refactorings, performance/stability fixes, etc - hopefully all future updates will be just incremental improvements
