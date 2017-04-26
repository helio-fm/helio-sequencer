#!/bin/bash

# Travis builds on Trusty image, and webkit2gtk-4.0 dev package is not available.
# But Projucer adds this dependency in the makefile,
# even if the project doesn't need it, i.e. has JUCE_WEB_BROWSER=0.
# So here are some the dirty hackarounds:
pushd ThirdParty/JUCE/extras/Projucer/Builds/LinuxMakefile
sed -i 's/\/\/#define\ JUCE_WEB_BROWSER/#define\ JUCE_WEB_BROWSER 0/g' ../../JuceLibraryCode/AppConfig.h
sed -i 's/webkit2gtk-4.0//g' Makefile
export CONFIG=Release
make
./build/Projucer --resave ./../../../../../../Projects/Projucer/Helio\ Workstation.jucer
popd
cd Projects/LinuxMakefile
sed -i 's/webkit2gtk-4.0//g' Makefile
export CONFIG=Release64
make
