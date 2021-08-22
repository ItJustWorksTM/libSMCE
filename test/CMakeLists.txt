#
#  test/CMakeLists.txt
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

find_package (Catch2 QUIET)
if (NOT Catch2_FOUND)
  include (FetchContent)
  FetchContent_Declare (
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        devel
  )
  FetchContent_GetProperties (Catch2)
  if (NOT catch2_POPULATED)
    FetchContent_Populate (Catch2)
  endif ()
  set (CATCH_BUILD_STATIC_LIBRARY On CACHE INTERNAL "" FORCE)
  add_subdirectory ("${catch2_SOURCE_DIR}" "${catch2_BINARY_DIR}" EXCLUDE_FROM_ALL)
  list (APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")
endif ()

include (CTest)
include (Catch)

set (SMCE_LINK_TARGET SMCE)
if (NOT SMCE_BUILD_SHARED)
  string (APPEND SMCE_LINK_TARGET "_static")
endif ()

add_executable (SMCE_Tests
    defs.hpp
    Board.cpp
    BoardView.cpp
    LibManagement.cpp
    Polyfills.cpp
    Toolchain.cpp
)
configure_coverage (SMCE_Tests)
target_link_libraries (SMCE_Tests PUBLIC "${SMCE_LINK_TARGET}" Catch2::Catch2WithMain)
target_compile_definitions (SMCE_Tests PUBLIC SMCE_ARDRIVO_MQTT=$<BOOL:${SMCE_ARDRIVO_MQTT}>)
if (MSVC)
  target_compile_definitions (SMCE_Tests PUBLIC MSVC_DEBUG=$<CONFIG:Debug>)
endif ()
catch_discover_tests (SMCE_Tests)

if (WIN32 AND SMCE_BUILD_SHARED)
  add_custom_command (TARGET SMCE_Tests PRE_LINK
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SMCE>" "$<TARGET_FILE_DIR:SMCE_Tests>"
  )
endif ()

set (SMCE_TEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/test_dir")
file (MAKE_DIRECTORY "${SMCE_TEST_DIR}/smce_root")
add_custom_target (SMCE_Tests_ResDir
    COMMAND "${CMAKE_COMMAND}" -E tar xf "${SMCE_RESOURCES_ARK}"
    DEPENDS "${SMCE_RESOURCES_ARK}"
    WORKING_DIRECTORY "${SMCE_TEST_DIR}/smce_root"
    COMMENT "Inflating resources for testsuite execution"
)
file (COPY sketches DESTINATION "${SMCE_TEST_DIR}")
file (COPY patches DESTINATION "${SMCE_TEST_DIR}")
target_compile_definitions (SMCE_Tests PUBLIC "SMCE_TEST_DIR=\"${SMCE_TEST_DIR}\"")
add_dependencies (SMCE_Tests SMCE_Tests_ResDir)

unset (SMCE_LINK_TARGET)
