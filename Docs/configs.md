# Configs

Helio uses several configuration files, such as: scales, in-scale chords, temperaments, hotkeys, colour schemes and translations.

For each of those configs, there can be up to three versions, which are loaded and merged in this order: 
 * ones that a built-in into executable,
 * latest updates fetched from helio.fm - they extend and override the built-in ones,
   * for example, translations, which is now the only resource updated in the runtime,
 * user's configs - they extend and override the previous step.
   * for example, arpeggiators that you create in the app,
   * or any other configuration you might [add](#user-configs) and fill by hand.

## Built-in configs

### Chords

This config lists all chords displayed in the [chord tool](tips-and-tricks.md#chord-tool):

```json
{{#include ../Resources/chords.json}}
```

### Temperaments

This file lists all available temperaments, to which you can [switch](getting-microtonal.md#switching-temperaments) your project:

```json
{{#include ../Resources/temperaments.json}}
```

Temperaments define how highlighting works in the piano roll, including octave size and key names, and which scales are available to choose from. Temperament description also provides a chromatic approximation, which is used as the "least common denominator" to be able to convert a piece from one temperament to another.

Temperaments allow to specify up to 3 enharmonic equivalents per key, and custom chromatic scale note namings for each enharmonic equivalent. This is to make note names more consistent with traditional notation language, although not ideal, because it only uses names from chromatic scales; in the context of diatonic scales, note names will still differ from traditional notation because the sequencer displays both the chromatic scale (the grid itself) and whatever scales within it (the highlighted rows) at the same time.

### Scales

Scales listed here are available to choose from in the key signature dialog, and in the rescale [tool](tips-and-tricks.md#quick-rescale-tool). Scales with octave size mismatching the current temperament's octave size are ignored.

```json
{{#include ../Resources/scales.json}}
```

### Meters

Meters listed here are available to choose from in the time signature dialog.

Each meter comes with a [metronome](getting-polymetric.md#metronome), which is described using ["Oona Pana"](https://medium.com/@theBobavich/introducing-a-new-way-to-count-music-59b69158001f) scheme, suggested by Rods Bobavich - a simplified, but handy way to describe musical rhythms, which is also easy to read and edit manually.

Metronome schemes listed here are just the default ones, and they can be edited for each time signature separately in the time signature dialog.

```json
{{#include ../Resources/meters.json}}
```

### Hotkeys

See the [hotkeys page](hotkeys.md).

### Translations

The translations file is too big to be included here; if you want to help proofread and improve the translation for your native language, please follow this [link](https://helio.fm/translations).

The translations are updated in the runtime, if there are any changes: the latest translations are saved in the `translations.helio` file in the app's [config directory](index.md#the-configuration-directory).


## User configs

To override or extend the built-in data, you can create a file with one of these names in your [projects directory](index.md#the-projects-directory):

 * `chords.json`
 * `scales.json`
 * `meters.json`
 * `temperaments.json`
 * etc.

### Custom scales

Let's say, you want to add a scale - here's the example content for `scales.json`:

```json
{
  "scales": {
    "scale": [
      { "period": 12, "name": "Oriental", "intervals": "1 3 1 1 3 1 2" }
    ]
  }
}
```

What it means:
 * `"period": 12` - this scale will be available only in twelve-tone projects,
 * `"name": "Oriental"` - scales are merged by name; if the built-in list contained a scale with such name, it would be replaced,
 * `"intervals": "1 3 1 1 3 1 2"` - this one is a heptatonic scale, i.e. it contains 7 intervals.

### Custom temperaments

Or, if you want to add a temperament, create `temperaments.json`; for example, let's add 24-EDO:

```json
{
  "temperaments": {
    "temperament": [
      {
        "id": "24edo",
        "name": "24 equal temperament",
        "period": "C ^C C# vD D ^D Eb vE E ^E F ^F F# vG G ^G Ab vA A ^A Bb vB B ^B",
		"periodRange": 2.0,
        "highlighting": "3 4 3 4 3 4 3",
        "chromaticMap": "2 2 2 2 2 2 2 2 2 2 2 2"
      }
    ]
  }
}
```

What it means:
 * `id` - if the built-in list contained a temperament with such id, it would be replaced,
 * `name` - this is displayed in the menus,
 * `period` - key names separated by whitespace; the number of keys defines the octave size,
 * `periodRange` - the pitch range that makes up a period, defaults 2.0 representing one octave; for nonoctave tunings use 3.0 for the duodecime used in the Bohlen-Pierce tuning or 1.5 for the pure fifth, used in the Carlos Alpha, Beta and Gamma tunings; for backward compatibility, if this parameter isn't given, Helio assumes one octave; this parameter only affects the built-in instrument at the moment,
 * `highlighting` - the default highlighting scheme in the piano roll, i.e. which rows are highlighted as "white keys" by default,
 * `chromaticMap` - this is used to [convert](getting-microtonal.md#switching-temperaments) temperaments; here, since 24-EDO just divides the 12-tone semitone in two, the chromatic map just jumps over quarter-tones.

You might want to add some scales for it, for example:

```json
{
  "scales": {
    "scale": [
      // append this list to your scales:
      { "period": 24, "name": "Maqam Nahfat", "intervals": "3 3 4 4 4 2 4" },
      { "period": 24, "name": "Maqam Saba", "intervals": "3 3 2 6 2 4 4" },
      { "period": 24, "name": "Maqam Sabr Jadid", "intervals": "3 3 2 6 2 6 2" }
      // etc, etc.
      // (source: https://en.xen.wiki/w/24edo)
    ]
  }
}
```

### Custom colour schemes

To add your theme to the built-in themes, put a `colourSchemes.json` file in your [projects directory](index.md#the-projects-directory). Here's an example:

```json
{
  "colourSchemes": {
    "colourScheme": [
      {
        "name": "Tonsky Mode",
        "colourMap":
        {
          "text": "ff000000",
          "headlineFill": "fffddb29",
          "pageFill": "fffddb29",
          "sidebarFill": "fffddb29",
          "dialogFill": "fffddb29",
          "lassoFill": "aaf9b579",
          "lassoBorder": "55000000",
          "buttonFill": "18000000",
          "frameBorder": "58000000",
          "whiteKey": "fffddb29",
          "blackKey": "fffdce29",
          "bar": "ff978717",
          "row": "fffbc851",
          "timeline": "fffddb29",
          "iconBase": "5f000000",
          "iconShadow": "27fde258"
        }
      }
    ]
  }
}
```
