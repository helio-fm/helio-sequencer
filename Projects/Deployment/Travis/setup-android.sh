#!/bin/bash

set -e

if [[ ${TRAVIS_PULL_REQUEST} != "false" ]]; then

    # For PR builds, copy a public debug keystore
    cp ${TRAVIS_BUILD_DIR}/Projects/Deployment/Travis/helio.debug.jks ${HOME}/helio.keystore

else

    # Decrypt the release keystore
    openssl aes-256-cbc -K $encrypted_cf8a6ca4972b_key -iv $encrypted_cf8a6ca4972b_iv -in ${TRAVIS_BUILD_DIR}/Projects/Deployment/Travis/helio.keystore.enc -out ${HOME}/helio.keystore -d

fi

echo y | sdkmanager 'cmake;3.22.1'
