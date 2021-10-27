#
#  SetupBoost.cmake
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

add_library (SMCE_Boost INTERFACE)
if ("${SMCE_BOOST_LINKING}" STREQUAL "STATIC")
  set (Boost_USE_STATIC_LIBS True)
else ()
  set (Boost_USE_STATIC_LIBS False)
endif ()

if (MSVC)
  find_package (Boost 1.74 COMPONENTS atomic filesystem date_time)
else ()
  find_package (Boost 1.74 COMPONENTS atomic filesystem)
endif ()
if (Boost_FOUND)
  target_link_libraries (SMCE_Boost INTERFACE Boost::headers Boost::atomic Boost::filesystem)
  target_include_directories (SMCE_Boost SYSTEM INTERFACE ${Boost_INCLUDE_DIRS})
  target_link_directories (SMCE_Boost INTERFACE ${Boost_LIBRARY_DIRS})
else ()
# set (Boost_DEBUG True)
  set (BOOST_ENABLE_CMAKE True)

  if (EXISTS "${PROJECT_SOURCE_DIR}/ext_deps/boost")
    set (boost_SOURCE_DIR ext_deps/boost)
  else ()
    include (FetchContent)
    message ("Downloading Boost")
    FetchContent_Declare (Boost
        GIT_REPOSITORY "https://github.com/boostorg/boost"
        GIT_TAG "boost-1.76.0"
        GIT_SHALLOW On
        GIT_PROGRESS On
    )
    FetchContent_GetProperties (Boost)
    if (NOT boost_POPULATED)
      FetchContent_Populate (Boost)
    endif ()
  endif ()

  set (PREV_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")
  if ("${SMCE_BOOST_LINKING}" STREQUAL "SHARED")
    set (BUILD_SHARED_LIBS True)
  else ()
    set (BUILD_SHARED_LIBS False)
  endif ()

  set (PREV_CMAKE_POSITION_INDEPENDENT_CODE "${CMAKE_POSITION_INDEPENDENT_CODE}")
  set (CMAKE_POSITION_INDEPENDENT_CODE On)

  set (BUILD_TESTING Off)
  add_subdirectory ("${boost_SOURCE_DIR}" "${boost_BINARY_DIR}" EXCLUDE_FROM_ALL)

  set (CMAKE_POSITION_INDEPENDENT_CODE "${PREV_CMAKE_POSITION_INDEPENDENT_CODE}")
  set (BUILD_SHARED_LIBS "${PREV_BUILD_SHARED_LIBS}")

  target_link_libraries (SMCE_Boost INTERFACE
      Boost::atomic # Dependency of Interprocess
      Boost::filesystem # Dependency of Process
      Boost::type_index # Dependency of Process
      Boost::intrusive # Dependency of Interprocess
      Boost::container # Dependency of Interprocess
      Boost::date_time # Dependency of Interprocess
  )
  target_include_directories (SMCE_Boost SYSTEM INTERFACE
      "${boost_SOURCE_DIR}/libs/process/include"
      "${boost_SOURCE_DIR}/libs/interprocess/include"
      "${boost_SOURCE_DIR}/libs/asio/include" # Dependency of Process
      "${boost_SOURCE_DIR}/libs/algorithm/include" # Dependency of Interprocess
      "${boost_SOURCE_DIR}/libs/range/include" # Dependency of Interprocess
      "${boost_SOURCE_DIR}/libs/numeric/conversion/include" # Dependency of Interprocess
  )
endif ()

add_library (Boost_ipc INTERFACE)
if (WIN32)
  target_link_libraries (Boost_ipc INTERFACE ole32 oleaut32 psapi advapi32)
elseif (NOT APPLE)
  target_link_libraries (Boost_ipc INTERFACE rt)
endif ()
add_library (Boost::ipc ALIAS Boost_ipc)

target_link_libraries (SMCE_Boost INTERFACE Boost_ipc)

### Version/Platform-specific workarounds

if (Boost_VERSION VERSION_LESS 1.78)
  include (CheckCXXSymbolExists)
  check_cxx_symbol_exists ("_LIBCPP_VERSION" "ciso646" HAS_LIBCPP) # Would use <version> but not all platforms have it
  if (HAS_LIBCPP)
    file (MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/LibcppVersion")
    file (WRITE "${PROJECT_BINARY_DIR}/LibcppVersion/LibcppVersion.cxx" [[
      #include <cstdio>
      #define XSTR(s) #s
      #define STR(s) XSTR(s)
      int main() {
        std::puts(STR(_LIBCPP_VERSION));
      }
    ]])
    try_run (LIBCPP_VERCHK_RUN_RESULT LIBCPP_VERCHK_COMPILED "${PROJECT_BINARY_DIR}/LibcppVersion" "${PROJECT_BINARY_DIR}/LibcppVersion/LibcppVersion.cxx"
        RUN_OUTPUT_VARIABLE LIBCPP_VER
    )
    if (NOT LIBCPP_VERCHK_COMPILED)
      message (FATAL_ERROR "Failed to determine libc++ version: try_run failed to compile")
    elseif (LIBCPP_VERCHK_RUN_RESULT STREQUAL "FAILED_TO_RUN")
      message (FATAL_ERROR "Failed to determine libc++ version: try_run failed to run")
    endif ()

    if (LIBCPP_VER GREATER_EQUAL 13000)
      target_compile_definitions (SMCE_Boost INTERFACE BOOST_ASIO_HAS_STD_INVOKE_RESULT=1)
      message (WARNING "Your version of Boost is too old to know about libc++ >= 13; consider updating Boost to 1.78 or above")
    endif ()
  endif ()
endif ()
