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

# based on scheme grammar for highlight.js:
echo $'hljs.registerLanguage("lisp",function(){"use strict";return e=>{const t="[^\\\\(\\\\)\\",\'`;#|\\\\\\\\\\\\s]+",n="(-|\\\\+)?\\\\d+([./]\\\\d+)?",r={$pattern:t,name:"define lambda filter reduce map if begin - + * / % = != <= >= λ"},a={className:"literal",begin:"(nil|true|false|d2|d3|d4|d5|d6|d7|d8|d9|d10)"},i={className:"number",begin:n},c=e.QUOTE_STRING_MODE,s=[e.COMMENT(";","$")],l={begin:t},u={endsWithParent:!0},d={className:"symbol",begin:t,keywords:r},p={begin:"\\\\(",end:"\\\\)",contains:[{begin:/lambda/,endsWithParent:!0,returnBegin:!0,contains:[d,{endsParent:!0,begin:/\(/,end:/\)/,contains:[l]}]},d,u]};return u.contains=[a,i,c,l,p].concat(s),{name:"Scheme",illegal:/\S/,contains:[e.SHEBANG(),i,c,p].concat(s)}}}());' >> highlight.js

wget https://unpkg.com/@highlightjs/cdn-assets@11.11.1/styles/atom-one-dark.css -O ->> tomorrow-night.css
