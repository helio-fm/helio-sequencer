# Configs

Helio has several built-in configuration files for [scales](#built-in-scales), [temperaments](#built-in-temperaments), in-scale [chords](#built-in-chords), [meters](#built-in-meters), [hotkeys](hotkeys.md), and colour schemes.

You can extend them by creating any of these files in your [projects directory](index.md#the-projects-directory):
 * `meters.json`
 * `chords.json`
 * [`scales.json`](#custom-scales)
 * [`temperaments.json`](#custom-temperaments)
 * [`colourSchemes.json`](#custom-colour-schemes)

This page lists built-in defaults and examples of how to extend them:

### Built-in chords

This config lists all chords displayed in the [chord tool](tips-and-tricks.md#chord-tool):

```json
{{#include ../Resources/chords.json}}
```

### Built-in temperaments

Temperaments define how highlighting works in the piano roll, including octave size and key names, and which scales are available to choose from. Temperament description also provides a chromatic approximation, which is used when [converting a piece](getting-microtonal.md#switching-temperaments) from one temperament to another:

```json
{{#include ../Resources/temperaments.json}}
```

Temperaments allow to specify up to 3 enharmonic equivalents per key, and custom chromatic scale note namings for each enharmonic equivalent. This is to make note names more consistent with traditional notation language, although not ideal, because it only uses names from chromatic scales; in the context of diatonic scales, note names will still differ from traditional notation because the sequencer displays both the chromatic scale (the grid itself) and whatever scales within it (the highlighted rows) at the same time.

### Built-in scales

Scales listed here are available to choose from in the key signature dialog, and in the rescale [tool](tips-and-tricks.md#quick-rescale-tool). Scales with octave size mismatching the current temperament's octave size are ignored.

```json
{{#include ../Resources/scales.json}}
```

### Built-in meters

Meters listed here are available to choose from in the time signature dialog.

Each meter comes with a [metronome](getting-polymetric.md#metronome), which is described using ["Oona Pana"](https://medium.com/@theBobavich/introducing-a-new-way-to-count-music-59b69158001f) scheme, suggested by Rods Bobavich - a simplified, but handy way to describe musical rhythms, which is also easy to read and edit manually.

Metronome schemes listed here are just the default ones, and they can be edited for each time signature separately in the time signature dialog.

```json
{{#include ../Resources/meters.json}}
```

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

Here's the example config with additional [strictly proper scales for 31-edo](https://en.xen.wiki/w/Strictly_proper_7-tone_31edo_scales):

```json
{
  "scales": {
    "scale": [
      { "name": "Breed, Hemisub", "period": 31, "intervals": "3 4 5 6 2 6 5" },
      { "name": "Breed, Inverse Hemisub", "period": 31, "intervals": "4 3 5 6 2 6 5" },
      { "name": "Breed, Heminewt", "period": 31, "intervals": "6 3 5 4 5 3 5" },
      { "name": "Breed, Inverse Heminewt", "period": 31, "intervals": "4 5 3 6 5 3 5" },

      { "name": "Orwell, Orminor", "period": 31, "intervals": "5 3 6 4 4 6 3" },
      { "name": "Orwell, Ormed", "period": 31, "intervals": "4 6 4 4 6 4 3" },
      { "name": "Orwell, Augton", "period": 31, "intervals": "4 6 4 4 3 6 4" },
      { "name": "Orwell, Inverse Augton", "period": 31, "intervals": "4 6 4 4 6 3 4" },

      { "name": "Mothra, Lydic", "period": 31, "intervals": "5 5 5 3 6 4 3" },
      { "name": "Mothra, Inverse Lydic", "period": 31, "intervals": "5 5 5 3 4 6 3" },
      { "name": "Mothra, Ionic", "period": 31, "intervals": "5 5 3 5 4 6 3" },
      { "name": "Mothra, Inverse Ionic", "period": 31, "intervals": "5 3 6 4 5 3 5" },
      { "name": "Mothra, Quahog", "period": 31, "intervals": "4 6 4 4 4 6 3" },
      { "name": "Mothra, Inverse Quahog", "period": 31, "intervals": "6 4 4 4 6 4 3" },
      { "name": "Mothra, Higasi", "period": 31, "intervals": "6 4 5 3 6 4 3" },
      { "name": "Mothra, Inverse Higasi", "period": 31, "intervals": "4 6 3 5 4 6 3" },

      { "name": "Hemiwuerschmidt, Hemithirds", "period": 31, "intervals": "5 5 5 5 5 1 5" },
      { "name": "Hemiwuerschmidt, Hemaj", "period": 31, "intervals": "5 5 4 4 6 5 2" },
      { "name": "Hemiwuerschmidt, Hemin", "period": 31, "intervals": "4 4 5 5 2 5 6" },
      { "name": "Hemiwuerschmidt, Gypsi", "period": 31, "intervals": "4 5 2 5 5 4 6" },
      { "name": "Hemiwuerschmidt, Inverse Gypsi", "period": 31, "intervals": "4 5 5 2 5 4 6" },
      { "name": "Hemiwuerschmidt, Leadhole", "period": 31, "intervals": "5 5 5 5 5 4 2" },
      { "name": "Hemiwuerschmidt, Inverse Leadhole", "period": 31, "intervals": "5 5 5 5 5 2 4" },

      { "name": "Mohajira, Sikah", "period": 31, "intervals": "4 5 5 4 4 5 4" },
      { "name": "Mohajira, Sheimanic", "period": 31, "intervals": "4 4 4 4 5 5 5" },
      { "name": "Mohajira, Thaiic", "period": 31, "intervals": "4 5 5 4 4 4 5" },
      { "name": "Mohajira, Inverse Thaiic", "period": 31, "intervals": "4 5 4 4 4 5 5" },
      { "name": "Mohajira, Neutral Dorian", "period": 31, "intervals": "4 5 4 5 4 5 4" },

      { "name": "Miracle, Subdom", "period": 31, "intervals": "5 4 3 6 3 4 6" },
      { "name": "Miracle, Superton", "period": 31, "intervals": "5 6 4 3 6 3 4" },
      { "name": "Miracle, Nudia", "period": 31, "intervals": "5 4 3 6 3 6 4" },
      { "name": "Miracle, Inverse Nudia", "period": 31, "intervals": "5 4 6 3 6 3 4" },
      { "name": "Miracle, Nurow", "period": 31, "intervals": "3 6 3 6 3 4 6" },
      { "name": "Miracle, Inverse Nurow", "period": 31, "intervals": "6 3 6 3 6 4 3" },

      { "name": "Valentine, Silver", "period": 31, "intervals": "6 4 4 6 2 7 2" },
      { "name": "Valentine, Harmonia", "period": 31, "intervals": "6 4 4 4 4 7 2" },
      { "name": "Valentine, Inverse Harmonia", "period": 31, "intervals": "4 4 4 6 2 7 4" },
      { "name": "Valentine, Pendragon", "period": 31, "intervals": "6 4 4 4 5 6 2" },
      { "name": "Valentine, Inverse Pendragon", "period": 31, "intervals": "4 6 2 6 5 4 4" },
      { "name": "Valentine, Hipop", "period": 31, "intervals": "6 2 6 4 4 6 3" },
      { "name": "Valentine, Inverse Hipop", "period": 31, "intervals": "4 6 2 6 3 6 4" },
      { "name": "Valentine, Malakon", "period": 31, "intervals": "4 6 2 6 4 4 5" },
      { "name": "Valentine, Inverse Malakon", "period": 31, "intervals": "4 6 2 6 4 5 4" },
      { "name": "Valentine, Bagpipe", "period": 31, "intervals": "4 4 6 4 5 2 6" },
      { "name": "Valentine, Inverse Bagpipe", "period": 31, "intervals": "4 6 4 4 6 2 5" },
      { "name": "Valentine, Pitu", "period": 31, "intervals": "6 4 4 4 6 2 5" },
      { "name": "Valentine, Inverse Pitu", "period": 31, "intervals": "6 4 4 4 6 5 2" },
      { "name": "Valentine, Porch", "period": 31, "intervals": "6 4 4 4 4 4 5" },
      { "name": "Valentine, Inverse Porch", "period": 31, "intervals": "4 4 4 6 5 4 4" },

      { "name": "Meantone, Diatonic", "period": 31, "intervals": "5 5 3 5 5 5 3" },
      { "name": "Meantone, Melodic Minor", "period": 31, "intervals": "5 3 5 5 5 5 3" },
      { "name": "Meantone, Harmonic Minor", "period": 31, "intervals": "5 3 5 5 3 7 3" },
      { "name": "Meantone, Harmonic Major", "period": 31, "intervals": "5 5 3 5 3 7 3" },
      { "name": "Meantone, Major Locrian", "period": 31, "intervals": "5 5 3 3 5 5 5" },
      { "name": "Meantone, Diminished Diatonic", "period": 31, "intervals": "3 7 1 7 3 5 5" },
      { "name": "Meantone, Enharmonic Major", "period": 31, "intervals": "5 5 3 5 6 4 3" },
      { "name": "Meantone, Enharmonic Minor", "period": 31, "intervals": "5 3 4 6 5 3 5" },
      { "name": "Meantone, Enharmonic Mixolydian", "period": 31, "intervals": "5 5 3 5 6 2 5" },
      { "name": "Meantone, Inverse Enharmonic Mixolydian", "period": 31, "intervals": "5 5 2 6 5 3 5" },
      { "name": "Meantone, Enharmonic Major-Minor", "period": 31, "intervals": "5 5 2 6 3 5 5" },
      { "name": "Meantone, Inverse Enharmonic Major-Minor", "period": 31, "intervals": "5 5 5 3 6 2 5" },
      { "name": "Meantone, Major Doubleflat", "period": 31, "intervals": "6 4 3 5 6 2 5" },
      { "name": "Meantone, Minor Doubleflat", "period": 31, "intervals": "5 3 4 6 5 2 6" },
      { "name": "Meantone, Ping", "period": 31, "intervals": "5 5 5 3 6 5 2" },
      { "name": "Meantone, Inverse Ping", "period": 31, "intervals": "3 5 5 5 2 5 6" },
      { "name": "Meantone, Alhijaz", "period": 31, "intervals": "3 7 2 6 3 5 5" },
      { "name": "Meantone, Ambika", "period": 31, "intervals": "6 2 7 3 5 5 3" },
      { "name": "Meantone, Kung", "period": 31, "intervals": "6 4 5 3 6 5 2" },
      { "name": "Meantone, Ousak", "period": 31, "intervals": "3 5 4 6 2 5 6" }
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
        "name": "Grayscale Light",
        "colourMap":
        {
          "text": "fff0f0f0",
          "headlineFill": "ff2d2d30",
          "pageFill": "ff2d2d30",
          "sidebarFill": "ff2d2d30",
          "dialogFill": "ff333333",
          "lassoFill": "25000000",
          "lassoBorder": "ffd4d4d4",
          "buttonFill": "18ffffff",
          "frameBorder": "ff585858",
          "whiteKey": "ff262626",
          "blackKey": "ff1e1e1e",
          "bar": "271b1923",
          "row": "ff1b1923",
          "timeline": "ff2d2d30",
          "iconBase": "27ffffff",
          "iconShadow": "ff000000"
        }
      }
    ]
  }
}
```
