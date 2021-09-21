#!/bin/bash
INSTALLED_VERSION=$(cmake --version)
MINIMUM_VERSION=3.12.0

INSTALLED_VERSION_ARRAY=($INSTALLED_VERSION)

CMAKE_LOCATION=$(where cmake)
CHOCO_LOCATION=$(where choco)

#BREW_LOCATION
#APT_GET_LOCATION

HOST_OS_VERSION=$(uname -s)
HOST_OS=''

case $HOST_OS_VERSION in
   Darwin) HOST_OS='mac';;
   Linux) HOST_OS='linux';;
   CYGWIN*|MINGW32*|MSYS*|MINGW*) HOST_OS='windows';;
   *) HOST_OS='other';;
esac

if [[ -f $CMAKE_LOCATION ]]; then
  echo "CMake found on host."

  if [[ "$HOST_OS" = "windows" ]]; then
    if [[ -f $CHOCO_LOCATION ]]; then
      echo "Chocolatey found on host."
    else 
      echo "Chocolatey not found on host."
      echo "Install Chocolatey manually."
      # Install Choco
    fi

  elif [[ "$HOST_OS" = "other" ]];  then
    echo "Host OS not supported."
  fi

else 
  echo "CMake not found on host."
  if [[ "$HOST_OS" = "windows" ]]; then
    choco install cmake
  elif [[ "$HOST_OS" = "mac" ]]; then
    brew install cmake
  elif [[ "$HOST_OS" = "linux" ]]; then
    sudo apt-get -y install cmake
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
    sudo apt-get -y update
  elif [[ "$HOST_OS" = "other" ]];  then
    echo "Host OS not supported."
  fi
  
else
  echo "CMake version supported."
fi

sleep 3s
