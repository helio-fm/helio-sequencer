#!/bin/bash

set -e
# TODO check for $TRAVIS_BRANCH and process develop and master branches differently

# Create tarball
cd ${TRAVIS_BUILD_DIR}/Projects/LinuxMakefile/build
tar --transform 's/.*/\L&/' -czf /tmp/deploy.tar.gz ./Helio
scp -C /tmp/deploy.tar.gz deploy@helio.fm:/opt/musehackers/files/ci/helio-dev-64-bit.tar.gz

# Create deb package
cd ${TRAVIS_BUILD_DIR}/Projects
mkdir -p ./Deployment/Linux/Debian/x64/usr/bin
cp ./LinuxMakefile/build/Helio ./Deployment/Linux/Debian/x64/usr/bin/helio
chmod +x ./Deployment/Linux/Debian/x64/usr/bin/helio

dpkg-deb --build ./Deployment/Linux/Debian/x64
scp -C ./Deployment/Linux/Debian/x64.deb deploy@helio.fm:/opt/musehackers/files/ci/helio-dev-64-bit.deb

# Create AppImage
cd ${TRAVIS_BUILD_DIR}
mkdir -p ./Projects/Deployment/Linux/AppImage/x64/usr/bin
cp ./Projects/LinuxMakefile/build/Helio ./Projects/Deployment/Linux/AppImage/x64/usr/bin/helio
chmod +x ./Projects/Deployment/Linux/AppImage/x64/usr/bin/helio

unset QTDIR; unset QT_PLUGIN_PATH; unset LD_LIBRARY_PATH

wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod a+x linuxdeployqt-continuous-x86_64.AppImage

./linuxdeployqt-continuous-x86_64.AppImage --appimage-extract
./squashfs-root/AppRun ./Projects/Deployment/Linux/AppImage/x64/usr/share/applications/Helio.desktop -appimage

mv Helio*.AppImage helio-dev-64-bit.AppImage
scp -C helio-dev-64-bit.AppImage deploy@helio.fm:/opt/musehackers/files/ci/helio-dev-64-bit.AppImage
