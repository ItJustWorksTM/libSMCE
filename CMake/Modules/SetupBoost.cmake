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

add_library (SMCE_Boost INTERFACE)

# set (Boost_DEBUG True)
set (BOOST_ENABLE_CMAKE True)

if (DEFINED BOOST_SOURCE_ROOT)
  set (boost_SOURCE_DIR "${BOOST_SOURCE_ROOT}")
else ()
  include (FetchContent)
  # Can't reasonably default to Git until https://gitlab.kitware.com/cmake/cmake/-/issues/16144 is fixed
  if (NOT BOOST_SOURCE_USE_GIT)
    if (NOT WIN32)
      FetchContent_Declare (Boost
          URL "https://boostorg.jfrog.io/artifactory/main/release/1.78.0/source/boost_1_78_0.tar.gz"
          URL_HASH SHA256=94ced8b72956591c4775ae2207a9763d3600b30d9d7446562c552f0a14a63be7
      )
    else ()
      FetchContent_Declare (Boost
          URL "https://boostorg.jfrog.io/artifactory/main/release/1.78.0/source/boost_1_78_0.7z"
          URL_HASH SHA256=090cefea470bca990fa3f3ed793d865389426915b37a2a3258524a7258f0790c
      )
    endif ()
  else ()
    FetchContent_Declare (Boost
        GIT_REPOSITORY "https://github.com/boostorg/boost"
        GIT_TAG "boost-1.78.0"
        GIT_SHALLOW On
        GIT_PROGRESS On
    )
  endif ()
  FetchContent_GetProperties (Boost)
  if (NOT boost_POPULATED)
    message ("Downloading Boost")
    FetchContent_Populate (Boost)

    if (NOT EXISTS "${boost_SOURCE_DIR}/CMakeLists.txt")
      file (DOWNLOAD "https://github.com/boostorg/boost/raw/boost-1.78.0/CMakeLists.txt" "${boost_SOURCE_DIR}/CMakeLists.txt")
    endif ()
    message ("Download complete")
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
  set (BOOST_EXCLUDE_LIBRARIES compute context)
  add_library (boost_context INTERFACE) # We don't need it and it breaks macOS universal builds
  add_library (Boost::context ALIAS boost_context)
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
    Boost::process
    Boost::interprocess
)
target_include_directories (SMCE_Boost SYSTEM INTERFACE
    "${boost_SOURCE_DIR}/libs/process/include"
    "${boost_SOURCE_DIR}/libs/interprocess/include"
)
if (EXISTS "${boost_SOURCE_DIR}/boost")
  message (STATUS "Detected \"${boost_SOURCE_DIR}/boost\" - using as Boost headers incdir")
  target_include_directories (SMCE_Boost SYSTEM INTERFACE "${boost_SOURCE_DIR}")
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
