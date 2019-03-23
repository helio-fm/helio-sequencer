#!/bin/bash

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

cd ${TRAVIS_BUILD_DIR}/Projects/Deployment

# Build the .app
xcodebuild -archivePath \
	/tmp/archive.xcarchive \
	-exportArchive \
	-exportPath /tmp/exported.app \
	-exportOptionsPlist ./Travis/export-options.plist;

# Create disk image
bash ./macOS/create-dmg.sh \
	--volname Helio \
	--background ./macOS/splash.jpg \
	--window-size 700 400 \
	--icon Helio.app 230 150 \
	--app-drop-link 470 150 \
	--no-internet-enable \
	/tmp/${RELEASE_FILENAME}.dmg \
	/tmp/exported.app/Helio.app

# Sign (and check) it as well
codesign --force --sign "Developer ID Application: Peter Rudenko (EQL633LLC8)" /tmp/${RELEASE_FILENAME}.dmg
spctl -a -t open --context context:primary-signature -v /tmp/${RELEASE_FILENAME}.dmg

# Finally, upload
scp -C /tmp/${RELEASE_FILENAME}.dmg ${DEPLOY_HOST}:${DEPLOY_PATH}/${RELEASE_FILENAME}.dmg
