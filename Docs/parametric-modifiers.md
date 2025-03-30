# Parametric modifiers

Parametric modifiers are another prototyping/sketching tool that allows you to apply effects in a non-destructive way, without affecting the base notes of your sequence.

Add or edit them via the "Modifiers" menu of the selected clip or track. Modifiers can be stacked and reordered there:

![modifiers-menu]

Parametric modifiers are meant for testing [arpeggiators](refactoring.md#arpeggiators) and refactoring options, while keeping the underlying chords editable. Keeping the original notes editable lowers the cost of error, especially on the early stages of development. The goal, as with [version control](getting-started.md#version-control), is to reduce friction while experimenting and sketching the piece.

The generated sequence cannot be edited manually and is displayed as dashed semi-transparent notes. When playback starts, the sequencer switches between display modes for the generated and original notes to indicate which notes are being played:

![modifiers-arp]

*(the main intended use case is prototyping arpeggios for a chord sequence)*

A few tips:
 * The generated sequences are used in playback and MIDI export.
 * Modifiers can be stacked on any individual clip (a.k.a. sequence instance), allowing you to experiment with different effects on the same notes in different places of the project.
 * To quickly switch the playback between the generated and original sequences, mute and unmute all modifiers using the `Alt + M` hotkey.
 * To make all changes permanent and editable, select "Apply all" from the modifiers menu (this basically replaces the original sequence with the generated one and resets the modifiers stack).


[modifiers-menu]: images/modifiers-menu.png "Parametric modifiers menu"
[modifiers-arp]: images/modifiers-arp.png "Arpeggiator applied as a non-destructive modifier"
