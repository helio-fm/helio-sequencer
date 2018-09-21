#!/bin/bash

# TODO check for $TRAVIS_BRANCH and process develop and master branches differently

# For develop build, just create a tarball and upload
cd ${TRAVIS_BUILD_DIR}/Projects/LinuxMakefile/build
tar --transform 's/.*/\L&/' -czf /tmp/deploy.tar.gz ./Helio
scp -C /tmp/deploy.tar.gz deploy@helio.fm:/opt/musehackers/files/ci/helio-dev.tar.gz

# In future, this could be improved by building an AppImage,
# or a .deb package, and uploading them as well
