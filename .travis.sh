#!/bin/bash

# Travis builds on Trusty image, and webkit2gtk-4.0 dev package is not available.
# But Projucer adds this dependency in the makefile,
# even if the project doesn't need it, i.e. has JUCE_WEB_BROWSER=0.
# So here is the dirty hackaround:
cd Projects/LinuxMakefile
sed -i 's/webkit2gtk-4.0//g' Makefile
make
