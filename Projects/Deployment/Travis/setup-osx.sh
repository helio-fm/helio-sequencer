#!/bin/bash

# Create the keychain with a password
security create-keychain -p travis osx-build.keychain

# Make the custom keychain default, so xcodebuild will use it for signing
security default-keychain -s osx-build.keychain

# Unlock the keychain
security unlock-keychain -p travis osx-build.keychain
security set-keychain-settings -lut 3600 ~/Library/Keychains/osx-build.keychain;

# Add certificates to keychain and allow codesign to access them
security import /tmp/apple-wwdrca.cer -k ~/Library/Keychains/osx-build.keychain -T /usr/bin/codesign
security import /tmp/developer-id-certificate.cer -k ~/Library/Keychains/osx-build.keychain -T /usr/bin/codesign
security import /tmp/developer-id-certificate.p12 -k ~/Library/Keychains/osx-build.keychain -P "" -T /usr/bin/codesign
mkdir -p ~/Library/MobileDevice/Provisioning\ Profiles
cp /tmp/developer-id-mac-production.provisionprofile ~/Library/MobileDevice/Provisioning\ Profiles

# Prevent locking keychain for codesign on macOS Sierra and later
security set-key-partition-list -S apple-tool:,apple:,codesign: -s -k travis ~/Library/Keychains/osx-build.keychain
