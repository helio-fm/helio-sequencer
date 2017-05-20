## ![Vive la culture libre](Resources/Screenshot.png)
[Helio Workstation](https://helioworkstation.com) is free and open-source music sequencer, designed to be used on all major platforms.

### Why another sequencer?

Most of the DAW interfaces often seem overcomplicated, and they only tend to get more and more bloated over time. Also most of the DAWs are commercial, proprietary-licensed and almost none of them support all major operating systems at the same time.

Helio aims to be a modern music creation tool, featuring linear-based sequencer with a clean interface, high-performance C++ core, integrated version control providing intelligent synchronization between devices, saved undo history, translations to many languages and more.

### Building

||Linux|Windows|macOS|iOS|
|---|---|---|---|---|
|master|[![Linux Build Status](https://travis-ci.org/peterrudenko/helio-workstation.svg?branch=master)](https://travis-ci.org/peterrudenko/helio-workstation)|[![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/peterrudenko/helio-workstation?svg=true&branch=master)](https://ci.appveyor.com/project/peterrudenko/helio-workstation)|[![macOS Build Status](https://app.nevercode.io/api/projects/8fe1daf4-2cde-4f36-ad57-430f7a2816e6/workflows/5a4dd672-ef90-4a66-8943-5c7682a6545d/status_badge.svg?branch=master&style=shields)](https://app.nevercode.io/#/project/8fe1daf4-2cde-4f36-ad57-430f7a2816e6/workflow/5a4dd672-ef90-4a66-8943-5c7682a6545d/latestBuild?branch=master)|[![iOS Build Status](https://app.nevercode.io/api/projects/2540e222-b017-4cfd-a4af-80399e319629/workflows/6e9ff9ae-ede3-4f01-bced-8ff4e0761fea/status_badge.svg?branch=master&style=shields)](https://app.nevercode.io/#/project/2540e222-b017-4cfd-a4af-80399e319629/workflow/6e9ff9ae-ede3-4f01-bced-8ff4e0761fea/latestBuild?branch=master)|
|develop|[![Linux Build Status](https://travis-ci.org/peterrudenko/helio-workstation.svg?branch=develop)](https://travis-ci.org/peterrudenko/helio-workstation)|[![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/peterrudenko/helio-workstation?svg=true&branch=develop)](https://ci.appveyor.com/project/peterrudenko/helio-workstation)|[![macOS Build Status](https://app.nevercode.io/api/projects/8fe1daf4-2cde-4f36-ad57-430f7a2816e6/workflows/5a4dd672-ef90-4a66-8943-5c7682a6545d/status_badge.svg?branch=develop&style=shields)](https://app.nevercode.io/#/project/8fe1daf4-2cde-4f36-ad57-430f7a2816e6/workflow/5a4dd672-ef90-4a66-8943-5c7682a6545d/latestBuild?branch=develop)|[![ios Build Status](https://app.nevercode.io/api/projects/2540e222-b017-4cfd-a4af-80399e319629/workflows/6e9ff9ae-ede3-4f01-bced-8ff4e0761fea/status_badge.svg?branch=develop&style=shields)](https://app.nevercode.io/#/project/2540e222-b017-4cfd-a4af-80399e319629/workflow/6e9ff9ae-ede3-4f01-bced-8ff4e0761fea/latestBuild?branch=develop)|

#### Basic build instructions

 * Git clone,
 * Get submodules, if not yet done - `git submodule update --init --recursive`,
 * Get VST and ASIO SDKs (which can't be redistributed in this project due to licensing restrictions, but you may use `ThirdParty/get_asio_and_vst_sdks.sh` script to download and extract the SDKs sources; this is invoked automatically in macOS and iOS projects pre-build phase),
 * On Linux, you'll need to have the following packages installed: `libfreetype6-dev libx11-dev libxinerama-dev libxrandr-dev libxcursor-dev libxcomposite-dev mesa-common-dev libasound2-dev freeglut3-dev libcurl4-openssl-dev libasound2-dev libjack-dev`,
 * Pick the right project for you OS from `Projects` directory and build.

### Links

 * Homepage with all latest builds: [https://helioworkstation.com](https://helioworkstation.com/).
 * Project page at [KVR database](http://www.kvraudio.com/product/helio-workstation-by-peter-rudenko).
 * Some [screencasts on Youtube](https://www.youtube.com/channel/UCO3K8iCd1k2FTqSocoE-WXw/).

### Contributing

Helio is a work in progress and still there are many essential features missing, so pull requests are appreciated.

Your ideas are also welcome: friendly and lightweight UI is the main development priority in this project, and if you have a vision on how to improve user experience in music composing, feel free to share it.

You could also [make a new translation](http://helioworkstation.com/translations/) for your native language or improve any of existing translations, which include English, German, French, Italian, Spanish and Russian.

### License

GNU GPL v3 © [Peter Rudenko](https://www.facebook.com/rudenko.peter)

See ``LICENSE`` for more information.

Built-in piano samples are the part of [Salamander Grand Piano by Alexander Holm](https://archive.org/details/SalamanderGrandPianoV3), distributed under [CC-BY license](https://creativecommons.org/licenses/by/3.0/).

Used fonts and icons taken from various free icon fonts (see [Icomoon](https://icomoon.io)) are licensed under [SIL Open Font License](http://scripts.sil.org/cms/scripts/page.php?id=OFL) and [CC-BY](https://creativecommons.org/licenses/by/3.0/).

All Helio logos and translations are also distrubuted under [CC-BY](https://creativecommons.org/licenses/by/3.0/).
