#!/bin/bash

# Decrypt the secrets
openssl aes-256-cbc -K $encrypted_cf8a6ca4972b_key -iv $encrypted_cf8a6ca4972b_iv -in ${TRAVIS_BUILD_DIR}/Projects/Deployment/Travis/helio.keystore.enc -out ${HOME}/helio.keystore -d

# NDK is not included in Travis initial setup:
echo y | sdkmanager 'ndk-bundle'
echo y | sdkmanager 'cmake;3.6.4111459'
