# Introduction

Welcome to the documentation for the Helio project, a free lightweight music sequencer that runs on all major desktop and mobile operating systems.

Helio was designed to save me time struggling with the MIDI editor, so I could focus more on musical ideas. In this documentation, I'll also try to save time by keeping it as short and simple as possible.

If you notice that something important is missing, send me an [email](mailto:peter.rudenko@gmail.com), or file a [PR](https://github.com/helio-fm/helio-sequencer/pulls) on Github.

*Some generated content will be missing if you read this page in the project's repository, see the full rendered version at [docs.helio.fm](https://docs.helio.fm).*

#### How to read this

If you're starting out with Helio, just keep reading the [Getting Started](getting-started.md) page to learn the basic concepts of UI navigation, setting up [instruments](getting-started.md#instruments), [creating](getting-started.md#creating-a-project) a project, [editing and arranging](getting-started.md#editing-and-arranging) it, and saving your work in the [version control](getting-started.md#version-control).

Otherwise, you might find the [editing tips](tips-and-tricks.md) useful: the majority of the nifty tools and hacks are described there.

If you've already tinkered with Helio for a while, you'll probably only need the [hotkeys](hotkeys.md) section.

## Installation

Helio is released as a single portable executable file, where possible, but installers are also available for some systems.
In both flavors it has a small disk footprint (less than 10 Mb) and should run on most available hardware and platform versions.

### Portable vs installer

Installers are provided for Windows and Debian-based Linux distributions. The installer version is only required if you want the desktop shortcut and the uninstall tool.

To use the portable version, simply download the compressed archive and unzip it to a folder of your choice.

### 32 or 64 bits

The only practical difference between the two is that the 64-bit version can only host 64-bit plugins, and the 32-bit version can only host 32-bit plugins.

Helio does not currently support plugin sandboxing or hosting both 32-bit and 64-bit plugins.

### Stable build or development build

The stable builds are supposed to be more reliable, but they are updated less frequently. You can keep up to date on the latest changes in stable builds through the [release notes](changelog.md). Install the development build if you want to test new features or verify bug fixes.

The documentation is not versioned at the moment, and some of the features described at [docs.helio.fm](https://docs.helio.fm) may be present only in the development build.

## Used directories

Helio keeps all files in two directories: one for project files, and another for configuration files.

### The configuration directory

All configuration files are created on the first start under the user application data directory. The directory location is platform-dependent:

* Windows: %APPDATA%\Helio
* macOS: ~/Library/Application Support/Helio
* Linux: ~/.config/Helio

#### *settings.helio*

This file basically contains all the settings, so deleting or renaming it will cause the app to run as if it were the first time. The settings are intended to be human-readable and are stored in XML format.

#### *translations.helio* and maybe others

If the update checks are enabled, some additional [resources](configs.md) are updated in the runtime. At the moment of writing, only translations are being updated in this manner.

### The projects directory

The project files are saved to the user's default documents folder in the `Helio` subfolder. This directory's location is also platform-dependent: 

* Windows: %HOMEPATH%\Helio (for example, "c:\Users\Peter\Documents\Helio\")
* macOS: ~/Documents/Helio
* Linux: ~/Documents/Helio

## Building from source

Building the app from source will require a C++14 compiler, e.g. GCC 5.0 or Clang 3.4 on Linux, Visual Studio 2015 on Windows, or Xcode 9.2 on macOS.

Minimum deployment targets are:

* Windows: Windows Vista
* macOS: macOS 10.7
* Linux: mainstream distributions

#### Basic build instructions

* Clone with submodules: `git clone --recursive https://github.com/helio-fm/helio-sequencer.git`.
* Install dependencies:
  * On Windows, get ASIO SDK (which can't be redistributed in this project due to licensing restrictions, but you may use `ThirdParty/ASIO/get_asio_sdk.ps1` powershell script to download and extract the SDK source).
  * On Linux, you'll need to have the following packages installed: `libfreetype6-dev libx11-dev libxinerama-dev libxrandr-dev libxcursor-dev libxcomposite-dev mesa-common-dev freeglut3-dev libasound2-dev libjack-dev libc++-dev`; the makefile assumes you've set up either `export CONFIG=Debug`, `export CONFIG=Release32` or `export CONFIG=Release64` before you `make`.
* Pick the right project for your OS from the `Projects` directory and build.
