
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was Config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################
#
#  Config.cmake.in
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

include ("${CMAKE_CURRENT_LIST_DIR}/SMCETargets.cmake")

set_and_check (SMCE_RESOURCES_ARK "${PACKAGE_PREFIX_DIR}/share/SMCE/SMCE_Resources.zip")

if (NOT SMCE_PROPERTY_RESOURCES_ARCHIVE_DEFINED)
    define_property (TARGET PROPERTY RESOURCES_ARCHIVE
      BRIEF_DOCS "The full path to SMCE_Resources.zip"
      FULL_DOCS "The full path to SMCE_Resources.zip, which contains the runtime files for SMCE"
    )
  set (SMCE_PROPERTY_RESOURCES_ARCHIVE_DEFINED True)
endif ()
set_property (TARGET SMCE::SMCE PROPERTY RESOURCES_ARCHIVE "${SMCE_RESOURCES_ARK}")

set (SMCE_BUILD_SHARED       "ON")
set (SMCE_BUILD_STATIC       "ON")
set (SMCE_CXXRT_LINKING      "SHARED")
set (SMCE_BOOST_LINKING      "SHARED")
set (SMCE_ARDRIVO_MQTT       "ON")
if (SMCE_ARDRIVO_MQTT)
  set (SMCE_MOSQUITTO_LINKING  "SOURCE")
  set (SMCE_OPENSSL_LINKING    "SHARED")
endif ()
set (SMCE_ARDRIVO_OV767X "ON")

check_required_components (SMCE)

include ("${CMAKE_CURRENT_LIST_DIR}/BindGen.cmake")
