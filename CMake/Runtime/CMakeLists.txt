#
#  Runtime/CMakeLists.txt
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
# ARDPRE_EXECUTABLE - Path of the arduino-prelude executable
# SKETCH_DIR - Path to the sketch

cmake_minimum_required (VERSION 3.12)

project (Sketch CXX)

foreach (CXX_COMPILE_FEATURE ${CMAKE_CXX_COMPILE_FEATURES})
  if (CXX_COMPILE_FEATURE MATCHES "cxx_std_([0-8][0-9]+)")
    list (APPEND CXX_SUPPORTED_VERSIONS "${CMAKE_MATCH_1}")
  endif ()
endforeach ()
list (SORT CXX_SUPPORTED_VERSIONS)
list (REVERSE CXX_SUPPORTED_VERSIONS)
list (GET CXX_SUPPORTED_VERSIONS 1 CXX_HIGHEST_SUPPORTED_VERSION)
set (CMAKE_CXX_STANDARD "${CXX_HIGHEST_SUPPORTED_VERSION}")
message ("Using CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

include ("${SMCE_DIR}/RtResources/Ardrivo/share/ArdrivoOutputNames.cmake")
add_library (Ardrivo SHARED IMPORTED)
target_include_directories (Ardrivo SYSTEM INTERFACE "${SMCE_DIR}/RtResources/Ardrivo/include/Ardrivo")
target_compile_definitions (Ardrivo INTERFACE "__SMCE__=1")
target_compile_features (Ardrivo INTERFACE cxx_std_11)
set_property (TARGET Ardrivo PROPERTY IMPORTED_LOCATION "${SMCE_DIR}/RtResources/Ardrivo/bin/${ARDRIVO_FILE_NAME}")
set_property (TARGET Ardrivo PROPERTY IMPORTED_IMPLIB "${SMCE_DIR}/RtResources/Ardrivo/bin/${ARDRIVO_LINKER_FILE_NAME}")

if (MSVC)
  target_compile_options (Ardrivo INTERFACE "/permissive-" "/W4" "/w34265" "/w44289" "/w44296" "/w14545" "/w14546" "/w14547" "/w14548" "/w14549" "/w14555" "/w44574" "/w44582" "/w44583" "/w34619" "/w44749" "/w44777" "/w44837" "/w44841" "/w44842" "/w14928" "/w14946" "/w44986" "/w44987" "/w45022" "/w45023" "/w45029" "/w45038" "/wd4250")
else ()
  target_compile_options (Ardrivo INTERFACE "-Wpedantic" "-Wall" "-Wextra" "-Wnon-virtual-dtor" "-Wold-style-cast" "-Wcast-align" "-Woverloaded-virtual" "-Wnull-dereference")
endif ()

if (WIN32)
  file (CREATE_LINK "${SMCE_DIR}/RtResources/Ardrivo/bin/${ARDRIVO_FILE_NAME}"
      "${PROJECT_BINARY_DIR}/${ARDRIVO_FILE_NAME}" COPY_ON_ERROR SYMBOLIC)
endif ()

add_executable (Sketch)
target_sources (Sketch PRIVATE "${SMCE_DIR}/RtResources/Ardrivo/share/sketch_main.cpp")
target_include_directories (Sketch PRIVATE "${SKETCH_DIR}")
target_link_libraries (Sketch Ardrivo)
target_compile_definitions (Sketch PUBLIC SMCE__COMPILING_USERCODE=1)
add_custom_command (TARGET Sketch POST_BUILD COMMAND "${CMAKE_COMMAND}" -E rename "$<TARGET_FILE:Sketch>" "${PROJECT_BINARY_DIR}/Sketch")
file (GLOB CXX_SOURCES LIST_DIRECTORIES false "${SKETCH_DIR}/*.cpp" "${SKETCH_DIR}/*.cxx" "${SKETCH_DIR}/*.cc" "${SKETCH_DIR}/*.c++")
target_sources (Sketch PRIVATE ${CXX_SOURCES})

file (GLOB LIBS LIST_DIRECTORIES true "${PROJECT_SOURCE_DIR}/libs/*")
foreach (LIB ${LIBS})
  if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20")
    cmake_path (GET LIB FILENAME LIB_NAME)
  else ()
    get_filename_component (LIB_NAME "${LIB}" NAME)
  endif ()
  file (GLOB_RECURSE LIB_SOURCES LIST_DIRECTORIES false "${LIB}/src/*")
  if (NOT LIB_SOURCES)
    message (FATAL_ERROR "No sources found for library \"${LIB_NAME}\" (at \"${LIB}\")")
  endif ()

  add_library ("${LIB_NAME}" OBJECT ${LIB_SOURCES})
  target_include_directories ("${LIB_NAME}" PUBLIC "${LIB}/src")
  target_link_libraries ("${LIB_NAME}" PUBLIC Ardrivo)
  target_link_libraries (Sketch "${LIB_NAME}")
endforeach ()

if (NOT DEFINED ENV{SMCE_LEGACY_PREPROCESSING} OR NOT "$ENV{SMCE_LEGACY_PREPROCESSING}")
  message (STATUS "Setting up preprocessing harness")

  set (INCLUDE_DIRS)
  set (COMP_DEFS)

  get_target_property (SKETCH_INCLUDE_DIRS Sketch INCLUDE_DIRECTORIES)
  list (APPEND INCLUDE_DIRS ${SKETCH_INCLUDE_DIRS})

  get_target_property (SKETCH_COMP_DEFS Sketch COMPILE_DEFINITIONS)
  list (APPEND COMP_DEFS ${SKETCH_COMP_DEFS})

  file (WRITE "${PROJECT_BINARY_DIR}/empty" "")
  if (MSVC AND NOT CMAKE_CXX_SIMULATE_ID)
    # FIXME - Handle VS < 2015 as well as Windows < 10

    list (GET CMAKE_CXX_COMPILER 0 CL_EXECUTABLE)
    if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20")
      cmake_path (GET CL_EXECUTABLE PARENT_PATH CL_BINDIR)
      cmake_path (CONVERT "${CL_BINDIR}/../../../include" TO_NATIVE_PATH_LIST MSSTL_INCDIR NORMALIZE)
    else ()
      get_filename_component (CL_BINDIR "${CL_EXECUTABLE}" DIRECTORY)
      file (TO_NATIVE_PATH "${CL_BINDIR}/../../../include" MSSTL_INCDIR)
    endif ()
    list (APPEND INCLUDE_DIRS "${MSSTL_INCDIR}")

    # Detect Windows SDK and use its include dirs
    if (DEFINED ENV{CMAKE_WINDOWS_KITS_10_DIR})
      set (WINSDK_ROOT "$ENV{CMAKE_WINDOWS_KITS_10_DIR}")
    else ()
      get_filename_component (WINSDK_ROOT "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" ABSOLUTE)
    endif ()
    set (WINSDK_INCDIR "${WINSDK_ROOT}/Include/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/")
    list (APPEND INCLUDE_DIRS "${WINSDK_INCDIR}/ucrt" "${WINSDK_INCDIR}/um" "${WINSDK_INCDIR}/shared")
  else ()
    if (MSVC)
      message (WARNING "Clang-cl unsupported")
      file (TO_NATIVE_PATH "${PROJECT_BINARY_DIR}/empty" EMPTY_FILE_MSPATH)
      execute_process (COMMAND "${CMAKE_CXX_COMPILER}" "/E" "/Tp" "${EMPTY_FILE_MSPATH}" "/clang:-Wp,-v"
          RESULT_VARIABLE COMPILER_SEARCH_DIRS_COMMAND_RESULT
          ERROR_VARIABLE COMPILER_SEARCH_DIRS_RAW
          OUTPUT_QUIET
      )
    else ()
      execute_process (COMMAND "${CMAKE_CXX_COMPILER}" -E -x c++ "${PROJECT_BINARY_DIR}/empty" -Xpreprocessor -v
          RESULT_VARIABLE COMPILER_SEARCH_DIRS_COMMAND_RESULT
          ERROR_VARIABLE COMPILER_SEARCH_DIRS_RAW
          OUTPUT_QUIET
      )
    endif ()
    if (COMPILER_SEARCH_DIRS_COMMAND_RESULT)
      message (FATAL_ERROR "Unable to probe the compiler for its header search locations (${COMPILER_SEARCH_DIRS_COMMAND_RESULT})")
    endif ()

    string (REGEX MATCH "#include <...> search starts here:\n(.+)End of search list." IGNORE "${COMPILER_SEARCH_DIRS_RAW}")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
      string (REPLACE " \(framework directory\)" "" CMAKE_MATCH_1 "${CMAKE_MATCH_1}")
    endif ()
    string (REGEX REPLACE ";" "\\\\;" COMPILER_SEARCH_DIRS "${CMAKE_MATCH_1}")
    string (REGEX REPLACE "\n" ";" COMPILER_SEARCH_DIRS "${COMPILER_SEARCH_DIRS}")
    foreach (DIR ${COMPILER_SEARCH_DIRS})
      string (STRIP "${DIR}" DIR)
      list (APPEND INCLUDE_DIRS "${DIR}")
    endforeach ()
  endif ()

  get_target_property (SKETCH_LINK_LIBS Sketch LINK_LIBRARIES)
  foreach (LIB ${SKETCH_LINK_LIBS})
    if (NOT TARGET "${LIB}")
      continue ()
    endif ()
    get_target_property (LIB_INCLUDE_DIRS "${LIB}" INCLUDE_DIRECTORIES)
    if (LIB_INCLUDE_DIRS)
      list (APPEND INCLUDE_DIRS ${LIB_INCLUDE_DIRS})
    endif ()
    get_target_property (LIB_COMP_DEFS "${LIB}" COMPILE_DEFINITIONS)
    if (LIB_COMP_DEFS)
      list (APPEND COMP_DEFS ${LIB_COMP_DEFS})
    endif ()
    get_target_property (LIB_INCLUDE_DIRS "${LIB}" INTERFACE_INCLUDE_DIRECTORIES)
    if (LIB_INCLUDE_DIRS)
      list (APPEND INCLUDE_DIRS ${LIB_INCLUDE_DIRS})
    endif ()
    get_target_property (LIB_COMP_DEFS "${LIB}" INTERFACE_COMPILE_DEFINITIONS)
    if (LIB_COMP_DEFS)
      list (APPEND COMP_DEFS ${LIB_COMP_DEFS})
    endif ()
  endforeach ()

  set (INCDIR_FLAGS "")
  foreach (INCDIR ${INCLUDE_DIRS})
    string (APPEND INCDIR_FLAGS " \"-I${INCDIR}\"")
  endforeach ()
  string (REPLACE "\\" "\\\\" INCDIR_FLAGS "${INCDIR_FLAGS}")

  set (COMPDEF_FLAGS "")
  foreach (COMPDEF ${COMP_DEFS})
    string (APPEND COMPDEF_FLAGS " \"-D${COMPDEF}\"")
  endforeach ()
  string (REPLACE "\\" "\\\\" COMPDEF_FLAGS "${COMPDEF_FLAGS}")

  set (EXTRA_FLAGS "-std=c++${CMAKE_CXX_STANDARD}")
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set (EXTRA_FLAGS "-std=gnu++${CMAKE_CXX_STANDARD}")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    string (APPEND EXTRA_FLAGS " -I${CMAKE_OSX_SYSROOT}/usr/include")
  elseif (MSVC)
    string (APPEND EXTRA_FLAGS " -fms-extensions")
  endif ()

  string (APPEND EXTRA_FLAGS " -nostdinc -nostdinc++ -include Arduino.h")

  file (WRITE "${PROJECT_BINARY_DIR}/ArduinoSourceBuild.cmake" "execute_process (COMMAND \"${ARDPRE_EXECUTABLE}\" \"${SKETCH_DIR}\" ${COMPDEF_FLAGS} ${INCDIR_FLAGS} ${EXTRA_FLAGS} ${CMAKE_CXX_FLAGS} RESULT_VARIABLE ARDPRE_EXITCODE OUTPUT_FILE \"${PROJECT_SOURCE_DIR}/sketch.cpp\")\n")
  file (APPEND "${PROJECT_BINARY_DIR}/ArduinoSourceBuild.cmake" [[
    if (ARDPRE_EXITCODE)
      message (FATAL_ERROR "Preprocessing failed: ${ARDPRE_EXITCODE}")
    endif ()
  ]])

  set (SKETCH_SOURCE_FILES)
  file (GLOB SKETCH_SOURCE_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${SKETCH_DIR}/*.ino" "${SKETCH_DIR}/*.pde")

  add_custom_command (OUTPUT "${PROJECT_SOURCE_DIR}/sketch.cpp"
      COMMAND "${CMAKE_COMMAND}" -E env ARDUINO_PRELUDE_DUMP_COMPOSITE=1 "${CMAKE_COMMAND}" -P "${PROJECT_BINARY_DIR}/ArduinoSourceBuild.cmake"
      DEPENDS ${SKETCH_SOURCE_FILES}
      COMMENT "Preprocessing sketch \"${SKETCH_DIR}\""
  )
endif ()
target_sources (Sketch PRIVATE "${PROJECT_SOURCE_DIR}/sketch.cpp")
