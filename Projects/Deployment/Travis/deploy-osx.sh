#!/bin/sh

# TODO check for $TRAVIS_BRANCH and process develop and master branches differently

cd ${TRAVIS_BUILD_DIR}/Projects/Deployment/Travis
xcodebuild -archivePath /tmp/archive.xcarchive -exportArchive -exportPath /tmp/exported.app -exportOptionsPlist export-options.plist;
scp -C /tmp/exported.app/Helio.app deploy@helio.fm:/opt/musehackers/files/ci/helio-dev.app
