#!/bin/bash

set -e

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

#####################################
# Update OpenSSH
#
# Ubuntu Trusty seems to be the only option for Android builds on Travis at the moment,
# but I need a more recent OpenSSH client to be able to connect to upload the build

sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 762E3157
sudo apt-add-repository 'deb http://old-releases.ubuntu.com/ubuntu yakkety main universe multiverse'

# Also remove dead(?) repos which fail with 503
sudo add-apt-repository --remove "deb http://toolbelt.heroku.com/ubuntu /"
sudo rm -fv /etc/apt/sources.list.d/heroku-toolbelt.list
sudo add-apt-repository --remove "deb https://download.docker.com/linux/ubuntu /"
sudo rm -fv /etc/apt/sources.list.d/docker.list

sudo apt-get update
sudo apt-get install openssh-server=1:7.3p1-1

#####################################
# Upload apk

cd ${TRAVIS_BUILD_DIR}/Projects/Android/app/build/outputs/apk/
scp -o StrictHostKeyChecking=no -C ./release_/release/app-release_-release.apk ${DEPLOY_HOST}:${DEPLOY_PATH}/${RELEASE_FILENAME}.apk
