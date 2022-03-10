#
#  JuniperTranspile.cmake
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

## Expected variables
# SMCE_DIR - Path to the SMCE dir
# SKETCH_DIR - Path to the sketch

## Expected targets:
# Sketch - sketch executable target

file (GLOB JUN_SOURCE_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${SKETCH_DIR}/*.jun")

if (JUN_SOURCE_FILES STREQUAL "")
  return ()
endif ()

find_program (JUNIPER_EXECUTABLE "juniper"
    PATHS "${SMCE_DIR}/RtResources/Juniper/bin"
    DOC "Path to the Juniper compiler"
)
if (NOT JUNIPER_EXECUTABLE)
  message ("Seeing Juniper sources ${JUN_SOURCE_FILES}")
  message (SEND_ERROR "You appear to have Juniper sources in your sketch, but the Juniper compiler cannot be found; these files will be ignored")
  return ()
endif ()

set (JUN_SRC_ARGS "")
foreach (src IN LISTS JUN_SOURCE_FILES)
  string (APPEND JUN_SRC_ARGS " \"${src}\"")
endforeach ()

file (WRITE "${PROJECT_BINARY_DIR}/JuniperSourcesTranspile.cmake" "execute_process (COMMAND \"${JUNIPER_EXECUTABLE}\" -s ${JUN_SRC_ARGS} -o \"${PROJECT_SOURCE_DIR}/juniper_sketch.cpp\" RESULT_VARIABLE JUNIPER_EXITCODE)\n")
file (APPEND "${PROJECT_BINARY_DIR}/JuniperSourcesTranspile.cmake" [[
    if (JUNIPER_EXITCODE)
      message (FATAL_ERROR "Juniper transpilation failed: ${JUNIPER_EXITCODE}")
    endif ()
]])

add_custom_command (OUTPUT "${PROJECT_SOURCE_DIR}/juniper_sketch.cpp"
    COMMAND "${CMAKE_COMMAND}" -P "${PROJECT_BINARY_DIR}/JuniperSourcesTranspile.cmake"
    DEPENDS ${JUN_SOURCE_FILES}
    COMMENT "Transpiling Juniper sources of sketch \"${SKETCH_DIR}\""
)

target_sources (Sketch PRIVATE "${PROJECT_SOURCE_DIR}/juniper_sketch.cpp")
