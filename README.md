## ![Vive la culture libre](Resources/screenshot-v1.png)
[Helio Workstation](https://helio.fm) is free and open-source music sequencer, designed to be used on all major platforms.


## Why another sequencer?

Most of the DAW interfaces often seem [overcomplicated](http://mashable.com/2015/09/18/german-u-boat/), and they only tend to get more and more bloated over time. Many of them are commercial, proprietary-licensed and almost none of them support all major operating systems at the same time.

Helio is an attempt to rethink a music sequencer to create a tool that **feels right**.

It aims to be a modern music creation software, featuring linear-based sequencer with a clean interface, high-performance C++ core, integrated version control providing intelligent synchronization between devices, saved undo history, translations to many languages and more.


## Building

||Linux|Windows|macOS|
|---|---|---|---|
|master|[![Linux Build Status](https://travis-ci.org/helio-fm/helio-workstation.svg?branch=master)](https://travis-ci.org/helio-fm/helio-workstation)|[![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/helio-fm/helio-workstation?svg=true&branch=master)](https://ci.appveyor.com/project/helio-fm/helio-workstation)|[![macOS Build Status](https://travis-ci.org/helio-fm/helio-workstation.svg?branch=master)](https://travis-ci.org/helio-fm/helio-workstation)|
|develop|[![Linux Build Status](https://travis-ci.org/helio-fm/helio-workstation.svg?branch=develop)](https://travis-ci.org/helio-fm/helio-workstation)|[![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/helio-fm/helio-workstation?svg=true&branch=develop)](https://ci.appveyor.com/project/helio-fm/helio-workstation)|[![macOS Build Status](https://travis-ci.org/helio-fm/helio-workstation.svg?branch=develop)](https://travis-ci.org/helio-fm/helio-workstation)|

### Basic build instructions

 * Git clone with submodules: `git clone --recurse-submodules https://github.com/helio-fm/helio-workstation.git`.
 * Install dependencies:
   * On Windows, get ASIO SDK (which can't be redistributed in this project due to licensing restrictions, but you may use `ThirdParty/ASIO/get_asio_sdk.ps1` powershell script to download and extract the SDK source).
   * On Linux, you'll need to have the following packages installed: `libfreetype6-dev libx11-dev libxinerama-dev libxrandr-dev libxcursor-dev libxcomposite-dev mesa-common-dev freeglut3-dev libcurl4-openssl-dev libasound2-dev libjack-dev libc++-dev`; the makefile assumes you've set up either `export CONFIG=Debug`, `export CONFIG=Release32` or `export CONFIG=Release64` before you `make`.
 * Pick the right project for your OS from `Projects` directory and build.

### Development builds

* Windows: [ci.helio.fm/helio-dev-64-bit.zip](https://ci.helio.fm/helio-dev-64-bit.zip)
* macOS: [ci.helio.fm/helio-dev.dmg](https://ci.helio.fm/helio-dev.dmg)
* Linux: [ci.helio.fm/helio-dev-64-bit.tar.gz](https://ci.helio.fm/helio-dev-64-bit.tar.gz)


## Links

 * [Homepage](https://helio.fm) with all latest builds and its [source code](https://github.com/helio-fm/muse-hackers).
 * Project page at [KVR database](https://www.kvraudio.com/product/helio-workstation-by-peter-rudenko).
 * Some [screencasts on Youtube](https://www.youtube.com/channel/UCO3K8iCd1k2FTqSocoE-WXw/).
 * [Discussion](https://news.ycombinator.com/item?id=14212054) at HN.


## Contributing

Helio is still a work in progress with many essential features missing, and there are several ways you could help:

* [Proofread and improve the translation](https://helio.fm/translations) for your native language.

* Your ideas are welcome: friendly and lightweight UI is the main development priority in this project, and if you have a vision on how to improve user experience in music composing, feel free to share it.

* Implement new features: some guys were asking me if I have any tasks for them to start working on. But, as a sole unpaid developer with limited amount of time and interest, I'm not planning/decomposing stories or organising the board, instead I only work on the parts that are fun and important for me at the moment. If you feel like contributing and don't know where to start, my suggestion is that you do the same: find a missing feature or behavior you're lacking the most, and make the app a little more convinient for yourself. Anyway, feel free to ask questions and create incomplete PR's to get intermediate feedback.


## Contributors

This project exists thanks to all the people who contribute. 
<a href="https://github.com/helio-fm/helio-workstation/graphs/contributors"><img src="https://opencollective.com/helio-workstation/contributors.svg?width=890&button=false" /></a>


### Translation and proofreading, in alphabetical order

Afrikaans - Jacques Viviers  
Brazilian Portuguese - Dario Silva  
Chinese - Bowen Sun  
Italian - Claudio Stano  
Japanese - Kotone Itaya  
Korean - YoungGwang Jeon, DaYeon Lee, HyoHee Jeon  
Polish - Dawid Bugajski  


### Sponsors

Support this project by becoming a sponsor. Your logo will show up here with a link to your website. [[Become a sponsor](https://opencollective.com/helio-workstation#sponsor)]

<a href="https://opencollective.com/helio-workstation/sponsor/0/website" target="_blank"><img src="https://opencollective.com/helio-workstation/sponsor/0/avatar.svg"></a>
<a href="https://opencollective.com/helio-workstation/sponsor/1/website" target="_blank"><img src="https://opencollective.com/helio-workstation/sponsor/1/avatar.svg"></a>
<a href="https://opencollective.com/helio-workstation/sponsor/2/website" target="_blank"><img src="https://opencollective.com/helio-workstation/sponsor/2/avatar.svg"></a>
<a href="https://opencollective.com/helio-workstation/sponsor/3/website" target="_blank"><img src="https://opencollective.com/helio-workstation/sponsor/3/avatar.svg"></a>
<a href="https://opencollective.com/helio-workstation/sponsor/4/website" target="_blank"><img src="https://opencollective.com/helio-workstation/sponsor/4/avatar.svg"></a>
<a href="https://opencollective.com/helio-workstation/sponsor/5/website" target="_blank"><img src="https://opencollective.com/helio-workstation/sponsor/5/avatar.svg"></a>
<a href="https://opencollective.com/helio-workstation/sponsor/6/website" target="_blank"><img src="https://opencollective.com/helio-workstation/sponsor/6/avatar.svg"></a>
<a href="https://opencollective.com/helio-workstation/sponsor/7/website" target="_blank"><img src="https://opencollective.com/helio-workstation/sponsor/7/avatar.svg"></a>
<a href="https://opencollective.com/helio-workstation/sponsor/8/website" target="_blank"><img src="https://opencollective.com/helio-workstation/sponsor/8/avatar.svg"></a>
<a href="https://opencollective.com/helio-workstation/sponsor/9/website" target="_blank"><img src="https://opencollective.com/helio-workstation/sponsor/9/avatar.svg"></a>


## License

GNU GPL v3, see ``LICENSE`` for more information.

Icons taken from various [free icon fonts](https://icomoon.io) are licensed under [SIL Open Font License](http://scripts.sil.org/cms/scripts/page.php?id=OFL) and [CC-BY](https://creativecommons.org/licenses/by/3.0/).

Built-in piano samples are the part of [Salamander Grand Piano](https://archive.org/details/SalamanderGrandPianoV3) by Alexander Holm, distributed under [CC-BY](https://creativecommons.org/licenses/by/3.0/).

All Helio logos and translations are distrubuted under [CC-BY](https://creativecommons.org/licenses/by/3.0/) as well.
