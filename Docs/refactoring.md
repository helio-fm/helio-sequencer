# Refactoring

This page describes several refactoring options, available via selection menu and hotkeys:

![refactoring-menu]

## Transposition and inversion

Not listed in the menu, but also handy: `Shift + Up` and `Shift + Down` hotkeys transpose the selected notes one octave up or down.

Also, `Alt + Shift + Up` and `Alt + Shift + Down` are used for transposition by a "fifth" - simply put, a transposition by +7 or -7 semitones in the 12-tone temperament, or by the closest equivalent of a perfect fifth in other temperaments, e.g. +18/-18 for 31-edo, etc.

The latter is useful for introducing short-term chord modulations to a "neighboring key" (the key that moves one step either direction on the circle of fifths).

Note that the above hotkeys also work on the pattern roll selection: creating track ["instances"](tips-and-tricks.md#clips-and-track-grouping) and modulating them is one of the easiest way of prototyping the structure of the piece.

### Chord inversion

Use `Control + Up` and `Control + Down` for chord inversion (don't confuse it with [melodic inversion](#melodic-inversion)).

Chord inversion treats selected notes as chord(s); the lowest note in each chord moves one octave up (or the highest note moves one octave down), all others stay in place.

### In-scale transposition

Use `Alt + Up` and `Alt + Down` hotkeys to transpose the selected notes using in-scale keys only:

![inscale-transposition]

The notes which are out of scale will be aligned up or down to the nearest in-scale keys.

### Melodic inversion

`Alt + I` hotkey applies melodic inversion to selected notes. This contrapuntal derivation is better [described on Wikipedia](https://en.wikipedia.org/wiki/Inversion_(music)#Melodies); in short, it "flips" the melodic line upside down.

### Retrograde

`Alt + R` hotkey applies another contrapuntal derivation, [retrograde](https://en.wikipedia.org/wiki/Retrograde_(music)) or "walking backward".

[Retrograde inversion](https://en.wikipedia.org/wiki/Retrograde_inversion), which is "backwards and upside down", can be done by simply applying both inversion and retrograde.

**Tip**: use retrograde hotkey to quickly swap two neighbor chords:

![retrograde-swap-chords]

Or to swap two notes like this:

![retrograde-swap-notes]

Retrograde hotkey also works in the pattern roll, re-ordering all selected clips backwards for each row:

![reverse-clips-order]

### Cleanup overlaps

This option simply removes duplicate notes and corrects lengths in a way that notes do not overlap each other.

### Move to track

Pretty self-explanatory shorthand; note that this function is also available in the [command palette](command-palette.md): hit `:` and select another track to move the selected notes to ("closest" tracks will be listed first for convenience).

## Rescaling

This re-aligns the selected notes into one of the parallel modes of the same tonic.

You can think of it as introducing the in-place [modal interchange](https://en.wikipedia.org/wiki/Borrowed_chord).

As well as the [chord tool](tips-and-tricks.md#chord-tool), re-scaling assumes that the harmonic context is specified correctly. In the example below, the first section is marked as D Dorian, and all the notes in that section are in the key. Any out-of-scale notes will be left in their places.

Note that this function doesn't change the key signature itself. Consider using it for introducing brief or transitory variations to bring more harmonic color.

If you want to re-scale entire section of your piece, use the next option:

#### Quick rescale tool

Once you right-click on any key signature at the timeline, you can choose another scale, into which all the tracks will be translated. This affects all notes of all tracks up to the next key signature (or the very end of the project), and updates the key signature, which may be useful for experiments.

This example shows rescaling, along with some undo/redo to see the difference:

![quick-rescale]

## Arpeggiators

Arpeggiators submenu is available in the notes selection menu. If you have created any arpeggiators, you will also find a button on the right sidebar to apply one of them to selection.

### How arpeggiators work

The idea behind arpeggiators was to remember any custom sequence of notes in their in-scale keys, so that the sequence doesn't depend on the scale anymore, and later apply that scale-agnostic sequence to some chords in whatever different harmonic context.

That said, creating and using arpeggiators is tied to harmonic context, meaning arpeggiators rely on valid key signatures at the timeline.

### Creating an arpeggiator

 * First, make sure you have a key signature, then create any sequence in that key and scale;
 * Select that sequence and hit `Shift + A` hotkey (or select a submenu item) to create a named arpeggiator from it.
 * After that, in any other place, apply that arpeggiator to any other selection (presumably, to some chords).

Arpeggiators is one of the unfinished parts of this app: there's still no convenient way to edit or delete an arpeggiator, other than manually editing your [arpeggiators.json](configs.md#user-configs) in the [documents](index.md#the-projects-directory) directory. Hopefully this will be improved in future; as a workaround for editing arpeggiators, I'm maintaining a separate project just for arpeggiator sequences.


[refactoring-menu]: images/refactoring-menu.png "Selection refactoring menu"
[retrograde-swap-notes]: images/retrograde-swap-notes.png "Swap two neighbor notes with Alt + R hotkey"
[retrograde-swap-chords]: images/retrograde-swap-chords.png "Swap two neighbor chords with Alt + R hotkey"
[reverse-clips-order]: images/reverse-clips-order.png "Retrograde hotkey applied to pattern roll selection"
[inscale-transposition]: images/inscale-transposition.png "In-scale transposition"
[quick-rescale]: images/quick-rescale.png "The quick rescale tool"
