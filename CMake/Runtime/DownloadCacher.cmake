#
#  DownloadCacher.cmake
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

# function for cleaning the cache
function (clear_download_cache)
  # delete all contents in the cached folder
  file (REMOVE_RECURSE "${SMCE_DIR}/cached_downloads/")
  message (STATUS "Cache cleared")
endfunction ()

#[================================================================================================================[.rst:
cached_download
--------------------

Function to allow for caching of various downloads.

Usage:
.. code-block:: cmake

  cached_download (URL <url> DEST <dest-var> [RESULT_VARIABLE <res-var>] [FORCE_UPDATE])

Where ``<url>`` is the URL to the file to be downloaded, ``<dest-var>`` is the name of the variable to store the absolute
real path to the download location passed to the parent scope by the function, ``<res-var>``
is the name of the variable to store the result of all processes passed to the parent scope by
the function, ``[FORCE_UPDATE]`` will define whether an already cached download should be re-downloaded and cached.


Note:
No additional arguments except for the ones defined are allowed in the function call.
Uses SHA256 to create a uniquely hashed download location for each download.
Download file is locked until the file has been downloaded and cached, this is done in order to avoid
possible race conditions.
#]================================================================================================================]
function (cached_download)
  # initialize the cache download directory
  file (MAKE_DIRECTORY "${SMCE_DIR}/cached_downloads")

  # parse args
  set (options FORCE_UPDATE)
  set (oneValueArgs URL RESULT_VARIABLE DEST)
  set (multiValueArgs)
  cmake_parse_arguments ("ARG" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (${ARG_UNPARSED_ARGUMENTS})
    if (DEFINED ARG_RESULT_VARIABLE)
      set ("Function was called with too many arguments" PARENT_SCOPE)
    else ()
      message (FATAL_ERROR "Function was called with too many arguments")
    endif ()
  endif ()

  # use SHA256 hash to URL to create unique identifier, lock download location mutex-style
  string (SHA256 hashed_dest "${ARG_URL}")
  set (download_path "${SMCE_DIR}/cached_downloads/${hashed_dest}")
  file (TOUCH "${download_path}.lock")
  file (LOCK "${download_path}.lock")

  # check if plugin has already been downloaded and cached before  
  if (EXISTS "${download_path}")
    set (index 1)
  else ()
    set (index -1)
  endif ()

  # if download has been cached previously and is requested a forced re-download, clean previous download and re-cache 
  if (${index} GREATER -1 AND ${ARG_FORCE_UPDATE})
    file (REMOVE "${download_path}")
    set (${index} -1)
  endif ()

  # if download has not been cached, download. Otherwise pass.
  if (${index} LESS 0)
    message (DEBUG "Downloading")

    file (DOWNLOAD "${ARG_URL}" "${download_path}" STATUS dlstatus)
    list (GET dlstatus 0 dlstatus_code)
    if (dlstatus_code)
      list (GET dlstatus 1 dlstatus_message)
      file (REMOVE "${download_path}")
      file (REMOVE "${download_path}.lock")
      if (DEFINED ARG_RESULT_VARIABLE)
        set ("${ARG_RESULT_VARIABLE}" "${dlstatus}" PARENT_SCOPE)
      else ()
        message (FATAL_ERROR "Download failed: (${dlstatus_code}) ${dlstatus_message}")
      endif ()
    endif ()
    message (DEBUG "Download complete")
    message (DEBUG "Cached!")
  else ()
    message (DEBUG "Has already been cached!")
  endif ()

  # Unlock file and output absolute real path to download location
  file (LOCK "${download_path}.lock" RELEASE)
  file (REMOVE "${download_path}.lock")

  if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.19")
    file (REAL_PATH "${download_path}" download_path)
  else ()
    get_filename_component (download_path "${download_path}" REALPATH)
  endif ()

  set ("${ARG_DEST}" "${download_path}" PARENT_SCOPE)
endfunction ()