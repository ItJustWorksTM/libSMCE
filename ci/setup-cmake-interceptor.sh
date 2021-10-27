#!/usr/bin/env bash
base="${GITHUB_WORKSPACE}"
cmake_bindir="$(dirname "$(which cmake)")"

mv "${cmake_bindir}/cmake" "${cmake_bindir}/cmake-real"
sed "s@INJECTION-POINT@cmake_bindir=\"${cmake_bindir}\"@" "${base}/ci/cmake-interceptor.sh" > "${cmake_bindir}/cmake"
chmod +x "${cmake_bindir}/cmake"
