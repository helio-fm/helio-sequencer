#!/bin/bash

# TODO check for $TRAVIS_BRANCH and process develop and master branches differently

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
	/tmp/helio.dmg \
	/tmp/exported.app/Helio.app

# Sign (and check) it as well
codesign --force --sign "Developer ID Application: Peter Rudenko (EQL633LLC8)" /tmp/helio.dmg
spctl -a -t open --context context:primary-signature -v /tmp/helio.dmg

# Finally, upload
scp -C /tmp/helio.dmg deploy@helio.fm:/opt/musehackers/files/ci/helio-dev.dmg
