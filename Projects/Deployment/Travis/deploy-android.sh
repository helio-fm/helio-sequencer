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
# Sign and upload apk

cd ${TRAVIS_BUILD_DIR}/Projects/Android/app/build/outputs/apk/

cp ${TRAVIS_BUILD_DIR}/Deployment/Travis/.keystore $HOME
jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore $HOME/keystore.jks -storepass $storepass -keypass $keypass ${RELEASE_FILENAME}-unsigned.apk helio_keystore

jarsigner -verify ${RELEASE_FILENAME}-unsigned.apk

"${ANDROID_HOME}/build-tools/28.0.3/zipalign -v 4 ${RELEASE_FILENAME}-unsigned.apk ${RELEASE_FILENAME}-signed.apk"

scp -C ${RELEASE_FILENAME}-signed.apk ${DEPLOY_HOST}:${DEPLOY_PATH}/${RELEASE_FILENAME}.apk
