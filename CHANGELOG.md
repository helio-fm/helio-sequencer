Helio features and updates
--------------------------

All notable changes to this project will be documented in this file.
For more detailed info, please see the git log.

## Version 3.0
 - Added initial support for MIDI recording (fucking finally), hotkey F12
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
 - Arpeggiators can use advanced options, like length multiplier, invertion, randomness level (shift/ctrl/atl + arp button)
 - Refactored instruments managements to fix rare crashes on the orchestra pit page
 - Fixed shift-drag-to-copy for notes, which was broken even before v2 release
 - Fixed automation curves interpolation
 - Some refactorings for lesser memory usage

## Version 2.1
 - Clips now can be muted/soloed
 - Patterns can be grouped by name/colour/instrument
 - Notes can be split into triplets/quadruplets/quintuplets/etc
 - Binary size is much smaller and more optimised overall, due to unity build

## Version 2.0
 - Spent entire 2018 paying off the tech debt of version 1 (or tech mortgage, if you will), which was pretty much of a POC
 - Changed bundle ids to more consistent ones
 - Rewritten the backend side API's from scratch
 - LOTS of refactorings, performance/stability fixes, etc - hopefully all future updates will be just incremental improvements
