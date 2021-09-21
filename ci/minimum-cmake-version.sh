#!/bin/bash
INSTALLED_VERSION=$(cmake --version)
MINIMUM_VERSION=3.12.0

INSTALLED_VERSION_ARRAY=($INSTALLED_VERSION)

CMAKE_LOCATION=$(where cmake)

HOST_OS_VERSION=$(uname -s)
HOST_OS=''

case $HOST_OS_VERSION in
   Darwin) HOST_OS='mac';;
   Linux) HOST_OS='linux';;
   CYGWIN*|MINGW32*|MSYS*|MINGW*) HOST_OS='windows';;
   *) HOST_OS='other';;
esac

'''
if [[ "$HOST_OS" = "windows" ]]; then
  if [[ -f $CHOCO_LOCATION ]]; then
    echo "Chocolatey found on host."
  else 
    echo "Chocolatey not found on host."
    echo "Install Chocolatey manually."
    # Install Choco
elif [[ "$HOST_OS" = "mac" ]]; then
  if [[ -f $BREW_LOCATION ]]; then
    echo "Brew found on host."
  else 
    echo "Brew not found on host."
    echo "Install Brew manually."
    # Install Brew
elif [[ "$HOST_OS" = "linux" ]]; then
  if [[ -f $BREW_LOCATION ]]; then
    echo "apt-get found on host."
  else 
    echo "apt-get not found on host."
    echo "Install apt-get manually."
    # Install apt-get
elif [[ "$HOST_OS" = "other" ]];  then
  echo "Host OS not supported."
fi
'''

if [[ -f $CMAKE_LOCATION ]]; then
  echo "CMake found on host."
else 
  echo "CMake not found on host."
  if [[ "$HOST_OS" = "windows" ]]; then
    choco install cmake
  elif [[ "$HOST_OS" = "mac" ]]; then
    brew install cmake
  elif [[ "$HOST_OS" = "linux" ]]; then
    sudo apt-get install cmake
  elif [[ "$HOST_OS" = "other" ]];  then
    echo "Host OS not supported."
  fi

fi

if [[ "${INSTALLED_VERSION_ARRAY[2]}" < "$MINIMUM_VERSION" ]]; then
  echo "CMake version not supported. Minimum version required 3.12.0"
  if [[ "$HOST_OS" = "windows" ]]; then
    choco upgrade cmake
  elif [[ "$HOST_OS" = "mac" ]]; then
    brew upgrade cmake -v
  elif [[ "$HOST_OS" = "linux" ]]; then
    sudo apt-get update
  elif [[ "$HOST_OS" = "other" ]];  then
    echo "Host OS not supported."
  fi
  
else
  echo "CMake version supported."
fi

#sleep 3s
