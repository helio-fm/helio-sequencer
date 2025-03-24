#!/bin/bash

set -e

# Bail out on PR builds immediately
if [[ ${TRAVIS_PULL_REQUEST} != "false" ]]; then
    echo "Skipping deployment for PR build"
    exit 0
fi

# Assumed to be run either on a development branch or a tagged master branch
if [[ ${TRAVIS_BRANCH} == "develop" ]]; then
    RELEASE_FILENAME="helio-dev"
elif [[ ${TRAVIS_TAG} != "" ]]; then
    RELEASE_FILENAME="helio-${TRAVIS_TAG}"
else
    echo "Skipping deployment: will only run either on a tagged commit, or a develop branch commit"
    exit 0
fi

#####################################
# Upload apk

cd ${TRAVIS_BUILD_DIR}/Projects/Android/app/build/outputs/apk/
scp -o StrictHostKeyChecking=no -C ./release_/release/app-release_-release.apk ${DEPLOY_HOST}:${DEPLOY_PATH}/${RELEASE_FILENAME}.apk
