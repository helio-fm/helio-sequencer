#!/bin/bash

set -e

# Bail out on PR builds immediately
if [[ ${TRAVIS_PULL_REQUEST} != "false" ]]; then
    echo "Skipping setup step for PR build"
    exit 0
fi

# Decrypt the secrets
openssl aes-256-cbc -K $encrypted_4bdaf07bd33c_key -iv $encrypted_4bdaf07bd33c_iv -in ${TRAVIS_BUILD_DIR}/Projects/Deployment/Travis/certificates-ios.tar.enc -out /tmp/certificates.tar -d
tar xvf /tmp/certificates.tar -C /tmp

# Create the keychain with a password
security create-keychain -p travis ios-build.keychain

# Make the custom keychain default, so xcodebuild will use it for signing
security default-keychain -s ios-build.keychain

# Unlock the keychain
security unlock-keychain -p travis ios-build.keychain
security set-keychain-settings -lut 3600 ~/Library/Keychains/ios-build.keychain;

# Add certificates to keychain and allow codesign to access them
security import /tmp/apple-wwdrca.cer -k ~/Library/Keychains/ios-build.keychain -T /usr/bin/codesign
security import /tmp/ios-distribution-certificate.cer -k ~/Library/Keychains/ios-build.keychain -T /usr/bin/codesign
security import /tmp/ios-distribution-certificate.p12 -k ~/Library/Keychains/ios-build.keychain -P "" -T /usr/bin/codesign
mkdir -p ~/Library/MobileDevice/Provisioning\ Profiles
cp /tmp/ios-distribution-appstore.mobileprovision ~/Library/MobileDevice/Provisioning\ Profiles/

# Prevent locking keychain for codesign on macOS Sierra and later
security set-key-partition-list -S apple-tool:,apple:,codesign: -s -k travis ~/Library/Keychains/ios-build.keychain
