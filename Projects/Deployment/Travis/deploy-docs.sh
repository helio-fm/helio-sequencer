#!/bin/bash

set -e

# Bail out on PR builds immediately
if [[ ${TRAVIS_PULL_REQUEST} != "false" ]]; then
    echo "Skipping deployment for PR build"
    exit 0
fi

# Assumed to be run either on a development branch or a tagged master branch
if [[ ${TRAVIS_BRANCH} != "develop" || ${TRAVIS_TAG} == "" ]]; then
    echo "Skipping deployment: will only run either on a tagged commit, or a develop branch commit"
    exit 0
fi

#####################################
# Simply copy the book

cd ${TRAVIS_BUILD_DIR}/Docs
scp -rp ./book ${DEPLOY_HOST}:${DOCS_PATH}
