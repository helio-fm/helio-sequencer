#!/bin/bash

# NDK is not included in Travis initial setup:
echo y | sdkmanager 'ndk-bundle'
echo y | sdkmanager 'cmake;3.6.4111459'
