#!/bin/bash

set -e

# Bail out on PR builds immediately
if [[ ${TRAVIS_PULL_REQUEST} != "false" ]]; then
    echo "Skipping setup step for PR build"
    exit 0
fi

(test -x $HOME/.cargo/bin/cargo-install-update || cargo install cargo-update)
(test -x $HOME/.cargo/bin/mdbook || cargo install --vers "^0.1" mdbook)
cargo install-update -a
