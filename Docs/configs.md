# Configs

Helio uses several configuration files, such as: scales, in-scale chords, temperaments, hotkeys, colour schemes and translations.

Each configuration type can have up to three versions, which are loaded and merged in this order:
 * built-in into the executable,
 * latest updates fetched from helio.fm, which extend and override the built-in ones,
   * for example, translations, which is now the only resource updated in the runtime,
 * [user's configs](#user-configs), which extend and override the previous step
   * for example, arpeggiators that you create in the app,
   * or any other configuration you might add and fill by hand.

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

The minimal content for `scales.json` is as follows:

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
 * `"intervals": "1 3 1 1 3 1 2"` - successive intervals, in dieses (this one is a heptatonic scale, i.e. it contains 7 intervals).

Here's the example config with additional heptatonic scales for [19-edo](https://xen.wiki/w/19edo_modes) and [31-edo](https://xen.wiki/w/31edo_modes):

```json
{
  "scales": {
    "scale": [
      { "name": "Deutone 0|6", "period": 19, "intervals": "1 3 3 3 3 3 3" },
      { "name": "Deutone 1|5", "period": 19, "intervals": "3 1 3 3 3 3 3" },
      { "name": "Deutone 2|4", "period": 19, "intervals": "3 3 1 3 3 3 3" },
      { "name": "Deutone 3|3", "period": 19, "intervals": "3 3 3 1 3 3 3" },
      { "name": "Deutone 4|2", "period": 19, "intervals": "3 3 3 3 1 3 3" },
      { "name": "Deutone 5|1", "period": 19, "intervals": "3 3 3 3 3 1 3" },
      { "name": "Deutone 6|0", "period": 19, "intervals": "3 3 3 3 3 3 1" },
      { "name": "Kleismic 0|6", "period": 19, "intervals": "1 4 1 4 1 4 4" },
      { "name": "Kleismic 1|5", "period": 19, "intervals": "1 4 1 4 4 1 4" },
      { "name": "Kleismic 2|4", "period": 19, "intervals": "1 4 4 1 4 1 4" },
      { "name": "Kleismic 3|3", "period": 19, "intervals": "4 1 4 1 4 1 4" },
      { "name": "Kleismic 4|2", "period": 19, "intervals": "4 1 4 1 4 4 1" },
      { "name": "Kleismic 5|1", "period": 19, "intervals": "4 1 4 4 1 4 1" },
      { "name": "Kleismic 6|0", "period": 19, "intervals": "4 4 1 4 1 4 1" },
      { "name": "Liese 0|6", "period": 19, "intervals": "1 1 1 7 1 1 7" },
      { "name": "Liese 1|5", "period": 19, "intervals": "1 1 7 1 1 1 7" },
      { "name": "Liese 2|4", "period": 19, "intervals": "1 1 7 1 1 7 1" },
      { "name": "Liese 3|3", "period": 19, "intervals": "1 7 1 1 1 7 1" },
      { "name": "Liese 4|2", "period": 19, "intervals": "1 7 1 1 7 1 1" },
      { "name": "Liese 5|1", "period": 19, "intervals": "7 1 1 1 7 1 1" },
      { "name": "Liese 6|0", "period": 19, "intervals": "7 1 1 7 1 1 1" },
      { "name": "Magic 0|6", "period": 19, "intervals": "1 1 5 1 5 1 5" },
      { "name": "Magic 1|5", "period": 19, "intervals": "1 5 1 1 5 1 5" },
      { "name": "Magic 2|4", "period": 19, "intervals": "1 5 1 5 1 1 5" },
      { "name": "Magic 3|3", "period": 19, "intervals": "1 5 1 5 1 5 1" },
      { "name": "Magic 4|2", "period": 19, "intervals": "5 1 1 5 1 5 1" },
      { "name": "Magic 5|1", "period": 19, "intervals": "5 1 5 1 1 5 1" },
      { "name": "Magic 6|0", "period": 19, "intervals": "5 1 5 1 5 1 1" },
      { "name": "Altered Dorian", "period": 31, "intervals": "5 3 5 5 5 4 4" },
      { "name": "Altered Neapolitan Major", "period": 31, "intervals": "3 5 5 5 5 4 4" },
      { "name": "AugmentedPlus", "period": 31, "intervals": "8 2 8 2 8 2 1" },
      { "name": "Enharmonic Dorian", "period": 31, "intervals": "1 2 10 5 1 2 10" },
      { "name": "Enharmonic Hypodorian", "period": 31, "intervals": "5 1 2 10 1 2 10" },
      { "name": "Enharmonic Hypolydian", "period": 31, "intervals": "2 10 5 1 2 10 1" },
      { "name": "Enharmonic Hypophrygian", "period": 31, "intervals": "10 5 1 2 10 1 2" },
      { "name": "Enharmonic Lydian", "period": 31, "intervals": "2 10 1 2 10 5 1" },
      { "name": "Enharmonic Mixolydian", "period": 31, "intervals": "1 2 10 1 2 10 5" },
      { "name": "Enharmonic Phrygian", "period": 31, "intervals": "10 1 2 10 5 1 2" },
      { "name": "Harmonic Series 7", "period": 31, "intervals": "6 5 5 4 4 4 3" },
      { "name": "Harrison Major", "period": 31, "intervals": "5 6 2 5 5 6 2" },
      { "name": "Hyperblue Dorian", "period": 31, "intervals": "8 5 2 3 6 2 5" },
      { "name": "Hyperblue Harmonic", "period": 31, "intervals": "8 5 2 3 2 9 2" },
      { "name": "Hypermavila", "period": 31, "intervals": "8 3 3 8 3 3 3" },
      { "name": "Maqam Bayati", "period": 31, "intervals": "4 4 5 5 3 5 5" },
      { "name": "Maqam Rast", "period": 31, "intervals": "5 4 4 5 5 4 4" },
      { "name": "Neutral[7]", "period": 31, "intervals": "4 5 4 5 4 5 4" },
      { "name": "Phrygian Harmonic", "period": 31, "intervals": "2 8 3 5 2 5 6" },
      { "name": "Scorp", "period": 31, "intervals": "5 4 5 3 5 4 5" },
      { "name": "Screamapillar", "period": 31, "intervals": "5 5 4 4 5 5 3" },
      { "name": "Sheimanic", "period": 31, "intervals": "4 4 4 4 5 5 5" },
      { "name": "Subminor Altered", "period": 31, "intervals": "2 5 3 5 5 5 6" },
      { "name": "Thaiic", "period": 31, "intervals": "4 5 5 4 4 4 5" },
      { "name": "Tropical Major", "period": 31, "intervals": "5 7 1 5 7 5 1" },
      { "name": "Turkish Major", "period": 31, "intervals": "5 5 3 5 4 5 4" }
    ]
  }
}
```

### Custom temperaments

To add a temperament, create `temperaments.json`; for example, let's add 24-EDO:

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
