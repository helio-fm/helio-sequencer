# Tips and tricks

This page lists features, hacks and nuances, which might be handy, or non-obvious, or both.

## Piano roll

### Warnings

While playback, Helio may sometimes draw red or yellow vertical warning lines:

![warnings]

These are the clipping and oversaturation warning markers:

 - red lines indicate problematic regions with [clipping sound](https://en.wikipedia.org/wiki/Clipping_(audio)),

 - yellow warning lines will appear in the areas where the perceived loudness (a.k.a. the root mean square loudness) is way lower than the peak loudness, which is considered ~~harmful~~ unhealthy: in my setup it typically means that I have some redundant duplicate notes at the same place.

### Spacebar panning

One quick way to switch between the current editing mode and the canvas panning mode is holding the `Space` key:

![space-drag]

Dragging with the right mouse button does similar thing, but it will also switch to another track, if clicked on any semi-transparent note.

### Time measure tool

Hold the `Space` key, then click-and-drag over the timeline to measure time between two points at the timeline:

![time-measure-tool]

### Sound probe tool

Finally, holding `Space` and clicking at the timeline is something I call "sound probe", which is supposed to give an idea of what notes are playing at the given point:

![sound-probe]

### Velocity map fine-tuning

It's possible to use the mouse wheel when dragging the velocity ramping tool to control how it blends with the original notes' velocities. The main use case for that is fine-tuning the dynamics:

![velocity-map-fine-tuning]

### Resizing a group

Resize a group of notes proportionally by holding `Shift`:

![group-resizing]

### Drag-and-copy

Hold `Shift` to drag-and-copy notes in the piano roll, clips in the pattern roll, key/time signatures and annotations at the timeline:

![drag-and-copy]

### Pen tool

Also hold `Shift` or any modifier key to change the behaviour of the pen tool when adding notes. By default, the newly added note is edited in the transpose-and-resize mode. With `Shift`, it's the drag mode, more familiar:

![pen-tool-alt]

### UI flags

A couple of display options are available to provide a visual cue. They are toggled either in the navigation panel, or via hotkeys (`G` and `H` by default).

The first one is for displaying the note name guides:

![note-names]

Another one highlights the in-scale keys of the key signatures that are added at the timeline. If you prefer C Major coloring in the piano roll, just turn it off:

![scales-highlighting]

### More UI flags

The audio monitor view and the project mini-map view will toggle different modes on click or tap.

The audio monitor switches between the simple spectrogram mode and the waveform mode. The mini map is stretched either to fit all project, or to fit some fixed duration into the display area box.

![monitor-and-map]

### Chord tool

By double-clicking on a row in the piano roll, you invoke the chord tool:

![chord-tool]

It pick the current key signature from the timeline to know what scale and root key to use to generate chords. Hence the main limitation of this tool: it can only generate chords that are easy to define with in-scale keys.

It can be dragged around by the center node — kinda helpful if you clicked the wrong row or position.

Since it depends on the harmonic context, it will do nothing when placed on the out-of-scale note (the grey row). It might be a good idea to make sure the [scales highlighting](#ui-flags) is enabled to avoid confusion.

### Quick rescale tool

Another tool for experiments and prototyping is the quick rescale menu: once you right-click on any key signature at the timeline, you can choose another scale, into which all the tracks will be translated. This affects all notes of all tracks up to the next key signature (or the very end of the project).

This example shows rescaling, along with some undo/redo to see the difference:

![quick-rescale]

As well as the chord tool, this tool assumes that the harmonic context is specified correctly. In this example, the first section is marked as D Dorian, and all the notes in that section are in the key. Any out-of-scale notes will be left in their places.

#### See also: [piano roll hotkeys](hotkeys.md#piano-roll)


## Pattern roll

### Knife tool

In my workflow, I'm often adding new tracks with a knife tool: even though there's a normal way to add an empty track via project menu, or duplicate a track, I often end up having added some sketches in different places of a single sequence, and then, after switching to the pattern mode I see that they represent different parts, and can be cut into different tracks:

![patterns-knife-tool]

### Clips and track grouping

Note that in the example above, two split tracks remain on the same row, just because they are grouped by name, and the knife tool keeps the track name the same. Pattern roll can group tracks by color, or by instrument, or by track id — yet grouping by name works better for me.

So, what you see in one row might be either different tracks, or there might be also several instances of the same track. Instances (or clips, as I call them in a code) always share the same name and same notes, but they can be slightly modified: have different position, key shift or volume multiplier, which is mainly meant for prototyping:

![patterns-clips]

#### See also: [pattern roll hotkeys](hotkeys.md#pattern-roll)


## Command palette

This command palette control is inspired by 'Goto Anywhere' in Sublime Text or 'Locator' in QT Creator, and is meant for quick access to the commands available for the current context, and things like projects and timeline events.

Toggled by `~` or `Control + P` hotkeys by default:

![command-palette]

Besides just commands, it supports several modes, which are triggered by typing a certain symbol. This symbol also acts as a hotkey to show the command palette in that mode:

* `/` is for the project list,
* `@` is for the timeline events list + tracks list,
* `!` is for the chord compiler, which deserves a separate description:

### Chord compiler

One of the modes of that command palette allows to generate chords based on the the chord name [decoding rules](https://en.wikipedia.org/wiki/Chord_letters). This tool is not aware of any of the key signatures present at the timeline, all the chord's notes are inferred from the given description.

Just hit `!` and start typing a chord description, or pick some of the suggestions it provides:

![chord-compiler]


[space-drag]: images/space-drag.png "Dragging the canvas"
[time-measure-tool]: images/time-measure.png "Time measure tool"
[sound-probe]: images/sound-probe.png "Sound probe tool"
[group-resizing]: images/group-resizing.png "Resizing notes with shift"
[pen-tool-alt]: images/pen-tool-alt.png "Adding notes sith shift"
[drag-and-copy]: images/drag-and-copy.png "Drag-and-copy"

[monitor-and-map]: images/monitor-and-map.png "Audio monitor and minimap view modes"
[scales-highlighting]: images/scales-highlighting.png "Scales highlighting"
[note-names]: images/note-names.png "Note name guides"

[chord-tool]: images/chord-tool.png "The chord tool"
[chord-compiler]: images/chord-compiler.png "The chord compiler"
[quick-rescale]: images/quick-rescale.png "The quick rescale tool"
[command-palette]: images/command-palette.png "The command palette"

[warnings]: images/warnings.png "Clipping and oversaturation warning markers"
[velocity-map-fine-tuning]: images/velocity-map-fine-tuning.png "Velocity map fine-tuning"

[patterns-knife-tool]: images/patterns-knife-tool.png "Using knife tool in pattern mode"
[patterns-clips]: images/patterns-track-clips.png "Track instances (clips) and their modifications"
