#
#  iSMCE.cmake
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

include_guard ()

include (GNUInstallDirs)

add_library (iSMCE INTERFACE)
target_compile_features (iSMCE INTERFACE cxx_std_20)
target_include_directories (iSMCE
    INTERFACE
      "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_link_libraries (iSMCE
    INTERFACE
      Threads::Threads
      $<BUILD_INTERFACE:SMCE_Boost>
)
