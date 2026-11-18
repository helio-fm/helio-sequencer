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
echo $'hljs.registerLanguage("lisp",function(){"use strict";return e=>{const t="[^\\\\(\\\\)\\\\[\\\\]\\\\{\\\\}\\",\'`;#|\\\\\\\\\\\\s]+",n="(-|\\\\+)?\\\\d+([./]\\\\d+)?",r={$pattern:t,name:"define lambda filter filter-indexed reduce map map-indexed for-each if cond begin modulo nth - + * / % = != <= >= λ"},a={className:"literal",begin:"(nil|true|false|d99|d32|d30|d20|d16|d15|d14|d13|d12|d11|d10|d9|d8|d7|d6|d5|d4|d3|d2|d1)"},i={className:"number",variants:[{begin:n,relevance:0},{begin:n+"[+\\\\-]"+n+"i",relevance:0},{begin:"#b[0-1]+(/[0-1]+)?"},{begin:"#o[0-7]+(/[0-7]+)?"},{begin:"#x[0-9a-f]+(/[0-9a-f]+)?"}]},c=e.QUOTE_STRING_MODE,s=[e.COMMENT(";","$",{relevance:0}),e.COMMENT("#\\\\|","\\\\|#")],l={begin:t,relevance:0},o={className:"literal",begin:"\'"+t},u={endsWithParent:!0,relevance:0},g={variants:[{begin:/\'/}],contains:[{begin:"\\\\(",end:"\\\\)",contains:["self",a,c,i,l,o]}]},d={className:"symbol",relevance:0,begin:t,keywords:r},p={variants:[{begin:"\\\\(",end:"\\\\)"},{begin:"\\\\[",end:"\\\\]"}],contains:[{begin:/lambda/,endsWithParent:!0,returnBegin:!0,contains:[d,{endsParent:!0,variants:[{begin:/\(/,end:/\)/},{begin:/\[/,end:/\]/}],contains:[l]}]},d,u]};return u.contains=[a,i,c,l,o,g,p].concat(s),{name:"Scheme",illegal:/\S/,contains:[e.SHEBANG(),i,c,o,g,p].concat(s)}}}());' >> highlight.js

wget https://unpkg.com/@highlightjs/cdn-assets@11.11.1/styles/atom-one-dark.css -O ->> tomorrow-night.css
