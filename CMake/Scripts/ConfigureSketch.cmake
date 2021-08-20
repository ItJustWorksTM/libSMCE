#
#  ConfigureSketch.cmake
#  Copyright 2021 ItJustWorksTM
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

## Expected variables
# SMCE_DIR - Path to the SMCE dir
# SKETCH_FQBN - Fully qualified board name to use
# SKETCH_PATH - Path to the Arduino sketch
# PREPROC_REMOTE_LIBS - whitespace-separated of remote libs to pull for preprocessing
# COMPLINK_REMOTE_LIBS - remote libs needed at compile/link-time
# COMPLINK_PATCH_LIBS - remote libs to patch for compile/link-time

## Optional env
# SMCE_LEGACY_PREPROCESSING - use arduino-cli to preprocess instead of arduino-prelude

cmake_policy (SET CMP0011 NEW)

# CMake info
message ("CMAKE_VERSION: ${CMAKE_VERSION}")

# System info
unset (CMAKE_BINARY_DIR)
include (CMakeDetermineSystem)
message ("CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM}")
message ("CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

set (MODULES_DIR "${SMCE_DIR}/RtResources/CMake/Modules")
list (APPEND CMAKE_MODULE_PATH "${MODULES_DIR}")

# Find arduino-prelude or obtain it
set (ARDPRE_ROOT "${SMCE_DIR}/RtResources/arduino-prelude")
find_program (ARDPRE_EXECUTABLE arduino-prelude PATHS "${ARDPRE_ROOT}/bin")
if (NOT ARDPRE_EXECUTABLE)
  set (ARDPRE_KNOWN_LATEST "1.0.1")
  set (ARDPRE_DL_STEM "https://github.com/ItJustWorksTM/arduino-prelude/releases")
  set (ARDPRE_FILENAME_SYS "${CMAKE_HOST_SYSTEM_NAME}")
  if (WIN32)
    if ("${CMAKE_HOST_SYSTEM_PROCESSOR}" MATCHES "64")
      set (ARDPRE_FILENAME_SYS win64)
    else()
      set (ARDPRE_FILENAME_SYS win32)
    endif()
  endif ()
  set (ARDPRE_DL_FILENAME "arduino-prelude-${ARDPRE_KNOWN_LATEST}-${ARDPRE_FILENAME_SYS}")
  if (WIN32)
    if ("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "AMD64" OR "${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "IA64")
      set (ARDPRE_DL_URI "${ARDPRE_DL_STEM}/download/v${ARDPRE_KNOWN_LATEST}/${ARDPRE_DL_FILENAME}.zip")
    endif ()
  elseif (APPLE)
    # We'll assume we never run on iOS, watchOS, or tvOS, nor on a PPC OSX
    set (ARDPRE_DL_URI "${ARDPRE_DL_STEM}/download/v${ARDPRE_KNOWN_LATEST}/${ARDPRE_DL_FILENAME}.tar.gz")
  elseif ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux" AND "${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
    set (ARDPRE_DL_URI "${ARDPRE_DL_STEM}/download/v${ARDPRE_KNOWN_LATEST}/${ARDPRE_DL_FILENAME}.tar.gz")
  endif ()

  if (DEFINED ARDPRE_DL_URI)
    file (MAKE_DIRECTORY "${ARDPRE_ROOT}/")
    set (ARDPRE_ARK_EXT ".tgz")
    if (WIN32)
      set (ARDPRE_ARK_EXT ".zip")
    endif ()
    set (ARDPRE_ARK "${ARDPRE_ROOT}/arduino-prelude${ARDPRE_ARK_EXT}")
    message (STATUS "Downloading arduino-prelude")
    file (DOWNLOAD "${ARDPRE_DL_URI}" "${ARDPRE_ARK}")
    if (NOT EXISTS "${ARDPRE_ARK}")
      message (FATAL_ERROR "Failed to download arduino-prelude; do you have an active internet connection?")
    endif ()
    execute_process (COMMAND "${CMAKE_COMMAND}" -E tar xf "${ARDPRE_ARK}"
        WORKING_DIRECTORY "${ARDPRE_ROOT}"
    )
    file (REMOVE "${ARDPRE_ARK}")
    file (RENAME "${ARDPRE_ROOT}/${ARDPRE_DL_FILENAME}" "${ARDPRE_ROOT}/bin")
    set (ARDPRE_EXECUTABLE "${ARDPRE_ROOT}/bin/arduino-prelude${CMAKE_EXECUTABLE_SUFFIX}")
  else ()
    message (FATAL_ERROR "arduino-prelude could not be found on your system or automatically installed")
  endif ()
endif ()
execute_process (COMMAND "${ARDPRE_EXECUTABLE}" --version)

# Download latest CMAW if not preinstalled
set (CMAW_AUTO_PATH "${SMCE_DIR}")
set (CMAW_URL "https://github.com/AeroStun/CMAW/raw/master/CMAW.cmake")
set (CMAW_RUNLOC "${MODULES_DIR}/CMAW.cmake")
include ("${CMAW_RUNLOC}" OPTIONAL RESULT_VARIABLE CMAW_FOUND)
if (CMAW_FOUND)
  if (CMAW_VERSION VERSION_LESS "0.1.3")
    set (CMAW_OLD True)
  else ()
    set (CMAW_OLD False)
  endif ()
endif ()
if (NOT CMAW_FOUND OR CMAW_OLD)
  file (MAKE_DIRECTORY "${MODULES_DIR}")
  file (DOWNLOAD "${CMAW_URL}" "${CMAW_RUNLOC}")
  include ("${CMAW_RUNLOC}")
endif ()
message (STATUS "Using CMAW version ${CMAW_VERSION}")

cmaw_arduinocli_version (ARDCLI_VERSION)
message (STATUS "Using ArduinoCLI version ${ARDCLI_VERSION}")

if ($ENV{SMCE_LEGACY_PREPROCESSING})
  string (REPLACE ":" ";" SKETCH_FQBN_PARTS ${SKETCH_FQBN})
  list (GET SKETCH_FQBN_PARTS 0 SKETCH_FQBN_PACKAGER)
  list (GET SKETCH_FQBN_PARTS 1 SKETCH_FQBN_ARCH)
  cmaw_install_cores ("${SKETCH_FQBN_PACKAGER}:${SKETCH_FQBN_ARCH}")
endif ()

if (NOT DEFINED ENV{SMCE_INDEX_UPDATE} OR \"$ENV{SMCE_INDEX_UPDATE}\")
  cmaw_update_library_index ()
endif ()
foreach (REMOTE_LIB ${PREPROC_REMOTE_LIBS} ${COMPLINK_REMOTE_LIBS})
  cmaw_install_libraries ("${REMOTE_LIB}")
endforeach ()

cmaw_dump_config (ARDCLI_CONFIG)
string (REGEX REPLACE ";" "\\\\;" ARDCLI_CONFIG "${ARDCLI_CONFIG}")
string (REGEX REPLACE "\n" ";" ARDCLI_CONFIG "${ARDCLI_CONFIG}")
set (ARDCLI_CONFIG_USERDIR "NOTFOUND")
foreach (ARDCLI_CONFIG_LINE ${ARDCLI_CONFIG})
  if (ARDCLI_CONFIG_LINE MATCHES "^  user: (.*)$")
    string (STRIP "${CMAKE_MATCH_1}" ARDCLI_CONFIG_USERDIR)
    break ()
  endif ()
endforeach ()
if (NOT ARDCLI_CONFIG_USERDIR)
  message (FATAL_ERROR "Could not find the userdir in the ArduinoCLI config dump")
elseif (NOT EXISTS "${ARDCLI_CONFIG_USERDIR}")
  message (WARNING "ArduinoCLI userdir could not be found on disk (\"${ARDCLI_CONFIG_USERDIR}\")")
endif ()

string (RANDOM LENGTH 13 COMP_DIRNAME)
set (COMP_DIR "${SMCE_DIR}/tmp/${COMP_DIRNAME}")
file (MAKE_DIRECTORY "${COMP_DIR}")
message (STATUS "SMCE: Compilation directory is \"${COMP_DIR}\"")

if (IS_DIRECTORY "${SKETCH_PATH}")
  set (SKETCH_DIR "${SKETCH_PATH}")
else ()
  if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20")
    cmake_path (GET SKETCH_PATH PARENT_PATH SKETCH_DIR)
  else ()
    get_filename_component (SKETCH_DIR "${SKETCH_PATH}" DIRECTORY)
  endif ()
endif ()

file (MAKE_DIRECTORY "${COMP_DIR}/libs")
foreach (COMPLINK_PATCH_LIB ${COMPLINK_PATCH_LIBS})
  string (REGEX MATCH "^([^|]+)\\|([^@]*)(@?[0-9.]*)$" MATCH "${COMPLINK_PATCH_LIB}")
  if (NOT MATCH)
    message (FATAL_ERROR "Invalid COMPLINK_PATCH_LIB (\"${COMPLINK_PATCH_LIB}\")")
  endif ()
  set (COMPLINK_PATCH_LIB_PATH "${CMAKE_MATCH_1}")
  string (REPLACE " " "_" COMPLINK_PATCH_LIB_TARGET "${CMAKE_MATCH_2}")

  message (STATUS "Processing library \"${COMPLINK_PATCH_LIB_TARGET}\" (patched by \"${COMPLINK_PATCH_LIB_PATH}\")")
  # Copy original library
  file (COPY "${ARDCLI_CONFIG_USERDIR}/libraries/${COMPLINK_PATCH_LIB_TARGET}" DESTINATION "${COMP_DIR}/libs")

  # Merge-in the patch tree
  file (GLOB_RECURSE COMPLINK_PATCH_LIB_FILEPATHS
      LIST_DIRECTORIES false RELATIVE "${COMPLINK_PATCH_LIB_PATH}"
      "${COMPLINK_PATCH_LIB_PATH}/*")
  foreach (COMPLINK_PATCH_LIB_RELFILEPATH ${COMPLINK_PATCH_LIB_FILEPATHS})
    if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20")
      cmake_path (GET COMPLINK_PATCH_LIB_RELFILEPATH PARENT_PATH COMPLINK_PATCH_LIB_RELFILEDIR)
    else ()
      get_filename_component (COMPLINK_PATCH_LIB_RELFILEDIR "${COMPLINK_PATCH_LIB_RELFILEPATH}" DIRECTORY)
    endif ()
    file (MAKE_DIRECTORY "${COMP_DIR}/libs/${COMPLINK_PATCH_LIB_TARGET}/${COMPLINK_PATCH_LIB_RELFILEDIR}")
    file (COPY "${COMPLINK_PATCH_LIB_PATH}/${COMPLINK_PATCH_LIB_RELFILEPATH}" DESTINATION "${COMP_DIR}/libs/${COMPLINK_PATCH_LIB_TARGET}/${COMPLINK_PATCH_LIB_RELFILEDIR}")
  endforeach ()
endforeach ()

if ("$ENV{SMCE_LEGACY_PREPROCESSING}")
  cmaw_preprocess (PREPROCD_SKETCH "${SKETCH_FQBN}" "${SKETCH_PATH}")
  if ("${PREPROCD_SKETCH}" STREQUAL "")
    message (FATAL_ERROR "Preprocessing failed")
  endif ()
  set (COMP_SRC "${COMP_DIR}/sketch.cpp")
  file (WRITE "${COMP_SRC}" "${PREPROCD_SKETCH}")
endif ()

if (DEFINED ENV{SMCE_TOOLCHAIN})
  set (TOOLCHAIN "-DCMAKE_TOOLCHAIN_FILE=\"$ENV{SMCE_TOOLCHAIN}\"")
endif ()

file (COPY "${SMCE_DIR}/RtResources/SMCE/share/Runtime/CMakeLists.txt" DESTINATION "${COMP_DIR}")
file (MAKE_DIRECTORY "${COMP_DIR}/build")
execute_process (COMMAND "${CMAKE_COMMAND}" "-DSMCE_DIR=${SMCE_DIR}" "-DARDPRE_EXECUTABLE=${ARDPRE_EXECUTABLE}" "-DSKETCH_DIR=${SKETCH_DIR}" ${TOOLCHAIN} -S "${COMP_DIR}" -B "${COMP_DIR}/build")

message (STATUS "SMCE: Sketch binary will be at \"${COMP_DIR}/build/Sketch\"")
