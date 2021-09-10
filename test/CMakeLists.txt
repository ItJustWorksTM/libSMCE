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


include (BindGen)
smce_bindgen_host (TestUDD "TestUDD" 1.0 "s8 f0" "u32 f1" "au16 f2" "mutex f3" "mutex f4")
target_link_libraries (TestUDD PUBLIC "${SMCE_LINK_TARGET}")

add_executable (SMCE_Tests
    defs.hpp
    Board.cpp
    BoardView.cpp
    BoardDevice.cpp
    LibManagement.cpp
    Polyfills.cpp
    Toolchain.cpp
)
configure_coverage (SMCE_Tests)
target_link_libraries (SMCE_Tests PUBLIC TestUDD "${SMCE_LINK_TARGET}" Catch2::Catch2WithMain SMCE_Boost)
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
file (GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/inflate_ark_if_different.cmake" CONTENT [[
  # HSD generated
  file (TIMESTAMP "${CMAKE_ARGV3}" curr_filetime)
  if (EXISTS "${CMAKE_BINARY_DIR}/ark-filetime.txt")
    file (READ "${CMAKE_BINARY_DIR}/ark-filetime.txt" prev_filetime)
    if (curr_filetime STREQUAL prev_filetime)
      return ()
    endif ()
  endif ()

  message (STATUS "Inflating resources for testsuite execution)")
  execute_process (COMMAND "${CMAKE_COMMAND}" -E tar xf "${CMAKE_ARGV3}"
    WORKING_DIRECTORY "${CMAKE_ARGV4}"
  )
  file (WRITE "${CMAKE_BINARY_DIR}/ark-filetime.txt" "${curr_filetime}")
]])
add_custom_target (SMCE_Tests_ResDir
    COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/inflate_ark_if_different.cmake" "${SMCE_RESOURCES_ARK}" "\"${SMCE_TEST_DIR}/smce_root\""
    DEPENDS ArdRtRes "${SMCE_RESOURCES_ARK}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Checking resources change"
)
file (COPY sketches DESTINATION "${SMCE_TEST_DIR}")
file (COPY patches DESTINATION "${SMCE_TEST_DIR}")
file (COPY plugins DESTINATION "${SMCE_TEST_DIR}")
file (COPY manifests DESTINATION "${SMCE_TEST_DIR}")
target_compile_definitions (SMCE_Tests PUBLIC "SMCE_TEST_DIR=\"${SMCE_TEST_DIR}\"")
add_dependencies (SMCE_Tests SMCE_Tests_ResDir)

unset (SMCE_LINK_TARGET)
