#!/bin/sh

# Create a custom keychain
security create-keychain -p travis osx-build.keychain

# Make the custom keychain default, so xcodebuild will use it for signing
security default-keychain -s osx-build.keychain

# Unlock the keychain
security unlock-keychain -p travis osx-build.keychain

# Set keychain timeout to 1 hour for long builds
# see http://www.egeek.me/2013/02/23/jenkins-and-xcode-user-interaction-is-not-allowed/
security set-keychain-settings -t 3600 -l ~/Library/Keychains/osx-build.keychain

# Add certificates to keychain and allow codesign to access them
security import Projects/Deployment/Travis/apple.cer -k ~/Library/Keychains/osx-build.keychain -T /usr/bin/codesign
security import Projects/Deployment/Travis/dist.cer -k ~/Library/Keychains/osx-build.keychain -T /usr/bin/codesign
security import Projects/Deployment/Travis/dist.p12 -k ~/Library/Keychains/osx-build.keychain -P $KEY_PASSWORD -T /usr/bin/codesign

# Put the provisioning profile in place
mkdir -p ~/Library/MobileDevice/Provisioning\ Profiles
cp "./scripts/profile/${PROFILE_NAME}.mobileprovision" ~/Library/MobileDevice/Provisioning\ Profiles
