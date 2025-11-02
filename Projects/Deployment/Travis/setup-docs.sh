#!/bin/bash

set -e

# Bail out on PR builds immediately
if [[ ${TRAVIS_PULL_REQUEST} != "false" ]]; then
    echo "Skipping setup step for PR build"
    exit 0
fi

(test -x $HOME/.cargo/bin/cargo-install-update || cargo install cargo-update)
(test -x $HOME/.cargo/bin/mdbook || cargo install mdbook)
cargo install-update -a

# A custom (smaller) code highlighter and a theme for it
mkdir ${TRAVIS_BUILD_DIR}/Docs/theme
cd ${TRAVIS_BUILD_DIR}/Docs/theme
wget https://unpkg.com/@highlightjs/cdn-assets@10.1.1/highlight.min.js -O highlight.js
sed -i '/hljs.registerLanguage/d' ./highlight.js
wget https://unpkg.com/@highlightjs/cdn-assets@10.1.1/languages/json.min.js -O ->> highlight.js
wget https://unpkg.com/@highlightjs/cdn-assets@10.1.1/languages/lisp.min.js -O ->> highlight.js
wget https://unpkg.com/@highlightjs/cdn-assets@11.11.1/styles/github-dark-dimmed.css -O tomorrow-night.css
