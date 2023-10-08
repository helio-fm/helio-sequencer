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
    -exportOptionsPlist ./Travis/export-options-osx.plist;

DISK_IMAGE_DIR="/tmp/disk-image"
mkdir -p ${DISK_IMAGE_DIR}
cp -r /tmp/exported.app/Helio.app ${DISK_IMAGE_DIR}/Helio.app

# Simply copy the the pre-generated .DS_Store file (styling data),
# generating it on the fly with applescript only works in GUI mode since Mojave
cp ./macOS/DS_Store ${DISK_IMAGE_DIR}/.DS_Store

# Create disk image
bash ./macOS/create-dmg.sh \
    --volname "Helio Installer" \
    --app-drop-link 100 100 \
    --no-internet-enable \
    /tmp/${RELEASE_FILENAME}.dmg \
    ${DISK_IMAGE_DIR}/

# Sign
codesign --force --sign "Developer ID Application: Peter Rudenko (${OSX_ITC_PROVIDER_ID})" /tmp/${RELEASE_FILENAME}.dmg

# Notarize
# "The notary service generates a ticket for the top-level file ... as well as each nested file"
xcrun notarytool submit /tmp/${RELEASE_FILENAME}.dmg --apple-id "${OSX_ITC_USERNAME}" --password "${OSX_ITC_APP_PASSWORD}" --team-id "${OSX_ITC_PROVIDER_ID}" --verbose --wait

# Staple and verify
xcrun stapler staple /tmp/${RELEASE_FILENAME}.dmg
spctl -a -t open --context context:primary-signature -v /tmp/${RELEASE_FILENAME}.dmg

# Finally, upload
scp -o StrictHostKeyChecking=no -C /tmp/${RELEASE_FILENAME}.dmg ${DEPLOY_HOST}:${DEPLOY_PATH}/${RELEASE_FILENAME}.dmg
