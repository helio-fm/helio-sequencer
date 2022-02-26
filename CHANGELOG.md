Helio features and updates
--------------------------

All notable changes to this project will be documented in this file.
For more detailed info, please see the git log.

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
