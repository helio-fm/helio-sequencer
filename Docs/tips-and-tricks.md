# Tips and tricks

This page lists features, hacks and nuances, which might be handy, or non-obvious, or both.

## Piano roll

### Warnings

While playback, Helio may sometimes draw red or yellow vertical warning lines:

![warnings]

These are the clipping and over-saturation warning markers:

 - red lines indicate problematic regions with [clipping sound](https://en.wikipedia.org/wiki/Clipping_(audio)),

 - yellow warning lines will appear in the areas where the perceived loudness (a.k.a. the root mean square loudness) is way lower than the peak loudness, which is considered ~~harmful~~ unhealthy: in my setup it typically means that I have some redundant duplicate notes at the same place.

### Spacebar panning

One quick way to switch between the current editing mode and the canvas panning mode is holding the `Space` key:

![space-drag]

Dragging with the right mouse button does a similar thing, but it will also switch to another track, if clicked on any semi-transparent note.

### Time measure tool

Hold the `Space` key, then click-and-drag over the timeline to measure time between two points at the timeline:

![time-measure-tool]

### Sound probe tool

Finally, holding `Space` and clicking at the timeline is something I call "sound probe", which is supposed to give an idea of what notes are playing at the given point:

![sound-probe]

### Range selection

Click-and-drag at the timeline while holding any modifier key (`Control`/`Alt`/`Shift`) to select all notes or clips in some time range:

![range-select]

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

Also hold `Shift` or any modifier key to change the behavior of the pen tool when adding notes. By default, the newly added note is edited in the transpose-and-resize mode. Alternatively, it's the drag mode, more familiar:

![pen-tool-alt]

### UI flags

A couple of display options are available to provide a visual cue. They are toggled either in the navigation panel, or via hotkeys (`G` and `H` by default).

The first one is for displaying the note name guides:

![note-names]

Another one highlights the in-scale keys of the key signatures that are added to the timeline. If you prefer C Major coloring in the piano roll, just turn it off:

![scales-highlighting]

### More UI flags

The audio monitor view can be toggled between two modes on click or tap, switching between the simple spectrogram mode and the waveform mode:

![monitors]

The mini-map mode can be toggled with `B` hotkey, or by clicking at any area except the screen range rectangle.

When in compact mode, it is stretched to fit all project:

![toggle-minimap]

Tip: when in full mode, the mini-map allows to draw a region to zoom at:

![zoom-to-region]

### Chord tool

By double-clicking on a row in the piano roll, you invoke the chord tool:

![chord-tool]

It picks the current key signature from the timeline to determine what scale and root key to use to generate chords. Hence the main limitation of this tool: it can only generate chords that are easy to define with in-scale keys.

It can be dragged around by the center node — kinda helpful if you clicked the wrong row or position.

Since it depends on the harmonic context, it will do nothing when placed on the out-of-scale note (the grey row). It might be a good idea to make sure the [scales highlighting](#ui-flags) is enabled to avoid confusion.

#### See also: [piano roll hotkeys](hotkeys.md#piano-roll), [refactoring options](refactoring.md)


## Pattern roll

### Knife tool

In my workflow, I'm often adding new tracks with a knife tool: even though there's a normal way to add an empty track via project menu, or duplicate a track, I often end up having added some sketches in different places of a single sequence, and then, after switching to the pattern mode I see that they represent different parts, and can be cut into different tracks:

![patterns-knife-tool]

#### Merging tracks

Knife tool has an alternative mode: use right-click-&-drag (or long-tap-&-drag on mobile platforms) to merge one clip with another:

![patterns-merge-tool]

### Clips and track grouping

In the example above, two split tracks remain on the same row because the tracks are grouped by name, and the knife tool keeps the track name the same. Pattern roll can also group tracks by color, or by instrument, or by track id — yet grouping by name works better for me.

So, the segments on one row might be either different tracks, or they also might be several instances (or "clips") of the same track. Instances always share the same notes, and have the same name and color, but they can be slightly modified: have different position, key shift or volume multiplier, which is mainly meant for prototyping:

![patterns-clips]

Note that when you select an item in the pattern roll, all its instances are highlighted with a dashed header: this helps to tell where are the "instances", and where are other tracks of the same name or color on the same row. A quick way to convert an instance to a unique track is `F6` hotkey.

Track grouping also affects MIDI export: all segments on one row will be exported as one track in the resulting MIDI file.

#### See also: [pattern roll hotkeys](hotkeys.md#pattern-roll)


[space-drag]: images/space-drag.png "Dragging the canvas"
[time-measure-tool]: images/time-measure.png "Time measure tool"
[sound-probe]: images/sound-probe.png "Sound probe tool"
[range-select]: images/range-select.png "Range selection"

[group-resizing]: images/group-resizing.png "Resizing notes with shift"
[pen-tool-alt]: images/pen-tool-alt.png "Adding notes with shift"
[drag-and-copy]: images/drag-and-copy.png "Drag-and-copy"

[monitors]: images/monitors.png "Audio monitor view modes"
[toggle-minimap]: images/toggle-minimap.png "Mini-map view modes"
[zoom-to-region]: images/zoom-to-region.png "Mini-map zoom-to-region"
[scales-highlighting]: images/scales-highlighting.png "Scales highlighting"
[note-names]: images/note-names.png "Note name guides"

[chord-tool]: images/chord-tool.png "The chord tool"

[warnings]: images/warnings.png "Clipping and oversaturation warning markers"
[velocity-map-fine-tuning]: images/velocity-map-fine-tuning.png "Velocity map fine-tuning"

[patterns-knife-tool]: images/patterns-knife-tool.png "Using knife tool in pattern mode"
[patterns-merge-tool]: images/patterns-merge-tool.png "Merging tracks in pattern mode"
[patterns-clips]: images/patterns-track-clips.png "Track instances (clips) and their modifications"
