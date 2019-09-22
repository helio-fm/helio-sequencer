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

# Sign (and check)
codesign --force --sign "Developer ID Application: Peter Rudenko (EQL633LLC8)" /tmp/${RELEASE_FILENAME}.dmg
spctl -a -t open --context context:primary-signature -v /tmp/${RELEASE_FILENAME}.dmg

# Pain-in-the-ass notarization workflow
# "The notary service generates a ticket for the top-level file ... as well as each nested file", so let's pass the disk image
NOTARIZATION_RESPONSE=$(xcrun altool --notarize-app -t osx -f /tmp/${RELEASE_FILENAME}.dmg --primary-bundle-id "fm.helio" -u ${OSX_ITC_USERNAME} -p ${OSX_ITC_APP_PASSWORD} --output-format xml)
REQUEST_UUID=$(xmllint --xpath "/plist/dict[key='notarization-upload']/dict/key[.='RequestUUID']/following-sibling::string[1]/text()" - <<<"$NOTARIZATION_RESPONSE")
echo "Notarization started with request uuid: ${REQUEST_UUID}"
sleep 10s

for i in {0..40} # 20 mins should be enough I guess
do
    NOTARIZATION_INFO=$(xcrun altool --notarization-info ${REQUEST_UUID} -u ${OSX_ITC_USERNAME} -p ${OSX_ITC_APP_PASSWORD} --output-format xml)
    NOTARIZATION_STATUS=$(xmllint --xpath "/plist/dict[key='notarization-info']/dict/key[.='Status']/following-sibling::string[1]/text()" - <<<"$NOTARIZATION_INFO")
    if [[ ${NOTARIZATION_STATUS} == "in progress" ]]; then
        echo "Notarization in progress, waiting"
        sleep 30s
    elif [[ ${NOTARIZATION_STATUS} == "success" ]]; then
        echo "Notarization done"
        # Staple and verify
        xcrun stapler staple /tmp/${RELEASE_FILENAME}.dmg
        spctl -a -t open --context context:primary-signature -v /tmp/${RELEASE_FILENAME}.dmg
        break
    else
        echo "Notarization fault (core dumped)"
        echo ${NOTARIZATION_RESPONSE}
        echo ${NOTARIZATION_INFO}
        exit 1
    fi
done

if [[ ${NOTARIZATION_STATUS} != "success" ]]; then
    # It should be "less than an hour", Apple says, but looks like this time we're stuck
    echo "Ah fuck :("
    # Don't fail here though, it's better to ship the unstapled disk image, at least
    # exit 1
fi

# Finally, upload
scp -C /tmp/${RELEASE_FILENAME}.dmg ${DEPLOY_HOST}:${DEPLOY_PATH}/${RELEASE_FILENAME}.dmg
