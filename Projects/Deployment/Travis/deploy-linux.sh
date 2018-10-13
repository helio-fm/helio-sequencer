#!/bin/bash

set -e
# TODO check for $TRAVIS_BRANCH and process develop and master branches differently

# For develop build, just create a tarball and upload
# cd ${TRAVIS_BUILD_DIR}/Projects/LinuxMakefile/build
# tar --transform 's/.*/\L&/' -czf /tmp/deploy.tar.gz ./Helio
# scp -C /tmp/deploy.tar.gz deploy@helio.fm:/opt/musehackers/files/ci/helio-dev.tar.gz

# In future, this could be improved by building an AppImage,
# or a .deb package, and uploading them as well

sed -i 's,/opt/helio_workstation/,,g' ./Projects/Deployment/Linux/Helio-*-amd64/usr/share/applications/Helio.desktop
sed -i 's,helio_workstation,helio,g' ./Projects/Deployment/Linux/Helio-*-amd64/usr/share/applications/Helio.desktop

cp Projects/LinuxMakefile/build/Helio ./Projects/Deployment/Linux/Helio-*-amd64/usr/bin/helio
chmod +x ./Projects/Deployment/Linux/Helio-*-amd64/usr/bin/helio

# # Copy icon to here
cp ./Projects/Deployment/Linux/Helio*-amd64/usr/share/{icons/hicolor/256x256/apps/helio-workstation.png,applications}

unset QTDIR; unset QT_PLUGIN_PATH ; unset LD_LIBRARY_PATH
VERSION=$(git rev-parse --short HEAD)
export VERSION

cd "$TRAVIS_BUILD_DIR"
wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod a+x linuxdeployqt-continuous-x86_64.AppImage

./linuxdeployqt-continuous-x86_64.AppImage --appimage-extract
./squashfs-root/AppRun ./Projects/Deployment/Linux/Helio-*-amd64/usr/share/applications/Helio.desktop -appimage

curl --upload-file Helio*.AppImage "https://transfer.sh/helio_workstation-git.$(git rev-parse --short HEAD)-x86_64.AppImage"
