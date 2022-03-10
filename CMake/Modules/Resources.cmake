#
#  Resources.cmake
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

function (copy_at_build)
  set (options)
  set (oneValueArgs DESTINATION COMMENT APPEND_TRACKER)
  set (multiValueArgs DIRECTORIES FILES)
  cmake_parse_arguments ("ARG" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if (NOT DEFINED ARG_DESTINATION)
    message (FATAL_ERROR "No DESTINATION specified")
  endif ()
  file (MAKE_DIRECTORY "${ARG_DESTINATION}")

  if (NOT DEFINED ARG_FILES AND NOT DEFINED ARG_DIRECTORIES)
    message (FATAL_ERROR "No FILES or DIRECTORIES specified")
  endif ()
  if (DEFINED ARG_COMMENT)
    set (ARG_COMMENT COMMENT ${ARG_COMMENT})
  endif ()

  set (outputs)
  if (DEFINED ARG_FILES)
    foreach (file IN LISTS ARG_FILES)
      if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.20")
        cmake_path (GET file FILENAME filename)
      else ()
        get_filename_component (filename "${file}" NAME)
      endif ()
      list (APPEND outputs "${ARG_DESTINATION}/${filename}")
    endforeach ()
    set (files_copy_command COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${ARG_FILES} "${ARG_DESTINATION}")
  endif ()

  set (depends)
  set (dirs_copy_commands)
  if (DEFINED ARG_DIRECTORIES)
    foreach (dir IN LISTS ARG_DIRECTORIES)
      if (NOT IS_ABSOLUTE "${dir}")
        string (PREPEND dir "${CMAKE_CURRENT_SOURCE_DIR}/")
      endif ()
      if (NOT IS_DIRECTORY "${dir}")
        message (FATAL_ERROR "DIRECTORIES contains \"${dir}\" which is not a directory")
      endif ()
      if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.20")
        cmake_path (GET dir PARENT_PATH parent)
      else ()
        get_filename_component (parent "${dir}" DIRECTORY)
      endif ()
      file (GLOB_RECURSE files LIST_DIRECTORIES false RELATIVE "${parent}" CONFIGURE_DEPENDS "${dir}/*")
      if (NOT files STREQUAL "")
        foreach (file IN LISTS files)
          if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.20")
            cmake_path (GET file FILENAME filename)
            cmake_path (GET file PARENT_PATH subdirs)
          else ()
            get_filename_component (filename "${file}" NAME)
            get_filename_component (subdirs "${file}" DIRECTORY)
          endif ()
          file (MAKE_DIRECTORY "${ARG_DESTINATION}/${subdirs}")
          list (APPEND outputs "${ARG_DESTINATION}/${file}")
          list (APPEND dirs_copy_commands COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${parent}/${file}" "${ARG_DESTINATION}/${subdirs}")
          list (APPEND depends "${parent}/${file}")
        endforeach ()
      endif ()
    endforeach ()
  endif ()

  add_custom_command (OUTPUT ${outputs}
      ${files_copy_command}
      ${dirs_copy_commands}
      DEPENDS ${depends}
      ${ARG_COMMENT}
  )
  if (DEFINED ARG_APPEND_TRACKER)
    set ("${ARG_APPEND_TRACKER}" ${${ARG_APPEND_TRACKER}} ${outputs} PARENT_SCOPE)
  endif ()
endfunction()

set (SMCE_RUNTIME_CMAKE_MODULES)
macro (copy_runtime_module MODULE_NAME)
  copy_at_build (FILES "${PROJECT_SOURCE_DIR}/CMake/Runtime/${MODULE_NAME}.cmake" DESTINATION "${SMCE_RTRES_DIR}/SMCE/share/CMake/Modules/"
      COMMENT "Setting-up runtime module ${MODULE_NAME}"
      APPEND_TRACKER SMCE_RUNTIME_CMAKE_MODULES
  )
endmacro ()

macro (setup_smce_resources)
  set (SMCE_RTRES_DIR "${PROJECT_BINARY_DIR}/RtResources")
  file (REMOVE_RECURSE "${SMCE_RTRES_DIR}")
  file (MAKE_DIRECTORY "${SMCE_RTRES_DIR}")

  file (MAKE_DIRECTORY "${SMCE_RTRES_DIR}/SMCE/share")

  copy_at_build (FILES "${PROJECT_SOURCE_DIR}/CMake/Runtime/CMakeLists.txt" DESTINATION "${SMCE_RTRES_DIR}/SMCE/share/CMake/Runtime/")
  copy_at_build (FILES "${PROJECT_SOURCE_DIR}/CMake/Scripts/ConfigureSketch.cmake" DESTINATION "${SMCE_RTRES_DIR}/SMCE/share/CMake/Scripts/")
  copy_at_build (FILES "${PROJECT_SOURCE_DIR}/CMake/Modules/BindGen.cmake" DESTINATION "${SMCE_RTRES_DIR}/SMCE/share/CMake/Modules/")
  copy_runtime_module (ArduinoPreludeVersion)
  copy_runtime_module (InstallArduinoPrelude)
  copy_runtime_module (JuniperTranspile)
  copy_runtime_module (LegacyPreprocessing)
  copy_runtime_module (Preprocessing)
  copy_runtime_module (ProcessManifests)
  copy_runtime_module (ProbeCompilerIncdirs)
  copy_runtime_module (UseHighestCxxStandard)

  file (MAKE_DIRECTORY "${SMCE_RTRES_DIR}/Ardrivo")
  file (MAKE_DIRECTORY "${SMCE_RTRES_DIR}/Ardrivo/bin")
  file (MAKE_DIRECTORY "${SMCE_RTRES_DIR}/Ardrivo/share")

  copy_at_build (FILES "${PROJECT_SOURCE_DIR}/share/Ardrivo/sketch_main.cpp" DESTINATION "${SMCE_RTRES_DIR}/Ardrivo/share/")

  set(SMCE_RESOURCES_ARK "${PROJECT_BINARY_DIR}/SMCE_Resources.zip")
  file (GENERATE
      OUTPUT "${SMCE_RTRES_DIR}/Ardrivo/share/ArdrivoOutputNames.cmake"
      CONTENT [[ #HSD Generated
        set (ARDRIVO_FILE_NAME "$<TARGET_FILE_NAME:Ardrivo>")
        set (ARDRIVO_LINKER_FILE_NAME "$<TARGET_LINKER_FILE_NAME:Ardrivo>")
      ]]
  )

  file (MAKE_DIRECTORY "${SMCE_RTRES_DIR}/Ardrivo/include")
  file (COPY "${PROJECT_SOURCE_DIR}/include/Ardrivo" DESTINATION "${SMCE_RTRES_DIR}/Ardrivo/include")

  set (SMCE_RUNTIME_BG_HEADERS)
  copy_at_build (DESTINATION "${SMCE_RTRES_DIR}/Ardrivo/include/SMCE_rt"
      FILES "${PROJECT_SOURCE_DIR}/include/SMCE_rt/SMCE_proxies.hpp"
      COMMENT "Setting-up bindgen headers"
      APPEND_TRACKER SMCE_RUNTIME_BG_HEADERS
  )
  copy_at_build (DESTINATION "${SMCE_RTRES_DIR}/Ardrivo/include/SMCE_rt/internal"
      FILES
        "${PROJECT_SOURCE_DIR}/include/SMCE_rt/internal/SMCE_api.hpp"
        "${PROJECT_SOURCE_DIR}/include/SMCE_rt/internal/BoardDeviceAllocationBases.hpp"
        "${PROJECT_SOURCE_DIR}/include/SMCE_rt/internal/sketch_rt.hpp"
      COMMENT "Setting-up internal bindgen headers"
      APPEND_TRACKER SMCE_RUNTIME_BG_HEADERS
  )

  add_custom_command (OUTPUT "${SMCE_RESOURCES_ARK}"
      COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:Ardrivo>" "${SMCE_RTRES_DIR}/Ardrivo/bin"
      COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_LINKER_FILE:Ardrivo>" "${SMCE_RTRES_DIR}/Ardrivo/bin"
      COMMAND "${CMAKE_COMMAND}" -E tar cf "${SMCE_RESOURCES_ARK}" --format=zip -- "${SMCE_RTRES_DIR}"
      DEPENDS
        Ardrivo
        "${SMCE_RTRES_DIR}/Ardrivo/share/sketch_main.cpp"
        "${SMCE_RTRES_DIR}/SMCE/share/CMake/Runtime/CMakeLists.txt"
        "${SMCE_RTRES_DIR}/SMCE/share/CMake/Scripts/ConfigureSketch.cmake"
        "${SMCE_RTRES_DIR}/SMCE/share/CMake/Modules/BindGen.cmake"
        ${SMCE_RUNTIME_CMAKE_MODULES}
        ${SMCE_RUNTIME_BG_HEADERS}
      COMMENT "Generating resources archive"
  )

  add_custom_target (ArdRtRes DEPENDS "${SMCE_RESOURCES_ARK}")
endmacro ()
