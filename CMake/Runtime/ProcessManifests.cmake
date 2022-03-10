#
#  ProcessManifests.cmake
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

function (process_manifests)
  file (GLOB manifests "${CMAKE_SOURCE_DIR}/manifests/*.cmake")

  set (plugins)
  foreach (manifest ${manifests})
    message (DEBUG "Scanning manifest ${manifest}")
    function (scope_warden_high)
      function (scope_warden_low)
        include (${manifest})

        if (NOT DEFINED PLUGIN_NAME OR PLUGIN_NAME STREQUAL "")
          message (FATAL_ERROR "No plugin name in manifest at ${manifest}")
        endif ()

        string (MAKE_C_IDENTIFIER "${PLUGIN_NAME}" name_c)
        if (NOT PLUGIN_NAME STREQUAL name_c)
          message (FATAL_ERROR "Plugin name must be a valid C/C++ identifier\n\t(Note: \"${PLUGIN_NAME}\" is not)\n\t(manifest is ${manifest})")
        endif ()

        if (NOT DEFINED PLUGIN_VERSION OR PLUGIN_VERSION STREQUAL "")
          message (FATAL_ERROR "No plugin version for plugin ${PLUGIN_NAME}")
        elseif (NOT PLUGIN_VERSION MATCHES "[0-9]+|[0-9]+\\.[0-9]+|[0-9]+\\.[0-9]+\\.[0-9]+")
          message (FATAL_ERROR "Invalid version for plugin ${PLUGIN_NAME}")
        endif ()

        macro (pass_through suff)
          if (DEFINED PLUGIN_${suff})
            set (PLUGIN_${suff} "${PLUGIN_${suff}}" PARENT_SCOPE)
          endif ()
        endmacro ()

        set (PLUGIN_NAME "${PLUGIN_NAME}" PARENT_SCOPE)
        pass_through (VERSION)
        pass_through (DEPENDS)
        pass_through (URI)
        pass_through (PATCH_URI)
        pass_through (DEFAULTS)
        pass_through (INCDIRS)
        pass_through (SOURCES)
        pass_through (LINKDIRS)
        pass_through (LINKBINS)

      endfunction ()
      scope_warden_low ()

      macro (pass_through suff)
        if (DEFINED PLUGIN_${suff})
          set (PLUGIN_${PLUGIN_NAME}_${suff} "${PLUGIN_${suff}}" PARENT_SCOPE)
        endif ()
      endmacro ()

      set (PLUGIN_NAME "${PLUGIN_NAME}" PARENT_SCOPE)
      pass_through (VERSION)
      pass_through (DEPENDS)
      pass_through (URI)
      pass_through (PATCH_URI)
      pass_through (DEFAULTS)
      pass_through (INCDIRS)
      pass_through (SOURCES)
      pass_through (LINKDIRS)
      pass_through (LINKBINS)

    endfunction ()
    scope_warden_high ()
    list (APPEND plugins "${PLUGIN_NAME}")
    set (PLUGIN_${PLUGIN_NAME}_MANIFEST "${manifest}")

    unset (PLUGIN_NAME)
  endforeach ()

  # FIXME scan plugin dependencies, and sort the plugins list accordingly

  function (process_plugin plugin)
    macro (pass_through suff)
      if (DEFINED PLUGIN_${PLUGIN_NAME}_${suff})
        set (PLUGIN_${suff} "${PLUGIN_${PLUGIN_NAME}_${suff}}")
      endif ()
    endmacro ()

    set (PLUGIN_NAME "${plugin}")
    pass_through (VERSION)
    pass_through (DEPENDS)
    pass_through (URI)
    pass_through (PATCH_URI)
    pass_through (DEFAULTS)
    pass_through (INCDIRS)
    pass_through (SOURCES)
    pass_through (LINKDIRS)
    pass_through (LINKBINS)

    message (DEBUG "Processing plugin ${PLUGIN_NAME}")

    macro (get_root mode)
      if ("${mode}" STREQUAL "SOURCE")
        set (uPATCH "")
        set (upatch "")
        set (patchu "")
        set (sPatch "")
        set (spatch "")
      elseif ("${mode}" STREQUAL "PATCH")
        set (uPATCH "_PATCH")
        set (upatch "_patch")
        set (patchu "patch_")
        set (sPatch " Patch")
        set (spatch " patch")
      else ()
        message (FATAL_ERROR "Unknown mode \"${mode}\" for get_root")
      endif ()

      if (PLUGIN${uPATCH}_URI MATCHES "^file://(.+)$")
        if (NOT IS_ABSOLUTE "${CMAKE_MATCH_1}")
          message (FATAL_ERROR "[Plugin ${PLUGIN_NAME}] ${sPatch} URI is not absolute (${CMAKE_MATCH_1})")
        elseif (NOT EXISTS "${CMAKE_MATCH_1}" OR NOT IS_DIRECTORY "${CMAKE_MATCH_1}")
          message (FATAL_ERROR "[Plugin ${PLUGIN_NAME}] ${sPatch} URI does not point to a valid directory (\"${CMAKE_MATCH_1}\")")
        endif ()
        set (${patchu}root "${CMAKE_MATCH_1}")

      elseif (PLUGIN${uPATCH}_URI MATCHES "^https?://(.+)$")
        message (STATUS "[Plugin ${PLUGIN_NAME}] Downloading${spatch}...")
        file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins${upatch}_dls")
        set (ark${upatch}_path "${CMAKE_BINARY_DIR}/plugins${upatch}_dls/${PLUGIN_NAME}.ark")
        if (EXISTS "${ark${upatch}_path}")
          message (STATUS "[Plugin ${PLUGIN_NAME}] Download was cached")
        else ()
          file (DOWNLOAD "${CMAKE_MATCH_1}" "${ark${upatch}_path}" STATUS ark${upatch}_dlstatus)
          list (GET ark${upatch}_dlstatus 0 ark${upatch}_dlstatus_code)
          if (ark${upatch}_dlstatus_code)
            list (GET ark${upatch}_dlstatus 1 ark${upatch}_dlstatus_message)
            file (REMOVE "${ark${upatch}_path}")
            message (FATAL_ERROR "[Plugin ${PLUGIN_NAME}] Download failed: (${ark${upatch}_dlstatus_code}) ${ark${upatch}_dlstatus_message}")
          endif ()
          message (STATUS "[Plugin ${PLUGIN_NAME}] Download complete")
        endif ()

        message (STATUS "[Plugin ${PLUGIN_NAME}] Inflating${spatch}...")
        set (${patchu}root "${CMAKE_BINARY_DIR}/plugins${upatch}_roots/${PLUGIN_NAME}")
        file (MAKE_DIRECTORY "${${patchu}root}")
        if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.18)
          file (ARCHIVE_EXTRACT INPUT "${ark${upatch}_path}" DESTINATION "${${patchu}root}")
        else ()
          execute_process (COMMAND "${CMAKE_COMMAND}" -E tar xf "${CMAKE_MATCH_1}" "${ark${upatch}_path}"
              WORKING_DIRECTORY "${${patchu}root}"
              RESULT_VARIABLE ark${upatch}_extract_result
          )
          if (ark${upatch}_extract_result)
            message (FATAL_ERROR "[Plugin ${PLUGIN_NAME}] Failed to inflate${spatch} archive")
          endif ()
        endif ()

      elseif (NOT DEFINED PLUGIN${uPATCH}_URI)
        message (FATAL_ERROR "[Plugin ${PLUGIN_NAME}] No${spatch} URI")
      else ()
        message (FATAL_ERROR "[Plugin ${PLUGIN_NAME}] Malformed${spatch} URI (${PLUGIN${uPATCH}_URI})")
      endif ()
    endmacro ()
    get_root (SOURCE)
    if (PLUGIN_PATCH_URI)
      get_root (PATCH)
    endif ()

    # Find the intended root dir if source was downloaded
    if (NOT "${PLUGIN_URI}" STREQUAL "file://${root}")
      while (1)
        file (GLOB root_files "${root}/*")
        list (LENGTH root_files root_files_count)
        if (root_files_count EQUAL 0)
          message (FATAL_ERROR "[Plugin ${PLUGIN_NAME}] Unable to deduce root directory in inflated archive")
        elseif (root_files_count EQUAL 1 AND IS_DIRECTORY "${root_files}")
          set (root "${root_files}")
        else ()
          break ()
        endif ()
      endwhile ()
    endif ()

    message (DEBUG "[Plugin ${PLUGIN_NAME}] Root is ${root}")

    if (PLUGIN_PATCH_URI)
      # Merge-in the patch tree
      message (STATUS "[Plugin ${PLUGIN_NAME}] Patching...")
      file (GLOB_RECURSE patch_relfiles LIST_DIRECTORIES false RELATIVE "${patch_root}" "${patch_root}/*")
      foreach (patch_relfile ${patch_relfiles})
        if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.20")
          cmake_path (GET patch_relfile PARENT_PATH patch_relfile_dir)
        else ()
          get_filename_component (patch_relfile_dir "${patch_relfile}" DIRECTORY)
        endif ()
        file (MAKE_DIRECTORY "${root}/${patch_relfile_dir}")
        file (COPY "${patch_root}/${patch_relfile}" DESTINATION "${root}/${patch_relfile_dir}")
      endforeach ()
      message (STATUS "[Plugin ${PLUGIN_NAME}] Patching complete")
    endif ()

    # Expand defaults
    set (plugin_defaulted_incdirs)
    set (plugin_defaulted_sources)
    set (plugin_defaulted_linkdirs)
    set (plugin_defaulted_linklibs)
    if (NOT DEFINED PLUGIN_DEFAULTS OR PLUGIN_DEFAULTS STREQUAL "")
    elseif (PLUGIN_DEFAULTS STREQUAL "CMAKE")
      function (scope_warden_high)
        function (scope_warden_low)
          add_subdirectory ("${root}")
        endfunction ()
        scope_warden_low ()
      endfunction ()
      scope_warden_high ()
      return ()
    elseif (PLUGIN_DEFAULTS STREQUAL "ARDUINO")
      # FIXME read library.properties
      set (plugin_defaulted_incdirs "src/")
      set (plugin_defaulted_sources "src/**.cpp" "src/**.hpp" "src/**.c" "src/**.h" "src/**.cxx" "src/**.hxx" "src/**.c++" "src/**.h++")
    elseif (PLUGIN_DEFAULTS STREQUAL "C")
      set (plugin_defaulted_incdirs "include/")
      set (plugin_defaulted_sources "src/**.cpp" "include/**.hpp" "src/**.c" "include/**.h" "src/**.cxx" "include/**.hxx" "src/**.c++" "include/**.h++")
      set (plugin_defaulted_linkdirs "lib/")
    elseif (PLUGIN_DEFAULTS STREQUAL "SINGLE")
      set (plugin_defaulted_incdirs "./")
      set (plugin_defaulted_sources "./*.cpp" "./**.hpp" "./*.c" "./*.h" "./*.cxx" "./*.hxx" "./*.c++" "./*.h++")
      set (plugin_defaulted_linkdirs "./")
    else ()
      message (FATAL_ERROR "[Plugin ${PLUGIN_NAME}] Invalid defaults value")
    endif ()

    # Collect paths
    set (incdirs ${plugin_defaulted_incdirs} ${PLUGIN_INCDIRS})
    set (sources_mixed ${plugin_defaulted_sources} ${PLUGIN_SOURCES})
    set (linkdirs ${plugin_defaulted_linkdirs} ${PLUGIN_LINKDIRS})
    set (linklibs_mixed ${plugin_defaulted_linklibs} ${PLUGIN_LINKLIBS})

    # Convert to absolute
    function (absolute_paths LIST BASE)
      set (out)
      foreach (path ${${LIST}})
        if (NOT IS_ABSOLUTE "${path}")
          string (PREPEND path "${BASE}/")
        endif ()
        if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.20)
          cmake_path (NORMAL_PATH path)
        endif ()
        list (APPEND out "${path}")
      endforeach ()
      set ("${LIST}" ${out} PARENT_SCOPE)
    endfunction ()
    absolute_paths (incdirs "${root}")
    absolute_paths (sources_mixed "${root}")
    absolute_paths (linkdirs "${root}")
    absolute_paths (linklibs_mixed "${root}")

    function (gather_files BASENAME)
      set (plain ${${BASENAME}_mixed})
      list (FILTER plain EXCLUDE REGEX "[^\\]\\*")

      set (patterns ${${BASENAME}_mixed})
      list (FILTER patterns INCLUDE REGEX "[^\\]\\*")
      list (FILTER patterns EXCLUDE REGEX "[^\\]\\*\\*")
      set (patterns_recurse ${${BASENAME}_mixed})
      list (FILTER patterns_recurse INCLUDE REGEX "[^\\]\\*\\*")

      list (TRANSFORM patterns_recurse REPLACE "^\\*\\*" "*")
      list (TRANSFORM patterns_recurse REPLACE "([^\\])\\*\\*" "\\1*")

      set (dev_configure_depends)
      if (PLUGIN_DEV)
        set (dev_configure_depends CONFIGURE_DEPENDS)
      endif ()

      file (GLOB paths_g LIST_DIRECTORIES false ${dev_configure_depends} ${patterns})
      file (GLOB_RECURSE paths_gr LIST_DIRECTORIES false ${dev_configure_depends} ${patterns_recurse})

      set (${BASENAME} ${plain} ${paths_g} ${paths_gr} PARENT_SCOPE)
    endfunction ()
    gather_files (sources)
    gather_files (linklibs)

    if (sources)
      message (DEBUG "[Plugin ${PLUGIN_NAME}] Declaring OBJECT target")
      add_library (smce_plugin_${PLUGIN_NAME} OBJECT ${sources})
      set (visibility PUBLIC)
    else ()
      message (DEBUG "[Plugin ${PLUGIN_NAME}] Declaring INTERFACE target")
      add_library (smce_plugin_${PLUGIN_NAME} INTERFACE)
      set (visibility INTERFACE)
    endif ()

    target_include_directories (smce_plugin_${PLUGIN_NAME} SYSTEM PRIVATE "${SMCE_DIR}/RtResources/Ardrivo/include" "${PROJECT_BINARY_DIR}/SMCE_Devices/include")
    target_include_directories (smce_plugin_${PLUGIN_NAME} ${visibility} ${incdirs})
    target_link_directories (smce_plugin_${PLUGIN_NAME} ${visibility} ${linkdirs})
    target_link_libraries (smce_plugin_${PLUGIN_NAME} ${visibility} ${linklibs})

    list (TRANSFORM PLUGIN_DEPENDS PREPEND "smce_plugin_" OUTPUT_VARIABLE link_targets)
    target_link_libraries (smce_plugin_${PLUGIN_NAME} ${visibility} Ardrivo ${link_targets})

    target_link_libraries (Sketch PUBLIC smce_plugin_${PLUGIN_NAME})
  endfunction ()

  foreach (plugin ${plugins})
    process_plugin ("${plugin}")
  endforeach ()
endfunction ()
process_manifests ()
