#!/bin/bash

if [[ -v GITHUB_WORKSPACE ]]; then
  base_dir=$GITHUB_WORKSPACE
else
  base_dir=$(pwd)
fi

negative_report='{ "coverage": {} }'

for f in $(find "${base_dir}" -type d \( -path "${base_dir}/build" -o -path "${base_dir}/cmake-build*" -o -path "${base_dir}/boost" \) -prune -o \( -name 'CMakeLists.txt' -o -name '*.cmake' \)); do
  echo 'Scanning' $f
  json_lno=$(jq -sc 'map({ (.|tostring): 0}) | add' < <(perl -nE '/(?:\[(=+)\[(?:.|\n)+?\]\1\](?:.|\n)+?)*^\K(?=[ \t]*?\w+?\s*?\()/ && say $.' "$f"))
  negative_report=$(jq -c --arg fname "$f" --argjson lines "${json_lno}" '.coverage += {($fname): $lines}' <<< "${negative_report}")
done

jq -c "select(has(\"version\") | not) | select(.file | startswith(\"${base_dir}/build\") | not) | select(.file | startswith(\"${base_dir}\")) | {\"file\": .file, \"line\": .line}" CMakeTrace/*.json \
| jq -nc --argjson init "${negative_report}" 'reduce inputs as $e ($init; .coverage[$e.file][$e.line | tostring] += 1)' \
| sed "s#${base_dir}/##g" \
> cmake-codecov-report.json
