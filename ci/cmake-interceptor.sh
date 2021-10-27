#!/usr/bin/env bash
base="${GITHUB_WORKSPACE}"

INJECTION-POINT

echo "$@" >> "$base/CMakeCalls.log"
case "$1" in
  -E)
    ;&
  --build)
    ;&
  --install)
    ;&
  --version)
    ;&
  --find-package)
    exec -a "$cmake_bindir/cmake" "$cmake_bindir/cmake-real" "$@"
    ;;
  *)
    if ! [[ -d "$base/CMakeTrace" ]]; then
      mkdir -p "$base/CMakeTrace"
      echo 0 > "$base/CMakeTrace/.count"
    fi
    flock -x "$base/CMakeTrace/.lock" true
    idx=$(<$base/CMakeTrace/.count)
    echo $((idx+1)) > "$base/CMakeTrace/.count"
    flock -u "$base/CMakeTrace/.lock" true
    exec -a "$cmake_bindir/cmake" "$cmake_bindir/cmake-real" --trace-format=json-v1 "--trace-redirect=${base}/CMakeTrace/${idx}.json" "$@"
esac
