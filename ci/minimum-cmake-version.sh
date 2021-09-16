#!/bin/bash
INSTALLED_VERSION=$(cmake --version)
MINIMUM_VERSION=3.12.0

INSTALLED_VERSION_ARRAY=($INSTALLED_VERSION)

if [[ "${INSTALLED_VERSION_ARRAY[2]}" < "$MINIMUM_VERSION" ]]; then
  echo "CMake version not supported. Minimum version required 3.12.0"
fi

echo "CMake version supported."