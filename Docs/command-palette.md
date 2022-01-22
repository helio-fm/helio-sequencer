# Command palette

The command palette control is inspired by 'Goto Anywhere' in Sublime Text or 'Locator' in QT Creator, and is meant for quick access to the commands available for the current context, and things like projects and timeline events.

Toggled by `~` or `Control + P` hotkeys by default:

![command-palette]

Besides just commands, it supports special modes, triggered by typing a certain symbol. This symbol also acts as a hotkey to show the command palette in that mode:

* `/` is for the project list,
* `:` is for moving selected notes into another track (target tracks are ordered "closest first"),
* `@` is for the timeline events list + tracks list,
* `!` is for the chord compiler, which deserves a separate description:

## Chord compiler

One of the modes of that command palette allows to generate chords based on the chord name [decoding rules](https://en.wikipedia.org/wiki/Chord_letters). This tool is not aware of any of the key signatures present at the timeline, all the chord's notes are inferred from the given description.

Just hit `!` and start typing a chord description, or pick some of the suggestions it provides:

![chord-compiler]


[chord-compiler]: images/chord-compiler.png "The chord compiler"
[command-palette]: images/command-palette.png "The command palette"
