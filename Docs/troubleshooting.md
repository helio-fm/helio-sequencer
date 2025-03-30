# Troubleshooting

This page will collect known issues and workarounds. If you find any more, please [share](https://github.com/helio-fm/helio-sequencer/issues/new).

## Fonts

To fix font issues, particularly for Chinese/Japanese/Korean translations, install the [Noto CJK font](https://github.com/notofonts/noto-cjk) and select it on the settings page.

The app will try to use Noto CJK by default if it's already installed. If not, the app will also look for the fonts YeHei, Hei, and Heiti SC for Chinese or Japanese translations, and Dotum for Korean translation.

## Plugin scanning

On desktop platforms, plugins are scanned in a separate process to avoid crashes (even while scanning, some plugins may crash for a variety of reasons).

On Linux, for plugin scanning to work, the app needs to be launched with an absolute path so that it can see it's own executable file. This already already should be the case in most distributions.

## LV2 plugins

You need to place LV2 plugins in the proper directories for your OS. E.g. for Windows it is `%APPDATA%\LV2\` or `%COMMONPROGRAMFILES%\LV2\`. On Windows 10 it is `%APPDATA%\Roaming\LV2\`. On Linux, it is `/usr/lib/lv2/`, `/usr/local/lib/lv2/` or a directory mentioned in your LV2_PATH environment variable.

#### See also: the Ardour [documentation for plugin directories](https://manual.ardour.org/appendix/files-and-directories/#Plugins).

## iOS issues

The iOS file picker will allow you to select either the app's local folder or the iCloud folder, if available. Keep all your projects, as well as SoundFonts and metronome samples, in the app's local folder.

The iCloud folder should only be used for one-time actions like importing/exporting MIDI or rendering WAV/FLAC audio. Due to the iOS security model, the folder will not be accessible across sessions.

## Sidebar cut-off

Some mobile devices have a display notch or a frontal camera cutout, partially covering either of the sidebars, depending on which orientation you're using. To workaround this, sidebars allow dragging them sideways a bit:

![stretch-sidebars]

Sidebar positions are remembered between sessions.

## Building from source

* If you experience the error: `'exchange' is not a member of 'std'`, add `#include <utility>` to `JUCE/modules/juce_core/system/juce_StandardHeader.h` in the appropriate place. This is an issue pending resolution with the JUCE library, and is probably not a problem with your compiler.
* Be sure to clone the repository with recursive mode enabled so that the ThirdParty folder is pre-populated with the necessary files. Alternatively, add them yourself by going to their respective repositories.
* If you experience build errors, make sure you have all required dependencies installed in your system.


[stretch-sidebars]: images/stretch-sidebars.png "Sidebars can be stretched sideways to avoid being cut-off by the display notch"
