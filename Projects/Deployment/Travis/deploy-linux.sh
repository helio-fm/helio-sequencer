#!/bin/bash

set -e

# Bail out on PR builds immediately
if [[ ${TRAVIS_PULL_REQUEST} != "false" ]]; then
    echo "Skipping deployment for PR build"
    exit 0
fi

# TODO add 32-bit builds someday
ARCH="x64"

# Assumed to be run either on a development branch or a tagged master branch
if [[ ${TRAVIS_BRANCH} == "develop" ]]; then
    RELEASE_FILENAME="helio-dev-${ARCH}"
elif [[ ${TRAVIS_TAG} != "" ]]; then
    RELEASE_FILENAME="helio-${TRAVIS_TAG}-${ARCH}"
else
    echo "Skipping deployment: will only run either on a tagged commit, or a develop branch commit"
    exit 0
fi

#####################################
# Create tarball

cd ${TRAVIS_BUILD_DIR}/Projects/LinuxMakefile/build
tar --transform 's/.*/\L&/' -czf ${RELEASE_FILENAME}.tar.gz ./helio
scp -C ${RELEASE_FILENAME}.tar.gz ${DEPLOY_HOST}:${DEPLOY_PATH}/${RELEASE_FILENAME}.tar.gz

#####################################
# Create deb package

cd ${TRAVIS_BUILD_DIR}/Projects
mkdir -p ./Deployment/Linux/Debian/${ARCH}/usr/bin
cp ./LinuxMakefile/build/helio ./Deployment/Linux/Debian/${ARCH}/usr/bin/helio
chmod +x ./Deployment/Linux/Debian/${ARCH}/usr/bin/helio

dpkg-deb --build ./Deployment/Linux/Debian/${ARCH}
scp -C ./Deployment/Linux/Debian/${ARCH}.deb ${DEPLOY_HOST}:${DEPLOY_PATH}/${RELEASE_FILENAME}.deb

#####################################
# Create AppImage

cd ${TRAVIS_BUILD_DIR}/Projects
mkdir -p ./Deployment/Linux/AppImage/${ARCH}/usr/bin
cp ./LinuxMakefile/build/helio ./Deployment/Linux/AppImage/${ARCH}/usr/bin/helio
chmod +x ./Deployment/Linux/AppImage/${ARCH}/usr/bin/helio

unset QTDIR; unset QT_PLUGIN_PATH; unset LD_LIBRARY_PATH

wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod a+x linuxdeployqt-continuous-x86_64.AppImage

./linuxdeployqt-continuous-x86_64.AppImage --appimage-extract
./squashfs-root/AppRun ./Deployment/Linux/AppImage/${ARCH}/usr/share/applications/Helio.desktop -appimage

mv Helio*.AppImage ${RELEASE_FILENAME}.AppImage
scp -C ${RELEASE_FILENAME}.AppImage ${DEPLOY_HOST}:${DEPLOY_PATH}/${RELEASE_FILENAME}.AppImage
