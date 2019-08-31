Helio features and updates
--------------------------

All notable changes to this project will be documented in this file.
For more detailed info, please see the git log.

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
