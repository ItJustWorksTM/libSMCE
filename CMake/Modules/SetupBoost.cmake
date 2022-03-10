#
#  SetupBoost.cmake
#  Copyright 2021-2022 ItJustWorksTM
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

message (STATUS "SMCE_BOOST_LINKING: ${SMCE_BOOST_LINKING}")

add_library (SMCE_Boost INTERFACE)

set (SMCE_BOOST_MINVER 1.74)
set (SMCE_BOOST_COMPONENTS atomic filesystem)
if (MSVC)
  list (APPEND SMCE_BOOST_COMPONENTS date_time)
endif ()

if ("${SMCE_BOOST_LINKING}" STREQUAL "STATIC")
  set (Boost_USE_STATIC_LIBS True)
else ()
  set (Boost_USE_STATIC_LIBS False)
endif ()
if (MSVC)
  if ("${SMCE_CXXRT_LINKING}" STREQUAL "STATIC")
    set (Boost_USE_STATIC_RUNTIME True)
  else ()
    set (Boost_USE_STATIC_RUNTIME False)
  endif ()
endif ()

if ("${SMCE_BOOST_LINKING}" STREQUAL "AUTO")
  find_package (Boost "${SMCE_BOOST_MINVER}" COMPONENTS ${SMCE_BOOST_COMPONENTS})
  if (Boost_FOUND)
    set (SMCE_BOOST_LINKING "SHARED")
  else ()
    set (SMCE_BOOST_LINKING "SOURCE")
  endif ()
  message (STATUS "Resolved SMCE_BOOST_LINKING: AUTO -> ${SMCE_BOOST_LINKING}")
endif ()

if ("${SMCE_BOOST_LINKING}" IN_LIST SMCE_LIBRARY_TYPES)
  find_package (Boost "${SMCE_BOOST_MINVER}" REQUIRED COMPONENTS ${SMCE_BOOST_COMPONENTS})
  target_link_libraries (SMCE_Boost INTERFACE Boost::headers Boost::atomic Boost::filesystem)
  target_include_directories (SMCE_Boost SYSTEM INTERFACE ${Boost_INCLUDE_DIRS})
  target_link_directories (SMCE_Boost INTERFACE ${Boost_LIBRARY_DIRS})
elseif ("${SMCE_BOOST_LINKING}" STREQUAL "SOURCE")
# set (Boost_DEBUG True)
  set (BOOST_ENABLE_CMAKE True)

  if (DEFINED BOOST_SOURCE_ROOT)
    set (boost_SOURCE_DIR "${BOOST_SOURCE_ROOT}")
  else ()
    include (FetchContent)
    # Can't reasonably default to Git until https://gitlab.kitware.com/cmake/cmake/-/issues/16144 is fixed
    if (NOT BOOST_SOURCE_USE_GIT)
      # Skip 1.77 because of the Boost.CMake issue mentioned below
      if (NOT WIN32)
        FetchContent_Declare (Boost
            URL "https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.gz"
            URL_HASH SHA256=7bd7ddceec1a1dfdcbdb3e609b60d01739c38390a5f956385a12f3122049f0ca
        )
      else ()
        FetchContent_Declare (Boost
            URL "https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.7z"
            URL_HASH SHA256=88782714f8701b6965f3fce087a66a1262601dd5ccd5b2e5305021beb53042a1
        )
      endif ()
    else ()
      FetchContent_Declare (Boost
          GIT_REPOSITORY "https://github.com/boostorg/boost"
          GIT_TAG "boost-1.77.0"
          GIT_SHALLOW On
          GIT_PROGRESS On
      )
    endif ()
    FetchContent_GetProperties (Boost)
    if (NOT boost_POPULATED)
      message ("Downloading Boost")
      FetchContent_Populate (Boost)

      if (NOT EXISTS "${boost_SOURCE_DIR}/CMakeLists.txt")
        file (DOWNLOAD "https://github.com/boostorg/boost/raw/boost-1.76.0/CMakeLists.txt" "${boost_SOURCE_DIR}/CMakeLists.txt")
      else ()
        # The following two commands can be dropped once we use GIT_TAG boost-1.78.0 (which will be released end of Dec 2021)
        find_package (Git REQUIRED)
        execute_process (COMMAND "${GIT_EXECUTABLE}" checkout develop
            WORKING_DIRECTORY "${boost_SOURCE_DIR}/tools/cmake"
        )
      endif ()
    endif ()
  endif ()

  set (PREV_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")
  set (BUILD_SHARED_LIBS False)

  set (PREV_CMAKE_POSITION_INDEPENDENT_CODE "${CMAKE_POSITION_INDEPENDENT_CODE}")
  set (CMAKE_POSITION_INDEPENDENT_CODE On)

  if (DEFINED BUILD_TESTING)
    set (PREV_BUILD_TESTING "${BUILD_TESTING}")
  else ()
    set (PREV_BUILD_TESTING "=UNDEFINED=")
  endif ()
  set (BUILD_TESTING Off)

  if (NOT DEFINED BOOST_EXCLUDE_LIBRARIES)
    set (BOOST_EXCLUDE_LIBRARIES compute)
  endif ()
  string (TOLOWER "${SMCE_CXXRT_LINKING}" BOOST_RUNTIME_LINK)

  if (EXISTS "${boost_SOURCE_DIR}/boost")
    include_directories (SYSTEM "${boost_SOURCE_DIR}")
  endif ()

  add_subdirectory ("${boost_SOURCE_DIR}" "${boost_BINARY_DIR}" EXCLUDE_FROM_ALL)

  if (BUILD_TESTING STREQUAL "=UNDEFINED=")
    unset (BUILD_TESTING)
  else ()
    set (BUILD_TESTING "${PREV_BUILD_TESTING}")
  endif ()
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
  if (EXISTS "${boost_SOURCE_DIR}/boost")
    message (STATUS "Detected \"${boost_SOURCE_DIR}/boost\" - using as Boost headers incdir")
    target_include_directories (SMCE_Boost SYSTEM INTERFACE "${boost_SOURCE_DIR}")
  endif ()
endif ()

add_library (Boost_ipc INTERFACE)
target_link_libraries (Boost_ipc INTERFACE
    $<$<NOT:$<OR:$<BOOL:${APPLE}>,$<BOOL:${WIN32}>>>:rt>
    $<$<BOOL:${WIN32}>:ole32 oleaut32 psapi advapi32>
    $<$<BOOL:${MINGW}>:ws2_32 ntdll>
)
if (MINGW)
  target_compile_definitions (Boost_ipc INTERFACE __kernel_entry=)
  target_compile_options (Boost_ipc INTERFACE -Wno-unknown-pragmas)
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
